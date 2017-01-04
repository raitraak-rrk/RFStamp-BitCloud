/*********************************************************************************************//**
\file

\brief

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal

$Id: N_Task-Internal.h 24615 2013-02-27 05:14:24Z arazinkov $

***************************************************************************************************/

#ifndef N_TASK_INTERNAL_H
#define N_TASK_INTERNAL_H

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

/** Internal call-back propagated from the host OS's task handler. */
typedef void (*pfTaskHandler_t)(void);

/***************************************************************************************************
* EXPORTED MACROS AND CONSTANTS
***************************************************************************************************/


/***************************************************************************************************
* EXPORTED FUNCTIONS
***************************************************************************************************/

/** Enter a critical section.
    \returns A value to be passed to N_Task_Internal_ExitCriticalSection()
*/
uint8_t N_Task_Internal_EnterCriticalSection(void);

/** Exit a critical section.
    \param intState The value returned from the matching N_Task_Internal_EnterCriticalSection().
*/
void N_Task_Internal_ExitCriticalSection(uint8_t intState);

/** Set an event to the internal task. This will cause the registered task callback function to be called (decoupled).
*/
void N_Task_Internal_SetEvent(void);

/** Initialise the OS-specific part of N_Task and register the task callback function.
*/
void N_Task_Internal_Init(pfTaskHandler_t taskCallback);

/***************************************************************************************************
* END OF C++ DECLARATION WRAPPER
***************************************************************************************************/

#ifdef __cplusplus
}
#endif

#endif // N_TASK_INTERNAL_H
