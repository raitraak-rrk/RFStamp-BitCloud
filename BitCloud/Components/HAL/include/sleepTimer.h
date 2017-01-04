/**************************************************************************//**
  \file  sleepTimer.h

  \brief The header file describes the sleepTimer interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    29/05/07 E. Ivanov - Created
 ******************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/

#ifndef _SLEEPTIMER_H
#define _SLEEPTIMER_H

// \cond
/******************************************************************************
                   Includes section
******************************************************************************/
#include <bcTimer.h>
// \endcond

/******************************************************************************
                   Types section
******************************************************************************/
/** \brief fields of structure \n
    \brief uint32_t interval - timer firing interval (set by user) \n
    \brief TimerMode_t mode - timer work mode (set by user). Must be chosen from: \n
           TIMER_REPEAT_MODE \n
           TIMER_ONE_SHOT_MODE \n
    \brief void (*callback)() - pointer to the timer callback function (set by user);
                                can be set to NULL \n */
typedef Timer_t HAL_SleepTimer_t;

/******************************************************************************
                   Prototypes section
******************************************************************************/
/**************************************************************************//**
\brief Starts sleep timer. Interval must be greater one time of sleep timer tick.
\param[in]
    sleepTimer - pointer to sleep timer structure.
\return
  -1 - NULL pointer, \n
  -2 - interval can not be counted out, \n
  -3 - sleep timer has already started, \n
   0 - otherwise.
******************************************************************************/
int HAL_StartSleepTimer(HAL_SleepTimer_t *sleepTimer);

/**************************************************************************//**
\brief Removes timer.
\param[in]
  sleepTimer - address of the timer to be removed from the list
\return
  -1 - there is no active sleep timer, \n
   0 - otherwise.
******************************************************************************/
int HAL_StopSleepTimer(HAL_SleepTimer_t *sleepTimer);

#endif /* _SLEEPTIMER_H */
// eof sleepTimer.h
