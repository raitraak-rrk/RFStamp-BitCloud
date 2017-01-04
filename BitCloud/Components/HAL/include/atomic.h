/**************************************************************************//**
  \file  atomic.h

  \brief The header file describes the atomic sections

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    20/08/07 A. Khromykh - Created
    07/04/14 karthik.p_u - Modified
 ******************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/

#ifndef _ATOMIC_H
#define _ATOMIC_H

/******************************************************************************
                   Includes section
******************************************************************************/
#if defined(AT91SAM7X256) || defined(AT32UC3A0512) || defined(AT91SAM3S4C) || defined(AT91SAM4S16C) || defined(ATSAMD20J18) \
 || defined(ATSAMR21G18A) || defined(ATSAMR21E18A) || defined(ATSAMR21E19A)
  #include <inttypes.h>
#elif defined(ATMEGA1281) || defined(ATMEGA2561) || defined(ATMEGA1284) || defined(AT90USB1287) \
   || defined(ATXMEGA128A1) || defined(ATXMEGA256A3) || defined(ATXMEGA128B1) || defined(ATXMEGA256D3) \
   || defined(ATMEGA128RFA1) || defined(ATMEGA256RFR2) || defined(ATMEGA2564RFR2) || defined(WIN)
  #include <halAtomic.h>
#endif

#if defined(AT91SAM7X256) || defined(AT32UC3A0512) || defined(AT91SAM3S4C) || defined(AT91SAM4S16C) || defined(ATSAMD20J18) \
 || defined(ATSAMR21G18A) || defined(ATSAMR21E18A) || defined(ATSAMR21E19A)
/******************************************************************************
                   Types section
******************************************************************************/
typedef  uint32_t atomic_t;

/******************************************************************************
                   Inline static functions section
******************************************************************************/
/******************************************************************************
 Disables global interrupt. Enter atomic section.
******************************************************************************/
void halStartAtomic(atomic_t volatile *pAtomic);

/******************************************************************************
 Exit atomic section
******************************************************************************/
void halEndAtomic(atomic_t volatile *pAtomic);
  /** \brief Marks the begin of atomic section */
  #define ATOMIC_SECTION_ENTER  {volatile atomic_t __atomic; halStartAtomic(&__atomic);
  /** \brief Marks the end of atomic section */
  #define ATOMIC_SECTION_LEAVE  halEndAtomic(&__atomic);}
#elif defined(ATMEGA1281) || defined(ATMEGA2561) || defined(ATMEGA1284) || defined(AT90USB1287) \
   || defined(ATXMEGA128A1) || defined(ATXMEGA256A3) || defined(ATXMEGA128B1) || defined(ATXMEGA256D3) \
   || defined(ATMEGA128RFA1) || defined(ATMEGA256RFR2) || defined(ATMEGA2564RFR2)
  /** \brief Marks the begin of atomic section */
  #define ATOMIC_SECTION_ENTER {atomic_t __atomic = halStartAtomic();
  /** \brief Marks the end of atomic section */
  #define ATOMIC_SECTION_LEAVE halEndAtomic(__atomic);}
#elif defined(SIMULATOR)
  /** \brief Marks the begin of atomic section */
  #define ATOMIC_SECTION_ENTER  do {} while (0);
  /** \brief Marks the end of atomic section */
  #define ATOMIC_SECTION_LEAVE  do {} while (0);
#elif defined(WIN)
  /** \brief Marks the begin of atomic section */
  #define ATOMIC_SECTION_ENTER {halStartAtomic();
  /** \brief Marks the end of atomic section */
  #define ATOMIC_SECTION_LEAVE halEndAtomic();}
#endif

#endif // _ATOMIC_H

// eof atomic.h
