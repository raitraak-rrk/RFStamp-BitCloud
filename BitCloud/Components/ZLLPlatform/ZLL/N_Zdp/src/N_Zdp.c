/**************************************************************************//**
  \file N_Zdp.c

  \brief Implementation of interface to ZDP fot BitCloud stack.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    28.08.12 A. Razinkov - created
******************************************************************************/

/******************************************************************************
                    Includes section
******************************************************************************/
#include <zdo.h>
#include <sysUtils.h>

#include <N_Address.h>
#include <N_ErrH.h>
#include <N_Task.h>

#include <N_Zdp_Bindings.h>
#include <N_Zdp.h>
#include <N_Zdp_Init.h>
#include <N_DeviceInfo_Bindings.h>
#include <N_DeviceInfo.h>
#include <N_Cmi.h>
#include <N_EndDeviceRobustness.h>
#include <N_Binding_Bindings.h>
#include <N_Binding.h>
/******************************************************************************
                    Defines section
******************************************************************************/
#define COMPID "N_Zdp"

#ifndef N_ZDP_MAX_CLIENT_SUBSCRIBERS
  #define N_ZDP_MAX_CLIENT_SUBSCRIBERS 10u
#endif /* N_ZDP_MAX_CLIENT_SUBSCRIBERS */

#ifndef N_ZDP_MAX_SERVER_SUBSCRIBERS
  #define N_ZDP_MAX_SERVER_SUBSCRIBERS 2u
#endif /* N_ZDP_MAX_SERVER_SUBSCRIBERS */

#define N_ZDP_MAX_CLUSTERS_LIST_SIZE 10u
#define N_ZDP_MAX_ENDPOINTS_LIST_SIZE 10u

/******************************************************************************
                    Types section
******************************************************************************/
typedef struct _N_Zdp_ZdpReq_t
{
  ZDO_ZdpReq_t        zdpReq;
  bool                busy;
} N_Zdp_ZdpReq_t;

/******************************************************************************
                    Prototypes section
******************************************************************************/
static ZDO_ZdpReq_t *getFreeZdpReq(void);
static void freeZdpReq(ZDO_ZdpResp_t *resp);
static void zdpIeeeAddrResp(ZDO_ZdpResp_t* zdpResp);
static void zdpNodeDescResp(ZDO_ZdpResp_t* zdpResp);
static void zdpSimpleDescResp(ZDO_ZdpResp_t* zdpResp);
static void zdpActiveEndPointResp(ZDO_ZdpResp_t* zdpResp);
static void zdpMatchDescResp(ZDO_ZdpResp_t* zdpResp);
static void zdpPermitJoiningResp(ZDO_ZdpResp_t *zdpResp);
static void nZdpObserver(SYS_EventId_t eventId, SYS_EventData_t data);
static void fillAddressStructure(APS_DataInd_t *ind);
static void fillDeviceAnnounceStructure(uint8_t *req);
static void zdpNwkAddrResp(ZDO_ZdpResp_t* zdpResp);
static bool zdpIsZigBeeStandardStatus(ZDO_Status_t bcStatus);
static void zdpBindResp(ZDO_ZdpResp_t* zdpResp);
static void zdpUnbindResp(ZDO_ZdpResp_t* zdpResp);
static void zdpPrepareBindUnbindReq(ZDO_ZdpReq_t *zdpReq, N_Address_t* pBindSource,
  uint16_t clusterId, N_Address_t* pBindDestination, N_Address_t* pDestinationAddress);
static void zdpMgmtBindResp(ZDO_ZdpResp_t* zdpResp);
static void zdpMgmtLqiResp(ZDO_ZdpResp_t* zdpResp);
static void zdpMgmtRtgResp(ZDO_ZdpResp_t* zdpResp);
static void zdpMgmtLeaveResp(ZDO_ZdpResp_t* zdpResp);

/******************************************************************************
                    Static variables section
******************************************************************************/
static N_Zdp_ZdpReq_t zdpReqPool[N_ZDP_REQUESTS_POOL_SIZE];
N_UTIL_CALLBACK_DECLARE(N_Zdp_ClientCallback_t, s_N_ZdpClient_Subscribers, N_ZDP_MAX_CLIENT_SUBSCRIBERS);
N_UTIL_CALLBACK_DECLARE(N_Zdp_ServerCallback_t, s_N_ZdpServer_Subscribers, N_ZDP_MAX_SERVER_SUBSCRIBERS);
static SYS_EventReceiver_t nZdpEventReceiver = {.func = nZdpObserver};
static N_Address_t address;
static N_Zdp_DeviceAnnounce_t deviceAnnounce;

static union
{
  struct PACK
  {
    N_Zdp_SimpleDescRsp_t         rsp;
    ClusterId_t                   inClusterList[N_ZDP_MAX_CLUSTERS_LIST_SIZE - 1U];
    N_Zdp_SimpleDescRsp_OutList_t outList;
    ClusterId_t                   outClusterList[N_ZDP_MAX_CLUSTERS_LIST_SIZE - 1U];
  } simpleDescRsp;

  struct PACK
  {
    N_Zdp_ActiveEndPointRsp_t rsp;
    Endpoint_t                endpointList[N_ZDP_MAX_ENDPOINTS_LIST_SIZE];
  } activeEndPointRsp;

  struct PACK
  {
    N_Zdp_MatchDescRsp_t  rsp;
    Endpoint_t            matchList[N_ZDP_MAX_ENDPOINTS_LIST_SIZE];
  } matchDescRsp;

  struct
  {
    N_Zdp_MgmtBindRsp_t rsp;
    N_Zdp_BindingTableEntry_t list[ZDP_BINDING_TABLE_LIST_SIZE];
  } mgmtBind;

  struct PACK
  {
    N_Zdp_MgmtLqiRsp_t rsp;
    N_Zdp_NeighborTableRecord_t list[ZDP_MAX_NEIGHBOR_TABLE_LIST_COUNT - 1U];
  } mgmtLqi;

  struct PACK
  {
    N_Zdp_MgmtRtgRsp_t rsp;
    N_Zdp_RoutingTableRecord_t list[ZDP_ROUTING_TABLE_LIST_SIZE - 1U];
  } mgmtRtg;

  /* Subscript for the bytesArray has been changed to avoid GCC compilation warning 
  in order to access the member of the simpleDescRsp (union)*/
  uint8_t bytesArray[sizeof(N_Zdp_SimpleDescRsp_t) + (sizeof(ClusterId_t) * N_ZDP_MAX_CLUSTERS_LIST_SIZE)];
} rspExtraMemory;

/***********************************************************************************
                    Implementation section
***********************************************************************************/
/** Initialises the component
*/
void N_Zdp_Init(void)
{
  memset(zdpReqPool, 0x00, sizeof(zdpReqPool));

  SYS_SubscribeToEvent(BC_ZDP_REQUEST_RECEIVED, &nZdpEventReceiver);
  SYS_SubscribeToEvent(BC_ZDP_RESPONSE_RECEIVED, &nZdpEventReceiver);
  SYS_SubscribeToEvent(BC_ZDP_DEVICE_ANNOUNCE_RECEIVED, &nZdpEventReceiver);
  SYS_SubscribeToEvent(BC_ZDP_JOIN_PERMITTED_CHANGED, &nZdpEventReceiver);
}

/** Subscribes for N_Zdp responses.
    \param pSubscriber The subscriber
*/
void N_Zdp_ClientSubscribe_Impl(const N_Zdp_ClientCallback_t* pSubscriber)
{
  N_UTIL_CALLBACK_SUBSCRIBE(N_Zdp_ClientCallback_t, s_N_ZdpClient_Subscribers, pSubscriber);
}

/** Subscribes for N_Zdp responses.
    \param pSubscriber The subscriber
*/
void N_Zdp_ServerSubscribe_Impl(const N_Zdp_ServerCallback_t* pSubscriber)
{
  N_UTIL_CALLBACK_SUBSCRIBE(N_Zdp_ServerCallback_t, s_N_ZdpServer_Subscribers, pSubscriber);
}

/** Get the source address of the received ZDP message.
    \returns The source address

    \note Only use from a ZDP callback function, otherwise the result is undefined.
*/
N_Address_t* N_Zdp_GetSourceAddress_Impl(void)
{
  return &address;
}

/** Send NWK_addr_req, see ZigBee 2007 specification 2.4.3.1.1.
    \param pIeeeAddress IEEE address to request
    \param pDestinationAddress The address to send the command to
    \returns The sequence number of the sent command, this can be used to match a received response with the request

    \note Only the single request type is supported.
*/
uint8_t N_Zdp_SendNwkAddrReq_Impl(N_Address_Extended_t* pIeeeAddress, N_Address_t* pDestinationAddress)
{
  ZDO_ZdpReq_t *zdpReq = getFreeZdpReq();

  if (zdpReq)
  {
    ZDO_NwkAddrReq_t *nwkAddrReq = &zdpReq->req.reqPayload.nwkAddrReq;
    ExtAddr_t extAddr;

    zdpReq->dstAddrMode = N_Cmi_PlatformToBcAddressModeConvert(pDestinationAddress->addrMode);
    N_Cmi_PlatformToBcAddressingConvert(&zdpReq->dstAddress, pDestinationAddress);
    zdpReq->reqCluster = NWK_ADDR_CLID;
    zdpReq->ZDO_ZdpResp = zdpNwkAddrResp;

    memcpy(&extAddr, pIeeeAddress, sizeof(ExtAddr_t));
    nwkAddrReq->ieeeAddrOfInterest = extAddr;
    nwkAddrReq->reqType = SINGLE_RESPONSE_REQUESTTYPE;
    nwkAddrReq->startIndex = 0;

    N_Zdp_ZdpRequest(zdpReq);

    return zdpReq->service.seqNumCopy;
  }
  /* What should be returned here? */
  return 0;
}

/** ZDP NwkAddrResponse handler
    \param resp ZDP response parameters
*/
static void zdpNwkAddrResp(ZDO_ZdpResp_t* zdpResp)
{
  if (zdpIsZigBeeStandardStatus(zdpResp->respPayload.status))
  {
    N_Zdp_AddrRsp_t nwkAddrResp;
    ZDO_ZdpReq_t *zdpReq = GET_PARENT_BY_FIELD(ZDO_ZdpReq_t, resp, zdpResp);
    ExtAddr_t extAddr;

    nwkAddrResp.sequenceNumber = zdpReq->service.seqNumCopy;
    nwkAddrResp.status = zdpResp->respPayload.status;
    nwkAddrResp.nwkAddrRemoteDevice = zdpResp->respPayload.nwkAddrResp.nwkAddrRemote;
    extAddr = zdpResp->respPayload.nwkAddrResp.ieeeAddrRemote;
    memcpy(nwkAddrResp.ieeeAddrRemoteDevice, &extAddr,
      sizeof(ExtAddr_t));

    N_UTIL_CALLBACK(N_Zdp_ClientCallback_t, s_N_ZdpClient_Subscribers, ReceivedNwkAddrRsp, (&nwkAddrResp));
  }

  freeZdpReq(zdpResp);
}

/** Send IEEE_addr_req, see ZigBee 2007 specification 2.4.3.1.2.
    \param nwkAddressOfInterest network address to request
    \param pDestinationAddress The address to send the command to
    \returns The sequence number of the sent command, this can be used to match a received response with the request

    \note Only the single request type is supported.
*/
uint8_t N_Zdp_SendIeeeAddrReq_Impl(uint16_t nwkAddressOfInterest, N_Address_t* pDestinationAddress)
{
  ZDO_ZdpReq_t *zdpReq = getFreeZdpReq();

  if (zdpReq)
  {
    ZDO_IeeeAddrReq_t *ieeeAddrReq = &zdpReq->req.reqPayload.ieeeAddrReq;

    zdpReq->dstAddrMode = N_Cmi_PlatformToBcAddressModeConvert(pDestinationAddress->addrMode);
    N_Cmi_PlatformToBcAddressingConvert(&zdpReq->dstAddress, pDestinationAddress);
    zdpReq->reqCluster = IEEE_ADDR_CLID;
    zdpReq->ZDO_ZdpResp = zdpIeeeAddrResp;

    ieeeAddrReq->nwkAddrOfInterest = nwkAddressOfInterest;
    ieeeAddrReq->reqType = SINGLE_RESPONSE_REQUESTTYPE;
    ieeeAddrReq->startIndex = 0;

    N_Zdp_ZdpRequest(zdpReq);

    return zdpReq->service.seqNumCopy;
  }
  /* What should be returned here? */
  return 0;
}

/** ZDP IEEEAddrResponse handler
    \param resp ZDP response parameters
*/
static void zdpIeeeAddrResp(ZDO_ZdpResp_t* zdpResp)
{
  if (zdpIsZigBeeStandardStatus(zdpResp->respPayload.status))
  {
    N_Zdp_AddrRsp_t ieeeAddrResp;
    ZDO_ZdpReq_t *zdpReq = GET_PARENT_BY_FIELD(ZDO_ZdpReq_t, resp, zdpResp);
    ExtAddr_t extAddr;

    ieeeAddrResp.sequenceNumber = zdpReq->service.seqNumCopy;
    ieeeAddrResp.status = zdpResp->respPayload.status;
    ieeeAddrResp.nwkAddrRemoteDevice = zdpResp->respPayload.ieeeAddrResp.nwkAddrRemote;
    extAddr = zdpResp->respPayload.ieeeAddrResp.ieeeAddrRemote;
    memcpy(ieeeAddrResp.ieeeAddrRemoteDevice, &extAddr, sizeof(ExtAddr_t));

    N_UTIL_CALLBACK(N_Zdp_ClientCallback_t, s_N_ZdpClient_Subscribers, ReceivedIeeeAddrRsp, (&ieeeAddrResp));
  }

  freeZdpReq(zdpResp);
}

/** Send Node_Desc_Req, see ZigBee 2007 specification 2.4.3.1.3.
    \param nwkAddrOfInterest
    \param pDestinationAddress Destination address of message
    \returns The sequence number of the sent command, this can be used to match a received response with the request
*/
uint8_t N_Zdp_SendNodeDescReq_Impl(uint16_t nwkAddrOfInterest, N_Address_t* pDestinationAddress)
{
  ZDO_ZdpReq_t *zdpReq = getFreeZdpReq();

  if (zdpReq)
  {
    ZDO_NodeDescReq_t *nodeDescReq = &zdpReq->req.reqPayload.nodeDescReq;

    zdpReq->dstAddrMode = N_Cmi_PlatformToBcAddressModeConvert(pDestinationAddress->addrMode);
    N_Cmi_PlatformToBcAddressingConvert(&zdpReq->dstAddress, pDestinationAddress);
    zdpReq->reqCluster = NODE_DESCRIPTOR_CLID;
    zdpReq->ZDO_ZdpResp = zdpNodeDescResp;
    nodeDescReq->nwkAddrOfInterest = nwkAddrOfInterest;

    N_Zdp_ZdpRequest(zdpReq);

    return zdpReq->service.seqNumCopy;
  }
  /* What should be returned here? */
  return 0;
}

/** ZDP NodeDecsResponse handler
    \param resp ZDP response parameters
*/
static void zdpNodeDescResp(ZDO_ZdpResp_t* zdpResp)
{
  if (zdpIsZigBeeStandardStatus(zdpResp->respPayload.status))
  {
    N_Zdp_NodeDescRsp_t nodeDescResp;
    ZDO_ZdpReq_t *zdpReq = GET_PARENT_BY_FIELD(ZDO_ZdpReq_t, resp, zdpResp);

    nodeDescResp.sequenceNumber = zdpReq->service.seqNumCopy;
    nodeDescResp.status = zdpResp->respPayload.status;
    nodeDescResp.nwkAddrOfInterest = zdpResp->respPayload.nodeDescResp.nwkAddrOfInterest;

    if (N_ZDP_STATUS_SUCCESS == nodeDescResp.status)
    {
      nodeDescResp.nodeDescriptor.logicalType = zdpResp->respPayload.nodeDescResp.nodeDescriptor.logicalType;
      nodeDescResp.nodeDescriptor.complexDescriptorAvailable = zdpResp->respPayload.nodeDescResp.nodeDescriptor.complexDescriptorAvailable;
      nodeDescResp.nodeDescriptor.userDescriptorAvailable = zdpResp->respPayload.nodeDescResp.nodeDescriptor.userDescriptorAvailable;
      nodeDescResp.nodeDescriptor.apsFlags = zdpResp->respPayload.nodeDescResp.nodeDescriptor.apsFlags;
      nodeDescResp.nodeDescriptor.frequencyBand = zdpResp->respPayload.nodeDescResp.nodeDescriptor.frequencyBand;
      memcpy(&nodeDescResp.macCapabilityFlags, &zdpResp->respPayload.nodeDescResp.nodeDescriptor.macCapbFlags, sizeof(N_Zdp_MacCapability_t));
      nodeDescResp.manufacturerCode = zdpResp->respPayload.nodeDescResp.nodeDescriptor.manufacturerCode;
      nodeDescResp.maximumBufferSize = zdpResp->respPayload.nodeDescResp.nodeDescriptor.maxBufferSize;
      nodeDescResp.maxmimumIncomingTransferSize = zdpResp->respPayload.nodeDescResp.nodeDescriptor.maxTransferSize;
      nodeDescResp.serverMask = zdpResp->respPayload.nodeDescResp.nodeDescriptor.serverMask;
      nodeDescResp.maximumOutgoingTransferSize = zdpResp->respPayload.nodeDescResp.nodeDescriptor.maxOutcTransferSize;
      nodeDescResp.descriptorCapabilityField = zdpResp->respPayload.nodeDescResp.nodeDescriptor.descriptorCapabilityField;
    }

    N_UTIL_CALLBACK(N_Zdp_ClientCallback_t, s_N_ZdpClient_Subscribers, ReceivedNodeDescRsp, (&nodeDescResp));
  }

  freeZdpReq(zdpResp);
}

/** Send Simple_Desc_req, see ZigBee 2007 specification 2.4.3.1.5.
    \param nwkAddrOfInterest
    \param endPoint
    \param pDestinationAddress Destination address of message
    \returns The sequence number of the sent command, this can be used to match a received response with the request
*/
uint8_t N_Zdp_SendSimpleDescReq_Impl(uint16_t nwkAddrOfInterest, uint8_t endPoint, N_Address_t *pDestinationAddress)
{
  ZDO_ZdpReq_t *zdpReq = getFreeZdpReq();

  if (zdpReq)
  {
    ZDO_SimpleDescReq_t *simpleDescReq = &zdpReq->req.reqPayload.simpleDescReq;

    zdpReq->dstAddrMode = N_Cmi_PlatformToBcAddressModeConvert(pDestinationAddress->addrMode);
    N_Cmi_PlatformToBcAddressingConvert(&zdpReq->dstAddress, pDestinationAddress);
    zdpReq->reqCluster = SIMPLE_DESCRIPTOR_CLID;
    zdpReq->ZDO_ZdpResp = zdpSimpleDescResp;

    simpleDescReq->nwkAddrOfInterest = nwkAddrOfInterest;
    simpleDescReq->endpoint = endPoint;

    N_Zdp_ZdpRequest(zdpReq);

    return zdpReq->service.seqNumCopy;
  }

  /* What should be returned here? */
  return 0;
}

/** ZDP SimpleDescriptorResponse handler
    \param resp ZDP response parameters
*/
static void zdpSimpleDescResp(ZDO_ZdpResp_t* zdpResp)
{
  if (zdpIsZigBeeStandardStatus(zdpResp->respPayload.status))
  {
    N_Zdp_SimpleDescRsp_t *simpleDescRsp = &rspExtraMemory.simpleDescRsp.rsp;
    memset(simpleDescRsp, 0, sizeof(N_Zdp_SimpleDescRsp_t));
    N_Zdp_SimpleDescRsp_OutList_t *simpleDescOutList = &rspExtraMemory.simpleDescRsp.outList;
    memset(simpleDescOutList, 0, sizeof(N_Zdp_SimpleDescRsp_OutList_t));
    ZDO_ZdpReq_t *zdpReq = GET_PARENT_BY_FIELD(ZDO_ZdpReq_t, resp, zdpResp);
    ZDO_SimpleDescResp_t *simpleDescResp = &zdpResp->respPayload.simpleDescResp;

    simpleDescRsp->sequenceNumber = zdpReq->service.seqNumCopy;
    simpleDescRsp->status = zdpResp->respPayload.status;
    simpleDescRsp->nwkAddrOfInterest = simpleDescResp->nwkAddrOfInterest;

    if (N_ZDP_STATUS_SUCCESS == simpleDescRsp->status)
    {
      uint8_t clustersAmount;

      simpleDescRsp->length = simpleDescResp->length;
      simpleDescRsp->endpoint = simpleDescResp->simpleDescriptor.endpoint;
      simpleDescRsp->appProfileId = simpleDescResp->simpleDescriptor.AppProfileId;
      simpleDescRsp->appDeviceId = simpleDescResp->simpleDescriptor.AppDeviceId;
      rspExtraMemory.simpleDescRsp.rsp.appDeviceVersion = simpleDescResp->simpleDescriptor.AppDeviceVersion;
      rspExtraMemory.simpleDescRsp.rsp.reserved = 0;

      /* In clusters list */
      clustersAmount = MIN(simpleDescResp->simpleDescriptor.AppInClustersCount,
                           N_ZDP_MAX_CLUSTERS_LIST_SIZE);

      simpleDescRsp->appNumInClusters = clustersAmount;
      for (uint8_t i = 0; i < clustersAmount; i++)
        simpleDescRsp->pAppInClusterList[i] = simpleDescResp->simpleDescriptor.AppInClustersList[i];

      /* Out clusters list */
      clustersAmount = MIN(simpleDescResp->simpleDescriptor.AppOutClustersCount,
                           N_ZDP_MAX_CLUSTERS_LIST_SIZE);

      simpleDescOutList->appNumOutClusters = clustersAmount;
      for (uint8_t i = 0; i < clustersAmount; i++)
        simpleDescOutList->pAppOutClusterList[i] = simpleDescResp->simpleDescriptor.AppOutClustersList[i];

      {
        uint8_t *to = &rspExtraMemory.bytesArray[sizeof(N_Zdp_SimpleDescRsp_t) + (simpleDescRsp->appNumInClusters - 1) * sizeof(ClusterId_t)];
        uint8_t *from = &rspExtraMemory.bytesArray[sizeof(rspExtraMemory.simpleDescRsp.rsp) + sizeof(rspExtraMemory.simpleDescRsp.inClusterList)];
        memmove(to, from, sizeof(uint8_t) + simpleDescOutList->appNumOutClusters * sizeof(ClusterId_t));
        
        /* Pack the response to get monolite structure. */
        /*uint8_t *outListDesiredPosition = (uint8_t *)simpleDescRsp->pAppInClusterList +
                                          simpleDescRsp->appNumInClusters * sizeof(ClusterId_t);
        memmove(outListDesiredPosition,
                (uint8_t *)&rspExtraMemory.simpleDescRsp.outList,
                sizeof(uint8_t) + simpleDescOutList->appNumOutClusters * sizeof(ClusterId_t));*/
      }
    }
    else
      simpleDescRsp->length = 0U;

    N_UTIL_CALLBACK(N_Zdp_ClientCallback_t, s_N_ZdpClient_Subscribers, ReceivedSimpleDescRsp, (simpleDescRsp));
  }

  freeZdpReq(zdpResp);
}

/** Send Active_EP_req, see ZigBee 2007 specification 2.4.3.1.6.
    \param nwkAddrOfInterest
    \param pDestinationAddress Destination address of message
    \returns The sequence number of the sent command, this can be used to match a received response with the request
*/
uint8_t N_Zdp_SendActiveEndPointReq_Impl(uint16_t nwkAddrOfInterest, N_Address_t *pDestinationAddress)
{
  ZDO_ZdpReq_t *zdpReq = getFreeZdpReq();

  if (zdpReq)
  {
    ZDO_ActiveEPReq_t *activeEndPointReq = &zdpReq->req.reqPayload.activeEPReq;

    zdpReq->dstAddrMode = N_Cmi_PlatformToBcAddressModeConvert(pDestinationAddress->addrMode);
    N_Cmi_PlatformToBcAddressingConvert(&zdpReq->dstAddress, pDestinationAddress);
    zdpReq->reqCluster = ACTIVE_ENDPOINTS_CLID;
    zdpReq->ZDO_ZdpResp = zdpActiveEndPointResp;

    activeEndPointReq->nwkAddrOfInterest = nwkAddrOfInterest;

    N_Zdp_ZdpRequest(zdpReq);

    return zdpReq->service.seqNumCopy;
  }
  /* What should be returned here? */
  return 0;
}

/** ZDP ActiveEndPointResponse handler
    \param resp ZDP response parameters
*/
static void zdpActiveEndPointResp(ZDO_ZdpResp_t* zdpResp)
{
  if (zdpIsZigBeeStandardStatus(zdpResp->respPayload.status))
  {
    ZDO_ActiveEPResp_t *activeEPResp = &zdpResp->respPayload.activeEPResp;
    N_Zdp_ActiveEndPointRsp_t *activeEndPointRsp = &rspExtraMemory.activeEndPointRsp.rsp;
    memset(activeEndPointRsp, 0, sizeof(N_Zdp_ActiveEndPointRsp_t));
    ZDO_ZdpReq_t *zdpReq = GET_PARENT_BY_FIELD(ZDO_ZdpReq_t, resp, zdpResp);

    activeEndPointRsp->status = zdpResp->respPayload.status;
    activeEndPointRsp->sequenceNumber = zdpReq->service.seqNumCopy;
    activeEndPointRsp->nwkAddrOfInterest = activeEPResp->nwkAddrOfInterest;
    activeEndPointRsp->ActiveEPList = rspExtraMemory.activeEndPointRsp.endpointList;

    if (N_ZDP_STATUS_SUCCESS == activeEndPointRsp->status)
    {
      uint8_t endpointsInList;

      endpointsInList = (activeEPResp->activeEPCount < N_ZDP_MAX_ENDPOINTS_LIST_SIZE) ?
        activeEPResp->activeEPCount : N_ZDP_MAX_ENDPOINTS_LIST_SIZE;

      activeEndPointRsp->activeEPCount = endpointsInList;
      memcpy(activeEndPointRsp->ActiveEPList, activeEPResp->activeEPList, endpointsInList);
    }
    else
      activeEndPointRsp->activeEPCount = 0U;

    N_UTIL_CALLBACK(N_Zdp_ClientCallback_t, s_N_ZdpClient_Subscribers, ReceivedActiveEndPointRsp, (activeEndPointRsp));
  }

  freeZdpReq(zdpResp);
}

/** Send a Match_Desc_req command, see ZigBee 2007 Specification  2.4.3.1.7.
    \param nwkAddrOfInterest Network (Short) address to be used for matching
    \param profileId Profile ID to be used for matching
    \param inClusterId The Input ClusterID to be used for matching
    \param pDestinationAddress Destination address for the request.
    \returns The sequence number of the sent command, this can be used to match a received response with the request

    \note This Implementation only allows a match with a single input cluster and no output cluster.
*/
uint8_t N_Zdp_SendMatchDescReq_Impl(uint16_t nwkAddrOfInterest, uint16_t profileId, uint16_t inClusterId, N_Address_t* pDestinationAddress)
{
  ZDO_ZdpReq_t *zdpReq = getFreeZdpReq();

  if (zdpReq)
  {
    ZDO_MatchDescReq_t *matchDescReq = &zdpReq->req.reqPayload.matchDescReq;

    zdpReq->dstAddrMode = N_Cmi_PlatformToBcAddressModeConvert(pDestinationAddress->addrMode);
    N_Cmi_PlatformToBcAddressingConvert(&zdpReq->dstAddress, pDestinationAddress);
    zdpReq->reqCluster = MATCH_DESCRIPTOR_CLID;
    zdpReq->ZDO_ZdpResp = zdpMatchDescResp;

    matchDescReq->nwkAddrOfInterest = nwkAddrOfInterest;
    matchDescReq->profileId = profileId;
    matchDescReq->numInClusters = 1;
    matchDescReq->inClusterList[0] = inClusterId;
    matchDescReq->numOutClusters = 0;

    N_Zdp_ZdpRequest(zdpReq);

    return zdpReq->service.seqNumCopy;
  }

  /* What should be returned here? */
  return 0;
}

/** ZDP MatchDescriptorResponse handler
    \param resp ZDP response parameters
*/
static void zdpMatchDescResp(ZDO_ZdpResp_t* zdpResp)
{
  const ZDO_Status_t status = zdpResp->respPayload.status;
  N_Zdp_MatchDescRsp_t *const matchDescRsp = &rspExtraMemory.matchDescRsp.rsp;
  memset(matchDescRsp, 0, sizeof(N_Zdp_MatchDescRsp_t));
  const ZDO_MatchDescResp_t *const matchDescResp = &zdpResp->respPayload.matchDescResp;
  const ZDO_ZdpReq_t *const zdpReq = GET_PARENT_BY_FIELD(ZDO_ZdpReq_t, resp, zdpResp);

  if (ZDO_CMD_COMPLETED_STATUS == status || !zdpIsZigBeeStandardStatus(status))
  {
    freeZdpReq(zdpResp);
    return;
  }

  matchDescRsp->sequenceNumber = zdpReq->service.seqNumCopy;
  matchDescRsp->status = status;
  matchDescRsp->nwkAddrOfInterest = matchDescResp->nwkAddrOfInterest;
  matchDescRsp->matchList = rspExtraMemory.matchDescRsp.matchList;

  if (N_ZDP_STATUS_SUCCESS == status)
  {
    uint8_t endpointsInList;

    endpointsInList = (matchDescResp->matchLength < N_ZDP_MAX_ENDPOINTS_LIST_SIZE) ?
      matchDescResp->matchLength : N_ZDP_MAX_ENDPOINTS_LIST_SIZE;

    matchDescRsp->matchLength = endpointsInList;
    memcpy(matchDescRsp->matchList, matchDescResp->matchList, endpointsInList);
  }
  else
    matchDescRsp->matchLength = 0U;

  N_UTIL_CALLBACK(N_Zdp_ClientCallback_t, s_N_ZdpClient_Subscribers, ReceivedMatchDescRsp, (matchDescRsp));
}

/** Prepare Bind_req and Unbind_req.
    \param zdpReq - pointer to allocated ZDP request.
    \param pBindSource Binding source IEEE address and endpoint. Address type must be afAddr64Bit.
    \param clusterId Cluster ID to bind.
    \param pBindDestination Binding destination address. Address type must be afAddr64Bit or afAddrGroup.
    \param pDestinationAddress Destination address of message
*/
static void zdpPrepareBindUnbindReq(ZDO_ZdpReq_t *zdpReq, N_Address_t* pBindSource,
  uint16_t clusterId, N_Address_t* pBindDestination, N_Address_t* pDestinationAddress)
{
  ZDO_BindReq_t *bindReq = &zdpReq->req.reqPayload.bindReq;
  ExtAddr_t extAddr;

  zdpReq->dstAddrMode = N_Cmi_PlatformToBcAddressModeConvert(pDestinationAddress->addrMode);
  N_Cmi_PlatformToBcAddressingConvert(&zdpReq->dstAddress, pDestinationAddress);

  N_ERRH_ASSERT_FATAL( N_Address_Mode_Extended == pBindSource->addrMode );
  memcpy(&extAddr, pBindSource->address.extendedAddress, sizeof(bindReq->srcAddr));
  bindReq->srcAddr = extAddr;
  bindReq->srcEndpoint = pBindSource->endPoint;
  bindReq->clusterId = clusterId;
  bindReq->dstAddrMode = N_Cmi_PlatformToBcAddressModeConvert(pBindDestination->addrMode);
  if (APS_EXT_ADDRESS == bindReq->dstAddrMode)
  {
    memcpy(&extAddr, pBindDestination->address.extendedAddress, sizeof(bindReq->dstExtAddr));
    bindReq->dstExtAddr = extAddr;
    bindReq->dstEndpoint = pBindDestination->endPoint;
  }
  else
  {
    N_ERRH_ASSERT_FATAL( APS_GROUP_ADDRESS == bindReq->dstAddrMode );
    bindReq->dstGroupAddr = pBindDestination->address.shortAddress;
  }
}

/** Send Bind_req.
    \param pBindSource Binding source IEEE address and endpoint. Address type must be afAddr64Bit.
    \param clusterId Cluster ID to bind.
    \param pBindDestination Binding destination address. Address type must be afAddr64Bit or afAddrGroup.
    \param pDestinationAddress Destination address of message
    \returns The sequence number of the sent command, this can be used to match a received response with the request
*/
uint8_t N_Zdp_SendBindReq_Impl(N_Address_t* pBindSource, uint16_t clusterId, N_Address_t* pBindDestination, N_Address_t* pDestinationAddress)
{
    ZDO_ZdpReq_t *zdpReq = getFreeZdpReq();

    if (NULL == zdpReq)
      return 0;

    zdpPrepareBindUnbindReq(zdpReq, pBindSource, clusterId, pBindDestination, pDestinationAddress);
    zdpReq->reqCluster = BIND_CLID;
    zdpReq->ZDO_ZdpResp = zdpBindResp;

    N_Zdp_ZdpRequest(zdpReq);

    return zdpReq->service.seqNumCopy;
}

/** Transmission of ZDP Bind Request is done
    \param resp ZDP response parameters
*/
static void zdpBindResp(ZDO_ZdpResp_t* zdpResp)
{
  if (zdpIsZigBeeStandardStatus(zdpResp->respPayload.status))
  {
    N_Zdp_StatusRsp_t response;
    response.status = zdpResp->respPayload.status;
    response.sequenceNumber = zdpResp->respPayload.seqNum;

    N_UTIL_CALLBACK(N_Zdp_ClientCallback_t, s_N_ZdpClient_Subscribers, ReceivedBindRsp, (&response));
  }

  freeZdpReq(zdpResp);
}

/** Send Unbind_req.
    \param pBindSource Binding source IEEE address and endpoint. Address type must be afAddr64Bit
    \param clusterId Cluster ID to unbind
    \param pBindDestination Binding destination address. Address type must be afAddr64Bit or afAddrGroup
    \param pDestinationAddress Destination address of message
    \returns The sequence number of the sent command, this can be used to match a received response with the request
*/
uint8_t N_Zdp_SendUnbindReq_Impl(N_Address_t* pBindSource, uint16_t clusterId, N_Address_t* pBindDestination, N_Address_t* pDestinationAddress)
{
  ZDO_ZdpReq_t *zdpReq = getFreeZdpReq();

  if (NULL == zdpReq)
    return 0;

  zdpPrepareBindUnbindReq(zdpReq, pBindSource, clusterId, pBindDestination, pDestinationAddress);
  zdpReq->reqCluster = UNBIND_CLID;
  zdpReq->ZDO_ZdpResp = zdpUnbindResp;

  N_Zdp_ZdpRequest(zdpReq);

  return zdpReq->service.seqNumCopy;
}

/** Transmission of ZDP Unbind Request is done
    \param resp ZDP response parameters
*/
static void zdpUnbindResp(ZDO_ZdpResp_t* zdpResp)
{
  if (zdpIsZigBeeStandardStatus(zdpResp->respPayload.status))
  {
    N_Zdp_StatusRsp_t response;
    response.status = zdpResp->respPayload.status;
    response.sequenceNumber = zdpResp->respPayload.seqNum;

    N_UTIL_CALLBACK(N_Zdp_ClientCallback_t, s_N_ZdpClient_Subscribers, ReceivedUnbindRsp, (&response));
  }

  freeZdpReq(zdpResp);
}

/** Send Mgmt_Bind_req, see ZigBee 2007 specification 2.4.3.3.4.
    \param startIndex
    \param pDestinationAddress Destination address of message
    \returns The sequence number of the sent command, this can be used to match a received response with the request
*/
uint8_t N_Zdp_SendMgmtBindReq_Impl(uint8_t startIndex, N_Address_t* pDestinationAddress)
{

    ZDO_ZdpReq_t *zdpReq = getFreeZdpReq();

    if (NULL == zdpReq)
      return 0;

    zdpReq->dstAddrMode = N_Cmi_PlatformToBcAddressModeConvert(pDestinationAddress->addrMode);
    N_Cmi_PlatformToBcAddressingConvert(&zdpReq->dstAddress, pDestinationAddress);
    zdpReq->reqCluster = MGMT_BIND_CLID;
    zdpReq->ZDO_ZdpResp = zdpMgmtBindResp;
    zdpReq->req.reqPayload.mgmtBindReq.startIndex = startIndex;

    N_Zdp_ZdpRequest(zdpReq);

    return zdpReq->service.seqNumCopy;
}

/** Transmission of ZDP MgmtBind Request is done
    \param resp ZDP response parameters
*/
static void zdpMgmtBindResp(ZDO_ZdpResp_t* zdpResp)
{
  if (zdpIsZigBeeStandardStatus(zdpResp->respPayload.status))
  {
    N_Zdp_MgmtBindRsp_t *dstRsp = &rspExtraMemory.mgmtBind.rsp;
    memset(dstRsp, 0, sizeof(N_Zdp_MgmtBindRsp_t));
    ZDO_MgmtBindResp_t *srcRsp = &zdpResp->respPayload.mgmtBindResp;
    uint8_t i;
    ExtAddr_t extAddr;

    dstRsp->sequenceNumber = zdpResp->respPayload.seqNum;
    dstRsp->status = zdpResp->respPayload.status;

    if (dstRsp->status == N_ZDP_STATUS_SUCCESS)
    {
      dstRsp->bindingTableEntries = srcRsp->bindingTableEntries;
      dstRsp->startIndex = srcRsp->startIndex;
      N_ERRH_ASSERT_FATAL( srcRsp->bindingTableListCount <= ZDP_BINDING_TABLE_LIST_SIZE );
      dstRsp->bindingTableListCount = srcRsp->bindingTableListCount;
      dstRsp->pBindingTableList = rspExtraMemory.mgmtBind.list;
      for (i = 0U; i < dstRsp->bindingTableListCount; ++i)
      {
        dstRsp->pBindingTableList[i].clusterId = srcRsp->bindingTableList[i].clusterId;
        dstRsp->pBindingTableList[i].bindSource.endPoint = srcRsp->bindingTableList[i].srcEndpoint;
        dstRsp->pBindingTableList[i].bindSource.panId = 0U;
        dstRsp->pBindingTableList[i].bindSource.addrMode = N_Address_Mode_Extended;
        extAddr = srcRsp->bindingTableList[i].srcAddr;
        memcpy(dstRsp->pBindingTableList[i].bindSource.address.extendedAddress,
               &extAddr, sizeof(ExtAddr_t));

        dstRsp->pBindingTableList[i].bindDestination.panId = 0U;
        if (APS_EXT_ADDRESS == srcRsp->bindingTableList[i].dstAddrMode)
        {
          dstRsp->pBindingTableList[i].bindDestination.addrMode = N_Address_Mode_Extended;
          extAddr = srcRsp->bindingTableList[i].dstExtAddr;
          memcpy(dstRsp->pBindingTableList[i].bindDestination.address.extendedAddress,
                 &extAddr, sizeof(ExtAddr_t));
          dstRsp->pBindingTableList[i].bindDestination.endPoint = srcRsp->bindingTableList[i].dstEndpoint;
        }
        else
        {
          dstRsp->pBindingTableList[i].bindDestination.addrMode = N_Address_Mode_Group;
          dstRsp->pBindingTableList[i].bindDestination.address.shortAddress = srcRsp->bindingTableList[i].dstGroupAddr;
        }
      }
    }

    N_UTIL_CALLBACK(N_Zdp_ClientCallback_t, s_N_ZdpClient_Subscribers, ReceivedMgmtBindRsp, (dstRsp));
  }

  freeZdpReq(zdpResp);
}

/** Send Mgmt_Lqi_req, see ZigBee 2007 specification 2.4.3.3.2.
    \param startIndex
    \param pDestinationAddress Destination address of message
    \returns The sequence number of the sent command, this can be used to match a received response with the request
*/
uint8_t N_Zdp_SendMgmtLqiReq_Impl(uint8_t startIndex, N_Address_t *pDestinationAddress)
{
  ZDO_ZdpReq_t *zdpReq = getFreeZdpReq();

  if (NULL == zdpReq)
    return 0;

  zdpReq->dstAddrMode = N_Cmi_PlatformToBcAddressModeConvert(pDestinationAddress->addrMode);
  N_Cmi_PlatformToBcAddressingConvert(&zdpReq->dstAddress, pDestinationAddress);
  zdpReq->reqCluster = MGMT_LQI_CLID;
  zdpReq->ZDO_ZdpResp = zdpMgmtLqiResp;
  zdpReq->req.reqPayload.mgmtLqiReq.startIndex = startIndex;

  N_Zdp_ZdpRequest(zdpReq);

  return zdpReq->service.seqNumCopy;
}

/** Transmission of ZDP MgmtLqi Request is done
    \param resp ZDP response parameters
*/
static void zdpMgmtLqiResp(ZDO_ZdpResp_t* zdpResp)
{
  if (zdpIsZigBeeStandardStatus(zdpResp->respPayload.status))
  {
    N_Zdp_MgmtLqiRsp_t *dstRsp = &rspExtraMemory.mgmtLqi.rsp;
    memset(dstRsp, 0, sizeof(N_Zdp_MgmtLqiRsp_t));

    ZDO_MgmtLqiResp_t *srcRsp = &zdpResp->respPayload.mgmtLqiResp;
    ZDO_ZdpReq_t *zdpReq = GET_PARENT_BY_FIELD(ZDO_ZdpReq_t, resp, zdpResp);

    dstRsp->sequenceNumber = zdpReq->service.seqNumCopy;
    dstRsp->status = zdpResp->respPayload.status;

    if (dstRsp->status == N_ZDP_STATUS_SUCCESS)
    {
      dstRsp->neighborTableEntries =  srcRsp->neighborTableEntries;
      dstRsp->startIndex = srcRsp->startIndex;
      /** Maximun allowed neighborTableListCount is 2 as per the specification
          See ZigBee spec r20, Table 2.126, page 228. */
      dstRsp->neighborTableListCount = MIN(srcRsp->neighborTableListCount, ZDP_MAX_NEIGHBOR_TABLE_LIST_COUNT);
      memcpy(dstRsp->pNeighborTableList, srcRsp->neighborTableList,
             dstRsp->neighborTableListCount * sizeof(dstRsp->pNeighborTableList[0]));
    }

    N_UTIL_CALLBACK(N_Zdp_ClientCallback_t, s_N_ZdpClient_Subscribers, ReceivedMgmtLqiRsp, (dstRsp));
  }

  freeZdpReq(zdpResp);
}

/** Send Mgmt_Permit_Joining_req, see ZigBee 2007 specification 2.4.3.3.7.
    \param permitDuration
    \param trustCenterSignificance
    \param pDestinationAddress Destination address of message
    \returns The sequence number of the sent command, this can be used to match a received response with the request
*/
uint8_t N_Zdp_SendMgmtPermitJoiningReq_Impl(uint8_t permitDuration, uint8_t trustCenterSignificance, N_Address_t* pDestinationAddress)
{
  ZDO_ZdpReq_t *zdpReq = getFreeZdpReq();

  if (zdpReq)
  {
    ZDO_MgmtPermitJoiningReq_t *permitJoinReq = &zdpReq->req.reqPayload.mgmtPermitJoiningReq;

    zdpReq->ZDO_ZdpResp = zdpPermitJoiningResp;
    zdpReq->reqCluster = MGMT_PERMIT_JOINING_CLID;

    zdpReq->dstAddrMode = N_Cmi_PlatformToBcAddressModeConvert(pDestinationAddress->addrMode);
    N_Cmi_PlatformToBcAddressingConvert(&zdpReq->dstAddress, pDestinationAddress);

    permitJoinReq->permitDuration = permitDuration;
    permitJoinReq->tcSignificance = trustCenterSignificance;

    N_Zdp_ZdpRequest(zdpReq);

    return zdpReq->service.seqNumCopy;
  }

  /* What should be returned here? */
  return 0;
}

/** ZDP MgmtPermitJoiningResponse handler
    \param resp ZDP response parameters
*/
static void zdpPermitJoiningResp(ZDO_ZdpResp_t *zdpResp)
{
  if (zdpIsZigBeeStandardStatus(zdpResp->respPayload.status))
  {
    N_Zdp_StatusRsp_t response;
    response.status = zdpResp->respPayload.status;
    response.sequenceNumber = zdpResp->respPayload.seqNum;

    N_UTIL_CALLBACK(N_Zdp_ClientCallback_t, s_N_ZdpClient_Subscribers, ReceivedMgmtPermitJoiningRsp, (&response));
  }

  freeZdpReq(zdpResp);
}

/** Send Mgmt_NWK_Update_req, see ZigBee 2007 specification 2.4.3.3.9.
    \param channelMask The channels to scan
    \param scanDuration Scan duration
    \param scanCount Number of energy scans to be conducted (Not used)
    \param nwkUpdateId New network ID
    \param nwkManagerAddr Network address of the network manager (Not used)
    \param pDestinationAddress Destination address of message
    \returns The sequence number of the sent command, this can be used to match a received response with the request
*/
uint8_t N_Zdp_SendMgmtNwkUpdateReq_Impl(uint32_t channelMask, uint8_t scanDuration,
  uint8_t scanCount, uint8_t nwkUpdateId, uint16_t nwkManagerAddr, N_Address_t *pDestinationAddress)
{
  ZDO_ZdpReq_t *zdpReq = getFreeZdpReq();

  if (NULL == zdpReq)
    return 0;

  zdpReq->dstAddrMode = N_Cmi_PlatformToBcAddressModeConvert(pDestinationAddress->addrMode);
  N_Cmi_PlatformToBcAddressingConvert(&zdpReq->dstAddress, pDestinationAddress);
  zdpReq->reqCluster = MGMT_NWK_UPDATE_CLID;
  zdpReq->ZDO_ZdpResp = freeZdpReq;

  zdpReq->req.reqPayload.mgmtNwkUpdateReq.scanChannels = channelMask;
  zdpReq->req.reqPayload.mgmtNwkUpdateReq.scanDuration = scanDuration;
  zdpReq->req.reqPayload.mgmtNwkUpdateReq.nwkManagerAddr = nwkManagerAddr;
  if (ZDO_MGMT_CHANNEL_CHANGE == scanDuration || ZDO_MGMT_NWK_PARAMS_CHANGE == scanDuration)
    zdpReq->req.reqPayload.mgmtNwkUpdateReq.nwkUpdateId = nwkUpdateId;
  else
    zdpReq->req.reqPayload.mgmtNwkUpdateReq.scanCount = scanCount;

  N_Zdp_ZdpRequest(zdpReq);

  return zdpReq->service.seqNumCopy;
}

/** Send Mgmt_Rtg_req, see ZigBee 2007 specification 2.4.3.3.3.
    \param startIndex
    \param pDestinationAddress Destination address of message
    \returns The sequence number of the sent command, this can be used to match a received response with the request
*/
uint8_t N_Zdp_SendMgmtRtgReq_Impl(uint8_t startIndex, N_Address_t *pDestinationAddress)
{
  ZDO_ZdpReq_t *zdpReq = getFreeZdpReq();

  if (NULL == zdpReq)
    return 0;

  zdpReq->dstAddrMode = N_Cmi_PlatformToBcAddressModeConvert(pDestinationAddress->addrMode);
  N_Cmi_PlatformToBcAddressingConvert(&zdpReq->dstAddress, pDestinationAddress);
  zdpReq->reqCluster = MGMT_RTG_CLID;
  zdpReq->ZDO_ZdpResp = zdpMgmtRtgResp;
  zdpReq->req.reqPayload.mgmtRtgReq.startIndex = startIndex;

  N_Zdp_ZdpRequest(zdpReq);

  return zdpReq->service.seqNumCopy;
}

/** Transmission of ZDP MgmtRtg Request is done
    \param resp ZDP response parameters
*/

static void zdpMgmtRtgResp(ZDO_ZdpResp_t* zdpResp)
{
  if (zdpIsZigBeeStandardStatus(zdpResp->respPayload.status))
  {
    N_Zdp_MgmtRtgRsp_t *dstRsp = &rspExtraMemory.mgmtRtg.rsp;
    memset(dstRsp, 0, sizeof(N_Zdp_MgmtRtgRsp_t));

    ZDO_MgmtRtgResp_t *srcRsp = &zdpResp->respPayload.mgmtRtgResp;
    ZDO_ZdpReq_t *zdpReq = GET_PARENT_BY_FIELD(ZDO_ZdpReq_t, resp, zdpResp);

    dstRsp->sequenceNumber = zdpReq->service.seqNumCopy;
    dstRsp->status = zdpResp->respPayload.status;

    if (dstRsp->status == N_ZDP_STATUS_SUCCESS)
    {
       dstRsp->routingTableEntries =  srcRsp->routingTableEntries;
       dstRsp->startIndex = srcRsp->startIndex;
       dstRsp->routingTableListCount = MIN(srcRsp->routingTableListCount, ZDP_ROUTING_TABLE_LIST_SIZE);
       memcpy(dstRsp->pRoutingTableList, srcRsp->routingTableList,
               dstRsp->routingTableListCount * sizeof(dstRsp->pRoutingTableList[0]));
    }

    N_UTIL_CALLBACK(N_Zdp_ClientCallback_t, s_N_ZdpClient_Subscribers, ReceivedMgmtRtgRsp, (dstRsp));
  }

  freeZdpReq(zdpResp);
}

/** Send Mgmt_Leave_Req, see ZigBee 2007 specificaiton 2.4.3.3.5.
    \param deviceIeeeAddress The IEEE address of the device to be removed.
    \param removeChildren If TRUE the device should also remove its children.
    \param rejoin If TRUE the device is requested to rejoin the network.
    \param pDestinationAddress Destination address of message
*/
uint8_t N_Zdp_SendMgmtLeaveReq_Impl(N_Address_Extended_t deviceIeeeAddress, bool removeChildren, bool rejoin, N_Address_t* pDestinationAddress)
{
  ZDO_ZdpReq_t *zdpReq = getFreeZdpReq();

  if (zdpReq)
  {
    ZDO_MgmtLeaveReq_t *mgmtLeaveReq = &zdpReq->req.reqPayload.mgmtLeaveReq;
    ExtAddr_t extAddr;
    NWK_LeaveControl_t leaveControl;

    zdpReq->dstAddrMode = N_Cmi_PlatformToBcAddressModeConvert(pDestinationAddress->addrMode);
    N_Cmi_PlatformToBcAddressingConvert(&zdpReq->dstAddress, pDestinationAddress);
    zdpReq->reqCluster = MGMT_LEAVE_CLID;
    zdpReq->ZDO_ZdpResp = zdpMgmtLeaveResp;

    memcpy(&extAddr, deviceIeeeAddress, sizeof(ExtAddr_t));
    mgmtLeaveReq->deviceAddr = extAddr;
    mgmtLeaveReq->removeChildren = removeChildren;
    mgmtLeaveReq->rejoin = rejoin;

    /* Configure post-leave actions */
    leaveControl.cleanRouteTable    = 1U;
    leaveControl.cleanNeighborTable = 1U;
    leaveControl.cleanAddresmap     = 1U;
    leaveControl.cleanKeyPairSet    = 0U;
    leaveControl.cleanBindingTable  = 0U;
    leaveControl.cleanGroupTable    = 0U;
    leaveControl.cleanNwkSecKeys    = 0U;
    leaveControl.silent             = 0U;
    CS_WriteParameter(CS_LEAVE_NON_REJOIN_CONTROL_ID, &leaveControl);

    N_Zdp_ZdpRequest(zdpReq);

    return zdpReq->service.seqNumCopy;
  }

  return 0;  
}

/** ZDP MgmtLeaveResp handler
    \param resp ZDP response parameters
*/
static void zdpMgmtLeaveResp(ZDO_ZdpResp_t* zdpResp)
{
  if (zdpIsZigBeeStandardStatus(zdpResp->respPayload.status))
  {
    N_Zdp_StatusRsp_t mgmtLeaveResp;

    mgmtLeaveResp.sequenceNumber = zdpResp->respPayload.seqNum;
    mgmtLeaveResp.status = zdpResp->respPayload.status;

    N_UTIL_CALLBACK(N_Zdp_ClientCallback_t, s_N_ZdpClient_Subscribers, ReceivedMgmtLeaveRsp, (&mgmtLeaveResp));
  }

  freeZdpReq(zdpResp);
}

/** Send Mgmt_NWK_Update_notify, see ZigBee 2007 specification 2.4.4.3.9.
    \param totalTransmissions The number of transmissions reported by the device
    \param transmissionFailures Sum of the total transmission failures reported by the device
    \param transSeq The sequence number of the message to send
    \param pEDScanConfirm The Energy Detect Scan Confirm message
    \param pDestinationAddress Destination address of message
*/
void N_Zdp_SendMgmtNwkUpdateNotify_Impl(uint16_t totalTransmissions, uint16_t transmissionFailures,
  uint8_t transSeq, N_Zdp_EDScanConfirm_t *pEDScanConfirm, N_Address_t *pDestinationAddress)
{
  ZDO_ZdpReq_t *zdpReq = getFreeZdpReq();
  EDScan_t *edScan;
  uint8_t channel = 0U;

  if (NULL == zdpReq)
    return;

  zdpReq->dstAddrMode = N_Cmi_PlatformToBcAddressModeConvert(pDestinationAddress->addrMode);
  N_Cmi_PlatformToBcAddressingConvert(&zdpReq->dstAddress, pDestinationAddress);
  zdpReq->reqCluster = MGMT_NWK_UPDATE_CLID | ZDP_RESPONSE_MASK;
  zdpReq->ZDO_ZdpResp = freeZdpReq;

  zdpReq->req.reqPayload.mgmtNwkUpdateNotf.status = ZDO_SUCCESS_STATUS;
  edScan = &zdpReq->req.reqPayload.mgmtNwkUpdateNotf.scanResult;
  edScan->scannedChannels = pEDScanConfirm->scannedChannels;
  edScan->totalTransmissions = totalTransmissions;
  edScan->transmissionsFailures = transmissionFailures;
  edScan->scannedChannelsListCount = 0U;

  do
  {
    if (edScan->scannedChannels & (1U << channel))
    {
      edScan->energyValues[edScan->scannedChannelsListCount]
        = pEDScanConfirm->energyDetectList[edScan->scannedChannelsListCount];
      edScan->scannedChannelsListCount++;
      if (ED_SCAN_RESULTS_AMOUNT == edScan->scannedChannelsListCount)
        break;
    }
    ++channel;
  } while (channel < 8 * sizeof(edScan->scannedChannels));

  N_Zdp_ZdpRequest(zdpReq);
  zdpReq->service.seqNumCopy = transSeq;
}

/** Send DeviceAnnounce.
*/
uint8_t N_Zdp_SendDeviceAnnounce_Impl(void)
{
  ZDO_ZdpReq_t *zdpReq = getFreeZdpReq();

  if (NULL == zdpReq)
    return 0;

  zdpReq->dstAddrMode = APS_SHORT_ADDRESS;
  zdpReq->dstAddress.shortAddress = BROADCAST_ADDR_RX_ON_WHEN_IDLE;
  zdpReq->reqCluster = DEVICE_ANNCE_CLID;
  zdpReq->ZDO_ZdpResp = freeZdpReq;

  zdpReq->req.reqPayload.deviceAnnce.nwkAddrLocal = NWK_GetShortAddr();
  COPY_EXT_ADDR(zdpReq->req.reqPayload.deviceAnnce.ieeeAddrLocal, *MAC_GetExtAddr());
  zdpReq->req.reqPayload.deviceAnnce.macCapability = ZDO_GetNodeDescriptor()->macCapbFlags;

  N_Zdp_ZdpRequest(zdpReq);

  return zdpReq->service.seqNumCopy;
}

/** BitCloud events observer routine
    \param eventId Event identifier
    \param data Relative data
*/
static void nZdpObserver(SYS_EventId_t eventId, SYS_EventData_t data)
{
  if (BC_ZDP_REQUEST_RECEIVED == eventId)
  {
    fillAddressStructure((APS_DataInd_t *)data);
    N_UTIL_CALLBACK(N_Zdp_ServerCallback_t, s_N_ZdpServer_Subscribers, MessageReceived, ());
  }
  else if (BC_ZDP_RESPONSE_RECEIVED == eventId)
  {
    fillAddressStructure((APS_DataInd_t *)data);
    N_UTIL_CALLBACK(N_Zdp_ClientCallback_t, s_N_ZdpClient_Subscribers, MessageReceived, ());
  }
  else if (BC_ZDP_DEVICE_ANNOUNCE_RECEIVED == eventId)
  {
    fillDeviceAnnounceStructure((uint8_t *)data);
    N_UTIL_CALLBACK(N_Zdp_ServerCallback_t, s_N_ZdpServer_Subscribers, ReceivedDeviceAnnounce, (&deviceAnnounce));
  }
  else if (BC_ZDP_JOIN_PERMITTED_CHANGED == eventId)
  {
    bool permitted = (bool)data;
    N_UTIL_CALLBACK(N_Zdp_ServerCallback_t, s_N_ZdpServer_Subscribers, JoinPermitted, (permitted));
  }
}

/** Gather free buffer for ZDP request

    \return Pointer to free ZDP request structure, or NULL if no one exists.
*/
static ZDO_ZdpReq_t *getFreeZdpReq(void)
{
  for (uint8_t item = 0; item < N_ZDP_REQUESTS_POOL_SIZE; item++)
  {
    if (!zdpReqPool[item].busy)
    {
      zdpReqPool[item].busy = true;
      return &zdpReqPool[item].zdpReq;
    }
  }
  return NULL;
}

/** Release occupied ZDP request buffer
    \param resp ZDP response pointer
*/
static void freeZdpReq(ZDO_ZdpResp_t *resp)
{
  N_Zdp_ZdpReq_t *nZdpReq = GET_PARENT_BY_FIELD(N_Zdp_ZdpReq_t, zdpReq.resp, resp);

  N_ERRH_ASSERT_FATAL(nZdpReq->busy == true);

  nZdpReq->zdpReq.ZDO_ZdpResp = NULL;
  nZdpReq->busy = false;
}

/** Checks if received status is one of ZigBee spec. standard ZDP statuses.
    \param bcStatus BitCloud ZDO status

    \returns True, if status is ZigBee spec. standard statuses, false - BitCloud-specific one.
*/
static bool zdpIsZigBeeStandardStatus(ZDO_Status_t bcStatus)
{
  switch ((uint8_t)bcStatus)
  {
    case ZDO_SUCCESS_STATUS:
    case ZDO_INVALID_REQUEST_STATUS:
    case ZDO_DEVICE_NOT_FOUND_STATUS:
    case ZDO_INVALID_EP_STATUS:
    case ZDO_NOT_ACTIVE_STATUS:
    case ZDO_NOT_SUPPORTED_STATUS:
    case ZDO_TIMEOUT_STATUS:
    case ZDO_NO_MATCH_STATUS:
    case ZDO_NO_ENTRY_STATUS:
    case ZDO_NO_DESCRIPTOR_STATUS:
    case ZDO_INSUFFICIENT_SPACE_STATUS:
    case ZDO_NOT_PERMITTED_STATUS:
    case ZDO_TABLE_FULL_STATUS:
    case ZDO_NOT_AUTHORIZED_STATUS:
      return true;

    default:
      return false;
  }
}

/** Fill N_Address_t structure by information from received ZDP request
    \param ind - pointer to APS_DataInd_t structure
*/
static void fillAddressStructure(APS_DataInd_t *ind)
{
  address.endPoint = 0;
  address.panId    = N_DeviceInfo_GetNetworkPanId();
  address.addrMode = ind->srcAddrMode;
  if (N_DeviceInfo_IsFactoryNew())
  {
    address.addrMode = N_Address_Mode_Extended;
    memcpy(address.address.extendedAddress, &ind->srcAddress.extAddress, sizeof(ExtAddr_t));
  }
  else
  {
    address.addrMode = N_Address_Mode_Short;
    address.address.shortAddress = ind->srcAddress.shortAddress;
  }
}

/** Fill N_Zdp_DeviceAnnounce_t structure by information from received device announce frame
    \param req - pointer to ZDO_DeviceAnnceReq_t structure
*/
static void fillDeviceAnnounceStructure(uint8_t *req)
{
  ZDO_ZdpReqFrame_t *zdpReq = (ZDO_ZdpReqFrame_t *)req;
  ZDO_DeviceAnnceReq_t *devAnnceReq = &zdpReq->deviceAnnce;
  ExtAddr_t extAddr;

  deviceAnnounce.sequenceNumber = zdpReq->seqNum;
  deviceAnnounce.nwkAddr        = devAnnceReq->nwkAddrLocal;
  extAddr = devAnnceReq->ieeeAddrLocal;
  memcpy(deviceAnnounce.ieeeAddr, &extAddr, sizeof(ExtAddr_t));
  deviceAnnounce.capability     = *(uint8_t *)&devAnnceReq->macCapability;
}

/* eof N_Zdp.c */
