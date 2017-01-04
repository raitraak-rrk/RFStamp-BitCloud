/**************************************************************************//**
\file  adc.c

\brief Implementation of ADC interface.

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal
  History:
    11/06/14 P. Mahendran - Created
*******************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/
#if defined(HAL_USE_ADC)
/******************************************************************************
                   Includes section
******************************************************************************/
#include <adc.h>
#include <halDbg.h>

/******************************************************************************
                   Global variables section
******************************************************************************/
HAL_AdcDescriptor_t *adcDesc = NULL;

/******************************************************************************
                   Implementations section
******************************************************************************/
/**************************************************************************//**
  \brief Opens the ADC to make the measurement on the ADC channel.

  \param[in]
    descriptor - pointer to HAL_AdcDescriptor_t structure

  \return
    -1 - unsupported parameter or ADC is busy. \n
     0 - success.
******************************************************************************/
int HAL_OpenAdc(HAL_AdcDescriptor_t *descriptor)
{
  if (adcDesc)
    return -1;

  adcDesc = descriptor;
  if ((NULL == adcDesc->bufferPointer) || (0 == adcDesc->selectionsAmount))
    return -1;
  if (adcDesc->resolution > RESOLUTION_8_BIT)
    return -1;

  halOpenAdc();
  return 0;
}

/**************************************************************************//**
  \brief Starts ADC with the parameters defined in HAL_OpenAdc.

  \param[in] descriptor - pointer to HAL_AdcDescriptor_t structure

  \param[in] channel - logical channel. It means from what input(simple, differential or internal)
             is going to be read voltage.

  \return
    -1 - unable to open ADC (unsupported channel number). \n
     0 - success.
******************************************************************************/
int HAL_ReadAdc(HAL_AdcDescriptor_t *descriptor, HAL_AdcChannelNumber_t channel)
{
  if (adcDesc != descriptor)
    return -1;

  halStartAdc(channel);
  return 0;
}

/**************************************************************************//**
  \brief Closes the ADC.

  \param[in] descriptor - pointer to HAL_AdcDescriptor_t structure.

  \return
    -1  - the module was not open. \n
     0  - success.
******************************************************************************/
int HAL_CloseAdc(HAL_AdcDescriptor_t *descriptor)
{
  if (adcDesc != descriptor)
    return -1;

  adcDesc = NULL;
  halCloseAdc();
  return 0;
}

/**************************************************************************//**
\brief ADC interrupt handler.
******************************************************************************/
void halSigAdcHandler(void)
{
  sysAssert(adcDesc->callback, ADC_NULLCALLBACK_0);
  adcDesc->callback();
}
#endif // defined(HAL_USE_ADC)

// eof adc.c
