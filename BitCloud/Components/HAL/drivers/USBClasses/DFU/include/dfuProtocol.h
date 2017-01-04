/****************************************************************************//**
  \file dfuProtocol.h

  \brief Declaration of Device firmware upgrade commands.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    08/09/11 N. Fomin - Created
*******************************************************************************/
#ifndef _DFUPROTOCOL_H
#define _DFUPROTOCOL_H

/******************************************************************************
                   Includes section
******************************************************************************/
#include <sysTypes.h>
#include <usbSetupProcess.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
// data size in request structure
#define DFU_REQUEST_DATA_SIZE 6
// request codes for mass storage class
#define DFU_DETACH    0x00
#define DFU_GETSTATUS 0x03
#define DFU_GETSTATE  0x05

/******************************************************************************
                   Types section
******************************************************************************/
BEGIN_PACK
// Usb host request
typedef struct PACK
{
  UsbRequest_t request;
  uint8_t  bData[DFU_REQUEST_DATA_SIZE];
} UsbDfuRequest_t;

// Usb dfu get status response
typedef struct PACK
{
  uint8_t  bStatus;
  uint8_t  bPollTimeout[3];
  uint8_t  bState;
  uint8_t  iString;
} DfuGetStatusResponse_t;

// Usb dfu get state response
typedef struct PACK
{
  uint8_t  bState;
} DfuGetStateResponse_t;

typedef union PACK
{
  DfuGetStatusResponse_t getStatusResponse;
  DfuGetStateResponse_t getStateResponse;
} UsbDfuResponse_t;

END_PACK

/******************************************************************************
                   Prototypes section
******************************************************************************/
/**************************************************************************//**
\brief Device firmware upgrade request handler.
\param[in]
  data - pointer to host's request.
******************************************************************************/
void dfuRequestHandler(uint8_t *data);

#endif /* _DFUPROTOCOL_H */
// eof dfuProtocol.h