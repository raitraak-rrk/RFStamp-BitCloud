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
    09/09/14 Unithra.C - Created
*******************************************************************************/
#ifndef _HALTS_H
#define _HALTS_H
/******************************************************************************
                   Define(s) section
******************************************************************************/
#define VREF       1.65
#define ADCMAXVAL  4095
#define BASETEMP   25
#define BASEVLTG   0.667
#define TSGRAD     (1000/2.4)
#define TSPIN      0x18
/******************************************************************************
                   Prototypes section
******************************************************************************/
/**************************************************************************//**
\brief Enable temperature sensor and initialize ADC for measurement
******************************************************************************/
int halOpenTempSensor(void);
/**************************************************************************//**
\brief Measure the voltage given by temperature sensor through ADC
******************************************************************************/
int halStartTempSensor(void);
/**************************************************************************//**
\brief Stop measuring the temp sensor voltage through ADC
******************************************************************************/
int halCloseTempSensor(void);

#endif  /* _HALTS_H */

// eof halTempSensor.h


