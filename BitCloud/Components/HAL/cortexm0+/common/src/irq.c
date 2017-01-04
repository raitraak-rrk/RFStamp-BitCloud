/************************************************************************//**
  \file irq.c

  \brief
    External IRQs for Cortex-M3

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    19/08/13 Agasthian.s - Created
******************************************************************************/

/*****************************************************************************
                               Includes section
******************************************************************************/
#include <sysTypes.h>
#include <halIrq.h>
#include <halDbg.h>
#include <sysAssert.h>

/******************************************************************************
                        Implementations section
******************************************************************************/
/**************************************************************************//**
\brief Register user external interrupt

\param[in] irqNumber - IRQ number
\param[in] irqMode - IRQ mode (sense control)
\param[in] f - user callback on IRQ firing. Must be executed in less than 100 us.
\return -1 if irqNumber is out of range or such interrupt has been registered               
  already, or handler is NULL. 0 otherwise.
******************************************************************************/
int HAL_RegisterIrq(HAL_IrqNumber_t irqNumber, HAL_IrqMode_t irqMode, void (*f)(void))
{
  if (NULL == f)
  {
    SYS_E_ASSERT_ERROR(false, IRQ_NULLCALLBACK_0);
    return -1;
  }
  if (!halIsUserExtIrqValid(irqNumber))
    return -1;

  if (halIsUserExtIrqRegistered(irqNumber))
    return -1;

  return halRegisterExtIrq(irqNumber, irqMode, f) ? 0 : -1;
}

/*************************************************************************//**
\brief Enables the irqNumber interrupt.
\param[in]
   irqNumber - IRQ number
\return
   -1 - if irqNumber is out of range or has not been registered yet. \n
    0 - otherwise.
*****************************************************************************/
int HAL_EnableIrq(HAL_IrqNumber_t irqNumber)
{
  if (!halIsUserExtIrqValid(irqNumber))
    return -1;

  if (!halIsUserExtIrqRegistered(irqNumber))
    return -1;

  halEnableExtIrq(irqNumber);

  return 0;
}

/*************************************************************************//**
\brief Disables the irqNumber interrupt.
\param[in]
   irqNumber - IRQ number
\return
   -1 - if irqNumber is out of range or has not been registered yet. \n
    0 - otherwise.
*****************************************************************************/
int HAL_DisableIrq(HAL_IrqNumber_t irqNumber)
{
  if (!halIsUserExtIrqValid(irqNumber))
    return -1;

  if (!halIsUserExtIrqRegistered(irqNumber))
    return -1;

  halDisableExtIrq(irqNumber);

  return 0;
}

/*************************************************************************//**
\brief Unregisters the user's irqNumber interrupt
\param[in]
   irqNumber - IRQ number
\return
   -1 - if irqNumber is out of range or has not been registered yet. \n
    0 - otherwise.
*****************************************************************************/
int HAL_UnregisterIrq(HAL_IrqNumber_t irqNumber)
{
  if (!halIsUserExtIrqValid(irqNumber))
    return -1;

  if (!halIsUserExtIrqRegistered(irqNumber))
    return -1;

  return halUnregisterExtIrq(irqNumber) ? 0 : -1;
}
// eof irq.c
