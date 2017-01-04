/**************************************************************************//**
\file  joystick.c

\brief Implementation of joystick interface.

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal
  History:
    06/06/14 P. Mahendran - Created
*******************************************************************************/
#if BSP_SUPPORT != BOARD_FAKE

/******************************************************************************
                   Includes section
******************************************************************************/
#include <joystick.h>
#include <bspTaskManager.h>
#include <appTimer.h>
#include <stddef.h>
#include <bspDbg.h>
#include <adc.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
#define MAJ_SCANN_TIME  330
#define IS_JOYSTICK_PUSH_ACTION(x) (x > 25 && x < 35)
#define IS_JOYSTICK_UP_ACTION(x) (x > 85 && x < 95)
#define IS_JOYSTICK_LEFT_ACTION(x) (x > 120 && x < 130)
#define IS_JOYSTICK_DOWN_ACTION(x) (x > 220 && x < 230)
#define IS_JOYSTICK_RIGHT_ACTION(x) (x > 200 && x < 215)

/******************************************************************************
                   Types section
******************************************************************************/
typedef enum
{
  BSP_JOYSTICK_IDLE = 0,
  BSP_JOYSTICK_BUSY
} BSP_JoystickModulState_t;

// callback joystick functions
typedef void (*BSP_JoystickEventFunc_t)(BSP_JoystickState_t state);

/******************************************************************************
                   Prototypes section
******************************************************************************/
static void joystickHandler(void);
extern void bspJoystickHandler(void);
/******************************************************************************
                   Global variables section
******************************************************************************/
static BSP_JoystickModulState_t state = BSP_JOYSTICK_IDLE;
static volatile BSP_JoystickState_t joystickReal;
static BSP_JoystickEventFunc_t bspJoystickHandle = NULL;
static HAL_AppTimer_t glitchTimer;
static HAL_AdcDescriptor_t joyStick_adcParam;
static uint16_t joyStickData;
/******************************************************************************
                   Implementations section
******************************************************************************/

/******************************************************************
port IO (joystick) interrupt handler
******************************************************************/
static void joystickHandler(void)
{
  if (IS_JOYSTICK_UP_ACTION(joyStickData))
  {
      joystickReal = JOYSTICK_UP;
  }
  else if (IS_JOYSTICK_DOWN_ACTION(joyStickData))
  {
      joystickReal = JOYSTICK_DOWN;
  }
  else if (IS_JOYSTICK_LEFT_ACTION(joyStickData))
  {
      joystickReal = JOYSTICK_LEFT;
  }
  else if (IS_JOYSTICK_RIGHT_ACTION(joyStickData))
  {
      joystickReal = JOYSTICK_RIGHT;
  }
  else if (IS_JOYSTICK_PUSH_ACTION(joyStickData))
  {
      joystickReal = JOYSTICK_PUSH;
  }
  else
  {
    joystickReal = JOYSTICK_NONE;
  }
  bspJoystickHandler();
}

/******************************************************************************
 Initializes joystick module.
******************************************************************************/
void bspInitJoystick(void)
{
  /* Configure ADC Pin as Input */
  PORTA_DIRCLR = 1 << 6;
  PORTA_PINCFG3 |= PORTA_PINCFG3_PMUXEN;
  PORTA_PMUX3 |= PORTA_PMUX3_PMUXE(1);

  joyStick_adcParam.bufferPointer = &joyStickData;
  joyStick_adcParam.callback = joystickHandler;
  joyStick_adcParam.resolution = RESOLUTION_8_BIT;
  joyStick_adcParam.sampleRate = ADC_350KSPS;
  joyStick_adcParam.selectionsAmount = 1;
  joyStick_adcParam.voltageReference = INTVCC0;

  HAL_OpenAdc(&joyStick_adcParam);
}

/******************************************************************************
Registers handlers for joystick events.
Paremeters:
  generalHandler - the joystick handler
  state- joystick state
Returns:
  BC_FAIL - joystick module is busy,
  BC_SUCCESS in other cases.
******************************************************************************/
result_t BSP_OpenJoystick(void (*generalHandler)(BSP_JoystickState_t state))
{
  if(state != BSP_JOYSTICK_IDLE)
    return BC_FAIL;
  state = BSP_JOYSTICK_BUSY;
  bspJoystickHandle = generalHandler;
  bspInitJoystick();
  bspPostTask(BSP_JOYSTICK);
  return BC_SUCCESS;
};

/******************************************************************************
Remove joystick handlers.
Parameters:
  none.
Returns:
  BC_FAIL - joystick module was not opened,
  BC_SUCCESS in other cases.
******************************************************************************/
result_t BSP_CloseJoystick(void)
{
  if(state != BSP_JOYSTICK_BUSY)
    return BC_FAIL;
  bspJoystickHandle = NULL;
  state = BSP_JOYSTICK_IDLE;
  return BC_SUCCESS;
};

/******************************************************************************
Reads state of joystick.
Parameters:
  none.
Returns:
    Current joystick state.
******************************************************************************/
BSP_JoystickState_t BSP_ReadJoystickState(void)
{
  /* read joystick state */
  return joystickReal;
}

/******************************************************************
enable joystick interrupt
******************************************************************/
void bspEnableJoystickAdcRead(void)
{
  HAL_ReadAdc(&joyStick_adcParam, HAL_ADC_CHANNEL6);
}

/******************************************************************************
BSP's event about joystick has changed state.
******************************************************************************/
void bspJoystickHandler(void)
{
  glitchTimer.interval = MAJ_SCANN_TIME;
  glitchTimer.mode = TIMER_ONE_SHOT_MODE;
  glitchTimer.callback = bspEnableJoystickAdcRead;
  HAL_StartAppTimer(&glitchTimer);

  if (bspJoystickHandle)
    bspJoystickHandle(joystickReal);
}

#endif // BSP_SUPPORT != BOARD_FAKE

// eof joystick.c
