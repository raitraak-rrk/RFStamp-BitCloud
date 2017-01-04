/**************************************************************************//**
  \file tempSensor.h

  \brief The header file describes the ADC interface

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    09/09/14 Unithra.C - Created
 ******************************************************************************/
#ifndef _TEMPSENSOR_H
#define _TEMPSENSOR_H
/******************************************************************************
                   Includes section
******************************************************************************/
#include <adc.h>
#include <sysTypes.h>
/******************************************************************************
                   Types section
******************************************************************************/
typedef struct
{
  int16_t *temperature;
  /** \brief Pointer to a callback function that is going to be called after
      all conversions. Must not be set to NULL. */
  void (* tsCallback)(void);
  HAL_AdcDescriptor_t tsAdcDescriptor;
} HAL_TSDescriptor_t;
/******************************************************************************
                   Prototypes section
******************************************************************************/
/**************************************************************************//**
  \brief Enable temperature sensor and initialize ADC for measurement

  \param[in]
    descriptor - pointer to HAL_TSDescriptor_t structure

  \return
    -1 - unsupported parameter
     0 - success.
******************************************************************************/
int HAL_OpenTempSensor(HAL_TSDescriptor_t *descriptor);
/**************************************************************************//**
  \brief Measure the voltage given by temperature sensor through ADC

  \param[in]
    descriptor - pointer to HAL_TSDescriptor_t structure

  \return
    -1 - unsupported parameter
     0 - success.
******************************************************************************/
int HAL_ReadTempSensor(HAL_TSDescriptor_t *descriptor);
/**************************************************************************//**
  \brief  Stop measuring the temp sensor voltage through ADC

  \param[in]
    descriptor - pointer to HAL_TSDescriptor_t structure

  \return
    -1 - unsupported parameter
     0 - success.
******************************************************************************/
extern int HAL_CloseTempSensor(HAL_TSDescriptor_t *descriptor);

extern HAL_TSDescriptor_t *tsDesc;
#endif	/* _TEMPSENSOR_H */
