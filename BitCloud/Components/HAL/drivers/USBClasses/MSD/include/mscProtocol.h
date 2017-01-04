/****************************************************************************//**
  \file mscProtocol.h

  \brief Declaration of mass storage device protocol command.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    29/08/11 N. Fomin - Created
*******************************************************************************/
#ifndef _MSCPROTOCOL_H
#define _MSCPROTOCOL_H

/******************************************************************************
                   Includes section
******************************************************************************/
#include <sysTypes.h>
#include <usbSetupProcess.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
// data size in request structure
#define MSC_REQUEST_DATA_SIZE       1

// request codes for mass storage class
#define BULK_ONLY_MASS_STORAGE_RESET   0xFF
#define GET_MAX_LUN                    0xFE
// maximum size of CBW data
#define CBW_DATA_SIZE 16

/******************************************************************************
                   Types section
******************************************************************************/
BEGIN_PACK
typedef struct PACK
{
  UsbRequest_t request;
  uint8_t  bData[MSC_REQUEST_DATA_SIZE];
} UsbMscRequest_t;
// MSC command block wrapper (CBW)
typedef struct PACK
{
  uint32_t dCBWSignature;
  uint32_t cDBWTag;
  uint32_t dCBWDataTransferLength;
  uint8_t  bmCBWFlags;
  uint8_t  bCBWLUN;
  uint8_t  bCBWCBLength;
  uint8_t  CBWCB[CBW_DATA_SIZE];
} MscCBW_t;
// MSC command status wrapper (CSW)
typedef struct PACK
{
  uint32_t dCSWSignature;
  uint32_t cDSWTag;
  uint32_t dCSWDataResidue;
  uint8_t  bCSWStatus;
} MscCSW_t;
END_PACK

/******************************************************************************
                   Prototypes section
******************************************************************************/
/**************************************************************************//**
\brief Mass storage device request handler.
\param[in]
  data - pointer to host's request.
******************************************************************************/
void msdRequestHandler(uint8_t *data);

#endif /* _MSCPROTOCOL_H */
// eof msdProtocol.h