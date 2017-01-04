/**************************************************************************//**
  \file N_Connection_ReconnectHandler.c

  \brief Implementation of reconnect handler. Only for EndDevice.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    29.01.13 D. Kolmakov - Created.
******************************************************************************/

#if defined(ZIGBEE_END_DEVICE)
/******************************************************************************
                    Includes section
******************************************************************************/
#include <N_ConnectionEndDevice_Bindings.h>
#include <N_Connection_ReconnectHandler.h>
#include <N_Connection_Private.h>
#include <N_DeviceInfo.h>
#include <N_ErrH.h>
#include <N_Cmi.h>
#include <N_EndDeviceRobustness.h>
#include <N_AddressManager.h>
#include <N_AddressManager_Bindings.h>

#include <configServer.h>
#include <zdo.h>
#include <sysUtils.h>

/******************************************************************************
                   Defines section
******************************************************************************/
#define COMPID "N_Connection_ReconnectHandler"

/******************************************************************************
                    Types section
******************************************************************************/
typedef enum _ReconnectionType_t
{
  RECONNECTION_TYPE_NONE,
  RECONNECTION_TYPE_UNDEFINED,
  RECONNECTION_TYPE_REJOIN,
  RECONNECTION_TYPE_REJOIN_PARENT,
  RECONNECTION_TYPE_ORPHAN_JOIN,
  RECONNECTION_TYPE_UNSECURE_REJOIN
} ReconnectionType_t;

typedef struct _ReconnectHandlerMem_t
{
  ReconnectionType_t type;
  N_Connection_ReconnectHandler_JoinDone_t done;
  N_Beacon_t beacon;
  bool removeParent;
  bool reconnectOngoing;
} ReconnectHandlerMem_t;
/******************************************************************************
                    Prototypes section
******************************************************************************/
static void nReconnectHandlerStartNwkConf(ZDO_StartNetworkConf_t* conf);
static void nReconnectHandlerAddNeighbor(N_Beacon_t* pSelectedBeacon);
static void nReconnectHandlerStartNetwork(NWK_JoinControl_t *joinControl);
static void nReconnectHandlerLeaveNetwork(bool removeParent);
static void nReconnectHandlerLeaveResp(ZDO_ZdpResp_t *leaveResp);
static void networkLeftHandler(SYS_EventId_t eventId, SYS_EventData_t data);
static void nReconnectHandlerRejoin(ReconnectionType_t type);

/******************************************************************************
                    Static section
******************************************************************************/
static const N_Connection_ReconnectHandler_Callback_t* s_N_Connection_ReconnectHandler_callback = NULL;

static N_Beacon_t receivedBeacon = { 0u };
static ReconnectHandlerMem_t reconnectMem;

/* Request params for ZDO_StartNetworkReq */
static ZDO_StartNetworkReq_t startNetworkReq =
{
  .ZDO_StartNetworkConf = nReconnectHandlerStartNwkConf
};

/* Leave network parameters */
static ZDO_ZdpReq_t leaveReq =
{
  .ZDO_ZdpResp = nReconnectHandlerLeaveResp,
  .reqCluster  = MGMT_LEAVE_CLID,
  .dstAddrMode = APS_EXT_ADDRESS
};

static SYS_EventReceiver_t nConnectionReconnectEventReceiver = {
  .func = networkLeftHandler
};

/***********************************************************************************
                    Implementation section
***********************************************************************************/
/** Subscribes callbacks to the component internal events
*/
void N_Connection_ReconnectHandler_Subscribe_Impl(const N_Connection_ReconnectHandler_Callback_t* pSubscriber)
{
  N_ERRH_ASSERT_FATAL(pSubscriber != NULL);
  s_N_Connection_ReconnectHandler_callback = pSubscriber;
}

/** Try to find a parent (end-device only, no-op on a router).
*/
void N_ConnectionEndDevice_Reconnect_Impl(void)
{
  if ((!reconnectMem.reconnectOngoing) && (!N_Connection_UpdateNetwork_IsBusy()) && (!N_Connection_Is_ReconnectUrgentOngoing()))
  { // Only if parent lost detected
    reconnectMem.type = RECONNECTION_TYPE_UNDEFINED;
    ZDO_StopSyncReq();
    N_ERRH_ASSERT_FATAL(s_N_Connection_ReconnectHandler_callback != NULL);
    reconnectMem.reconnectOngoing = true;
    CS_WriteParameter(CS_ZDO_JOIN_ATTEMPTS_ID, &(uint8_t){1});
    s_N_Connection_ReconnectHandler_callback->Reconnect();
  }
}

/** Perform a network discovery (active scan), returning only devices of the own PAN.
    \param channelMask The channels to scan
    \param durationPerChannel The time to stay at each channel (enum)
    \param pfBeaconReceivedCallback Pointer to the function that should received incoming beacon indications during the network discovery
    \param pfDoneCallback Pointer to the function that should be called after the network discovery is done
    \note Only allowed between the Reconnect callback and the N_Connection_ReconnectHandler_ReconnectDone call.
*/
void N_Connection_ReconnectHandler_NetworkDiscovery_Impl(uint32_t channelMask,
                                                         N_Beacon_Order_t durationPerChannel,
                                                         N_Connection_ReconnectHandler_BeaconReceived_t pfBeaconReceivedCallback,
                                                         N_Connection_ReconnectHandler_NetworkDiscoveryDone_t pfDoneCallback)
{
  N_ERRH_ASSERT_FATAL(RECONNECTION_TYPE_UNDEFINED == reconnectMem.type);

  N_Cmi_NetworkDiscovery(channelMask,
                         durationPerChannel,
                         &receivedBeacon,
                         N_Connection_AssociateDiscoveryMode_OnlyThisPan,
                         N_DeviceInfo_GetNetworkExtendedPanId(),
                         BEACON_FILTERING_CRITERION_RECONNECT,
                         pfBeaconReceivedCallback,
                         pfDoneCallback);
}

/** Perform a ZigBee network rejoin to a new parent.
    \param pSelectedBeacon The beacon of the device to rejoin to
    \param pfDoneCallback Pointer to the function that should be called after the rejoin is done
    \note Only allowed between the Reconnect callback and the N_Connection_ReconnectHandler_ReconnectDone call.
*/
void N_Connection_ReconnectHandler_Rejoin_Impl(N_Beacon_t* pSelectedBeacon, N_Connection_ReconnectHandler_JoinDone_t pfDoneCallback)
{
  N_ERRH_ASSERT_FATAL(RECONNECTION_TYPE_UNDEFINED == reconnectMem.type);

  reconnectMem.type = RECONNECTION_TYPE_REJOIN;
  reconnectMem.done = pfDoneCallback;
  memcpy(&reconnectMem.beacon, pSelectedBeacon, sizeof(N_Beacon_t));
  reconnectMem.removeParent = true;
  reconnectMem.reconnectOngoing = true;
  CS_WriteParameter(CS_ZDO_JOIN_ATTEMPTS_ID, &(uint8_t){1});

  if (ZDO_IN_NETWORK_STATUS == ZDO_GetNwkStatus())
    nReconnectHandlerLeaveNetwork(reconnectMem.removeParent);
  else
    nReconnectHandlerRejoin(reconnectMem.type);
}

/** Perform a ZigBee network rejoin with the old parent.
    \note Only allowed between the Reconnect callback and the N_Connection_ReconnectHandler_ReconnectDone call.
*/
void N_Connection_ReconnectHandler_RejoinParent_Impl(N_Connection_ReconnectHandler_JoinDone_t pfDoneCallback)
{
  N_ERRH_ASSERT_FATAL(RECONNECTION_TYPE_UNDEFINED == reconnectMem.type);

  reconnectMem.type = RECONNECTION_TYPE_REJOIN_PARENT;
  reconnectMem.done = pfDoneCallback;
  memset(&reconnectMem.beacon, 0x00, sizeof(N_Beacon_t));
  reconnectMem.removeParent = false;
  reconnectMem.reconnectOngoing = true;
  CS_WriteParameter(CS_ZDO_JOIN_ATTEMPTS_ID, &(uint8_t){1});

  if (ZDO_IN_NETWORK_STATUS == ZDO_GetNwkStatus())
    nReconnectHandlerLeaveNetwork(reconnectMem.removeParent);
  else
    nReconnectHandlerRejoin(reconnectMem.type);
}

/** Perform a IEEE 802.15.4 orphan join to rejoin the old parent.
    \param pfDoneCallback Pointer to the function that should be called after the rejoin is done
    \note Only allowed between the Reconnect callback and the N_Connection_ReconnectHandler_ReconnectDone call.
*/
void N_Connection_ReconnectHandler_OrphanJoin_Impl(N_Connection_ReconnectHandler_JoinDone_t pfDoneCallback)
{
  N_ERRH_ASSERT_FATAL(RECONNECTION_TYPE_UNDEFINED == reconnectMem.type);

  reconnectMem.type = RECONNECTION_TYPE_ORPHAN_JOIN;
  reconnectMem.done = pfDoneCallback;
  memset(&reconnectMem.beacon, 0x00, sizeof(N_Beacon_t));
  reconnectMem.removeParent = false;
  reconnectMem.reconnectOngoing = true;
  CS_WriteParameter(CS_ZDO_JOIN_ATTEMPTS_ID, &(uint8_t){1});

  if (ZDO_IN_NETWORK_STATUS == ZDO_GetNwkStatus())
    nReconnectHandlerLeaveNetwork(reconnectMem.removeParent);
  else
    nReconnectHandlerRejoin(reconnectMem.type);
}

/** Perform a ZigBee network unsecure rejoin to a parent router.
    \param pSelectedBeacon The beacon of the device to rejoin to
    \param pfDoneCallback Pointer to the function that should be called after the rejoin is done (including authentication)
    \note Only allowed between the Reconnect callback and the N_Connection_ReconnectHandler_ReconnectDone call.
*/
void N_Connection_ReconnectHandler_UnsecureRejoin_Impl(N_Beacon_t* pSelectedBeacon, N_Connection_ReconnectHandler_JoinDone_t pfDoneCallback)
{
  N_ERRH_ASSERT_FATAL(RECONNECTION_TYPE_UNDEFINED == reconnectMem.type);

  reconnectMem.type = RECONNECTION_TYPE_UNSECURE_REJOIN;
  reconnectMem.done = pfDoneCallback;
  memcpy(&reconnectMem.beacon, pSelectedBeacon, sizeof(N_Beacon_t));
  reconnectMem.removeParent = true;
  reconnectMem.reconnectOngoing = true;
  CS_WriteParameter(CS_ZDO_JOIN_ATTEMPTS_ID, &(uint8_t){1});

  if (ZDO_IN_NETWORK_STATUS == ZDO_GetNwkStatus())
    nReconnectHandlerLeaveNetwork(reconnectMem.removeParent);
  else
    nReconnectHandlerRejoin(reconnectMem.type);
}

/** Indicates that the reconnect-handler has finished finding a parent.
    \param status Indicates N_Connection_Result_Success (a parent was found) or N_Connection_Result_Failure (no parent found)
    \note Only allowed after a Reconnect callback.
*/
void N_Connection_ReconnectHandler_ReconnectDone_Impl(N_Connection_Result_t status)
{
  N_ERRH_ASSERT_FATAL(RECONNECTION_TYPE_NONE != reconnectMem.type);

  reconnectMem.type = RECONNECTION_TYPE_NONE;

  if (N_Connection_Result_Success == status)
  {
    ZDO_StartSyncReq();
    N_Connection_Connected();
  }
  reconnectMem.reconnectOngoing = false;
  CS_WriteParameter(CS_ZDO_JOIN_ATTEMPTS_ID, &(uint8_t){3});
}

/** Peforms rejoin procedure with accordance to the chosen reconnection type.
    \param type reconnection type.
*/
static void nReconnectHandlerRejoin(ReconnectionType_t type)
{
  NWK_JoinControl_t joinControl =
  {
    .annce = true,
    .discoverNetworks = false,
    .secured = true,
  };

  switch (type)
  {
    case RECONNECTION_TYPE_REJOIN:
      joinControl.method = NWK_JOIN_VIA_REJOIN;
      nReconnectHandlerAddNeighbor(&reconnectMem.beacon);
      break;

    case RECONNECTION_TYPE_REJOIN_PARENT:
      joinControl.method = NWK_JOIN_VIA_REJOIN;
      break;

    case RECONNECTION_TYPE_ORPHAN_JOIN:
      joinControl.method = NWK_JOIN_VIA_ORPHAN_SCAN;
      break;

    case RECONNECTION_TYPE_UNSECURE_REJOIN:
      joinControl.method = NWK_JOIN_VIA_REJOIN;
      joinControl.secured = false;
      nReconnectHandlerAddNeighbor(&reconnectMem.beacon);
      break;

    case RECONNECTION_TYPE_NONE:
    case RECONNECTION_TYPE_UNDEFINED:
    default:
      N_ERRH_ASSERT_FATAL(false);
      break;
  }

  nReconnectHandlerStartNetwork(&joinControl);
}

/** Creates new record in the neighbor table and fills it with information from beacon.
    \param pSelectedBeacon pointer to the beacon.
*/
static void nReconnectHandlerAddNeighbor(N_Beacon_t *pSelectedBeacon)
{
  ExtAddr_t extAddr = 0ULL;
  NwkNeighbor_t *neighbor = NWK_FindNeighborByShortAddr(N_DeviceInfo_GetParentNetworkAddress());

  N_ERRH_ASSERT_FATAL(pSelectedBeacon);

  if (neighbor)
    NWK_RemoveNeighbor(neighbor, true);

  neighbor = NWK_AddKnownNeighbor(pSelectedBeacon->sourceAddress, &extAddr, true);
  N_ERRH_ASSERT_FATAL(neighbor); /* Neighbor table full */
  neighbor->logicalChannel = pSelectedBeacon->logicalChannel;
  neighbor->depth = pSelectedBeacon->depth;
  neighbor->panId = pSelectedBeacon->panId;
  memcpy((uint8_t*)&neighbor->extPanId, pSelectedBeacon->extendedPanId, sizeof(uint64_t));
  neighbor->updateId = pSelectedBeacon->updateId;
  neighbor->permitJoining = pSelectedBeacon->permitJoining;
}

/** Stores join parameters and performs ZDO_StartNetworkReq.
    \param joinControl parameters for joining network procedure.
*/
static void nReconnectHandlerStartNetwork(NWK_JoinControl_t *joinControl)
{
  startNetworkReq.ZDO_StartNetworkConf = nReconnectHandlerStartNwkConf;
  CS_WriteParameter(CS_JOIN_CONTROL_ID, joinControl);
  ZDO_StartNetworkReq(&startNetworkReq);
}

/** Start network confirm.
    \param conf confirm parameters.
*/
static void nReconnectHandlerStartNwkConf(ZDO_StartNetworkConf_t *conf)
{
  N_Connection_Result_t result;
  NWK_JoinControl_t joinControl;

  if (ZDO_SUCCESS_STATUS == conf->status)
    result = N_Connection_Result_Success;
  else if (ZDO_AUTHENTICATION_FAIL_STATUS == conf->status)
    result = N_Connection_Result_NoNetworkKey;
  else
    result = N_Connection_Result_Failure;

  CS_ReadParameter(CS_JOIN_CONTROL_ID, &joinControl);

  /*Clear Address Manager if Successfully joined through association procedure*/
  if(conf->status == ZDO_SUCCESS_STATUS)
  {
    if(joinControl.method == NWK_JOIN_VIA_ASSOCIATION)
      N_AddressManager_AssignAddressRange(0, 0, 0, 0);
  }

  reconnectMem.type = RECONNECTION_TYPE_UNDEFINED;

  N_ERRH_ASSERT_FATAL(reconnectMem.done); /* No callback function */
  reconnectMem.done(result);
  reconnectMem.done = NULL;
}

/** Leave network routine.
    \param removeParent if true then neighbor table will be cleaned during leave.
*/
static void nReconnectHandlerLeaveNetwork(bool removeParent)
{
  ZDO_MgmtLeaveReq_t *zdpLeaveReq = &leaveReq.req.reqPayload.mgmtLeaveReq;
  NWK_LeaveControl_t leaveControl;

  COPY_EXT_ADDR(leaveReq.dstAddress.extAddress,  *MAC_GetExtAddr());
  COPY_EXT_ADDR(zdpLeaveReq->deviceAddr, *MAC_GetExtAddr());
  zdpLeaveReq->rejoin         = 0;
  zdpLeaveReq->removeChildren = 0;
  zdpLeaveReq->reserved       = 0;

  /* Configure post-leave actions */
  leaveControl.cleanRouteTable    = removeParent;
  leaveControl.cleanNeighborTable = removeParent;
  leaveControl.cleanAddresmap     = 0U;
  leaveControl.cleanKeyPairSet    = 0U;
  leaveControl.cleanBindingTable  = 0U;
  leaveControl.cleanGroupTable    = 0U;
  leaveControl.cleanNwkSecKeys    = 0U;
  leaveControl.silent             = 1U;
  CS_WriteParameter(CS_LEAVE_NON_REJOIN_CONTROL_ID, &leaveControl);

  SYS_SubscribeToEvent(BC_EVENT_NETWORK_LEFT, &nConnectionReconnectEventReceiver);
  ZDO_ZdpReq(&leaveReq);
}

/** Leave network response handler.
    \param leaveResp Leave response parameters
*/
static void nReconnectHandlerLeaveResp(ZDO_ZdpResp_t *leaveResp)
{
  /* Retry until success */
  if ((ZDO_SUCCESS_STATUS != leaveResp->respPayload.status) && (ZDO_FAIL_STATUS != leaveResp->respPayload.status))
    nReconnectHandlerLeaveNetwork(reconnectMem.removeParent);
}

/** BC_EVENT_NETWORK_LEFT event handler.
    \param eventId - event identifier.
    \param data - event data.
*/
static void networkLeftHandler(SYS_EventId_t eventId, SYS_EventData_t data)
{
  nReconnectHandlerRejoin(reconnectMem.type);
  SYS_UnsubscribeFromEvent(BC_EVENT_NETWORK_LEFT, &nConnectionReconnectEventReceiver);
  (void)eventId;
  (void)data;
}

#endif /* ZIGBEE_END_DEVICE */
// eof N_EndDeviceRobustness.c
