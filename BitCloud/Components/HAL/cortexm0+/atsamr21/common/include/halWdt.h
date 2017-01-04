/**************************************************************************//**
  \file  halWdt.h

  \brief  Declarations of wdt hardware-dependent module.

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

#ifndef _HALWDT_H
#define _HALWDT_H

/******************************************************************************
                   Includes section
******************************************************************************/
#include <sysTypes.h>
#include <atsamr21.h>
#include <halClkCtrl.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
/**************************************************************************//**
\brief Resets the WDT Counter
******************************************************************************/
void wdt_reset(void);

/******************************************************************************
 Polling the Sync. flag for register access
 Parameters:
   none
 Returns:
   none
 *****************************************************************************/
INLINE void wdtTCSync(void)
{
  while (TC5_16_STATUS_s.syncbusy);
}

#endif /* _HALWDT_H */

//eof halWdt.h
