/**************************************************************************//**
  \file N_Watchdog_Internal.h

  \brief Watchdog related definitions for use only within the platform.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2012-01-21 D. Kolmakov - Created
 ******************************************************************************/

#ifndef N_WATCHDOG_INTERNAL_H
#define N_WATCHDOG_INTERNAL_H

/******************************************************************************
                   Includes section
******************************************************************************/
#include "N_Watchdog.h"

/******************************************************************************
                   Prototypes section
******************************************************************************/
/*******************************************************************//**
\brief Disable the watchdog.
***********************************************************************/
void N_Watchdog_Disable(void);

/*******************************************************************//**
\brief Initializes the watchdog component.
***********************************************************************/
void N_Watchdog_ReInit(void);

#endif // N_WATCHDOG_INTERNAL_H
// eof N_Watchdog_Internal.h