/**************************************************************************//**
\file  statStack.h

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
    $Id: statStack.h 27648 2015-02-24 05:42:55Z mukesh.basrani $
*******************************************************************************/

/******************************************************************************
                   Includes section
******************************************************************************/

/******************************************************************************
                   Define section
******************************************************************************/
#ifndef _STAT_STACK_H_
#define _STAT_STACK_H_

#define STACK_CHECK_DISABLED (0xFFFFu)

#define UTIL_CSTACK_PTR_HIADDR (0x001Du)
#define UTIL_CSTACK_PTR_LOADDR (UTIL_CSTACK_PTR_HIADDR-1)

#ifdef __BOARD_SAMR21__
#define STACK_INIT_MARGIN 0xF0
#else
#define STACK_INIT_MARGIN 0x0
#endif
/******************************************************************************
                    Static functions prototypes section
******************************************************************************/




/**************************************************************************//**
\brief Measure stack usage by searching for first overwritten byte of
       default fill pattern

\param[in] start - start of section
\param[in] end - end of section (address of byte after the last one)
\return number of used bytes
******************************************************************************/
uint16_t probeStack(const volatile uint8_t *start, const volatile uint8_t *end);


/**************************************************************************//**
\brief Measures stack and returns

\param[out] None
\param[in] None
\return number of actually placed bytes
******************************************************************************/
uint16_t Stat_GetCurrentStackLeft(void);

#endif
// eof stackStat.h
