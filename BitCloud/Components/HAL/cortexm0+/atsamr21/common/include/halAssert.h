/**************************************************************************//**
\file  halAssert.h

\brief Implementation of arm assert algorithm.

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal
  History:
    07/04/14 karthik.p_u - Created
******************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/

#ifndef _HALASSERT_H
#define _HALASSERT_H

/******************************************************************************
                   Includes section
******************************************************************************/
#include <atomic.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
#ifdef _SYS_ASSERT_ON_

/******************************************************************************
                   Inline static functions section
******************************************************************************/
INLINE void halAssert(uint8_t condition, uint16_t dbgCode)
{
  ATOMIC_SECTION_ENTER
  if (!(condition))
  {
    while(1)
      (void)dbgCode;
  }
  ATOMIC_SECTION_LEAVE
}
#else /* _SYS_ASSERT_ON_ */
  #define halAssert(condition, dbgCode) {do {} while (0);}
#endif /* _SYS_ASSERT_ON_ */
#endif /* _HALASSERT_H */

// eof halAssert.h
