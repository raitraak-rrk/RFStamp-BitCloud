/**************************************************************************//**
  \file  N_LowPower.c

  \brief Implementation power management event handler.

  \author
      Atmel Corporation: http://www.atmel.com \n
      Support email: avr@atmel.com

    Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
    Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
     17/01/13 D. Kolmakov - Created
 ******************************************************************************/

/******************************************************************************
                   Includes section
******************************************************************************/
#include <sysSleep.h>
#include <sysIdleHandler.h>
#include <zdo.h>
#include <halSleep.h>

#include <N_Watchdog_Internal.h>
#include <N_Watchdog_Init.h>
#include <N_LowPower.h>
#include <N_ErrH.h>

#if (defined(ZIGBEE_END_DEVICE))
  #include <N_Connection_Private.h>
#endif

/******************************************************************************
                   Defines section
******************************************************************************/
#define COMPID "N_LowPower"

#ifndef N_LOW_POWER_MAX_SUBSCRIBERS
  #define N_LOW_POWER_MAX_SUBSCRIBERS 2
#endif /* N_LOW_POWER_MAX_SUBSCRIBERS */

/******************************************************************************
                   Prototypes section
******************************************************************************/
static void sleepEventsHandler(SYS_EventId_t eventId, SYS_EventData_t data);
static void ZDO_WakeUpConf(ZDO_WakeUpConf_t *conf);
static void isPollRequired(SYS_EventId_t eventId, SYS_EventData_t data);

/******************************************************************************
                   Global variables section
******************************************************************************/
static N_LowPower_Callback_t nLowPowerSubscribers[N_LOW_POWER_MAX_SUBSCRIBERS] = NULL;
static N_LowPower_SleepLevel_t sleepLevel;
static ZDO_WakeUpReq_t zdoWakeUpReq;
static SYS_EventReceiver_t nLowPowerEventListener = { .func = sleepEventsHandler};
static SYS_EventReceiver_t nLowPowerPollCheck = { .func = isPollRequired};

/******************************************************************************
                   Extern section
******************************************************************************/
#if defined(ZIGBEE_END_DEVICE) && defined(_SLEEP_WHEN_IDLE_)
extern uint32_t appPollRate;
#endif

/******************************************************************************
                   Implementations section
******************************************************************************/
/**************************************************************************//**
  \brief Notifies subscribers about N_LowPower events

  \param[in] status - power mode status
 ******************************************************************************/
INLINE void nLowPowerNotifySubscribers(N_LowPower_SleepStatus_t status)
{
  for (uint8_t i = 0; i < N_LOW_POWER_MAX_SUBSCRIBERS; i++)
  {
    if (nLowPowerSubscribers[i])
      nLowPowerSubscribers[i](status, sleepLevel);
  }
}

/**************************************************************************//**
  \brief Initializes component.
 ******************************************************************************/
void N_LowPower_Init(void)
{
  // Subscribe to BitCloud events
  SYS_SubscribeToEvent(HAL_EVENT_FALLING_ASLEEP, &nLowPowerEventListener);
  SYS_SubscribeToEvent(HAL_EVENT_WAKING_UP, &nLowPowerEventListener);
  SYS_SubscribeToEvent(BC_EVENT_POLL_REQUEST, &nLowPowerPollCheck);
}
/**************************************************************************//**
  \brief Subscribe for notifications from this component.

  \param[in] pCallback - the callback to subscribe.
 ******************************************************************************/
void N_LowPower_Subscribe_Impl(N_LowPower_Callback_t pCallback)
{
  for (uint8_t i = 0; i < N_LOW_POWER_MAX_SUBSCRIBERS; i++)
  {
    if (!nLowPowerSubscribers[i])
    {
      nLowPowerSubscribers[i] = pCallback;
      return;
    }
  }

  N_ERRH_ASSERT_FATAL(0); /* Not enough memory to subscribe to N_LowPower */
}

/**************************************************************************//**
  \brief Sets the sleep mode. The mode after startup is "allow sleep".

  \param[in] mode - the mode (allow sleep, keep awake).
 ******************************************************************************/
void N_LowPower_SetSleepMode_Impl(N_LowPower_SleepMode_t mode)
{
#if defined(_ENDDEVICE_) && defined(_SLEEP_WHEN_IDLE_)
  if (N_LowPower_SleepMode_AllowSleep == mode)
  {
    SYS_EnableSleepWhenIdle();
  }
  else if (N_LowPower_SleepMode_KeepAwake == mode)
  {
    SYS_DisableSleepWhenIdle();
  }
#endif /* _ENDDEVICE_ && _SLEEP_WHEN_IDLE_ */
}

/**************************************************************************//**
  \brief Processes HAL_EVENT_WAKING_UP and HAL_EVENT_FALLING_ASLEEP events

  \param[in] eventId - id of raised event;
  \param[in] data    - event's data.
 ******************************************************************************/
static void sleepEventsHandler(SYS_EventId_t eventId, SYS_EventData_t data)
{
  HAL_SleepControl_t *sleepControl = (HAL_SleepControl_t *)data;

  switch (eventId)
  {
    case HAL_EVENT_FALLING_ASLEEP:

      nLowPowerNotifySubscribers(N_LowPower_SleepStatus_EnterSleep);

      N_Watchdog_Disable();
      break;

    case HAL_EVENT_WAKING_UP:
      if (HAL_SLEEP_MODE_POWER_DOWN == sleepControl->sleepMode)
        sleepLevel = N_LowPower_SleepLevel_Deep;
      else
        sleepLevel = N_LowPower_SleepLevel_Timed;

      N_Watchdog_ReInit();

      if (HAL_WAKEUP_SOURCE_EXT_IRQ == sleepControl->wakeupSource)
      {
        zdoWakeUpReq.ZDO_WakeUpConf = ZDO_WakeUpConf;
        ZDO_WakeUpReq(&zdoWakeUpReq);
      }

      nLowPowerNotifySubscribers(N_LowPower_SleepStatus_LeaveSleep);

# if (defined(ZIGBEE_END_DEVICE))
      if (N_LowPower_SleepLevel_Deep == sleepLevel)
      {
        N_Connection_Disconnected();
      }
# endif
      break;

    default: break;
  }
}

/**************************************************************************//**
  \brief Processes BC_EVENT_POLL_REQUEST event

  \param[in] eventId - id of raised event;
  \param[in] data    - event's data.
 ******************************************************************************/
static void isPollRequired(SYS_EventId_t eventId, SYS_EventData_t data)
{
#if defined(ZIGBEE_END_DEVICE) && defined(_SLEEP_WHEN_IDLE_)
  bool *check = (bool *)data;

  if (appPollRate && (ZDO_IN_NETWORK_STATUS == ZDO_GetNwkStatus()))
  {
    *check |= true;
  }
#else
  (void)eventId, (void)data;
#endif
}

/**************************************************************************//**
  \brief Wake up confirmation handler.

  \param[in] conf - confirmation parameters.
 ******************************************************************************/
void ZDO_WakeUpConf(ZDO_WakeUpConf_t *conf)
{
  (void)conf;
}

// Listener to events