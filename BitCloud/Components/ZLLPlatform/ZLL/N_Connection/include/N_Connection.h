/*********************************************************************************************//**
\file

\brief Create and maintain a connection with a ZigBee network.

\section Remarks
\par
There are two implementations of this interface: router and end-device.

\subsection Router
\par
A router does not need any other device to join a network: just set the
network parameters by calling \ref N_Connection_JoinNetwork.

\subsection End-device
\par
An end-device needs a parent to be able to communicate with other devices
in the network. This component provides the interface to find a parent after
joining a network and find a new parent if the old one is no longer available.

\par
Joining a network with \ref N_Connection_JoinNetwork will trigger a search
for a parent. A search for a new parent can be triggered by calling
\ref N_Connection_Reconnect.


\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal
$Id: N_Connection.h 24615 2013-02-27 05:14:24Z arazinkov $
***************************************************************************************************/

#ifndef N_CONNECTION_H
#define N_CONNECTION_H

/***************************************************************************************************
* INCLUDE FILES
***************************************************************************************************/

#include "N_Address.h"
#include "N_Beacon.h"
#include "N_Types.h"

/***************************************************************************************************
* C++ DECLARATION WRAPPER
***************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************************************
* EXPORTED MACROS AND CONSTANTS
***************************************************************************************************/

#define N_CONNECTION_NETWORK_CHANNEL        0u

/***************************************************************************************************
* EXPORTED TYPES
***************************************************************************************************/

/** Result codes for the N_Connection actions. */
typedef enum N_Connection_Result_t
{
    N_Connection_Result_Success,
    N_Connection_Result_Failure,
    N_Connection_Result_Aborted,
    N_Connection_Result_NoNetworkKey,
    N_Connection_Invalid_Request,
} N_Connection_Result_t;

/** Possible Inter-PAN modes. */
typedef enum N_Connection_InterPanMode_t
{
    N_Connection_InterPanMode_Off,
    N_Connection_InterPanMode_On,
} N_Connection_InterPanMode_t;

/** Filters for associate discovery. */
typedef enum N_Connection_AssociateDiscoveryMode_t
{
    N_Connection_AssociateDiscoveryMode_AnyPan,
    N_Connection_AssociateDiscoveryMode_NotThisPan,
    N_Connection_AssociateDiscoveryMode_OnlyThisPan,
} N_Connection_AssociateDiscoveryMode_t;

/** Confirms that \ref N_Connection_JoinNetwork has completed.
    \param result Result of the action (Success or Failure)
*/
typedef void (*N_Connection_JoinNetworkDone_t)(N_Connection_Result_t result);

/** Confirms that \ref N_Connection_AssociateDiscovery has completed.
    \param result Result of the action (Success, Failure or Aborted)
*/
typedef void (*N_Connection_AssociateDiscoveryDone_t)(N_Connection_Result_t result);

/** Confirms that \ref N_Connection_AssociateJoin has completed.
    \param result Result of the action (Success, Failure or NoNetworkKey)

    \note The result NoNetworkKey is given if a devices joins via associate
          and did not receive a network key in time.
*/
typedef void (*N_Connection_AssociateJoinDone_t)(N_Connection_Result_t result);

/** Confirms that \ref N_Connection_UpdateNetwork has completed.
*/
typedef void (*N_Connection_UpdateNetworkDone_t)(void);

/** Confirms that \ref N_Connection_FindFreePan has completed and provides the result.
    \param panId The chosen PAN ID
    \param extendedPanId The chosen Extended PAN ID
*/
typedef void (*N_Connection_FindFreePanDone_t)(uint16_t panId, N_Address_ExtendedPanId_t extendedPanId);

/** Confirm callback type for the SetInterPanMode functions. */
typedef void (*N_Connection_SetInterPanModeDone_t)(void);

/** Callbacks of this component. */
typedef struct N_Connection_Callback_t
{
    /** Notification that the device has been (re)connected to the network.

        This is called in the following situations:

        1. A router is ready, just after the JoinNetworkDone callback.

        2. An end-device has a (re)connection to a parent.
    */
    void (*Connected)(void);

    /** Notification that the device has lost it connection to the network.

        The function is called in the following situations:

        1. A non-factory new device joins a new network, just before the
           new network settings are stored.

        2. An end-device has lost the connection with its parent (sync loss).

        \note This component will not automatically try to find a new parent
              on a disconnect. This auto-reconnect can be implemented in the
              application by calling the Reconnect function from the Disconnected
              callback.
    */
    void (*Disconnected)(void);

    /** Notification that the device has joined a (new) network.

        The function is called when the device has successfully joined the
        network, just after the JoinNetworkDone callback.
        On a router the Connected and NetworkJoined callbacks always come
        at the same time. On an end-device the Connected callback may come
        much later than the NetworkJoined callback.

        Use this callback to set-up the network settings that are reset by
        joining a network (e.g. default groups, default bindings).
    */
    void (*NetworkJoined)(void);

    /** Notification that the device needs to reset.

        The function is called after the device has received a leave request and is
        not allowed to rejoin again.
        The application should reset the device to factory defaults, because that's the only way to
        fully get out of the network.
    */
    void (*ResetToFactoryDefaults)(void);

    /** Notification that a device has associated with this device. 

        This callback is called when a child joins with this device.
    */
    void (*ChildAssociated)(uint16_t networkAddress, N_Address_Extended_t ieeeAddress);


    int8_t endOfList;
} N_Connection_Callback_t;

/***************************************************************************************************
* EXPORTED FUNCTIONS
***************************************************************************************************/

// Platform public interface (also used internally)

/** Subscribe for events from this component.
*/
void N_Connection_Subscribe(const N_Connection_Callback_t* pSubscriber);

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
void N_Connection_JoinNetwork(N_Address_Extended_t extendedPanId,
                              uint8_t channel,
                              uint16_t panId,
                              uint16_t networkAddress,
                              uint8_t networkUpdateId,
                              uint8_t* pNetworkKey,
                              N_Connection_JoinNetworkDone_t pfDoneCallback);

/** Find a beacon of an open ZigBee Pro network.
    \param pBeacon Pointer to memory that is used to return a beacon from an open network.
    \param mode The discovery mode. Can be used to blacklist or specify a network.
    \param extendedPanId Extended pan id to join or to blacklist depending on the mode parameter.
    \param pfDoneCallback Callback function pointer that is called upon completion. \see
           N_Connection_AssociateDiscoveryDone_t.

    This is an asynchronous call: do not call any other function of this
    component except for N_Connection_AbortAssociateDiscovery until the callback is received.
*/
void N_Connection_AssociateDiscovery(N_Beacon_t* pBeacon, N_Connection_AssociateDiscoveryMode_t mode, N_Address_ExtendedPanId_t* extendedPanId, N_Connection_AssociateDiscoveryDone_t pfDoneCallback);

/** Abort the discovery of open ZigBee Pro networks.

    This is an asynchronous call: do not call any other function of this
    component until the N_Connection_AssociateDiscoveryDone_t callback is received.
*/
void N_Connection_AbortAssociateDiscovery(void);

/** Associate with an open ZigBee Pro network.
    \param pBeacon Beacon of the device to associate with.
    \param pfDoneCallback Callback function pointer that is called upon completion. \see
           N_Connection_AssociateJoinDone_t.
*/
void N_Connection_AssociateJoin(N_Beacon_t* pBeacon, N_Connection_AssociateJoinDone_t pfDoneCallback);

/** Update network channel/update id.
    \param networkChannel The new network channel
    \param networkUpdateId The new network update id
    \param pfDoneCallback Pointer to the function that will be called when the action is done

    This is an asynchronous call: do not call any other function of this
    component until the callback is received.
*/
void N_Connection_UpdateNetwork(uint8_t networkChannel,
                                uint8_t networkUpdateId,
                                N_Connection_UpdateNetworkDone_t pfDoneCallback);

/** Check if the device has a connection to the network (end-device has a parent).
    \returns TRUE if the device is a non-factory-new router, or if it has a parent
*/
bool N_Connection_IsConnected(void);

/** Try to find a parent (end-device only, no-op on a router).
*/
void N_Connection_Reconnect(void);

/** Set the poll rate to be used when polling.
    \param pollIntervalMs The poll interval in milliseconds, or 0 to disable polling.

    Polling is an end-device-only feature. On routers, it is a dummy function.
    When the poll rate is never set, the default of 1000ms is used.
*/
void N_Connection_SetPollRate(uint32_t pollIntervalMs);

/** Force to use the application defined poll rate after joining a network.

    When an end-device is touchlinked, the pollrate is normally set to 300 milliseconds
    for a period of 5 seconds. This is to ensure that the end-device will receive the
    "communication check"-messages from the other device. The application is able
    to stop this fast polling mode by calling this function.
    In this case the application is responsible for the correct poll rate.

    Polling is an end-device-only feature. On routers, it is a dummy function.
*/
void N_Connection_ForceNormalPolling(void);

/** Finds a PAN ID and Extended PAN ID that is unique on all channels of the channel mask, in the RF proximity.

    \param pfDoneCallback Pointer to the function that will be called when the action is done

    This typically takes 240ms per channel in the mask. In extremely rare conditions, it can take longer.
    Only implemented on routers.
*/
void N_Connection_FindFreePan(N_Connection_FindFreePanDone_t pfDoneCallback);

/** Request to turn on Inter-PAN mode (asynchronous call).
    \param channel The requested ZigBee channel
    \param pConfirmCallback Pointer to the function called when the request has been handled

    After the application is done sending/receiving Inter-PAN messages
    \ref N_Connection_SetInterPanModeOff must be called.
*/
void N_Connection_SetInterPanModeOn(uint8_t channel, N_Connection_SetInterPanModeDone_t pConfirmCallback);

/** Request to leave Inter-PAN mode (asynchronous call).
    \param pConfirmCallback Pointer to the function called the when request has been handled

    This function must be called is when the application is done
    sending/receiving Inter-PAN messages.
*/
void N_Connection_SetInterPanModeOff(N_Connection_SetInterPanModeDone_t pConfirmCallback);


/** Send Network Leave Request.
    \param rejoin Set to TRUE to indicate the device will rejoin immediately after leaving
    \param removeChildren Set to TRUE to indicate also the children of this device are removed
*/
void N_Connection_SendNetworkLeaveRequest(bool rejoin, bool removeChildren);

/** Processes postponed setting interPan mode to ON state if required.
*/
void N_Connection_ProcessPostponedInterPanMode(void);

/** Returns the state of Update Network.
*/
bool N_Connection_UpdateNetwork_IsBusy(void);

/***************************************************************************************************
* END OF C++ DECLARATION WRAPPER
***************************************************************************************************/

#ifdef __cplusplus
}
#endif

#endif // N_CONNECTION_H
