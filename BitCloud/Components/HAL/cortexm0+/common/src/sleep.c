/**************************************************************************//**
\file  sleep.c

\brief The module to make power off mode.

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal
  History:
    19/08/13 Agasthian.s - Created
    07/04/14 karthik.p_u - Modified
*******************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/

/******************************************************************************
                   Includes section
******************************************************************************/
#include <halSleep.h>
#include <appTimer.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
#define HAL_NULL_POINTER                      -1
#define HAL_SLEEP_TIMER_HAS_ALREADY_STARTED   -3
#define HAL_SLEEP_TIMER_IS_BUSY               -2
#define HAL_SLEEP_SYSTEM_HAS_ALREADY_STARTED  -3

/******************************************************************************
                   Global variables section
******************************************************************************/
HAL_SleepControl_t halSleepControl =
{
  .wakeupStation = HAL_ACTIVE_MODE,
  .sleepTimerState = HAL_SLEEP_TIMER_IS_STOPPED
};

/******************************************************************************
                   Implementations section
******************************************************************************/
/**************************************************************************//**
\brief Starts sleep timer and HAL sleep. When system is wake up send callback
\param[in]
    sleepParam - pointer to sleep structure.
\return
    -1 - bad pointer,   \n
    -2 - sleep timer is busy, \n
    -3 - sleep system has been started.
     0 - success.
******************************************************************************/
int HAL_StartSystemSleep(HAL_Sleep_t *sleepParam)
{
  HAL_SleepTimer_t sleepTimer;
  int sleepTimerStatus;

  if (!sleepParam)
    return HAL_NULL_POINTER;

  halSleepControl.callback = sleepParam->callback;
  halSleepControl.startPoll = sleepParam->startPoll;
  sleepTimer.interval = sleepParam->sleepTime;
  sleepTimer.mode = TIMER_ONE_SHOT_MODE;
  sleepTimer.callback = NULL;

  sleepTimerStatus = HAL_StartSleepTimer(&sleepTimer);
  if ((HAL_NULL_POINTER == sleepTimerStatus) || (HAL_SLEEP_TIMER_HAS_ALREADY_STARTED == sleepTimerStatus))
    return HAL_SLEEP_TIMER_IS_BUSY;

  if (-1 == HAL_Sleep())
    return HAL_SLEEP_SYSTEM_HAS_ALREADY_STARTED;

  return 0;
}

// eof sleep.c
