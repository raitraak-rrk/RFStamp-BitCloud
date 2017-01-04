/************************************************************//**
  \file rs485Controller.h

  \brief The header file describes functions for enabling\disabling rs485 interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    12/11/08 A. Khromykh - Created
*****************************************************************/
#ifndef _RS485CONTROLLER_H
#define _RS485CONTROLLER_H

/****************************************************************
                   Prototypes section
*****************************************************************/
/************************************************************//**
\brief Enable RTS driving for RS485 driver.
****************************************************************/
void BSP_EnableRs485(void);

/************************************************************//**
\brief Disable RTS driving for RS485 driver.
****************************************************************/
void BSP_DisableRs485(void);

#endif /* _RS485CONTROLLER_H */
