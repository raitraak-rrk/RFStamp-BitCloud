/***************************************************************************//**
  \file  buttons.h

  \brief The header file describes the buttons interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    29/05/07 E. Ivanov - Created
*******************************************************************************/

#ifndef _BSPBUTTONS_H
#define _BSPBUTTONS_H

/******************************************************************************
                   Includes section
******************************************************************************/
// \cond
#include <sysTypes.h>
// \endcond

/******************************************************************************
                   Define(s) section
******************************************************************************/
// \cond
#define BSP_KEY0 ((uint8_t)1 << 0)
#define BSP_KEY1 ((uint8_t)1 << 1)
#define BSP_KEY2 ((uint8_t)1 << 2)
#define BSP_KEY3 ((uint8_t)1 << 3)
#define BSP_KEY4 ((uint8_t)1 << 4)
#define BSP_KEY5 ((uint8_t)1 << 5)
#define BSP_KEY6 ((uint8_t)1 << 6)
#define BSP_KEY7 ((uint8_t)1 << 7)
// for avr32_evk1105
#define BSP_UP     0
#define BSP_DOWN   1
#define BSP_RIGHT  2
#define BSP_LEFT   3
#define BSP_ENTER  4
// for samR21 ZLL-EK
#define BSP_FN_KEY  0
// \endcond

/******************************************************************************
                   Types section
******************************************************************************/

// \cond
typedef void (*BSP_ButtonsEventFunc_t)(uint8_t);
// \endcond

/******************************************************************************
                   Prototypes section
******************************************************************************/
/**************************************************************************//**
\brief Registers handlers for button events.
\param[in]
    pressed - the handler to process pressing of the button; can be set to NULL \n
    bn - button number.
\param[in]
    released - the handler to process releasing of the button; can be set to NULL \n
    bn - button number.
\return
  BC_FAIL - buttons module is busy, \n
  BC_SUCCESS - otherwise.
******************************************************************************/
result_t BSP_OpenButtons(void (*pressed)(uint8_t bn), void (*released)(uint8_t bn));

/**************************************************************************//**
\brief Cancel buttons handlers.
\return
  BC_FAIL - buttons module was not opened, \n
  BC_SUCCESS - otherwise.
******************************************************************************/
result_t BSP_CloseButtons(void);

/**************************************************************************//**
\brief Reads state of the buttons.
\return
    Current buttons state in a binary way. \n
    Bit 0 defines the state of button 1, \n
    bit 1 defines the state of button 2 (for meshbean). \n
    Bit 0 defines the state of up, \n
    bit 1 defines the state of down, \n
    bit 2 defines the state of right, \n
    bit 3 defines the state of left, \n
    bit 4 defines the state of enter(for avr32_evk1105).
******************************************************************************/
uint8_t BSP_ReadButtonsState(void);

#endif /* _BSPBUTTONS_H */
//eof buttons.h
