/*********************************************************************************************//**
\file  

\brief 

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal

$Id: N_Timer-Internal-Atmel.c 24615 2013-02-27 05:14:24Z arazinkov $

***************************************************************************************************/

/***************************************************************************************************
* LOCAL INCLUDE FILES
***************************************************************************************************/

#include "N_Timer_Bindings.h"
#include "N_Timer_Init.h"
#include "N_Timer.h"
#include "N_Timer-Internal.h"

/***************************************************************************************************
* EXTERNAL INCLUDE FILES
***************************************************************************************************/

#include "N_Types.h"

#include <appTimer.h>

/***************************************************************************************************
* LOCAL VARIABLES
***************************************************************************************************/

static HAL_AppTimer_t s_halTimer;

/***************************************************************************************************
* EXPORTED FUNCTIONS
***************************************************************************************************/

void N_Timer_Internal_SetTimer(int32_t timeoutMs)
{
    // NOTE: this code is untested because the ZigBee Platform does not yet have a build that integrates the BitCloud.

    // stopping the timer because it is unclear if it is possible
    // to restart an already running timer.
    (void) HAL_StopAppTimer(&s_halTimer);

    if ( timeoutMs <= 0 )
    {
        s_halTimer.interval = 0u;
    }
    else
    {
        s_halTimer.interval = (uint32_t) timeoutMs;
    }

    (void) HAL_StartAppTimer(&s_halTimer);
}

uint32_t N_Timer_GetSystemTime_Impl(void)
{
    return HAL_GetSystemTime();
}

void N_Timer_Init(void)
{
    // NOTE: this code is untested because the ZigBee Platform does not yet have a build that integrates the BitCloud.

    s_halTimer.mode = TIMER_ONE_SHOT_MODE;
    s_halTimer.callback = N_Timer_Internal_Update;
}

void N_Timer_Internal_Stop(void)
{
    HAL_StopAppTimer(&s_halTimer);
}