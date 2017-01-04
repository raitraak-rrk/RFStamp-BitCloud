/****************************************************************************//**
  \file usbSetupProcess.c

  \brief Implementation of setup (after numeration) proccess.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    26/08/11 N. Fomin - Created
*******************************************************************************/
/******************************************************************************
                   Includes section
******************************************************************************/
#include <usbDescriptors.h>
#if (APP_INTERFACE == APP_INTERFACE_VCP)
  #include <vcpCdcProtocol.h>
  #if (MSD_SUPPORT == 1)
    #include <mscProtocol.h>
  #endif // (MSD_SUPPORT == 1)
#else
  #if (MSD_SUPPORT == 1)
    #include <mscProtocol.h>
  #else
    #include <usbSetupProcess.h>
  #endif // (MSD_SUPPORT == 1)
#endif // (APP_INTERFACE == APP_INTERFACE_VCP)
#if (DFU_SUPPORT == 1)
  #include <dfuProtocol.h>
  #if (APP_INTERFACE != APP_INTERFACE_VCP) && (MSD_SUPPORT != 1)
    #error
  #endif // (APP_INTERFACE != APP_INTERFACE_VCP) || (MSD_SUPPORT != 1)
#endif // (DFU_SUPPORT == 1)

/******************************************************************************
Usb setup process request handler

Parameters:
  data - pointer to host's request
******************************************************************************/
void setupProcessRequestHandler(uint8_t *data)
{
  UsbRequest_t *pRequest = NULL;

  pRequest = (UsbRequest_t *)data;
  if (NULL == pRequest)
    return;

  switch (pRequest->wIndex)
  {
  #if (APP_INTERFACE == APP_INTERFACE_VCP)
    case NUMBER_OF_FIRST_INTERFACE:
      vcpRequestHandler(data);
      break;
  #endif // (APP_INTERFACE == APP_INTERFACE_VCP)
    /* MSC commands */
  #if (MSD_SUPPORT == 1)
    #if (APP_INTERFACE == APP_INTERFACE_VCP)
      case NUMBER_OF_THIRD_INTERFACE:
    #else
      case NUMBER_OF_FIRST_INTERFACE:
    #endif
        msdRequestHandler(data);
        break;
  #endif // (MSD_SUPPORT == 1)
  #if (DFU_SUPPORT == 1)
    #if (APP_INTERFACE == APP_INTERFACE_VCP)
      #if (MSD_SUPPORT == 1)
        case NUMBER_OF_FOURTH_INTERFACE:
      #else
        case NUMBER_OF_THIRD_INTERFACE:
      #endif
    #else
      case NUMBER_OF_SECOND_INTERFACE:
    #endif // (APP_INTERFACE == APP_INTERFACE_VCP)
        dfuRequestHandler(data);
        break;
  #endif // (DFU_SUPPORT == 1)
    default:
      HAL_Stall(0);
      break;
  }
}

// eof usbSetupProcess.c
