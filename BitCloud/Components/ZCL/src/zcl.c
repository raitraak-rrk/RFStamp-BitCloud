/************************************************************************//**
  \file zcl.c

  \brief
    The ZCL implementation

    The file inmplements the ZCL

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    01.12.08 I. Fedina & A. Potashov - Created.
******************************************************************************/
#if ZCL_SUPPORT == 1

/******************************************************************************
                   Includes section
******************************************************************************/
#include <zclDbg.h>
#include <zcl.h>
#include <zclInt.h>
#include <zclParser.h>
#include <zclAttributes.h>
#include <clusters.h>
#include <zclTaskManager.h>
#include <zclMemoryManager.h>
#include <sysQueue.h>
#include <appFramework.h>
#include <sysUtils.h>
#include <aps.h>
#include <configServer.h>
#include <zclCommandAnalyzer.h>
#if (defined _LINK_SECURITY_) && (!defined _LIGHT_LINK_PROFILE_)
  #include <zclSecurityManager.h>
#endif // (defined _LINK_SECURITY_) && (!defined _LIGHT_LINK_PROFILE_)
#if APP_USE_OTAU == 1
  #include <zclOTAUCluster.h>
#endif // APP_USE_OTAU == 1

#if defined(ATMEL_APPLICATION_SUPPORT)
#include <appBindings.h>
#include <N_EndDeviceRobustness.h>
#include <N_Cmi.h>
#endif
#include <sysTimer.h>
#include <sysIdleHandler.h>
#include <apsmeBind.h>
#include <sysAssert.h>

/******************************************************************************
                            Definitions section.
******************************************************************************/
#define ALL_ATTRIBUTES_ARE_WRITTEN 1

#define IS_NON_UNICAST_TRANSMISSION(apsDataInd) \
  (IS_BROADCAST_ADDR((apsDataInd)->dstAddress.shortAddress) || \
  (APS_GROUP_ADDRESS == (apsDataInd)->dstAddrMode))

#define GET_BUFFER_DESCRIPTOR_BY_BUFFER(buffer) \
  GET_STRUCT_BY_FIELD_POINTER(ZclMmBufferDescriptor_t, buf, buffer);

#define REPOST_TASK_TIMER_PERIOD   10

/******************************************************************************
                            Types section
******************************************************************************/
typedef struct
{
  SYS_Timer_t       waitTimer;
  SYS_Timer_t       reportTimer;
  SYS_Timer_t       repostTaskTimer;
  QueueDescriptor_t requestQueue;
} ZclModuleMem_t;

/******************************************************************************
                   Static functions prototype section
******************************************************************************/
static ZCL_Status_t zclExecuteRequest(ZCL_Request_t *req, ZclMmBuffer_t *buf);

static void attributeConfirm(APS_DataConf_t *conf);
static void commandConfirm(APS_DataConf_t *conf);
static void reportConfirm(APS_DataConf_t *conf);
static void responseConfirm(APS_DataConf_t *conf);
static void configureReportingResponseConfirm(APS_DataConf_t *conf);

static void zclWaitTimerFired(void);
static void zclReportTimerFired(void);
static void zclRepostTaskTimerFired(void);
static uint8_t addToReportPayload(uint8_t *reportPayload, ZclAttribute_t *attr);
static void zclVerifyConfirmResponseOrder(APS_DataConf_t *conf);
#ifndef ZAPPSI_HOST
static void isZclBusyOrPollRequest(SYS_EventId_t eventId, SYS_EventData_t data);
#if defined _ENDDEVICE_ && defined _SLEEP_WHEN_IDLE_
static bool zclIsPollRequired(void);
#endif
#endif
static void zclStartWaitResponseTimer(ZclMmBufferDescriptor_t *descriptor);

/******************************************************************************
                   Global variables section
******************************************************************************/
ZclMem_t zclMem;

/******************************************************************************
                   Static variables section
******************************************************************************/
static ZclModuleMem_t zclModuleMem;
#ifndef ZAPPSI_HOST
static SYS_EventReceiver_t zdoBusyPollCheck = { .func = isZclBusyOrPollRequest};
#endif

/******************************************************************************
                   Implementation section
******************************************************************************/
/**************************************************************************//**
  \brief Resets ZigBee Cluster Library.

  \param none.
  \return none.
******************************************************************************/
void ZCL_ResetReq(void)
{
#ifndef ZAPPSI_HOST
  SYS_SubscribeToEvent(BC_EVENT_BUSY_REQUEST, &zdoBusyPollCheck);
  SYS_SubscribeToEvent(BC_EVENT_POLL_REQUEST, &zdoBusyPollCheck);
#endif
  resetQueue(&zclModuleMem.requestQueue);

  zclStopResponseWaitTimer();
  SYS_StopTimer(&zclModuleMem.repostTaskTimer);
  SYS_InitTimer(&zclModuleMem.repostTaskTimer, TIMER_ONE_SHOT_MODE, REPOST_TASK_TIMER_PERIOD, zclRepostTaskTimerFired);
  SYS_StopTimer(&zclModuleMem.reportTimer);

  zclParserInit();

#if (defined _LINK_SECURITY_) && (!defined _LIGHT_LINK_PROFILE_)
  ZCL_ResetSecurity();
#endif // (defined _LINK_SECURITY_) && (!defined _LIGHT_LINK_PROFILE_)
}

/**************************************************************************//**
  \brief Register device application.

  \param[in] endpoint - device descriptor.
  \return none.
******************************************************************************/
void ZCL_RegisterEndpoint(ZCL_DeviceEndpoint_t *endpoint)
{
  endpoint->service.apsEndpoint.simpleDescriptor = &endpoint->simpleDescriptor;
  endpoint->service.apsEndpoint.APS_DataInd = zclDataInd;

  APS_RegisterEndpointReq(&endpoint->service.apsEndpoint);
}

/**************************************************************************//**
  \brief Unregister device application.

  \param[in] endpoint - device descriptor.
  \return none.
******************************************************************************/
void ZCL_UnregisterEndpoint(ZCL_DeviceEndpoint_t *endpoint)
{
  endpoint->service.unregEpReq.endpoint = endpoint->simpleDescriptor.endpoint;
  APS_UnregisterEndpointReq(&endpoint->service.unregEpReq);
}

/**************************************************************************//**
  \brief Request to read/write/configure an attribute on a remote device.

  \param req - attribute descriptor.
  \return none
******************************************************************************/
void ZCL_AttributeReq(ZCL_Request_t *req)
{
  req->service.requestType = ZCL_STANDARD_REQ_TYPE;
  req->dstAddressing.sequenceNumber = ZCL_GetNextSeqNumber();
  putQueueElem(&zclModuleMem.requestQueue, req);
  zclPostTask(ZCL_SUBTASK_ID);
}

/**************************************************************************//**
  \brief Request to send a cluster command to a remote device.

  \param req - request descriptor.
  \return none.
******************************************************************************/
void ZCL_CommandReq(ZCL_Request_t *req)
{
  req->service.requestType = ZCL_SPECIAL_REQ_TYPE;
  putQueueElem(&zclModuleMem.requestQueue, req);
  zclPostTask(ZCL_SUBTASK_ID);
}

/*************************************************************************//**
 \brief Starts attributes reporting if any attributes are configured for
   reporting by application. Restarts reporting if it was in progress.

 \ingroup zcl_common
*****************************************************************************/
void ZCL_StartReporting(void)
{
  uint32_t  minTimeout = ~0ul;
  ApsBindingEntry_t *bindingEntry = NULL;

  /* Go through binding table to activate default attribute reporting */
  while (NULL != (bindingEntry = APS_NextBindingEntry(bindingEntry)))
  {
    if (IS_ACTIVE_BINDING_ENTRY(bindingEntry))
    {
      ZCL_DeviceEndpoint_t *endpoint = zclGetEndpoint(bindingEntry->srcEndpoint);
      ZCL_Cluster_t *cluster = ZCL_GetCluster(bindingEntry->srcEndpoint, bindingEntry->clusterId, ZCL_CLUSTER_SIDE_SERVER);
      ZclAttribute_t *attr;

      if (!endpoint || !cluster)
        continue;

      attr = (ZclAttribute_t *)cluster->attributes;
      // For all attributes
      for (uint8_t attrIndex = 0; attrIndex < cluster->attributesAmount; attrIndex++)
      {
        if ((attr->properties & ZCL_REPORTABLE_ATTRIBUTE) && !(attr->properties & ZCL_REPORTING_CONFIGURED))
        {
          uint8_t attrLength = ZCL_GetAttributeLength(attr->type, (uint8_t *)&attr->value);
          uint8_t *reportCounterPtr = (uint8_t *)attr + ATTRIBUTE_ID_SIZE + ATTRIBUTE_TYPE_SIZE +
                                      sizeof(uint8_t) + attrLength; // properties + value
          ZclReportableAttributeTail_t *tail = (ZclReportableAttributeTail_t *)reportCounterPtr;

          attr->properties |= ZCL_REPORTING_CONFIGURED;
          tail->reportCounter = 0;
          minTimeout = MIN(minTimeout, tail->maxReportInterval);
        }
        attr = jumpToNextAttribute(attr);
      }
    }
  }

  if (~0ul != minTimeout)
  {
    SYS_InitTimer(&zclModuleMem.reportTimer, TIMER_ONE_SHOT_MODE, minTimeout * 1000, zclReportTimerFired);
    SYS_StartTimer(&zclModuleMem.reportTimer);
  }
}

/**************************************************************************//**
  \brief Stops response wait timer
*****************************************************************************/
void zclStopResponseWaitTimer(void)
{
  SYS_StopTimer(&zclModuleMem.waitTimer);
}

/**************************************************************************//**
\brief Execute ZCL request

\param[in] req - request
\param[in] buf - memory buffer

\return ZCL_SUCCESS_STATUS - success case
           ZCL_INVALID_PARAMETER_STATUS - when req has undefined cluster
******************************************************************************/
static ZCL_Status_t zclExecuteRequest(ZCL_Request_t *req, ZclMmBuffer_t *buf)
{
  ZCL_Cluster_t *cluster;
  uint8_t headerLength;
  ZclCommand_t *command;
  APS_DataReq_t *apsdr = &buf->primitive.apsDataReq;

  cluster = ZCL_GetCluster(req->endpointId,
                          req->dstAddressing.clusterId,
                          getSrcClusterSideByDstClusterSide(req->dstAddressing.clusterSide));
  if (NULL == cluster)
  {
    SYS_E_ASSERT_ERROR(false, ZCL_UNDEFINED_CLUSTER_IN_REQUEST);
    return ZCL_INVALID_PARAMETER_STATUS;
  }
  /* if cluster is undefined, control breaks here with failure status */

  req->service.statusflags = ZCL_REQ_INIT_VAL;

  // initialize primitive
  apsdr->asdu = buf->frame + getZclAsduOffset();

  // form APS Data Req primitive
  apsdr->srcEndpoint = req->endpointId;
  apsdr->dstAddrMode = req->dstAddressing.addrMode;
  apsdr->dstAddress  = req->dstAddressing.addr;
  apsdr->dstEndpoint = req->dstAddressing.endpointId;
  apsdr->clusterId   = req->dstAddressing.clusterId;

  if ((PROFILE_ID_LIGHT_LINK == req->dstAddressing.profileId) &&
     (ZLL_COMMISSIONING_CLUSTER_ID != req->dstAddressing.clusterId))
  {
    apsdr->profileId = PROFILE_ID_HOME_AUTOMATION;
  }
  else
  {
    apsdr->profileId = req->dstAddressing.profileId;
  }

  if (ZCL_STANDARD_REQ_TYPE == req->service.requestType)
  {
    apsdr->APS_DataConf = attributeConfirm;
    if ((ZCL_REPORT_ATTRIBUTES_COMMAND_ID == req->id) ||
        (ZCL_WRITE_ATTRIBUTES_NO_RESPONSE_COMMAND_ID == req->id))
    {
      if(ZCL_FRAME_CONTROL_ENABLE_DEFAULT_RESPONSE == req->defaultResponse)
        req->service.statusflags |= ZCL_REQ_DEFAULT_RESP_RQRD | ZCL_REQ_RESP_RQRD;
    }
    else
      req->service.statusflags |= ZCL_REQ_RESP_RQRD;
  }
  else
  {
    apsdr->APS_DataConf = commandConfirm;

    command = zclGetCommandByCluster(cluster,
                                     getOutgoingCommandDirectionByDstClusterSide(req->dstAddressing.clusterSide),
                                     req->id);

    if (command && zclIsResponseRequiredByAddrMode(req))
    {
      if (ZCL_RESPONSE_CONTROLLED_BY_PAYLOAD == command->options.waitingResponse)
        req->service.statusflags |= zclIsResponseRequiredByPayload(req->dstAddressing.clusterId, req->id,
        req->dstAddressing.clusterSide, req->requestPayload) ? ZCL_REQ_RESP_RQRD : 0;
      else if (ZCL_THERE_IS_RELEVANT_RESPONSE == command->options.waitingResponse)
        req->service.statusflags |= ZCL_REQ_RESP_RQRD;
      else if (ZCL_FRAME_CONTROL_ENABLE_DEFAULT_RESPONSE == req->defaultResponse)
        req->service.statusflags |= ZCL_REQ_RESP_RQRD | ZCL_REQ_DEFAULT_RESP_RQRD;
    }

    apsdr->txOptions.acknowledgedTransmission = 0;
    if (command && command->options.ackRequest)
      apsdr->txOptions.acknowledgedTransmission = 1;
  }

  if ((APS_SHORT_ADDRESS == req->dstAddressing.addrMode &&
       IS_BROADCAST_ADDR(req->dstAddressing.addr.shortAddress))
#ifdef _APS_MULTICAST_
       || APS_GROUP_ADDRESS == req->dstAddressing.addrMode
#endif
      )
    apsdr->txOptions.acknowledgedTransmission = 0;

  apsdr->txOptions.noRouteDiscovery = 0;
  apsdr->radius = 0;
  // ---
  // form request header
  headerLength = zclFormRequest(apsdr,
                                req->service.requestType,
                                getOutgoingCommandDirectionByDstClusterSide(req->dstAddressing.clusterSide),
                                req->id,
                                req->defaultResponse,
                                req->dstAddressing.manufacturerSpecCode,
                                req->dstAddressing.sequenceNumber);

  // copy request payload
  SYS_BYTE_MEMCPY(apsdr->asdu + headerLength, req->requestPayload, req->requestLength);
  // set frame payload size
  apsdr->asduLength = headerLength + req->requestLength;

#ifdef _APS_FRAGMENTATION_
  if(apsdr->txOptions.securityEnabledTransmission)
  {
    if (apsdr->asduLength > APS_MAX_ASDU_SIZE)
      apsdr->txOptions.acknowledgedTransmission = 1;
  }
  else /* APS security frame and APS footer are not required*/
  {
    if (apsdr->asduLength > APS_MAX_NON_SECURITY_ASDU_SIZE)
      apsdr->txOptions.acknowledgedTransmission = 1;
  }
#endif // _APS_FRAGMENTATION_

  //send command
  zclApsDataReq(apsdr, cluster->options.security);

  return ZCL_SUCCESS_STATUS;
}

/**************************************************************************//**
  \brief Task handler of ZCL component.
******************************************************************************/
void zclTaskHandler(void)
{
  ZclMmBuffer_t *reqMem;
  ZCL_Request_t *req;
  ZclMmBufferDescriptor_t *descr;

  while (NULL != (req = (ZCL_Request_t *)getQueueElem(&zclModuleMem.requestQueue)))
  {
    // Try to get memory for request
    reqMem = zclMmGetMem(ZCL_OUTPUT_DATA_BUFFER);
    if (!reqMem)
    {
      // no free memory buffers, try later
      SYS_StartTimer(&zclModuleMem.repostTaskTimer);
      return;
    }

    deleteQueueElem(&zclModuleMem.requestQueue, req);

    // verify that request fits into zcl memory buffer
    if ((uint16_t)req->requestLength > ZCL_MAX_TX_ZSDU_SIZE)
    {
      if (req->ZCL_Notify)
      {
        SYS_E_ASSERT_WARN(false, ZCL_BUFFER_SIZE_IS_TOO_LOW);
        req->notify.id = ZCL_APS_CONFIRM_ID;
        req->notify.status = ZCL_INSUFFICIENT_SPACE_STATUS;
        req->ZCL_Notify(&req->notify);
      }
	  else
	  {
	    SYS_E_ASSERT_FATAL(false, ZCL_BUFFER_SIZE_IS_TOO_LOW);
	  }
      zclMmFreeMem(reqMem);
      continue;
    }

    // store the link to an appropriate request
    descr = GET_BUFFER_DESCRIPTOR_BY_BUFFER(reqMem);
    descr->link = req;

    if (ZCL_SUCCESS_STATUS != zclExecuteRequest(req, reqMem))
    {
      if (req->ZCL_Notify)
      {
        req->notify.id = ZCL_APS_CONFIRM_ID;
        req->notify.status = ZCL_INVALID_PARAMETER_STATUS;
        req->ZCL_Notify(&req->notify);
      }
    }
  }
}

/*************************************************************************//**
  \brief Confirmation of APS layer about data sending.

  \param[in] - confirm parameters.
  \return none.
*****************************************************************************/
static void attributeConfirm(APS_DataConf_t *conf)
{
  ZclMmBufferDescriptor_t *descr = GET_PARENT_BY_FIELD(ZclMmBufferDescriptor_t, buf.primitive.apsDataReq.confirm, conf);
  ZCL_Request_t *req             = descr->link;
  ZCL_Notify_t  *ntfy            = &req->notify;

  if (APS_SUCCESS_STATUS == conf->status)
  {
    switch (req->id)
    {
      case ZCL_REPORT_ATTRIBUTES_COMMAND_ID:
      case ZCL_WRITE_ATTRIBUTES_NO_RESPONSE_COMMAND_ID:
        ntfy->status = ZCL_SUCCESS_STATUS;
        ntfy->id = ZCL_APS_CONFIRM_ID;
        if (req->ZCL_Notify)
          req->ZCL_Notify(ntfy);
      case ZCL_DEFAULT_RESPONSE_COMMAND_ID:
        zclMmFreeMem(&descr->buf);
        break;
      default:
        descr->timeout = NWK_GetUnicastDeliveryTime() * 2 + ZCL_RESPONSE_SPACING;
        zclStartWaitResponseTimer(descr);
        break;
    }
  }
  else
  {
    ntfy->status = conf->status;
    ntfy->id = ZCL_APS_CONFIRM_ID;
    if (req->ZCL_Notify)
      req->ZCL_Notify(ntfy);
    zclMmFreeMem(&descr->buf);
    if (NULL == zclMmGetNextOutputMemDescriptor(NULL))
      zclStopResponseWaitTimer();
    return;
  }

  zclVerifyConfirmResponseOrder(conf);
}

/**************************************************************************//**
\brief Confirmation callback for sending of ZCL command

\param[in] conf - confirmation data
******************************************************************************/
static void commandConfirm(APS_DataConf_t *conf)
{
  ZclMmBufferDescriptor_t *descr = GET_PARENT_BY_FIELD(ZclMmBufferDescriptor_t, buf.primitive.apsDataReq.confirm, conf);
  ZCL_Request_t *req             = descr->link;
  ZCL_Notify_t *ntfy = &req->notify;

  // set sending status
  if (APS_SUCCESS_STATUS == conf->status)
    ntfy->status = ZCL_SUCCESS_STATUS;
  else
    ntfy->status = conf->status;

  ntfy->id = ZCL_APS_CONFIRM_ID;
  // call response function
  if ((APS_SUCCESS_STATUS != conf->status) || !(req->service.statusflags & ZCL_REQ_DEFAULT_RESP_RQRD))
  {
    if (req->ZCL_Notify)
      req->ZCL_Notify(ntfy);
  }
  if (APS_SUCCESS_STATUS != conf->status)
  {
    zclMmFreeMem(&descr->buf);
    if (NULL == zclMmGetNextOutputMemDescriptor(NULL))
      zclStopResponseWaitTimer();
    return;
  }
  if (req->responseWaitTimeout)
    descr->timeout = req->responseWaitTimeout;
  else
    descr->timeout = NWK_GetUnicastDeliveryTime();
  zclStartWaitResponseTimer(descr);

  zclVerifyConfirmResponseOrder(conf);
}

/**************************************************************************//**
\brief Confirmation callback for sending ZCL report

\param[in] conf - confirmation data
******************************************************************************/
static void reportConfirm(APS_DataConf_t *conf)
{
  APS_DataReq_t *req = GET_PARENT_BY_FIELD(APS_DataReq_t, confirm, conf);

  ZCL_Cluster_t *cluster = ZCL_GetCluster(req->srcEndpoint, req->clusterId, ZCL_CLUSTER_SIDE_SERVER);
  if (cluster)
    cluster->isReporting = 0;

  // free memory
  zclMmFreeMem((ZclMmBuffer_t *)req);
}

/**************************************************************************//**
\brief Confirmation callback for sending ZCL response

\param[in] conf - confirmation data
******************************************************************************/
static void responseConfirm(APS_DataConf_t *conf)
{
  // free memory
  zclMmFreeMem((ZclMmBuffer_t *)GET_PARENT_BY_FIELD(APS_DataReq_t, confirm, conf));
}


/**************************************************************************//**
\brief Confirmation callback for sending ZCL configure reporting response

\param[in] conf - confirmation data
******************************************************************************/
static void configureReportingResponseConfirm(APS_DataConf_t *conf)
{
  if (APS_SUCCESS_STATUS == conf->status)
  {
    zclStartReportTimer();
  }
  // free memory
  zclMmFreeMem((ZclMmBuffer_t *)GET_PARENT_BY_FIELD(APS_DataReq_t, confirm, conf));
}

/*************************************************************************//**
  \brief Response has been received.

  \param[in] auxData - pointer to structure with command payload and
  descriptor of received command frame.
  \param[in] req - pointer to request, which is the reason of the response.
*****************************************************************************/
void zclResponseInd(ZclAuxParseData_t *auxData, ZCL_Request_t *req)
{
  APS_DataInd_t           *apsData   = auxData->ind;
  ZclFrameDescriptor_t    *frameDesc = auxData->frameDescriptor;
  ZCL_Addressing_t        rxAddressing;

  req->notify.responseLength = apsData->asduLength - frameDesc->headerLength;
  req->notify.responsePayload = apsData->asdu + frameDesc->headerLength;
  req->notify.id = ZCL_ZCL_RESPONSE_ID;
  req->notify.addressing = &rxAddressing;

  rxAddressing.addrMode             = apsData->srcAddrMode;
  rxAddressing.addr                 = apsData->srcAddress;
  rxAddressing.profileId            = apsData->profileId;
  rxAddressing.endpointId           = apsData->srcEndpoint;
  rxAddressing.clusterId            = apsData->clusterId;
  rxAddressing.clusterSide          = getDstClusterSideByIncommingCommandDirection(frameDesc->direction);

  rxAddressing.sequenceNumber = frameDesc->sequenceNumber;

  if (ZCL_FRAME_CONTROL_MANUFACTURER_NONSPECIFIC == frameDesc->manufacturerSpecific)
    rxAddressing.manufacturerSpecCode = 0;
  else
    rxAddressing.manufacturerSpecCode = frameDesc->manufacturerCode;

  if (ZCL_WRITE_ATTRIBUTES_COMMAND_ID == req->id)
  {
    if (ALL_ATTRIBUTES_ARE_WRITTEN == req->notify.responseLength)
      req->notify.status = ZCL_SUCCESS_STATUS;
    else
      req->notify.status = ZCL_WRITE_ATTRIBUTES_FAILURE_STATUS;
  }
  else
    req->notify.status = ZCL_SUCCESS_STATUS;

  if (req->ZCL_Notify)
    req->ZCL_Notify(&req->notify);
}

/*************************************************************************//**
  \brief Report has been received.

  \param[in] apsData - report payload
  \param[in] frameDesc - descriptor of received report frame
*****************************************************************************/
void zclReportInd(APS_DataInd_t *apsData, ZclFrameDescriptor_t *frameDesc)
{
  ZCL_Addressing_t addressing;
  ZCL_Cluster_t *cluster = ZCL_GetCluster(apsData->dstEndpoint, apsData->clusterId, frameDesc->direction);

  if (!cluster || !cluster->ZCL_ReportInd)
    return;

  addressing.addrMode             = apsData->srcAddrMode;
  addressing.addr                 = apsData->srcAddress;
  addressing.profileId            = apsData->profileId;
  addressing.endpointId           = apsData->srcEndpoint;
  addressing.clusterId            = apsData->clusterId;
  addressing.clusterSide          = getDstClusterSideByIncommingCommandDirection(frameDesc->direction);
  addressing.sequenceNumber       = frameDesc->sequenceNumber;

  if (ZCL_FRAME_CONTROL_MANUFACTURER_NONSPECIFIC == frameDesc->manufacturerSpecific)
    addressing.manufacturerSpecCode = 0;
  else
    addressing.manufacturerSpecCode = frameDesc->manufacturerCode;

  cluster->ZCL_ReportInd(&addressing,
                         apsData->asduLength - frameDesc->headerLength,
                         apsData->asdu + frameDesc->headerLength);
}

/*************************************************************************//**
  \brief The attribute has been read or written.

  \param[in] apsDataInd - attribute payload
  \param[in] incomingFrameInfo - information from the header of the incoming frame
  \param[in] event - event type
  \param[in] attributeId - attribute identifier
*****************************************************************************/
void zclAttributeEventInd(const APS_DataInd_t *apsDataInd,
                          const ZclFrameDescriptor_t *incomingFrameInfo,
                          ZCL_AttributeEvent_t event,
                          ZCL_AttributeId_t attributeId)
{
  ZCL_Addressing_t addressing;
  ZCL_Cluster_t *cluster = ZCL_GetCluster(apsDataInd->dstEndpoint, apsDataInd->clusterId, incomingFrameInfo->direction);

  if (!cluster || !cluster->ZCL_AttributeEventInd)
    return;

  addressing.addrMode = apsDataInd->srcAddrMode;
  addressing.addr = apsDataInd->srcAddress;
  addressing.profileId = apsDataInd->profileId;
  addressing.endpointId = apsDataInd->srcEndpoint;
  addressing.clusterId = apsDataInd->clusterId;
  addressing.clusterSide = getDstClusterSideByIncommingCommandDirection(incomingFrameInfo->direction);
  addressing.manufacturerSpecCode = 0;
  addressing.sequenceNumber = incomingFrameInfo->sequenceNumber;

  cluster->ZCL_AttributeEventInd(&addressing, attributeId, event);
}

/*************************************************************************//**
  \brief Special cluster command has been received.

  \param[in] auxData - pointer to structure with command payload and
  descriptor of received command frame.
*****************************************************************************/
void zclCommandInd(ZclAuxParseData_t *auxData)
{
  ZclCommand_t *command;
  APS_DataInd_t *apsData = auxData->ind;
  ZclFrameDescriptor_t *frameDesc = auxData->frameDescriptor;
  ZCL_Addressing_t addressing;

  command = zclGetCommand(apsData->dstEndpoint,
                          apsData->clusterId,
                          frameDesc->direction,
                          frameDesc->commandId);

  // Default result
  auxData->commandStatus = ZCL_UNSUP_CLUSTER_COMMAND_STATUS;

  if (!command || !command->callback)
    return;

  addressing.addrMode             = apsData->srcAddrMode;
  addressing.addr                 = apsData->srcAddress;
  addressing.profileId            = apsData->profileId;
  addressing.endpointId           = apsData->srcEndpoint;
  addressing.clusterId            = apsData->clusterId;
  addressing.clusterSide          = getDstClusterSideByIncommingCommandDirection(frameDesc->direction);
  addressing.sequenceNumber       = frameDesc->sequenceNumber;
  addressing.nonUnicast           = IS_NON_UNICAST_TRANSMISSION(apsData);

  if (ZCL_FRAME_CONTROL_MANUFACTURER_NONSPECIFIC == frameDesc->manufacturerSpecific)
    addressing.manufacturerSpecCode = 0;
  else
    addressing.manufacturerSpecCode = frameDesc->manufacturerCode;

  auxData->commandStatus = command->callback(&addressing, apsData->asduLength - frameDesc->headerLength, apsData->asdu + frameDesc->headerLength);
  if (ZCL_RESPONSE_CONTROLLED_BY_PAYLOAD == command->options.waitingResponse)
    auxData->relevantResponse = zclIsResponseRequiredByPayload(apsData->clusterId,
      frameDesc->commandId, frameDesc->direction, apsData->asdu + frameDesc->headerLength);
  else
    auxData->relevantResponse = (bool)command->options.waitingResponse;
}

/**************************************************************************//**
\brief Handler for Read Reporting Configuration command

\param[in] apsDataInd - APS data indication;
\param[in, out] frameDescriptor - descriptor for response frame creation

\returns true if corresponding request may be removed, false otherwise
******************************************************************************/
bool zclReadReportingConfigurationHandler(APS_DataInd_t *apsDataInd, ZclFrameDescriptor_t *frameDescriptor)
{
  ZclMmBuffer_t *outBuf;
  ZCL_ReadReportingConfigurationResp_t *resp;
  ZCL_ReadReportingConfigurationReq_t *req = (ZCL_ReadReportingConfigurationReq_t *)frameDescriptor->payload;
  uint8_t inPayloadLength = frameDescriptor->payloadLength;
  uint8_t outPayloadLength;
  ZclAttribute_t *attribute;
  Endpoint_t endpoint = apsDataInd->dstEndpoint;
  ClusterId_t clusterId = apsDataInd->clusterId;
  uint8_t predictedRecordLength;
  uint8_t recordsProcessedAmount = 0;
  ZCL_Status_t status;
  uint8_t *ptr;
  APS_DataReq_t *apsDataReq;
  ZCL_DataTypeDescriptor_t dataTypeDescriptor;

  // get buffer for response
  outBuf = zclMmGetMem(ZCL_OUTPUT_RESPONSE_BUFFER);
  if (NULL == outBuf)
    return false;

  apsDataReq = &outBuf->primitive.apsDataReq;
  apsDataReq->asdu = outBuf->frame + getZclAsduOffset();
  apsDataReq->APS_DataConf = responseConfirm;

  // form request header
  outPayloadLength = zclFormRequest(apsDataReq,
                                    ZCL_STANDARD_REQ_TYPE,
                                    ZCL_FRAME_CONTROL_DIRECTION_SERVER_TO_CLIENT,
                                    ZCL_READ_REPORTING_CONFIGURATION_RESPONSE_COMMAND_ID,
                                    ZCL_FRAME_CONTROL_DISABLE_DEFAULT_RESPONSE,
                                    ZCL_FRAME_CONTROL_MANUFACTURER_NONSPECIFIC,
                                    frameDescriptor->sequenceNumber);

  resp = (ZCL_ReadReportingConfigurationResp_t *) (outBuf->frame + getZclAsduOffset() + outPayloadLength);

  while (0 != inPayloadLength)
  {
    status = ZCL_SUCCESS_STATUS;
    //Get the next attribute descriptor to predict possible response record length.
    //It is needed to prevent memeory damage.
    attribute = zclGetAttribute(endpoint, clusterId, frameDescriptor->direction, req->attributeId);
    predictedRecordLength = SLICE_SIZE(ZCL_ReadReportingConfigurationResp_t, status, attributeId);

    if (ZCL_FRAME_CONTROL_DIRECTION_CLIENT_TO_SERVER == req->direction)
    {
      if (attribute)                //attribute is supported
      {
        if (attribute->properties & ZCL_REPORTABLE_ATTRIBUTE)
        {
          ZCL_GetDataTypeDescriptor(attribute->type, attribute->value, &dataTypeDescriptor);
          predictedRecordLength += SLICE_SIZE(ZCL_ReadReportingConfigurationResp_t, attributeType, maxReportingInterval);
          if (ZCL_DATA_TYPE_ANALOG_KIND == dataTypeDescriptor.kind)
            predictedRecordLength += /*reportableChange*/     dataTypeDescriptor.length;
        }
        else
        {
          status = ZCL_UNREPORTABLE_ATTRIBUTE_STATUS;
        }
      }
      else
      {
        status = ZCL_UNSUPPORTED_ATTRIBUTE_STATUS;
      }
    }
    else
    {
      predictedRecordLength += /*timeoutPeriod*/ sizeof(uint16_t);
    }

    if (ZCL_MAX_TX_ZSDU_SIZE >= outPayloadLength + predictedRecordLength)
    {
      resp->status = status;
      resp->direction = req->direction;
      resp->attributeId = req->attributeId;
      if (ZCL_SUCCESS_STATUS == status)
      {
        if (ZCL_FRAME_CONTROL_DIRECTION_CLIENT_TO_SERVER == req->direction)
        {
          resp->attributeType = attribute->type;
          ptr = (uint8_t *)attribute + SLICE_SIZE(ZclAttribute_t, id, properties) + sizeof(ZCL_ReportTime_t) + dataTypeDescriptor.length;
          resp->minReportingInterval = ((u16Packed_t *)ptr)->val;
          ptr += sizeof(ZCL_ReportTime_t);
          resp->maxReportingInterval = ((u16Packed_t *)ptr)->val;
          if (ZCL_DATA_TYPE_ANALOG_KIND == dataTypeDescriptor.kind)
          {
            ptr += sizeof(ZCL_ReportTime_t);
            SYS_BYTE_MEMCPY(resp->reportableChange, ptr, dataTypeDescriptor.length);
          }
        }
        else
        {
          resp->timeoutPeriod = 0xffff;
        }
      }

      if (sizeof(ZCL_ReadReportingConfigurationReq_t) <= inPayloadLength)
      {
        inPayloadLength -= sizeof(ZCL_ReadReportingConfigurationReq_t);
      }
      else
      {
        inPayloadLength = 0;
      }
      req++;
      resp = (ZCL_ReadReportingConfigurationResp_t *)(((uint8_t *) resp) + predictedRecordLength);
      recordsProcessedAmount++;
      outPayloadLength += predictedRecordLength;
    }
    else
    {
      break;
    }
  }

  if (0 == recordsProcessedAmount)
  {
    zclMmFreeMem(outBuf);
  }
  else
  {
    //APS Data Request for response
    apsDataReq->dstAddrMode = apsDataInd->srcAddrMode;
    SYS_BYTE_MEMCPY(&apsDataReq->dstAddress, &apsDataInd->srcAddress, sizeof(APS_Address_t));
    apsDataReq->dstEndpoint = apsDataInd->srcEndpoint;
    apsDataReq->srcEndpoint = apsDataInd->dstEndpoint;
    apsDataReq->clusterId = clusterId;
    apsDataReq->profileId = apsDataInd->profileId;
    apsDataReq->asduLength = outPayloadLength;
    apsDataReq->txOptions.acknowledgedTransmission    = 1;
    apsDataReq->txOptions.doNotDecrypt                = 1;
    apsDataReq->txOptions.noRouteDiscovery            = 0;
    apsDataReq->radius                                = 0;
    apsDataReq->asduLength = outPayloadLength;
    zclApsDataReq(apsDataReq, APS_SECURED_WITH_ANY_LINK_KEY_STATUS(apsDataInd->securityStatus));
  }

  return true;
}


/**************************************************************************//**
\brief Handler for Configure Reporting command

\param[in] apsDataInd - APS data indication
\param[in, out] frameDescriptor - descriptor for creating response frame

\returns true if corresponding request may be removed, false otherwise
******************************************************************************/
bool zclConfigureReportingHandler(APS_DataInd_t *apsDataInd, ZclFrameDescriptor_t *frameDescriptor)
{
  ZclMmBuffer_t *outBuf;
  ZCL_ConfigureReportingResp_t *resp;
  ZCL_ConfigureReportingReq_t *req = (ZCL_ConfigureReportingReq_t *) frameDescriptor->payload;
  uint8_t inPayloadLength = frameDescriptor->payloadLength;
  uint8_t outPayloadLength;
  ZclAttribute_t *attribute;
  Endpoint_t endpoint = apsDataInd->dstEndpoint;
  ClusterId_t clusterId = apsDataInd->clusterId;
  uint8_t predictedRecordLength;
  uint8_t recordsProcessedAmount = 0;
  ZCL_Status_t status;
  uint8_t *ptr;
  APS_DataReq_t *apsDataReq;
  ZCL_DataTypeDescriptor_t dataTypeDescriptor;

  // get buffer for response
  outBuf = zclMmGetMem(ZCL_OUTPUT_RESPONSE_BUFFER);
  if (NULL == outBuf)
    return false;

  apsDataReq = &outBuf->primitive.apsDataReq;
  apsDataReq->asdu = outBuf->frame + getZclAsduOffset();
  apsDataReq->APS_DataConf = configureReportingResponseConfirm;

  // form request header
  outPayloadLength = zclFormRequest(apsDataReq,
                                    ZCL_STANDARD_REQ_TYPE,
                                    getDstClusterSideByIncommingCommandDirection(frameDescriptor->direction),
                                    ZCL_CONFIGURE_REPORTING_RESPONSE_COMMAND_ID,
                                    ZCL_FRAME_CONTROL_DISABLE_DEFAULT_RESPONSE,
                                    ZCL_FRAME_CONTROL_MANUFACTURER_NONSPECIFIC,
                                    frameDescriptor->sequenceNumber);

  resp = (ZCL_ConfigureReportingResp_t *) (outBuf->frame + getZclAsduOffset() + outPayloadLength);

  while (0 != inPayloadLength)
  {
    status = ZCL_SUCCESS_STATUS;
    predictedRecordLength = /*direction*/     sizeof(uint8_t) +
                            /*attributeId*/   sizeof(uint16_t);
    if (ZCL_MAX_TX_ZSDU_SIZE >= outPayloadLength + sizeof(ZCL_ConfigureReportingResp_t))
    {
      if (ZCL_FRAME_CONTROL_DIRECTION_CLIENT_TO_SERVER == req->direction)
      {
        ZCL_GetDataTypeDescriptor(req->attributeType, req->reportableChange, &dataTypeDescriptor);
        predictedRecordLength +=  /*attributeType*/       sizeof(uint8_t) +
                                  /*minReportInterval*/   sizeof(uint16_t) +
                                  /*maxReportInterval*/   sizeof(uint16_t);
        if (ZCL_DATA_TYPE_ANALOG_KIND == dataTypeDescriptor.kind)
          predictedRecordLength += /*reportableChange*/    dataTypeDescriptor.length;

        attribute = zclGetAttribute(endpoint, clusterId, frameDescriptor->direction, req->attributeId);
        if (NULL == attribute)
          status = ZCL_UNSUPPORTED_ATTRIBUTE_STATUS;
        else
        {
          if (!(attribute->properties & ZCL_REPORTABLE_ATTRIBUTE))
            status = ZCL_UNREPORTABLE_ATTRIBUTE_STATUS;
          else
          {
            if (predictedRecordLength <= inPayloadLength)
            {
              if (attribute->type == req->attributeType)
              {
                if (req->maxReportingInterval && (req->maxReportingInterval < req->minReportingInterval))
                    status = ZCL_INVALID_VALUE_STATUS;
                else
                {
                if (0xFFFF != req->maxReportingInterval)
                  attribute->properties |= ZCL_REPORTING_CONFIGURED;
                  else
                    attribute->properties &= ~(uint8_t)ZCL_REPORTING_CONFIGURED;
                //Modify the internal ZCL attribute properties
                ptr = (uint8_t *)attribute + SLICE_SIZE(ZclAttribute_t, id, properties);
                //((u16Packed_t*)ptr)->val = req->maxReportingInterval;
                ptr += sizeof(ZCL_ReportTime_t) + dataTypeDescriptor.length;
                ((u16Packed_t*)ptr)->val = req->minReportingInterval;
                ptr += sizeof(ZCL_ReportTime_t);
                ((u16Packed_t*)ptr)->val = req->maxReportingInterval;
                ptr += sizeof(ZCL_ReportTime_t);
                if (ZCL_DATA_TYPE_ANALOG_KIND == dataTypeDescriptor.kind)
                  SYS_BYTE_MEMCPY(ptr, req->reportableChange, dataTypeDescriptor.length);
                  
                zclAttributeEventInd(apsDataInd, frameDescriptor, ZCL_CONFIGURE_ATTRIBUTE_REPORTING_EVENT, req->attributeId);
              }
              }
              else
                status = ZCL_INVALID_DATA_TYPE_STATUS;
            }
            else
              status = ZCL_INVALID_FIELD_STATUS;
          }
        }
      }
      else
      {
        predictedRecordLength += sizeof(ZCL_ReportTime_t); // timeoutPeriod
        zclAttributeEventInd(apsDataInd, frameDescriptor, ZCL_CONFIGURED_ATTRIBUTE_REPORTING_NOTIFICATION_EVENT, req->attributeId);
      }

      resp->status = status;
      resp->direction = req->direction;
      resp->attributeId = req->attributeId;

      if (predictedRecordLength <= inPayloadLength)
        inPayloadLength -= predictedRecordLength;
      else
        inPayloadLength = 0;
      resp++;
      req = (ZCL_ConfigureReportingReq_t *) (((uint8_t *) req) + predictedRecordLength);
      recordsProcessedAmount++;
      outPayloadLength += sizeof(ZCL_ConfigureReportingResp_t);
    }
    else
      break;
  }

  if (0 == recordsProcessedAmount)
    zclMmFreeMem(outBuf);
  else
  {
    //APS Data Request for response
    apsDataReq->dstAddrMode = apsDataInd->srcAddrMode;
    SYS_BYTE_MEMCPY(&apsDataReq->dstAddress, &apsDataInd->srcAddress, sizeof(APS_Address_t));
    apsDataReq->dstEndpoint = apsDataInd->srcEndpoint;
    apsDataReq->srcEndpoint = apsDataInd->dstEndpoint;
    apsDataReq->clusterId = clusterId;
    apsDataReq->profileId = apsDataInd->profileId;
    apsDataReq->asduLength = outPayloadLength;
    apsDataReq->txOptions.acknowledgedTransmission    = 1;
    apsDataReq->txOptions.doNotDecrypt                = 1;
    apsDataReq->txOptions.noRouteDiscovery            = 0;
    apsDataReq->radius                                = 0;
    apsDataReq->asduLength = outPayloadLength;
    zclApsDataReq(apsDataReq, APS_SECURED_WITH_ANY_LINK_KEY_STATUS(apsDataInd->securityStatus));
  }

  return true;
}

/**************************************************************************//**
\brief Callback for ZCL wait-for-response timer
******************************************************************************/
static void zclWaitTimerFired(void)
{
  ZclMmBufferDescriptor_t *descr = NULL;
  uint32_t minTimeout = ~0ul;

  while (NULL != (descr = zclMmGetNextOutputMemDescriptor(descr)))
  {
    ZCL_Request_t *req = descr->link;

    if (req->service.statusflags & ZCL_REQ_FLAG_CONF_RCVD)
    {
      descr->timeout -= zclModuleMem.waitTimer.timer.interval;
      if (0 == descr->timeout)
      {
        zclMmFreeMem(&descr->buf);
        req->notify.status = ZCL_NO_RESPONSE_ERROR_STATUS;
        req->notify.id     = ZCL_ZCL_RESPONSE_ID;
        if (req->ZCL_Notify)
          req->ZCL_Notify(&req->notify);
      }
      else
        minTimeout = MIN(minTimeout, descr->timeout);
    }
  }

  if (~0ul != minTimeout)
  {
    SYS_InitTimer(&zclModuleMem.waitTimer, TIMER_ONE_SHOT_MODE, minTimeout, zclWaitTimerFired);
    SYS_StartTimer(&zclModuleMem.waitTimer);
  }
  else
    zclStopResponseWaitTimer();
}

/**************************************************************************//**
\brief Callback for ZCL periodic report timer
******************************************************************************/
static void zclReportTimerFired(void)
{
  ZclMmBuffer_t *reportFrame = NULL;
  ZCL_DeviceEndpoint_t *endpoint = NULL;
  ZCL_Cluster_t *cluster = NULL;
  uint8_t clusterCounter;
  uint32_t minTimeout = ~0ul;

  // For all endpoints
  while (NULL != (endpoint = zclNextEndpoint(endpoint)))
  {
    clusterCounter = endpoint->simpleDescriptor.AppInClustersCount;
    // For all clusters
    if (clusterCounter)
      cluster = ZCL_GetHeadCluster(endpoint, ZCL_CLUSTER_SIDE_SERVER);

    while (clusterCounter--)
    {
      uint8_t *reportPayload = NULL;
      uint8_t *attrPtr = cluster->attributes;
      ZclCommand_t *command = NULL;
      // For all attributes from this cluster
      for (uint8_t attrIndex = 0; attrIndex < cluster->attributesAmount; attrIndex++)
      {
        ZclAttribute_t *attr = (ZclAttribute_t *)attrPtr;
        uint8_t attrLength = ZCL_GetAttributeLength(attr->type, (uint8_t *)&attr->value);
		uint8_t boundaryCheckParamLength = 0;
		if (attr->properties & ZCL_BOUNDARY_CHECK)
        {
          if (attr->properties & ZCL_CHECK_OTHER_ATTR)
            boundaryCheckParamLength = sizeof(ZCL_AttributeId_t) * 2;
		  else
            boundaryCheckParamLength = attrLength * 2;
		}
        // Advance to next attribute (length of attribute struct = size of common fields + size of value)
        attrPtr += SLICE_SIZE(ZclAttribute_t, id, properties) + attrLength;

        if (!(attr->properties & ZCL_REPORTABLE_ATTRIBUTE))
        {
          attrPtr += boundaryCheckParamLength;
          continue;
        }
        if (!isReportingPermitted(attr))
        {
          attrPtr += sizeof(ZclReportableAttributeTail_t) + attrLength + sizeof(ZCL_ReportTime_t) + attrLength + boundaryCheckParamLength; // tail + reportableChange + timeoutPeriod + value + boundary check parameters
          continue;
        }

        // Reportable attributes have an extra fields - attributePointer in fact points to them
        ZclReportableAttributeTail_t *tail = (ZclReportableAttributeTail_t *)attrPtr;
        uint8_t *pLastRepValue = attrPtr + sizeof(ZclReportableAttributeTail_t) + attrLength + sizeof(ZCL_ReportTime_t);

        attrPtr += sizeof(ZclReportableAttributeTail_t) + attrLength + sizeof(ZCL_ReportTime_t) + attrLength + boundaryCheckParamLength; // tail + reportableChange + timeoutPeriod + value + boundary check parameters
        tail->reportCounter += zclModuleMem.reportTimer.timer.interval / 1000;

        if (attr->properties & ZCL_ON_CHANGE_REPORT)
        {
          if (tail->reportCounter < tail->minReportInterval)
          {
            minTimeout = MIN(minTimeout, (uint32_t)(tail->minReportInterval - tail->reportCounter));
            continue;
          }
          attr->properties &= ~ZCL_ON_CHANGE_REPORT;
          // Need to send report and check for maxReportInterval to schedulethe next report
          if (tail->maxReportInterval)
            minTimeout = MIN(minTimeout, tail->maxReportInterval);
        }
        else if(tail->maxReportInterval)
        {
          if (tail->reportCounter < tail->maxReportInterval)
          {
          minTimeout = MIN(minTimeout, (uint32_t)(tail->maxReportInterval - tail->reportCounter));
          continue;
        }
          // scheduling next report after sending the current report
        minTimeout = MIN(minTimeout, tail->maxReportInterval);
        }
        else
        {
          // maxReportInterval is zero & change report is not enabled so nothing to do
          continue;
        }
        tail->reportCounter = 0;

        if (NULL == reportFrame)
        {
          // First attribute in report - construct header
          uint8_t headerLength;

          reportFrame = zclMmGetMem(ZCL_OUTPUT_REPORT_BUFFER);
          if (NULL == reportFrame)
          {
            //report will be postponed to next iteration
            continue;
          }
          reportFrame->primitive.apsDataReq.asdu = reportFrame->frame + getZclAsduOffset();
          reportFrame->primitive.apsDataReq.APS_DataConf = reportConfirm;

          // form request header
          headerLength = zclFormRequest(&reportFrame->primitive.apsDataReq,
                                        ZCL_STANDARD_REQ_TYPE,
                                        ZCL_FRAME_CONTROL_DIRECTION_SERVER_TO_CLIENT,
                                        ZCL_REPORT_ATTRIBUTES_COMMAND_ID,
                                        ZCL_FRAME_CONTROL_DISABLE_DEFAULT_RESPONSE,
                                        ZCL_FRAME_CONTROL_MANUFACTURER_NONSPECIFIC,
                                        ZCL_GetNextSeqNumber());

          reportPayload = reportFrame->primitive.apsDataReq.asdu + headerLength;
        }

        memcpy(pLastRepValue, attr->value, attrLength);
        tail->reportCounter = 0;
        reportPayload += addToReportPayload(reportPayload, attr);
      }

      //if there is something to send
      if (reportPayload)
      {
        reportFrame->primitive.apsDataReq.dstAddrMode = APS_NO_ADDRESS;
        reportFrame->primitive.apsDataReq.clusterId = cluster->id;
        reportFrame->primitive.apsDataReq.profileId = endpoint->simpleDescriptor.AppProfileId;
        reportFrame->primitive.apsDataReq.txOptions.acknowledgedTransmission = 0;
        command = zclGetCommandByCluster(cluster,
                                         ZCL_FRAME_CONTROL_DIRECTION_SERVER_TO_CLIENT,
                                         ZCL_REPORT_ATTRIBUTES_COMMAND_ID);
        if (command && command->options.ackRequest)
          reportFrame->primitive.apsDataReq.txOptions.acknowledgedTransmission = 1;

        reportFrame->primitive.apsDataReq.txOptions.doNotDecrypt = 0;
        reportFrame->primitive.apsDataReq.txOptions.noRouteDiscovery = 0;
        reportFrame->primitive.apsDataReq.radius = 0;
        reportFrame->primitive.apsDataReq.srcEndpoint = endpoint->simpleDescriptor.endpoint;
        // set frame payload size
        reportFrame->primitive.apsDataReq.asduLength = reportPayload - reportFrame->primitive.apsDataReq.asdu;
        cluster->isReporting = 1;
        zclApsDataReq(&reportFrame->primitive.apsDataReq, cluster->options.security);
        reportFrame = NULL; //allocate new frame if needed
      }
      if (clusterCounter)
        cluster = ZCL_GetNextCluster(cluster);
    }
  }
  if (~0ul != minTimeout)
  {
    SYS_InitTimer(&zclModuleMem.reportTimer, TIMER_ONE_SHOT_MODE, minTimeout * 1000, zclReportTimerFired);
    SYS_StartTimer(&zclModuleMem.reportTimer);
  }
  else
    SYS_StopTimer(&zclModuleMem.reportTimer);
}

/**************************************************************************//**
\brief Add attribute to payload when generating periodic report

\param[in, out] reportPayload - report payload buffer
\param[in] attr - attribute
\return length of added data
******************************************************************************/
static uint8_t addToReportPayload(uint8_t *reportPayload, ZclAttribute_t *attr)
{
  uint8_t len = SLICE_SIZE(ZclAttribute_t, id, type);
  uint8_t valLen = ZCL_GetAttributeLength(attr->type, attr->value);
  // Copy id and type
  SYS_BYTE_MEMCPY(reportPayload, attr, len);
  // Copy value
  SYS_BYTE_MEMCPY(reportPayload + len, attr->value, valLen);
  return len + valLen;
}

/**************************************************************************//**
  \brief Interface to pass ZCL request to APS

  APS_DataConf field in APS_DataReq must be an actual confirm handler pointer.

  \param[in] req The APS_DataReq_t primitive pointer.
  \param[in] apsSecurity If true APS security is switched on.
*****************************************************************************/
void zclApsDataReq(APS_DataReq_t *req, bool apsSecurity)
{
  req->txOptions.useNwkKey = 0;

#ifdef _APS_FRAGMENTATION_
  req->txOptions.fragmentationPermitted = 1;
#else
  req->txOptions.fragmentationPermitted = 0;
#endif
#if (defined _LINK_SECURITY_) && (!defined _LIGHT_LINK_PROFILE_)
  req->txOptions.securityEnabledTransmission = apsSecurity;
#else
  req->txOptions.securityEnabledTransmission = 0;
#endif // (defined _LINK_SECURITY_) && (!defined _LIGHT_LINK_PROFILE_)
  (void)apsSecurity;

#if defined(ATMEL_APPLICATION_SUPPORT)
  IN_AfDataRequest_DataRequest(req);
#else
  APS_DataReq(req);
#endif
}

/***************************************************************************//**
\brief Checks whether ZCL is busy or not.

\return true - if ZCL is busy, false - otherwise.
******************************************************************************/
bool ZCL_IsBusy(void)
{
#if APP_USE_OTAU == 1
  #ifdef _SLEEP_WHEN_IDLE_
    if (SYS_IsSleepWhenIdleEnabled())
      return false;
    else
  #endif
      return zclIsOtauBusy() || (NULL != zclMmGetNextOutputMemDescriptor(NULL));
#else
  #ifdef _SLEEP_WHEN_IDLE_
    if (SYS_IsSleepWhenIdleEnabled())
      return false;
    else
  #endif
      return (NULL != zclMmGetNextOutputMemDescriptor(NULL));
#endif // APP_USE_OTAU == 1
}

#if APP_USE_OTAU == 1
/***************************************************************************//**
\brief Checks whether ZCL OTAU is busy or not.

\return true - if ZCL OTAU is busy, false - otherwise.
******************************************************************************/
bool ZCL_IsOtauBusy(void)
{
  return zclIsOtauBusy();
}
#endif // APP_USE_OTAU == 1

/**************************************************************************//**
\brief Callback for ZCL repost task timer
******************************************************************************/
static void zclRepostTaskTimerFired(void)
{
  SYS_StopTimer(&zclModuleMem.repostTaskTimer);
  zclPostTask(ZCL_SUBTASK_ID);
}

/**************************************************************************//**
  \brief Verifies order of received confirmation and response

  \param[in] conf - pointer to confirmation structure from APS layer
*****************************************************************************/
static void zclVerifyConfirmResponseOrder(APS_DataConf_t *conf)
{
  ZCL_Request_t *req;
  ZclMmBufferDescriptor_t *descr = NULL;

  while ((descr = zclMmGetNextOutputMemDescriptor(descr)))
  {
    if (&descr->buf.primitive.apsDataReq.confirm == conf)
    {
      req = descr->link;
      if (ZCL_REQ_RESP_RQRD != (req->service.statusflags & ZCL_REQ_RESP_RQRD_MASK))
      { // delete data which have not been needed in responses anymore
        // free memory located by sent request
        zclMmFreeMem(&descr->buf);
        if (NULL == zclMmGetNextOutputMemDescriptor(NULL))
          zclStopResponseWaitTimer();
        return;
      }

      if (ZCL_REQ_FLAG_RESP_RCVD == (req->service.statusflags & ZCL_REQ_RESP_CONF_MASK))
      {
        req->service.statusflags |= ZCL_REQ_FLAG_CONF_RCVD;
        zclPostTask(ZCL_PARSER_TASK_ID);
        return;
      }

      req->service.statusflags |= ZCL_REQ_FLAG_CONF_RCVD;
    }
  }
}

#ifndef ZAPPSI_HOST
/**************************************************************************//**
  \brief Processes BC_EVENT_BUSY_REQUEST request

  \param[in] eventId - id of raised event;
  \param[in] data    - event's data
 ******************************************************************************/
static void isZclBusyOrPollRequest(SYS_EventId_t eventId, SYS_EventData_t data)
{
  bool *check = (bool *)data;

  if (BC_EVENT_BUSY_REQUEST == eventId)
    *check &= !ZCL_IsBusy();
#if defined _ENDDEVICE_ && defined _SLEEP_WHEN_IDLE_
  else if (BC_EVENT_POLL_REQUEST == eventId)
    *check |= zclIsPollRequired();
#endif
}

/**************************************************************************//**
  \brief Checks if polling is required

  \return 'true' if it is otherwise - 'false'
 ******************************************************************************/
#if defined _ENDDEVICE_ && defined _SLEEP_WHEN_IDLE_
static bool zclIsPollRequired(void)
{
  ZclMmBufferDescriptor_t *descr = NULL;
  ZCL_Request_t           *req;

#if APP_USE_OTAU == 1
  if (zclIsOtauBusy())
    return true;
#endif

  while ((descr = zclMmGetNextOutputMemDescriptor(descr)))
  {
    req = descr->link;
    if (ZCL_REQ_RESP_RQRD == (req->service.statusflags & ZCL_REQ_RESP_RQRD_MASK))
      return true;
    else if (ZCL_FRAME_CONTROL_ENABLE_DEFAULT_RESPONSE == req->defaultResponse)
    {
      if ((APS_SHORT_ADDRESS == req->dstAddressing.addrMode &&
          !IS_BROADCAST_ADDR(req->dstAddressing.addr.shortAddress))
#ifdef _APS_MULTICAST_
          || !(APS_GROUP_ADDRESS == req->dstAddressing.addrMode)
#endif
      )
        return true;
    }
  }

  return false;
}

#endif // defined _ENDDEVICE_ && defined _SLEEP_WHEN_IDLE_
#endif // ZAPPSI_HOST

/*************************************************************************//**
  \brief Adjusts ZCL response wait timer's interval. Sets interval to the shortest
    timeout among all timeouts.

  \param[in] timeout - new timeout, ms.
 *****************************************************************************/
static void zclStartWaitResponseTimer(ZclMmBufferDescriptor_t *descriptor)
{
  ZclMmBufferDescriptor_t *descr = NULL;
  uint32_t minTimeout = ~0ul;
  uint32_t curTime    = halGetTimeOfAppTimer();

  // the only active transaction
  if (SYS_TIMER_STOPPED == zclModuleMem.waitTimer.state)
  {
    SYS_InitTimer(&zclModuleMem.waitTimer, TIMER_ONE_SHOT_MODE, descriptor->timeout, zclWaitTimerFired);
    SYS_StartTimer(&zclModuleMem.waitTimer);
    return;
  }

  // look for the shortest timeout
  while (NULL != (descr = zclMmGetNextOutputMemDescriptor(descr)))
  {
    ZCL_Request_t *req = descr->link;

    if(req->service.statusflags & ZCL_REQ_FLAG_CONF_RCVD)
      // skip the request which caused timer restart
      if (descr->timeout && (descr != descriptor))
      {
        if (descr->timeout > (curTime - zclModuleMem.waitTimer.timer.service.sysTimeLabel))
          descr->timeout -= (curTime - zclModuleMem.waitTimer.timer.service.sysTimeLabel);
        else
          descr->timeout = 0;
        minTimeout = MIN(minTimeout, descr->timeout);
      }
  }
  minTimeout = MIN(minTimeout, descriptor->timeout);

  SYS_InitTimer(&zclModuleMem.waitTimer, TIMER_ONE_SHOT_MODE, minTimeout, zclWaitTimerFired);
  SYS_StartTimer(&zclModuleMem.waitTimer);
}

/*************************************************************************//**
  \brief starts the report timer on recieving configure report cmd or
    reportOnChange triggered due to change in attribute value
 *****************************************************************************/
void zclStartReportTimer(void)
{
  ZCL_DeviceEndpoint_t *endpoint = NULL;
  uint32_t  minTimeout = ~0ul;
  uint32_t curTime     = halGetTimeOfAppTimer();

  // look for the closest reporting time
  // For all endpoints
  while (NULL != (endpoint = zclNextEndpoint(endpoint)))
  {
    ZCL_Cluster_t *cluster = ZCL_GetHeadCluster(endpoint, ZCL_CLUSTER_SIDE_SERVER);

    // For all server side clusters
    for (uint8_t clusterIndex = 0; clusterIndex < endpoint->simpleDescriptor.AppInClustersCount; clusterIndex++)
    {
      ZclAttribute_t *attr = (ZclAttribute_t *)cluster->attributes;
      uint8_t attrLength;
      uint8_t *reportCounterPtr;
      ZclReportableAttributeTail_t *tail;
      // For all attributes
      for (uint8_t attrIndex = 0; attrIndex < cluster->attributesAmount; attrIndex++)
      {
        if ((attr->properties & ZCL_REPORTABLE_ATTRIBUTE) && isReportingPermitted(attr))
        {
          attrLength = ZCL_GetAttributeLength(attr->type, (uint8_t *)&attr->value);
          reportCounterPtr = (uint8_t *)attr + ATTRIBUTE_ID_SIZE + ATTRIBUTE_TYPE_SIZE +
                                      sizeof(uint8_t) + attrLength; // properties + value
          tail = (ZclReportableAttributeTail_t *)reportCounterPtr;

          if (0xFFFFu != tail->minReportInterval)
          {
            tail->reportCounter += (curTime - zclModuleMem.reportTimer.timer.service.sysTimeLabel) / 1000;
            if (attr->properties & ZCL_ON_CHANGE_REPORT)
              minTimeout = ((tail->reportCounter >= tail->minReportInterval) ? 0 : MIN(minTimeout,  (uint32_t)(tail->minReportInterval - tail->reportCounter)));
            else if (tail->maxReportInterval)
              minTimeout = ((tail->reportCounter >= tail->maxReportInterval) ? 0 : MIN(minTimeout,  (uint32_t)(tail->maxReportInterval - tail->reportCounter)));
          }
        }
        attr = jumpToNextAttribute(attr);
      }
      cluster = ZCL_GetNextCluster(cluster);
    }
  }
  
  if (minTimeout != 0xFFFFFFFFul)
  {
  SYS_InitTimer(&zclModuleMem.reportTimer, TIMER_ONE_SHOT_MODE, minTimeout * 1000, zclReportTimerFired);
  SYS_StartTimer(&zclModuleMem.reportTimer);
  }
}

#endif // ZCL_SUPPORT == 1
//eof zcl.c
