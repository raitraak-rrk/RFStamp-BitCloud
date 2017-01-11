/**************************************************************************//**
\file buttons.c

\brief Implementation of buttons interface.

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal
  History:
    07/04/14 karthik.p_u - Created
*******************************************************************************/
#if (BSP_SUPPORT != BOARD_FAKE)

/******************************************************************************
                   Includes section
******************************************************************************/
#include <sysTypes.h>
#include <buttons.h>
#include <irq.h>
#include <bspTaskManager.h>
#include <gpio.h>
#include <bspDbg.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
#define PRESSED              1
#define RELEASED             0
#define BSP_BUTTONS_IDLE     0
#define BSP_BUTTONS_BUSY     1
#define ACT_ON               1
#define ACT_OFF              0

#if BSP_SUPPORT == BOARD_SAMR21_ZLLEK
#define BSP_readKEY0() (GPIO_B23_read())
#elif BSP_SUPPORT == BOARD_SAMR21_XPRO
#define BSP_readKEY0() (GPIO_A28_read())
#elif BSP_SUPPORT == BOARD_RFSTRIP
#define BSP_readKEY0() (GPIO_A27_read())
#endif

#if BSP_SUPPORT == BOARD_SAMR21_ZLLEK
#define BUTTON_IRQ IRQ_EIC_EXTINT7
#elif BSP_SUPPORT == BOARD_SAMR21_XPRO
#define BUTTON_IRQ IRQ_EIC_EXTINT8
#elif BSP_SUPPORT == BOARD_RFSTRIP
#define BUTTON_IRQ IRQ_EIC_EXTINT15
#endif
/******************************************************************************
                   Types section
******************************************************************************/
typedef struct _BSP_ButtonsAction_t
{
  uint8_t wasPressed   : 1;
  uint8_t waitReleased  : 1;
  uint8_t currentState : 1;
} BSP_ButtonsAction_t;

typedef struct _BSP_ButtonsDescriptor_t
{
  /* Pin number concerned with button */
  uint8_t pinNumber;
  /* Button action map */
  volatile BSP_ButtonsAction_t action;
  /* Pin pullup processing function */
  void (*enablePinPullup)(void);
  /* Read pin processing function */
  uint8_t (*readPin)(void);
  /* Busy flag*/
  bool busy;
} BSP_ButtonDescriptor_t;

#if (BSP_SUPPORT == BOARD_SAMR21_XPRO) || (BSP_SUPPORT == BOARD_SAMR21_ZLLEK) || (BSP_SUPPORT == BOARD_RFSTRIP)
/******************************************************************************
                   Prototypes section
******************************************************************************/
/**************************************************************************//**
\brief  HAL's event handlers about any key has changed state.
******************************************************************************/
static void bspKeyInterruptHandler(void);

/******************************************************************************
                   Global variables section
******************************************************************************/
static uint8_t state = BSP_BUTTONS_IDLE;
static volatile BSP_ButtonsAction_t buttonsAction;
static BSP_ButtonsEventFunc_t bspButtonPressHandle;   // callback
static BSP_ButtonsEventFunc_t bspButtonReleaseHandle; // callback


/******************************************************************************
                   Implementations section
******************************************************************************/

/**************************************************************************//**
\brief Initializes buttons module.
******************************************************************************/
static void bspInitButtons(void)
{
  HAL_IrqMode_t HAL_IrqMode = IRQ_FALLING_EDGE;

#if BSP_SUPPORT == BOARD_SAMR21_ZLLEK
  GPIO_B23_make_in();
  GPIO_B23_make_pullup();
  PORTB_PINCFG23_s.pmuxen = 1;
  PORTB_PINCFG23_s.inen = 1;
  PORTB_PMUX12_s.pmuxo = 0;
#elif BSP_SUPPORT == BOARD_SAMR21_XPRO
  GPIO_A28_make_in();
  GPIO_A28_make_pullup();
  PORTA_PINCFG28_s.pmuxen = 1;
  PORTA_PINCFG28_s.inen = 1;
  PORTA_PMUX14_s.pmuxe = 0;
#elif BSP_SUPPORT == BOARD_RFSTRIP
  GPIO_A27_make_in();
  GPIO_A27_make_pullup();
  PORTA_PINCFG27_s.pmuxen = 1;
  PORTA_PINCFG27_s.inen = 1;
  PORTA_PMUX13_s.pmuxo = 0;
#else
#error 'Unsupported board.'
#endif
  HAL_RegisterIrq(BUTTON_IRQ, HAL_IrqMode, bspKeyInterruptHandler);

  if (BSP_readKEY0())
    buttonsAction.currentState = RELEASED;
  else
    buttonsAction.currentState = PRESSED;

}

/**************************************************************************//**
\brief Registers handlers for button events.

\param[in]
    pressed - the handler to process pressing the button
\param[in]
    released - the handler to process releasing the button
\param[in]
    bn - button number.

\return
  BC_SUCCESS - always.
******************************************************************************/
result_t BSP_OpenButtons(void (*pressed)(uint8_t bn), void (*released)(uint8_t bn))
{
  if (state != BSP_BUTTONS_IDLE)
    return BC_FAIL;
  state = BSP_BUTTONS_BUSY;
  bspButtonPressHandle = pressed;
  bspButtonReleaseHandle = released;
  bspInitButtons();
  return BC_SUCCESS;
}

/**************************************************************************//**
\brief Cancel buttons handlers.

\return
  BC_SUCCESS - always.
******************************************************************************/
result_t BSP_CloseButtons(void)
{
  if (state != BSP_BUTTONS_BUSY)
    return BC_FAIL;
  HAL_UnregisterIrq(BUTTON_IRQ);
  bspButtonPressHandle = NULL;
  bspButtonReleaseHandle = NULL;
  state = BSP_BUTTONS_IDLE;
  return BC_SUCCESS;
}

/**************************************************************************//**
\brief Reads state of buttons.

\return
    Current buttons state in a binary way. \n
    Bit 0 defines state of the button 1, \n
    bit 1 defines state of the button 2.
******************************************************************************/
uint8_t BSP_ReadButtonsState(void)
{
  return buttonsAction.currentState;
}

/**************************************************************************//**
\brief  HAL's event about KEY has changed state.
******************************************************************************/
static void bspKeyInterruptHandler(void)
{
  HAL_DisableIrq(BUTTON_IRQ);
  buttonsAction.currentState = PRESSED;
  buttonsAction.wasPressed = 1;
  bspPostTask(BSP_BUTTONS);
}

/**************************************************************************//**
\brief  BSP's event about KEY has changed state.
******************************************************************************/
void bspButtonsHandler(void)
{
  if (buttonsAction.wasPressed)
  {
    buttonsAction.wasPressed = 0;
    buttonsAction.waitReleased = 1;
    if (NULL != bspButtonPressHandle)
      bspButtonPressHandle(BSP_KEY0);
  }

  if (buttonsAction.waitReleased)
  {
    if (BSP_readKEY0())
    {
      buttonsAction.waitReleased = 0;
      buttonsAction.currentState = RELEASED;
      if (NULL != bspButtonReleaseHandle)
        bspButtonReleaseHandle(BSP_KEY0);
      HAL_EnableIrq(BUTTON_IRQ);
    }
    else
    {
      bspPostTask(BSP_BUTTONS);
    }
  }

}

#else /* (BSP_SUPPORT == BOARD_SAMR21B18_MZ210PA_MODULE) || (BSP_SUPPORT == BOARD_SAMR21_CUSTOM) || \
         (BSP_SUPPORT == BOARD_SAMR21G18_MR210UA_MODULE) */
/**************************************************************************//**
\brief Registers handlers for button events.

\param[in]
    pressed - the handler to process pressing the button
\param[in]
    released - the handler to process releasing the button
\param[in]
    bn - button number.

\return
  BC_SUCCESS - always.
******************************************************************************/
result_t BSP_OpenButtons(void (*pressed)(uint8_t bn), void (*released)(uint8_t bn))
{
  (void)pressed;
  (void)released;

  return BC_SUCCESS;
}

/**************************************************************************//**
\brief Cancel buttons handlers.

\return
  BC_SUCCESS - always.
******************************************************************************/
result_t BSP_CloseButtons(void)
{
  return BC_SUCCESS;
}

/**************************************************************************//**
\brief Reads state of buttons.

\return
    Current buttons state in a binary way. \n
    Bit 0 defines state of the button 1, \n
    bit 1 defines state of the button 2.
******************************************************************************/
uint8_t BSP_ReadButtonsState(void)
{
  return 0;
}

/**************************************************************************//**
\brief  BSP's event about KEY has changed state.
******************************************************************************/
void bspButtonsHandler(void)
{
}
#endif // (BSP_SUPPORT == BOARD_SAMR21_XPRO) || (BSP_SUPPORT == BOARD_SAMR21_ZLLEK)
#endif // BSP_SUPPORT != BOARD_FAKE
// end of buttons.c
