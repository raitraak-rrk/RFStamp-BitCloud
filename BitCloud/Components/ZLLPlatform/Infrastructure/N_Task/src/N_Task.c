/*********************************************************************************************//**
\file  

\brief 

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal

$Id: N_Task.c 24615 2013-02-27 05:14:24Z arazinkov $

***************************************************************************************************/

/***************************************************************************************************
* LOCAL INCLUDE FILES
***************************************************************************************************/

#include "N_Task_Bindings.h"
#include "N_Task_Init.h"
#include "N_Task.h"
#include "N_Task-Internal.h"

/***************************************************************************************************
* EXTERNAL INCLUDE FILES
***************************************************************************************************/

#include "N_ErrH.h"
#include "N_Init.h"
#include "N_Log.h"
#include "N_Memory.h"
#include "N_Types.h"
#include "N_Util.h"

/***************************************************************************************************
* LOCAL MACROS AND CONSTANTS
***************************************************************************************************/

#define COMPID "N_Task"

/** The maximum event number. */
#define N_TASK_EVENT_MAX 32u

/***************************************************************************************************
* LOCAL TYPES
***************************************************************************************************/


/***************************************************************************************************
* LOCAL VARIABLES
***************************************************************************************************/

static const N_Task_HandleEvent_t* s_pTasks;
static uint8_t s_numTasks;
static uint32_t* s_pEvents = NULL;

/***************************************************************************************************
* LOCAL FUNCTION DECLARATIONS
***************************************************************************************************/


/***************************************************************************************************
* STATE MACHINE
***************************************************************************************************/


/***************************************************************************************************
* LOCAL FUNCTIONS
***************************************************************************************************/

/** Get the task index with an event set. Returns 0xFFu when none have an event set. */
static uint8_t GetFirstTaskWithEventSet(void)
{
    uint8_t taskIndex = 0u;
    while ( taskIndex != s_numTasks )
    {
        if (s_pEvents[taskIndex] != 0u)
        {
            return taskIndex;
        }
        taskIndex++;
    }
    return 0xFFu;
}

static void HandleOneTaskEvent(void)
{
    uint8_t taskIndex = GetFirstTaskWithEventSet();
    if ( taskIndex == 0xFFu )
    {
        return;
    }
    else
    {
        // Find the lowest set bit and handle it
        uint32_t mask = 1uL;
        uint8_t evt = 0u; // note that the Visual Studio debugger falsely identifs the name 'event' as a reserved word

        N_Util_CriticalSection_SaveState_t state = N_Util_CriticalSection_Enter();

        while ( (mask != 0uL) && ((s_pEvents[taskIndex] & mask) == 0uL) ) // mask becomes 0u after 32 shifts
        {
            mask <<= 1u;
            evt++;
        }
        if (evt == N_TASK_EVENT_MAX)   // The event may have been just cleared by an ISR (race condition)
        {
            N_Util_CriticalSection_Exit(state);
        }
        else
        {
            s_pEvents[taskIndex] &= ~mask;
            N_Util_CriticalSection_Exit(state);

            // Call the task's event handler
            uint8_t baseTaskIndex = N_Task_GetIdFromEventHandler(s_pTasks[taskIndex]) - 1u;
            uint8_t instance = baseTaskIndex - taskIndex;
            bool handled = s_pTasks[taskIndex]((instance << 5u) | evt);
            
            if (!handled)
            {
                N_LOG_ALWAYS(("Event %hu of task %hu not handled", evt, taskIndex+1u));
                N_ERRH_FATAL();
            }
        }
    }
}

static void TaskHandler(void)
{
    if (N_Init_IsInitDone())
    {
        HandleOneTaskEvent();
        if (GetFirstTaskWithEventSet() != 0xFFu)
        {
            // More events to handle
            N_Task_Internal_SetEvent();
        }
    }
}

/***************************************************************************************************
* EXPORTED FUNCTIONS
***************************************************************************************************/

void N_Task_SetEvent(N_Task_Id_t task, N_Task_Event_t evt)
{
    N_ERRH_ASSERT_FATAL(((uint8_t)task != 0u) // <= because task is always 1 higher than the index
        && ((uint8_t)task <= s_numTasks)
        && (evt <= N_TASK_EVENT_MAX) );
    uint32_t eventMask = (1uL << evt);

    N_Util_CriticalSection_SaveState_t state = N_Util_CriticalSection_Enter();
    s_pEvents[task-1u] |= eventMask;
    N_Util_CriticalSection_Exit(state);

    N_Task_Internal_SetEvent();
}

void N_Task_ClearEvent(N_Task_Id_t task, N_Task_Event_t evt)
{
    N_ERRH_ASSERT_FATAL(((uint8_t)task != 0u) // <= because task is always 1 higher than the index
        && ((uint8_t)task <= s_numTasks)
        && (evt <= N_TASK_EVENT_MAX) );
    uint32_t eventMask = (1uL << evt);

    N_Util_CriticalSection_SaveState_t state = N_Util_CriticalSection_Enter();
    s_pEvents[task-1u] &= ~eventMask;
    N_Util_CriticalSection_Exit(state);

    // Don't bother clearing the event - we'll just get one spurious event.
}

N_Task_Id_t N_Task_GetIdFromEventHandler(const N_Task_HandleEvent_t pfTaskEventHandler)
{
    uint8_t taskId = s_numTasks;
    while ( taskId != 0u )
    {
        taskId--;
        if (s_pTasks[taskId] == pfTaskEventHandler)
        {
            break;
        }
    }

    // Assert here means the task's event handler is not registered in N_Task_Init()
    N_ERRH_ASSERT_FATAL(s_pTasks[taskId] == pfTaskEventHandler);

    // ID 0 is reserved
    return taskId + 1u;
}

void N_Task_Init(uint8_t numTasks, const N_Task_HandleEvent_t* pTaskList)
{
    s_numTasks = numTasks;
    s_pTasks = pTaskList;

    // Allocate memory for the events of all tasks. This memory is never released.
    uint16_t size = numTasks * (uint16_t)sizeof(*s_pEvents);
    s_pEvents = (uint32_t*) N_Memory_AllocChecked((size_t)size);

    N_Task_Internal_Init(TaskHandler);
}

uint8_t N_Task_GetNumberOfEventHandlers(const N_Task_HandleEvent_t pfTaskEventHandler)
{
    uint8_t numHandlers = 0u;

    if (pfTaskEventHandler == NULL)
    {
        // Total number of task handlers
        numHandlers = s_numTasks;
    }
    else
    {
        for (uint8_t i = 0u; i < s_numTasks; i++)
        {
            if (s_pTasks[i] == pfTaskEventHandler)
            {
                numHandlers++;
            }
        }
    }
    return numHandlers;
}
