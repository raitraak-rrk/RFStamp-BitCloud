/**************************************************************************//**
\file  stackStat.c

\brief Collection of stack statistics runtime data for report

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal
  History:
    21/02/2015 Mukesh Kumar Basrani - Created
  Last Change:
    $Id: statStack.c 27648 2015-02-24 05:42:55Z mukesh.basrani $
*******************************************************************************/
#if defined(_STATS_ENABLED_)

/******************************************************************************
                   Includes section
******************************************************************************/
#include <sysTypes.h>
#include <statStack.h>
#include <atsamr21.h>
#include <core_cm0plus.h>

/******************************************************************************
                   Define section
******************************************************************************/
/******************************************************************************
				   Extern section
******************************************************************************/

#ifdef __GNUC__
extern unsigned int __stack_start;
extern unsigned int __stack;
#endif

/******************************************************************************
                    Static functions prototypes section
******************************************************************************/

uint16_t probeStack(const volatile uint8_t *start, const volatile uint8_t *end);


// Preincluded generator for reporting stack usage. Only classic AVRs are supported for now

/**************************************************************************//**
\brief Measure stack usage by searching for first overwritten byte of
       default fill pattern

\param[in] start - start of section
\param[in] end - end of section (address of byte after the last one)
\return number of used bytes
******************************************************************************/
uint16_t probeStack(const volatile uint8_t *start, const volatile uint8_t *end)
{  
  while (start < end && *(uint32_t *)start == 0x0A0B0C0D)
    start+=4;
  return end - start;
}


/**************************************************************************//**
\brief Measures stack and returns

\param[out] None
\param[in] None
\return number of actually placed bytes
******************************************************************************/

uint16_t Stat_GetCurrentStackLeft(void)
{
#if defined(__ICCARM__)  //Only for ARM
  #pragma section = "CSTACK"
  volatile uint8_t* pCStackEnd = __segment_begin("CSTACK");
#elif defined(__GNUC__) 
  volatile uint8_t* pCStackEnd = (uint8_t *)&__stack_start;
#else
  return STACK_CHECK_DISABLED;
#endif	
  uint8_t *pCStack = (uint8_t *)__get_MSP();

  return ((uint16_t) (pCStack - pCStackEnd));
}
#endif
// eof stackStat.c
