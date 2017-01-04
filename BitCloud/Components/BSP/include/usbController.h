/************************************************************//**
  \file usbController.h

  \brief The header file describes functions for driving usb line.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    12/11/08 A. Khromykh - Created
*****************************************************************/
#ifndef _USBCONTROLLER_H
#define _USBCONTROLLER_H

/****************************************************************
                   Prototypes section
*****************************************************************/
/************************************************************//**
\brief Turn on pull-up on D+.
****************************************************************/
void BSP_EnableUsb(void);

/************************************************************//**
\brief Turn off pull-up on D+.
****************************************************************/
void BSP_DisableUsb(void);

/************************************************************//**
\brief Read VBUS line for detection of usb cable connection.

\return
  0 - cable is not connected.
  1 - cable is not connected.
****************************************************************/
uint8_t BSP_ReadVBUS(void);

#endif /* _USBCONTROLLER_H */
