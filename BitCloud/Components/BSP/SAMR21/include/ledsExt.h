/**************************************************************************//**
  \file ledsExt.h

  \brief
    Additional LEDs interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    08/04/14 karthik.p_u - Created
******************************************************************************/
#ifndef _LEDS_EXTENSION_H
#define _LEDS_EXTENSION_H

/******************************************************************************
                    Includes section
******************************************************************************/
#include <sysTypes.h>

/******************************************************************************
                    Types section
******************************************************************************/
enum
{
  LED_1   = 0,
  LED_2   = 1,
  LED_3   = 2,
  LED_4   = 3,
  LED_5   = 4,
};

/******************************************************************************
                    Prototypes section
******************************************************************************/
/**************************************************************************//**
\brief Initialize LEDs.
******************************************************************************/
void BSP_InitLeds(void);

/**************************************************************************//**
\brief Set brightness level
\param[in] level - new brightness level
******************************************************************************/
void BSP_SetLedBrightnessLevel(uint8_t level);

/**************************************************************************//**
\brief Set the color using Hue and Saturation as parameters
\param[in] level - a new brightness level
******************************************************************************/
void BSP_SetLedColorHS(uint16_t hue, uint8_t saturation);

/**************************************************************************//**
\brief Set the color using X and Y as parameters
\param[in] level - a new brightness level
******************************************************************************/
void BSP_SetLedColorXY(uint16_t x, uint16_t y);

#endif // _LEDS_EXTENSION_H

// eof leds.h
