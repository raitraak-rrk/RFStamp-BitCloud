/**************************************************************************//**
  \file  appTimer.c

  \brief Implementation of appTimer.

  \author
      Atmel Corporation: http://www.atmel.com \n
      Support email: avr@atmel.com

    Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
    Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    19/08/13 Agasthian.s - Created
 ******************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/
/******************************************************************************
                   Includes section
******************************************************************************/
#include <appTimer.h>
#include <halAppClock.h>
#include <halSleepTimerClock.h>
#include <halTaskManager.h>
#include <atomic.h>
#include <halDbg.h>
#include <sysAssert.h>

/******************************************************************************
                   External global variables section
******************************************************************************/
extern uint8_t halAppTimeOvfw;

/******************************************************************************
                   Global variables section
******************************************************************************/
static HAL_AppTimer_t *halAppTimerHead = NULL; // head of appTimer list

/******************************************************************************
                   Implementations section
******************************************************************************/
/******************************************************************************
  \brief Returns the time left value for the smallest app timer.
  \return time left for the smallest application timer.
******************************************************************************/
uint32_t halGetTimeToNextAppTimer(void)
{
  uint32_t timeToFire = ~0UL;
  uint32_t currentTime = halGetTimeOfAppTimer();

  if (halAppTimerHead)
  {
    if ((uint64_t)(halAppTimerHead->service.sysTimeLabel) + halAppTimerHead->interval > currentTime)
      timeToFire = (uint64_t)(halAppTimerHead->service.sysTimeLabel) + halAppTimerHead->interval - currentTime;
    else
      timeToFire = 0;
  }

  return timeToFire;
}

/******************************************************************************
Interrupt handler of appTimer clock.
******************************************************************************/
void halAppTimerHandler(void)
{
  uint32_t sysTime;

  // search for expired timers and call their callbacks
  while (halAppTimerHead
      && ((sysTime = halGetTimeOfAppTimer()) - halAppTimerHead->service.sysTimeLabel) >= halAppTimerHead->interval)
  {
    HAL_AppTimer_t *p = halAppTimerHead;
    halRemoveTimer(&halAppTimerHead, NULL, p);
    if (TIMER_REPEAT_MODE == p->mode)
    {
      p->service.sysTimeLabel = sysTime;
      halAddTimer(&halAppTimerHead, p, sysTime);
    }

    SYS_E_ASSERT_FATAL(p->callback, APPTIMER_HANDLER_0);
    p->callback();
  }
}

/******************************************************************************
Check if timer is already started.
Parameters:
  appTimer - pointer to HAL_AppTimer_t.
Returns:
  true - timer specified already started and presents in the system timers queue
  false - timer is't started yet
******************************************************************************/
static bool isTimerAlreadyStarted(HAL_AppTimer_t *appTimer)
{
  bool result = false;
  Timer_t *p; // p is bottom of list
  p = halAppTimerHead;

  while (NULL != p)
  {
    if (p == appTimer)
    {
      SYS_E_ASSERT_ERROR(false, APPTIMER_MISTAKE);
      return true;
    }
    p = (Timer_t *)p->service.next;
  }
  return result;
}

/******************************************************************************
Starts to count an interval.
Parameters:
  appTimer - pointer to HAL_AppTimer_t.
Returns:
  -1 - pointer is NULL.
  0 - success
******************************************************************************/
int HAL_StartAppTimer(HAL_AppTimer_t *appTimer)
{
  uint32_t sysTime;

  if (!appTimer)
    return -1;

  if (true == isTimerAlreadyStarted(appTimer))
    return 0;

  sysTime = halGetTimeOfAppTimer();
  appTimer->service.next = NULL;
  appTimer->service.sysTimeLabel = sysTime;
  halAddTimer((Timer_t**)(&halAppTimerHead), (Timer_t*)appTimer, sysTime);
  return 0;
}

/******************************************************************************
Stops the timer.
Parameters:
  appTimer - pointer to HAL_AppTimer_t.
Returns:
 -1 there is not the appTimer.
 0 - success
******************************************************************************/
int HAL_StopAppTimer(HAL_AppTimer_t *appTimer)
{
  Timer_t *prev = 0;
  Timer_t **t = &appTimer;

  if (!appTimer)
    return -1;
  if (halAppTimerHead != *t)
  {
    if (!(prev = halFindPrevTimer((Timer_t**)(&halAppTimerHead), appTimer)))
      return -1;  // This timer is not in the list
  }
  halRemoveTimer((Timer_t**)(&halAppTimerHead), prev, appTimer);
  return 0;
}

/**************************************************************************//**
\brief Gets system time.

\return
  time since power up in milliseconds(8 bytes).
******************************************************************************/
BcTime_t HAL_GetSystemTime(void)
{
  BcTime_t sysTime = 0ull;

  sysTime = halGetTimeOfAppTimer();
  sysTime |= ((BcTime_t)halAppTimeOvfw << 32);
  return sysTime;
}

// eof appTimer.c
