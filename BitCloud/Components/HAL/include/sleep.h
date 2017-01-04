/**************************************************************************//**
  \file  sleep.h

  \brief The header file describes the sleep interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    1/12/09 A. Khromykh - Created
 ******************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/
#ifndef _SLEEP_H
#define _SLEEP_H

/******************************************************************************
                   Includes section
******************************************************************************/
#include <sysTypes.h>

/******************************************************************************
                   Types section
******************************************************************************/
typedef void (* HAL_WakeUpCallback_t)(bool);

/** \brief System sleep parameters */
typedef struct
{
  uint32_t sleepTime; //!< Time of mcu's and radio's sleeping
  bool startPoll;
  HAL_WakeUpCallback_t callback; //!< Pointer to a wake up callback function. Can be set to NULL.
} HAL_Sleep_t;

/** \brief Sleep modes enumeration. Avaible only for AVR. */
typedef enum _HAL_SleepMode_t
{
  HAL_SLEEP_MODE_NONE = 0x00,
  HAL_SLEEP_MODE_POWER_SAVE = 0x01,
  HAL_SLEEP_MODE_POWER_DOWN = 0x02,
  HAL_SLEEP_MODE_IDLE = 0x03
} HAL_SleepMode_t;

/** \brief Wake up source. Avaible only for AVR. */
typedef enum _HAL_WakeUpSource_t
{
  HAL_WAKEUP_SOURCE_SLEEP_TIMER = 0x00,
  HAL_WAKEUP_SOURCE_EXT_IRQ = 0x01
} HAL_WakeUpSource_t;

/******************************************************************************
                   Prototypes section
******************************************************************************/
/**************************************************************************//**
\brief Starts sleep timer and HAL sleep. When system is wake up send callback
\param[in]
    sleepParam - pointer to sleep structure.
\return
    -1 - bad parameters,   \n
    -2 - sleep timer busy, \n
    -3 - sleep system has been started.
     0 - success.
******************************************************************************/
int HAL_StartSystemSleep(HAL_Sleep_t *sleepParam);

/**************************************************************************//**
\brief Prepares mcu for power-save, power-down.
  Power-down the mode is possible only when internal RC is used
\return
  -1 - there is no possibility to sleep.
******************************************************************************/
int HAL_Sleep(void);

/***************************************************************************//**
\brief Makes MCU's entering the Idle mode.
*******************************************************************************/
void HAL_IdleMode(void);

#endif /* _SLEEP_H */
