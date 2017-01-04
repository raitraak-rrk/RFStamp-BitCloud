/**************************************************************************//**
\file  halAdc.h

\brief Declaration of hardware depended ADC interface.

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal
  History:
    15/06/14 A. Mahendran - Created
*******************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/

#ifndef _HALADC_H
#define _HALADC_H

/******************************************************************************
                   Includes section
******************************************************************************/
#include <halTaskManager.h>
#include <halClkCtrl.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
#define ALL_CHANNEL_DISABLE  ALL_PERIPHERIAL_INTERRUPT_DISABLE

/******************************************************************************
                   Types section
******************************************************************************/
typedef uint8_t AdcHwChannel_t;

/**************************************************************************//**
\brief adc sample rate
******************************************************************************/
typedef enum
{
  ADC_350KSPS
} HAL_AdcSampleRate_t;

/**************************************************************************//**
\brief adc resolution
******************************************************************************/
typedef enum
{
  RESOLUTION_12_BIT,
  RESOLUTION_16_BIT,
  RESOLUTION_10_BIT,
  RESOLUTION_8_BIT,
} HAL_AdcResolution_t;

/**************************************************************************//**
\brief adc voltage reference
******************************************************************************/
typedef enum
{
  /* 1.0 voltage reference */
  INT1V,
  /* 1/1.48 VDDANA */
  INTVCC0,
  /* 1/2 VDDANA (only for VDDANA > 2.0V) */
  INTVCC1,
  /* External reference */
  AREFA,
  /* External reference */
  AREFB
} HAL_AdcVoltageReference_t;

/**************************************************************************//**
\brief channel number.
******************************************************************************/
typedef enum
{
  HAL_ADC_CHANNEL0 = 0,
  HAL_ADC_CHANNEL1 = 1,
  HAL_ADC_CHANNEL2 = 2,
  HAL_ADC_CHANNEL3 = 3,
  HAL_ADC_CHANNEL4 = 4,
  HAL_ADC_CHANNEL5 = 5,
  HAL_ADC_CHANNEL6 = 6,
  HAL_ADC_CHANNEL7 = 7,
  HAL_ADC_CHANNEL10 = 10,
  HAL_ADC_CHANNEL11 = 11,
  HAL_ADC_CHANNEL16 = 16,
  HAL_ADC_CHANNEL17 = 17,
} HAL_AdcChannelNumber_t;

typedef uint8_t HalAdcService_t;

/******************************************************************************
                   Prototypes section
******************************************************************************/
/**************************************************************************//**
\brief Set up parametres to ADC.
******************************************************************************/
void halOpenAdc(void);

/**************************************************************************//**
\brief starts convertion on the ADC channel.
\param[in]
  channel - channel number.
******************************************************************************/
void halStartAdc(HAL_AdcChannelNumber_t channel);

/**************************************************************************//**
\brief Closes the ADC.
******************************************************************************/
void halCloseAdc(void);

#endif /* _HALADC_H */

// eof halAdc.h
