/**************************************************************************//**
  \file pwm.h

  \brief Description of the PWM interface

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
   10/11/08 A. Taradov - Created
   5/04/11 A.Razinkov - Refactored
    07/04/14 karthik.p_u - Modified
 ******************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/

#ifndef _PWM_H
#define _PWM_H

/******************************************************************************
                   Includes section
******************************************************************************/
#include <sysTypes.h>
#include <atomic.h>
#include <sysUtils.h>

/******************************************************************************
                   Defines section
******************************************************************************/
#if defined(ATSAMR21G18A) || defined(ATSAMR21E18A) || defined(ATSAMR21E19A)
#define CTRLA(pwmUnit)        MMIO_REG(pwmUnit, uint32_t)
#define CTRLBCLR(pwmUnit)     MMIO_REG(pwmUnit + 4U, uint8_t)
#define CTRLBSET(pwmUnit)     MMIO_REG(pwmUnit + 5U, uint8_t)
#define SYNCBUSY(pwmUnit)     MMIO_REG(pwmUnit + 8U, uint32_t)
#define DRVCTRL(pwmUnit)      MMIO_REG(pwmUnit + 0x18U, uint32_t)
#define COUNT(pwmUnit)        MMIO_REG(pwmUnit + 0x34U, uint32_t)
#define WAVE(pwmUnit)         MMIO_REG(pwmUnit + 0x3CU, uint32_t)
#define PER(pwmUnit)          MMIO_REG(pwmUnit + 0x40U, uint32_t)
#define CC0(pwmUnit)          MMIO_REG(pwmUnit + 0x44U, uint32_t)
#define CC1(pwmUnit)          MMIO_REG(pwmUnit + 0x48U, uint32_t)
#define CC2(pwmUnit)          MMIO_REG(pwmUnit + 0x4CU, uint32_t)
#define CC3(pwmUnit)          MMIO_REG(pwmUnit + 0x50U, uint32_t)
#define CCx(pwmUnit,channel)  MMIO_REG(pwmUnit + 0x44U + channel*4, uint32_t)

#define CHANNELS_PER_UNIT 8

#else
#define TCCRnA(pwmUnit) MMIO_BYTE(pwmUnit)
#define TCCRnB(pwmUnit) MMIO_BYTE(pwmUnit + 1U)
#define TCCRnC(pwmUnit) MMIO_BYTE(pwmUnit + 2U)
#define TCNTn(pwmUnit)  MMIO_WORD(pwmUnit + 4U)
#define ICRn(pwmUnit)   MMIO_WORD(pwmUnit + 6U)
#define OCRnA(pwmUnit)  MMIO_WORD(pwmUnit + 8U)
#define OCRnB(pwmUnit)  MMIO_WORD(pwmUnit + 10U)
#define OCRnC(pwmUnit)  MMIO_WORD(pwmUnit + 12U)
#endif

#define PWM_SUCCESS_STATUS            0
#define PWM_INVALID_UNIT_STATUS      -1
#define PWM_INVALID_CHANNEL_STATUS   -2
#define PWM_INVALID_PRESCALER_STATUS -3

/******************************************************************************
                   Types section
******************************************************************************/
#if defined(ATSAMR21G18A) || defined(ATSAMR21E18A) || defined(ATSAMR21E19A)

typedef  volatile int32_t * ddrPtr;
typedef  volatile uint32_t * ocrPtr;
typedef  volatile uint32_t   pinPos;

#else
typedef  volatile uint8_t  * ddrPtr;
typedef  volatile uint16_t * ocrPtr;
typedef  volatile uint8_t    pinPos;

#endif
/**************************************************************************//**
\brief PWM prescaler
******************************************************************************/
#if defined(ATSAMR21G18A) || defined(ATSAMR21E18A) || defined(ATSAMR21E19A)
typedef enum
{
  PWM_STOPPED,
  PWM_PRESCALER_1=1,
  PWM_PRESCALER_2=2,
  PWM_PRESCALER_4=4,
  PWM_PRESCALER_8=8,
  PWM_PRESCALER_16=16,
  PWM_PRESCALER_64=64,
  PWM_PRESCALER_256=256,
  PWM_PRESCALER_1024=1024,
  PWM_PRESCALER_INVALID
} HAL_PwmPrescaler_t;
#else
typedef enum
{
  PWM_STOPPED,
  PWM_PRESCALER_1=1,
  PWM_PRESCALER_8=8,
  PWM_PRESCALER_64=64,
  PWM_PRESCALER_256=256,
  PWM_PRESCALER_1024=1024,
  PWM_PRESCALER_EXT_CLOCK_ON_FALLING_EDGE,
  PWM_PRESCALER_EXT_CLOCK_ON_RISING_EDGE,
  PWM_PRESCALER_INVALID
} HAL_PwmPrescaler_t;
#endif

/**************************************************************************//**
\brief PWM channel
******************************************************************************/
#if defined(ATSAMR21G18A) || defined(ATSAMR21E18A) || defined(ATSAMR21E19A)
typedef enum
{
  PWM_CHANNEL_0,
  PWM_CHANNEL_1,
  PWM_CHANNEL_2,
  PWM_CHANNEL_3,
  PWM_CHANNEL_4,
  PWM_CHANNEL_5,
  PWM_CHANNEL_6,
  PWM_CHANNEL_7,
  PWM_INVALID_CHANNEL
} HAL_PwmChannel_t;
#else
typedef enum
{
  PWM_CHANNEL_0,
  PWM_CHANNEL_1,
#ifndef ATMEGA1284
  PWM_CHANNEL_2,
#endif /* ATMEGA1284 */
  PWM_INVALID_CHANNEL
} HAL_PwmChannel_t;
#endif

/**************************************************************************//**
\brief PWM impulse polarity
******************************************************************************/
typedef enum
{
  /** \brief PWM output is low when duty cycle = 0% */
  PWM_POLARITY_NON_INVERTED,
  /** \brief PWM output is high when duty cycle = 0% */
  PWM_POLARITY_INVERTED
} HAL_PwmPolarity_t;

/**************************************************************************//**
\brief PWM unit number. Relative to corresponding Timer/Counter.
******************************************************************************/
#if defined(ATSAMR21G18A) || defined(ATSAMR21E18A) || defined(ATSAMR21E19A)
typedef enum
{
  /* Base address of TCC0 register */
  PWM_UNIT_0 = 0x42002000,
  /* Base address of TCC1 register */
  PWM_UNIT_1 = 0x42002400,
  /* Base address of TCC2 register */
  PWM_UNIT_2 = 0x42002800,
  PWM_INVALID_UNIT
} HAL_PwmUnit_t;

typedef enum
{
  /* No dithering */
  NO_DITH,
  /* dithering every 16 PWM frames */
  DITH4,
  /* dithering every 32 PWM frames */
  DITH5,
  /* dithering every 64 PWM frames */
  DITH6
} HAL_PwmDithering_t;

#else
typedef enum
{
  /* Base address of T/C1 PWM related registers */
  PWM_UNIT_1 = 0x80,
  /* Base address of T/C3 PWM related registers */
  PWM_UNIT_3 = 0x90
} HAL_PwmUnit_t;
#endif

/**************************************************************************//**
\brief PWM module descriptor
******************************************************************************/
typedef struct _HAL_PwmDescriptor_t
{
  /* Service fields for internal use. */
  struct
  {
    /* Pointer to Data Direction Register. PWM port dependent. */
    ddrPtr DDRn;
    /* PWN channel pin number. PWM port dependent. */
    pinPos pwmBaseChannelPin;
    /* Compare Output Mode low bit position. PWM channel dependent. */
    uint8_t COMnx0;
    /* Output Compare Register. PWM channel dependent. */
    ocrPtr OCRnx;
  } service;
  /* PWM unit number. Equal to ID of Timer/Counter witch serves PWM module. */
  HAL_PwmUnit_t unit;
#if defined(ATSAMR21G18A) || defined(ATSAMR21E18A) || defined(ATSAMR21E19A)
  /* channel pin cfg */
  PinConfig_t channelPinCfg[CHANNELS_PER_UNIT];
  /* channel mask for channels 0-7 */
  uint8_t channelMask;
  /* polarity mask for channels 0-7 */
  uint8_t polarityMask;
  /* dithering mask for channels 0-7 */
  uint8_t ditheringMask;
  /* Enhanced Dithering resoultion */
  HAL_PwmDithering_t dithResoultion;
  /* Extra cycles to add into the frame if Dithering is enabled*/
  uint8_t dithercycles;
#else
  /* PWM channel */
  HAL_PwmChannel_t channel;
  /* PWM polarity */
  HAL_PwmPolarity_t polarity;
#endif
} HAL_PwmDescriptor_t;

/******************************************************************************
                   Prototypes section
******************************************************************************/
/**************************************************************************//**
\brief Initializes the PWM.

\ingroup hal_pwm

\param [in] pwmUnit - PWM unit number.
                      Equal to ID of Timer/Counter witch serves PWM module.

\return operation status
******************************************************************************/
int HAL_OpenPwm(HAL_PwmUnit_t pwmUnit);

/**************************************************************************//**
\brief Starts PWM on specified channel.

\ingroup hal_pwm

\param [in] descriptor - PWM channel descriptor.

\return operation status
******************************************************************************/
int HAL_StartPwm(HAL_PwmDescriptor_t *descriptor);

/**************************************************************************//**
\brief Stops PWM on specified channel.

\ingroup hal_pwm

\param [in] descriptor - PWM channel descriptor.

\return operation status
******************************************************************************/
int HAL_StopPwm(HAL_PwmDescriptor_t *descriptor);

/**************************************************************************//**
\brief Sets base frequency of module. Common for all module channels.

\ingroup hal_pwm

\param [in] pwmUnit - PWM unit number. Equal to corresponding Timer/Counter ID.
\param [in] top - value for the TOP register.
\param [in] prescaler - clock prescaler.

\return operation status
******************************************************************************/
#if !defined(ATSAMR21G18A) && !defined(ATSAMR21E18A) && !defined(ATSAMR21E19A)
int HAL_SetPwmFrequency(HAL_PwmUnit_t pwmUnit, uint16_t top, HAL_PwmPrescaler_t prescaler);
#else
int HAL_SetPwmFrequency(HAL_PwmDescriptor_t *descriptor, uint16_t top, HAL_PwmPrescaler_t prescaler);
#endif

/**************************************************************************//**
\brief Sets compare value for the PWM channel.

\ingroup hal_pwm

\param [in] descriptor - PWM channel descriptor.

\return operation status
******************************************************************************/
int HAL_SetPwmCompareValue(HAL_PwmDescriptor_t *descriptor, uint16_t cmpValue);

/**************************************************************************//**
\brief Closes the PWM.

\ingroup hal_pwm

\param [in] pwmUnit - PWM unit number.
                      Equal to ID of Timer/Counter witch serves PWM module.

\return operation status
******************************************************************************/
int HAL_ClosePwm(HAL_PwmUnit_t pwmUnit);

#endif /* _PWM_H */

// eof pwm.h
