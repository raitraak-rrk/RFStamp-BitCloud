/*****************************************************************************//**
\file   halRfPio.h

\brief  AT86RF230 control pins declarations.

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal
  History:
    07/04/14 karthik.p_u - Created
**********************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/

#ifndef _HALRFPIO_H
#define _HALRFPIO_H

/******************************************************************************
                        Includes section.
******************************************************************************/
#include <gpio.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
/* samR21 custom board */
#define IRQ_RF_PORT       IRQ_PORT_B
#define IRQ_RF_PIN        IRQ_EIC_EXTINT0

/******************************************************************************
                 Inline static functions prototypes section.
******************************************************************************/
/* samR21 custom board */
HAL_ASSIGN_PIN(RF_RST,    B, 15);
HAL_ASSIGN_PIN(RF_IRQ,    B, 0);
HAL_ASSIGN_PIN(RF_SLP_TR, A, 20);
HAL_ASSIGN_PIN(RF_CS,     B, 31);
HAL_ASSIGN_PIN(RF_MISO,   C, 19);
HAL_ASSIGN_PIN(RF_MOSI,   B, 30);
HAL_ASSIGN_PIN(RF_SCK,    C, 18);
#endif // _HALRFPIO_H
// eof halRfPio.h
