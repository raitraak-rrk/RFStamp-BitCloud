/***************************************************************************//**
  \file  joystick.h

  \brief The header file describes the joystick interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    10/09/07 A. Khromykh - Created
*******************************************************************************/

#ifndef _BSPJOYSTICK_H
#define _BSPJOYSTICK_H

/******************************************************************************
                   Includes section
******************************************************************************/
// \cond
#include <sysTypes.h>
// \endcond

/******************************************************************************
                   Types section
******************************************************************************/
/*! \brief joystick states */
typedef enum
{
  JOYSTICK_NONE,
  JOYSTICK_LEFT,
  JOYSTICK_RIGHT,
  JOYSTICK_UP,
  JOYSTICK_DOWN,
  JOYSTICK_PUSH
} BSP_JoystickState_t;

/******************************************************************************
                   Prototypes section
******************************************************************************/
/**************************************************************************//**
\brief Opens component and registers handler for joystick events.
\param[in]
    generalHandler - the joystick handler; can be set to NULL
\param[in]
    state - joystick state 
\return
  BC_FAIL - joystick module is busy, \n
  BC_SUCCESS - joystick module is free.
******************************************************************************/
result_t BSP_OpenJoystick(void (*generalHandler)(BSP_JoystickState_t state));

/**************************************************************************//**
\brief closes joystick component.

\return
  BC_FAIL - joystick module was not opened, \n
  BC_SUCCESS - joystick module was closed.
******************************************************************************/
result_t BSP_CloseJoystick(void);

/**************************************************************************//**
\brief Reads state of joystick.

\return
    Joystick state.
******************************************************************************/
BSP_JoystickState_t BSP_ReadJoystickState(void);

#endif /* _BSPJOYSTICK_H */
//eof joystick.h
