/**************************************************************************//**
\file  halTaskManager.c

\brief Implemenattion of HAL task manager.

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
#include <halTaskManager.h>
#include <atomic.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
#define HANDLERS_GET(A, I) memcpy_P(A, &halHandlers[I], sizeof(HalTask_t))

#define HAL_APP_TIMER_HANDLER           halAppTimerHandler

#ifdef HAL_USE_USART
  #define HAL_USART_HANDLER             halSigUsartHandler
#else
  #define HAL_USART_HANDLER             halEmptyHandler
#endif // HAL_USE_USART

#ifdef HAL_USE_EE_READY
  #define HAL_EEPROM_HANDLER            halSigEepromReadyHandler
#else
  #define HAL_EEPROM_HANDLER            halEmptyHandler
#endif // HAL_USE_EE_READY

#ifdef HAL_USE_TWI
  #define HAL_TWI_HANDLER               halSig2WireSerialHandler
#else
  #define HAL_TWI_HANDLER               halEmptyHandler
#endif // HAL_USE_TWI

#ifdef HAL_USE_ADC
  #define HAL_ADC_HANDLER               halSigAdcHandler
#else
  #define HAL_ADC_HANDLER               halEmptyHandler
#endif // HAL_USE_ADC

#ifdef HAL_USE_USB
  #define HAL_USB_HANDLER               halSigUsbHandler
#else
  #define HAL_USB_HANDLER               halEmptyHandler
#endif // HAL_USE_USB

#define HAL_WAKEUP_HANDLER            halWakeupHandler
#define HAL_SLEEP_HANDLER             halPowerOff
#define HAL_ASYNC_TIMER_HANDLER       halAsyncTimerHandler
#define HAL_SLEEP_TIME_SYNC_HANDLER   halSleepSystemTimeSynchronize

/******************************************************************************
                   Prototypes section
******************************************************************************/
void halAppTimerHandler(void);
void halSigUsartHandler(void);
void halSigUsbHandler(void);
void halSigEepromReadyHandler(void);
void halSig2WireSerialHandler(void);
void halAsyncTimerHandler(void);
void halSleepSystemTimeSynchronize(void);
void halWakeupHandler(void);
void halPowerOff(void);
void halSigAdcHandler(void);
static void halEmptyHandler(void);


/******************************************************************************
                   Global variables section
******************************************************************************/
volatile HalTaskBitMask_t halTaskFlags = 0; // HAL USART tasks' bit mask.
volatile HalTaskBitMask_t halAcceptedTasks = HAL_ALL_TASKS_ACCEPTED_MASK;
HalTask_t PROGMEM_DECLARE(halHandlers[HAL_MAX_TASKS_ID]) =
{
  HAL_APP_TIMER_HANDLER,
  HAL_USART_HANDLER,
  HAL_ADC_HANDLER
#if defined(ATSAMR21G18A) || defined(ATSAMR21E18A) || defined(ATSAMR21E19A)
  ,HAL_EEPROM_HANDLER,
  HAL_TWI_HANDLER,
  HAL_ASYNC_TIMER_HANDLER,
  HAL_WAKEUP_HANDLER,
  HAL_SLEEP_HANDLER,
  HAL_SLEEP_TIME_SYNC_HANDLER,
  HAL_USB_HANDLER
#endif
};

/******************************************************************************
                   Implementations section
******************************************************************************/
/**************************************************************************//**
\brief Empty handler to fill empty peripheries.
******************************************************************************/
static void halEmptyHandler(void)
{}

/**************************************************************************//**
\brief HAL task handler.
******************************************************************************/
void HAL_TaskHandler(void)
{
  HalTask_t         handler;
  HalTaskBitMask_t  mask = 1;
  uint8_t           index = 0;
  HalTaskBitMask_t  tmpFlags;

  ATOMIC_SECTION_ENTER
  tmpFlags = halTaskFlags;
  ATOMIC_SECTION_LEAVE

  if (!(tmpFlags & halAcceptedTasks))
    return;

  for ( ; index < (uint8_t)HAL_MAX_TASKS_ID; index++, mask <<= 1)
  {
    if (tmpFlags & mask & halAcceptedTasks)
    {
      ATOMIC_SECTION_ENTER
      halTaskFlags ^= mask;
      ATOMIC_SECTION_LEAVE
      HANDLERS_GET(&handler, index);
      handler();
    }
  }

  (void)halEmptyHandler; // to kill warning "unused function"
}
// eof halTaskManager.c
