/**************************************************************************//**
  \file  halSleepTimerClock.h

  \brief Definition for count out requested sleep interval.

  \author
      Atmel Corporation: http://www.atmel.com \n
      Support email: avr@atmel.com

    Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
    Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      09/05/14 Viswanadham Kotla - Created
 ******************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/

#ifndef _HALSLEEPTIMERCLOCK_H
#define _HALSLEEPTIMERCLOCK_H

/******************************************************************************
                   Includes section
******************************************************************************/
#include <sysTypes.h>
#include <halTaskManager.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
#define SLEEPTIMER_CLOCK                    32768ul
#define SLEEPTIMER_PRESCALER                (5)
#define SLEEPTIMER_DIVIDER                  (1 << SLEEPTIMER_PRESCALER)
#define HAL_MIN_SLEEP_TIME_ALLOWED          (1)

#define HAL_WakeUpCPU                        halWakeupFromIrq
/******************************************************************************
                   Prototypes section
******************************************************************************/
/******************************************************************************
Starts the sleep timer clock.
******************************************************************************/
void halStartSleepTimerClock(void);

/******************************************************************************
Stops the sleep timer clock.
******************************************************************************/
void halStopSleepTimerClock(void);

/******************************************************************************
Sets interval.
Parameters:
  value - contains number of ticks which the timer must count out.
Returns:
  none.
******************************************************************************/
void halSetSleepTimerInterval(uint32_t value);

/******************************************************************************
Returns the sleep timer frequency in Hz.
Parameters:
  none.
Returns:
  the sleep timer frequency in Hz.
******************************************************************************/
uint32_t halSleepTimerFrequency(void);

/**************************************************************************//**
\brief Clear timer control structure
******************************************************************************/
void halClearTimeControl(void);

/**************************************************************************//**
\brief Wake up procedure for all external interrupts
******************************************************************************/
void halWakeupFromIrq(void);

/**************************************************************************//**
\brief Get time of sleep timer.

\return
  time in ms.
******************************************************************************/
uint64_t halGetTimeOfSleepTimer(void);

/******************************************************************************
                   Inline static functions section
******************************************************************************/

/******************************************************************************
  Interrupt handler signal implementation
******************************************************************************/
INLINE void halInterruptSleepClock(void)
{
  halPostTask(HAL_ASYNC_TIMER);
}

/******************************************************************************
  Interrupt handler signal implementation
******************************************************************************/
INLINE void halSynchronizeSleepTime(void)
{
  halPostTask(HAL_SYNC_SLEEP_TIME);
}


#endif /* _HALSLEEPTIMERCLOCK_H */

// eof halSleepTimerClock.h
