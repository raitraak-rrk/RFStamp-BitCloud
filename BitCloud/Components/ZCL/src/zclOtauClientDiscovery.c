/************************************************************************//**
  \file zclOtauClientDiscovery.c

  \brief
    The file implements the OTAU server discovery part

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    16.10.14 Karthik.P_u - Created.
  Last change:
    $Id: zclOtauClient.c 27261 2014-10-15 11:19:50Z karthik.p_u $
******************************************************************************/

#if (ZCL_SUPPORT == 1) && (APP_USE_OTAU == 1)

/******************************************************************************
                   Includes section
******************************************************************************/
#include <zclOtauManager.h>
#include <zclOtauClient.h>

#if defined(ATMEL_APPLICATION_SUPPORT)
#include <colorSceneRemoteBindings.h>
#include <N_Zdp_Bindings.h>
#include <N_Cmi.h>
#include <N_EndDeviceRobustness.h>
#endif

/******************************************************************************
                   External variables section
******************************************************************************/
extern ZclOtauClientStateMachine_t stateMachine;
extern ZCL_OtauClusterClientAttributes_t otauClientAttributes;
extern ExtAddr_t serverExtAddr;
extern ExtAddr_t otauUnauthorizedServers[OTAU_MAX_UNAUTHORIZED_SERVERS];
extern QueueDescriptor_t zclOtauServerDiscoveryQueue;
extern ZclOtauImageNotifyParams_t imgNtfyServer;
extern uint8_t retryCount, otauMaxRetryCount;

/******************************************************************************
                   Static variables section
******************************************************************************/
static ZclOtauDiscoveryResult_t otauActiveServer = {
  .busy               = false,
  .serverShortAddress = BROADCAST_ADDR_RX_ON_WHEN_IDLE,
  .serverExtAddress   = ZERO_SERVER_EXT_ADDRESS,
  .serverEndpoint     = 0xFF,
  .service.next       = NULL
};
static uint8_t nwkAddrRetryCount;
static uint8_t matchDescUcRetryCount;
static uint8_t ieeeAddrRetryCount;
#if (defined _LINK_SECURITY_) && (!defined _LIGHT_LINK_PROFILE_)
static uint8_t keyReqRetryCount;
#endif

/******************************************************************************
                   Global variables section
******************************************************************************/
ZclOtauDiscoveryResult_t *actvServer = &otauActiveServer;

/******************************************************************************
                   Implementation section
******************************************************************************/

/***************************************************************************//**
\brief Enqueues an element to discovery queue
******************************************************************************/
void otauEnqueueDiscoveryQueue(ShortAddr_t shortAddr, uint8_t endpoint)
{
  ZCL_OtauClientMem_t *clientMem = zclGetOtauClientMem();
  uint8_t tmpAmount = clientMem->discoveredServerAmount;
  ZclOtauDiscoveryResult_t *tmpMem = clientMem->discoveredServerMem;

  while (tmpAmount--)
  {
    if (tmpMem->busy)
    {
      tmpMem++;
      continue;
    }

    tmpMem->busy = true;
    tmpMem->serverShortAddress = shortAddr;
    tmpMem->serverEndpoint = endpoint;
    putQueueElem(&zclOtauServerDiscoveryQueue, tmpMem);
    break;
  }
}

/***************************************************************************//**
\brief Brodcast Match Desc Response CMD completed status handling
******************************************************************************/
static void otauBroadcastMatchDescRespCmdCompletedStatusHandling(void)
{
  actvServer = (ZclOtauDiscoveryResult_t *)deleteHeadQueueElem(&zclOtauServerDiscoveryQueue);
  if (IS_IMGNTFY_PENDING(imgNtfyServer.addr))
  {
    OTAU_SET_STATE(stateMachine, OTAU_GET_IEEE_ADDR_STATE);
    otauIeeeAddrReq(&imgNtfyServer.addr);
  }
  else
  {
    if (NULL != actvServer)
    {
      OTAU_SET_STATE(stateMachine, OTAU_GET_IEEE_ADDR_STATE);
      otauIeeeAddrReq(actvServer);
    }
    else
    {
      OTAU_SET_STATE(stateMachine, OTAU_WAIT_TO_DISCOVER_STATE);
      otauStartDiscoveryTimer();
    }
  }
}

/***************************************************************************//**
\brief Response on broadcast discovery of upgrade server

\param zdpResp - pointer to response.
******************************************************************************/
void otauBroadcastMatchDescResp(ZDO_ZdpResp_t *zdpResp)
{
  if (!OTAU_CHECK_STATE(stateMachine, OTAU_GET_MATCH_DESC_BROADCAST_STATE))
  {
    SYS_E_ASSERT_WARN(false, ZCL_OTAU_INVALID_STATE_BROADCAST_MATCH_DESC_RSP);
    return;
  }

  if ((ZDO_SUCCESS_STATUS == zdpResp->respPayload.status) && (zdpResp->respPayload.matchDescResp.matchLength))
  {
    otauEnqueueDiscoveryQueue(zdpResp->respPayload.matchDescResp.nwkAddrOfInterest, \
                              zdpResp->respPayload.matchDescResp.matchList[0]);
  }
  else if (ZDO_CMD_COMPLETED_STATUS == zdpResp->respPayload.status)
  {
    otauStartGenericTimer(CONTEXT_BREAK_QUICK_TIMEOUT, otauBroadcastMatchDescRespCmdCompletedStatusHandling);
  }
  else
  {
    zclRaiseCustomMessage(OTAU_SERVER_HAS_NOT_BEEN_FOUND);
    OTAU_SET_STATE(stateMachine, OTAU_WAIT_TO_DISCOVER_STATE);
    otauStartDiscoveryTimer();
  }
}

/***************************************************************************//**
\brief Prepares the state machine to move to query state
******************************************************************************/
void otauTransitionToQuery(void)
{
  ExtAddr_t tcExtAddr = NULL;

#if (defined _LINK_SECURITY_) && (!defined _LIGHT_LINK_PROFILE_)
  if (!APS_KEYS_FOUND(APS_FindKeys(&actvServer->serverExtAddress)))
  {
    CS_ReadParameter(CS_APS_TRUST_CENTER_ADDRESS_ID, &tcExtAddr);
    OTAU_SET_STATE(stateMachine, OTAU_GET_LINK_KEY_STATE);
    otauRequestKeyReq(&tcExtAddr);
    return;
  }
#endif // (defined _LINK_SECURITY_) && (!defined _LIGHT_LINK_PROFILE_)
  OTAU_SET_STATE(stateMachine, OTAU_QUERY_FOR_IMAGE_STATE);
  otauStartQuery();
  (void)tcExtAddr;
  return;
}

/***************************************************************************//**
\brief Unicast Match Desc Response Img Notify pending handling
******************************************************************************/
static void otauUnicastMatchDescRespImgNtfyHandling(void)
{
  otauCheckServerAddrAndTakeAction(false, false);
}

/***************************************************************************//**
\brief Unicast Match Desc Retry handling
******************************************************************************/
static void otauUnicastMatchDescRetryHandling(void)
{
  if (matchDescUcRetryCount--)
  {
    otauMatchDescReq(otauUnicastMatchDescResp);
  }
  else
  {
    matchDescUcRetryCount = otauMaxRetryCount;
    CS_ReadParameter(CS_ZCL_OTAU_DEFAULT_UPGRADE_SERVER_IEEE_ADDRESS_ID, &serverExtAddr);

    if (IS_EQ_EXT_ADDR(serverExtAddr, actvServer->serverExtAddress))
    {
      serverExtAddr = ZERO_SERVER_EXT_ADDRESS;
      actvServer = &otauActiveServer;
    }

    if (IS_IMGNTFY_PENDING(imgNtfyServer.addr))
    {
      OTAU_SET_STATE(stateMachine, OTAU_GET_IEEE_ADDR_STATE);
      otauIeeeAddrReq(&imgNtfyServer.addr);
    }
    else
    {
      OTAU_SET_STATE(stateMachine, OTAU_WAIT_TO_DISCOVER_STATE);
      otauStartDiscoveryTimer();
    }
  }
}

/***************************************************************************//**
\brief Response on unicast match descriptor discovery

\param[in] zdpResp - pointer to response.
******************************************************************************/
void otauUnicastMatchDescResp(ZDO_ZdpResp_t *zdpResp)
{
  if (!OTAU_CHECK_STATE(stateMachine, OTAU_GET_MATCH_DESC_UNICAST_STATE))
  {
    {
      SYS_E_ASSERT_WARN(false, ZCL_OTAU_INVALID_STATE_UNICAST_MATCH_DESC_RSP);
    }
    return;
  }

  if (ZDO_SUCCESS_STATUS == zdpResp->respPayload.status)
  {
    actvServer->serverEndpoint = zdpResp->respPayload.matchDescResp.matchList[0];
    matchDescUcRetryCount = otauMaxRetryCount;
  }
  else if (ZDO_CMD_COMPLETED_STATUS == zdpResp->respPayload.status)
  {
    if (IS_IMGNTFY_PENDING(imgNtfyServer.addr))
    {
      // if IN server is the active server, proceed. Otherwise,
      // send IEEE Addr Req to IN server
      otauStartGenericTimer(CONTEXT_BREAK_QUICK_TIMEOUT, otauUnicastMatchDescRespImgNtfyHandling);
    }
    else
    {
      retryCount = otauMaxRetryCount;
      otauTransitionToQuery();
    }
  }
  else
  {
    otauStartGenericTimer(CONTEXT_BREAK_QUICK_TIMEOUT, otauUnicastMatchDescRetryHandling);
  }
}

/***************************************************************************//**
\brief Send match descriptor response to know server endpoint

\param[in] zdoZdpResp - pointer to response
******************************************************************************/
void otauMatchDescReq(void (* zdoZdpResp)(ZDO_ZdpResp_t *zdpResp))
{
  ZCL_OtauClientMem_t *clientMem = zclGetOtauClientMem();
  ZDO_ZdpReq_t *tmpZdpReq = &clientMem->reqMem.zdpOtauReq;

  if ((!OTAU_CHECK_STATE(stateMachine, OTAU_GET_MATCH_DESC_BROADCAST_STATE)) && \
      (!OTAU_CHECK_STATE(stateMachine, OTAU_GET_MATCH_DESC_UNICAST_STATE)))
  {
    SYS_E_ASSERT_ERROR(false, ZCL_OTAU_INVALID_STATE_MATCH_DESC_REQ);
    return;
  }

  tmpZdpReq->reqCluster = MATCH_DESCRIPTOR_CLID;
  tmpZdpReq->dstAddrMode = APS_SHORT_ADDRESS;

  tmpZdpReq->dstAddress.shortAddress = actvServer->serverShortAddress;

  tmpZdpReq->req.reqPayload.matchDescReq.nwkAddrOfInterest = tmpZdpReq->dstAddress.shortAddress;
  tmpZdpReq->req.reqPayload.matchDescReq.profileId = zclOtauMem.initParam.profileId;
  tmpZdpReq->req.reqPayload.matchDescReq.numInClusters = 1;
  tmpZdpReq->req.reqPayload.matchDescReq.inClusterList[0] = OTAU_CLUSTER_ID;
  tmpZdpReq->req.reqPayload.matchDescReq.numOutClusters = 0;
  tmpZdpReq->ZDO_ZdpResp = zdoZdpResp;

  isOtauBusy = true;
#if defined(ATMEL_APPLICATION_SUPPORT)
  N_Zdp_ZdpRequest(tmpZdpReq);
#else
  ZDO_ZdpReq(tmpZdpReq);
#endif
}

/***************************************************************************//**
\brief Unicast Match Desc Retry handling
******************************************************************************/
static void otauIeeeAddrReqFailureHandling(void)
{
  ZclOtauDiscoveryResult_t *srv = actvServer;

  if (IS_IMGNTFY_PENDING(imgNtfyServer.addr))
  {
    srv = &imgNtfyServer.addr;
  }

  if (ieeeAddrRetryCount--)
  {
    otauIeeeAddrReq(srv);
  }
  else
  {
    ieeeAddrRetryCount = otauMaxRetryCount;
    if (IS_IMGNTFY_PENDING(imgNtfyServer.addr))
    {
      // No response from IN server. Choose another candidate. Drop IN.
      ieeeAddrRetryCount = otauMaxRetryCount;
      otauWriteImgNtfyBusyStatus(false);
      // push actvserver to disc Q, and start discovery
      if (actvServer)
      {
        otauEnqueueDiscoveryQueue(actvServer->serverShortAddress, actvServer->serverEndpoint);
      }
    }

    actvServer = (ZclOtauDiscoveryResult_t *)deleteHeadQueueElem(&zclOtauServerDiscoveryQueue);
    if (NULL != actvServer)
    {
      ieeeAddrRetryCount = otauMaxRetryCount;
      otauIeeeAddrReq(actvServer);
    }
    else
    {
      zclRaiseCustomMessage(OTAU_SERVER_HAS_NOT_BEEN_FOUND);
      OTAU_SET_STATE(stateMachine, OTAU_WAIT_TO_DISCOVER_STATE);
      otauStartDiscoveryTimer();
    }
  }
}

/***************************************************************************//**
\brief Send ieeeAddrReq to Image Notify server
******************************************************************************/
static void otauIeeeAddrReqToImgNtfyServer(void)
{
  otauIeeeAddrReq(&imgNtfyServer.addr);
}

/***************************************************************************//**
\brief Send ieeeAddrReq to active server
******************************************************************************/
static void otauIeeeAddrReqToActiveServer(void)
{
  otauIeeeAddrReq(actvServer);
}

/***************************************************************************//**
\brief Server IEEE addresses response

\param zdpResp - pointer to response.
******************************************************************************/
void otauIeeeAddrResp(ZDO_ZdpResp_t *zdpResp)
{
  uint8_t i;
  bool serverIsUnauthorized = false;
  ExtAddr_t tcExtAddr = NULL, ieeeAddr;

  if (!OTAU_CHECK_STATE(stateMachine, OTAU_GET_IEEE_ADDR_STATE))
  {
    SYS_E_ASSERT_WARN(false, ZCL_OTAU_INVALID_STATE_IEEE_ADDR_RSP);
    return;
  }

  if (ZDO_SUCCESS_STATUS != zdpResp->respPayload.status)
  {
    if (IS_IMGNTFY_PENDING(imgNtfyServer.addr))
    {
      if (zdpResp->srcAddress.shortAddress != imgNtfyServer.addr.serverShortAddress)
      {
        // received failure resp from another server.Restart the req with IN srv
        retryCount = otauMaxRetryCount;
      }
    }
    otauStartGenericTimer(CONTEXT_BREAK_QUICK_TIMEOUT, otauIeeeAddrReqFailureHandling);
    return;
  }

  ieeeAddrRetryCount = otauMaxRetryCount;

  for (i = 0; i < OTAU_MAX_UNAUTHORIZED_SERVERS; i++)
  {
    COPY_EXT_ADDR(ieeeAddr, zdpResp->respPayload.ieeeAddrResp.ieeeAddrRemote);
    if (IS_EQ_EXT_ADDR(otauUnauthorizedServers[i], ieeeAddr))
    {
      serverIsUnauthorized = true;
    }
  }

  if(serverIsUnauthorized)
  {
    if (IS_IMGNTFY_PENDING(imgNtfyServer.addr))
    {
      if (zdpResp->respPayload.ieeeAddrResp.ieeeAddrRemote == imgNtfyServer.addr.serverShortAddress)
      {
        otauWriteImgNtfyBusyStatus(false);
      }
      else
      {
        // active server is an unauthorized server, continue with IN server
        otauStartGenericTimer(CONTEXT_BREAK_QUICK_TIMEOUT, otauIeeeAddrReqToImgNtfyServer);
        return;
      }
    }
    else
    {
      actvServer = (ZclOtauDiscoveryResult_t *)deleteHeadQueueElem(&zclOtauServerDiscoveryQueue);
    }

    if (NULL != actvServer)
    {
      otauStartGenericTimer(CONTEXT_BREAK_QUICK_TIMEOUT, otauIeeeAddrReqToActiveServer);
    }
    else
    {
      OTAU_SET_STATE(stateMachine, OTAU_WAIT_TO_DISCOVER_STATE);
      otauStartDiscoveryTimer();
    }
  }
  else
  {
    ieeeAddrRetryCount = otauMaxRetryCount;

    if (IS_IMGNTFY_PENDING(imgNtfyServer.addr))
    {
      if (zdpResp->srcAddress.shortAddress == imgNtfyServer.addr.serverShortAddress)
      {
        // Choosing IN server going forward. Queue the previously chosen
        // candidate back(ideally to the top of the queue)
        if (actvServer &&
            (actvServer->serverShortAddress != zdpResp->srcAddress.shortAddress))
        {
          otauEnqueueDiscoveryQueue(actvServer->serverShortAddress, actvServer->serverEndpoint);
          actvServer->serverShortAddress = imgNtfyServer.addr.serverShortAddress;
          actvServer->serverEndpoint = imgNtfyServer.addr.serverEndpoint;
        }
        otauWriteImgNtfyBusyStatus(false);
      }
      else
      {
        // received successful response for another candidate. Now,
        // send req to IN server
        otauStartGenericTimer(CONTEXT_BREAK_QUICK_TIMEOUT, otauIeeeAddrReqToImgNtfyServer);
        return;
      }
    }

    COPY_EXT_ADDR(actvServer->serverExtAddress, zdpResp->respPayload.ieeeAddrResp.ieeeAddrRemote);
    COPY_EXT_ADDR(serverExtAddr, actvServer->serverExtAddress);
    otauTransitionToQuery();

    return;
  }
  (void)tcExtAddr;
}

/***************************************************************************//**
\brief Server IEEE addresses request
******************************************************************************/
void otauIeeeAddrReq(ZclOtauDiscoveryResult_t *srv)
{
  ZCL_OtauClientMem_t *clientMem = zclGetOtauClientMem();
  ZDO_ZdpReq_t *tmpZdpReq = &clientMem->reqMem.zdpOtauReq;

  if (!OTAU_CHECK_STATE(stateMachine, OTAU_GET_IEEE_ADDR_STATE))
  {
    SYS_E_ASSERT_ERROR(false, ZCL_OTAU_INVALID_STATE_IEEE_ADDR_REQ);
    return;
  }

  tmpZdpReq->reqCluster = IEEE_ADDR_CLID;
  tmpZdpReq->dstAddrMode = APS_SHORT_ADDRESS;

  tmpZdpReq->dstAddress.shortAddress = srv->serverShortAddress;

  tmpZdpReq->req.reqPayload.ieeeAddrReq.nwkAddrOfInterest = srv->serverShortAddress;
  tmpZdpReq->req.reqPayload.ieeeAddrReq.reqType = 0;
  tmpZdpReq->ZDO_ZdpResp = otauIeeeAddrResp;

#if defined(ATMEL_APPLICATION_SUPPORT)
  N_Zdp_ZdpRequest(&clientMem->reqMem.zdpOtauReq);
#else
  ZDO_ZdpReq(&clientMem->reqMem.zdpOtauReq);
#endif
}

/***************************************************************************//**
\brief Send match desc req uincast
******************************************************************************/
static void otauSendMatchDescReqUnicast(void)
{
  otauMatchDescReq(otauUnicastMatchDescResp);
}

/***************************************************************************//**
\brief Server short address response.

\param zdpResp - pointer to response structute.
******************************************************************************/
void otauNwkAddrResp(ZDO_ZdpResp_t *zdpResp)
{
  if (!OTAU_CHECK_STATE(stateMachine, OTAU_GET_NWK_ADDR_STATE))
  {
    SYS_E_ASSERT_WARN(false, ZCL_OTAU_INVALID_STATE_NWK_ADDR_RSP);
    return;
  }

  if (ZDO_SUCCESS_STATUS == zdpResp->respPayload.status)
  {
    nwkAddrRetryCount = otauMaxRetryCount;
    actvServer->serverShortAddress = zdpResp->respPayload.nwkAddrResp.nwkAddrRemote;

    if (IS_IMGNTFY_PENDING(imgNtfyServer.addr))
    {
      if (zdpResp->srcAddress.shortAddress == imgNtfyServer.addr.serverShortAddress)
      {
        // since EP, Nwk Addr, Ext Addr are known, goto query
        actvServer->serverEndpoint = imgNtfyServer.addr.serverEndpoint;
        otauWriteImgNtfyBusyStatus(false);
        nwkAddrRetryCount = otauMaxRetryCount;
        otauTransitionToQuery();
        return;
      }
      else
      {
        // choose IN as the server going forward
        OTAU_SET_STATE(stateMachine, OTAU_GET_IEEE_ADDR_STATE);
        otauStartGenericTimer(CONTEXT_BREAK_QUICK_TIMEOUT, otauIeeeAddrReqToImgNtfyServer);
        return;
      }
    }
    OTAU_SET_STATE(stateMachine, OTAU_GET_MATCH_DESC_UNICAST_STATE);
    otauStartGenericTimer(CONTEXT_BREAK_QUICK_TIMEOUT, otauSendMatchDescReqUnicast);
    return;
  }

  if (nwkAddrRetryCount--)
  {
    otauStartGenericTimer(CONTEXT_BREAK_QUICK_TIMEOUT, otauNwkAddrReq);
  }
  else
  {
    nwkAddrRetryCount = otauMaxRetryCount;
    CS_ReadParameter(CS_ZCL_OTAU_DEFAULT_UPGRADE_SERVER_IEEE_ADDRESS_ID, &serverExtAddr);

    if (IS_EQ_EXT_ADDR(serverExtAddr, actvServer->serverExtAddress))
    {
      serverExtAddr = ZERO_SERVER_EXT_ADDRESS;
      actvServer = &otauActiveServer;
    }

    if (IS_IMGNTFY_PENDING(imgNtfyServer.addr))
    {
      OTAU_SET_STATE(stateMachine, OTAU_GET_IEEE_ADDR_STATE);
      otauStartGenericTimer(CONTEXT_BREAK_QUICK_TIMEOUT, otauIeeeAddrReqToImgNtfyServer);
    }
    else
    {
      OTAU_SET_STATE(stateMachine, OTAU_WAIT_TO_DISCOVER_STATE);
      otauStartDiscoveryTimer();
    }
  }
}

/***************************************************************************//**
\brief Send server short address request
******************************************************************************/
void otauNwkAddrReq(void)
{
  ZCL_OtauClientMem_t *clientMem = zclGetOtauClientMem();
  ZDO_ZdpReq_t *tmpZdpReq = &clientMem->reqMem.zdpOtauReq;

  if (!OTAU_CHECK_STATE(stateMachine, OTAU_GET_NWK_ADDR_STATE))
  {
    SYS_E_ASSERT_ERROR(false, ZCL_OTAU_INVALID_STATE_NWK_ADDR_REQ);
    return;
  }

  tmpZdpReq->reqCluster = NWK_ADDR_CLID;
  tmpZdpReq->dstAddrMode = APS_EXT_ADDRESS;

  COPY_EXT_ADDR(tmpZdpReq->dstAddress.extAddress, actvServer->serverExtAddress);

  tmpZdpReq->req.reqPayload.nwkAddrReq.ieeeAddrOfInterest = actvServer->serverExtAddress;
  tmpZdpReq->req.reqPayload.nwkAddrReq.reqType = 1;
  tmpZdpReq->ZDO_ZdpResp = otauNwkAddrResp;
#if defined(ATMEL_APPLICATION_SUPPORT)
  N_Zdp_ZdpRequest(tmpZdpReq);
#else
  ZDO_ZdpReq(tmpZdpReq);
#endif
}

#if (defined _LINK_SECURITY_) && (!defined _LIGHT_LINK_PROFILE_)
/***************************************************************************//**
\brief APS get link key response

\param conf - pointer to confirm.
******************************************************************************/
void otauRequestKeyResp(APS_RequestKeyConf_t *conf)
{
  ExtAddr_t tcExtAddr;
  
  if (!OTAU_CHECK_STATE(stateMachine, OTAU_GET_LINK_KEY_STATE))
  {
    SYS_E_ASSERT_WARN(false, ZCL_OTAU_INVALID_STATE_REQUEST_KEY_RSP);
    return;
  }

  if (APS_RKR_SUCCESS_STATUS == conf->status)
  {
    keyReqRetryCount = otauMaxRetryCount;
    COPY_EXT_ADDR(serverExtAddr, actvServer->serverExtAddress);
    if (IS_IMGNTFY_PENDING(imgNtfyServer.addr))
    {
      if (ZCL_SUCCESS_STATUS == otauCheckServerAddrAndTakeAction(false, false))
      {
        return;
      }
    }

    zclRaiseCustomMessage(OTAU_SERVER_HAS_BEEN_FOUND);
    OTAU_SET_STATE(stateMachine, OTAU_QUERY_FOR_IMAGE_STATE);
    otauStartQuery();
    return;
  }

  if (keyReqRetryCount--)
  {
    if (IS_IMGNTFY_PENDING(imgNtfyServer.addr))
    {
      if (actvServer->serverShortAddress != imgNtfyServer.addr.serverShortAddress)
      {
        OTAU_SET_STATE(stateMachine, OTAU_GET_IEEE_ADDR_STATE);
        otauStartGenericTimer(CONTEXT_BREAK_QUICK_TIMEOUT, otauIeeeAddrReqToImgNtfyServer);
        return;
      }
    }
    CS_ReadParameter(CS_APS_TRUST_CENTER_ADDRESS_ID, &tcExtAddr);
    otauRequestKeyReq(&tcExtAddr);
  }
  else
  {
    if (IS_IMGNTFY_PENDING(imgNtfyServer.addr))
    {
      if (actvServer->serverShortAddress != imgNtfyServer.addr.serverShortAddress)
      {
        OTAU_SET_STATE(stateMachine, OTAU_GET_IEEE_ADDR_STATE);
        otauStartGenericTimer(CONTEXT_BREAK_QUICK_TIMEOUT, otauIeeeAddrReqToImgNtfyServer);
        return;
      }
      else
      {
        //drop IN
        otauWriteImgNtfyBusyStatus(false);
      }
    }

    keyReqRetryCount = otauMaxRetryCount;
    CS_ReadParameter(CS_ZCL_OTAU_DEFAULT_UPGRADE_SERVER_IEEE_ADDRESS_ID, &serverExtAddr);

    if (IS_EQ_EXT_ADDR(serverExtAddr, actvServer->serverExtAddress))
    {
      serverExtAddr = ZERO_SERVER_EXT_ADDRESS;
      actvServer = &otauActiveServer;
    }

    OTAU_SET_STATE(stateMachine, OTAU_WAIT_TO_DISCOVER_STATE);
    otauStartDiscoveryTimer();
  }
}

/***************************************************************************//**
\brief APS link key request

\param tcAddr - pointer to trust center extended address.
******************************************************************************/
void otauRequestKeyReq(ExtAddr_t *tcExtAddr)
{
  ZCL_OtauClientMem_t *clientMem = zclGetOtauClientMem();
  APS_RequestKeyReq_t *tmpKeyReq = &clientMem->reqMem.apsKeyReq;

  if (!OTAU_CHECK_STATE(stateMachine, OTAU_GET_LINK_KEY_STATE))
  {
    SYS_E_ASSERT_ERROR(false, ZCL_OTAU_INVALID_STATE_REQUEST_KEY_REQ);
    return;
  }

  COPY_EXT_ADDR(tmpKeyReq->destAddress, *tcExtAddr);
  tmpKeyReq->partnerAddress = actvServer->serverExtAddress;
  tmpKeyReq->keyType = APS_APP_KEY_TYPE;

  tmpKeyReq->APS_RequestKeyConf = otauRequestKeyResp;
  APS_RequestKeyReq(tmpKeyReq);
}
#endif // (defined _LINK_SECURITY_) && (!defined _LIGHT_LINK_PROFILE_)

/***************************************************************************//**
\brief Start timer to elapse discovery interval
******************************************************************************/
void otauStartDiscoveryTimer(void)
{
  uint32_t discoveryPeriod = 5000ul;
  CS_ReadParameter(CS_ZCL_OTAU_DEFAULT_SERVER_DISCOVERY_PERIOD_ID, (void *)&discoveryPeriod);

  isOtauBusy = false;

  otauStartGenericTimer(discoveryPeriod, otauStartDiscovery);
}

/***************************************************************************//**
\brief Start discovery of upgrade server
******************************************************************************/
void otauStartDiscovery(void)
{
  uint8_t i;
  ExtAddr_t zeroAddr = ZERO_SERVER_EXT_ADDRESS;
  ZCL_OtauClientMem_t *clientMem = zclGetOtauClientMem();

  if (!OTAU_CHECK_STATE(stateMachine, OTAU_WAIT_TO_DISCOVER_STATE))
  {
    SYS_E_ASSERT_ERROR(false, ZCL_OTAU_INVALID_STATE_START_DISCOVERY);
    return;
  }

  clientMem->blockRequestDelayOn = OTAU_BLOCK_REQUEST_DELAY_OFF;

  for (i = 0; i < OTAU_MAX_UNAUTHORIZED_SERVERS; i++)
  {
    COPY_EXT_ADDR(otauUnauthorizedServers[i], zeroAddr);
  }

  resetQueue(&zclOtauServerDiscoveryQueue);
  zclClearOtauClientMemory();
  CS_ReadParameter(CS_ZCL_OTAU_MAX_RETRY_COUNT_ID, &matchDescUcRetryCount);
  CS_ReadParameter(CS_ZCL_OTAU_MAX_RETRY_COUNT_ID, &ieeeAddrRetryCount);
  CS_ReadParameter(CS_ZCL_OTAU_MAX_RETRY_COUNT_ID, &nwkAddrRetryCount);
#if (defined _LINK_SECURITY_) && (!defined _LIGHT_LINK_PROFILE_)
  CS_ReadParameter(CS_ZCL_OTAU_MAX_RETRY_COUNT_ID, &keyReqRetryCount);
#endif
  
  if (!isExtAddrValid(serverExtAddr))
  {
    OTAU_SET_STATE(stateMachine, OTAU_GET_MATCH_DESC_BROADCAST_STATE);

    actvServer = &otauActiveServer;
    otauMatchDescReq(otauBroadcastMatchDescResp);
  }
  else
  {
    OTAU_SET_STATE(stateMachine, OTAU_GET_NWK_ADDR_STATE);
    actvServer->busy = true;
    COPY_EXT_ADDR(actvServer->serverExtAddress, serverExtAddr);
    otauNwkAddrReq();
  }
}
#endif // (ZCL_SUPPORT == 1) && (APP_USE_OTAU == 1)

//eof zclOtauClientDiscovery.c
