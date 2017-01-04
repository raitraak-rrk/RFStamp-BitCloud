/**************************************************************************//**
  \file  sysIdleHandler.c

  \brief Implementation of system idle handler.

  \author
      Atmel Corporation: http://www.atmel.com \n
      Support email: avr@atmel.com

    Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
    Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
     14/01/13 D. Kolmakov - Created
 ******************************************************************************/

/******************************************************************************
                   Includes section
******************************************************************************/
#include <zdo.h>
#include <sysSleep.h>

/******************************************************************************
                   Prototypes section
******************************************************************************/
#if defined(_ENDDEVICE_) && defined(_SLEEP_WHEN_IDLE_)
#ifndef _ZGPD_SPECIFIC_
static void sysIdleHandlerSleepConf(ZDO_SleepConf_t *conf);
#endif
#endif /* _ENDDEVICE_ && _SLEEP_WHEN_IDLE_ */

/******************************************************************************
                   Global variables section
******************************************************************************/
#if defined(_ENDDEVICE_) && defined(_SLEEP_WHEN_IDLE_)
#ifndef _ZGPD_SPECIFIC_
static ZDO_SleepReq_t sleepReq;
#endif
static bool sleepWhenIdleEnabled = false;
#endif /* _ENDDEVICE_ && _SLEEP_WHEN_IDLE_ */

/******************************************************************************
                   Implementations section
******************************************************************************/
void SYS_IdleHandler(void)
{
#if defined(_ENDDEVICE_) && defined(_SLEEP_WHEN_IDLE_)
  if (sleepWhenIdleEnabled)
  {
#ifdef _ZGPD_SPECIFIC_
     HAL_Sleep_t halSleep;
     uint32_t maxSleepTime = ~0UL;
     SYS_PostEvent(BC_EVENT_MAX_SLEEP_TIME_REQUEST, (SYS_EventData_t)&maxSleepTime);

     if(maxSleepTime != ~0UL)
     {
       halSleep.sleepTime = maxSleepTime;
       halSleep.callback = NULL;
       SYS_Sleep(&halSleep);
     }
#else
    bool rxOnWhenIdle;
    DeviceType_t deviceType;

    CS_ReadParameter(CS_DEVICE_TYPE_ID, &deviceType);
    CS_ReadParameter(CS_RX_ON_WHEN_IDLE_ID, &rxOnWhenIdle);

    if (DEVICE_TYPE_END_DEVICE == deviceType &&
        false == rxOnWhenIdle)
    {
      sleepReq.ZDO_SleepConf = sysIdleHandlerSleepConf;
      ZDO_SleepReq(&sleepReq);
      return;
    }
#endif
  }
#endif /* _ENDDEVICE_ && _SLEEP_WHEN_IDLE_ */

  SYS_IDLE;
}

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
void SYS_EnableSleepWhenIdle(void)
{
#if defined(_ENDDEVICE_)
  sleepWhenIdleEnabled = true;
#endif /* _ENDDEVICE_ */
}

/**************************************************************************//**
  \brief Disables sleep when device is idle.
******************************************************************************/
void SYS_DisableSleepWhenIdle(void)
{
#if defined(_ENDDEVICE_)
  sleepWhenIdleEnabled = false;
#endif /* _ENDDEVICE_ */
}

/**************************************************************************//**
  \brief Returns true if sleep when idle is enabled. False otherwise.
******************************************************************************/
bool SYS_IsSleepWhenIdleEnabled(void)
{
#if defined(_ENDDEVICE_) && defined(_SLEEP_WHEN_IDLE_)
  return sleepWhenIdleEnabled;
#else
  return false;
#endif /* _ENDDEVICE_ && _SLEEP_WHEN_IDLE_ */
}

#if defined(_ENDDEVICE_)
#ifndef _ZGPD_SPECIFIC_
/**************************************************************************//**
  \brief Sleep request confirm.

  \param[in]  conf - parameters of the confirm.
******************************************************************************/
static void sysIdleHandlerSleepConf(ZDO_SleepConf_t *conf)
{
  (void)conf;
}

#endif /* _ZGPD_SPECIFIC_ */
#endif /* _ENDDEVICE_ */
#endif /* _SLEEP_WHEN_IDLE_ */

// eof sysIdleHandler.c