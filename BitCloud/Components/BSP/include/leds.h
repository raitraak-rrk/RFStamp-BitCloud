/************************************************************//**
  \file  leds.h

  \brief The header file describes the leds interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    29/05/07 E. Ivanov - Created
    08/04/14 karthik.p_u - Modified
*****************************************************************/

#ifndef _LEDS_H
#define _LEDS_H

/****************************************************************
                   Includes section
*****************************************************************/
// \cond
#include <sysTypes.h>
// \endcond

/****************************************************************
                   Define(s) section
*****************************************************************/
#define LED_FIRST     0
#define LED_SECOND    1
#define LED_THIRD     2
#define LED_FOURTH    3
#define LED_FIFTH     4
#define LED_SIXTH     5
#define LED_SEVENTH   6
#define LED_EIGHTH    7
#define LED_RED       LED_FIRST
#define LED_YELLOW    LED_SECOND
#define LED_GREEN     LED_THIRD
#define LED_BLUE      LED_FOURTH

/****************************************************************
                   Prototypes section
*****************************************************************/
/************************************************************//**
\brief Opens leds module to use.
\return
  BC_SUCCESS - always.
****************************************************************/
result_t BSP_OpenLeds(void);

/************************************************************//**
\brief Closes leds module.
\return
  BC_SUCCESS - always.
****************************************************************/
result_t BSP_CloseLeds(void);

/************************************************************//**
\brief Turns on a led.
\param[in]
  id - number of the led to proceed. Must be chosen from defines.
****************************************************************/
void BSP_OnLed(uint8_t id);

/************************************************************//**
\brief Turns off a led.
\param[in]
  id - number of the led to proceed. Must be chosen from defines.
****************************************************************/
void BSP_OffLed(uint8_t id);

/************************************************************//**
\brief Switches the led state to the opposite.
\param[in]
  id - number of the led to proceed. Must be chosen from defines.
****************************************************************/
void BSP_ToggleLed(uint8_t id);
#endif /* _LEDS_H */
// eof leds.h
