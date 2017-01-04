/*********************************************************************************************//**
\file  

\brief 

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal

$Id: N_Timer-Internal.h 24615 2013-02-27 05:14:24Z arazinkov $

***************************************************************************************************/

#ifndef N_TIMER_INTERNAL_H
#define N_TIMER_INTERNAL_H

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
* EXPORTED FUNCTIONS
***************************************************************************************************/

/** Update the running timers.

    Implemented by the common code, called by the platform specific code.
*/
void N_Timer_Internal_Update(void);

/** Start a timer on the underlying platform.
    \param timeoutMs The timer delay in milliseconds.

    Implemented by the platform specific code, called by the common code.
*/
void N_Timer_Internal_SetTimer(int32_t timeoutMs);

/** Stop the platform timer on the bitcloud stack.

    Implemented by the common code, called by the platform specific code.
*/
void N_Timer_Internal_Stop(void);

/***************************************************************************************************
* END OF C++ DECLARATION WRAPPER
***************************************************************************************************/

#ifdef __cplusplus
}
#endif

#endif // N_TIMER_INTERNAL_H
