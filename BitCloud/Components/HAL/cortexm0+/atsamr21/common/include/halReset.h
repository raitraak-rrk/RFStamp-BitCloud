/**************************************************************************//**
  \file  halReset.h

  \brief Declaration of HWD Reset interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    07/04/14 karthik.p_u - Created
*****************************************************************************/
#ifndef _HALRESET_H
#define _HALRESET_H

/******************************************************************************
                        defines section
******************************************************************************/
#include <resetReason.h>

/******************************************************************************
                        external functions section
******************************************************************************/
/**************************************************************************//**
 \brief Read the reset status register.
 \return Current value of the reset status register.
******************************************************************************/
HAL_ResetReason_t halReadResetReason(void);

/**************************************************************************//**
 \brief Reset core and peripherals.
******************************************************************************/
void halSoftwareReset(void);

#endif /* _HALRESET_H */

// eof halReset.h
