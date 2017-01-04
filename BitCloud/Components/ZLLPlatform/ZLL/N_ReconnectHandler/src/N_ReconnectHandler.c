/*********************************************************************************************//**
\file

\brief

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal

$Id: N_ReconnectHandler.c 24615 2013-02-27 05:14:24Z arazinkov $

\section Description

\par
This implementation of a ReconnectHandler first does 4 NetworkDiscovery attempts
on the current network channel of 60ms each.
If they all fail, a NetworkDiscovery over all channels in the current channel mask
is done, of 240ms per channel.

\par
If one of the discoveries finds the network, a Rejoin is done of 440ms.

\par
With a channel mask of 4 channels, the longest possible time to find back a parent is:
4x60 + 4x240 + 440 = 1640ms, which is shorter than the 2000ms that
N_EndDeviceRobustness stores messages.

***************************************************************************************************/

/***************************************************************************************************
* LOCAL INCLUDE FILES
***************************************************************************************************/

#include "N_ReconnectHandler_Bindings.h"
#include "N_ReconnectHandler_Init.h"
#include "N_ReconnectHandler.h"

/***************************************************************************************************
* EXTERNAL INCLUDE FILES
***************************************************************************************************/

#include "N_Connection.h"
#include "N_Connection_ReconnectHandler.h"
#include "N_DeviceInfo.h"
#include "N_ErrH.h"
#include "N_Fsm.h"
#include "N_Log.h"
#include "N_Util.h"
#include "N_Task.h"
#include "N_Types.h"

/***************************************************************************************************
* LOCAL MACROS AND CONSTANTS
***************************************************************************************************/

#define COMPID "N_ReconnectHandler"

#define EVENT_NETWORK_DISCOVERY_SUCCEEDED   0u
#define EVENT_NETWORK_DISCOVERY_FAILED      1u
#define EVENT_JOIN_SUCCEEDED                2u
#define EVENT_JOIN_FAILED                   3u

#define NUM_DISCOVERY_ATTEMPTS_ON_NETWORK_CHANNEL   4u

/***************************************************************************************************
* LOCAL TYPES
***************************************************************************************************/


/***************************************************************************************************
* LOCAL VARIABLES
***************************************************************************************************/

static N_Task_Id_t s_taskId = N_TASK_ID_NONE;
static uint8_t s_retryCounter;

static N_Beacon_t s_selectedBeacon = { 0u };

static bool isBusy;

/***************************************************************************************************
* LOCAL FUNCTION DECLARATIONS
***************************************************************************************************/

static void ReconnectHandler_NetworkDiscoveryDone(void);
static void ReconnectHandler_JoinDone(N_Connection_Result_t result);
static void ReconnectHandler_BeaconReceivedCallback(N_Beacon_t* pBeacon);

/***************************************************************************************************
* STATE MACHINE
***************************************************************************************************/

static void PerformAction(N_FSM_Action_t action, int32_t arg1, int32_t arg2);
static bool CheckCondition(N_FSM_Condition_t condition, int32_t arg1, int32_t arg2);

enum N_ReconnectHandler_states // states
{
    sIdle,
    sDiscoveringOnNetworkChannel,
    sDiscoveringOnAllChannels,
    sRejoining,
    sUnsecureRejoining,
};

enum N_ReconnectHandler_events // events
{
    eReconnect,
    eNetworkNotFound,
    eNetworkFound,
    eRejoinSuccess,
    eRejoinFailure,
};

enum N_ReconnectHandler_actions // actions
{
    aDiscoveryOnAllChannels,
    aSetRetryCounter_DiscoveryOnNetworkChannel,
    aDecrementRetryCounter_DiscoveryOnNetworkChannel,
    aRejoin,
    aUnsecureRejoin,
    aReconnectionSucceeded_SetNotBusy_ProcessPotponedInterPanMode,
    aReconnectionFailed_SetNotBusy_ProcessPotponedInterPanMode,
};

enum N_ReconnectHandler_conditions // conditions
{
    cAnotherTryOnNetworkChannel,
    cHasTrustCenter,
};

/** \image html N_ReconnectHandler_Fsm.png */
/* @Fsm2PlantUml:begin file=N_ReconnectHandler_Fsm.png */

static const N_FSM_Transition_t s_transitionTable[] =
{
//     event                    condF                       actF                                                           newState
N_FSM_STATE( sIdle ),
N_FSM( eReconnect,              N_FSM_NONE,                 aSetRetryCounter_DiscoveryOnNetworkChannel,                    sDiscoveringOnNetworkChannel ),

N_FSM_STATE( sDiscoveringOnNetworkChannel ),
N_FSM( eNetworkFound,           N_FSM_NONE,                 aRejoin,                                                       sRejoining ),
N_FSM( eNetworkNotFound,        cAnotherTryOnNetworkChannel,aDecrementRetryCounter_DiscoveryOnNetworkChannel,              N_FSM_SAME_STATE ),
N_FSM( eNetworkNotFound,        N_FSM_ELSE,                 aDiscoveryOnAllChannels,                                       sDiscoveringOnAllChannels ),

N_FSM_STATE( sDiscoveringOnAllChannels ),
N_FSM( eNetworkFound,           N_FSM_NONE,                 aRejoin,                                                       sRejoining ),
N_FSM( eNetworkNotFound,        N_FSM_NONE,                 aReconnectionFailed_SetNotBusy_ProcessPotponedInterPanMode,    sIdle ),

N_FSM_STATE( sRejoining ),
N_FSM( eRejoinSuccess,          N_FSM_NONE,                 aReconnectionSucceeded_SetNotBusy_ProcessPotponedInterPanMode, sIdle ),
N_FSM( eRejoinFailure,          cHasTrustCenter,            aUnsecureRejoin,                                               sUnsecureRejoining ),
N_FSM( eRejoinFailure,          N_FSM_ELSE,                 aReconnectionFailed_SetNotBusy_ProcessPotponedInterPanMode,    sIdle ),

N_FSM_STATE( sUnsecureRejoining ),
N_FSM( eRejoinSuccess,          N_FSM_NONE,                 aReconnectionSucceeded_SetNotBusy_ProcessPotponedInterPanMode, sIdle ),
N_FSM( eRejoinFailure,          N_FSM_NONE,                 aReconnectionFailed_SetNotBusy_ProcessPotponedInterPanMode,    sIdle ),
};

static uint8_t s_currentState;

static const N_FSM_StateMachine_t s_fsm =
{
    s_transitionTable,
    N_FSM_TABLE_SIZE(s_transitionTable),
    NULL,
    0u,
    PerformAction,
    CheckCondition
};

/* @Fsm2PlantUml:end  */

static void FsmHandleEvent(enum N_ReconnectHandler_events e)
{
    (void) N_FSM_PROCESS_EVENT(&s_fsm, &s_currentState, e);
}

/***************************************************************************************************
* STATE MACHINE CONDITION FUNCTIONS
***************************************************************************************************/

static inline bool AnotherTryOnNetworkChannel(void)
{
    return N_UTIL_BOOL(s_retryCounter > 0u);
}

static inline bool HasTrustCenter(void)
{
    return N_UTIL_BOOL(N_DeviceInfo_GetTrustCenterMode() == N_DeviceInfo_TrustCenterMode_Central);
}

static bool CheckCondition(N_FSM_Condition_t condition, int32_t arg1, int32_t arg2)
{
    (void)arg1;
    (void)arg2;
    switch (condition)
    {
    case cAnotherTryOnNetworkChannel:
        return AnotherTryOnNetworkChannel();
    case cHasTrustCenter:
        return HasTrustCenter();
    default:
        return TRUE;
    }
}

/***************************************************************************************************
* STATE MACHINE ACTION FUNCTIONS
***************************************************************************************************/

static inline void SetRetryCounter(void)
{
    s_retryCounter = NUM_DISCOVERY_ATTEMPTS_ON_NETWORK_CHANNEL - 1u;
}

static inline void DecrementRetryCounter(void)
{
    s_retryCounter--;
}

static inline void SetNotBusy(void)
{
    isBusy = false;
}

static inline void DiscoveryOnNetworkChannel(void)
{
    uint32_t channelMask = 1uL << N_DeviceInfo_GetNetworkChannel();

    memset(&s_selectedBeacon, 0, sizeof(s_selectedBeacon));
    s_selectedBeacon.updateId = N_DeviceInfo_GetNetworkUpdateId();

    N_Connection_ReconnectHandler_NetworkDiscovery(channelMask, N_Beacon_Order_60ms,
        ReconnectHandler_BeaconReceivedCallback, ReconnectHandler_NetworkDiscoveryDone);
}

static inline void DiscoveryOnAllChannels(void)
{
    uint32_t channelMask;
    if ( N_DeviceInfo_GetTrustCenterMode() == N_DeviceInfo_TrustCenterMode_Distributed )
    {
        // scan primary channels
        channelMask = N_DeviceInfo_GetPrimaryChannelMask();
    }
    else
    {
        // scan primary and secondary channels
        channelMask = N_DeviceInfo_GetPrimaryChannelMask() | N_DeviceInfo_GetSecondaryChannelMask();
    }
    memset(&s_selectedBeacon, 0, sizeof(s_selectedBeacon));
    s_selectedBeacon.updateId = N_DeviceInfo_GetNetworkUpdateId();

    N_Connection_ReconnectHandler_NetworkDiscovery(channelMask, N_Beacon_Order_240ms,
        ReconnectHandler_BeaconReceivedCallback, ReconnectHandler_NetworkDiscoveryDone);
}

static inline void Rejoin(void)
{
    N_Connection_ReconnectHandler_Rejoin(&s_selectedBeacon, ReconnectHandler_JoinDone);
}

static inline void UnsecureRejoin(void)
{
    N_Connection_ReconnectHandler_UnsecureRejoin(&s_selectedBeacon, ReconnectHandler_JoinDone);
}

static inline void ReconnectionSucceeded(void)
{
    N_Connection_ReconnectHandler_ReconnectDone(N_Connection_Result_Success);
}

static inline void ReconnectionFailed(void)
{
    N_Connection_ReconnectHandler_ReconnectDone(N_Connection_Result_Failure);
}

static void PerformAction(N_FSM_Action_t action, int32_t arg1, int32_t arg2)
{
    switch(action)
    {
    case aSetRetryCounter_DiscoveryOnNetworkChannel:
        SetRetryCounter();
        DiscoveryOnNetworkChannel();
        break;
    case aDecrementRetryCounter_DiscoveryOnNetworkChannel:
        DecrementRetryCounter();
        DiscoveryOnNetworkChannel();
        break;
    case aDiscoveryOnAllChannels:
        DiscoveryOnAllChannels();
        break;
    case aRejoin:
        Rejoin();
        break;
    case aUnsecureRejoin:
        UnsecureRejoin();
        break;
    case aReconnectionSucceeded_SetNotBusy_ProcessPotponedInterPanMode:
        ReconnectionSucceeded();
        SetNotBusy();
        N_Connection_ProcessPostponedInterPanMode();
        break;
    case aReconnectionFailed_SetNotBusy_ProcessPotponedInterPanMode:
        ReconnectionFailed();
        SetNotBusy();
        N_Connection_ProcessPostponedInterPanMode();
        break;
    default:
        break;
    }
    (void)arg1;
    (void)arg2;
}

/***************************************************************************************************
* LOCAL FUNCTIONS
***************************************************************************************************/

static void ReconnectHandler_Reconnect(void)
{
    FsmHandleEvent(eReconnect);
    isBusy = true;
}

static void ReconnectHandler_BeaconReceivedCallback(N_Beacon_t* pBeacon)
{
    // check if the device can accomodate one more child
    if ( pBeacon->hasDeviceCapacity )
    {
        // check if the network update id is better
        s_selectedBeacon.updateId = N_Util_HighestNetworkUpdateId(s_selectedBeacon.updateId, pBeacon->updateId);
        if ( pBeacon->updateId == s_selectedBeacon.updateId )
        {
            memcpy(&s_selectedBeacon, pBeacon, sizeof(s_selectedBeacon));
        }
    }
}

static void ReconnectHandler_NetworkDiscoveryDone(void)
{
    if ( s_selectedBeacon.logicalChannel != 0u )    // was at least one valid beacon received?
    {
        N_Task_SetEvent(s_taskId, EVENT_NETWORK_DISCOVERY_SUCCEEDED);
    }
    else
    {
        N_Task_SetEvent(s_taskId, EVENT_NETWORK_DISCOVERY_FAILED);
    }
}

static void ReconnectHandler_JoinDone(N_Connection_Result_t result)
{
    if ( result == N_Connection_Result_Success )
    {
        N_Task_SetEvent(s_taskId, EVENT_JOIN_SUCCEEDED);
    }
    else
    {
        N_Task_SetEvent(s_taskId, EVENT_JOIN_FAILED);
    }
}

/***************************************************************************************************
* EXPORTED FUNCTIONS
***************************************************************************************************/

bool N_ReconnectHandler_EventHandler(N_Task_Event_t evt)
{
    switch (evt)
    {
    case EVENT_NETWORK_DISCOVERY_SUCCEEDED:
        FsmHandleEvent(eNetworkFound);
        break;
    case EVENT_NETWORK_DISCOVERY_FAILED:
        FsmHandleEvent(eNetworkNotFound);
        break;
    case EVENT_JOIN_SUCCEEDED:
        FsmHandleEvent(eRejoinSuccess);
        break;
    case EVENT_JOIN_FAILED:
        FsmHandleEvent(eRejoinFailure);
        break;
    default:
        return FALSE;
    }
    return TRUE;
}

void N_ReconnectHandler_Init(void)
{
    s_taskId = N_Task_GetIdFromEventHandler(N_ReconnectHandler_EventHandler);

    static const N_Connection_ReconnectHandler_Callback_t cb =
    {
        ReconnectHandler_Reconnect,
        -1
    };
    N_Connection_ReconnectHandler_Subscribe(&cb);

    N_FSM_INITIALIZE(&s_fsm, &s_currentState, sIdle);
}

bool N_ReconnectHandler_IsBusy(void)
{
    return isBusy;
}
