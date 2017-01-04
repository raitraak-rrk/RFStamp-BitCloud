/**************************************************************************//**
  \file   bspVoltageSensor.c

  \brief  Implementation of Voltage sensor interface.

  \author
      Atmel Corporation: http://www.atmel.com \n
      Support email: avr@atmel.com

    Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
    Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      18/11/14 Prashanth - Created
 ******************************************************************************/
#if BSP_SUPPORT != BOARD_FAKE
/******************************************************************************
                   includes section
******************************************************************************/
#include <systypes.h>
#include <stdlib.h>
#include <bspVoltageSensor.h>

/**************************************************************************//**
  \brief Reads Voltage in millivolts
  \param value - pointer to the value to be written.
  \return none
******************************************************************************/
void BSP_ReadVoltageSensor(uint16_t *value)
{
  /* Read measured Voltage sensor */
  *value = (uint16_t)rand();
}

/**************************************************************************//**
  \brief Reads Battery percentage in percentage
  \param value - pointer to the value to be written.
  \return none
******************************************************************************/
void BSP_ReadBatPercentageRemaining(uint8_t *value)
{
  /* Read Battery percentage */
  *value = (uint8_t)rand();
}

/**************************************************************************//**
  \brief Initializes Voltage sensor
  \param none
  \return none
******************************************************************************/
void BSP_OpenVoltageSensor(void)
{
  /* To do Voltage sensor initialization and measurement */
}
/**************************************************************************//**
  \brief stops measuring the voltage
  \param none
  \return none
******************************************************************************/
void BSP_CloseVoltageSensor(void)
{
  /* To do stop measuring */
}
#endif // BSP_SUPPORT != BOARD_FAKE

// eof bspVoltageSensor.h
