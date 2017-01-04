/**************************************************************************//**
\file   dithTimerIsr.c

\brief  Hardware timer interface implementation.

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal
  History:
    19/02/13 A. Khromykh - Created
*******************************************************************************/

#if APP_ZLL_DEVICE_TYPE >= APP_DEVICE_TYPE_COLOR_LIGHT

/******************************************************************************
                   Includes section
******************************************************************************/
#include <dithTimerIsr.h>
#include <dithPwm.h>

/******************************************************************************
                             External variables section
******************************************************************************/
extern PwmParams_t currPwmParam;

/******************************************************************************
                   Implementations section
******************************************************************************/
/**************************************************************************//**
\brief  the COMPx interrupts are used for duty cycle dithering
*******************************************************************************/
INLINE void channelADithering(void)
{
  if (currPwmParam.red_ena_dith)
  {
    currPwmParam.red_sum += currPwmParam.red_m;
    if (currPwmParam.red_mode == 1)
    {
      if (currPwmParam.red_sum > UINT8_MAX)
      {
        OCRxA = currPwmParam.ocra - 1;
        currPwmParam.red_sum = currPwmParam.red_sum & 0xFF;
      }
      else
      {
        OCRxA = currPwmParam.ocra;
      }
    }
    else
    {
      if (currPwmParam.red_sum > UINT8_MAX)
      {
        TCCRxA |= (1 << COMxA1 | 1 << COMxA0);
        currPwmParam.red_sum = currPwmParam.red_sum & 0xFF;
      }
      else
      {
        TCCRxA &= ~(1 << COMxA1 | 1 << COMxA0);
      }
    }
  }
}

/**************************************************************************//**
\brief  the COMPx interrupts are used for duty cycle dithering
*******************************************************************************/
INLINE void channelBDithering(void)
{
  if (currPwmParam.green_ena_dith)
  {
    currPwmParam.green_sum += currPwmParam.green_m;
    if (currPwmParam.green_mode == 1)
    {
      if (currPwmParam.green_sum > UINT8_MAX)
      {
        currPwmParam.green_sum = currPwmParam.green_sum & 0xFF;
        OCRxB = currPwmParam.ocrb + 1;
      }
      else
      {
        OCRxB = currPwmParam.ocrb;
      }
    }
    else
    {
      if (currPwmParam.green_sum > UINT8_MAX)
      {
        currPwmParam.green_sum = currPwmParam.green_sum & 0xFF;
        TCCRxA |= (1 << COMxB1);
      }
      else
      {
        TCCRxA &= ~(1 << COMxB1);
      }
    }
  }
}

/**************************************************************************//**
\brief  the COMPx interrupts are used for duty cycle dithering
*******************************************************************************/
#if !defined(APP_THIRD_CHANNEL_ON_8BITS_TIMER)
INLINE void channelCDithering(void)
{
  if (currPwmParam.blue_ena_dith)
  {
    currPwmParam.blue_sum += currPwmParam.blue_m;
    if (currPwmParam.blue_mode == 1)
    {
      if (currPwmParam.blue_sum > UINT8_MAX)
      {
        currPwmParam.blue_sum = currPwmParam.blue_sum & 0xFF;
        OCRxC = currPwmParam.ocrc + 1;
      }
      else
      {
        OCRxC = currPwmParam.ocrc;
      }
    }
    else
    {
      if (currPwmParam.blue_sum > UINT8_MAX)
      {
        currPwmParam.blue_sum = currPwmParam.blue_sum & 0xFF;
        TCCRxA |= (1 << COMxC1);
      }
      else
      {
        TCCRxA &= ~(1 << COMxC1);
      }
    }
  }
}
#endif // !defined(APP_THIRD_CHANNEL_ON_8BITS_TIMER)

/**************************************************************************//**
\brief  Overflow interrupt handler to count dithering
*******************************************************************************/
ISR(TIMERx_OVF_vect)
{
  channelADithering();
  channelBDithering();
#if !defined(APP_THIRD_CHANNEL_ON_8BITS_TIMER)
  channelCDithering();
#endif // !defined(APP_THIRD_CHANNEL_ON_8BITS_TIMER)
}

#endif // APP_ZLL_DEVICE_TYPE >= APP_DEVICE_TYPE_COLOR_LIGHT
