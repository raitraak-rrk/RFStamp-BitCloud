/*********************************************************************************************//**
\file

\brief React to touchlink actions.

A linking sequence consists roughly of the following steps:
\image html N_LinkTarget_LinkingSequence.png
<!--
@startuml N_LinkTarget_LinkingSequence.png

participant "Initiator" as i
participant "Target" as t

group Scanning
  i->t : ScanRequest
  t->i : ScanResponse
end
group Identify (optional)
  i->t : IdentifyRequest
end
group Add device
  i->t : NetworkJoinRequest
  t->i : NetworkJoinResponse
end
@enduml
-->

The N_LinkTarget component implements the software interface to implement the target-side of this.

\section IIL Being Initiator and Target simultaneously
\par
When starting a linking action from 2 devices at the same time (e.g. linking two ZLL remotes), one
of the devices should become target (to be decided by the application) and the other one should
continue being an initiator.
So when a device that is both target and initiator at the same time receives a scan request while it
is scanning, the application can decide to abort scanning (and remain target) so it can be linked.

Only during the Scanning step, a device may be Target and Initiator at the same time. During the
other steps, the device must give up being a Target.

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal
$Id: N_LinkTarget.h 24615 2013-02-27 05:14:24Z arazinkov $
***************************************************************************************************/

#ifndef N_LINKTARGET_H
#define N_LINKTARGET_H

/***************************************************************************************************
* INCLUDE FILES
***************************************************************************************************/

#include "N_Interpan.h"
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

#define N_LinkTarget_TargetType_None            0x00u
#define N_LinkTarget_TargetType_Touchlink       0x02u
typedef uint8_t N_LinkTarget_TargetType_t;

/** Completion callback for N_LinkTarget_Enable. */
typedef void (*N_LinkTarget_SetTargetTypeDone_t)(void);

typedef struct N_LinkTarget_Callback_t
{
    /** Notify the application that the device has been scanned.
    */
    void (*ScanIndication)(N_InterPan_ScanRequest_t* pScanRequest);

    /** Requests the application to identify itself with the given timeout.
        \param timeoutInSec The requested timeout in seconds
    */
    void (*IdentifyStartIndication)(uint16_t timeoutInSec);

    /** Requests the application to stop identifying itself.
    */
    void (*IdentifyStopIndication)(void);

    /** Requests the application to reset itself to factory defaults.
    */
    void (*ResetIndication)(void);

    /** Indication that the device has been linked to a new network by the initiator.
        \param groupIdFirst The first group identifier assigned to the device during linking.
            Zero if no groups where assigned
        \param groupIdLast The last group identifier assigned to the device during linking.
            Zero if no groups where assigned
        \note The target type has been set to None, see \ref N_LinkTarget_SetTargetType.
    */
    void (*JoinNetworkIndication)(uint16_t groupIdFirst, uint16_t groupIdLast);

    /** Indication that the network settings of the device have been updated by the initiator.
        \note The target type has been set to None, see \ref N_LinkTarget_SetTargetType.
    */
    void (*UpdateNetworkIndication)(void);

    /** Guard. Must be -1. */
    int8_t endOfList;

} N_LinkTarget_Callback_t;

/***************************************************************************************************
* EXPORTED MACROS AND CONSTANTS
***************************************************************************************************/


/***************************************************************************************************
* EXPORTED FUNCTIONS
***************************************************************************************************/

/** Subscribe to indication callbacks from this component.
    \param pCb Pointer to structure with callback functions
*/
void N_LinkTarget_Subscribe(const N_LinkTarget_Callback_t* pCb);

/** Set the link target type.
    \param targetType The target type (none, touchlink)
    \param pfDoneCallback Pointer to function called when the asynchronous call is done

    For a router this is typically called during initialisation to allow touchlinking.
    For an end-device this is not possible as Inter-Pan communication cannot
    be combined with normal ZigBee communication, so touchlinking should only be allowed
    for a short period after a explicit user action.

    \note The target type is automatically set to None if the device is linked
        to a new network, when the network settings are updated.
        On a router the \ref N_LinkTarget_Callback_t::JoinNetworkIndication and
        \ref N_LinkTarget_Callback_t::UpdateNetworkIndication callbacks
        can be used to detect this and immediately allow touchlinking again.
        On an end-device touchlinking should not be enabled
        again until a new explicit user action.
*/
void N_LinkTarget_SetTargetType(N_LinkTarget_TargetType_t targetType, N_LinkTarget_SetTargetTypeDone_t pfDoneCallback);

/** Enable/Disable Touchlink priority
    \param priorityEnabled Enable touchlink priority

    For touchlink targets the priority can be set to request priority during touchlink operation
    Zll profile specification 7.1.2.3.1.4 ZLL information field. touchlink priority request.
*/
void N_LinkTarget_SetTouchlinkPriority(bool priorityEnabled);

/** Allow or disallow stealing
    \param allowStealing Set to TRUE to allow stealing (default), or FALSE to disallow it.

    Stealing means: being pulled from an existing network into another network. When stealing is
    disallowed, the device can only be linked (i.e., reacts to a valid incoming Interpan Join/Start
    request) when the device is factory new. If not factory new, an error is returned to the
    initiator.
*/
void N_LinkTarget_AllowStealing(bool allowStealing);

/***************************************************************************************************
* END OF C++ DECLARATION WRAPPER
***************************************************************************************************/

#ifdef __cplusplus
}
#endif

#endif // N_LINKTARGET_H
