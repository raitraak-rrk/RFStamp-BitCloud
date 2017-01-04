/**************************************************************************//**
\file  halStartupGcc.c

\brief Implementation of the start up code for GCC.

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2012, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal
  History:
    07/04/14 karthik.p_u - Created
*******************************************************************************/

/*******************************************************************************
                   Includes section
*******************************************************************************/
#include <atsamr21.h>
#include <core_cm0plus.h>
#include <halInterrupt.h>

/*******************************************************************************
                     External variables section
*******************************************************************************/
extern uint32_t __data;
extern uint32_t __data_end;
extern uint32_t __data_load;
extern uint32_t __bss;
extern uint32_t __bss_end;
extern uint32_t __vectors;
extern HalIntVector_t __ram_vector_table[];

/*******************************************************************************
                   Extern functions prototypes section
*******************************************************************************/
extern int main(void) __attribute__((noreturn));
extern void halLowLevelInit(void);

/******************************************************************************
                              Implementations section
******************************************************************************/
/**************************************************************************//**
\brief Setup environment, do low-level initialization and proceed to main
******************************************************************************/
void gccReset(void)
{
  halLowLevelInit();

  // Remap vector table to real location
  SCB->VTOR = (uint32_t)__ram_vector_table;

  // Relocate .data section
  for (uint32_t *from = &__data_load, *to = &__data; to < &__data_end; from++, to++)
  {
    *to = *from;
  }

  // Clear .bss
  for (uint32_t *to = &__bss; to < &__bss_end; to++)
  {
    *to = 0;
  }

  main();
}
// eof halStartupGcc.c
