/**************************************************************************//**
  \file  sysIdleHandler.h

  \brief Definitions and function prototypes for system idle handler.

  \author
      Atmel Corporation: http://www.atmel.com \n
      Support email: avr@atmel.com

    Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
    Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
     15/01/13 D. Kolmakov - Created
 ******************************************************************************/

#ifndef _SYS_IDLE_HANDLER_H_
#define _SYS_IDLE_HANDLER_H_
/******************************************************************************
                   Prototypes section
******************************************************************************/
void SYS_IdleHandler(void);

#if defined(_SLEEP_WHEN_IDLE_)
/**************************************************************************//**
  \brief Enables sleep when a device is idle.
  It is disabled by default. If this is enabled, an end device automatically
  tries to go to sleep mode when there are no pending tasks and wakes up on
  expiration of CS_SLEEP_PERIOD or any application timer.
  The sleep period is equal to the minimum value between the rest of
  CS_SLEEP_PERIOD (CS_SLEEP_PERIOD minus time from the last poll)
  and the shortest time to fire of all application timers.
  If an end device wakes up on expiration of CS_SLEEP_PERIOD it polls
  its parent, otherwise it doesn't.
  Also, an end device can be wakep up by external interrupts. Application
  must be subscribed to event HAL_EVENT_WAKING_UP and call ZDO_WakeUpReq()
  in callback function.
  When the stack tries to go to sleep mode, it raises two events -
  BC_EVENT_BUSY_REQUEST and BC_EVENT_POLL_REQUEST allowing application to
  control sleep.
******************************************************************************/
void SYS_EnableSleepWhenIdle(void);

/**************************************************************************//**
  \brief Disables sleep when a device is idle.
******************************************************************************/
void SYS_DisableSleepWhenIdle(void);

/**************************************************************************//**
  \brief Returns true if sleep when idle is enabled. False otherwise.
******************************************************************************/
bool SYS_IsSleepWhenIdleEnabled(void);

#endif /* _SLEEP_WHEN_IDLE_ */

#endif /* _SYS_IDLE_HANDLER_H_ */
// eof sysIdleHandler.c