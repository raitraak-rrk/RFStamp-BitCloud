/**************************************************************************//**
  \file  bspVoltageSensor.h

  \brief The header file describes the voltage sensor interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    18/11/14 Prashanth - Created
 ******************************************************************************/

#ifndef _BSPVOLTAGESENSOR_H
#define _BSPVOLTAGESENSOR_H

#if BSP_SUPPORT != BOARD_FAKE

/**************************************************************************//**
  \brief Reads voltage in millivolts
  \param value - pointer to the value to be written.
  \return none
******************************************************************************/
void BSP_ReadVoltageSensor(uint16_t *value);
/**************************************************************************//**
  \brief Reads Battery percentage in percentage
  \param value - pointer to the value to be written.
  \return none
******************************************************************************/
void BSP_ReadBatPercentageRemaining(uint8_t *value);
/**************************************************************************//**
  \brief Initializes voltage sensor
  \param none
  \return none
******************************************************************************/
void BSP_OpenVoltageSensor(void);
/**************************************************************************//**
  \brief stops measuring voltage
  \param none
  \return none
******************************************************************************/
void BSP_CloseVoltageSensor(void);
#endif //BSP_SUPPORT != BOARD_FAKE
#endif
// eof bspVoltageSensor.h
