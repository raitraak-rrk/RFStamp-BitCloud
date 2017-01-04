/**************************************************************************//**
  \file  pwm.c

  \brief Implementation of PWM interface.

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
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/
#if defined(HAL_USE_PWM)
/******************************************************************************
                   Includes section
******************************************************************************/
#include <halPwm.h>
#include <pwm.h>

/******************************************************************************
                   Defines section
******************************************************************************/

/******************************************************************************
                   Constants section
******************************************************************************/

/******************************************************************************
                   Global variables section
******************************************************************************/

/******************************************************************************
                   Implementations section
******************************************************************************/

/**************************************************************************//**
\brief Initializes the PWM.

\param [in] pwmUnit - PWM unit number.
                      Equal to ID of Timer/Counter witch serves PWM module.

\return operation status
******************************************************************************/
int HAL_OpenPwm(HAL_PwmUnit_t pwmUnit)
{
  if (PWM_INVALID_UNIT > pwmUnit)
    halOpenPwm(pwmUnit);
  else
    return PWM_INVALID_UNIT_STATUS;

  return PWM_SUCCESS_STATUS;
}

/**************************************************************************//**
\brief Starts PWM on specified channel.

\param [in] descriptor - PWM channel descriptor.

\return operation status
******************************************************************************/
int HAL_StartPwm(HAL_PwmDescriptor_t *descriptor)
{
  /* Check PWM unit */
  if (PWM_INVALID_UNIT > descriptor->unit)
    halStartPwm(descriptor);
  else
    return PWM_INVALID_UNIT_STATUS;

  return PWM_SUCCESS_STATUS;
}

/**************************************************************************//**
\brief Stops PWM on specified channel.

\param [in] descriptor - PWM channel descriptor.

\return operation status
******************************************************************************/
int HAL_StopPwm(HAL_PwmDescriptor_t *descriptor)
{
  /* Check PWM unit */
  if (PWM_INVALID_UNIT > descriptor->unit)
    halStopPwm(descriptor);
  else
    return PWM_INVALID_UNIT_STATUS;

  return PWM_SUCCESS_STATUS;
}

/**************************************************************************//**
\brief Sets base frequency of module. Common for all module channels.

\param [in] descriptor - PWM channel descriptor.
\param [in] top - value for the TOP register.
\param [in] prescaler - clock prescaler.

\return operation status
******************************************************************************/
int HAL_SetPwmFrequency(HAL_PwmDescriptor_t *descriptor, uint16_t top, HAL_PwmPrescaler_t prescaler)
{
  /* Check prescaler value */
  if (PWM_PRESCALER_INVALID <= prescaler)
    return PWM_INVALID_PRESCALER_STATUS;
  else halSetPwmFrequency(descriptor, top, prescaler);

  return PWM_SUCCESS_STATUS;
}

/**************************************************************************//**
\brief Sets compare value for the PWM channel.

\param [in] descriptor - PWM channel descriptor.

\return operation status
******************************************************************************/
int HAL_SetPwmCompareValue(HAL_PwmDescriptor_t *descriptor, uint16_t cmpValue)
{
  /* Check PWM unit */
  if (PWM_INVALID_UNIT > descriptor->unit)
    halSetPwmCompareValue(descriptor, cmpValue);
  else
    return PWM_INVALID_UNIT_STATUS;

  return PWM_SUCCESS_STATUS;
}

/**************************************************************************//**
\brief Closes the PWM.

\param [in] pwmUnit - PWM unit number.
                      Equal to ID of Timer/Counter witch serves PWM module.

\return operation status
******************************************************************************/
int HAL_ClosePwm(HAL_PwmUnit_t pwmUnit)
{
  /* Check PWM unit */
  if (PWM_INVALID_UNIT > pwmUnit)
    halClosePwm(pwmUnit);
  else
    return PWM_INVALID_UNIT_STATUS;

  return PWM_SUCCESS_STATUS;
}
#endif // defined(HAL_USE_PWM)

// eof pwm.c
