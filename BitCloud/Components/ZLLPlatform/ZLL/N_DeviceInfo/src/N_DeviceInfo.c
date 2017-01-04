/**************************************************************************//**
  \file N_DeviceInfo.c

  \brief
    device info implementation.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    21.08.12 N. Fomin - Created.
******************************************************************************/

/******************************************************************************
                    Includes section
******************************************************************************/
#include <N_DeviceInfo_Bindings.h>
#include <N_DeviceInfo.h>
#include <sysTypes.h>
#include <sysEvents.h>
#include <N_Task.h>
#include <macAddr.h>
#include <N_ErrH.h>
#include <N_DeviceInfo_Init.h>
#include <N_DeviceInfo_Version.h>
#include <configServer.h>
#include <aps.h>

#include <S_Nv.h>
#include <S_Nv_Stack_Ids.h>
#include <S_NvInternal.h>

#include <devconfig.h>

/******************************************************************************
                    Definitions section
******************************************************************************/
#define COMPID "N_DeviceInfo"

#ifndef N_DEVICEINFO_MAX_SUBSCRIBERS
  #define N_DEVICEINFO_MAX_SUBSCRIBERS        10u
#endif

#define LOWEST_ZIGBEE_CHANNEL_MASK          0x00000800uL
#define VALID_ZIGBEE_CHANNEL_MASK           0x07FFF800uL

#define ZLL_PRIMARY_CHANNELMASK             (ZB_CHANNEL_11 | ZB_CHANNEL_15 | ZB_CHANNEL_20 | ZB_CHANNEL_25)
#define ZLL_SECONDARY_CHANNELMASK           (VALID_ZIGBEE_CHANNEL_MASK & (~ZLL_PRIMARY_CHANNELMASK))

#define TASKEVENT_SETTINGS_CHANGED          (0u)

/******************************************************************************
                    Types section
******************************************************************************/
typedef struct
{
  bool                      addressAssignmentCapable;
  bool                      canBeLinkInitiator;
  uint8_t                   correction;
  int8_t                    threshold;
  N_Address_ExtendedPanId_t extPanId;
  ExtAddr_t                 parentExtAddr;
  uint8_t                   touchlinkZerodBmTransmitPower;
  bool                      factoryNew;
  uint32_t                  primaryChannelMask;
  uint32_t                  secondaryChannelMask;
  PanId_t                   panId;
  Channel_t                 channel;
  ShortAddr_t               nwkAddr;
  uint8_t                   nwkUpdateId;
  N_DeviceInfo_TrustCenterMode_t tcMode;
} DeviceInfoParameters_t;

/******************************************************************************
                    Prototypes section
******************************************************************************/
static void parameterWasUpdated(SYS_EventId_t eventId, SYS_EventData_t data);

/******************************************************************************
                    Local variables section
******************************************************************************/
static DeviceInfoParameters_t deviceInfoParams = {
  
  .factoryNew                    = true,
  .tcMode                        = N_DeviceInfo_TrustCenterMode_Distributed,
};
static SYS_EventReceiver_t eventReceiver = {
  .func = parameterWasUpdated
};
static uint16_t deviceInfoUpdateMask;
static N_Task_Id_t taskId = N_TASK_ID_NONE;
N_UTIL_CALLBACK_DECLARE(N_DeviceInfo_Callback_t, deviceInfo_Subscribers, N_DEVICEINFO_MAX_SUBSCRIBERS);

/******************************************************************************
                    Implementation section
******************************************************************************/

/** Subscribe to this component's callbacks.
    \param pCallback Pointer to a callback structure filled with functions to be called.
*/
void N_DeviceInfo_Subscribe_Impl(const N_DeviceInfo_Callback_t* pCallback)
{
  N_ERRH_ASSERT_FATAL(N_TASK_ID_NONE != taskId);
  N_UTIL_CALLBACK_SUBSCRIBE(N_DeviceInfo_Callback_t, deviceInfo_Subscribers, pCallback);
}

/** Return the version string for the ZigBee Platform.
    \returns Pointer to the version string

    To get a correct version string a pre-build action must be run to generate
    N_DeviceInfo_Version.h. This file must a define for N_DEVICE_INFO_PLATFORM_VERSION.
    The version string has the following format.

    (ZigBee Platform release),package_ZigBee (svnversion of package_ZigBee folder),package_Z_Stack (svnversion of package_ZigBee folder),built by (name of user of build machine)

    For the output format of svnversion run svnversion help on the command line.
*/
const char* N_DeviceInfo_GetStackVersion_Impl(void)
{
  return N_DEVICE_INFO_PLATFORM_VERSION;
}

/** Check if this device is address assignment capable.
    \returns TRUE if address assignment capable, else FALSE.
*/
bool N_DeviceInfo_IsAddressAssignmentCapable_Impl(void)
{
  return deviceInfoParams.addressAssignmentCapable;
}

/** Set if this device is address assignment capable.
    \param isAddressAssignmentCapable the address assignment capability.
*/
void N_DeviceInfo_SetAddressAssignmentCapable_Impl(bool isAddressAssignmentCapable)
{
  deviceInfoParams.addressAssignmentCapable = isAddressAssignmentCapable;
}

/** Check if this device can be link initiator.
    \returns TRUE if it can be link initiator, else FALSE.
*/
bool N_DeviceInfo_CanBeLinkInitiator_Impl(void)
{
  return deviceInfoParams.canBeLinkInitiator;
}

/** Set if this device can be link initiator.
    \param canBeLinkInitiator TRUE if the device can be link initiator, else FALSE.
*/
void N_DeviceInfo_SetCanBeLinkInitiator_Impl(bool canBeLinkInitiator)
{
  deviceInfoParams.canBeLinkInitiator = canBeLinkInitiator;
}

/** Get the touchlink rssi correction.
    \returns The touchlink rssi correction.
*/
uint8_t N_DeviceInfo_GetTouchlinkRssiCorrection_Impl(void)
{
  return deviceInfoParams.correction;
}

/** Set the touchlink rssi correction.
    \param correction The touchlink rssi correction.
*/
void N_DeviceInfo_SetTouchlinkRssiCorrection_Impl(uint8_t correction)
{
  deviceInfoParams.correction = correction;
}

/** Get the touchlink rssi threshold.
    \returns The touchlink rssi threshold.
*/
int8_t N_DeviceInfo_GetTouchlinkRssiThreshold_Impl(void)
{
  return deviceInfoParams.threshold;
}

/** Set the touchlink rssi threshold.
    \param threshold The touchlink rssi threshold.
*/
void N_DeviceInfo_SetTouchlinkRssiThreshold_Impl(int8_t threshold)
{
  deviceInfoParams.threshold = threshold;
}

/** Get the network pan id.
    \returns The network pan id.
*/
uint16_t N_DeviceInfo_GetNetworkPanId_Impl(void)
{
  return deviceInfoParams.panId;
}

/** Get the extended network pan id.
    \returns The extended network pan id.
*/
N_Address_ExtendedPanId_t* N_DeviceInfo_GetNetworkExtendedPanId_Impl(void)
{
  return &deviceInfoParams.extPanId;
}

/** Get the network address.
    \returns The network address.
*/
uint16_t N_DeviceInfo_GetNetworkAddress_Impl(void)
{
  return deviceInfoParams.nwkAddr;
}

/** Get the parents network address.
    \returns The parents network address.
*/
uint16_t N_DeviceInfo_GetParentNetworkAddress_Impl(void)
{
  // Not sure about zero value since it is a valid short address
  uint16_t parentAddress = NWK_GetParentShortAddr();

  if (0xFFFF != parentAddress)
    return parentAddress;

  return 0;
}

/** Get the network channel.
    \returns The network channel.
*/
uint8_t N_DeviceInfo_GetNetworkChannel_Impl(void)
{
  return deviceInfoParams.channel;
}

/** Get the network update id.
    \returns The network update id.
*/
uint8_t N_DeviceInfo_GetNetworkUpdateId_Impl(void)
{
  return deviceInfoParams.nwkUpdateId;
}

/** Get the IEEE address.
    \returns The IEEE address.
*/
const uint8_t* N_DeviceInfo_GetIEEEAddress_Impl(void)
{
  const ExtAddr_t *extAddr = MAC_GetExtAddr();

  return (const uint8_t *)extAddr;
}

/** Get the parents IEEE address.
    \returns The parents IEEE address.
*/
const uint8_t* N_DeviceInfo_GetParentIEEEAddress_Impl(void)
{
  if (N_DeviceInfo_IsEndDevice())
  {
    uint16_t addr = NWK_GetParentShortAddr();

    if (0xFFFF == addr)
      return (const uint8_t *)&deviceInfoParams.parentExtAddr;

    const ExtAddr_t *extAddr = NWK_GetExtByShortAddress(addr);
    if (!extAddr)
      N_ERRH_FATAL();

    return (const uint8_t *)extAddr;
  }
  // probably it should be set to all ff's
  // anyway it is just a stub for routers
  // zeros by the truth
  else
    return (const uint8_t *)&deviceInfoParams.parentExtAddr;
}

/** Get the touchlink 0 dBm transmit power.
    \returns The register value for the corresponding transmit power.
*/
uint8_t N_DeviceInfo_GetTouchlinkZerodBmTransmitPower_Impl(void)
{
  return deviceInfoParams.touchlinkZerodBmTransmitPower;
}

/** Check if the device is factory new.
    \returns TRUE if device is factory new, else returns FALSE.
*/
bool N_DeviceInfo_IsFactoryNew_Impl(void)
{
  return deviceInfoParams.factoryNew;
}

/** Check if the device is an end device.
    \returns TRUE if device is an end device, else returns FALSE.
*/
/*bool N_DeviceInfo_IsEndDevice(void)
{
  return (DEVICE_TYPE_END_DEVICE == NWK_GetDeviceType());
}*/

/** Get the primary channel mask.
    \returns The primary channel mask.
*/
uint32_t N_DeviceInfo_GetPrimaryChannelMask_Impl(void)
{
  return deviceInfoParams.primaryChannelMask;
}

/** Set the primary channel mask.
    \param channelMask The new primary channel mask.
*/
void N_DeviceInfo_SetPrimaryChannelMask_Impl(uint32_t channelMask)
{
  deviceInfoParams.primaryChannelMask = channelMask;
}

/** Get the secondary channel mask.
    \returns The secondary channel mask.
*/
uint32_t N_DeviceInfo_GetSecondaryChannelMask_Impl(void)
{
  return deviceInfoParams.secondaryChannelMask;
}

/** Set the secondary channel mask.
    \param channelMask The new secondary channel mask.
*/
void N_DeviceInfo_SetSecondaryChannelMask_Impl(uint32_t channelMask)
{
  deviceInfoParams.secondaryChannelMask = channelMask;
}

/** Get the number of channels in a channel mask (>=1).
    \param channelMask The channelmask to check
    \returns The number of channels in the given channel mask.
*/
uint8_t N_DeviceInfo_GetNrChannelsInChannelMask_Impl(uint32_t channelMask)
{
  uint8_t count = 0u;
  while (channelMask != 0u)
  {
    count++;
    // clear the rightmost bit
    channelMask &= (channelMask - 1u);
  }
  N_ERRH_ASSERT_FATAL(count != 0u);
 
  return count;
}

/** Retrieve one of the channels from a channel mask.
    \param index An index from 0 to N_DeviceInfo_GetNrChannelsInChannelMask()-1
    \param channelMask The channelmask
    \returns The channels from the given channel mask in order, e.g. index 0 will return the lowest channel.
*/
uint8_t N_DeviceInfo_GetChannelForIndex_Impl(uint8_t index, uint32_t channelMask)
{
  channelMask &= VALID_ZIGBEE_CHANNEL_MASK;

  uint8_t logicalChannel = LOWEST_ZIGBEE_CHANNEL - 1u;
  uint8_t i = 0u;

  while (i <= index)
  {
    if (0u == channelMask)
      // invalid index
      return 0u;
    else
    {
      logicalChannel++;
      // is the bit in the channel mask set?
      if (0u != (channelMask & LOWEST_ZIGBEE_CHANNEL_MASK))
        i++;
      // shift channel mask to check the next channel
      channelMask = (channelMask >> 1u) & VALID_ZIGBEE_CHANNEL_MASK;
    }
  }

  return logicalChannel;
}

/** Returns whether a channel is valid according to the current channel mask.
    \param channel The channel to check
    \param channelMask The channelMask to check
    \returns TRUE if the channel is in the mask, otherwise FALSE
*/
bool N_DeviceInfo_IsChannelInMask_Impl(uint8_t channel, uint32_t channelMask)
{
  return ((1uL << channel) & channelMask);
}

/** Gets the network mode.
    \returns The network mode this device is currently operating in.
    \note Must not be called when the device is factory new.
*/
N_DeviceInfo_TrustCenterMode_t N_DeviceInfo_GetTrustCenterMode_Impl(void)
{
  return deviceInfoParams.tcMode;
}

/** Aligns parameters in N_DeviceInfo with configuration server
    \param channel The network channel.
    \param panId The network channel.
    \param extPanId Pointer to the extended network pan id.
    \param nwkAddr The network address.
*/
void N_DeviceInfo_SynchronizeParameters(N_DeviceInfo_SynchronizationParameters_t *params)
{
  N_DeviceInfo_SetNetworkPanId(params->panId);
  N_DeviceInfo_SetNetworkExtendedPanId(params->extPanId);
  N_DeviceInfo_SetNetworkChannel(params->channel);
  N_DeviceInfo_SetNetworkAddress(params->nwkAddr);
}

/** Set if the device is factory new.
    \param factoryNew TRUE if device is factory new, else returns FALSE.
*/
void N_DeviceInfo_SetFactoryNew_Impl(bool factoryNew)
{
  if (deviceInfoParams.factoryNew != factoryNew)
  {
    deviceInfoParams.factoryNew = factoryNew;
    deviceInfoUpdateMask |= N_DEVICEINFO_SETTING_FACTORY_NEW;
    S_Nv_ReturnValue_t ret = S_Nv_Write(FACTORY_NEW_STORAGE_ID, 0u, sizeof(factoryNew), &factoryNew);
    N_ERRH_ASSERT_FATAL(ret == S_Nv_ReturnValue_Ok);

    N_Task_SetEvent(taskId, TASKEVENT_SETTINGS_CHANGED);
  }
}

/** Set the network pan id.
    \param panId The network pan id.
*/
void N_DeviceInfo_SetNetworkPanId_Impl(uint16_t panId)
{
  if (deviceInfoParams.panId != panId)
  {
    deviceInfoParams.panId = panId;
    deviceInfoUpdateMask |= N_DEVICEINFO_SETTING_PANID;
    N_Task_SetEvent(taskId, TASKEVENT_SETTINGS_CHANGED);
  }
}

/** Set the extended network pan id.
    \param pExtPanId Pointer to the extended network pan id.
*/
void N_DeviceInfo_SetNetworkExtendedPanId_Impl(uint8_t* pExtPanId)
{
  if (memcmp(deviceInfoParams.extPanId, pExtPanId, sizeof(deviceInfoParams.extPanId)))
  {
    memcpy(deviceInfoParams.extPanId, pExtPanId, sizeof(deviceInfoParams.extPanId));
    deviceInfoUpdateMask |= N_DEVICEINFO_SETTING_EXTPANID;
    N_Task_SetEvent(taskId, TASKEVENT_SETTINGS_CHANGED);
  }
}

/** Set the network address.
    \param nwkAddress The network address.
*/
void N_DeviceInfo_SetNetworkAddress_Impl(uint16_t nwkAddress)
{
  if (deviceInfoParams.nwkAddr != nwkAddress)
  {
    deviceInfoParams.nwkAddr = nwkAddress;
    deviceInfoUpdateMask |= N_DEVICEINFO_SETTING_NETWORK_ADDRESS;
    N_Task_SetEvent(taskId, TASKEVENT_SETTINGS_CHANGED);
  }
}

/** Set the network update id.
    \param updateId The network update id.
*/
void N_DeviceInfo_SetNetworkUpdateId_Impl(uint8_t updateId)
{
  if (deviceInfoParams.nwkUpdateId != updateId)
  {
    deviceInfoParams.nwkUpdateId = updateId;
    deviceInfoUpdateMask |= N_DEVICEINFO_SETTING_NETWORK_UPDATE_ID;
    N_Task_SetEvent(taskId, TASKEVENT_SETTINGS_CHANGED);
  }
}

/** Set the network channel.
    \param nwkChannel The network channel.
*/
void N_DeviceInfo_SetNetworkChannel_Impl(uint8_t nwkChannel)
{
  if (deviceInfoParams.channel != nwkChannel)
  {
    deviceInfoParams.channel = nwkChannel;
    deviceInfoUpdateMask |= N_DEVICEINFO_SETTING_CHANNEL;
    N_Task_SetEvent(taskId, TASKEVENT_SETTINGS_CHANGED);
  }
}

/** Sets the trust center mode.
    \param trustCenterMode The trust center mode the network is currently operating in.
*/
void N_DeviceInfo_SetTrustCenterMode_Impl(N_DeviceInfo_TrustCenterMode_t trustCenterMode)
{
  if (deviceInfoParams.tcMode != trustCenterMode)
  {
    deviceInfoParams.tcMode = trustCenterMode;
    deviceInfoUpdateMask |= N_DEVICEINFO_SETTING_TRUSTCENTERMODE;
    N_Task_SetEvent(taskId, TASKEVENT_SETTINGS_CHANGED);
  }
}

/** Return the profile ID of the given endpoint
    \param endpoint the endpoint to get the profile ID of
    \returns The profile ID.
*/
N_DeviceInfo_Profile_t N_DeviceInfo_GetProfileIdForEndpoint_Impl(uint8_t endpoint)
{
  APS_RegisterEndpointReq_t *epReq = NULL;

  while ((epReq = APS_NextEndpoint(epReq)))
    if (epReq->simpleDescriptor->endpoint == endpoint)
      return epReq->simpleDescriptor->AppProfileId;
   
  N_ERRH_FATAL();
  return 0;
}

bool N_DeviceInfo_EventHandler(N_Task_Event_t evt)
{
  switch(evt)
  {
    case TASKEVENT_SETTINGS_CHANGED:
    // Network settings N_DeviceInfo_SetNetworkPanIdchanged
    if (0u != deviceInfoUpdateMask)
      N_UTIL_CALLBACK(N_DeviceInfo_Callback_t, deviceInfo_Subscribers, NetworkSettingsChanged, (deviceInfoUpdateMask));
    deviceInfoUpdateMask = 0u;
    break;

    default:
      return FALSE;
  }

  return TRUE;
}

/** Initializes the component
*/
void N_DeviceInfo_Init(uint8_t touchlinkRssiCorrection, int8_t touchlinkRssiThreshold,
                       bool addressAssignmentCapable, bool canBeLinkInitiator, uint8_t touchlinkZerodBmTransmitPower)
{
  S_Nv_ReturnValue_t ret;

  /* Factory new initialization */
  ret = S_Nv_ItemInit(FACTORY_NEW_STORAGE_ID, sizeof(deviceInfoParams.factoryNew), &deviceInfoParams.factoryNew);
  N_ERRH_ASSERT_FATAL( (ret == S_Nv_ReturnValue_DidNotExist) || (ret == S_Nv_ReturnValue_Ok) );

  taskId = N_Task_GetIdFromEventHandler(N_DeviceInfo_EventHandler);

  deviceInfoParams.primaryChannelMask   = ZLL_PRIMARY_CHANNELMASK;
  deviceInfoParams.secondaryChannelMask = ZLL_SECONDARY_CHANNELMASK;

#if defined(DEVCFG_PRIMARY_CHANNELMASK)
    #warning USING FIXED PRIMARY CHANNELMASK!
    deviceInfoParams.primaryChannelMask = DEVCFG_PRIMARY_CHANNELMASK;
#endif

#if defined(DEVCFG_SECONDARY_CHANNELMASK)
    #warning USING FIXED SECONDARY CHANNELMASK!
    deviceInfoParams.secondaryChannelMask = DEVCFG_SECONDARY_CHANNELMASK;
#endif

  deviceInfoParams.addressAssignmentCapable      = addressAssignmentCapable;
  deviceInfoParams.canBeLinkInitiator            = canBeLinkInitiator;
  deviceInfoParams.correction                    = touchlinkRssiCorrection;
  deviceInfoParams.threshold                     = touchlinkRssiThreshold;
  deviceInfoParams.touchlinkZerodBmTransmitPower = touchlinkZerodBmTransmitPower;

  deviceInfoUpdateMask |= N_DEVICEINFO_SETTING_DURING_INIT;
  N_Task_SetEvent(taskId, TASKEVENT_SETTINGS_CHANGED);

  SYS_SubscribeToEvent(BC_EVENT_NETWORK_ENTERED,       &eventReceiver);
  SYS_SubscribeToEvent(BC_EVENT_NETWORK_STARTED,       &eventReceiver);
  SYS_SubscribeToEvent(BC_EVENT_NETWORK_LEFT,          &eventReceiver);
  SYS_SubscribeToEvent(BC_EVENT_NETWORK_UPDATE,        &eventReceiver);
  SYS_SubscribeToEvent(BC_EVENT_NWK_UPDATEID_CHANGED,  &eventReceiver);
  SYS_SubscribeToEvent(BC_EVENT_TC_MODE_CHANGED,       &eventReceiver);
  SYS_SubscribeToEvent(BC_EVENT_CHANNEL_CHANGED,       &eventReceiver);
}

/** Callback function for
*/
static void parameterWasUpdated(SYS_EventId_t eventId, SYS_EventData_t data)
{
  switch(eventId)
  {
    case BC_EVENT_NETWORK_LEFT:
    case BC_EVENT_NETWORK_ENTERED:
    case BC_EVENT_NETWORK_STARTED:
    case BC_EVENT_NETWORK_UPDATE:
    {
      PanId_t     panId;
      uint8_t     extPanId[8];
      ShortAddr_t addr;

      CS_ReadParameter(CS_NWK_ADDR_ID, &addr);
      CS_ReadParameter(CS_NWK_PANID_ID, &panId);
      CS_ReadParameter(CS_NWK_EXT_PANID_ID, extPanId);

      N_DeviceInfo_SetNetworkPanId(panId);
      N_DeviceInfo_SetNetworkExtendedPanId(extPanId);
      N_DeviceInfo_SetNetworkChannel(NWK_GetLogicalChannel());
      N_DeviceInfo_SetNetworkAddress(addr);
      N_DeviceInfo_SetNetworkUpdateId(NWK_GetUpdateId());
      N_DeviceInfo_SetFactoryNew(false);
      if (eventId == BC_EVENT_NETWORK_LEFT)
      {
        SYS_UnsubscribeFromEvent(BC_EVENT_CHANNEL_CHANGED, &eventReceiver);
      }
      else
      {
        SYS_SubscribeToEvent(BC_EVENT_CHANNEL_CHANGED, &eventReceiver);
      }
      break;
    }
    case BC_EVENT_NWK_UPDATEID_CHANGED:
      N_DeviceInfo_SetNetworkUpdateId(NWK_GetUpdateId());
      break;
    case BC_EVENT_TC_MODE_CHANGED:
      N_DeviceInfo_SetTrustCenterMode(APS_DISTRIBUTED_TRUST_CENTER == APS_GetOwnTcMode() ?
                                      N_DeviceInfo_TrustCenterMode_Distributed :
                                      N_DeviceInfo_TrustCenterMode_Central);
      break;
    case BC_EVENT_CHANNEL_CHANGED:
      N_DeviceInfo_SetNetworkChannel(NWK_GetLogicalChannel());
      break;
    default:
      N_ERRH_FATAL();
      break;
  }
  (void)data;
}
/** Get end device info.
    \param None.
    \returns TRUE - If if it end device else FALSE.
*/
bool N_DeviceInfo_IsEndDevice_Impl(void)
{
  #if defined(ZIGBEE_END_DEVICE)
  return TRUE;
  #else
  return FALSE;
  #endif
}

/* eof N_Device_Info.c */
