/***************************************************************************//**
\file  leds.c

\brief The module to access to the leds.

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal
  History:
    07/04/14 karthik.p_u - Created
*******************************************************************************/

/******************************************************************************
                   Includes section
******************************************************************************/
#include <bspLeds.h>
#include <leds.h>
#include <pwm.h>
#include <appTimer.h>
#include <hs2xy.h>
#include <maxL.h>
#include <xyl2rgb.h>

#if BSP_SUPPORT != BOARD_FAKE

/******************************************************************************
                    Definitions section
******************************************************************************/
/* Maximum duty cycle (%) */
#define APP_MAX_DUTY_CYCLE 100

/* Minimum duty cycle (%) */
#define APP_MIN_DUTY_CYCLE 0

/* Macro for top counter value calculation */
#define GET_TOP(freq, prescale)   (F_PERI / (2 * freq * prescale))
/* Use prescale factor 1 */
#define PRESCALER  PWM_PRESCALER_1
/* Set top counter value using PWM frequency */
#define TOP GET_TOP(APP_PWM_FREQUENCY, PRESCALER)

#if (BSP_SUPPORT == BOARD_SAMR21_XPRO) || (BSP_SUPPORT == BOARD_SAMR21_ZLLEK)
/******************************************************************************
                   Prototypes section
******************************************************************************/
#if defined(APP_USE_PWM)
static void initPwm(void);
void bspSetPwmChannelCompareValue(uint16_t r, uint16_t g, uint16_t b);
#endif // APP_USE_PWM

#if defined(APP_USE_PWM) &&  (BSP_SUPPORT == BOARD_SAMR21_ZLLEK)
static void ledsSetColor(void);
#endif
/******************************************************************************
                    Local variables
******************************************************************************/
#if defined(APP_USE_PWM) &&  (BSP_SUPPORT == BOARD_SAMR21_ZLLEK)
static uint8_t currentL;
static uint16_t currentX, currentY;
#endif /* APP_USE_PWM */

/******************************************************************************
                    Local variables
******************************************************************************/
#if defined(APP_USE_PWM)
static HAL_PwmDescriptor_t pwmChannel0;
#endif

/******************************************************************************
                   Implementations section
******************************************************************************/

/**************************************************************************//**
\brief Opens leds module to use.

\return
    operation state
******************************************************************************/
void BSP_InitLeds(void)
{
  halInitFirstLed();

#if defined(APP_USE_PWM)
  initPwm();
#endif // APP_USE_PWM

  BSP_OffLed(LED_FIRST);
}

/**************************************************************************//**
\brief Opens leds module to use.

\return
    operation state
******************************************************************************/
result_t BSP_OpenLeds(void)
{
  BSP_InitLeds();
  return BC_SUCCESS;
}

/****************************************************************
Closes leds module.
****************************************************************/
result_t BSP_CloseLeds(void)
{
  halUnInitFirstLed();
  return BC_SUCCESS;
}

/****************************************************************
Turn on LED.
Parameters:
    id - number of led
****************************************************************/
void BSP_OnLed(uint8_t id)
{
  if (LED_FIRST == id)
    halOnFirstLed();
}

/****************************************************************
Turn off LED.
Parameters:
      id - number of led
****************************************************************/
void BSP_OffLed(uint8_t id)
{
  if (LED_FIRST == id)
    halOffFirstLed();
}

/****************************************************************
Change LED state to opposite.
    Parameters:
      id - number of led
****************************************************************/
void BSP_ToggleLed(uint8_t id)
{
  if (LED_FIRST == id)
    halToggleFirstLed();
}

/**************************************************************************//**
\brief Set brightness level
\param[in] level - new brightness level
******************************************************************************/
void BSP_SetLedBrightnessLevel(uint8_t level)
{
#if defined(APP_USE_PWM)
#if (BSP_SUPPORT == BOARD_SAMR21_XPRO)
  {
    uint16_t cmpValue = (TOP * level) / 255;
    pwmChannel0.channelMask = (1 << PWM_CHANNEL_3);
    HAL_SetPwmCompareValue(&pwmChannel0, cmpValue);
  }
#else
   currentL = level;
   ledsSetColor();
#endif
#else
  if (level)
    BSP_OnLed(LED_FIRST);
  else
    BSP_OffLed(LED_FIRST);
#endif /* APP_USE_PWM */
  (void)level;
}
#if defined(APP_USE_PWM)
/**************************************************************************//**
\brief Supported PWM channels initialization.
******************************************************************************/
static void initPwm(void)
{
#if BSP_SUPPORT == BOARD_SAMR21_XPRO
  /* Descriptors initialization */
  pwmChannel0.unit = PWM_UNIT_0;
  pwmChannel0.channelMask = (1 << PWM_CHANNEL_3);
  pwmChannel0.polarityMask = (PWM_POLARITY_INVERTED << PWM_CHANNEL_3);
  pwmChannel0.channelPinCfg[PWM_CHANNEL_3].portNum = PORT_A;
  pwmChannel0.channelPinCfg[PWM_CHANNEL_3].pinNum =  19;
  pwmChannel0.channelPinCfg[PWM_CHANNEL_3].functionConfig = 5;
#elif BSP_SUPPORT == BOARD_SAMR21_ZLLEK
  /* Descriptors initialization */
  pwmChannel0.unit = PWM_UNIT_0;
  pwmChannel0.channelMask = (1 << PWM_CHANNEL_0) | (1 << PWM_CHANNEL_1) | (1 << PWM_CHANNEL_2);
  pwmChannel0.polarityMask = (PWM_POLARITY_NON_INVERTED << PWM_CHANNEL_0) | (PWM_POLARITY_NON_INVERTED << PWM_CHANNEL_1) | (PWM_POLARITY_NON_INVERTED << PWM_CHANNEL_2);
  pwmChannel0.channelPinCfg[PWM_CHANNEL_0].portNum = PORT_A;
  pwmChannel0.channelPinCfg[PWM_CHANNEL_0].pinNum =  16;
  pwmChannel0.channelPinCfg[PWM_CHANNEL_0].functionConfig = 5;  
  pwmChannel0.channelPinCfg[PWM_CHANNEL_1].portNum = PORT_A;
  pwmChannel0.channelPinCfg[PWM_CHANNEL_1].pinNum =  17;
  pwmChannel0.channelPinCfg[PWM_CHANNEL_1].functionConfig = 5;
  pwmChannel0.channelPinCfg[PWM_CHANNEL_2].portNum = PORT_A;
  pwmChannel0.channelPinCfg[PWM_CHANNEL_2].pinNum =  18;
  pwmChannel0.channelPinCfg[PWM_CHANNEL_2].functionConfig = 5;
  pwmChannel0.ditheringMask = (ENABLE_DITHERING_A << PWM_CHANNEL_0) | (ENABLE_DITHERING_B << PWM_CHANNEL_1) | (ENABLE_DITHERING_C << PWM_CHANNEL_2);
  if(pwmChannel0.ditheringMask == 0x00)
     pwmChannel0.dithResoultion = NO_DITH;
  else
     pwmChannel0.dithResoultion = DITH6;
  pwmChannel0.dithercycles = 48;
#endif

  /* Prepare required PWM channels */
  HAL_OpenPwm(PWM_UNIT_0);
  /* Set PWM frequency for required module */
  HAL_SetPwmFrequency(&pwmChannel0, TOP, PRESCALER);

  /* Start PWM on supported channels */
  HAL_StartPwm(&pwmChannel0);
}
#endif
/**************************************************************************//**
\brief Set the color using Hue and Saturation as parameters
\param[in] level - a new brightness level
******************************************************************************/
void BSP_SetLedColorHS(uint16_t hue, uint8_t saturation)
{
#if defined(APP_USE_PWM) &&  (BSP_SUPPORT == BOARD_SAMR21_ZLLEK)
  unsigned int x, y;

  HS2XY(hue >> 8, saturation, &x, &y);
  currentX = x;
  currentY = y;
  ledsSetColor();
#else
  (void)hue, (void)saturation;
#endif
}

/**************************************************************************//**
\brief Set the color using X and Y as parameters
\param[in] level - a new brightness level
******************************************************************************/
void BSP_SetLedColorXY(uint16_t x, uint16_t y)
{
#if defined(APP_USE_PWM) &&  (BSP_SUPPORT == BOARD_SAMR21_ZLLEK)
  currentX = x;
  currentY = y;
  ledsSetColor();
#else
  (void)x, (void)y;
#endif
}

#if defined(APP_USE_PWM)
/**************************************************************************//**
\brief Set compare value for PWM channels
\param[in] ch1, ch2, ch3 - compare value
******************************************************************************/
void bspSetPwmChannelCompareValue(uint16_t r, uint16_t g, uint16_t b)
{
  pwmChannel0.channelMask = (1 << PWM_CHANNEL_2);
  HAL_SetPwmCompareValue(&pwmChannel0, b);

  pwmChannel0.channelMask = (1 << PWM_CHANNEL_1);
  HAL_SetPwmCompareValue(&pwmChannel0, g);

  pwmChannel0.channelMask = (1 << PWM_CHANNEL_0);
  HAL_SetPwmCompareValue(&pwmChannel0, r);
}
#endif

/**************************************************************************//**
\brief Set PWM values for current X, Y and L values
******************************************************************************/
#if defined(APP_USE_PWM) &&  (BSP_SUPPORT == BOARD_SAMR21_ZLLEK)
static void ledsSetColor(void)
{
#ifdef APP_USE_PWM
  int maxL;
  unsigned int r, g, b;

  maxL_FromXY(currentX, currentY, &maxL);
  XYL2RGB(currentX, currentY, currentL, maxL, &r, &g, &b);

  r = r * TOP / 65535;
  g = g * TOP / 65535;
  b = b * TOP / 65535;
  bspSetPwmChannelCompareValue(r, g, b);
#endif
}

#endif /* APP_USE_PWM */

#else /* (BSP_SUPPORT == BOARD_SAMR21B18_MZ210PA_MODULE) || (BSP_SUPPORT == BOARD_SAMR21_CUSTOM) || \
         (BSP_SUPPORT == BOARD_SAMR21G18_MR210UA_MODULE) */

/**************************************************************************//**
\brief Opens leds module to use.

\return
    operation state
******************************************************************************/
result_t BSP_OpenLeds(void)
{
  return BC_SUCCESS;
}

/**************************************************************************//**
\brief Closes leds module.

\return
    operation state
******************************************************************************/
result_t BSP_CloseLeds(void)
{
  return BC_SUCCESS;
}

/**************************************************************************//**
\brief Turns on the LED.

\param[in]
    id - number of led
******************************************************************************/
void BSP_OnLed(uint8_t id)
{
  (void)id;
}

/**************************************************************************//**
\brief Turns off the LED.

\param[in]
      id - number of led
******************************************************************************/
void BSP_OffLed(uint8_t id)
{
  (void)id;
}

/**************************************************************************//**
\brief Changes the LED state to opposite.

\param[in]
      id - number of led
******************************************************************************/
void BSP_ToggleLed(uint8_t id)
{
  (void)id;
}

/**************************************************************************//**
\brief Initialize LEDs.
******************************************************************************/
void BSP_InitLeds(void)
{
}

/**************************************************************************//**
\brief Set brightness level
\param[in] level - new brightness level
******************************************************************************/
void BSP_SetLedBrightnessLevel(uint8_t level)
{
  (void)level;
}

/**************************************************************************//**
\brief Set the color using Hue and Saturation as parameters
\param[in] level - a new brightness level
******************************************************************************/
void BSP_SetLedColorHS(uint16_t hue, uint8_t saturation)
{
  (void)hue;
  (void)saturation;
}

/**************************************************************************//**
\brief Set the color using X and Y as parameters
\param[in] level - a new brightness level
******************************************************************************/
void BSP_SetLedColorXY(uint16_t x, uint16_t y)
{
  (void)x;
  (void)y;
}

#endif /* (BSP_SUPPORT == BOARD_SAMR21_XPRO) || (BSP_SUPPORT == BOARD_SAMR21_ZLLEK) */
#endif // BSP_SUPPORT != BOARD_FAKE
// eof leds.c
