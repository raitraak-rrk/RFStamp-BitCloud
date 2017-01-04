/****************************************************************************//**
  \file dfuProtocol.c

  \brief Implementation of Device firmware upgrade commands.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    08/09/11 N. Fomin - Created
*******************************************************************************/
/******************************************************************************
                   Includes section
******************************************************************************/
#include <dfuProtocol.h>
#include <usb.h>
#include <resetReason.h>
#include <halFlash.h>
#include <usbEnumeration.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
#define DFU_STATUS_OK               0x00
#define DFU_STATUS_MAX_POLL_TIMEOUT 0x64
#define DFU_STATUS_NO_STRING        0x00

#define BOOTLOADER_FLAG_FLASH_OFFSET 5

/******************************************************************************
                   Global variables section
******************************************************************************/
static UsbDfuResponse_t dfuResponse;
static bool detachReceived = false;

/******************************************************************************
                   Implementations section
******************************************************************************/
/**************************************************************************//**
\brief EEPROM write callback.
******************************************************************************/
void dfuResetAction(void)
{
  HAL_WarmReset();
}
/**************************************************************************//**
\brief DFU detach request handler.
******************************************************************************/
static void dfuDetach(void)
{
  uint8_t bootloaderFlag = 0x5A;
  HAL_EepromParams_t eepromParams = {
    .address = (FLASH_BOOTLOADER_PAGES + FLASH_EEPROM_PAGES) * IFLASH_BYTES_PER_PAGE -
               BOOTLOADER_FLAG_FLASH_OFFSET,
    .data = &bootloaderFlag,
    .length = 1
  };
  uint16_t page;

#if defined(AT91SAM7X256) || defined(AT91SAM3S4C)
  // Acknowledge the request
  sendZLP();
#endif

  detachReceived = true;

  halFlashPrepareForWrite();
  page = halFlashPreparePage(&eepromParams);
  halFlashWritePage(page, false);
  halFlashRestoreFromWrite();

  HAL_RegisterEndOfBusResetHandler(dfuResetAction);
}
/**************************************************************************//**
\brief DFU get status request handler.
******************************************************************************/
static void dfuGetStatus(void)
{
  dfuResponse.getStatusResponse.bStatus = DFU_STATUS_OK;
  dfuResponse.getStatusResponse.bPollTimeout[0] = DFU_STATUS_MAX_POLL_TIMEOUT;
  dfuResponse.getStatusResponse.bState = detachReceived;
  dfuResponse.getStatusResponse.iString = DFU_STATUS_NO_STRING;

  HAL_UsbWrite(0, (void *)&dfuResponse, sizeof(DfuGetStatusResponse_t), NULL, NULL);
}

/**************************************************************************//**
\brief DFU get state request handler.
******************************************************************************/
static void dfuGetState(void)
{
  dfuResponse.getStateResponse.bState = detachReceived;

  HAL_UsbWrite(0, (void *)&dfuResponse, sizeof(DfuGetStateResponse_t), NULL, NULL);
}
/**************************************************************************//**
\brief Device firmware upgrade request handler.
\param[in]
  data - pointer to host's request.
******************************************************************************/
void dfuRequestHandler(uint8_t *data)
{
  UsbDfuRequest_t *pRequest = NULL;

  pRequest = (UsbDfuRequest_t *)data;
  if (NULL == pRequest)
    return;

  // Check request code
  switch (pRequest->request.bRequest)
  {
    case DFU_DETACH:
      dfuDetach();
      break;
    case DFU_GETSTATUS:
      dfuGetStatus();
      break;
    case DFU_GETSTATE:
      dfuGetState();
      break;
    default:
      HAL_Stall(0);
      break;
  }
}

//eof dfuProtocol.c
