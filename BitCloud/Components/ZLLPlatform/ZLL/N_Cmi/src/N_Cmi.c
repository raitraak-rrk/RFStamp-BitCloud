/**************************************************************************//**
  \file N_Cmi.c

  \brief Implementation of CMI component for BitCloud stack.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    22.08.12 A. Razinkov - created
******************************************************************************/

/******************************************************************************
                    Includes section
******************************************************************************/
#include <nwkSecurity.h>
#include <nwkNeighbor.h>
#include <configServer.h>
#include <pdsDataServer.h>
#include <nwk.h>
#include <zdo.h>

#include <N_Cmi_Bindings.h>
#include <N_Cmi.h>
#include <N_Cmi_Init.h>
#include <N_DeviceInfo.h>
#include <N_ErrH.h>
#include <N_Radio_Internal.h>
#include <N_Connection_Internal.h>
#include <N_Connection_Private.h>
#include <N_AddressManager.h>
#include <N_AddressManager_Bindings.h>

/******************************************************************************
                    Defines section
******************************************************************************/
#define COMPID "N_Cmi"

#define N_CMI_DEFAULT_NWK_UPDATE_ID 0x00u
#define N_CMI_DEFAULT_NWK_ADDR      0xfff7u
#define N_CMI_DEFAULT_PAN_ID        0xfffeu

#ifndef N_CMI_DEFAULT_TX_POWER
#define N_CMI_DEFAULT_TX_POWER      0x03
#endif

#ifndef N_CMI_DEFAULT_CH26_MAX_TX_POWER
#define N_CMI_DEFAULT_CH26_MAX_TX_POWER      (-12) //dBm
#endif

#ifndef N_CMI_MIN_TREE_LINK_COST
#define N_CMI_MIN_TREE_LINK_COST 6u
#endif

#ifndef N_CMI_LQI_RANDOM_BEACON_SELECTION_THRESHOLD
#define N_CMI_LQI_RANDOM_BEACON_SELECTION_THRESHOLD 44u
#endif

/* Macro for setting MAC attributes. */
#define SET_MAC_ATTR(id, name, value) \
  nCmiMacInit.mac.set.attrId.macPibId = (id); \
  nCmiMacInit.mac.set.attrValue.macPibAttr.name = (value);

#define SET_PHY_ATTR(id, name, value) \
  nCmiMacInit.mac.set.attrId.phyPibId = (id); \
  nCmiMacInit.mac.set.attrValue.phyPibAttr.name = (value);

#define UPDATE_ID_OVERFLOW_LIMIT  200U
/******************************************************************************
                    Types section
******************************************************************************/
/** MAC layer initialization states */
typedef enum _N_Cmi_MacInitState_t
{
  N_CMI_MAC_INIT_IDLE_STATE,
  N_CMI_MAC_INIT_SET_SHORT_ADDRESS_STATE,
  N_CMI_MAC_INIT_SET_PAN_ID_STATE,
  N_CMI_MAC_INIT_SET_CHANNEL_STATE,
  N_CMI_MAC_INIT_SET_EXT_ADDR_STATE,
  N_CMI_MAC_INIT_SET_RX_ON_WHEN_IDLE_STATE,
  N_CMI_MAC_INIT_SET_TX_POWER_STATE,
  N_CMI_MAC_INIT_SET_RX_ENABLE_STATE,
} N_Cmi_MacInitState_t;

/* Memory allocated for MAC initialization */
typedef struct _N_Cmi_MacInit_t
{
  N_Cmi_MacInitState_t state;
  union
  {
    MAC_SetReq_t      set;
    MAC_GetReq_t      get;
    MAC_RxEnableReq_t rxEnable;
  } mac;
  N_Cmi_InitMacLayerDone_t initMacLayerDoneCallback;
} N_Cmi_MacInit_t;

typedef bool (*N_Cmi_BeaconFilter_t)(const MAC_BeaconNotifyInd_t *const beaconNtfy);

typedef struct _N_Cmi_NwkDiscovery_t
{
  N_Cmi_NetworkDiscoveryDone_t pfDoneCallback;
  N_Cmi_BeaconIndication_t pfBeaconReceivedCallback;
  N_Beacon_t* pBeacon;
  N_Connection_AssociateDiscoveryMode_t mode;
  N_Address_ExtendedPanId_t* extendedPanId;
  NWK_NetworkDiscoveryReq_t request;
  N_Cmi_BeaconFilter_t isBeaconAccepted;
  N_Cmi_BeaconFilter_t isBestBeacon;
} N_Cmi_NwkDiscovery_t;

/******************************************************************************
                    Prototypes section
******************************************************************************/
static void nCmiMacSetConf(MAC_SetConf_t *conf);
static void nCmiRxEnableReq(void);
static void nCmiRxEnableConf(MAC_RxEnableConf_t *conf);
static void nCmiCommissionAsNfn(ZDO_StartNetworkConf_t* conf);
static void nCmiAssociationConf(ZDO_StartNetworkConf_t* conf);
static void nCmiObserver(SYS_EventId_t eventId, SYS_EventData_t data);
static void NWK_NetworkDiscoveryConf(NWK_NetworkDiscoveryConf_t *conf);
static bool nCmiBeaconIsAccepted(const MAC_BeaconNotifyInd_t *const beaconNtfy);
static bool nCmiIsBestBeacon(const MAC_BeaconNotifyInd_t *const beaconNtfy);
static void nCmiPickBeacon(const MAC_BeaconNotifyInd_t *const beaconNtfy);
static void setDistributedTcLinkKey(void);
static void setLinkKeyAsPrimary(void);
static void N_Cmi_SetBeaconFilter(N_Cmi_BeaconFilteringCriterion_t criterion);

#ifdef ZIGBEE_END_DEVICE
static void nCmiRestorePollRateTimerFired(void);
static bool nCmiReconnectIsBeaconAccepted(const MAC_BeaconNotifyInd_t *const beaconNtfy);
static void nCmiRestorePotentialParent(void);
#else
static bool nCmiNFNRouterIsBeaconAccepted(const MAC_BeaconNotifyInd_t *const beaconNtfy);
static bool nCmiNFNRouterIsBestBeacon(const MAC_BeaconNotifyInd_t *const beaconNtfy);
#endif /* ZIGBEE_END_DEVICE */
/******************************************************************************
                    Static variables section
******************************************************************************/
/* MAC layer initialization memory */
static N_Cmi_MacInit_t nCmiMacInit;

#ifdef ZIGBEE_END_DEVICE
/* Timer to restore default poll rate */
static HAL_AppTimer_t  pollRateTimer =
{
  .mode     = TIMER_ONE_SHOT_MODE,
  .callback = nCmiRestorePollRateTimerFired
};
#endif /* ZIGBEE_END_DEVICE */

/* Request params for ZDO_StartNetworkReq */
static ZDO_StartNetworkReq_t startNetworkReq;

/* Params for NWK NetworkDiscovery */
static N_Cmi_NwkDiscovery_t nwkDiscovery =
{
  .request =
  {
    .NWK_NetworkDiscoveryConf = NWK_NetworkDiscoveryConf,
  },
  .isBeaconAccepted = NULL,
  .isBestBeacon = NULL,
};

static N_Cmi_JoinDone_t associationDoneCallback;

static N_Cmi_SetZllLinkKeyAsPrimaryDone_t setLinkKeyAsPrimaryDoneCallback;

/* BitCloud events receiver */
static SYS_EventReceiver_t bcEventReceiver = { .func = nCmiObserver};

static uint8_t zllLinkKey[SECURITY_KEY_SIZE];

extern uint32_t pollRate;

extern bool isSetPollRateAllowed;

#if defined(ZIGBEE_END_DEVICE)
static uint8_t activeKeySeqNum;
#endif

/***********************************************************************************
                    Implementation section
***********************************************************************************/
/** Initializes BitCloud network parameters
*/
void N_Cmi_Init(void)
{
  bool factoryNew = N_DeviceInfo_IsFactoryNew();

#ifndef BC_PDS_DISABLE_AUTOSAVING
  /* Restore BitCloud parameters if NFN */
  if (factoryNew ||
    !PDS_IsAbleToRestore(BC_ALL_MEMORY_MEM_ID) || !PDS_Restore(BC_ALL_MEMORY_MEM_ID))
  /* Parameters couldn't be restored - continue as FN */
  {
    PDS_InitItems();
    N_DeviceInfo_SetFactoryNew(true);
    factoryNew = true;
  }
#endif /* BC_PDS_DISABLE_AUTOSAVING */

  /* Configure stack to default if FN */
  if (factoryNew)
  {
    DeviceType_t deviceType = N_DeviceInfo_IsEndDevice() ? DEVICE_TYPE_END_DEVICE :
      DEVICE_TYPE_ROUTER;
    CS_WriteParameter(CS_DEVICE_TYPE_ID, &deviceType);

    uint8_t joinAttempts = 3;
    CS_WriteParameter(CS_ZDO_JOIN_ATTEMPTS_ID, &joinAttempts);

    bool rxOnWhenIdle = (DEVICE_TYPE_ROUTER == deviceType) ? true : false;
    CS_WriteParameter(CS_RX_ON_WHEN_IDLE_ID, &rxOnWhenIdle);
    if (rxOnWhenIdle)
      CS_WriteParameter(CS_NWK_END_DEVICE_MAX_FAILURES_ID, &(uint8_t){4});

    uint8_t channel = N_DeviceInfo_GetChannelForIndex(0u, N_DeviceInfo_GetPrimaryChannelMask());
    CS_WriteParameter(CS_NWK_LOGICAL_CHANNEL_ID, &channel);
    NWK_SetLogicalChannel(channel);

    NWK_SetUpdateId(N_CMI_DEFAULT_NWK_UPDATE_ID);

    CS_WriteParameter(CS_NWK_PANID_ID, &(PanId_t){N_CMI_DEFAULT_PAN_ID});
    CS_WriteParameter(CS_NWK_PREDEFINED_PANID_ID, &(bool){true});

    CS_WriteParameter(CS_NWK_ADDR_ID, &(ShortAddr_t){N_CMI_DEFAULT_NWK_ADDR});
    CS_WriteParameter(CS_NWK_UNIQUE_ADDR_ID, &(bool){false});

    CS_WriteParameter(CS_RF_TX_POWER_ID, &(int8_t){N_CMI_DEFAULT_TX_POWER});
    CS_WriteParameter(CS_RF_MAX_CH26_TX_POWER_ID, &(int8_t){N_CMI_DEFAULT_CH26_MAX_TX_POWER});

    CS_WriteParameter(CS_SCAN_DURATION_ID, &(uint8_t){4});
    CS_WriteParameter(CS_ZDO_JOIN_INTERVAL_ID, &(uint32_t){350});

    CS_WriteParameter(CS_ZDO_SECURITY_STATUS_ID, &(uint8_t){PRECONFIGURED_TRUST_CENTER_LINK_KEY});
    CS_WriteParameter(CS_APS_TRUST_CENTER_ADDRESS_ID, &(ExtAddr_t){APS_DISTRIBUTED_TC_ADDRESS});
  }
  /* Synchronize DeviceInfo if NFN */
  else
  {
    N_DeviceInfo_SynchronizationParameters_t params;
    ExtPanId_t extPanId;

    CS_ReadParameter(CS_NWK_LOGICAL_CHANNEL_ID, &params.channel);
    CS_ReadParameter(CS_NWK_PANID_ID, &params.panId);
    CS_ReadParameter(CS_NWK_EXT_PANID_ID, &extPanId);
    params.extPanId = (uint8_t*)&extPanId;
    CS_ReadParameter(CS_NWK_ADDR_ID, &params.nwkAddr);

    N_DeviceInfo_SynchronizeParameters(&params);
  }

  /* Disable Association permission */
  CS_WriteParameter(CS_PERMIT_DURATION_ID, &(uint8_t){0});
  /* Configure Security policy to disable APS layer security for few commands */
  APS_UpdateSecurityPolicy(APS_UPDATE_DEVICE_SPID, 0x1F);
  APS_UpdateSecurityPolicy(APS_REMOVE_DEVICE_SPID, 0x1F);
  APS_UpdateSecurityPolicy(APS_REQUEST_KEY_SPID, 0x1F);
  APS_UpdateSecurityPolicy(APS_SWITCH_KEY_SPID, 0x1F);
}

/** Initializes BitCloud MAC layer parameters
*/
void N_Cmi_InitMacLayer_Impl(N_Cmi_InitMacLayerDone_t doneCallback)
{
  ShortAddr_t nwkShortAddr;

  N_ERRH_ASSERT_FATAL(!nCmiMacInit.initMacLayerDoneCallback); /* Unexpected function call */

  nCmiMacInit.initMacLayerDoneCallback = doneCallback;

  CS_ReadParameter(CS_NWK_ADDR_ID, &nwkShortAddr);
  SET_MAC_ATTR(MAC_PIB_SHORT_ADDR_ID, shortAddr, nwkShortAddr);
  nCmiMacInit.mac.set.MAC_SetConf = nCmiMacSetConf;
  MAC_SetReq(&nCmiMacInit.mac.set);
  nCmiMacInit.state = N_CMI_MAC_INIT_SET_SHORT_ADDRESS_STATE;
}

/** MLME-SET confirm callback function.
    \param conf Pointer to MLME-SET confirm primitive's parameters.
*/
static void nCmiMacSetConf(MAC_SetConf_t *conf)
{
  union
  {
    uint8_t  u8;
    uint16_t u16;
    uint64_t u64;
  } attribute;

  N_ERRH_ASSERT_FATAL(MAC_SUCCESS_STATUS == conf->status); /* invalid MAC parameter value */

  switch (nCmiMacInit.state)
  {
    case N_CMI_MAC_INIT_SET_SHORT_ADDRESS_STATE:
      CS_ReadParameter(CS_NWK_PANID_ID, &attribute.u16);
      SET_MAC_ATTR(MAC_PIB_PANID_ID, panId, attribute.u16);
      nCmiMacInit.state = N_CMI_MAC_INIT_SET_PAN_ID_STATE;
      break;
    case N_CMI_MAC_INIT_SET_PAN_ID_STATE:
      CS_ReadParameter(CS_NWK_LOGICAL_CHANNEL_ID, &attribute.u8);
      nCmiMacInit.state = N_CMI_MAC_INIT_SET_CHANNEL_STATE;
      N_Radio_SetChannel(attribute.u8, nCmiMacSetConf);
      return;
    case N_CMI_MAC_INIT_SET_CHANNEL_STATE:
      CS_ReadParameter(CS_RX_ON_WHEN_IDLE_ID, &attribute.u8);
      SET_MAC_ATTR(MAC_PIB_RX_ON_WHEN_IDLE_ID, rxOnWhenIdle, attribute.u8);
      nCmiMacInit.state = N_CMI_MAC_INIT_SET_EXT_ADDR_STATE;
      break;
    case N_CMI_MAC_INIT_SET_EXT_ADDR_STATE:
      CS_ReadParameter(CS_UID_ID, &attribute.u64);
      nCmiMacInit.mac.set.attrId.macPibId = MAC_PIB_EXT_ADDR_ID;
      COPY_64BIT_VALUE(nCmiMacInit.mac.set.attrValue.macPibAttr.extAddr, attribute.u64);
      nCmiMacInit.state = N_CMI_MAC_INIT_SET_RX_ON_WHEN_IDLE_STATE;
      break;
    case N_CMI_MAC_INIT_SET_RX_ON_WHEN_IDLE_STATE:
      CS_ReadParameter(CS_RF_TX_POWER_ID, &attribute.u8);
      SET_PHY_ATTR(PHY_PIB_TRANSMIT_POWER_ID, transmitPower, attribute.u8);
      nCmiMacInit.state = N_CMI_MAC_INIT_SET_TX_POWER_STATE;
      break;
    case N_CMI_MAC_INIT_SET_TX_POWER_STATE:
      nCmiRxEnableReq();
    return;
    default:
      N_ERRH_ASSERT_FATAL(0); /* Unknown MAC init state */
    return;
  }
  nCmiMacInit.mac.set.MAC_SetConf = nCmiMacSetConf;
  MAC_SetReq(&nCmiMacInit.mac.set);
}

/** MLME-RX_ENABLE attribute setting
*/
static void nCmiRxEnableReq(void)
{
  bool rxOnWhenIdle;
  CS_ReadParameter(CS_RX_ON_WHEN_IDLE_ID, &rxOnWhenIdle);

  nCmiMacInit.mac.rxEnable.rxOnDuration = rxOnWhenIdle;
  nCmiMacInit.mac.rxEnable.MAC_RxEnableConf = nCmiRxEnableConf;
  nCmiMacInit.state = N_CMI_MAC_INIT_SET_RX_ENABLE_STATE;
  MAC_RxEnableReq(&nCmiMacInit.mac.rxEnable);
}

/** MLME-RX_ENABLE confirm callback function.
    \param conf Pointer to MLME-RX-ENABLE confirm primitive's parameters.
*/
static void nCmiRxEnableConf(MAC_RxEnableConf_t *conf)
{
  N_ERRH_ASSERT_FATAL(MAC_SUCCESS_STATUS == conf->status); /* MAC rx enabling failed */

  /* Continue with link key setting if FN */
  if (N_DeviceInfo_IsFactoryNew())
  {
    N_Security_GetDistributedTrustCenterLinkKey(zllLinkKey, setDistributedTcLinkKey);
    /* Restore security counter, if applicable */
    PDS_Restore(NWK_SECURITY_COUNTERS_ITEM_ID);
  }
  /* Reenter the network if NFN */
  else
  {
    NWK_JoinControl_t joinControl =
    {
#if !defined(ZIGBEE_END_DEVICE)
      .method = NWK_JOIN_VIA_COMMISSIONING,
      .annce = false,
#else
      .method = NWK_JOIN_VIA_REJOIN,
      .annce = true,
#endif /* ZIGBEE_END_DEVICE */
      .discoverNetworks = false,
      .secured = true,
      .clearNeighborTable = false
    };

#ifndef BC_PDS_DISABLE_AUTOSAVING
    /* Configure PDS to store BitCloud parameters by internal events */
    PDS_StoreByEvents(BC_ALL_MEMORY_MEM_ID);
#endif /* !BC_PDS_DISABLE_AUTOSAVING*/

    startNetworkReq.ZDO_StartNetworkConf = nCmiCommissionAsNfn;
    CS_WriteParameter(CS_JOIN_CONTROL_ID, &joinControl);
    ZDO_StartNetworkReq(&startNetworkReq);
  }
}

/** Callback on ZLL preconfigured TC link key reading
*/
static void setDistributedTcLinkKey(void)
{
  APS_SetLinkKey(&(ExtAddr_t){APS_DISTRIBUTED_TC_ADDRESS}, zllLinkKey);

#ifndef BC_PDS_DISABLE_AUTOSAVING
  /* Configure PDS to store BitCloud parameters by internal events */
  PDS_StoreByEvents(BC_ALL_MEMORY_MEM_ID);
#endif /* !BC_PDS_DISABLE_AUTOSAVING*/

  nCmiMacInit.state = N_CMI_MAC_INIT_IDLE_STATE;
  nCmiMacInit.initMacLayerDoneCallback();
  nCmiMacInit.initMacLayerDoneCallback = NULL;
}

/** Confirmation on network reentering in NFN mode.
    \param conf Confirmation parameters pointer.
*/
static void nCmiCommissionAsNfn(ZDO_StartNetworkConf_t* conf)
{
  NWK_JoinControl_t joinControl;
#if !defined(ZIGBEE_END_DEVICE)
  N_ERRH_ASSERT_FATAL(ZDO_SUCCESS_STATUS == conf->status); /* Reentering the network as NFN failed */
#endif

  CS_ReadParameter(CS_JOIN_CONTROL_ID, &joinControl);

  if(ZDO_SUCCESS_STATUS == conf->status)
  {
    /* Connection Success */
    N_Connection_Connected();

    /*Clear Address Manager if Successfully joined through association procedure*/
    if(joinControl.method == NWK_JOIN_VIA_ASSOCIATION)
      N_AddressManager_AssignAddressRange(0, 0, 0, 0);
  }
#if defined(ZIGBEE_END_DEVICE)
  else if (joinControl.secured == true)
  {
    nCmiRestorePotentialParent();
    CS_WriteParameter(CS_NWK_EXT_PANID_ID, N_DeviceInfo_GetNetworkExtendedPanId());
    joinControl.secured = false;
    /* Store Active key seq number before initiating unsecure join*/
    if (NWK_IsAnyKeyActivated())
    {
      activeKeySeqNum  = NWK_GetActiveKeySeqNumber();
      NWK_DeactivateKey();
    }

    startNetworkReq.ZDO_StartNetworkConf = nCmiCommissionAsNfn;
    CS_WriteParameter(CS_JOIN_CONTROL_ID, &joinControl);
    ZDO_StartNetworkReq(&startNetworkReq);
    return;
  }
  else
  {
    /* Upon failure, restore the active key seq number back */
    NWK_ActivateKey(activeKeySeqNum);
  }
#endif

  // Initiate network search to determine if it has changed channel.
  N_Connection_SearchNetwork();

  nCmiMacInit.state = N_CMI_MAC_INIT_IDLE_STATE;
  nCmiMacInit.initMacLayerDoneCallback();
  nCmiMacInit.initMacLayerDoneCallback = NULL;
}

/** Retrieves the current network key.
    \param[out] networkKey The current network key
*/
void N_Cmi_GetNetworkKey_Impl(N_Security_Key_t networkKey)
{
  const uint8_t* key = NWK_GetActiveKey();

  memcpy(networkKey, key, SECURITY_KEY_SIZE);
}

/** Add an end device to the child table.
    \param ieeeAddressEndDevice The IEEE address of the end device to add to the child table
    \param networkAddressEndDevice The network address of the end device to add to the child table
    \note Calling this function for an end device is a fatal error.
*/
void N_Cmi_AddChild_Impl(N_Address_Extended_t ieeeAddressEndDevice, uint16_t networkAddressEndDevice)
{
  NwkNeighbor_t *neighbor = NWK_AddKnownNeighbor(networkAddressEndDevice, (ExtAddr_t*)ieeeAddressEndDevice, true);

  if (neighbor)
  {
    neighbor->deviceType = DEVICE_TYPE_END_DEVICE;
    neighbor->capability.deviceType = 0U;
    neighbor->capability.rxOnWhenIdle = 0U;
    neighbor->rxOnWhenIdle = 0U;
    neighbor->permitJoining = 0U;
    neighbor->potentialParent = 0U;
  }
}

/** Sets indirect poll rate for a given time interval; then restores the default poll rate.
    \param[in] rate - poll rate to be set
    \param[in] time - time interval for poll rate, ms
*/
void N_Cmi_SetPollRateForTimePeriod_Impl(uint32_t rate, uint32_t time)
{
#ifdef ZIGBEE_END_DEVICE
  CS_WriteParameter(CS_INDIRECT_POLL_RATE_ID, &rate);

  ZDO_StopSyncReq();
  ZDO_StartSyncReq();

  HAL_StopAppTimer(&pollRateTimer);
  pollRateTimer.interval = time;
  HAL_StartAppTimer(&pollRateTimer);
#endif /* ZIGBEE_END_DEVICE */

  (void)rate;
  (void)time;
}

#ifdef ZIGBEE_END_DEVICE
/** Restore poll rate timer has fired
*/
static void nCmiRestorePollRateTimerFired(void)
{
  isSetPollRateAllowed = true;
  
  if (pollRate == 0)
    ZDO_StopSyncReq();
  else
    CS_WriteParameter(CS_INDIRECT_POLL_RATE_ID, &pollRate);
}

/** Restore potential parent flag.
*/
static void nCmiRestorePotentialParent(void)
{
  NwkNeighbor_t *neighbor = NWK_FindNeighborByShortAddr(NWK_GetParentShortAddr());

  if (neighbor)
  {
    neighbor->potentialParent = 1U;
  }
}

#endif /* ZIGBEE_END_DEVICE */

/** Send APS data. Wrapper around APS_DataReq but with the indications as own broadcast transmissions*. */
void N_Cmi_DataRequest_Impl(APS_DataReq_t *apsDataReq)
{
  apsDataReq->txOptions.indicateBroadcasts = 1;
  APS_DataReq(apsDataReq);
}

/** Send ZDP request. */
void N_Cmi_ZdpRequest_Impl(ZDO_ZdpReq_t *zdpReq)
{
  ZDO_ZdpReq(zdpReq);
}

/** Sets the type of groupcast to be used - NWK multicast or APS Groupcast.
    \param[in] multicast - if true NWK multicast will be used.
*/
void N_Cmi_UseNwkMulticast_Impl(bool multicast)
{
  CS_WriteParameter(CS_NWK_USE_MULTICAST_ID, &multicast);
}

/** Perform a network discovery (active scan).

    \param channelMask The channels to scan
    \param durationPerChannel The time to stay at each channel (enum)
    \param pBeacon Pointer to memory that is used to return a beacon from an open network.
    \param mode The discovery mode. Can be used to blacklist or specify a network.
    \param extendedPanId Extended pan id to join or to blacklist depending on the mode parameter.
    \param filteringMode Beacon filtering mode - specifies the criterion for beacon handling.
    \param pfDoneCallback Pointer to the function that should be called after the network discovery is done
*/
void N_Cmi_NetworkDiscovery_Impl(uint32_t channelMask,
                            N_Beacon_Order_t durationPerChannel,
                            N_Beacon_t* pBeacon,
                            N_Connection_AssociateDiscoveryMode_t mode,
                            N_Address_ExtendedPanId_t* extendedPanId,
                            N_Cmi_BeaconFilteringCriterion_t filteringMode,
                            N_Cmi_BeaconIndication_t pfBeaconReceivedCallback,
                            N_Cmi_NetworkDiscoveryDone_t pfDoneCallback)
{
  N_ERRH_ASSERT_FATAL(!nwkDiscovery.pfDoneCallback); /* Unexpected function call */

  N_Cmi_SetBeaconFilter(filteringMode);

  nwkDiscovery.request.scanDuration = durationPerChannel;
  nwkDiscovery.request.scanChannels = channelMask;
  nwkDiscovery.extendedPanId = extendedPanId;
  nwkDiscovery.pfDoneCallback = pfDoneCallback;
  nwkDiscovery.pfBeaconReceivedCallback = pfBeaconReceivedCallback;
  nwkDiscovery.pBeacon = pBeacon;
  nwkDiscovery.mode = mode;

  /* Subscribe to beacon indication events */
  SYS_SubscribeToEvent(BC_EVENT_ACCESS_REQUEST, &bcEventReceiver);

  NWK_NetworkDiscoveryReq(&nwkDiscovery.request);
}


/** Sets beacon filter with accordance to specified filter mode.

    \param criterion - Beacon filtering criterion.
*/
static void N_Cmi_SetBeaconFilter(N_Cmi_BeaconFilteringCriterion_t criterion)
{
  switch (criterion)
  {
#if !defined(ZIGBEE_END_DEVICE)
    case BEACON_FILTERING_CRITERION_NETWORK_SEARCH:
      nwkDiscovery.isBeaconAccepted = nCmiNFNRouterIsBeaconAccepted;
      nwkDiscovery.isBestBeacon = nCmiNFNRouterIsBestBeacon;
    break;
#else
    case BEACON_FILTERING_CRITERION_RECONNECT:
      nwkDiscovery.isBeaconAccepted = nCmiReconnectIsBeaconAccepted;
      nwkDiscovery.isBestBeacon = NULL;
    break;
#endif /* ZIGBEE_END_DEVICE */
    case BEACON_FILTERING_CRITERION_ASSOCIATION:
      nwkDiscovery.isBeaconAccepted = nCmiBeaconIsAccepted;
      nwkDiscovery.isBestBeacon = nCmiIsBestBeacon;
    break;
    case BEACON_FILTERING_CRITERION_NONE:
    default:
      nwkDiscovery.isBeaconAccepted = NULL;
      nwkDiscovery.isBestBeacon = NULL;
    break;
  }
}

/** BitCloud events observer routine

    \param eventId Event ID
    \param data Data associated with event occured
*/
static void nCmiObserver(SYS_EventId_t eventId, SYS_EventData_t data)
{
  if (BC_EVENT_ACCESS_REQUEST == eventId)
  {
    BcAccessReq_t *const accessReq = (BcAccessReq_t*)data;

    if (BC_BEACON_FILTERING_ACTION == accessReq->action && nwkDiscovery.pfDoneCallback)
    {
      const MAC_BeaconNotifyInd_t *const beaconNtfy = (MAC_BeaconNotifyInd_t*)accessReq->context;

      /* All further beacons processing by NWK is prohibited */
      accessReq->denied = true;

      /* Apply internal filter */
      if (NULL != nwkDiscovery.isBeaconAccepted &&
          !nwkDiscovery.isBeaconAccepted(beaconNtfy))
        return;

      /* Pick the best beacon */
      if (NULL != nwkDiscovery.isBestBeacon &&
          !nwkDiscovery.isBestBeacon(beaconNtfy))
        return;

      nCmiPickBeacon(beaconNtfy);

      if (NULL != nwkDiscovery.pfBeaconReceivedCallback)
        nwkDiscovery.pfBeaconReceivedCallback(nwkDiscovery.pBeacon);
    }
  }
}

#if !defined(ZIGBEE_END_DEVICE)
/** Applies filter on received beacon. Filter is active only if
    router is NFN.

    \param beaconNtfy Beacon notification data.
    \returns true - if beacon passes the filtering; false - otherwise.
*/
static bool nCmiNFNRouterIsBeaconAccepted(const MAC_BeaconNotifyInd_t *const beaconNtfy)
{
  const NwkBeaconPayload_t *const beaconPayload = (NwkBeaconPayload_t*)beaconNtfy->msdu;
  ExtPanId_t extPanId = beaconPayload->nwkExtendedPanid;

  if (0 != memcmp((uint8_t*)&extPanId,
                  N_DeviceInfo_GetNetworkExtendedPanId(), sizeof(ExtPanId_t)))
    return false;

  /* Pick beacon with highest nwkUpdateId */
  if (!NWK_IsNewUpdateIdMoreRecent(beaconPayload->updateId))
    return false;

  return true;
}

/** Checks, if new beacon is better then the existent one. Active only if
    router is NFN.

    \param beaconNtfy Beacon notification data
    \returns true - if new beacon is better; false - otherwise
*/
static bool nCmiNFNRouterIsBestBeacon(const MAC_BeaconNotifyInd_t *const beaconNtfy)
{
  const NwkBeaconPayload_t *const beaconPayload = (NwkBeaconPayload_t*)beaconNtfy->msdu;

  /* Pick the very first beacon. */
  if (0 == memcmp((uint8_t*)&(uint64_t){0ULL}, (uint8_t*)nwkDiscovery.pBeacon->extendedPanId,
        sizeof(ExtPanId_t)))
    return true;

  /* Pick beacon with highest network update id. */
  return COMPARE_WITH_THRESHOLD(beaconPayload->updateId,
                                nwkDiscovery.pBeacon->updateId,
                                UPDATE_ID_OVERFLOW_LIMIT);
}
#else
/** Applies filter on received beacon. Filter is used for End Device
    reconnection.

    \param beaconNtfy Beacon notification data.
    \returns true - if beacon passes the filtering; false - otherwise.
*/
static bool nCmiReconnectIsBeaconAccepted(const MAC_BeaconNotifyInd_t *const beaconNtfy)
{
  const NwkBeaconPayload_t *const beaconPayload = (NwkBeaconPayload_t*)beaconNtfy->msdu;
  ExtPanId_t extPanId = beaconPayload->nwkExtendedPanid;

  if (0 != memcmp((uint8_t*)&extPanId,
                  N_DeviceInfo_GetNetworkExtendedPanId(), sizeof(ExtPanId_t)))
    return false;

  return true;
}
#endif /* ZIGBEE_END_DEVICE */

/** Applies custom filter on received beacon

    \param beaconNtfy Beacon notification data
    \returns true - if beacon passes the filtering; false - otherwise
*/
static bool nCmiBeaconIsAccepted(const MAC_BeaconNotifyInd_t *const beaconNtfy)
{
  const NwkBeaconPayload_t *const beaconPayload = (NwkBeaconPayload_t*)beaconNtfy->msdu;
  bool extPanIdIsMatched;

  if (N_Connection_AssociateDiscoveryMode_AnyPan != nwkDiscovery.mode)
  {
    ExtPanId_t extPanId = beaconPayload->nwkExtendedPanid;
    
    N_ERRH_ASSERT_FATAL(nwkDiscovery.extendedPanId); /* ExtPANId wasn't provided for Discovery*/

    extPanIdIsMatched = (0 == memcmp((uint8_t*)&extPanId, nwkDiscovery.extendedPanId, sizeof(ExtPanId_t)));

    /* Filter beacons from according to discovery mode */
    switch (nwkDiscovery.mode)
    {
      case N_Connection_AssociateDiscoveryMode_NotThisPan:
        if (extPanIdIsMatched)
          return false;
        break;

      case N_Connection_AssociateDiscoveryMode_OnlyThisPan:
        if (!extPanIdIsMatched)
          return false;
        break;

      default:
        break;
    }
  }

  /* Filter beacons with permit join flag set to false */
  if (!beaconNtfy->panDescr.superframeSpec.associationPermit)
    return false;

  /* Filter beacons with relevant capacity flag set to false */
#if defined(ZIGBEE_END_DEVICE)
  if (!beaconPayload->field.endDeviceCapacity)
    return false;
#else
  if (!beaconPayload->field.routerCapacity)
    return false;
#endif /* ZIGBEE_END_DEVICE */

  /* Filter beacons by depth */
  if (CS_MAX_NETWORK_DEPTH < beaconPayload->field.deviceDepth)
    return false;

  /* Filter beacons by LQI */
  if (N_CMI_MIN_TREE_LINK_COST < NWK_CostFromLqi(beaconNtfy->panDescr.quality))
    return false;

  return true;
}

/** Checks, if new beacon is better then the existent one

    \param beaconNtfy Beacon notification data
    \returns true - if new beacon is better; false - otherwise
*/
static bool nCmiIsBestBeacon(const MAC_BeaconNotifyInd_t *const beaconNtfy)
{
  /* Pick the very first beacon */
  if (0 == memcmp((uint8_t*)&(uint64_t){0ULL}, (uint8_t*)nwkDiscovery.pBeacon->extendedPanId,
        sizeof(ExtPanId_t)))
    return true;

  /* Pick beacon with highest LQI if this value is below the threshold */
  if (beaconNtfy->panDescr.quality < N_CMI_LQI_RANDOM_BEACON_SELECTION_THRESHOLD)
  {
    if (nwkDiscovery.pBeacon->lqi < beaconNtfy->panDescr.quality)
      return true;
  }
  /* Pick beacon randomly if LQI is above the threshold */
  else
  {
    uint8_t seed;

    N_Security_GetRandomData(&seed, sizeof(seed));
    return seed & 0x01;
  }

  return false;
}

/** Picks a beacon as a best one. The best beacon will be finaly returned as a result of
    the Discovery procedure.

    \param beaconNtfy Beacon notification data
*/
static void nCmiPickBeacon(const MAC_BeaconNotifyInd_t *const beaconNtfy)
{
  const NwkBeaconPayload_t *const beaconPayload = (NwkBeaconPayload_t*)beaconNtfy->msdu;
  N_Beacon_t *const beacon = nwkDiscovery.pBeacon;
  ExtPanId_t extPanId = beaconPayload->nwkExtendedPanid;

  beacon->depth = beaconPayload->field.deviceDepth;
  memcpy(beacon->extendedPanId, (uint8_t*)&extPanId, sizeof(ExtPanId_t));
  beacon->hasDeviceCapacity = beaconPayload->field.endDeviceCapacity;
  beacon->hasRouterCapacity = beaconPayload->field.routerCapacity;
  beacon->logicalChannel = beaconNtfy->panDescr.channel;
  beacon->lqi = beaconNtfy->panDescr.quality;
  beacon->panId = beaconNtfy->panDescr.coordPANId;
  beacon->permitJoining = beaconNtfy->panDescr.superframeSpec.associationPermit;
  beacon->protocolVersion = beaconPayload->protocolId;
  beacon->sourceAddress = beaconNtfy->panDescr.coordAddr.sh;
  beacon->stackProfile = beaconPayload->field.stackProfile;
  beacon->updateId = beaconPayload->updateId;
}

/** Network Discovery completion callback.

    \param conf NWK Discovery results
*/
static void NWK_NetworkDiscoveryConf(NWK_NetworkDiscoveryConf_t *conf)
{
  N_Cmi_NetworkDiscoveryDone_t callback = nwkDiscovery.pfDoneCallback;
  (void)conf;
  /* Done callback used as a busy indicator */
  nwkDiscovery.pfDoneCallback = NULL;
  nwkDiscovery.pfBeaconReceivedCallback = NULL;

  /* Unsubscribe from beacon indication events */
  SYS_UnsubscribeFromEvent(BC_EVENT_ACCESS_REQUEST, &bcEventReceiver);

  N_ERRH_ASSERT_FATAL(callback); /* No callback function */
  callback();
}

/** Perform a ZigBee network join (MAC association).
    \param pSelectedBeacon The beacon from a device to join to.
    \param pfDoneCallback Pointer to the function that should be called after the join is done
*/
void N_Cmi_Join_Impl(N_Beacon_t* pSelectedBeacon, N_Cmi_JoinDone_t pfDoneCallback)
{
  ExtAddr_t extAddr = 0ULL;
  NWK_JoinControl_t joinControl =
  {
    .method = NWK_JOIN_VIA_ASSOCIATION,
    .annce = true,
    .discoverNetworks = false,
    .secured = true,
  };
  NwkNeighbor_t *neighbor = NWK_AddKnownNeighbor(pSelectedBeacon->sourceAddress, &extAddr, true);

  N_ERRH_ASSERT_FATAL(NULL == associationDoneCallback); /* Unexpected function call */
  N_ERRH_ASSERT_FATAL(neighbor); /* Neighbor table full */

  associationDoneCallback = pfDoneCallback;

  neighbor->logicalChannel = pSelectedBeacon->logicalChannel;
  neighbor->depth = pSelectedBeacon->depth;
  neighbor->panId = pSelectedBeacon->panId;
  memcpy((uint8_t*)&neighbor->extPanId, pSelectedBeacon->extendedPanId, sizeof(uint64_t));
  neighbor->updateId = pSelectedBeacon->updateId;
  neighbor->permitJoining = pSelectedBeacon->permitJoining;

  startNetworkReq.ZDO_StartNetworkConf = nCmiAssociationConf;
  CS_WriteParameter(CS_JOIN_CONTROL_ID, &joinControl);
  ZDO_StartNetworkReq(&startNetworkReq);
}

static void nCmiAssociationConf(ZDO_StartNetworkConf_t* conf)
{
  N_Cmi_Result_t result = N_Cmi_Result_Failure;
  NWK_JoinControl_t joinControl;

  N_ERRH_ASSERT_FATAL(associationDoneCallback); /* No callback function */

  if (ZDO_SUCCESS_STATUS == conf->status)
    result = N_Cmi_Result_Success;
  else if (ZDO_AUTHENTICATION_FAIL_STATUS == conf->status)
    result = N_Cmi_Result_NoNetworkKey;
  else if (ZDO_INVALID_REQUEST_STATUS == conf->status)
    result = N_Cmi_Result_Invalid_Request;

  associationDoneCallback(result);

  CS_ReadParameter(CS_JOIN_CONTROL_ID, &joinControl);

  /*Clear Address Manager if Successfully joined through association procedure*/
  if(conf->status == ZDO_SUCCESS_STATUS)
  {
    if(joinControl.method == NWK_JOIN_VIA_ASSOCIATION)
      N_AddressManager_AssignAddressRange(0, 0, 0, 0);
  }

  /* Done callback used as a busy indicator */
  associationDoneCallback = NULL;
}

/** Marks ZLL preinstalled TC link key as a primary global key for a PAN
 *  \param doneCallback Pointer to the function that should be called after the setting is done
*/
void N_Cmi_SetZllLinkKeyAsPrimary_Impl(N_Cmi_SetZllLinkKeyAsPrimaryDone_t doneCallback)
{
  N_ERRH_ASSERT_FATAL(!setLinkKeyAsPrimaryDoneCallback); /* Unexpected function call */

  setLinkKeyAsPrimaryDoneCallback = doneCallback;
  N_Security_GetDistributedTrustCenterLinkKey(zllLinkKey, setLinkKeyAsPrimary);
}

/** Callback on ZLL distributed TC link key obtaining
*/
static void setLinkKeyAsPrimary(void)
{
  APS_KeyHandle_t keyHandle = APS_KeyHandleUninitialized();

  /* Scan through the Key Pair set and locate a record with ZLL link key */
  do
  {
    keyHandle = APS_NextKeys(keyHandle);
    if (0 == memcmp(APS_GetLinkKey(keyHandle), zllLinkKey, sizeof(zllLinkKey)))
      break;
  } while (APS_KEY_HANDLE_IS_VALID(keyHandle));

  N_ERRH_ASSERT_FATAL(APS_KEY_HANDLE_IS_VALID(keyHandle)); /* ZLL TC Link key is absent */
  APS_MarkGlobalKeyAsPrimary(keyHandle);

  N_ERRH_ASSERT_FATAL(setLinkKeyAsPrimaryDoneCallback); /* No callback function */
  setLinkKeyAsPrimaryDoneCallback();

  /* Done callback used as a busy indicator */
  setLinkKeyAsPrimaryDoneCallback = NULL;
}

/** ZLL Platform to BitCloud address mode convertion
 *  \param platformAddrMode ZLL Platform address mode
 *
 *  \return relative BitCloud address mode
*/
APS_AddrMode_t N_Cmi_PlatformToBcAddressModeConvert_Impl(N_Address_Mode_t platformAddrMode)
{
  if (N_Address_Mode_Broadcast == platformAddrMode)
    return APS_SHORT_ADDRESS;

  /* All other modes are equal to ones specified in the ZigBee spec. */
  return platformAddrMode;
}

/** ZLL Platform to BitCloud addresses convertion
 *  \param[in] platformAddressing ZLL Platform addressing pointer
 *  \param[out] bcAddress BitCloud addressing pointer
*/
void N_Cmi_PlatformToBcAddressingConvert_Impl(APS_Address_t *bcAddress, N_Address_t *platformAddressing)
{
  switch (platformAddressing->addrMode)
  {
    case N_Address_Mode_Extended:
      memcpy(&bcAddress->extAddress, platformAddressing->address.extendedAddress, sizeof(ExtAddr_t));
      break;

    default:
      bcAddress->shortAddress = platformAddressing->address.shortAddress;
      break;
  }
}

#if ATMEL_APPLICATION_SUPPORT != 1
/*******************************************************************************
  Description: just a stub.

  Parameters: are not used.

  Returns: nothing.
*******************************************************************************/
void BSP_TaskHandler(void){};

#ifdef _BINDING_
/***********************************************************************************
  Stub for ZDO Binding Indication

  Parameters:
    bindInd - indication

  Return:
    none

 ***********************************************************************************/
void ZDO_BindIndication(ZDO_BindInd_t *bindInd)
{
  (void)bindInd;
}

/***********************************************************************************
  Stub for ZDO Unbinding Indication

  Parameters:
    unbindInd - indication

  Return:
    none

 ***********************************************************************************/
void ZDO_UnbindIndication(ZDO_UnbindInd_t *unbindInd)
{
  (void)unbindInd;
}
#endif //_BINDING_

/*******************************************************************************
  Description: just a stub.

  Parameters: are not used.

  Returns: nothing.
*******************************************************************************/
void ZDO_MgmtNwkUpdateNotf(ZDO_MgmtNwkUpdateNotf_t *nwkParams)
{
  nwkParams = nwkParams;  // Unused parameter warning prevention
}


/*******************************************************************************
  Description: just a stub.

  Parameters: none.

  Returns: nothing.
*******************************************************************************/
void ZDO_WakeUpInd(void)
{
}

#endif /* ATMEL_APPLICATION_SUPPORT != 1 */

/**************************************************************************//**
\brief Assign a new address from the range of free addresses
\returns Newly assigned address or 0 in case of an error
******************************************************************************/
ShortAddr_t ZCL_ZllAssignAddress(void)
{
  return 0xDEAD;
}

/* eof N_Cmi.c */


