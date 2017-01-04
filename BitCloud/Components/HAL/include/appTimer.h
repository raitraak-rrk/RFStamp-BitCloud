/***************************************************************************//**
  \file  appTimer.h

  \brief The header file describes the appTimer interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    5/12/07 A. Khromykh - Created
 ******************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/

#ifndef _APPTIMER_H
#define _APPTIMER_H

// \cond
/******************************************************************************
                   Includes section
******************************************************************************/
#include <sysTypes.h>
#include <bcTimer.h>
#include <halAppClock.h>
// \endcond

/******************************************************************************
                   Types section
******************************************************************************/
/** \brief fields of structure: \n
    uint32_t interval - timer firing interval. Interval must be more than 10 ms (set by user) \n
    TimerMode_t mode - timer work mode (set by user). Must be chosen from: \n
           TIMER_REPEAT_MODE \n
           TIMER_ONE_SHOT_MODE \n
    void (*callback)(void) - pointer to timer callback function (set by user). 
    Must not be set to NULL.    
*/
typedef Timer_t HAL_AppTimer_t;

/******************************************************************************
                   Prototypes section
******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************//**
\brief Starts to count an interval (starts user timer).

\ingroup hal_misc

\param[in]
  appTimer - pointer to the timer structure (HAL_AppTimer_t is typedef Timer_t)

\return
 -1 - pointer is NULL
  0 - success
******************************************************************************/
int HAL_StartAppTimer(HAL_AppTimer_t *appTimer);

/**************************************************************************//**
\brief Stops the user timer.

\ingroup hal_misc

\param[in]
  appTimer - pointer to the timer structure.

\return
 -1 - there is no appTimer started or pointer is NULL
  0 - success
******************************************************************************/
int HAL_StopAppTimer(HAL_AppTimer_t *appTimer);

/**************************************************************************//**
\brief Gets system time.

\ingroup hal_misc

\return
  time since power up in milliseconds(8 bytes).
******************************************************************************/
BcTime_t HAL_GetSystemTime(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*_APPTIMER_H*/
//eof appTimer.h
