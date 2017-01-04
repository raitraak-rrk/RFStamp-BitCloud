/*********************************************************************************************//**
\file  

\brief 

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal

$Id: N_DeviceInfo.h 24615 2013-02-27 05:14:24Z arazinkov $

***************************************************************************************************/

#ifndef N_DEVICEINFO_H
#define N_DEVICEINFO_H

/***************************************************************************************************
* INCLUDE FILES
***************************************************************************************************/

#include "N_Address.h"
#include "N_Types.h"
//#include "ZComDef.h"

/***************************************************************************************************
* C++ DECLARATION WRAPPER
***************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************************************
* EXPORTED TYPES
***************************************************************************************************/

typedef struct N_DeviceInfo_Callback_t
{
    /** Pass the incoming data
    *   \param pPacket The incoming packet data
    */
    void (*NetworkSettingsChanged)(uint16_t settings);

    /** Guard to ensure the initializer contains all functions. Must be -1.
    */
    int8_t endOfList;
} N_DeviceInfo_Callback_t;

typedef enum N_DeviceInfo_TrustCenterMode_t
{
    N_DeviceInfo_TrustCenterMode_Central,
    N_DeviceInfo_TrustCenterMode_Distributed,
} N_DeviceInfo_TrustCenterMode_t;

/** The supported profiles. Can be cast to uint16_t to get the profile IDs. */
typedef enum N_DeviceInfo_Profile_t
{
    N_DeviceInfo_Profile_ZLL = 0xC05Eu,
    N_DeviceInfo_Profile_ZHA = 0x0104u,
} N_DeviceInfo_Profile_t;

typedef struct N_DeviceInfo_SynchronizationParameters_t
{
  uint8_t  channel;
  uint16_t panId;
  uint8_t *extPanId;
  uint16_t nwkAddr;
} N_DeviceInfo_SynchronizationParameters_t;

/***************************************************************************************************
* EXPORTED MACROS AND CONSTANTS
***************************************************************************************************/

#define N_DEVICEINFO_SETTING_PANID              0x0001u
#define N_DEVICEINFO_SETTING_EXTPANID           0x0002u
#define N_DEVICEINFO_SETTING_CHANNEL            0x0004u
#define N_DEVICEINFO_SETTING_NETWORK_ADDRESS    0x0008u
#define N_DEVICEINFO_SETTING_NETWORK_UPDATE_ID  0x0010u
#define N_DEVICEINFO_SETTING_FACTORY_NEW        0x0020u
#define N_DEVICEINFO_SETTING_TRUSTCENTERMODE    0x0040u
#define N_DEVICEINFO_SETTING_DURING_INIT        0x8000u

// ZigBee Channels (for creating a channelmask)
#define ZB_CHANNEL_26   0x04000000uL  // 0x1A
#define ZB_CHANNEL_25   0x02000000uL  // 0x19
#define ZB_CHANNEL_24   0x01000000uL  // 0x18
#define ZB_CHANNEL_23   0x00800000uL  // 0x17
#define ZB_CHANNEL_22   0x00400000uL  // 0x16
#define ZB_CHANNEL_21   0x00200000uL  // 0x15
#define ZB_CHANNEL_20   0x00100000uL  // 0x14
#define ZB_CHANNEL_19   0x00080000uL  // 0x13
#define ZB_CHANNEL_18   0x00040000uL  // 0x12
#define ZB_CHANNEL_17   0x00020000uL  // 0x11
#define ZB_CHANNEL_16   0x00010000uL  // 0x10
#define ZB_CHANNEL_15   0x00008000uL  // 0x0F
#define ZB_CHANNEL_14   0x00004000uL  // 0x0E
#define ZB_CHANNEL_13   0x00002000uL  // 0x0D
#define ZB_CHANNEL_12   0x00001000uL  // 0x0C
#define ZB_CHANNEL_11   0x00000800uL  // 0x0B

#define LOWEST_ZIGBEE_CHANNEL               11u
#define HIGHEST_ZIGBEE_CHANNEL              26u

#define BEACON_AMOUNT 3u

/***************************************************************************************************
* EXPORTED CONSTANTS
***************************************************************************************************/

extern const N_Address_t c_Addr64;
extern const N_Address_t c_Addr16;
extern const N_Address_t c_AddrAll;
extern const N_Address_t c_AddrRXON;
extern const N_Address_t c_AddrG;

/***************************************************************************************************
* EXPORTED FUNCTIONS
***************************************************************************************************/

/** Subscribe to this component's callbacks.
    \param pCallback Pointer to a callback structure filled with functions to be called.
*/
void N_DeviceInfo_Subscribe(const N_DeviceInfo_Callback_t* pCallback);

/** Return the version string for the ZigBee Platform.
    \returns Pointer to the version string

    To get a correct version string a pre-build action must be run to generate
    N_DeviceInfo_Version.h. This file must a define for N_DEVICE_INFO_PLATFORM_VERSION.
    The version string has the following format.

    (ZigBee Platform release),package_ZigBee (svnversion of package_ZigBee folder),package_Z_Stack (svnversion of package_ZigBee folder),built by (name of user of build machine)

    For the output format of svnversion run svnversion help on the command line.
*/
const char* N_DeviceInfo_GetStackVersion(void);

/** Check if this device is address assignment capable.
    \returns TRUE if address assignment capable, else FALSE.
*/
bool N_DeviceInfo_IsAddressAssignmentCapable(void);

/** Set if this device is address assignment capable.
    \param isAddressAssignmentCapable the address assignment capability.
*/
void N_DeviceInfo_SetAddressAssignmentCapable(bool isAddressAssignmentCapable);

/** Check if this device can be link initiator.
    \returns TRUE if it can be link initiator, else FALSE.
*/
bool N_DeviceInfo_CanBeLinkInitiator(void);

/** Set if this device can be link initiator.
    \param canBeLinkInitiator TRUE if the device can be link initiator, else FALSE.
*/
void N_DeviceInfo_SetCanBeLinkInitiator(bool canBeLinkInitiator);

/** Get the touchlink rssi correction.
    \returns The touchlink rssi correction.
*/
uint8_t N_DeviceInfo_GetTouchlinkRssiCorrection(void);

/** Set the touchlink rssi correction.
    \param correction The touchlink rssi correction.
*/
void N_DeviceInfo_SetTouchlinkRssiCorrection(uint8_t correction);

/** Get the touchlink rssi threshold.
    \returns The touchlink rssi threshold.
*/
int8_t N_DeviceInfo_GetTouchlinkRssiThreshold(void);

/** Set the touchlink rssi threshold.
    \param threshold The touchlink rssi threshold.
*/
void N_DeviceInfo_SetTouchlinkRssiThreshold(int8_t threshold);

/** Get the network pan id.
    \returns The network pan id.
*/
uint16_t N_DeviceInfo_GetNetworkPanId(void);

/** Get the extended network pan id.
    \returns The extended network pan id.
*/
N_Address_ExtendedPanId_t* N_DeviceInfo_GetNetworkExtendedPanId(void);

/** Get the network address.
    \returns The network address.
*/
uint16_t N_DeviceInfo_GetNetworkAddress(void);

/** Get the parents network address.
    \returns The parents network address.
*/
uint16_t N_DeviceInfo_GetParentNetworkAddress(void);

/** Get the network channel.
    \returns The network channel.
*/
uint8_t N_DeviceInfo_GetNetworkChannel(void);

/** Get the network update id.
    \returns The network update id.
*/
uint8_t N_DeviceInfo_GetNetworkUpdateId(void);

/** Get the IEEE address.
    \returns The IEEE address.
*/
const uint8_t* N_DeviceInfo_GetIEEEAddress(void);

/** Get the parents IEEE address.
    \returns The parents IEEE address.
*/
const uint8_t* N_DeviceInfo_GetParentIEEEAddress(void);

/** Get the touchlink 0 dBm transmit power.
    \returns The register value for the corresponding transmit power.
*/
uint8_t N_DeviceInfo_GetTouchlinkZerodBmTransmitPower(void);

/** Check if the device is factory new.
    \returns TRUE if device is factory new, else returns FALSE.
*/
bool N_DeviceInfo_IsFactoryNew(void);

/** Check if the device is an end device.
    \returns TRUE if device is an end device, else returns FALSE.
*/
bool N_DeviceInfo_IsEndDevice(void);

/** Get the primary channel mask.
    \returns The primary channel mask.
*/
uint32_t N_DeviceInfo_GetPrimaryChannelMask(void);

/** Set the primary channel mask.
    \param channelMask The new primary channel mask.
*/
void N_DeviceInfo_SetPrimaryChannelMask(uint32_t channelMask);

/** Get the secondary channel mask.
    \returns The secondary channel mask.
*/
uint32_t N_DeviceInfo_GetSecondaryChannelMask(void);

/** Set the secondary channel mask.
    \param channelMask The new secondary channel mask.
*/
void N_DeviceInfo_SetSecondaryChannelMask(uint32_t channelMask);

/** Get the number of channels in a channel mask (>=1).
    \param channelMask The channelmask to check
    \returns The number of channels in the given channel mask.
*/
uint8_t N_DeviceInfo_GetNrChannelsInChannelMask(uint32_t channelMask);

/** Retrieve one of the channels from a channel mask.
    \param index An index from 0 to N_DeviceInfo_GetNrChannelsInChannelMask()-1
    \param channelMask The channelmask
    \returns The channels from the given channel mask in order, e.g. index 0 will return the lowest channel.
*/
uint8_t N_DeviceInfo_GetChannelForIndex(uint8_t index, uint32_t channelMask);

/** Returns whether a channel is valid according to the current channel mask.
    \param channel The channel to check
    \param channelMask The channelMask to check
    \returns TRUE if the channel is in the mask, otherwise FALSE
*/
bool N_DeviceInfo_IsChannelInMask(uint8_t channel, uint32_t channelMask);

/** Gets the network mode.
    \returns The network mode this device is currently operating in.
    \note Must not be called when the device is factory new.
*/
N_DeviceInfo_TrustCenterMode_t N_DeviceInfo_GetTrustCenterMode(void);

/** Aligns parameters in N_DeviceInfo with configuration server
    \param channel The network channel.
    \param panId The network channel.
    \param extPanId Pointer to the extended network pan id.
    \param nwkAddr The network address.
*/
void N_DeviceInfo_SynchronizeParameters(N_DeviceInfo_SynchronizationParameters_t *params);

/***************************************************************************************************
* PLATFORM INTERNAL INTERFACE
***************************************************************************************************/
/** \privatesection
*/

/** Set the network pan id.
    \param panId The network pan id.
*/
void N_DeviceInfo_SetNetworkPanId(uint16_t panId);

/** Set the extended network pan id.
    \param pExtPanId Pointer to the extended network pan id.
*/
void N_DeviceInfo_SetNetworkExtendedPanId(uint8_t* pExtPanId);

/** Set the network address.
    \param nwkAddress The network address.
*/
void N_DeviceInfo_SetNetworkAddress(uint16_t nwkAddress);

/** Set the parents network address.
    \param parentNwkAddress The parents network address.
*/
void N_DeviceInfo_SetParentNetworkAddress(uint16_t parentNwkAddress);

/** Set the parents ieee address.
    \param ieeeAddress The parents ieee address.
*/
void N_DeviceInfo_SetParentIEEEAddress(N_Address_Extended_t ieeeAddress);

/** Set the network update id.
    \param updateId The network update id.
*/
void N_DeviceInfo_SetNetworkUpdateId(uint8_t updateId);

/** Set the network channel.
    \param nwkChannel The network channel.
*/
void N_DeviceInfo_SetNetworkChannel(uint8_t nwkChannel);

/** Set if the device is factory new.
    \param factoryNew TRUE if device is factory new, else returns FALSE.
*/
void N_DeviceInfo_SetFactoryNew(bool factoryNew);

/** Sets the trust center mode.
    \param trustCenterMode The trust center mode the network is currently operating in.
*/
void N_DeviceInfo_SetTrustCenterMode(N_DeviceInfo_TrustCenterMode_t trustCenterMode);

/** Return the profile ID of the given endpoint
    \param endpoint the endpoint to get the profile ID of
    \returns The profile ID.
*/
N_DeviceInfo_Profile_t N_DeviceInfo_GetProfileIdForEndpoint(uint8_t endpoint);

/***************************************************************************************************
* IMPLEMENTATION OF INLINE FUNCTIONS
***************************************************************************************************/

/** Implementation of \ref N_DeviceInfo_IsEndDevice

    This function is inline to make it possible for the compiler
    to remove router/enddevice only code.
*/
bool N_DeviceInfo_IsEndDevice_Impl(void);

/***************************************************************************************************
* END OF C++ DECLARATION WRAPPER
***************************************************************************************************/

#ifdef __cplusplus
}
#endif

#endif // N_DEVICEINFO_H
