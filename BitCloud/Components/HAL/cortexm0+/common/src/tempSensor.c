/**************************************************************************//**
\file  tempSensor.c

\brief Implementation of temperature sensor interface.

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal
  History:
    09/09/14 Unithra.C - Created
*******************************************************************************/
#if defined(HAL_USE_TS)
/******************************************************************************
                   Includes section
******************************************************************************/
#include <tempSensor.h>
#include <haltempSensor.h>
/******************************************************************************
                   Global variables section
******************************************************************************/
HAL_TSDescriptor_t *tsDesc = NULL;
/******************************************************************************
                   Implementations section
******************************************************************************/
/**************************************************************************//**
  \brief Enable temperature sensor and initialize ADC for measurement

  \param[in]
    descriptor - pointer to HAL_TSDescriptor_t structure

  \return
    -1 - unsupported parameter
     0 - success.
******************************************************************************/
int HAL_OpenTempSensor(HAL_TSDescriptor_t *descriptor)
{
  if (tsDesc)
    return -1;

  tsDesc = descriptor;
  if (NULL == tsDesc->temperature)
    return -1;
  if(NULL != ((tsDesc->tsAdcDescriptor).callback))
    return -1;

  return (halOpenTempSensor());
}
/**************************************************************************//**
  \brief Measure the voltage given by temperature sensor through ADC

  \param[in]
    descriptor - pointer to HAL_TSDescriptor_t structure

  \return
    -1 - unsupported parameter
     0 - success.
******************************************************************************/
int HAL_ReadTempSensor(HAL_TSDescriptor_t *descriptor)
{
  if (tsDesc != descriptor)
    return -1;

  return (halStartTempSensor());
}
/**************************************************************************//**
  \brief  Stop measuring the temp sensor voltage through ADC

  \param[in]
    descriptor - pointer to HAL_TSDescriptor_t structure

  \return
    -1 - unsupported parameter
     0 - success.
******************************************************************************/
int HAL_CloseTempSensor(HAL_TSDescriptor_t *descriptor)
{
  if (tsDesc != descriptor)
    return -1;

  tsDesc = NULL;
  return (halCloseTempSensor());
}

#endif // defined(HAL_USE_TS)

// eof tempSensor.c

