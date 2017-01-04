/**************************************************************************//**
  \file N_Watchdog.c

  \brief Implementation of Watchdog timer.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    13.11.12 N. Fomin - created
******************************************************************************/

/******************************************************************************
                    Includes section
******************************************************************************/
#include <N_Watchdog_Bindings.h>
#include <N_Watchdog.h>
#include <N_Watchdog_Init.h>
#include <wdtCtrl.h>
#include <resetReason.h>
#include <sysEvents.h>

#include <devconfig.h>

/******************************************************************************
                    Defines section
******************************************************************************/
#define COMPID "N_Watchdog"

#if defined DEVCFG_DISABLE_WATCHDOG
  #warning WATCHDOG DISABLED!
#endif // DEVCFG_DISABLE_WATCHDOG

#ifndef DEVCFG_DISABLE_WATCHDOG
/******************************************************************************
                    Prototypes section
******************************************************************************/
static void kickWatchdogOnEventReceiving(SYS_EventId_t id, SYS_EventData_t data);

/******************************************************************************
                    Static variables section
******************************************************************************/
static SYS_EventReceiver_t nWatchdogEventListener = {.func = kickWatchdogOnEventReceiving};
#endif /* DEVCFG_DISABLE_WATCHDOG */

/******************************************************************************
                    Implementation section
******************************************************************************/
/** Initializes the component.
*/
void N_Watchdog_Init(N_Watchdog_TimerExpired_t pfTimerExpired)
{
#ifndef DEVCFG_DISABLE_WATCHDOG
  SYS_SubscribeToEvent(HAL_EVENT_CONTINUING_SLEEP, &nWatchdogEventListener);
  SYS_SubscribeToEvent(SYS_EVENT_TASK_PROCESSED, &nWatchdogEventListener);
  HAL_RegisterWdtCallback(pfTimerExpired);
  HAL_StartWdt(WDT_INTERVAL_8000);
#endif /* DEVCFG_DISABLE_WATCHDOG */
}


/** Re Initializes the component.
*/
void N_Watchdog_ReInit(void)
{
#ifndef DEVCFG_DISABLE_WATCHDOG
  HAL_StartWdt(WDT_INTERVAL_8000);
#endif /* DEVCFG_DISABLE_WATCHDOG */
}

/** Disable the watchdog. */
void N_Watchdog_Disable(void)
{
#ifndef DEVCFG_DISABLE_WATCHDOG
  HAL_StopWdt();
#endif /* DEVCFG_DISABLE_WATCHDOG */
}

/** Kick the watchdog so it will not reset the hardware */
void N_Watchdog_Kick_Impl(void)
{
#ifndef DEVCFG_DISABLE_WATCHDOG
  HAL_ResetWdt();
#endif /* DEVCFG_DISABLE_WATCHDOG */
}

/** Get the reason the device reset last time
    \returns The cause of the previous reset
*/
ResetReason_t N_Watchdog_GetResetReason_Impl(void)
{
  HAL_ResetReason_t reason = HAL_ReadResetReason();

  if (POWER_ON_RESET == reason)
    return ResetReason_PowerOn;
  if (EXTERNAL_RESET == reason)
    return ResetReason_External;
  if (WDT_RESET == reason)
    return ResetReason_Watchdog;

  return ResetReason_Unknown;
}

#ifndef DEVCFG_DISABLE_WATCHDOG
/**************************************************************************//**
  \brief Callback that is called on event receiving which this component
         is subcribed to.

  \param[in] eventId - id of raised event;
  \param[in] data    - event's data
******************************************************************************/
static void kickWatchdogOnEventReceiving(SYS_EventId_t id, SYS_EventData_t data)
{
  N_Watchdog_Kick();
  (void)id, (void)data;
}
#endif /* DEVCFG_DISABLE_WATCHDOG */

/* eof N_Watchdog.c */
