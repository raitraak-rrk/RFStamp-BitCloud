/**************************************************************************//**
  \file  reducePower.h

  \brief The header file describes the power reducing interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    6/10/09 A. Khromykh - Created.
 ******************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/
#ifndef _REDUCEPOWER_H
#define _REDUCEPOWER_H

/******************************************************************************
                               Prototypes section
******************************************************************************/
/******************************************************************************
\brief Stops the clock to all peripherals, which are not used by BitCloud. \n
(implemented only for atxmega128a1 & atxmega256a3).

\ingroup hal_misc
******************************************************************************/
void HAL_SwitchOffPeripherals(void);

#endif /* _REDUCEPOWER_H */
