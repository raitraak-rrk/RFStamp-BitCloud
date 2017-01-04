/**************************************************************************//**
  \file  wdt.c

  \brief Implementation of WDT interrupt handler.

  \author
      Atmel Corporation: http://www.atmel.com \n
      Support email: avr@atmel.com

    Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
    Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    19/08/13 Agasthian.s - Created
 ******************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/
#if defined(HAL_USE_WDT)
/******************************************************************************
                   Includes section
******************************************************************************/
#include <wdtCtrl.h>
#include <atomic.h>
#include <halDbg.h>
#include <halDiagnostic.h>

/******************************************************************************
                   Global variables section
******************************************************************************/
void (*halWdtCallback)(void) = NULL;

/*******************************************************************************
Registers WDT fired callback.
Parameters:
  wdtCallback - callback.
Returns:
  none.
*******************************************************************************/
void HAL_RegisterWdtCallback(void (*wdtCallback)(void))
{
  halWdtCallback = wdtCallback;
}

/*******************************************************************************
Starts WDT with interval.
Parameters:
  interval - interval.
Returns:
  none.
*******************************************************************************/
void HAL_StartWdt(HAL_WdtInterval_t interval)
{
  uint8_t i = 0;

  if (halWdtCallback)
    i = (1<<WDE) | (1 << WDIE) | interval;
  else
    i = (1<<WDE) | interval;

  ATOMIC_SECTION_ENTER
  BEGIN_MEASURE
  wdt_reset();
  WDTCSR |= (1<<WDCE) | (1<<WDE);
  WDTCSR = i;
  END_MEASURE(HALATOM_WDT_START_MEASURE_CODE)
  ATOMIC_SECTION_LEAVE
}

/*******************************************************************//**
\brief Stops the Watch Dog timer

Note that function is empty for AT91SAM7X256. WDT for AT91SAM7X256 is
impossible to stop.
***********************************************************************/
#if defined(__ICCAVR__)
#pragma optimize=medium
#endif
void HAL_StopWdt(void)
{
  wdt_disable();
}

/*******************************************************************************
Interrupt handler.
*******************************************************************************/
ISR(WDT_vect)
{
  if (NULL != halWdtCallback)
    halWdtCallback();
  wdt_enable(0);
  for (;;);
}
#endif // defined(HAL_USE_WDT)

//eof wdt.c
