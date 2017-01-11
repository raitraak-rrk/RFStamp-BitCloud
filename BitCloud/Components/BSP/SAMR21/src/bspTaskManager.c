/**************************************************************************//**
\file  bspTaskManager.c

\brief Implemenattion of BSP task manager.

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal
  History:
    07/04/14 karthik.p_u - Created
******************************************************************************/

/******************************************************************************
                   Includes section
******************************************************************************/
#include <bspTaskManager.h>
#include <atomic.h>

/******************************************************************************
                   Prototypes section
******************************************************************************/
/**************************************************************************//**
\brief BSP button handler.
******************************************************************************/
void bspButtonsHandler(void);

/**************************************************************************//**
\brief BSP temperature sensor handler.
******************************************************************************/
void bspTemperatureSensorHandler(void);

/**************************************************************************//**
\brief BSP light sensor handler.
******************************************************************************/
void bspLightSensorHandler(void);

/**************************************************************************//**
\brief BSP Joystick handler.
******************************************************************************/
void bspJoystickHandler(void);

/**************************************************************************//**
\brief BSP battery handler.
******************************************************************************/
void bspEmptyBatteryHandler(void);

/**************************************************************************//**
\brief BSP Touch handler.
******************************************************************************/
#if((QTOUCH_SUPPORT == 1) && ((BSP_SUPPORT == BOARD_SAMR21_ZLLEK) ||\
    (BSP_SUPPORT == BOARD_RFRINGQM) || (BSP_SUPPORT == BOARD_RFRINGQT)))
void bspTouchHandler(void);
#endif

/**************************************************************************//**
\brief BSP RGBLED handler.
******************************************************************************/
#if ((BSP_SUPPORT == BOARD_RFRINGQT) || (BSP_SUPPORT == BOARD_RFRINGQM))
void bspRGBLEDHandler(void);
#endif

/******************************************************************************
                   Global variables section
******************************************************************************/
volatile uint8_t bspTaskFlags = 0;

/******************************************************************************
                   Implementations section
******************************************************************************/
/**************************************************************************//**
\brief BSP task handler.
******************************************************************************/
void BSP_TaskHandler(void)
{
  if (bspTaskFlags & BSP_JOYSTICK)
  {
    bspTaskFlags &= ~BSP_JOYSTICK;
    bspJoystickHandler();
  }

  if (bspTaskFlags & BSP_BUTTONS)
  {
    bspTaskFlags &= (~BSP_BUTTONS);
    bspButtonsHandler();
  }

  if (bspTaskFlags & BSP_TEMPERATURE)
  {
    bspTaskFlags &= (~BSP_TEMPERATURE);
    bspTemperatureSensorHandler();
  }

  if (bspTaskFlags & BSP_LIGHT)
  {
    bspTaskFlags &= (~BSP_LIGHT);
    bspLightSensorHandler();
  }

  if (bspTaskFlags & BSP_BATTERY)
  {
    bspTaskFlags &= (~BSP_BATTERY);
    bspEmptyBatteryHandler();
  }

#if((QTOUCH_SUPPORT == 1) && ((BSP_SUPPORT == BOARD_SAMR21_ZLLEK) ||\
    (BSP_SUPPORT == BOARD_RFRINGQM) || (BSP_SUPPORT == BOARD_RFRINGQT)))
  if (bspTaskFlags & BSP_TOUCH)
  {
    bspTaskFlags &= (~BSP_TOUCH);
    bspTouchHandler();
  }
#endif

#if ((BSP_SUPPORT == BOARD_RFRINGQT) || (BSP_SUPPORT == BOARD_RFRINGQM))
if (bspTaskFlags & BSP_RGBLED)
{
    bspTaskFlags &= (~BSP_RGBLED);
    bspRGBLEDHandler();
}
#endif

  if (bspTaskFlags)
  {
    SYS_PostTask(BSP_TASK_ID);
  }
}

// eof bspTaskManager.c
