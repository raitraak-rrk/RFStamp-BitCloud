/*********************************************************************************************//**
\file  

\brief 

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal

$Id: N_Fsm.c 28213 2015-07-22 09:43:04Z agasthian.s $

***************************************************************************************************/

/***************************************************************************************************
* LOCAL INCLUDE FILES
***************************************************************************************************/

#include "N_Fsm.h"

/***************************************************************************************************
* EXTERNAL INCLUDE FILES
***************************************************************************************************/

#include "N_ErrH.h"
#include "N_Log.h"
#include "N_Types.h"

/***************************************************************************************************
* LOCAL MACROS AND CONSTANTS
***************************************************************************************************/

#define COMPID "N_Fsm"

// This bit is used to check whether the Fsm is being illegally used recursively.
#define RECURSIVE_STATE_CHECK   N_FSM_STATE_BIT

/***************************************************************************************************
* LOCAL FUNCTIONS
***************************************************************************************************/


/***************************************************************************************************
* EXPORTED FUNCTIONS
***************************************************************************************************/

#ifdef N_FSM_ENABLE_LOGGING
void N_FSM_Initialize_Log(N_FSM_StateMachine_Log_t const* pFsm, N_FSM_State_t* pActualState, N_FSM_State_t initialState)
#else
void N_FSM_Initialize(N_FSM_StateMachine_t const* pFsm, N_FSM_State_t* pActualState, N_FSM_State_t initialState)
#endif
{
    *pActualState = initialState;
    for (uint8_t i = 0u; i < pFsm->entryExitTableSize; i++)
    {
        if ((initialState == pFsm->pEntryExitTable[i].state) && (pFsm->pEntryExitTable[i].OnEntry != NULL))
        {
            pFsm->pEntryExitTable[i].OnEntry();
        }
    }
}

#ifdef N_FSM_ENABLE_LOGGING
bool N_FSM_ProcessEvent2args_Log(N_FSM_StateMachine_Log_t const* pFsm, N_FSM_State_t* pActualState, N_FSM_Event_t event, int32_t arg1, int32_t arg2,
                             const char* compId, const char* description)
#else
bool N_FSM_ProcessEvent2args(N_FSM_StateMachine_t const* pFsm, N_FSM_State_t* pActualState, N_FSM_Event_t event, int32_t arg1, int32_t arg2)
#endif
{
    bool found = FALSE;
    uint8_t tableIndex = 0u;
    uint8_t state = 0u;
#ifdef N_FSM_ENABLE_LOGGING
    const char* fromStateName = NULL;
    const char* eventName = NULL;
    const N_FSM_Transition_Log_t N_UTIL_ROM* pTx = NULL;
#else
    const N_FSM_Transition_t N_UTIL_ROM* pTx = NULL;
#endif

    if (((*pActualState) & RECURSIVE_STATE_CHECK) != 0u)
    {
        N_LOG_ALWAYS(("Recursive FSM failure. tableSize=%hu, fromState=%hu, event=%hu", pFsm->tableSize, *pActualState, (uint8_t)event));
        N_ERRH_FATAL();
    }

    // find matching transition
    while ((!found) && (tableIndex < pFsm->tableSize))
    {
        pTx = &(pFsm->pTable[tableIndex]);
        tableIndex++;

        if (pTx->event & N_FSM_STATE_BIT)
        {
            state = pTx->event ^ N_FSM_STATE_BIT;
#ifdef N_FSM_ENABLE_LOGGING
            if (state == *pActualState)
            {
                fromStateName = pTx->description;
            }
#endif
        }
        else
        {
            //lint -esym(960,10.1)
            found = ((state == N_FSM_ANY_STATE) || (state == *pActualState));
            found = found && ((pTx->event == N_FSM_OTHER_EVENT) || (pTx->event == event));
            if (found && (pTx->ConditionalFunction != N_FSM_NONE))
            {
                *pActualState |= RECURSIVE_STATE_CHECK;
                found = pFsm->pCondition(pTx->ConditionalFunction, arg1, arg2);
                *pActualState &= ~RECURSIVE_STATE_CHECK;
            }
#ifdef N_FSM_ENABLE_LOGGING
            if ( pTx->event == event )
            {
                eventName = pTx->eventName;
            }
#endif
        }
        //lint +esym(960,10.1)
    }

#ifdef N_FSM_ENABLE_LOGGING
    {
        if ( eventName == NULL )
        {
            // eventName encountered yet (propably due to N_FSM_OTHER_EVENT)
            // let's search the complete transition table.
            for (uint8_t i = 0u; (i < pFsm->tableSize) && (eventName == NULL); i++)
            {
                if ( pFsm->pTable[i].event == event )
                {
                    eventName = pFsm->pTable[i].eventName;
                }
            }
        }

        if ( eventName == NULL )
        {
            // The event is not used in the transition table so we use the event number instead.
            eventName = description;
        }

        N_LOG_COMPID(compId, N_Log_Level_State, pFsm->fsmName, ("%s: event=%u (^%s)", fromStateName, (int16_t)event, eventName));
    }
#endif

    if (found)
    {
#ifdef N_FSM_ENABLE_LOGGING
        N_LOG_COMPID(compId, N_Log_Level_State, pFsm->fsmName, ("  %s", pTx->description));
#endif
        if (pTx->nextState == N_FSM_SAME_STATE)
        {
            // do not call onEntry/onExit; state is not changed
            if (pTx->ActionFunction != N_FSM_NONE)
            {
                *pActualState |= RECURSIVE_STATE_CHECK;
                pFsm->pAction(pTx->ActionFunction, arg1, arg2);
                *pActualState &= ~RECURSIVE_STATE_CHECK;
            }
        }
        else
        {
            // call onExit, change state, call ActionFunction, call onEntry
            for (uint8_t i = 0u; i < pFsm->entryExitTableSize; i++)
            {
                if ((*pActualState == pFsm->pEntryExitTable[i].state) && (pFsm->pEntryExitTable[i].OnExit != NULL))
                {
#ifdef N_FSM_ENABLE_LOGGING
                    N_LOG_COMPID(compId, N_Log_Level_State, pFsm->fsmName, ("  @exit()"));
#endif
                    *pActualState |= RECURSIVE_STATE_CHECK;
                    pFsm->pEntryExitTable[i].OnExit();
                }
            }
            *pActualState = pTx->nextState | RECURSIVE_STATE_CHECK;
            if (pTx->ActionFunction != N_FSM_NONE)
            {
                pFsm->pAction(pTx->ActionFunction, arg1, arg2);
            }
            // do state entry
            *pActualState &= ~RECURSIVE_STATE_CHECK;
            for (uint8_t i = 0u; i < pFsm->entryExitTableSize; i++)
            {
                if ((*pActualState == pFsm->pEntryExitTable[i].state) && (pFsm->pEntryExitTable[i].OnEntry != NULL))
                {
#ifdef N_FSM_ENABLE_LOGGING
                    N_LOG_COMPID(compId, N_Log_Level_State, pFsm->fsmName, ("  @entry()"));
#endif
                    *pActualState |= RECURSIVE_STATE_CHECK;
                    pFsm->pEntryExitTable[i].OnEntry();
                }
            }
            *pActualState &= ~RECURSIVE_STATE_CHECK;
        }
    }
    else
    {
#ifdef N_FSM_ENABLE_LOGGING
        N_LOG_COMPID(compId, N_Log_Level_State, pFsm->fsmName, ("  (no transition)"));
#endif
    }

    return found;
}

#ifdef N_FSM_ENABLE_LOGGING
bool N_FSM_ProcessEvent_Log(N_FSM_StateMachine_Log_t const* pFsm, N_FSM_State_t* pActualState, N_FSM_Event_t event,
                        const char* compId, const char* description)
{
    return N_FSM_ProcessEvent2args_Log(pFsm, pActualState, event, 0, 0, compId, description);
}
#else
bool N_FSM_ProcessEvent(N_FSM_StateMachine_t const* pFsm, N_FSM_State_t* pActualState, N_FSM_Event_t event)
{
    return N_FSM_ProcessEvent2args(pFsm, pActualState, event, 0, 0);
}
#endif


#ifdef N_FSM_ENABLE_LOGGING
bool N_FSM_ProcessEvent1arg_Log(N_FSM_StateMachine_Log_t const* pFsm, N_FSM_State_t* pActualState, N_FSM_Event_t event, int32_t arg1,
                            const char* compId, const char* description)
{
    return N_FSM_ProcessEvent2args_Log(pFsm, pActualState, event, arg1, 0, compId, description);
}
#else
bool N_FSM_ProcessEvent1arg(N_FSM_StateMachine_t const* pFsm, N_FSM_State_t* pActualState, N_FSM_Event_t event, int32_t arg1)
{
    return N_FSM_ProcessEvent2args(pFsm, pActualState, event, arg1, 0);
}
#endif