/*********************************************************************************************//**
\file

\brief Create and maintain a connection with a ZigBee network. Platform internal interface.

\section Remarks
\subsection Inter-PAN mode
\par
The component has a special mode for sending Inter-PAN messages. This
mode is selected by the N_LinkInitiator and N_LinkTarget components to
send/receive Inter-PAN messages.

\par
An end-device keeps the radio turned on in Inter-PAN mode, stops
searching for a parent and stops polling its parent, so an end-device
cannot send and receive normal ZigBee messages in Inter-PAN mode.

\par
A router does not do anything special in Inter-PAN mode, so a router can
send and received in normal ZigBee messages in Inter-PAN mode.

\par
To make it possible to control the Inter-PAN mode from the two different
components (N_LinkInitiator and N_LinkTarget), each component has its own
set of functions:
\ref N_Connection_SetInitiatorInterPanModeOn and
\ref N_Connection_SetInitiatorInterPanModeOff for N_LinkInitiator,
\ref N_Connection_SetTargetInterPanModeOn and
\ref N_Connection_SetTargetInterPanModeOff for N_LinkTarget.

\par
When both N_LinkInitiator and N_LinkTarget request Inter-PAN mode,
then the channel requested by N_LinkInitiator is used.

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal
$Id: N_Connection_Internal.h 24662 2013-03-04 08:37:46Z arazinkov $
***************************************************************************************************/

#ifndef N_CONNECTION_INTERNAL_H
#define N_CONNECTION_INTERNAL_H

/***************************************************************************************************
* INCLUDE FILES
***************************************************************************************************/

#include "N_Types.h"

/***************************************************************************************************
* C++ DECLARATION WRAPPER
***************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************************************
* INTERNAL CONSTANTS
***************************************************************************************************/

/** Time to wait for a network key after a AssociateResp before giving up. */
#define TIMEOUT_NETWORKKEYRECEIVED_MS 5000u

/** Maximum number of retries while associating. */
#define MAX_ASSOCIATE_RETRIES 3u

/** Maximum number of discovery retries. */
#define MAX_DISCOVERY_RETRIES 4u

/***************************************************************************************************
* EXPORTED FUNCTIONS
***************************************************************************************************/

/** Try to find a parent ugently, e.g. after touchlinking (end-device only, no-op on a router).

    Call from N_LinkInitiator on Non-Factory-New devices, just before
    starting to send the dummy unicast messages.
*/
void N_Connection_ReconnectUrgent(void);

/** Request to turn on Inter-PAN mode and set the communication channel from N_LinkInitiator (asynchronous call).
    \param channel The requested ZigBee channel
    \param pConfirmCallback Pointer to the function called when the request has been handled

    An Inter-PAN channel must be selected before sending Inter-PAN messages,
    even if the selected channel is the same as the network channel. Do NOT
    call \ref N_Cmi_SetMacChannel directly from N_LinkInitiator, this function
    can be called to change the communication channel when the Inter-PAN mode
    is already on.

    After the component is done sending/receiving Inter-PAN messages
    \ref N_Connection_SetInitiatorInterPanModeOff must be called.
*/
void N_Connection_SetInitiatorInterPanModeOn(uint8_t channel, N_Connection_SetInterPanModeDone_t pConfirmCallback);

/** Request to leave Inter-PAN mode from N_LinkInitiator (asynchronous call).
    \param pConfirmCallback Pointer to the function called the when request has been handled

    This function must be called is when the component is done sending Inter-PAN messages.
*/
void N_Connection_SetInitiatorInterPanModeOff(N_Connection_SetInterPanModeDone_t pConfirmCallback);

/** Request to turn on Inter-PAN mode and set the communication channel from N_LinkTarget (asynchronous call).
    \param channel The requested ZigBee channel
    \param pConfirmCallback Pointer to the function called when the request has been handled

    An Inter-PAN channel must be selected before sending Inter-PAN messages,
    even if the selected channel is the same as the network channel. Do NOT
    call \ref N_Cmi_SetMacChannel directly from N_LinkTarget, this function
    can be called to change the communication channel when the Inter-PAN mode
    is already on.

    After the component is done sending/receiving Inter-PAN messages
    \ref N_Connection_SetTargetInterPanModeOff must be called.
*/
void N_Connection_SetTargetInterPanModeOn(uint8_t channel, N_Connection_SetInterPanModeDone_t pConfirmCallback);

/** Request to leave Inter-PAN mode from N_LinkTarget (asynchronous call).
    \param pConfirmCallback Pointer to the function called the when request has been handled

    This function must be called is when the component is done
    sending/receiving Inter-PAN messages.
*/
void N_Connection_SetTargetInterPanModeOff(N_Connection_SetInterPanModeDone_t pConfirmCallback);


/** Performs network discovery and channel change if the network
    which this device is assotiated with works on other channel.
    Actual only for router.
*/
void N_Connection_SearchNetwork(void);

/** Performs leave network request.
*/
void N_Connection_LeaveNetwork(void);

/** Postpones setting interPan mode to ON state.
*/
void N_Connection_PostponeInterPanMode(uint8_t channel, N_Connection_SetInterPanModeDone_t pConfirmCallback);

/** change Target channel to macChannel
*/
void N_Connection_SetTargetInterPanMacChannel(void);

/***************************************************************************************************
* END OF C++ DECLARATION WRAPPER
***************************************************************************************************/

#ifdef __cplusplus
}
#endif

#endif // N_CONNECTION_INTERNAL_H
