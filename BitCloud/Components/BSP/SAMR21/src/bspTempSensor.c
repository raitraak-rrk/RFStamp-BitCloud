/**************************************************************************//**
  \file   bspTempSensor.c

  \brief  Implementation of temperature sensor interface.

  \author
      Atmel Corporation: http://www.atmel.com \n
      Support email: avr@atmel.com

    Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
    Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      5/9/14 Unithra.C - Created
 ******************************************************************************/
#if BSP_SUPPORT != BOARD_FAKE
/******************************************************************************
                   includes section
******************************************************************************/
#include <tempSensor.h>
#include <halAdc.h>
/******************************************************************************
                   Global variables section
******************************************************************************/
static HAL_TSDescriptor_t tempSensor;
static uint16_t adcBuffer;
static int16_t measuredTemp;
/******************************************************************************
                   Prototypes section
******************************************************************************/
static void tempSensorHandler(void);
/******************************************************************************
                   Implementation section
******************************************************************************/
/**************************************************************************//**
  \brief Reads temperature from internal temperature sensor
  \param value - pointer to the value to be written.
  \return none
******************************************************************************/
void BSP_ReadTempSensor(int16_t *value)
{
  HAL_ReadTempSensor(&tempSensor);
  *value = measuredTemp;
}
/**************************************************************************//**
  \brief Initializes temperature sensor and redirects the output to ADC
  \param none
  \return none
******************************************************************************/
void BSP_OpenTempSensor(void)
{
  /* Configure Internal temperature sensor*/
  tempSensor.temperature = &measuredTemp;
  tempSensor.tsCallback = tempSensorHandler;
  tempSensor.tsAdcDescriptor.resolution = RESOLUTION_16_BIT;
  tempSensor.tsAdcDescriptor.sampleRate = ADC_350KSPS;
  tempSensor.tsAdcDescriptor.selectionsAmount = 1;
  tempSensor.tsAdcDescriptor.voltageReference = INTVCC1;
  tempSensor.tsAdcDescriptor.corrEn = 0; //calibration of ADC is not done
  tempSensor.tsAdcDescriptor.sampleNum = 4; //for averaging
  tempSensor.tsAdcDescriptor.adjRes = 4; //for averaging 
  tempSensor.tsAdcDescriptor.sampTime = 1; //number of ADC clock cycles
  tempSensor.tsAdcDescriptor.bufferPointer = &adcBuffer;
  tempSensor.tsAdcDescriptor.callback = NULL;
  
  HAL_OpenTempSensor(&tempSensor);
}
/**************************************************************************//**
  \brief stops measuring and closes the ADC channel
  \param none
  \return none
******************************************************************************/
void BSP_CloseTempSensor(void)
{
  HAL_CloseTempSensor(&tempSensor);
}
/**************************************************************************//**
\brief Temperature sensor handler

\param NA
******************************************************************************/
static void tempSensorHandler(void)
{
 
}
#endif // BSP_SUPPORT != BOARD_FAKE

// eof bspTempSensor.h
