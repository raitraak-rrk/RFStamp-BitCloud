/****************************************************************************//**
  \file usbEnumeration.h

  \brief Declaration of enumeration process command.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    12/09/08 A. Khromykh - Created
    26/08/11 N. Fomin - Modified (MSD support)
*******************************************************************************/
#ifndef _USBENUMERATION_H
#define _USBENUMERATION_H

/******************************************************************************
                   Includes section
******************************************************************************/
#include <sysTypes.h>

/******************************************************************************
                   Prototypes section
******************************************************************************/
/******************************************************************************
Standard usb request handler

Parameters:
  data - pointer to host's request
******************************************************************************/
void runtimeRequestHandler(uint8_t *data);

/******************************************************************************
Standard usb request handler

Parameters:
  data - pointer to host's request
******************************************************************************/
void usbBusResetAction(void);

#if defined(AT91SAM7X256) || defined(AT91SAM3S4C)
/******************************************************************************
send zero-length packet through control pipe
******************************************************************************/
void sendZLP(void);
#endif

/**************************************************************************//**
\brief Sets serial number for serial string descriptor.
******************************************************************************/
void usbSetSerialNumber(uint32_t sn);

#endif /* _USBENUMERATION_H */
// eof usbEnumeration.h