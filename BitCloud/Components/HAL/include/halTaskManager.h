/**************************************************************************//**
  \file  halTaskManager.h

  \brief Declarations of enums and functions of HAL task manager.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    10/12/07 A. Khromykh - Created
    07/04/14 karthik.p_u - Modified
 ******************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/

#ifndef _HALTASKHANDLER_H
#define _HALTASKHANDLER_H

/******************************************************************************
                   Includes section
******************************************************************************/
#include <sysTaskManager.h>

/******************************************************************************
                               Types section
*******************************************************************************/
#if defined(AT91SAM7X256)

#define HAL_ALL_TASKS_ACCEPTED_MASK 0xFFFFU

typedef enum
{
  HAL_USB_ENDPOINTS,
  HAL_USB_SUSPEND,
  HAL_USB_RESUME,
  HAL_USB_BUS_RESET,
  HAL_TASK_USART,
  HAL_TWI,           /** twi bus interrupt is happened */
  HAL_TASK_ADC,      /** adc measurement is ready */
  HAL_TIMER4_COMPA,  /** timer interrupt is happened */
  HAL_SPI0_TXBUFE,   /** spi0 dma buffer is empty */
  HAL_SPI0_RXBUFF,   /** spi0 dma buffer is full */
  HAL_SPI1_TXBUFE,   /** spi1 dma buffer is empty */
  HAL_SPI1_RXBUFF,   /** timer interrupt is happened */
  HAL_MAX_TASKS_ID
} HalTaskIds_t;

typedef volatile uint16_t HalTaskBitMask_t;

#elif defined(ATMEGA1281) || defined(ATMEGA2561) || defined(ATMEGA1284) || defined(AT90USB1287) \
   || defined(ATXMEGA128A1) || defined(ATXMEGA256A3) || defined(ATXMEGA128B1) || defined(ATXMEGA256D3) \
   || defined(ATMEGA128RFA1) || defined(ATMEGA256RFR2) || defined(ATMEGA2564RFR2)

#define HAL_ALL_TASKS_ACCEPTED_MASK 0xFFFFU

typedef enum
{
  HAL_USB_ENDPOINTS,
  HAL_USB_SUSPEND,
  HAL_USB_RESUME,
  HAL_USB_BUS_RESET,
  HAL_TASK_USART,
  HAL_TIMER4_COMPA,
  HAL_ASYNC_TIMER,
  HAL_SYNC_SLEEP_TIME,
  HAL_WAKEUP,
  HAL_SLEEP,
  HAL_TASK_SPI,
  HAL_SM_REQ,
  HAL_TASK_ADC,
  HAL_EE_READY,
  HAL_TWI,
  HAL_EXT_HANDLER,
  HAL_MAX_TASKS_ID
} HalTaskIds_t;

typedef volatile uint16_t HalTaskBitMask_t;

#elif defined(AT32UC3A0512) || defined(WIN)

#define HAL_ALL_TASKS_ACCEPTED_MASK 0xFFU

typedef enum
{
  HAL_APPTIMER,
  HAL_TASK_USART,
  HAL_MAX_TASKS_ID
} HalTaskIds_t;

typedef volatile uint8_t HalTaskBitMask_t;

#elif defined(AT91SAM3S4C) || defined(AT91SAM4S16C)

#define HAL_ALL_TASKS_ACCEPTED_MASK 0xFFFFU

typedef enum
{
  HAL_USB_ENDPOINTS,
  HAL_USB_SUSPEND,
  HAL_USB_RESUME,
  HAL_USB_BUS_RESET,
  HAL_APPTIMER,
  HAL_ASYNC_TIMER,
  HAL_WAKEUP,
  HAL_SLEEP,
  HAL_TASK_USART,
  HAL_TASK_UART,
  HAL_EE_READY,
  HAL_HSMCI,
  HAL_HIDDEN_WAKEUP,
  HAL_MAX_TASKS_ID
} HalTaskIds_t;

typedef volatile uint16_t HalTaskBitMask_t;

#elif defined (ATSAMD20J18) || defined (ATSAMR21G18A) || defined(ATSAMR21E18A) || defined(ATSAMR21E19A)

#define HAL_ALL_TASKS_ACCEPTED_MASK 0xFFFFU

typedef enum
{
  HAL_APPTIMER,
  HAL_TASK_USART,
  #if defined(ATSAMR21G18A) || defined(ATSAMR21E18A) || defined(ATSAMR21E19A)
    HAL_TASK_ADC,
    HAL_EE_READY,
    HAL_TASK_TWI,
    HAL_ASYNC_TIMER,
    HAL_WAKEUP,
    HAL_SLEEP,
    HAL_SYNC_SLEEP_TIME,
    HAL_TASK_USB,
  #endif
  HAL_MAX_TASKS_ID
} HalTaskIds_t;

typedef volatile uint16_t HalTaskBitMask_t;

#endif

/**************************************************************************//**
  \brief HAL USART task type declaration.
******************************************************************************/
typedef void (* HalTask_t)(void);

/******************************************************************************
                   External variables section
******************************************************************************/
extern volatile HalTaskBitMask_t halTaskFlags;
extern volatile HalTaskBitMask_t halAcceptedTasks;

/******************************************************************************
                   Inline static functions section
******************************************************************************/
/**************************************************************************//**
\brief Set task for HAL task manager.

\param[in]
  task id
******************************************************************************/
INLINE void halPostTask(HalTaskIds_t id)
{
  ATOMIC_SECTION_ENTER
  halTaskFlags |= ((HalTaskBitMask_t)1 << id);
  ATOMIC_SECTION_LEAVE
  SYS_PostTask(HAL_TASK_ID);
}

/**************************************************************************//**
\brief Clear task for HAL task manager.

\param[in]
  task id
******************************************************************************/
INLINE void halClearTask(HalTaskIds_t id)
{
  ATOMIC_SECTION_ENTER
  halTaskFlags &= ~((HalTaskBitMask_t)1 << id);
  ATOMIC_SECTION_LEAVE
}

/**************************************************************************//**
\brief Holds execution of required tasks.

\param[in] mask - mask of tasks to not be held
******************************************************************************/
INLINE void HAL_HoldOnTasks(HalTaskBitMask_t mask)
{
  halAcceptedTasks = mask;
}

/**************************************************************************//**
\brief Accepts execution of previously holded tasks.
******************************************************************************/
INLINE void HAL_ReleaseAllHeldTasks(void)
{
  halAcceptedTasks = HAL_ALL_TASKS_ACCEPTED_MASK;
  SYS_PostTask(HAL_TASK_ID);
}

#endif  /*_HALTASKHANDLER_H*/

// eof halTaskManager.h
