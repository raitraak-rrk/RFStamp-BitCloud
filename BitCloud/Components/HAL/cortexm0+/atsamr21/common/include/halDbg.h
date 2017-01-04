/*****************************************************************************//**
\file  halDbg.h

\brief Declarations of hal , bsb mistake interface.

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

#ifndef _HALDBG_H
#define _HALDBG_H

/******************************************************************************
                   Includes section
******************************************************************************/
#include <dbg.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
enum
{
  APPTIMER_MISTAKE                         = 0x2000,
  USARTC_HALUNKNOWNERRORREASON_0           = 0x2001,
  USARTC_HALUNKNOWNERRORREASON_1           = 0x2002,
  USARTC_HALSIGUSARTTRANSMISSIONCOMPLETE_0 = 0x2003,
  USARTC_HALSIGUSARTRECEPTIONCOMPLETE_0    = 0x2004,
  APPTIMER_HANDLER_0                       = 0x2005,
  IRQ_NULLCALLBACK_0                       = 0x2006,
  EXT_IRQ_NULLCALLBACK0                    = 0x2007,
  FLASH_PAGEFILLBUFFERINCORRECTOFFSET_0    = 0x2008,
  I2C_NULLCALLBACK_0                       = 0x2009,
  SLEEPTIMER_NULLCALLBACK_0                = 0x200a,
  ADC_NULLCALLBACK_0                       = 0x200b,
  EEPROM_EMULATION_NO_MEMORY               = 0x200c,
  EEPROM_EMULATION_NOT_INITIALIZED_0       = 0x200d,
  EEPROM_EMULATION_NOT_INITIALIZED_1       = 0x200e,
  EEPROM_EMULATION_NOT_INITIALIZED_2       = 0x200f,
  EEPROM_EMULATION_BAD_ADDRESS_0           = 0x2010,
  EEPROM_EMULATION_BAD_ADDRESS_1           = 0x2011
};

/******************************************************************************
                   Prototypes section
******************************************************************************/

#endif /* _HALDBG_H */

// eof halDbg.h
