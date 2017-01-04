/**************************************************************************//**
  \file  wdtCtrl.h

  \brief The header file describes the WDT interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    10/12/07 A. Khromykh - Created
 ******************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/

#ifndef _WDTCTRL_H
#define _WDTCTRL_H

/******************************************************************************
                   Includes section
******************************************************************************/
#include <halWdt.h>

/******************************************************************************
                   Types section
******************************************************************************/
// \cond
// An interval before WDT will expire
#if defined(AT91SAM7X256)

  typedef enum
  {
    WDT_INTERVAL_4    = 0xFFE,   // 4 ms
    WDT_INTERVAL_16   = 0xFFB,   // 16 ms
    WDT_INTERVAL_32   = 0xFF9,   // 32 ms
    WDT_INTERVAL_64   = 0xFEF,   // 64 ms
    WDT_INTERVAL_125  = 0xFDF,   // 125 ms
    WDT_INTERVAL_250  = 0xFBF,   // 250 ms
    WDT_INTERVAL_500  = 0xF7F,   // 500 ms
    WDT_INTERVAL_1000 = 0xEFF,   // 1 second
    WDT_INTERVAL_2000 = 0xDFF,   // 2 seconds
    WDT_INTERVAL_4000 = 0xBFF,   // 4 seconds
    WDT_INTERVAL_8000 = 0x7FF    // 8 seconds
  } HAL_WdtInterval_t;

#elif defined(ATMEGA1281) || defined(ATMEGA2561) || defined(ATMEGA1284) || defined(AT90USB1287) || defined(ATMEGA128RFA1) \
   || defined(ATMEGA256RFR2) || defined(ATMEGA2564RFR2)

  typedef enum
  {
    WDT_INTERVAL_16   = 0x00,    // 16 ms
    WDT_INTERVAL_32   = 0x01,    // 32 ms
    WDT_INTERVAL_64   = 0x02,    // 64 ms
    WDT_INTERVAL_125  = 0x03,    // 125 ms
    WDT_INTERVAL_250  = 0x04,    // 250 ms
    WDT_INTERVAL_500  = 0x05,    // 500 ms
    WDT_INTERVAL_1000 = 0x06,    // 1 second
    WDT_INTERVAL_2000 = 0x07,    // 2 seconds
    WDT_INTERVAL_4000 = 0x20,    // 4 seconds
    WDT_INTERVAL_8000 = 0x21     // 8 seconds
  } HAL_WdtInterval_t;

#elif defined(ATXMEGA128A1) || defined(ATXMEGA256A3) || defined(ATXMEGA128B1) || \
    defined(ATXMEGA256D3)

  typedef enum
  {
    WDT_INTERVAL_8,       // 8 ms
    WDT_INTERVAL_16,      // 16 ms
    WDT_INTERVAL_32,      // 32 ms
    WDT_INTERVAL_64,      // 64 ms
    WDT_INTERVAL_125,     // 125 ms
    WDT_INTERVAL_250,     // 250 ms
    WDT_INTERVAL_500,     // 500 ms
    WDT_INTERVAL_1000,    // 1 second
    WDT_INTERVAL_2000,    // 2 seconds
    WDT_INTERVAL_4000,    // 4 seconds
    WDT_INTERVAL_8000     // 8 seconds
  } HAL_WdtInterval_t;

#elif defined(ATSAMD20J18) || defined(ATSAMR21G18A) || defined(ATSAMR21E18A) || defined(ATSAMR21E19A)
  typedef enum
  {
    WDT_INTERVAL_8,       // 8 ms
    WDT_INTERVAL_16,      // 16 ms
    WDT_INTERVAL_32,      // 32 ms
    WDT_INTERVAL_64,      // 64 ms
    WDT_INTERVAL_125,     // 125 ms
    WDT_INTERVAL_250,     // 250 ms
    WDT_INTERVAL_500,     // 500 ms
    WDT_INTERVAL_1000,    // 1 second
    WDT_INTERVAL_2000,    // 2 seconds
    WDT_INTERVAL_4000,    // 4 seconds
    WDT_INTERVAL_8000,    // 8 seconds
    WDT_INTERVAL_16000    // 16 seconds
  } HAL_WdtInterval_t;

#endif  
// \endcond

/******************************************************************************
                   Prototypes section
******************************************************************************/
/***************************************************************************//**
\brief Starts the watch dog timer with a given interval

\ingroup hal_wdt

\param[in]
  interval - the time interval that may be chosen from: \n
      WDT_INTERVAL_16      // 16 ms \n
      WDT_INTERVAL_32      // 32 ms \n
      WDT_INTERVAL_64      // 64 ms \n
      WDT_INTERVAL_125     // 125 ms \n
      WDT_INTERVAL_250     // 250 ms \n
      WDT_INTERVAL_500     // 500 ms \n
      WDT_INTERVAL_1000    // 1 second \n
      WDT_INTERVAL_2000    // 2 seconds \n
      WDT_INTERVAL_4000    // 4 seconds \n
      WDT_INTERVAL_8000    // 8 seconds
*******************************************************************************/
void HAL_StartWdt(HAL_WdtInterval_t interval);

/*******************************************************************//**
\brief Stops the watch dog timer

Note that the function is empty for AT91SAM7X256, because WDT on AT91SAM7X256
cannot be stopped for hardware reasons.
***********************************************************************/
void HAL_StopWdt(void);

/***************************************************************************//**
\brief Registers a callback function called by the watch dog timer when it
expires, before invoking hardware reset.

As soon as the callback function completes, hardware reset is invoked.

Note that function is empty for XMEGA-based platforms, since there is no WDT
interrupt vector for such platforms.

\ingroup hal_wdt

\param[in]
  wdtCallback - pointer to a callback function
*******************************************************************************/
void HAL_RegisterWdtCallback(void (*wdtCallback)(void));

/******************************************************************************
                   Inline static functions section
******************************************************************************/
/**************************************************************************//**
\brief Resets the watch dog timer by reloading its counter;
the timer continues, counting as if it was just started
******************************************************************************/
INLINE void HAL_ResetWdt(void)
{
  wdt_reset();
}

#endif /*_WDTCTRL_H*/

// eof wdtCtrl.h
