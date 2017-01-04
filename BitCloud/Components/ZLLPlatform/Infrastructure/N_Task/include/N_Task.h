/*********************************************************************************************//**
\file

\brief

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal

$Id: N_Task.h 24615 2013-02-27 05:14:24Z arazinkov $

***************************************************************************************************/

#ifndef N_TASK_H
#define N_TASK_H

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
* EXPORTED TYPES
***************************************************************************************************/

/** The task identifier.
    Components should call N_Task_GetTaskIdFromEventHandler() to get their task's N_Task_Id_t,
    which is typically done in the task's Init() function.
    0 is an invalid N_Task_Id_t, which is used to flag non-initialised task variables.
*/
typedef uint8_t N_Task_Id_t;

/** The event within the task (0..31).
*/
typedef uint8_t N_Task_Event_t;

/** Prototype of a task's event handler.
    \returns Whether the event was handled. N_Task will assert when TRUE is returned.
*/
typedef bool (*N_Task_HandleEvent_t)(N_Task_Event_t);

/***************************************************************************************************
* EXPORTED MACROS AND CONSTANTS
***************************************************************************************************/

/** This value of N_Task_Id_t is not used.
    The value zero is used so an unintentionally uninitialised static variable automatically uses it.
*/
#define N_TASK_ID_NONE 0x00u

#define INSTANCES_FUNC(num) INSTANCES_##num
#define N_TASK_INSTANCES(eventHandler, numOfInstances) INSTANCES_FUNC(numOfInstances) ( eventHandler )

#define INSTANCES_1u(eventHandler) eventHandler
#define INSTANCES_2u(eventHandler) INSTANCES_1u(eventHandler), eventHandler
#define INSTANCES_3u(eventHandler) INSTANCES_2u(eventHandler), eventHandler
#define INSTANCES_4u(eventHandler) INSTANCES_3u(eventHandler), eventHandler
#define INSTANCES_5u(eventHandler) INSTANCES_4u(eventHandler), eventHandler
#define INSTANCES_6u(eventHandler) INSTANCES_5u(eventHandler), eventHandler
#define INSTANCES_7u(eventHandler) INSTANCES_6u(eventHandler), eventHandler
#define INSTANCES_8u(eventHandler) INSTANCES_7u(eventHandler), eventHandler

#define N_TASK_INSTANCE_ID(event, maxInstances)         (((event >> 5u) > maxInstances) ? 0u : (event >> 5u))
#define N_TASK_EVENT_ID(event)                          (event & 0x1Fu)

/***************************************************************************************************
* EXPORTED FUNCTIONS
***************************************************************************************************/

/** Set an event.
    \param task The task ID (as returned by N_Task_GetTaskIdFromEventHandler()) of the task to set theevent to.
    \param evt The event to set.
*/
void N_Task_SetEvent(N_Task_Id_t task, N_Task_Event_t evt);

/** Clear an event.
    \param task The task ID (as returned by N_Task_GetTaskIdFromEventHandler()) of the task to clear the event from.
    \param evt The event to clear.
*/
void N_Task_ClearEvent(N_Task_Id_t task, N_Task_Event_t evt);

/** Get the task ID for a task, given its event handler.
    \param pfTaskEventHandler The event handler function of the task
    \returns The N_Task_Id_t of the task.
*/
N_Task_Id_t N_Task_GetIdFromEventHandler(const N_Task_HandleEvent_t pfTaskEventHandler);

/** Gets the total number of registered event handlers for a specific handler or for all task handlers.
    \param pfTaskEventHandler The event handler function to count. If set to NULL all event handlers will be counted.
    \returns The number of task handlers
*/
uint8_t N_Task_GetNumberOfEventHandlers(const N_Task_HandleEvent_t pfTaskEventHandler);

/***************************************************************************************************
* END OF C++ DECLARATION WRAPPER
***************************************************************************************************/

#ifdef __cplusplus
}
#endif

#endif // N_TASK_H
