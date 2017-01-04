/************************************************************************//**
  \file halIrq.h

  \brief
    HWD interface for external IRQs

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    07/04/14 karthik.p_u - Created
******************************************************************************/
#ifndef _HALIRQ_H_
#define _HALIRQ_H_

/******************************************************************************
                        Includes section
******************************************************************************/
#include <irq.h>

/******************************************************************************
                        Defines section
******************************************************************************/

#define halGlobalIrqEnable()   do {            \
	                            __DMB();        \
	                            __enable_irq(); \
	                          } while (0)
/******************************************************************************
                        Types section
******************************************************************************/
typedef void (* IrqCallback_t)(void);

/******************************************************************************
                    Functions prototypes section
******************************************************************************/
/**************************************************************************//**
\brief Register new handler for external IRQ and do necessary setup

\param[in] id - IRQ id
\param[in] mode - array of triggers each pin
\param[in] cb - callback on IRQ firing
\return 0 if there was an error (handler already registered or collision of
  triggers for different handlers), other value otherwise
******************************************************************************/
uint32_t halRegisterExtIrq(uint32_t id, const HAL_IrqMode_t mode, IrqCallback_t cb);

/**************************************************************************//**
\brief Unregister handler for external IRQ

\param[in] id - IRQ id
\return 0 if handler was not registered, other value otherwise
******************************************************************************/
uint32_t halUnregisterExtIrq(uint32_t id);

/**************************************************************************//**
\brief Enable external IRQ

\param[in] id - IRQ id
******************************************************************************/
void halEnableExtIrq(uint32_t id);

/**************************************************************************//**
\brief Disable external IRQ

\param[in] id - IRQ id
\return 0 if IRQ was already disabled, other value otherwise
******************************************************************************/
uint32_t halDisableExtIrq(uint32_t id);

/**************************************************************************//**
\brief Clear pending external IRQ

\param[in] id - IRQ id
******************************************************************************/
void halClearExtIrq(uint32_t id);

/**************************************************************************//**
\brief Check if user handler for external IRQ is already registered

\param[in] id - IRQ id
\return 0 if not registered, 1 otherwise
******************************************************************************/
uint32_t halIsUserExtIrqRegistered(uint32_t id);

/**************************************************************************//**
\brief Check if id for external IRQ is in valid range for user

\param[in] id - IRQ id
\return 0 if not valid, 1 otherwise
******************************************************************************/
uint32_t halIsUserExtIrqValid(uint32_t id);

#endif // _HALIRQ_H_
// eof halIrq.h

