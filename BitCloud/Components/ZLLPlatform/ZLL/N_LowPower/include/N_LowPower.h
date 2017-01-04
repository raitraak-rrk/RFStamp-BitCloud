/*********************************************************************************************//**
\file  

\brief 

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal

$Id: N_LowPower.h 24615 2013-02-27 05:14:24Z arazinkov $

***************************************************************************************************/

#ifndef N_LOWPOWER_H
#define N_LOWPOWER_H

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

typedef enum N_LowPower_SleepMode_t
{
    N_LowPower_SleepMode_AllowSleep,
    N_LowPower_SleepMode_KeepAwake,
} N_LowPower_SleepMode_t;

typedef enum N_LowPower_SleepStatus_t
{
    N_LowPower_SleepStatus_EnterSleep,
    N_LowPower_SleepStatus_LeaveSleep,
} N_LowPower_SleepStatus_t;

typedef enum N_LowPower_SleepLevel_t
{
    N_LowPower_SleepLevel_Timed,    //< sleep with timer running to wake up
    N_LowPower_SleepLevel_Deep,     //< sleep until interrupted, no timer running
} N_LowPower_SleepLevel_t;

/** Type of the callback function used in subscriptions to this component.
    \param status Enum indicating whether the device is going to sleep or waking up

    Called just before the device goes to sleep or just after it wakes up.
    It is not possible to influence the decision to go to sleep from the
    callback.
*/
typedef void (*N_LowPower_Callback_t)(N_LowPower_SleepStatus_t status, N_LowPower_SleepLevel_t level);

/***************************************************************************************************
* EXPORTED MACROS AND CONSTANTS
***************************************************************************************************/


/***************************************************************************************************
* EXPORTED FUNCTIONS
***************************************************************************************************/

/** Subscribe for notifications from this component.
    \param pCallback The callback to subscribe
*/
void N_LowPower_Subscribe(N_LowPower_Callback_t pCallback);

/** Set the sleep mode.
    \param mode The mode (allow sleep, keep awake)

    The mode after startup is "allow sleep".
*/
void N_LowPower_SetSleepMode(N_LowPower_SleepMode_t mode);

/***************************************************************************************************
* END OF C++ DECLARATION WRAPPER
***************************************************************************************************/

#ifdef __cplusplus
}
#endif

#endif // N_LOWPOWER_H
