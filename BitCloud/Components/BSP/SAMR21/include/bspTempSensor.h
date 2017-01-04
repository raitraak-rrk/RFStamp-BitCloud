/**************************************************************************//**
  \file  bspTempSensor.h

  \brief The header file describes the tempsensor interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    5/9/14 Unithra.C - Created
 ******************************************************************************/

#ifndef _BSPUID_H
#define _BSPUID_H

/**************************************************************************//**
  \brief Reads temperature from internal temperature sensor
  \param value - pointer to the value to be written.
  \return none
******************************************************************************/
void BSP_ReadTempSensor(int16_t *value);
/**************************************************************************//**
  \brief Initializes temperature sensor and redirects the output to ADC
  \param none
  \return none
******************************************************************************/
void BSP_OpenTempSensor(void);
/**************************************************************************//**
  \brief stops measuring and closes the ADC channel
  \param none
  \return none
******************************************************************************/
void BSP_CloseTempSensor(void);

#endif
// eof bspTempSensor.h
