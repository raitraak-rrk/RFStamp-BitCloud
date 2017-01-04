/**************************************************************************//**
  \file  halPwm.c

  \brief Implementation of hardware depended PWM interface.

  \author
      Atmel Corporation: http://www.atmel.com \n
      Support email: avr@atmel.com

    Copyright (c) 2008-2012, Atmel Corporation. All rights reserved.
    Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    07/04/14 karthik.p_u - Created
 ******************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/
#if defined(HAL_USE_PWM)
/******************************************************************************
                   Includes section
******************************************************************************/
#include <sysTypes.h>
#include <halPwm.h>
#include <atomic.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
#define SYNCBUSY_CHANNEL SYNCBUSY_CC2

#define SYNCBUSY_SWRST    (1 << 0)
#define SYNCBUSY_ENABLE   (1 << 1)
#define SYNCBUSY_CTRLB    (1 << 2)
#define SYNCBUSY_STATUS   (1 << 3)
#define SYNCBUSY_COUNT    (1 << 4)
#define SYNCBUSY_WAVE     (1 << 6)
#define SYNCBUSY_PER      (1 << 7)
#define SYNCBUSY_CC0      (1 << 8)
#define SYNCBUSY_CC1      (1 << 9)
#define SYNCBUSY_CC2      (1 << 10)
#define SYNCBUSY_CC3      (1 << 11)
#define SYNCBUSY_CC4      (1 << 12)
#define SYNCBUSY_CC5      (1 << 13)
#define SYNCBUSY_CC6      (1 << 14)
#define SYNCBUSY_CC7      (1 << 15)

#define DITH4_MASK        (0x0F)
#define DITH5_MASK        (0x1F)
#define DITH6_MASK        (0x3F)

/* Dithering Compare value shift */
#define DITH4_CV_SHIFT (4)
#define DITH5_CV_SHIFT (5)
#define DITH6_CV_SHIFT (6)
/******************************************************************************
                    Static functions prototypes section
******************************************************************************/
static uint8_t halGetPwmUnitIndex(HAL_PwmUnit_t pwmUnit);
static void halEnablePwmPins(HAL_PwmDescriptor_t *descriptor, uint8_t channel);
static void halDisablePwmPins(HAL_PwmDescriptor_t *descriptor, uint8_t channel);

/******************************************************************************
                    Static variables sections
******************************************************************************/
static uint16_t pwmTopValue;

/******************************************************************************
                   Implementations section
******************************************************************************/

/**************************************************************************//**
\brief Initializes the PWM.

\param [in] pwmUnit - PWM unit number.
                      Equal to ID of Timer/Counter witch serves PWM module.
******************************************************************************/
void halOpenPwm(HAL_PwmUnit_t pwmUnit)
{
  uint8_t pwmUnitIndex = halGetPwmUnitIndex(pwmUnit);

  PM_APBCMASK |= (1 << (8 + pwmUnitIndex));
  if ((PWM_UNIT_0 == pwmUnit) || (PWM_UNIT_1 == pwmUnit))
  {
    GCLK_CLKCTRL_s.id = 0x1A;  // enabling clock for TCC0
    GCLK_CLKCTRL_s.gen = 3;
    GCLK_CLKCTRL_s.clken = 1;
  }
  // GCLK for PWM_UNIT_2 is same as TC3(used for appClock)
  // which is already enabled 

  // SWRST
  CTRLA(pwmUnit) |= 1;
  while (SYNCBUSY(pwmUnit) & SYNCBUSY_SWRST);

  /* Clear Timer/Counter */
  COUNT(pwmUnit) = 0x0000;
  while (SYNCBUSY(pwmUnit) & SYNCBUSY_COUNT);

  /* Clear PER(TOP vlaue) register */
  PER(pwmUnit) = 0x0000;
  while (SYNCBUSY(pwmUnit) & SYNCBUSY_PER);

  /* Configure PWM mode: phase and frequency correct, TOP = ICRn */
  WAVE(pwmUnit) |= DSBOTTOM;
  while (SYNCBUSY(pwmUnit) & SYNCBUSY_WAVE);
}
/**************************************************************************//**
\brief Starts PWM on specified channel.

\param [in] descriptor - PWM channel descriptor.
******************************************************************************/
void halStartPwm(HAL_PwmDescriptor_t *descriptor)
{
  uint8_t bitPos = 0;
  uint8_t channelMask = descriptor->channelMask;

  while (channelMask)
  {
    if (channelMask & 0x01)
    {
      halEnablePwmPins(descriptor, bitPos);

      WAVE(descriptor->unit) |= (1 << (POLARITY_BITFIELD_POS + bitPos));
      while (SYNCBUSY(descriptor->unit) & SYNCBUSY_WAVE);

      if (PWM_POLARITY_INVERTED == ((descriptor->polarityMask >> bitPos) & 0x01))
      {
        WAVE(descriptor->unit) &= ~(1 << (POLARITY_BITFIELD_POS + bitPos));
        while (SYNCBUSY(descriptor->unit) & SYNCBUSY_WAVE);
      }

     /* Enable the channel */
      CTRLA(descriptor->unit) |= (1 << (bitPos + CAPTEN_BITPOS));
      while (SYNCBUSY(descriptor->unit) & SYNCBUSY_CC0);
    }
    bitPos++;
    channelMask >>= 1;
  }

  /* Set Dithering Resoultion */
  CTRLA(descriptor->unit) |= (descriptor->dithResoultion << RESOULTION_BITPOS);
  while (SYNCBUSY(descriptor->unit) & SYNCBUSY_CC0);

  /* Enable the TCCx */
  CTRLA(descriptor->unit) |= (1 << 1);
  while (SYNCBUSY(descriptor->unit) & SYNCBUSY_ENABLE);  

}

/**************************************************************************//**
\brief Stops PWM on specified channel.

\param [in] descriptor - PWM channel descriptor.
******************************************************************************/
void halStopPwm(HAL_PwmDescriptor_t *descriptor)
{
  uint8_t bitPos = 0;
  uint8_t channelMask = descriptor->channelMask;

  while (channelMask)
  {
    if (channelMask & 0x01)
    {
      /* Clean compare register and stop Timer/Counter */
      CCx(descriptor->unit, bitPos) = 0x0000;

      /* Disable the channel */
      CTRLA(descriptor->unit) &= ~(1 << (bitPos + CAPTEN_BITPOS));
      while (SYNCBUSY(descriptor->unit) & SYNCBUSY_ENABLE);

      halDisablePwmPins(descriptor, bitPos);
    }

    bitPos++;
    channelMask >>= 1;
  }
}

/**************************************************************************//**
\brief Sets base frequency of module. Common for all module channels.

\param [in] descriptor - PWM channel descriptor.
\param [in] top - value for the TOP register.
\param [in] prescaler - clock prescaler.
******************************************************************************/
void halSetPwmFrequency(HAL_PwmDescriptor_t *descriptor, uint16_t top, HAL_PwmPrescaler_t prescaler)
{
  /* Set new TOP register (PER in our case) */
  switch (descriptor->dithResoultion)
  {
    case DITH4:
      PER(descriptor->unit) = (top << DITH4_CV_SHIFT) | (descriptor->dithercycles & DITH4_MASK);
      break;
    case DITH5:
      PER(descriptor->unit) = (top << DITH5_CV_SHIFT) | (descriptor->dithercycles & DITH5_MASK);
      break;
    case DITH6:
      PER(descriptor->unit) = (top << DITH6_CV_SHIFT) | (descriptor->dithercycles & DITH6_MASK);
      break;
    default:
      PER(descriptor->unit) = top;
      break;
  }
  while (SYNCBUSY(descriptor->unit) & SYNCBUSY_PER);  

  pwmTopValue = top;
  /* Initialize prescaler */
  CTRLA(descriptor->unit) |= ((prescaler-1) << 8); // Prescaler is starting from 1
}

/**************************************************************************//**
\brief Sets compare value for the PWM channel.

\param [in] descriptor - PWM channel descriptor.
******************************************************************************/
void halSetPwmCompareValue(HAL_PwmDescriptor_t *descriptor, uint16_t cmpValue)
{
  uint8_t bitPos = 0;
  uint8_t channelMask = descriptor->channelMask;

  while (channelMask)
  {
    if (channelMask & 0x01)
    {
      switch (descriptor->dithResoultion)
      {
        case DITH4:
          if ((descriptor->ditheringMask >> bitPos) & 0x01)
          {
            PER(descriptor->unit) = (pwmTopValue << DITH4_CV_SHIFT) | (descriptor->dithercycles & DITH4_MASK);
            if(cmpValue)
              CCx(descriptor->unit, bitPos) = ((cmpValue << DITH4_CV_SHIFT) | (descriptor->dithercycles & DITH4_MASK));
            else
              CCx(descriptor->unit, bitPos) = cmpValue;   
          }
          else
          {
            PER(descriptor->unit) = pwmTopValue;
            CCx(descriptor->unit, bitPos) = cmpValue;          
          }
          break;
        case DITH5:
          if ((descriptor->ditheringMask >> bitPos) & 0x01)
          {
            PER(descriptor->unit) = (pwmTopValue << DITH5_CV_SHIFT) | (descriptor->dithercycles & DITH5_MASK);
            if(cmpValue)
              CCx(descriptor->unit, bitPos) = ((cmpValue << DITH5_CV_SHIFT) | (descriptor->dithercycles & DITH5_MASK));
            else
              CCx(descriptor->unit, bitPos) = cmpValue;   
          }
          else
          {
            PER(descriptor->unit) = pwmTopValue;
            CCx(descriptor->unit, bitPos) = cmpValue;          
          }
          break;
        case DITH6:
          if ((descriptor->ditheringMask >> bitPos) & 0x01)
          {
            PER(descriptor->unit) = (pwmTopValue << DITH6_CV_SHIFT) | (descriptor->dithercycles & DITH6_MASK);
            if(cmpValue)
              CCx(descriptor->unit, bitPos) = ((cmpValue << DITH6_CV_SHIFT) | (descriptor->dithercycles & DITH6_MASK));
            else
              CCx(descriptor->unit, bitPos) = cmpValue;   
          }
          else
          {
            PER(descriptor->unit) = pwmTopValue;
            CCx(descriptor->unit, bitPos) = cmpValue;          
          }
          break;
        default:
          PER(descriptor->unit) = pwmTopValue;
          CCx(descriptor->unit, bitPos) = cmpValue;
          break;
      }
      while (SYNCBUSY(descriptor->unit) & (1 << (SYNBUSY_CHANNEL_BITPOS + bitPos)));
    }
    bitPos++;
    channelMask >>= 1;
  }
}

/**************************************************************************//**
\brief Closes the PWM.

\param [in] pwmUnit - PWM unit number.
                      Equal to ID of Timer/Counter witch serves PWM module.
******************************************************************************/
void halClosePwm(HAL_PwmUnit_t pwmUnit)
{
  uint8_t pwmUnitIndex = halGetPwmUnitIndex(pwmUnit);
  /* Disable the TCCx */
  CTRLA(pwmUnit) &= ~(1 << 1);
  while (SYNCBUSY(pwmUnit) & SYNCBUSY_ENABLE);

  PM_APBCMASK &= ~(1 << (8 + pwmUnitIndex));

}

/**************************************************************************//**
\brief Get the index corresponding to the pwm Unit..

\param [in] pwmUnit - PWM unit number.
                      Equal to ID of Timer/Counter witch serves PWM module.
******************************************************************************/
static uint8_t halGetPwmUnitIndex(HAL_PwmUnit_t pwmUnit)
{
  uint8_t index = 0; // default value is for unit0

  if (PWM_UNIT_1 == pwmUnit)
    index = 1;
  else if (PWM_UNIT_2 == pwmUnit)
    index = 2;

  return index;
}

/**************************************************************************//**
\brief enable pwm pins

\param [in] descriptor - pwm descriptor.
\param [in] channel - channel belongs to the descriptor
******************************************************************************/
static void halEnablePwmPins(HAL_PwmDescriptor_t *descriptor, uint8_t channel)
{
  GPIO_make_out(&descriptor->channelPinCfg[channel]);
  GPIO_pinfunc_config(&descriptor->channelPinCfg[channel]);
}

/**************************************************************************//**
\brief disable pwm pins

\param [in] descriptor - pwm descriptor.
\param [in] channel - channel belongs to the descriptor
******************************************************************************/
static void halDisablePwmPins(HAL_PwmDescriptor_t *descriptor, uint8_t channel)
{
  GPIO_disable_pmux(&descriptor->channelPinCfg[channel]);
  GPIO_make_in(&descriptor->channelPinCfg[channel]);
}
#endif // defined(HAL_USE_PWM)

// eof halPwm.c
