/**************************************************************************//**
\file   dithPwm.h

\brief  The public API of pwm with dithering.

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal
  History:
    19/02/13 A. Khromykh - Created
*******************************************************************************/
#ifndef _PWMDITHERING_H
#define _PWMDITHERING_H

/******************************************************************************
                   Includes section
******************************************************************************/
#include <sysTypes.h>

/******************************************************************************
                   Prototypes section
******************************************************************************/
/**************************************************************************//**
\brief Enable/Disable dithering

\param[in]
  cha - enable/disable dithering on timer channel A
\param[in]
  chb - enable/disable dithering on timer channel B
\param[in]
  chc - enable/disable dithering on timer channel C
******************************************************************************/
void dithSetDithering(uint8_t cha, uint8_t chb, uint8_t chc);

/**************************************************************************//**
\brief Start hardware timer which is used for pwm with dithering

Initial values: frequency 20kHz
                fast pwm mode, TOP=ICRx
                prescaler 1
                ch A, B, C are set to 50% duty cycle
******************************************************************************/
void dithStartTimer(void);

/**************************************************************************//**
\brief Stop hardware timer which is used for pwm with dithering
******************************************************************************/
void dithStopTimer(void);

/**************************************************************************//**
\brief Set duty cycle on timer channels. From 0 to FFFF.

\param[in]
  dccha - duty cycle on channel A
\param[in]
  dcchb - duty cycle on channel B
\param[in]
  dcchc - duty cycle on channel C
******************************************************************************/
void dithSetDutyCycles(uint16_t dccha, uint16_t dcchb, uint16_t dcchc);

/**************************************************************************//**
\brief Set pwm frequency. There are two diapasons 200..1000Hz and 20..40kHz

\param[in]
  freq - duty cycle on channel A
******************************************************************************/
void dithSetFrequency(uint16_t freq);

#endif /* _PWMDITHERING_H */
