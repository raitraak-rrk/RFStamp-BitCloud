/*********************************************************************************************//**
\file

\brief Interface for N_ReconnectHandler or a custom reconnect-handler to find a parent

\section Remarks
\par
Only available on end-devices.

\subsection Callbacks
\par
The interface contains one callback: Reconnect(), which is called whenever N_Connection
needs to find a new parent. It is the responsibility of the reconnect-handler
to find a new parent. This sequence may be very product-specific. Deviations from the
standard reconnect-handler can be made by not initialising the N_ReconnectHandler but
using a project-specific reconnect-handler.

\par
When an end-device joins a new network, it does not call the ReconnectHandler, but instead
tries to find a parent in a fixed way.

The following sequence diagram shows the relation between N_Connection and a reconnect-handler
\image html N_Connection_ReconnectInterface.png

<!--
@startuml N_Connection_ReconnectInterface.png

N_Connection -> ReconnectHandler : ^Reconnect

group Application-specific example sequence
ReconnectHandler -> N_Connection : RejoinParent()
N_Connection -> ReconnectHandler : ^JoinDone(failure)
ReconnectHandler -> N_Connection : RejoinParent()  // retry
N_Connection -> ReconnectHandler : ^JoinDone(failure)
ReconnectHandler -> N_Connection : NetworkDiscovery(channel mask)
N_Connection -> ReconnectHandler : ^NetworkDiscoveryDone(network found)
ReconnectHandler -> N_Connection : Rejoin()
N_Connection -> ReconnectHandler : ^JoinDone(success)
end

ReconnectHandler -> N_Connection : ReconnectDone(N_Connection_Result_t status)

@enduml
-->

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal
$Id: N_Connection_ReconnectHandler.h 24615 2013-02-27 05:14:24Z arazinkov $
***************************************************************************************************/

#ifndef N_CONNECTION_RECONNECTHANDLER_H
#define N_CONNECTION_RECONNECTHANDLER_H

/***************************************************************************************************
* INCLUDE FILES
***************************************************************************************************/

#include "N_Beacon.h"
#include "N_Connection.h"
#include "N_Types.h"

/***************************************************************************************************
* C++ DECLARATION WRAPPER
***************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************************************
* EXPORTED TYPES
***************************************************************************************************/

/** Indicates that the \ref N_Connection_ReconnectHandler_NetworkDiscovery has completed.
*/
typedef void (*N_Connection_ReconnectHandler_NetworkDiscoveryDone_t)(void);

/** Indicates that the a beacon has been received during \ref N_Connection_ReconnectHandler_NetworkDiscovery.
    \param beacon The beacon that has been received
*/
typedef void (*N_Connection_ReconnectHandler_BeaconReceived_t)(N_Beacon_t* beacon);

/** Indicates that a \ref N_Connection_ReconnectHandler_Rejoin, \ref N_Connection_ReconnectHandler_RejoinParent or \ref N_Connection_ReconnectHandler_OrphanJoin has completed.
    \param result The result of the action
*/
typedef void (*N_Connection_ReconnectHandler_JoinDone_t)(N_Connection_Result_t result);

/** Callbacks of the reconnect-handler. */
typedef struct N_Connection_ReconnectHandler_Callback_t
{
    /** Notification that the device has lost its connection to a parent and needs to reconnect.
    */
    void (*Reconnect)(void);

    int8_t endOfList;
} N_Connection_ReconnectHandler_Callback_t;

/***************************************************************************************************
* EXPORTED FUNCTIONS
***************************************************************************************************/

// Platform public interface

/** Subscribe for events for the reconnect-handler
    \param pSubscriber The subscriber for the interface
    \note Only one subscriber is allowed on this interface.
*/
void N_Connection_ReconnectHandler_Subscribe(const N_Connection_ReconnectHandler_Callback_t* pSubscriber);

/** Perform a network discovery (active scan), returning only devices of the own PAN.
    \param channelMask The channels to scan
    \param durationPerChannel The time to stay at each channel (enum)
    \param pfBeaconReceivedCallback Pointer to the function that should received incoming beacon indications during the network discovery
    \param pfDoneCallback Pointer to the function that should be called after the network discovery is done
    \note Only allowed between the Reconnect callback and the N_Connection_ReconnectHandler_ReconnectDone call.
*/
void N_Connection_ReconnectHandler_NetworkDiscovery(uint32_t channelMask,
                                                    N_Beacon_Order_t durationPerChannel,
                                                    N_Connection_ReconnectHandler_BeaconReceived_t pfBeaconReceivedCallback,
                                                    N_Connection_ReconnectHandler_NetworkDiscoveryDone_t pfDoneCallback);

/** Perform a ZigBee network rejoin to a new parent.
    \param pSelectedBeacon The beacon of the device to rejoin to
    \param pfDoneCallback Pointer to the function that should be called after the rejoin is done
    \note Only allowed between the Reconnect callback and the N_Connection_ReconnectHandler_ReconnectDone call.
*/
void N_Connection_ReconnectHandler_Rejoin(N_Beacon_t* pSelectedBeacon, N_Connection_ReconnectHandler_JoinDone_t pfDoneCallback);

/** Perform a ZigBee network rejoin with the old parent.
    \note Only allowed between the Reconnect callback and the N_Connection_ReconnectHandler_ReconnectDone call.
*/
void N_Connection_ReconnectHandler_RejoinParent(N_Connection_ReconnectHandler_JoinDone_t pfDoneCallback);

/** Perform a IEEE 802.15.4 orphan join to rejoin the old parent.
    \param pfDoneCallback Pointer to the function that should be called after the rejoin is done
    \note Only allowed between the Reconnect callback and the N_Connection_ReconnectHandler_ReconnectDone call.
*/
void N_Connection_ReconnectHandler_OrphanJoin(N_Connection_ReconnectHandler_JoinDone_t pfDoneCallback);

/** Perform a ZigBee network unsecure rejoin to a parent router.
    \param pSelectedBeacon The beacon of the device to rejoin to
    \param pfDoneCallback Pointer to the function that should be called after the rejoin is done (including authentication)
    \note Only allowed between the Reconnect callback and the N_Connection_ReconnectHandler_ReconnectDone call.
*/
void N_Connection_ReconnectHandler_UnsecureRejoin(N_Beacon_t* pSelectedBeacon, N_Connection_ReconnectHandler_JoinDone_t pfDoneCallback);

/** Indicates that the reconnect-handler has finished finding a parent.
    \param status Indicates N_Connection_Result_Success (a parent was found) or N_Connection_Result_Failure (no parent found)
    \note Only allowed after a Reconnect callback.
*/
void N_Connection_ReconnectHandler_ReconnectDone(N_Connection_Result_t status);

/***************************************************************************************************
* END OF C++ DECLARATION WRAPPER
***************************************************************************************************/

#ifdef __cplusplus
}
#endif

#endif // N_CONNECTION_RECONNECTHANDLER_H
