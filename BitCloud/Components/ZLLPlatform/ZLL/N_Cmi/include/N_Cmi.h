/*********************************************************************************************//**
\file

\brief

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal

$Id: N_Cmi.h 24615 2013-02-27 05:14:24Z arazinkov $

***************************************************************************************************/

#ifndef N_CMI_H
#define N_CMI_H

/***************************************************************************************************
* INCLUDE FILES
***************************************************************************************************/

#include "macCommon.h"

#include "N_Address.h"
#include "N_Beacon.h"
#include "N_Security.h"
#include "N_Types.h"
#include "N_Connection.h"

#include "apsdeData.h"
#include "zdo.h"

/***************************************************************************************************
* C++ DECLARATION WRAPPER
***************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif
/***************************************************************************************************
* EXPORTED TYPES
***************************************************************************************************/

typedef enum N_Cmi_Result_t
{
    N_Cmi_Result_Success,
    N_Cmi_Result_Failure,
    N_Cmi_Result_NoNetworkKey,
    N_Cmi_Result_Invalid_Request,
} N_Cmi_Result_t;

typedef struct N_Cmi_Callback_t
{
    /** The end-device has detected that it has lost its parent.
    */
    void (*SyncLoss)(void);

    /** Indicate that the device is going to join a (new) network.

        This is called when \ref N_Cmi_StartNetworking is called.
        The callback can be used by platform components to reset variables.
        Application components can use the callback from N_DeviceInfo to
        see if a network is joined. This ensures that the platform components
        get a notification before the application components.
    */
    void (*StartNetworking)(void);

    /** The device has received a network key, for example after joining a network via association.
        \param networkKeySequenceNumber The sequence number of the received network key
    */
    void (*NetworkKeyIndication)(uint8_t networkKeySequenceNumber);

    /** The device has received a new network address (probably after an address conflict).
        \param networkAddress The new network address
    */
    void (*NetworkAddressChanged)(uint16_t networkAddress);

    /** The device has received a leave request or leave indication.
        \param leaveIndication The leave request/indication
    */

    /** Guard. */
    int8_t endOfList;

} N_Cmi_Callback_t;

/** Indicates that a beacon has been received during \ref N_Cmi_NetworkDiscovery.
    \param beaconIndication The beacon
*/
typedef void (*N_Cmi_BeaconIndication_t)(N_Beacon_t* beaconIndication);

/** Indicates that the \ref N_Cmi_NetworkDiscovery has completed.
*/
typedef void (*N_Cmi_NetworkDiscoveryDone_t)(void);

/** Indicates that a \ref N_Cmi_Join, \ref N_Cmi_Rejoin, \ref N_Cmi_RejoinParent or \ref N_Cmi_OrphanJoin has completed.
    \param result The result of the action
*/
typedef void (*N_Cmi_JoinDone_t)(N_Cmi_Result_t result);

/** Indicates that a \ref N_Cmi_StartNetworking has completed.
    \param result The result of the action
*/
typedef void (*N_Cmi_StartRouterDone_t)(N_Cmi_Result_t result);

/** Indicates that a \ref N_Cmi_SetMacChannel or N_Cmi_UpdateNetworkChannel has completed successfully.
*/
typedef void (*N_Cmi_SetChannelDone_t)(void);

/** Indicates that ZLL preinstalled TC link key was set marked as a primary global key.
*/
typedef void (*N_Cmi_SetZllLinkKeyAsPrimaryDone_t)(void);

/** Beacon filtering modes.
*/
typedef enum _N_Cmi_BeaconFilteringCriterion_t
{
  BEACON_FILTERING_CRITERION_NONE,
  BEACON_FILTERING_CRITERION_ASSOCIATION,
  BEACON_FILTERING_CRITERION_NETWORK_SEARCH,
  BEACON_FILTERING_CRITERION_RECONNECT
} N_Cmi_BeaconFilteringCriterion_t;

/***************************************************************************************************
* EXPORTED FUNCTIONS
***************************************************************************************************/

/** Subscribe for events from this component. */
void N_Cmi_Subscribe(const N_Cmi_Callback_t* pSubscriber);

/** Set the MAC channel. */
void N_Cmi_SetMacChannel(uint8_t channel, N_Cmi_SetChannelDone_t pfDoneCallback);

/** Get the MAC channel. */
uint8_t N_Cmi_GetMacChannel(void);

/** Start this device (router/end-device) in a ZigBee network.
*/
N_Cmi_Result_t N_Cmi_StartNetworking(N_Address_Extended_t pExtendedPanId,
                                     uint8_t channel,
                                     uint16_t panId,
                                     uint16_t networkAddress,
                                     uint8_t networkUpdateId,
                                     N_Security_Key_t networkKey);

/** Start routing on this device.
    \param pfDoneCallback Pointer to the function that will be called when this function is done

    A router device is not fully operational until this function returns success.

    \note This function is only implemented on a router
*/
void N_Cmi_StartRouter(N_Cmi_StartRouterDone_t pfDoneCallback);

/** Perform a network discovery (active scan).
    \param channelMask The channels to scan
    \param durationPerChannel The time to stay at each channel (enum)
    \param pBeacon Pointer to memory that is used to return a beacon from an open network.
    \param mode The discovery mode. Can be used to blacklist or specify a network.
    \param extendedPanId Extended pan id to join or to blacklist depending on the mode parameter.
    \param filteringMode Beacon filtering mode - specifies the criterion for beacon handling.
    \param pfBeaconReceivedCallback Pointer to the function that should be called on beacon reception.
    \param pfDoneCallback Pointer to the function that should be called after the network discovery is done
*/
void N_Cmi_NetworkDiscovery(uint32_t channelMask,
                            N_Beacon_Order_t durationPerChannel,
                            N_Beacon_t* pBeacon,
                            N_Connection_AssociateDiscoveryMode_t mode,
                            N_Address_ExtendedPanId_t* extendedPanId,
                            N_Cmi_BeaconFilteringCriterion_t filteringMode,
                            N_Cmi_BeaconIndication_t pfBeaconReceivedCallback,
                            N_Cmi_NetworkDiscoveryDone_t pfDoneCallback);

/** Perform a ZigBee network join (MAC association).
    \param pSelectedBeacon The beacon from a device to join to.
    \param pfDoneCallback Pointer to the function that should be called after the join is done
*/
void N_Cmi_Join(N_Beacon_t* pSelectedBeacon, N_Cmi_JoinDone_t pfDoneCallback);

/** Perform a ZigBee network rejoin to find a new parent (end-device only).
    \param pSelectedBeacon The beacon from a device to rejoin to.
    \param secure Whether to do a secure rejoin (TRUE) or an unsecure rejoin (FALSE, only possible with central trustcenter)
    \param pfDoneCallback Pointer to the function that should be called after the rejoin is done

    \note This function is not implemented on a router
*/
void N_Cmi_Rejoin(N_Beacon_t* pSelectedBeacon, bool secure, N_Cmi_JoinDone_t pfDoneCallback);

/** Perform a ZigBee network rejoin with the old parent (end-device only).

    \note This function is not implemented on a router
*/
void N_Cmi_RejoinParent(N_Cmi_JoinDone_t pfDoneCallback);

/** Perform a IEEE 802.15.4 orphan join to rejoin the old parent (end-device only).
    \param pfDoneCallback Pointer to the function that should be called after the rejoin is done

    \note This function is not implemented on a router
*/
void N_Cmi_OrphanJoin(N_Cmi_JoinDone_t pfDoneCallback);

/** Update the network channel and network update id
    \param channel The channel to change to.
    \param networkUpdateId The network update id to update to.
    \param pfDoneCallback Callback that is called after completion. Allowed to be NULL (no callback).
*/
void N_Cmi_UpdateNetworkChannel(uint8_t channel, uint8_t networkUpdateId, N_Cmi_SetChannelDone_t pfDoneCallback);

/** Add an end device to the child table.
    \param ieeeAddressEndDevice The IEEE address of the end device to add to the child table
    \param networkAddressEndDevice The network address of the end device to add to the child table
    \note Calling this function for an end device is a fatal error.
*/
void N_Cmi_AddChild(N_Address_Extended_t ieeeAddressEndDevice, uint16_t networkAddressEndDevice);

/** Set the parents short address
    \param networkAddress Parents new short address
*/
void N_Cmi_SetParentNetworkAddress(uint16_t networkAddress);

/** Send a link status.
    \param delay The delay before sending the link status
*/
void N_Cmi_SendLinkStatus(uint16_t delay);

/** Retrieves the current network key.
    \param[out] networkKey The current network key
*/
void N_Cmi_GetNetworkKey(N_Security_Key_t networkKey);

/** Clear the network settings, thus setting the network layer back to factory defaults.
*/
void N_Cmi_ResetNetworkSettings(void);

/** Get the network params from the NIB.
    \param[out] pNetworkAddress The current network address
    \param[out] pPanId The current network pan id
    \param[out] pExtendedPanId The current network extended pan id
    \param[out] pNetworkUpdateId The current network update id
    \param[out] pNetworkChannel The current network channel
*/
void N_Cmi_GetNetworkParams(uint16_t* pNetworkAddress, uint16_t* pPanId, N_Address_Extended_t* pExtendedPanId,
                            uint8_t* pNetworkUpdateId, uint8_t* pNetworkChannel);

/** Get the parent network params from the NIB.
    \param[out] pParentNetworkAddress The current parent's network address
    \param[out] pParentExtendedAddress The current parent's network extended address
*/
void N_Cmi_GetParentInfo(uint16_t* pParentNetworkAddress, N_Address_Extended_t* pParentExtendedAddress);

/** Switch to the network key with the given sequence number
    \param networkKeySequenceNumber The sequence number of the key to use
*/
void N_Cmi_SwitchNetworkKey(uint8_t networkKeySequenceNumber);

/** Set the network manager network address
    \param networkManagerAddress New address for the network manager
*/
void N_Cmi_SetNetworkManager(uint16_t networkManagerAddress);

/** Retrieve the amount of network transmissions from the NIB */
uint16_t N_Cmi_GetTotalTransmissions(void);

/** Send a leave indication for device
    \param rejoin Decides the device information need to be removed or not.
    \param removeChildren remove the associated Childrens with the device or not.
*/
void N_Cmi_SendLeaveIndication(bool rejoin, bool removeChildren);

/** Send an UpdateDevice command.
    \param ieeeAddr of the device whose status is being updated.
    \param shortAddr of the device whose status is being updated.
    \param status new status of the device.
*/
void N_Cmi_SendUpdateDevice(uint8_t* ieeeAddr, uint16_t shortAddr, uint8_t status);

/** Update the device security status to authenticated.
    \param nwkAddr the network address of the device
*/
void N_Cmi_AuthenticateChild(uint16_t nwkAddr);

/** Send APS data. Wrapper for APS_DataReq with the parameter transmission option set as own broadcast transmissions. 
    \param[in] apsDataReq - APS data request pointer
*/
void N_Cmi_DataRequest(APS_DataReq_t *apsDataReq);

/** Send ZDP request. */
void N_Cmi_ZdpRequest(ZDO_ZdpReq_t *zdpReq);

/** Sets indirect poll rate for a given time interval; then restores the default poll rate.
    \param[in] rate - poll rate to be set
    \param[in] time - time interval for poll rate, ms
*/
void N_Cmi_SetPollRateForTimePeriod(uint32_t rate, uint32_t time);

/** Sets the type of groupcast to be used - NWK multicast or APS Groupcast.
    \param[in] multicast - if true NWK multicast will be used.
*/
void N_Cmi_UseNwkMulticast(bool multicast);

/** Marks ZLL preinstalled TC link key as a primary global key for a PAN
 *  \param doneCallback Pointer to the function that should be called after the setting is done
*/
void N_Cmi_SetZllLinkKeyAsPrimary(N_Cmi_SetZllLinkKeyAsPrimaryDone_t doneCallback);

/** ZLL Platform to BitCloud addresses convertion
 *  \param[in] platformAddressing ZLL Platform addressing pointer
 *  \param[out] bcAddress BitCloud addressing pointer
*/
void N_Cmi_PlatformToBcAddressingConvert(APS_Address_t *bcAddress, N_Address_t *platformAddressing);

/** ZLL Platform to BitCloud address mode convertion
 *  \param platformAddrMode ZLL Platform address mode
 *
 *  \return relative BitCloud address mode
*/
APS_AddrMode_t N_Cmi_PlatformToBcAddressModeConvert(N_Address_Mode_t platformAddrMode);

/***************************************************************************************************
* END OF C++ DECLARATION WRAPPER
***************************************************************************************************/

#ifdef __cplusplus
}
#endif

#endif // N_CMI_H

