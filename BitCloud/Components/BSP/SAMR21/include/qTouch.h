/************************************************************************//**
\file qTouch.h

\brief
 Enabling Touch with Wireless on SAMR21

\author
Atmel Corporation: http://www.atmel.com \n
Support email: avr@atmel.com

Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal
History:
18.01.12 Yogesh  - Created to Support SAMR21 with Touch(Multi Cap) Integrated 
******************************************************************************/

#if((QTOUCH_SUPPORT == 1) && ((BSP_SUPPORT == BOARD_SAMR21_ZLLEK) || (BSP_SUPPORT == BOARD_RFRINGQM) || (BSP_SUPPORT == BOARD_RFRINGQT)))

#ifndef _QTOUCH_H
#define _QTOUCH_H

/******************************************************************************
                   Includes section
******************************************************************************/
#include <gpio.h>
/**************************************************************************//**
                    Types section
******************************************************************************/
typedef enum _BSP_TouchEvent_t
{
  TOUCHKEY_NO_EVENT,
  TOUCHKEY_PRESSED_EVENT,
  TOUCHKEY_RELEASED_EVENT,
  TOUCHKEY_SLIDER_EVENT,
  TOUCHKEY_WHEEL_EVENT,
} BSP_TouchEvent_t;

#if ((BSP_SUPPORT == BOARD_RFRINGQM) || (BSP_SUPPORT == BOARD_RFRINGQT))
typedef enum _BSP_ButtonId_t
{
	NONE,
	BUTTON,
	WHEEL
}BSP_ButtonId_t;
#elif (BSP_SUPPORT == BOARD_SAMR21_ZLLEK)
typedef enum _BSP_ButtonId_t
{
	NONE,
	BUTTON_L,
	BUTTON_R,
	SLIDER
}BSP_ButtonId_t;#endif
#endif

/** Type of Touch buttons event handler */
typedef void (* BSP_TouchEventHandler_t)(BSP_TouchEvent_t event, BSP_ButtonId_t button, uint8_t data);
void BSP_InitQTouch();
void qTimer_overflow_cb(void);
#endif// _QTTOUCH_H
#endif //#if((QTOUCH_SUPPORT == 1) && ((BSP_SUPPORT == BOARD_SAMR21_ZLLEK) || (BSP_SUPPORT == BOARD_RFRINGQM) || (BSP_SUPPORT == BOARD_RFRINGQT)))
//eof qTouch.h
