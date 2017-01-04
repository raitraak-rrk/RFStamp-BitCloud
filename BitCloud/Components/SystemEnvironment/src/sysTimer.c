/***************************************************************************//**
  \file sysTimer.c

  \brief Implementation of the system timer,

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2010-10-18 Max Gekk - Created.
   Last change:
    $Id: sysTimer.c 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
/******************************************************************************
                              Includes section
 ******************************************************************************/
#include <sysTimer.h>

/******************************************************************************
                           Implementations section
 ******************************************************************************/
/******************************************************************************
  \brief Initialize the System Timer.

  \param[in] sysTimer - timer pointer will be initialized.
  \param[in] mode - timer mode:  TIMER_ONE_SHOT_MODE or TIMER_REPEAT_MODE.
  \param[in] interval - timer interval (milliseconds).
  \param[in] handler - timer handler function pointer.

  \return None.
 *****************************************************************************/
void SYS_InitTimer(SYS_Timer_t *const sysTimer, const TimerMode_t mode,
  const uint32_t interval, void (*handler)(void))
{
  SYS_StopTimer(sysTimer);

  sysTimer->state = SYS_TIMER_STOPPED;
  sysTimer->timer.mode = mode;
  sysTimer->timer.interval = interval;
  sysTimer->timer.callback = handler;
}

/******************************************************************************
  \brief Start the System Timer.

  \param[in] sysTimer - timer pointer will be started.

  \return None.
 ******************************************************************************/
void SYS_StartTimer(SYS_Timer_t *const sysTimer)
{
  if (SYS_TIMER_STOPPED == sysTimer->state)
  {
    sysTimer->state = SYS_TIMER_STARTED;
    HAL_StartAppTimer(&sysTimer->timer);
  }
}

/******************************************************************************
  \brief Stop the System Timer.

  \param[in] sysTimer - timer pointer will be stopped.

  \return None.
 *****************************************************************************/
void SYS_StopTimer(SYS_Timer_t *const sysTimer)
{
  if (SYS_TIMER_STARTED == sysTimer->state)
  {
    sysTimer->state = SYS_TIMER_STOPPED;
    HAL_StopAppTimer(&sysTimer->timer);
  }
}

/* eof sysTimer.c */

