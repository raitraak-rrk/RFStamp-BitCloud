/****************************************************************************//**
  \file mscProtocol.c

  \brief Implementation of communication device protocol command.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    29/08/11 N. Fomin - Created
*******************************************************************************/
/******************************************************************************
                   Includes section
******************************************************************************/
#include <mscProtocol.h>
#include <usb.h>

/******************************************************************************
                   Implementations section
******************************************************************************/
/**************************************************************************//**
\brief Mass storage device request handler.
\param[in]
  data - pointer to host's request.
******************************************************************************/
void msdRequestHandler(uint8_t *data)
{
  UsbMscRequest_t *pRequest = NULL;

  pRequest = (UsbMscRequest_t *)data;
  if (NULL == pRequest)
    return;

  // Check request code
  switch (pRequest->request.bRequest)
  {
    case BULK_ONLY_MASS_STORAGE_RESET:
      break;
    case GET_MAX_LUN:
      HAL_Stall(0);
      break;
    default:
      HAL_Stall(0);
      break;
  }
}

//eof mscProtocol.c
