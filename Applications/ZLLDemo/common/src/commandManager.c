/**************************************************************************//**
  \file commandManager.c

  \brief
    Implementation of ZLLDemo ZCL commands management.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    04.07.11 A. Taradov - Created.
    21.03.12 A. Razinkov - Refactored, renamed from commonClusters.c
******************************************************************************/

/******************************************************************************
                    Includes section
******************************************************************************/
#include <zcl.h>
#include <zdo.h>
#include <clusters.h>
#include <sysUtils.h>
#include <commandManager.h>
#include <debug.h>
#include <zllDemo.h>

/******************************************************************************
                    Definitions section
******************************************************************************/
#define COMMAND_BUFFERS_AMOUNT     4
#define WAITING_RESPONSE_TIMEOUT   2000ul
#define ALL_ATTRIBUTES_ARE_WRITTEN 1

/******************************************************************************
                    Prototypes section
******************************************************************************/
static void processCommandQueue(void);
static void commandZclRequestResp(ZCL_Notify_t *ntfy);

/******************************************************************************
                    Local variables section
******************************************************************************/
static CommandDescriptor_t commandMemory[COMMAND_BUFFERS_AMOUNT];
static ZCL_Request_t commandZclRequest;

static DstAddressing_t addressingInfo;

static QueueDescriptor_t freeCommandBuffersQueue;
static QueueDescriptor_t activeCommandBuffersQueue;

static bool processing = false;

/******************************************************************************
                    Implementations section
******************************************************************************/

/**************************************************************************//**
\brief Initialize clusters' shared functionality
******************************************************************************/
void clustersInit(void)
{
  resetQueue(&freeCommandBuffersQueue);
  resetQueue(&activeCommandBuffersQueue);

  for (uint8_t i = 0; i < COMMAND_BUFFERS_AMOUNT; i++)
  {
    if (!putQueueElem(&freeCommandBuffersQueue, &commandMemory[i]))
    {
      /* failed to queue */
    }
  }

  addressingInfo.defaultResponse = true;
}

/**************************************************************************//**
\brief Set addressing parameters for all outgoing clisters requests
\param[in] dstAddressing - pointer to addressing information structure
******************************************************************************/
void clustersSetDstAddressing(DstAddressing_t *dstAddressing)
{
  addressingInfo = *dstAddressing;
}

/**************************************************************************//**
\brief Get addressing parameters for all outgoing clisters requests
\returns pointer to addressing information structure
******************************************************************************/
DstAddressing_t *clustersGetDstAddressing(void)
{
  return &addressingInfo;
}

/**************************************************************************//**
\brief Fill destination addressing information for the request
\param[in] addressing - structure to fill
******************************************************************************/
void fillDstAddressing(ZCL_Addressing_t *addressing)
{
  addressing->addrMode = addressingInfo.mode;

  if (addressing->addrMode == APS_GROUP_ADDRESS)
    addressing->addr.groupAddress = addressingInfo.addr;
  else if (addressing->addrMode == APS_SHORT_ADDRESS)
    addressing->addr.shortAddress = addressingInfo.addr;
  else if (addressing->addrMode == APS_EXT_ADDRESS)
    addressing->addr.extAddress = addressingInfo.addr;

  addressing->endpointId = addressingInfo.ep;
  addressing->manufacturerSpecCode = addressingInfo.manufacturerSpecCode;
  addressing->profileId = APP_PROFILE_ID;
  addressing->clusterSide = ZCL_CLUSTER_SIDE_SERVER;
}

/**************************************************************************//**
\brief Get a free buffer for a ZCL command
\return A pointer to a command's buffer or NULL if no free buffers are left
******************************************************************************/
CommandDescriptor_t *clustersAllocCommand(void)
{
  CommandDescriptor_t *res;

  res = deleteHeadQueueElem(&freeCommandBuffersQueue);

  if (res)
  {
    memset(res, 0, sizeof(CommandDescriptor_t));
    res->disableDefaultResp = true;
  }

  return res;
}

/**************************************************************************//**
\brief Get a free buffer for a ZCL command
\param[in] req - command to free
******************************************************************************/
void clustersFreeCommand(CommandDescriptor_t *req)
{
  if (!putQueueElem(&freeCommandBuffersQueue, req))
  {
    /* failed to queue */
  }
}

/**************************************************************************//**
\brief Send a command using default addressing
\param[in] req - request parameters
******************************************************************************/
void clustersSendCommand(CommandDescriptor_t *req)
{
  fillDstAddressing(&req->dstAddressing);
  if (!putQueueElem(&activeCommandBuffersQueue, req))
  {
    /* failed to queue */
  }
  processCommandQueue();
}

/**************************************************************************//**
\brief Send a command using the addressing provided in the argument
\param[in] req - request parameters
\param[in] addressing - addressing information specifying the destination
           for the command
******************************************************************************/
void clustersSendCommandWithAddressing(CommandDescriptor_t *req, ZCL_Addressing_t *addressing)
{
  req->dstAddressing = *addressing;
  if (!putQueueElem(&activeCommandBuffersQueue, req))
  {
    /* failed to queue */
  }

  processCommandQueue();
}

/**************************************************************************//**
\brief Process commands in the commands queue
******************************************************************************/
static void processCommandQueue(void)
{
  CommandDescriptor_t *req;
#if (APP_ZLL_DEVICE_TYPE == APP_DEVICE_TYPE_COLOR_SCENE_REMOTE) && (APP_USE_OTAU == 1)
  uint8_t type;
  uint8_t imageUpgradeStatus;
#endif
  if (processing)
    return;

  req = getQueueElem(&activeCommandBuffersQueue);

  commandZclRequest.id              = req->commandId;
  commandZclRequest.endpointId      = req->srcEndpoint ? req->srcEndpoint :
                                      APP_ENDPOINT_COLOR_SCENE_REMOTE;
  commandZclRequest.defaultResponse = addressingInfo.defaultResponse;
  commandZclRequest.dstAddressing   = req->dstAddressing;
  commandZclRequest.dstAddressing.clusterId = req->clusterId;
  commandZclRequest.requestPayload  = (uint8_t *)&req->payload;
  commandZclRequest.requestLength   = req->size;
  commandZclRequest.responseWaitTimeout = WAITING_RESPONSE_TIMEOUT;
  commandZclRequest.ZCL_Notify      = commandZclRequestResp;

  if (req->isAttributeOperation)
  {
    ZCL_AttributeReq(&commandZclRequest);
  }
  else
  {
    if (!req->seqNumberSpecified)
      commandZclRequest.dstAddressing.sequenceNumber = ZCL_GetNextSeqNumber();

    ZCL_CommandReq(&commandZclRequest);
  }

  processing = true;

  // if otau is complete and the end device is waiting for the second upgrade response to 
  // switch, it shall poll whenever a command is initiated.
#if (APP_ZLL_DEVICE_TYPE == APP_DEVICE_TYPE_COLOR_SCENE_REMOTE) && (APP_USE_OTAU == 1)
  ZCL_ReadAttributeValue(APP_ENDPOINT_COLOR_SCENE_REMOTE, OTAU_CLUSTER_ID, ZCL_CLUSTER_SIDE_CLIENT,
                                  ZCL_OTAU_CLUSTER_CLIENT_IMAGE_UPGRADE_STATUS_ID, &type, &imageUpgradeStatus);
  if(imageUpgradeStatus == OTAU_WAITING_TO_UPGRADE)
  {
    ZDO_StartSyncReq();
  }
#endif
}

/**************************************************************************//**
\brief ZCL command response
******************************************************************************/
static void commandZclRequestResp(ZCL_Notify_t *ntfy)
{
  CommandDescriptor_t *req;

  req = deleteHeadQueueElem(&activeCommandBuffersQueue);

  if (!putQueueElem(&freeCommandBuffersQueue, req))
  {
    /* failed to queue */
  }

  processing = false;

  if (getQueueElem(&activeCommandBuffersQueue))
    processCommandQueue();

  if (ZCL_APS_CONFIRM_ID == ntfy->id && ZCL_SUCCESS_STATUS != ntfy->status)
  {
    LOG_STRING(zclConfStr, "ZclConfirm: status = 0x%02x\r\n");
    appSnprintf(zclConfStr, ntfy->status);
  }
  else if (ZCL_ZCL_RESPONSE_ID == ntfy->id)
  {
    ZCL_Request_t *req = GET_STRUCT_BY_FIELD_POINTER(ZCL_Request_t, notify, ntfy);
    LOG_STRING(zclRespStr, "ZclResponse: status = 0x%02x\r\n");

    if ((ZCL_WRITE_ATTRIBUTES_COMMAND_ID == req->id ||
         ZCL_WRITE_ATTRIBUTES_UNDIVIDED_COMMAND_ID == req->id) &&
        ALL_ATTRIBUTES_ARE_WRITTEN != req->notify.responseLength)
    {
      ZCL_NextElement_t element;
      ZCL_WriteAttributeResp_t *writeAttributeResp;

      element.id            = ZCL_WRITE_ATTRIBUTES_RESPONSE_COMMAND_ID;
      element.payloadLength = ntfy->responseLength;
      element.payload       = ntfy->responsePayload;
      element.content       = NULL;

      while (element.payloadLength)
      {
        ZCL_GetNextElement(&element);
        writeAttributeResp = (ZCL_WriteAttributeResp_t *) element.content;
        appSnprintf(zclRespStr, writeAttributeResp->status);
        element.content = NULL;
      }

      (void)writeAttributeResp;
    }
    else
      appSnprintf(zclRespStr, ntfy->status);
  }

  /* Call the response if it is specified */
  if (req->ZCL_Notify)
    req->ZCL_Notify(ntfy);
}

/***************************************************************************//**
\brief Default response handler.

\param[in] resp - pointer to response parametres.
******************************************************************************/
void commandManagerDefaultResponseHandler(ZCL_Addressing_t *addressing, uint8_t payloadLength, uint8_t *payload)
{
  CommandDescriptor_t *req;
  ZCL_DefaultResp_t *defaultResp = (ZCL_DefaultResp_t*)payload;
  LOG_STRING(zclDefRespStr, "ZclDefaultResponse: status = 0x%02x\r\n");

  req = deleteHeadQueueElem(&activeCommandBuffersQueue);
  if (!putQueueElem(&freeCommandBuffersQueue, req))
  {
    /* failed to queue */
  }

  appSnprintf(zclDefRespStr, defaultResp->statusCode);

  processing = false;

  if (getQueueElem(&activeCommandBuffersQueue))
    processCommandQueue();

  (void)defaultResp;
  (void)addressing;
  (void)payloadLength;
}

/**************************************************************************//**
\brief Checks if there are any transmissions in progress

\return true - if a transmission is in progress; false - otherwise
******************************************************************************/
bool transmissionIsInProgress(void)
{
  return processing;
}

// eof commandManager.c
