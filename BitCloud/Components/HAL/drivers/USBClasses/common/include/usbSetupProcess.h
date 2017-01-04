/****************************************************************************//**
  \file usbSetupProcess.h

  \brief Declaration of setup (after numeration) process command.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    26/08/11 N. Fomin - Created
*******************************************************************************/
#ifndef _USBESETUPPROCESS_H
#define _USBESETUPPROCESS_H

/******************************************************************************
                   Includes section
******************************************************************************/
#include <sysTypes.h>

/******************************************************************************
                   Types section
******************************************************************************/
BEGIN_PACK
// Usb host request
typedef struct PACK
{
  uint8_t  bmRequestType;
  uint8_t  bRequest;
  uint16_t wValue;
  uint16_t wIndex;
  uint16_t wLength;
} UsbRequest_t;
END_PACK
/******************************************************************************
                   Prototypes section
******************************************************************************/
/******************************************************************************
Usb setup process request handler

Parameters:
  data - pointer to host's request
******************************************************************************/
void setupProcessRequestHandler(uint8_t *data);

#endif /* _USBESETUPPROCESS_H */
// eof usbSetupProcess.h