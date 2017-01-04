/**************************************************************************//**
  \file  calibration.h

  \brief Interface to calibrate the internal RC generator.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    29/06/07 E. Ivanov - Created
 ******************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/

#ifndef _CALIBRATION_H
#define _CALIBRATION_H
/**************************************************************************//**
\brief Performs calibration of the main clock generator \n
(only for AVR and internal RC oscillator).
******************************************************************************/
void HAL_CalibrateMainClock(void);

#endif /* _CALIBRATION_H */
// eof calibration.h
