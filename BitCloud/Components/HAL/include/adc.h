/**************************************************************************//**
  \file adc.h

  \brief The header file describes the ADC interface

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    22.11.07 A. Khromykh - Created.
 ******************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/

#ifndef _ADC_H
#define _ADC_H

// \cond
/******************************************************************************
                   Includes section
******************************************************************************/
#include <halAdc.h>
// \endcond

/******************************************************************************
                   Types section
******************************************************************************/
/**************************************************************************//**
\brief ADC configuration parameters
******************************************************************************/
typedef struct
{
  /** \brief ADC service field - contains variables for ADC module's
      internal needs */
  HalAdcService_t service;
  /** \brief tty - ADC_HW_CHANNEL_n to be used. "n" range depends on the platform.
      See the halAdc.h platform-specific file fore more details. */
  AdcHwChannel_t tty;
  /** \brief Conversion resolution. May be chosen from: \n
    RESOLUTION_8_BIT  \n
    RESOLUTION_10_BIT (only for AVR and ARM)\n
    RESOLUTION_12_BIT (only for XMEGA) */
  HAL_AdcResolution_t resolution;
  /** \brief ADC sample rate.
      See the halAdc.h platform-specific file fore more details. */
  HAL_AdcSampleRate_t sampleRate;
  /** \brief ADC voltage reference selections (only for AVR and XMEGA)
    May be chosen from: \n
    - for AVR hardware platform: \n
      AREF \n
      AVCC \n
      INTERNAL_1d1V \n
      INTERNAL_2d56V \n
    - for XMEGA hardware platform: \n
      INTERNAL_1V \n
      INTERNAL_VCCdiv1d6V \n
      AREFA \n
      AREFB \n */
  HAL_AdcVoltageReference_t voltageReference;
  /** \brief Pointer to application data */
  volatile void *bufferPointer;
  /** \brief The number of samples (buffer cells) */
  volatile uint16_t selectionsAmount;
  /** \brief Pointer to a callback function that is going to be called after
      all conversions. Must not be set to NULL. */
  void (* callback)(void);
  uint8_t corrEn;
  uint8_t sampleNum;
  uint8_t adjRes;
  uint8_t sampTime;
} HAL_AdcDescriptor_t;

/******************************************************************************
                   Prototypes section
******************************************************************************/
/**************************************************************************//**
\brief Opens ADC to make measurements on an ADC channel

The function must be called before the HAL_ReadAdc() function in order to start
working with ADC.

Note that for XMEGA hardware channels from the same group (channels are grouped
by 4 channels: ADC_HW_CHANNEL_A0..A3 and ADC_HW_CHANNEL_B0..B3) should have the
same resolution, conversion mode, reference voltage and sample rate. For other
supported hardware platforms channels from the same group may have different
parameters.

\param[in]
  descriptor - pointer to an instance of HAL_AdcDescriptor_t type

\return
  -1 - unsupported parameter or ADC is busy \n
   0 - success
******************************************************************************/
int HAL_OpenAdc(HAL_AdcDescriptor_t *descriptor);

/**************************************************************************//**
\brief Starts ADC with parameters defined in HAL_OpenAdc()

\param[in] descriptor - pointer to an instance of HAL_AdcDescriptor_t type
           that was provided to the HAL_OpenAdc() function

\param[in] channel - logical channel \n
Take a look into halAdc.h platform specific file fore more details.

\return
  -1 - unable to open ADC (unsupported channel number) \n
   0 - success
******************************************************************************/
int HAL_ReadAdc(HAL_AdcDescriptor_t *descriptor, HAL_AdcChannelNumber_t channel);

/**************************************************************************//**
\brief Closes ADC - must be called after work with ADC has finished

\param[in] descriptor - pointer to an instance of HAL_AdcDescriptor_t structure

\return
  -1 - the module was not open \n
   0 - success
******************************************************************************/
int HAL_CloseAdc(HAL_AdcDescriptor_t *descriptor);

#endif /* _ADC_H */
// eof adc.h
