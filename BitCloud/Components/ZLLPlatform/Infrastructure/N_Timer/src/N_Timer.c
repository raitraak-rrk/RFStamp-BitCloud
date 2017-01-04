/*********************************************************************************************//**
\file  

\brief 

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal

$Id: N_Timer.c 24615 2013-02-27 05:14:24Z arazinkov $

***************************************************************************************************/

/***************************************************************************************************
* LOCAL INCLUDE FILES
***************************************************************************************************/

#include "N_Timer_Bindings.h"
#include "N_Timer.h"
#include "N_Timer-Internal.h"

/***************************************************************************************************
* EXTERNAL INCLUDE FILES
***************************************************************************************************/

#include "N_ErrH.h"
#include "N_Log.h"
#include "N_Task.h"
#include "N_Util.h"

#if defined(N_TIMER_ENABLE_LOGGING)
#include "S_SerialComm.h"
#endif

/***************************************************************************************************
* LOCAL MACROS AND CONSTANTS
***************************************************************************************************/

#define COMPID "N_Timer"

#define EVENT_TIMER 0x0001u

/***************************************************************************************************
* LOCAL VARIABLES
***************************************************************************************************/

static N_Timer_t* s_pTimerList;

/***************************************************************************************************
* LOCAL FUNCTIONS
***************************************************************************************************/

static inline int32_t GetRemaining(uint32_t timestamp, uint32_t now)
{
    return (int32_t) timestamp - (int32_t) now;
}

static void StopTimer(N_Timer_t* pTimer)
{
    if ( s_pTimerList != NULL )
    {
        if ( s_pTimerList == pTimer )
        {
            s_pTimerList = pTimer->pNext;
        }
        else
        {
            N_Timer_t* pPrevious = s_pTimerList;
            for ( N_Timer_t* pIterator = s_pTimerList->pNext; pIterator != NULL; pIterator = pIterator->pNext )
            {
                if ( pTimer == pIterator )
                {
                    pPrevious->pNext = pIterator->pNext;
                    break;
                }
                pPrevious = pIterator;
            }
        }
    }

    N_Task_ClearEvent(pTimer->task, pTimer->evt);

    if ( s_pTimerList == NULL )
    {
         N_Timer_Internal_Stop();
    }
}

/***************************************************************************************************
* EXPORTED FUNCTIONS
***************************************************************************************************/

void N_Timer_Internal_Update(void)
{
    uint32_t now = N_Timer_GetSystemTime();

    while ( (s_pTimerList != NULL) && (GetRemaining(s_pTimerList->timestamp, now) <= 0) )
    {
        N_Task_SetEvent(s_pTimerList->task, s_pTimerList->evt);
        s_pTimerList = s_pTimerList->pNext;
    }

    if (s_pTimerList != NULL)
    {
        N_Timer_Internal_SetTimer(GetRemaining(s_pTimerList->timestamp, now));
    }
}

/** Interface function, see \ref N_Timer_Start32. */
void N_Timer_Start32_Impl(int32_t timeoutMs, N_Timer_t* pTimer)
{
#if defined(N_TIMER_ENABLE_LOGGING)
    S_SerialComm_TxMessage(COMPID, "Start", "%hu,%hu,%ld", pTimer->task, pTimer->evt, timeoutMs);
#endif

    StopTimer(pTimer);

    uint32_t now = N_Timer_GetSystemTime();
    pTimer->timestamp = now + (uint32_t) timeoutMs;

    N_Timer_t* pPrevious = NULL;
    N_Timer_t* pIterator;
    for ( pIterator = s_pTimerList; pIterator != NULL; pIterator = pIterator->pNext )
    {
        if ( GetRemaining(pIterator->timestamp, now) > timeoutMs )
        {
            break;
        }
        pPrevious = pIterator;
    }

    pTimer->pNext = pIterator;
    if ( pPrevious == NULL )
    {
        s_pTimerList = pTimer;
        N_Timer_Internal_SetTimer(timeoutMs);
    }
    else
    {
        pPrevious->pNext = pTimer;
    }
}

/** Interface function, see \ref N_Timer_Start16. */
void N_Timer_Start16_Impl(uint16_t timeoutMs, N_Timer_t* pTimer)
{
    N_Timer_Start32((int32_t) timeoutMs, pTimer);
}

/** Interface function, see \ref N_Timer_Stop. */
void N_Timer_Stop_Impl(N_Timer_t* pTimer)
{
#if defined(N_TIMER_ENABLE_LOGGING)
    S_SerialComm_TxMessage(COMPID, "Stop", "%hu,%hu", pTimer->task, pTimer->evt);
#endif
    StopTimer(pTimer);
}

/** Interface function, see \ref N_Timer_IsRunning. */
bool N_Timer_IsRunning_Impl(N_Timer_t* pTimer)
{
#if defined(N_TIMER_ENABLE_LOGGING)
    S_SerialComm_TxMessage(COMPID, "IsRunning", "%hu,%hu", pTimer->task, pTimer->evt);
#endif
    for ( N_Timer_t* pIterator = s_pTimerList; pIterator != NULL; pIterator = pIterator->pNext )
    {
        if ( pTimer == pIterator )
        {
            return TRUE;
        }
    }
    return FALSE;
}

/** Interface function, see \ref N_Timer_GetRemaining. */
int32_t N_Timer_GetRemaining_Impl(N_Timer_t* pTimer)
{
    return GetRemaining(pTimer->timestamp, N_Timer_GetSystemTime());
}

bool N_Timer_Expire(N_Task_Id_t task, N_Task_Event_t evt)
{
#if defined(N_TIMER_ENABLE_EXPIRE)
    N_Timer_t* pTimer = NULL;

    if ( s_pTimerList != NULL )
    {
        if ( (s_pTimerList->task == task) && (s_pTimerList->evt == evt) )
        {
            pTimer = s_pTimerList;
            s_pTimerList = pTimer->pNext;
        }
        else
        {
            N_Timer_t* pPrevious = s_pTimerList;
            for ( pTimer = s_pTimerList->pNext; pTimer != NULL; pTimer = pTimer->pNext )
            {
                if ( (pTimer->task == task) && (pTimer->evt == evt) )
                {
                    pPrevious->pNext = pTimer->pNext;
                    break;
                }
                pPrevious = pTimer;
            }
        }
    }

    if ( pTimer != NULL )
    {
    #if defined(N_TIMER_ENABLE_LOGGING)
        S_SerialComm_TxMessage(COMPID, "Expire", "%hu,%hu", pTimer->task, pTimer->evt);
    #endif

        N_Task_SetEvent(pTimer->task, pTimer->evt);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
#else
    N_ERRH_FATAL();
    (void)task;
    (void)evt;
    return FALSE;
#endif
}
