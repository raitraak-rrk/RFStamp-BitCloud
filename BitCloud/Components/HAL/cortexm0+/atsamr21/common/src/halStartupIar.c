/**************************************************************************//**
\file  halStartupIar.c

\brief Implementation of the start up code for IAR.

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2012, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal
  History:
    07/04/14 karthik.p_u - Created
*******************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/

/******************************************************************************
                   Includes section
******************************************************************************/
#include <atsamr21.h>
#include <core_cm0plus.h>
#include <halInterrupt.h>

/*******************************************************************************
                     External variables section
*******************************************************************************/
extern HalIntVector_t __ram_vector_table[];

/*******************************************************************************
                   Prototypes section
*******************************************************************************/
extern void halLowLevelInit(void);

/******************************************************************************
                              Implementations section
******************************************************************************/
/**************************************************************************//**
\brief Specify function for low level initialization. It needs for IAR.
The function must return 1 to initialize variable section

\return
  1 - always.
******************************************************************************/
int __low_level_init(void)
{
  halLowLevelInit();

  // Remap vector table to real location
  SCB_VTOR = (uint32_t)__ram_vector_table;
  return 1; /*  if return 0, the data sections will not be initialized. */
}

// eof halStartupIar.c
