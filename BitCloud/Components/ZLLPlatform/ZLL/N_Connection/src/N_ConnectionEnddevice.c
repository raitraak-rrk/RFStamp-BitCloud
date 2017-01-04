/**************************************************************************//**
  \file N_ConnectionEnddevice.c

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

#if defined(ZIGBEE_END_DEVICE)
/******************************************************************************
                    Includes section
******************************************************************************/
#include <N_ConnectionEndDevice_Bindings.h>
#include <N_DeviceInfo_Bindings.h>
#include <N_Connection.h>
#include <N_ConnectionEndDevice_Init.h>
#include <N_Connection_Internal.h>
#include <N_DeviceInfo.h>
#include <N_ErrH.h>
#include <N_Cmi.h>
#include <N_AddressManager.h>
#include <N_AddressManager_Bindings.h>

#include <configServer.h>
#include <zdo.h>
#include <sysUtils.h>
#include <N_Zdp.h>

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
  /** Service field, used for queue support */
  QueueElement_t next;

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
static void connected(void);
static void disconnected(void);
static void networkJoined(void);
static void nConnectionObserver(SYS_EventId_t eventId, SYS_EventData_t data);
static void NWK_NetworkDiscoveryConf(NWK_NetworkDiscoveryConf_t *conf);
static void resetToFactoryDefaults(void);
static void applyJoinParams(ApplyingJoinParamsDone_t applyDoneCallback);
static void applyingJoinParamsDone(void);
static void silentLeaveNetwork(void);
static void silentLeaveHandler(SYS_EventId_t eventId, SYS_EventData_t data);
static void leaveNetworkAndJoin(void);
static void nConnectionLeaveAndJoinObserver(SYS_EventId_t eventId, SYS_EventData_t data);
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
  .method = NWK_JOIN_VIA_REJOIN,
  .annce = true,
  .discoverNetworks = true,
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

static SYS_EventReceiver_t nConnectionSilentLeaveEventReceiver = {
  .func = silentLeaveHandler
};

/* Join network request parameters */
N_Connection_JoinNetworkParams_t joinNetworkParams;

/* BitCloud events receiver */
static SYS_EventReceiver_t nConnectionEventReceiver = {.func = nConnectionObserver};

static N_Task_Id_t s_taskId = N_TASK_ID_NONE;
N_UTIL_CALLBACK_DECLARE(N_Connection_Callback_t, s_N_Connection_Subscribers, N_CONNECTION_MAX_SUBSCRIBERS);
/* Application poll rate */
uint32_t pollRate = CS_INDIRECT_POLL_RATE;

/* Captures application poll rate irrespective of being zero or non-zero. This
   is to be used in N_LowPower for voting for fast-polling */
uint32_t appPollRate = CS_INDIRECT_POLL_RATE;

/* edConnected status */
static bool edConnected = false;
static bool receivedLeaveCmd = false;
static bool reconnectUrgentOngoing = false;

static uint8_t postponedInterPanModeChannel;
static N_Connection_SetInterPanModeDone_t postponedInterPanModeCb;
static SYS_EventReceiver_t nConnectionLeaveAndJoin = {.func = nConnectionLeaveAndJoinObserver};

/******************************************************************************
                   Extern section
******************************************************************************/
extern bool isSetPollRateAllowed;

/***********************************************************************************
                    Implementation section
***********************************************************************************/
/** Initialises the component
*/
void N_ConnectionEndDevice_Init(void)
{
  /* Device type configuration */
  //CS_WriteParameter(CS_DEVICE_TYPE_ID, &(DeviceType_t){DEVICE_TYPE_ROUTER});

  s_taskId = N_Task_GetIdFromEventHandler(N_ConnectionEndDevice_EventHandler);
  SYS_SubscribeToEvent(BC_EVENT_NETWORK_LOST, &nConnectionEventReceiver);
  SYS_SubscribeToEvent(BC_EVENT_NETWORK_STARTED, &nConnectionEventReceiver);
  SYS_SubscribeToEvent(BC_EVENT_LEAVE_COMMAND_RECEIVED, &nConnectionEventReceiver);
  SYS_SubscribeToEvent(BC_EVENT_NETWORK_LEFT, &nConnectionEventReceiver);

  CS_ReadParameter(CS_INDIRECT_POLL_RATE_ID, &pollRate);
}

/** Subscribes callbacks to the component internal events
*/
void N_ConnectionEndDevice_Subscribe_Impl(const N_Connection_Callback_t* pSubscriber)
{
  N_ERRH_ASSERT_FATAL(s_taskId != N_TASK_ID_NONE);    // check if initialized
  N_UTIL_CALLBACK_SUBSCRIBE(N_Connection_Callback_t, s_N_Connection_Subscribers, pSubscriber);
  if(N_Connection_IsConnected() && (pSubscriber->Connected != NULL))
  {
    pSubscriber->Connected();
  }
}

/** The event handler for the component's task
*/
bool N_ConnectionEndDevice_EventHandler(N_Task_Event_t evt)
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
void N_ConnectionEndDevice_JoinNetwork_Impl(N_Address_Extended_t extendedPanId,
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


/** Check if the device has a connection to the network (end-device has a parent).
    \returns TRUE if the device is a non-factory-new router, or if it has a parent
*/
bool N_ConnectionEndDevice_IsConnected_Impl(void)
{
  return edConnected;
}


/** Send Network Leave Request.
    \param rejoin Set to TRUE to indicate the device will rejoin immediately after leaving
    \param removeChildren Set to TRUE to indicate also the children of this device are removed
*/
void N_ConnectionEndDevice_SendNetworkLeaveRequest_Impl(bool rejoin, bool removeChildren)
{
  ZDO_MgmtLeaveReq_t *zdpLeaveReq = &leaveReq.req.reqPayload.mgmtLeaveReq;
  NWK_LeaveControl_t leaveControl;

  COPY_EXT_ADDR(leaveReq.dstAddress.extAddress,  *MAC_GetExtAddr());
  COPY_EXT_ADDR(zdpLeaveReq->deviceAddr, *MAC_GetExtAddr());
  zdpLeaveReq->rejoin = rejoin;
  zdpLeaveReq->removeChildren = removeChildren;
  zdpLeaveReq->reserved = 0;

  /* Configure post-leave actions */
  leaveControl.cleanRouteTable = 1U;
  leaveControl.cleanNeighborTable = 1U;
  leaveControl.cleanAddresmap = 1U;
  leaveControl.cleanKeyPairSet = 0U;
  leaveControl.cleanBindingTable = 1U;
  leaveControl.cleanGroupTable = 1U;
  leaveControl.cleanNwkSecKeys = 0U;
  leaveControl.silent = 0U;

  CS_WriteParameter(CS_LEAVE_NON_REJOIN_CONTROL_ID, &leaveControl);

  ZDO_ZdpReq(&leaveReq);
}

/** Callback on applying join parameters done
*/
static void applyingJoinParamsDone(void)
{
  N_ERRH_ASSERT_FATAL(NULL != joinNetworkDoneCallback);
    
  /* Synchornize network paramters with DeviceInfobase */  
  N_DeviceInfo_SetNetworkPanId(joinNetworkParams.panId);
  N_DeviceInfo_SetNetworkExtendedPanId(joinNetworkParams.extendedPanId);
  N_DeviceInfo_SetNetworkChannel(joinNetworkParams.channel);
  N_DeviceInfo_SetNetworkAddress(joinNetworkParams.networkAddress);
  
  /* Set factory new as false since the parameters are updated*/
  N_DeviceInfo_SetFactoryNew(false);

  joinNetworkDoneCallback(N_Connection_Result_Success);
  networkJoined();
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
    connected();
  }
  /* Unsubscribe from beacon indication events */
  SYS_UnsubscribeFromEvent(BC_EVENT_ACCESS_REQUEST, &nConnectionEventReceiver);
  joinNetworkDoneCallback = NULL;
  reconnectUrgentOngoing = false;
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
  leaveControl.silent             = 1U;
  CS_WriteParameter(CS_LEAVE_NON_REJOIN_CONTROL_ID, &leaveControl);

  SYS_SubscribeToEvent(BC_EVENT_NETWORK_LEFT, &nConnectionLeaveAndJoin);
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
  
  edConnected = true;
    
  N_UTIL_CALLBACK(N_Connection_Callback_t, s_N_Connection_Subscribers, Connected, ());
}

/** Calls callback on Disconnected event
*/
static void disconnected(void)
{
  edConnected = false;
  N_UTIL_CALLBACK(N_Connection_Callback_t, s_N_Connection_Subscribers, Disconnected, ());
}

/** Calls callback on NetworkJoined event
*/
static void networkJoined(void)
{
  N_UTIL_CALLBACK(N_Connection_Callback_t, s_N_Connection_Subscribers, NetworkJoined, ());
}

/** Postpones setting interPan mode to ON state.
*/
void N_Connection_PostponeInterPanMode(uint8_t channel, N_Connection_SetInterPanModeDone_t pConfirmCallback)
{
  postponedInterPanModeChannel = channel;
  postponedInterPanModeCb = pConfirmCallback;
}

/** Processes postponed setting interPan mode to ON state if required.
*/
void N_Connection_ProcessPostponedInterPanMode(void)
{
  if (postponedInterPanModeCb)
    N_Connection_SetTargetInterPanModeOn(postponedInterPanModeChannel, postponedInterPanModeCb);
  postponedInterPanModeCb = NULL;
}

/** BC_EVENT_NETWORK_LEFT event handler.
    \param eventId - event identifier.
    \param data - event data.
*/
static void nConnectionLeaveAndJoinObserver(SYS_EventId_t eventId, SYS_EventData_t data)
{
  NWK_LeaveControl_t leaveControl;
  SYS_UnsubscribeFromEvent(BC_EVENT_NETWORK_LEFT, &nConnectionLeaveAndJoin);
  disconnected();
  CS_ReadParameter(CS_LEAVE_NON_REJOIN_CONTROL_ID, &leaveControl);
  if(leaveControl.silent)
  {
    /* Restore join network parameters */
    applyJoinParams(applyingJoinParamsDone);        
  }
  (void)eventId;
  (void)data;
}

/** BitCloud events observer routine
    \param eventId Event identifier
    \param data Relative data
*/
static void nConnectionObserver(SYS_EventId_t eventId, SYS_EventData_t data)
{
  if (BC_EVENT_NETWORK_LOST == eventId)
    disconnected();
  else if (BC_EVENT_NETWORK_STARTED == eventId)
    connected();
  else if(BC_EVENT_LEAVE_COMMAND_RECEIVED == eventId)
  {
    receivedLeaveCmd = true;
    SYS_SubscribeToEvent(BC_EVENT_NETWORK_LEFT, &nConnectionEventReceiver);
  }
  else if (BC_EVENT_NETWORK_LEFT == eventId)
  {
    /* if network left upon reception of leave cmd */
    if(receivedLeaveCmd)
    {
      receivedLeaveCmd = false;
      disconnected();
      resetToFactoryDefaults();
      SYS_UnsubscribeFromEvent(BC_EVENT_NETWORK_LEFT, &nConnectionEventReceiver);
    }
    else
    {
      NWK_LeaveControl_t leaveControl;
      CS_ReadParameter(CS_LEAVE_NON_REJOIN_CONTROL_ID, &leaveControl);
      if(!leaveControl.silent)
      {
        disconnected();
        resetToFactoryDefaults();
      }
    }
  }
  else if (BC_EVENT_ACCESS_REQUEST == eventId)
  {
    BcAccessReq_t *const accessReq = (BcAccessReq_t*)data;
    if (BC_BEACON_FILTERING_ACTION == accessReq->action)
    {
      const MAC_BeaconNotifyInd_t *const beaconNtfy = (MAC_BeaconNotifyInd_t*)accessReq->context;
      const NwkBeaconPayload_t *const beaconPayload = (NwkBeaconPayload_t*)beaconNtfy->msdu;
      ExtPanId_t recvdExtPanId = beaconPayload->nwkExtendedPanid;
      ExtPanId_t extPanId;

      CS_ReadParameter(CS_EXT_PANID_ID, &extPanId);

      if (extPanId != recvdExtPanId)
        accessReq->denied = true;

      if (findFreePan.findFreePanDoneCallback)
	  {
	    /* All further beacons processing by NWK is prohibited */
        accessReq->denied = true;

        /* Check generated PanIds for uniqueness */
        if (findFreePan.panId == beaconNtfy->panDescr.coordPANId ||
            0 == memcmp(&findFreePan.extendedPanId, (uint8_t*)&recvdExtPanId, sizeof(ExtPanId_t)))
          findFreePan.panExists = true;
      }
    }
  }
}

/** Finds a PAN ID and Extended PAN ID that is unique on all channels of the channel mask, in the RF proximity.

    \param pfDoneCallback Pointer to the function that will be called when the action is done

    This typically takes 240ms per channel in the mask. In extremely rare conditions, it can take longer.
    Only implemented on EndDevices.
*/
void N_ConnectionEndDevice_FindFreePan_Impl(N_Connection_FindFreePanDone_t pfDoneCallback)
{
  uint8_t ucExtPanID =0;
  uint64_t ullExtPanID = 0xFFFFFFFFFFFFFFFFULL;

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
  SYS_UnsubscribeFromEvent(BC_EVENT_ACCESS_REQUEST, &nConnectionEventReceiver);

  /* If ExtPanId is not unique - retry */
  if (findFreePan.panExists)
  {
    N_ConnectionEndDevice_FindFreePan_Impl(findFreePan.findFreePanDoneCallback);
  }
  else
  {
    /* Callback execution */
    findFreePan.findFreePanDoneCallback(findFreePan.panId, (uint8_t*)&findFreePan.extendedPanId);
  }
}

/** Slient Leave network routine.
*/
static void silentLeaveNetwork(void)
{
  ZDO_MgmtLeaveReq_t *zdpLeaveReq = &leaveReq.req.reqPayload.mgmtLeaveReq;
  NWK_LeaveControl_t leaveControl;

  COPY_EXT_ADDR(leaveReq.dstAddress.extAddress,  *MAC_GetExtAddr());
  COPY_EXT_ADDR(zdpLeaveReq->deviceAddr, *MAC_GetExtAddr());
  zdpLeaveReq->rejoin         = 0U;
  zdpLeaveReq->removeChildren = 0U;
  zdpLeaveReq->reserved       = 0U;

  /* Configure post-leave actions */
  leaveControl.cleanRouteTable    = 1U;
  leaveControl.cleanNeighborTable = 1U;
  leaveControl.cleanAddresmap     = 0U;
  leaveControl.cleanKeyPairSet    = 0U;
  leaveControl.cleanBindingTable  = 0U;
  leaveControl.cleanGroupTable    = 0U;
  leaveControl.cleanNwkSecKeys    = 0U;
  leaveControl.silent             = 1U;
  CS_WriteParameter(CS_LEAVE_NON_REJOIN_CONTROL_ID, &leaveControl);

  SYS_SubscribeToEvent(BC_EVENT_NETWORK_LEFT, &nConnectionSilentLeaveEventReceiver);
  ZDO_ZdpReq(&leaveReq);
}

/** BC_EVENT_NETWORK_LEFT event handler.
    \param eventId - event identifier.
    \param data - event data.
*/
static void silentLeaveHandler(SYS_EventId_t eventId, SYS_EventData_t data)
{
  SYS_UnsubscribeFromEvent(BC_EVENT_NETWORK_LEFT, &nConnectionSilentLeaveEventReceiver);
  
  if (joinNetworkDoneCallback)
    ZDO_IncreaseJoinAttemptsAmount(BEACON_AMOUNT);
  else
  {
    CS_WriteParameter(CS_JOIN_CONTROL_ID, &joinControl);
    ZDO_StartNetworkReq(&startNetworkReq);
  }
  (void)eventId;
  (void)data;
}

/** Returns the state of Reconnect urgent.
*/
bool N_Connection_Is_ReconnectUrgentOngoing_Impl(void)
{
  return reconnectUrgentOngoing;
}

/** Try to find a parent ugently, e.g. after touchlinking (end-device only, no-op on a EndDevice).
    Call from N_LinkInitiator on Non-Factory-New devices, just before
    starting to send the dummy unicast messages.
*/
void N_ConnectionEndDevice_ReconnectUrgent_Impl(void)
{
  if (!N_Connection_IsConnected() && !reconnectUrgentOngoing)
  {
    reconnectUrgentOngoing = true;
    if (ZDO_IN_NETWORK_STATUS == ZDO_GetNwkStatus())
    {
      silentLeaveNetwork();
    }
    else
    {    
      if (joinNetworkDoneCallback)
        ZDO_IncreaseJoinAttemptsAmount(BEACON_AMOUNT);
      else
      {
        /* Subscribe to beacon indication events */
        SYS_SubscribeToEvent(BC_EVENT_ACCESS_REQUEST, &nConnectionEventReceiver);
        CS_WriteParameter(CS_JOIN_CONTROL_ID, &joinControl);
        ZDO_StartNetworkReq(&startNetworkReq);
      }
    }
  }
  else if(N_Connection_IsConnected())
  {
    N_Zdp_SendDeviceAnnounce();
  }
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

/** Set the poll rate to be used when polling.
    \param pollIntervalMs The poll interval in milliseconds, or 0 to disable polling.

    Polling is an end-device-only feature. On routers, it is a dummy function.
    When the poll rate is never set, the default of 1000ms is used.
*/
void N_ConnectionEndDevice_SetPollRate_Impl(uint32_t pollIntervalMs)
{
  pollRate = pollIntervalMs;
  appPollRate = pollIntervalMs;
  
  if (isSetPollRateAllowed)
  {
    ZDO_StopSyncReq();
    if (pollIntervalMs)
    {
      CS_WriteParameter(CS_INDIRECT_POLL_RATE_ID, &pollRate);
      ZDO_StartSyncReq();
    }
  }
}

/** Force to use the application defined poll rate after joining a network.

    When an end-device is touchlinked, the pollrate is normally set to 300 milliseconds
    for a period of 5 seconds. This is to ensure that the end-device will receive the
    "communication check"-messages from the other device. The application is able
    to stop this fast polling mode by calling this function.
    In this case the application is responsible for the correct poll rate.

    Polling is an end-device-only feature. On routers, it is a dummy function.
*/
void N_ConnectionEndDevice_ForceNormalPolling_Impl(void)
{
  N_Connection_SetPollRate((uint32_t)pollRate);
}

/** Calls callback on ResetToFactoryDefaults event
*/

static void resetToFactoryDefaults(void)
{
  N_UTIL_CALLBACK(N_Connection_Callback_t, s_N_Connection_Subscribers, ResetToFactoryDefaults, ());
}


/** Performs network discovery and channel change if the network
    which this device is assotiated with works on other channel.
    Actual only for router.
*/
void N_Connection_SearchNetwork(void)
{
  /* Do nothing. */
}

/* eof N_Connection.c */
#endif // defined(ZIGBEE_END_DEVICE)

