/**************************************************************************//**
\file  halTempSensor.c

\brief Implementation of hardware dependent tempearture sensor.

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal
  History:
    09/09/14 Unithra.C - Created
*******************************************************************************/
#if defined(HAL_USE_TS)
/******************************************************************************
                   Includes section
******************************************************************************/
#include <haltempSensor.h>
#include <tempSensor.h>
#include <adc.h>
/******************************************************************************
                   Define(s) section
******************************************************************************/
#define TEMPERATURE_SCALE 100
/******************************************************************************
                   Implementations section
******************************************************************************/
/**************************************************************************//**
\brief Temperature sensing done callback
******************************************************************************/
void tempSensorCallback(void)
{
  float temp;
  temp = *((int16_t *)((tsDesc->tsAdcDescriptor).bufferPointer));
  temp = (temp * VREF)/ADCMAXVAL;	/* temp sensor voltage output */
  temp = BASETEMP + ((temp - BASEVLTG)*TSGRAD);
  *((int *)(tsDesc->temperature)) = (int16_t)(temp*TEMPERATURE_SCALE);
  tsDesc->tsCallback();
}
/**************************************************************************//**
\brief Enable temperature sensor and initialize ADC for measurement
******************************************************************************/
int halOpenTempSensor(void)
{
  SYSCTRL_VREF_s.tsen = 1;
  (tsDesc->tsAdcDescriptor).callback = tempSensorCallback;
  return (HAL_OpenAdc(&(tsDesc->tsAdcDescriptor)));
}
/**************************************************************************//**
\brief Measure the voltage given by temperature sensor through ADC
******************************************************************************/
int halStartTempSensor(void)
{
  return (HAL_ReadAdc(&(tsDesc->tsAdcDescriptor), TSPIN));
}
/**************************************************************************//**
\brief Stop measuring the temp sensor voltage through ADC
******************************************************************************/
int halCloseTempSensor(void)
{
  return (HAL_CloseAdc(&(tsDesc->tsAdcDescriptor)));
}
#endif // defined(HAL_USE_TS)

// eof halTempSensor.c
