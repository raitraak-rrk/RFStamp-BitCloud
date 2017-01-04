/**************************************************************************//**
  \file N_ConnectionRouter.c

  \brief Implementation of ZigBee network connection creation and maintainance.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    22.08.12 A. Razinkov - created
******************************************************************************/

#if !defined(ZIGBEE_END_DEVICE)
/******************************************************************************
                    Includes section
******************************************************************************/
#include <N_ConnectionRouter_Bindings.h>
#include <N_DeviceInfo_Bindings.h>
#include <N_Connection.h>
#include <N_ConnectionRouter_Init.h>
#include <N_DeviceInfo.h>
#include <N_ErrH.h>
#include <N_Cmi.h>
#include <N_Zdp.h>
#include <N_Radio_Internal.h>
#include <N_Log.h>
#include <N_AddressManager.h>
#include <N_AddressManager_Bindings.h>

#include <configServer.h>
#include <zdo.h>
#include <sysUtils.h>

/******************************************************************************
                    Defines section
******************************************************************************/
#define COMPID "N_Connection"

#ifndef N_CONNECTION_MAX_SUBSCRIBERS
  #define N_CONNECTION_MAX_SUBSCRIBERS 10u
#endif

/******************************************************************************
                    Types section
******************************************************************************/
typedef struct _N_Connection_JoinNetworkParams_t
{
  N_Address_Extended_t extendedPanId;
  uint8_t channel;
  uint16_t panId;
  uint16_t networkAddress;
  uint8_t networkUpdateId;
  uint8_t networkKey[SECURITY_KEY_SIZE];
  N_Connection_JoinNetworkDone_t pfDoneCallback;
} N_Connection_JoinNetworkParams_t;

typedef struct _N_Connection_FindFreePanParams_t
{
  ExtPanId_t                     extendedPanId;
  PanId_t                        panId;
  N_Connection_FindFreePanDone_t findFreePanDoneCallback;
  NWK_NetworkDiscoveryReq_t      nwkDiscoveryRequest;
  bool                           panExists;
} N_Connection_FindFreePanParams_t;

typedef void (*ApplyingJoinParamsDone_t)(void);

/******************************************************************************
                    Prototypes section
******************************************************************************/
static void ZDO_StartNetworkConf(ZDO_StartNetworkConf_t* conf);
static void zdpLeaveResp(ZDO_ZdpResp_t *leaveResp);
static void applyJoinParams(ApplyingJoinParamsDone_t applyDoneCallback);
static void connected(void);
static void disconnected(void);
static void networkJoined(void);
static void nConnectionObserver(SYS_EventId_t eventId, SYS_EventData_t data);
static void applyingJoinParamsDone(void);
static void NWK_NetworkDiscoveryConf(NWK_NetworkDiscoveryConf_t *conf);
static void resetToFactoryDefaults(void);
static void searchNetworkTimerFired(void);
static void searchNetworkDiscoveryDone(void);
static void devAnnounceTimerFired(void);
static void switchToUpdatedNwkChannelDone(MAC_SetConf_t *conf);
static void leaveNetworkAndJoin(void);
static void nConnectionLeaveAndJoinObserver(SYS_EventId_t eventId, SYS_EventData_t data);
static void childAssociated(uint16_t networkAddress, N_Address_Extended_t ieeeAddress);
static void nChildAssociated(SYS_EventId_t eventId, SYS_EventData_t data);
/******************************************************************************
                    Static section
******************************************************************************/
/* Join network procedure finished callback pointer */
static N_Connection_JoinNetworkDone_t joinNetworkDoneCallback;

/* Request params for ZDO_StartNetworkReq */
static ZDO_StartNetworkReq_t startNetworkReq =
{
  .ZDO_StartNetworkConf = ZDO_StartNetworkConf
};

/* Network join procedure control */
static NWK_JoinControl_t joinControl =
{
  .method = NWK_JOIN_VIA_COMMISSIONING,
  .annce = true,
  .discoverNetworks = false,
  .secured = true,
  .clearNeighborTable = true
};

/* Leave network parameters */
static ZDO_ZdpReq_t leaveReq =
{
  .ZDO_ZdpResp =  zdpLeaveResp,
  .reqCluster = MGMT_LEAVE_CLID,
  .dstAddrMode = APS_EXT_ADDRESS
};

/* Find free PAN parameters */
static N_Connection_FindFreePanParams_t findFreePan =
{
  .nwkDiscoveryRequest =
  {
    .NWK_NetworkDiscoveryConf = NWK_NetworkDiscoveryConf,
    .scanDuration = 1,
  }
};

/* Join network request parameters */
N_Connection_JoinNetworkParams_t joinNetworkParams;

/* BitCloud events receiver */
static SYS_EventReceiver_t nConnectionEventReceiver = {.func = nConnectionObserver};

static N_Task_Id_t s_taskId = N_TASK_ID_NONE;
N_UTIL_CALLBACK_DECLARE(N_Connection_Callback_t, s_N_Connection_Subscribers, N_CONNECTION_MAX_SUBSCRIBERS);

static HAL_AppTimer_t searchNetworkTimer =
{
  .mode = TIMER_ONE_SHOT_MODE,
  .interval = 1000,
  .callback = searchNetworkTimerFired
};

static HAL_AppTimer_t devAnnounceTimer =
{
  .mode = TIMER_ONE_SHOT_MODE,
  .interval = 1500,
  .callback = devAnnounceTimerFired
};

static N_Beacon_t networkSearchBeacon;
static bool receivedRemoveDevice = false;
static SYS_EventReceiver_t nConnectionLeaveAndJoin = {.func = nConnectionLeaveAndJoinObserver};
static SYS_EventReceiver_t nChildAssociatedEventListener = { .func = nChildAssociated};
/***********************************************************************************
                    Implementation section
***********************************************************************************/
/** Initialises the component
*/
void N_ConnectionRouter_Init(void)
{
  s_taskId = N_Task_GetIdFromEventHandler(N_ConnectionRouter_EventHandler);
  SYS_SubscribeToEvent(BC_EVENT_REMOVE_DEVICE_RECEIVED, &nConnectionEventReceiver);
  SYS_SubscribeToEvent(BC_EVENT_NETWORK_LEFT, &nConnectionEventReceiver);
  SYS_SubscribeToEvent(BC_EVENT_CHILD_ASSOCIATED, &nChildAssociatedEventListener);
}

/** Performs network discovery and channel change if the network
    which this device is assotiated with works on other channel.
    Actual only for router.
*/
void N_Connection_SearchNetwork(void)
{
  if (N_DeviceInfo_IsFactoryNew())
  {
    searchNetworkTimer.interval = ((uint32_t)SYS_GetRandomNumber() * 2000UL) / 0xFFFF + 1000;
  }
  else
  {
    searchNetworkTimer.interval = ((uint32_t)SYS_GetRandomNumber() * 1000UL) / 0xFFFF + 1000;    
  }
  searchNetworkTimer.callback = searchNetworkTimerFired;
  HAL_StartAppTimer(&searchNetworkTimer);
}

/** Subscribes callbacks to the component internal events
*/
void N_ConnectionRouter_Subscribe_Impl(const N_Connection_Callback_t* pSubscriber)
{
  N_ERRH_ASSERT_FATAL(s_taskId != N_TASK_ID_NONE);    // check if initialized
  N_UTIL_CALLBACK_SUBSCRIBE(N_Connection_Callback_t, s_N_Connection_Subscribers, pSubscriber);
}

/** The event handler for the component's task
*/
bool N_ConnectionRouter_EventHandler(N_Task_Event_t evt)
{
  (void)evt;
  return false;
}

/** Join a network.
    \param extendedPanId Extended PAN ID of the network to join
    \param channel Channel of the network to join
    \param panId PAN ID of the network to join
    \param networkAddress Network address of this device on the network to join
    \param networkUpdateId Network update id of the network to join
    \param pNetworkKey Network key to use (8 bytes); will be cleared when it has been used, or NULL to have a random one generated
    \param pfDoneCallback Pointer to the function that will be called when the action is done

    This is an asynchronous call: do not call any other function of this
    component until the callback is received.
*/
void N_ConnectionRouter_JoinNetwork_Impl(N_Address_Extended_t extendedPanId,
                              uint8_t channel,
                              uint16_t panId,
                              uint16_t networkAddress,
                              uint8_t networkUpdateId,
                              uint8_t* pNetworkKey,
                              N_Connection_JoinNetworkDone_t pfDoneCallback)
{
  
  N_ERRH_ASSERT_FATAL((N_DeviceInfo_IsChannelInMask_Impl(channel,N_DeviceInfo_GetPrimaryChannelMask())) 
                     || (N_DeviceInfo_IsChannelInMask_Impl(channel,N_DeviceInfo_GetSecondaryChannelMask())));
  N_ERRH_ASSERT_FATAL((NULL != pfDoneCallback) || (NULL != joinNetworkDoneCallback));
  
  /* Generate random key, if NULL passed as a pointer */
  if (pNetworkKey)
    memcpy(joinNetworkParams.networkKey, pNetworkKey, SECURITY_KEY_SIZE);
  else
    N_Security_GenerateNewNetworkKey(joinNetworkParams.networkKey);
  
  /* Backup join network parameters */
  memcpy(joinNetworkParams.extendedPanId, extendedPanId, sizeof(N_Address_Extended_t));
  joinNetworkParams.channel         = channel;
  joinNetworkParams.panId           = panId;
  joinNetworkParams.networkAddress  = networkAddress;
  joinNetworkParams.networkUpdateId = networkUpdateId;
  joinNetworkParams.pfDoneCallback  = pfDoneCallback;
  
  /* Check for factory new */
  if (N_DeviceInfo_IsFactoryNew())
  {
    applyJoinParams(applyingJoinParamsDone);
  }
  else
  {
    if (ZDO_IN_NETWORK_STATUS == ZDO_GetNwkStatus())
    {
      /* Leave the network if device is in network*/
      leaveNetworkAndJoin();
    }
    else
    {
      applyJoinParams(applyingJoinParamsDone);
    }
  }
}

/** Callback on applying join parameters done
*/
static void applyingJoinParamsDone(void)
{    
    /* Synchornize network paramters with DeviceInfobase */  
  N_DeviceInfo_SetNetworkPanId(joinNetworkParams.panId);
  N_DeviceInfo_SetNetworkExtendedPanId(joinNetworkParams.extendedPanId);
  N_DeviceInfo_SetNetworkChannel(joinNetworkParams.channel);
  N_DeviceInfo_SetNetworkAddress(joinNetworkParams.networkAddress);
  
  /* Set factory new as false since the parameters are updated*/
  N_DeviceInfo_SetFactoryNew(false);
  
  /* Enter the network */
  CS_WriteParameter(CS_JOIN_CONTROL_ID, &joinControl);
  ZDO_StartNetworkReq(&startNetworkReq);
}


/** Configure BitCloud to join particular network.
    \param applyDoneCallback Pointer to the function that will be called after paramters applying done
*/
static void applyJoinParams(ApplyingJoinParamsDone_t applyDoneCallback)
{  
  /* Settings to Config Server */
  CS_WriteParameter(CS_EXT_PANID_ID, joinNetworkParams.extendedPanId);
  CS_WriteParameter(CS_NWK_EXT_PANID_ID, joinNetworkParams.extendedPanId);
  /* This is used in Zdo for join into the mentioned channel */
  CS_WriteParameter(CS_CHANNEL_MASK_ID, &(uint32_t){1ul << joinNetworkParams.channel});
  CS_WriteParameter(CS_NWK_LOGICAL_CHANNEL_ID, &joinNetworkParams.channel);
  NWK_SetLogicalChannel(joinNetworkParams.channel);

  /* Set the Key */
  NWK_SetKey(joinNetworkParams.networkKey, 0);
  NWK_ActivateKey(0);

  NWK_SetUpdateId(joinNetworkParams.networkUpdateId);

  CS_WriteParameter(CS_NWK_PANID_ID, &joinNetworkParams.panId);
  CS_WriteParameter(CS_NWK_PREDEFINED_PANID_ID, &(bool){true});
  
  /* This set is required, because we enter the network via rejoin */
  NWK_SetPanId(joinNetworkParams.panId);

  CS_WriteParameter(CS_NWK_ADDR_ID, &joinNetworkParams.networkAddress);

  joinNetworkDoneCallback = joinNetworkParams.pfDoneCallback;

  /* Enter the Distributed TC mode */
  APS_SetTrustCenterAddress(&(ExtAddr_t){APS_DISTRIBUTED_TC_ADDRESS});
  /* Set ZLL link key as a primary global key */
  N_Cmi_SetZllLinkKeyAsPrimary(applyDoneCallback);
}

/** Join network confirm handler.
    \param conf Confirmation parameters
*/
static void ZDO_StartNetworkConf(ZDO_StartNetworkConf_t* conf)
{
  if (ZDO_SUCCESS_STATUS == conf->status)
  {
    networkJoined();
    connected();
    joinNetworkDoneCallback(N_Connection_Result_Success);    
  }
  else
    joinNetworkDoneCallback(N_Connection_Result_Failure);
}

/** Leave network and Join routine.
*/
static void leaveNetworkAndJoin(void)
{
  ZDO_MgmtLeaveReq_t *zdpLeaveReq = &leaveReq.req.reqPayload.mgmtLeaveReq;
  NWK_LeaveControl_t leaveControl;

  COPY_EXT_ADDR(leaveReq.dstAddress.extAddress,  *MAC_GetExtAddr());
  COPY_EXT_ADDR(zdpLeaveReq->deviceAddr, *MAC_GetExtAddr());
  zdpLeaveReq->rejoin         = 0;
  zdpLeaveReq->removeChildren = 0;
  zdpLeaveReq->reserved       = 0;

  /* Configure post-leave actions */
  leaveControl.cleanRouteTable    = 1U;
  leaveControl.cleanNeighborTable = 1U;
  leaveControl.cleanAddresmap     = 1U;
  leaveControl.cleanKeyPairSet    = 0U;
  leaveControl.cleanBindingTable  = 1U;
  leaveControl.cleanGroupTable    = 1U;
  leaveControl.cleanNwkSecKeys    = 0U;
  leaveControl.silent             = 0U;
  CS_WriteParameter(CS_LEAVE_NON_REJOIN_CONTROL_ID, &leaveControl);

  SYS_SubscribeToEvent(BC_EVENT_NETWORK_LEFT, &nConnectionLeaveAndJoin);
  SYS_UnsubscribeFromEvent(BC_EVENT_NETWORK_LEFT, &nConnectionEventReceiver);

  ZDO_ZdpReq(&leaveReq);
}

/** Leave network response handler.
    \param leaveResp Leave response parameters
*/
static void zdpLeaveResp(ZDO_ZdpResp_t *leaveResp)
{
  /* Retry until success */
  if (ZDO_SUCCESS_STATUS != leaveResp->respPayload.status)
    leaveNetworkAndJoin();
}

/** Calls callback on Connected event
*/
static void connected(void)
{
  uint32_t channelMask = N_DeviceInfo_GetPrimaryChannelMask();

  if (N_DeviceInfo_TrustCenterMode_Distributed != N_DeviceInfo_GetTrustCenterMode())
    channelMask |= N_DeviceInfo_GetSecondaryChannelMask();

  CS_WriteParameter(CS_CHANNEL_MASK_ID, &channelMask);

  N_UTIL_CALLBACK(N_Connection_Callback_t, s_N_Connection_Subscribers, Connected, ());
}

/** Calls callback on Disconnected event
*/
static void disconnected(void)
{
  N_UTIL_CALLBACK(N_Connection_Callback_t, s_N_Connection_Subscribers, Disconnected, ());
}

/** Calls callback on NetworkJoined event
*/
static void networkJoined(void)
{
  N_UTIL_CALLBACK(N_Connection_Callback_t, s_N_Connection_Subscribers, NetworkJoined, ());
}

/** BC_EVENT_NETWORK_LEFT event handler.
    \param eventId - event identifier.
    \param data - event data.
*/
static void nConnectionLeaveAndJoinObserver(SYS_EventId_t eventId, SYS_EventData_t data)
{
  SYS_UnsubscribeFromEvent(BC_EVENT_NETWORK_LEFT, &nConnectionLeaveAndJoin);
  SYS_SubscribeToEvent(BC_EVENT_NETWORK_LEFT, &nConnectionEventReceiver);
  disconnected();
  /* Restore join network parameters */
  applyJoinParams(applyingJoinParamsDone);

  (void)eventId;
  (void)data;
}

/** BitCloud events observer routine
    \param eventId Event identifier
    \param data Relative data
*/
static void nConnectionObserver(SYS_EventId_t eventId, SYS_EventData_t data)
{
  if(BC_EVENT_REMOVE_DEVICE_RECEIVED == eventId)
  {
    receivedRemoveDevice = true;
  }
  else if (BC_EVENT_NETWORK_LEFT == eventId)
  {
    disconnected();
    /* if network left upon reception of SendremoveDevice */
    if(receivedRemoveDevice)
    {
      receivedRemoveDevice = false;
      resetToFactoryDefaults();
    }
    else
    {
      NWK_LeaveControl_t leaveControl;
      CS_ReadParameter(CS_LEAVE_NON_REJOIN_CONTROL_ID, &leaveControl);
      if(!leaveControl.silent)
      {
        resetToFactoryDefaults();
      }
    }
  }
  else if (BC_EVENT_ACCESS_REQUEST == eventId)
  {
    BcAccessReq_t *const accessReq = (BcAccessReq_t*)data;

    if (BC_BEACON_FILTERING_ACTION == accessReq->action && findFreePan.findFreePanDoneCallback)
    {
      const MAC_BeaconNotifyInd_t *const beaconNtfy = (MAC_BeaconNotifyInd_t*)accessReq->context;
      const NwkBeaconPayload_t *const beaconPayload = (NwkBeaconPayload_t*)beaconNtfy->msdu;
      ExtPanId_t extPanId = beaconPayload->nwkExtendedPanid;

      /* All further beacons processing by NWK is prohibited */
      accessReq->denied = true;

      /* Check generated PanIds for uniqueness */
      if (findFreePan.panId == beaconNtfy->panDescr.coordPANId ||
          0 == memcmp(&findFreePan.extendedPanId, (uint8_t*)&extPanId, sizeof(ExtPanId_t)))
        findFreePan.panExists = true;
    }
  }
}

/** Finds a PAN ID and Extended PAN ID that is unique on all channels of the channel mask, in the RF proximity.

    \param pfDoneCallback Pointer to the function that will be called when the action is done

    This typically takes 240ms per channel in the mask. In extremely rare conditions, it can take longer.
    Only implemented on routers.
*/
void N_ConnectionRouter_FindFreePan_Impl(N_Connection_FindFreePanDone_t pfDoneCallback)
{
  uint8_t ucExtPanID =0;
  uint64_t	ullExtPanID = 0xFFFFFFFFFFFFFFFFULL;

  /* Generate random valid values for PanIds */
  do
  {
    N_Security_GetRandomData((uint8_t*)&findFreePan.extendedPanId, sizeof(ExtPanId_t));
  } while (IS_EQ_64BIT_VALUES(findFreePan.extendedPanId, ucExtPanID) ||
           IS_EQ_64BIT_VALUES(findFreePan.extendedPanId, ullExtPanID));

  do
  {
    N_Security_GetRandomData((uint8_t*)&findFreePan.panId, sizeof(findFreePan.panId));
  } while (0x0000 == findFreePan.panId || 0xffff == findFreePan.panId);

  findFreePan.findFreePanDoneCallback = pfDoneCallback;

  /* Scan through all the channels and check for chosen parameters uniqueness */
  findFreePan.panExists = false;
  findFreePan.nwkDiscoveryRequest.scanChannels =
    (N_DeviceInfo_GetPrimaryChannelMask() | N_DeviceInfo_GetSecondaryChannelMask());

  /* Subscribe to beacon indication events */
  SYS_SubscribeToEvent(BC_EVENT_ACCESS_REQUEST, &nConnectionEventReceiver);

  NWK_NetworkDiscoveryReq(&findFreePan.nwkDiscoveryRequest);
}

/** Callback on discovery done

    \param conf Discovery results
*/
static void NWK_NetworkDiscoveryConf(NWK_NetworkDiscoveryConf_t *conf)
{
  N_ERRH_ASSERT_FATAL((NWK_SUCCESS_STATUS == conf->status) || (NWK_MAC_NO_BEACON_STATUS == conf->status)); /* Network Discovery error */

  /* Unsubscribe from beacon indication events */
  SYS_SubscribeToEvent(BC_EVENT_ACCESS_REQUEST, &nConnectionEventReceiver);

  /* If ExtPanId is not unique - retry */
  if (findFreePan.panExists)
  {
    N_ConnectionRouter_FindFreePan_Impl(findFreePan.findFreePanDoneCallback);
  }
  else
  {
    /* Callback execution */
    findFreePan.findFreePanDoneCallback(findFreePan.panId, (uint8_t*)&findFreePan.extendedPanId);
  }
  (void)conf;
}

/** BC_EVENT_CHILD_JOINED event handler.
    \param eventId - event identifier.
    \param data - event data.
*/
static void nChildAssociated(SYS_EventId_t eventId, SYS_EventData_t data)
{
  if (BC_EVENT_CHILD_ASSOCIATED == eventId)
  {
    ChildInfo_t *info = (ChildInfo_t *)data;
    ExtAddr_t extAddress = info->extAddr;
    childAssociated(info->shortAddr, *(N_Address_Extended_t*)&extAddress);
  }
}

/** Try to find a parent ugently, e.g. after touchlinking (end-device only, no-op on a router).

    Call from N_LinkInitiator on Non-Factory-New devices, just before
    starting to send the dummy unicast messages.
*/
void N_ConnectionRouter_ReconnectUrgent_Impl(void)
{
  /* Nothing to do for Router device */
}

/** Try to find a parent (end-device only, no-op on a router).
*/
void N_ConnectionRouter_Reconnect_Impl(void)
{
  /* Nothing to do for Router device */
}


/** Component internal networkJoined notification
*/
void N_Connection_NetworkJoined(void)
{
  networkJoined();
}

/** Component internal connected notification
*/
void N_Connection_Connected(void)
{
  connected();
}

/** Component internal disconnected notification
*/
void N_Connection_Disconnected(void)
{
  disconnected();
}

/** Set the poll rate to be used when polling. */
void N_ConnectionRouter_SetPollRate_Impl(uint32_t pollIntervalMs)
{
  (void)pollIntervalMs;
}

/** Force to use the application defined poll rate after joining a network. */
void N_ConnectionRouter_ForceNormalPolling_Impl(void)
{
  /* Just a stub for Router */
}

/** Check if the device has a connection to the network (end-device has a parent).
    \returns TRUE if the device is a non-factory-new router, or if it has a parent
*/
bool N_ConnectionRouter_IsConnected_Impl(void)
{
  return !N_DeviceInfo_IsFactoryNew();
}

/** Send Network Leave Request.
    \param rejoin Set to TRUE to indicate the device will rejoin immediately after leaving
    \param removeChildren Set to TRUE to indicate also the children of this device are removed
*/
void N_ConnectionRouter_SendNetworkLeaveRequest_Impl(bool rejoin, bool removeChildren)
{
  ZDO_MgmtLeaveReq_t *zdpLeaveReq = &leaveReq.req.reqPayload.mgmtLeaveReq;
  NWK_LeaveControl_t leaveControl;

  COPY_EXT_ADDR(leaveReq.dstAddress.extAddress,  *MAC_GetExtAddr());
  COPY_EXT_ADDR(zdpLeaveReq->deviceAddr, *MAC_GetExtAddr());
  zdpLeaveReq->rejoin = rejoin;
  zdpLeaveReq->removeChildren = removeChildren;
  zdpLeaveReq->reserved = 0;

  /* Configure post-leave actions */
  leaveControl.cleanRouteTable    = 1U;
  leaveControl.cleanNeighborTable = 1U;
  leaveControl.cleanAddresmap     = 1U;
  leaveControl.cleanKeyPairSet    = 0U;
  leaveControl.cleanBindingTable  = 1U;
  leaveControl.cleanGroupTable    = 1U;
  leaveControl.cleanNwkSecKeys    = 0U;
  leaveControl.silent             = 0U;
  CS_WriteParameter(CS_LEAVE_NON_REJOIN_CONTROL_ID, &leaveControl);

 ZDO_ZdpReq(&leaveReq);
}

/** Calls callback on ResetToFactoryDefaults event
*/

static void resetToFactoryDefaults(void)
{
  N_UTIL_CALLBACK(N_Connection_Callback_t, s_N_Connection_Subscribers, ResetToFactoryDefaults, ());
}

/** Calls callback on childAssociated event
*/
static void childAssociated(uint16_t networkAddress, N_Address_Extended_t ieeeAddress)
{
  N_UTIL_CALLBACK(N_Connection_Callback_t, s_N_Connection_Subscribers, ChildAssociated, (networkAddress, ieeeAddress));
}

/**************************************************************************//**
  \brief After power up delay timer callback.
******************************************************************************/
static void searchNetworkTimerFired(void)
{
  uint32_t channelMask = N_DeviceInfo_GetPrimaryChannelMask();

  if (N_DeviceInfo_TrustCenterMode_Distributed != N_DeviceInfo_GetTrustCenterMode())
    channelMask |= N_DeviceInfo_GetSecondaryChannelMask();

  CS_WriteParameter(CS_CHANNEL_MASK_ID, &channelMask);
  memset((uint8_t*)networkSearchBeacon.extendedPanId, 0x00, sizeof(ExtPanId_t));

  N_LOG_ALWAYS(("Starting discovery for updated networks"));

  // Begin network discovery
  N_Cmi_NetworkDiscovery(channelMask,
                         N_Beacon_Order_60ms,
                         &networkSearchBeacon,
                         N_Connection_AssociateDiscoveryMode_OnlyThisPan,
                         N_DeviceInfo_GetNetworkExtendedPanId(),
                         BEACON_FILTERING_CRITERION_NETWORK_SEARCH,
                         NULL,
                         searchNetworkDiscoveryDone);
}

/**************************************************************************//**
  \brief Searching network completion callback.

  \param[in] result - the result of the network discovery.
******************************************************************************/
static void searchNetworkDiscoveryDone(void)
{
  if (0 != memcmp((uint8_t*)&(uint64_t){0ULL}, (uint8_t*)networkSearchBeacon.extendedPanId,
        sizeof(ExtPanId_t)))
  {
    if (networkSearchBeacon.logicalChannel != N_DeviceInfo_GetNetworkChannel())
    {
      N_LOG_ALWAYS(("Discovery for updated networks completed"));
      N_Radio_SetChannel(networkSearchBeacon.logicalChannel, switchToUpdatedNwkChannelDone);
      return;
    }

    NWK_SetUpdateId(networkSearchBeacon.updateId);
  }

  // Initiate 1500ms timer for device announce
  HAL_StartAppTimer(&devAnnounceTimer); 
  N_LOG_ALWAYS(("Discovery for updated networks completed"));
}

/**************************************************************************//**
  \brief Callback on switching to channel of updated network

  \param[in] conf - confirmation parameters
******************************************************************************/
static void switchToUpdatedNwkChannelDone(MAC_SetConf_t *conf)
{
  N_ERRH_ASSERT_FATAL(MAC_SUCCESS_STATUS == conf->status); /* Channel change error */

  NWK_SetUpdateId(networkSearchBeacon.updateId);
  CS_WriteParameter(CS_NWK_LOGICAL_CHANNEL_ID, &networkSearchBeacon.logicalChannel);
  NWK_SetLogicalChannel(networkSearchBeacon.logicalChannel);

  // Initiate 1500ms timer for device announce
  HAL_StartAppTimer(&devAnnounceTimer); 
}

/**************************************************************************//**
  \brief dev annnounce timer callback.
******************************************************************************/
void devAnnounceTimerFired(void)
{
  N_Zdp_SendDeviceAnnounce();
}

/* eof N_Connection.c */
#endif // !defined(ZIGBEE_END_DEVICE)