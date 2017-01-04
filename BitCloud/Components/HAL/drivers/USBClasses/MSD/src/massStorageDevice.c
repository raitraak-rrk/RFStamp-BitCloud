/****************************************************************************//**
  \file massStorageDevice.c

  \brief Implementation of mass storage API.

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
#include <usbEnumeration.h>
#include <massStorageDevice.h>
#include <scsiProtocol.h>
#include <abstractMemory.h>
#include <usbDescriptors.h>
#include <mem.h>
#include <usbEnumeration.h>

/******************************************************************************
                   Defines section
******************************************************************************/
#define CBW_SIZE 31
#define CBW_SIGNATURE 0x43425355

#define CSW_SIZE 13
#define CSW_COMMAND_FAILED_STATUS  1
#define CSW_COMMAND_SUCCESS_STATUS 0

/******************************************************************************
                   Types section
******************************************************************************/
typedef enum
{
  MSD_COMMAND_TRANSPORT,
  MSD_SCSI_ANSWER,
  MSD_DATA_INOUT,
  MSD_STATUS_TRANSPORT
} MsdState_t;

typedef struct
{
  uint32_t bufferOffset;
  uint16_t buffersToTransfer;
  uint32_t blockAddress;
} MsdReadWriteControl_t;

/******************************************************************************
                   Prototypes section
******************************************************************************/
static void msdBufferReadCallback(MemoryStatus_t status);
static void msdBufferWriteCallback(MemoryStatus_t status);

/******************************************************************************
                   External global variables section
******************************************************************************/
static UsbMscRequest_t request;

/******************************************************************************
                   Local variables section
******************************************************************************/
// pointer to application hsmci descriptor
static HAL_HsmciDescriptor_t msdDescr;
HAL_HsmciDescriptor_t *msdPointDescr;
static HAL_HsmciCommandDescriptor_t commandDescr;
static HAL_HsmciDataTransferDescriptor_t dataTransferDescr;
static MsdReadWriteControl_t rwControl;
static MscCSW_t csw;
static MsdState_t msdState;
static MSD_Callback_t msdCallback = NULL;

void msdRcvCallback(void *pArg, uint8_t status, uint16_t transferred, uint16_t remaining);

/******************************************************************************
                   Implementations section
******************************************************************************/
/**************************************************************************//**
\brief Transmitting callback of usb.
\param[in]
  pArg - pointer to some data;
  status - result of the USB transfer.;
  transferred - how much data are transferred;
  remaining - how much data are not transferred.
******************************************************************************/
void msdTmtCallback(void *pArg, uint8_t status, uint16_t transferred, uint16_t remaining)
{
  (void)pArg;
  (void)status;
  (void)remaining;
  (void)transferred;

  if (MSD_DATA_INOUT == msdState)
  {
    rwControl.buffersToTransfer--;
    rwControl.blockAddress += msdDescr.dataTransferDescriptor->length / STANDARD_BLOCK_LENGTH;
    if (0 == rwControl.buffersToTransfer)
    {
      csw.bCSWStatus = CSW_COMMAND_SUCCESS_STATUS;
      msdState = MSD_STATUS_TRANSPORT;
      memcpy(msdDescr.dataTransferDescriptor->buffer, (uint8_t*)&csw, CSW_SIZE);
      HAL_UsbWrite(MSD_TRANSMIT_PIPE, msdDescr.dataTransferDescriptor->buffer, CSW_SIZE, msdTmtCallback, NULL);
      return;
    }
    else
      if (memorySuccessStatus != absMemRead(&msdDescr, rwControl.blockAddress, msdBufferReadCallback))
        msdCallback(MSD_READ_ERROR);

  }
  if (MSD_SCSI_ANSWER == msdState)
  {
    csw.bCSWStatus = CSW_COMMAND_SUCCESS_STATUS;
    msdState = MSD_STATUS_TRANSPORT;
    memcpy(msdDescr.dataTransferDescriptor->buffer, (uint8_t*)&csw, CSW_SIZE);
    HAL_UsbWrite(MSD_TRANSMIT_PIPE, msdDescr.dataTransferDescriptor->buffer, CSW_SIZE, msdTmtCallback, NULL);
    return;
  }
  if (MSD_STATUS_TRANSPORT == msdState)
  {
    if (msdCallback)
      msdCallback(MSD_STATUS_SUCCESS);
    msdState = MSD_COMMAND_TRANSPORT;
    HAL_UsbRead(MSD_RECEIVE_PIPE, msdDescr.dataTransferDescriptor->buffer, CBW_SIZE, msdRcvCallback, NULL);
    return;
  }
}

/**************************************************************************//**
\brief Receiving callback of usb.
\param[in]
  pArg - pointer to some data;
  status - result of the USB transfer.;
  transferred - how much data are transferred;
  remaining - how much data are not transferred.
******************************************************************************/
void msdRcvCallback(void *pArg, uint8_t status, uint16_t transferred, uint16_t remaining)
{
  bool validAndMeaningful = true;
  MscCBW_t *cbw = (MscCBW_t *)(msdDescr.dataTransferDescriptor->buffer);
  uint8_t length;

  (void)pArg;
  (void)status;
  (void)remaining;

  if (MSD_COMMAND_TRANSPORT == msdState)
  {
    /* check that CBW is valid */
    if ((CBW_SIZE != transferred) || (CBW_SIGNATURE != cbw->dCBWSignature))
      validAndMeaningful = false;
    if (!validAndMeaningful)
    {
      HAL_Stall(ADDRESS_MSC_BULKIN_PIPE & 0x03);
      return;
    }
    /* check that CBW is meaningful */
    if ((cbw->bCBWLUN > 0x0F) || (cbw->bCBWCBLength == 0) || (cbw->bCBWCBLength > 0x10) || (cbw->bmCBWFlags & 0x7F))
      validAndMeaningful = false;
    if (!validAndMeaningful)
      return;
    /* fill csw with parameters from cbw */
    csw.cDSWTag = cbw->cDBWTag;
    csw.dCSWSignature = CBW_SIGNATURE;
    /* check that command is valid */
    if (!scsiIsValidCommand(cbw->CBWCB))
    {
      csw.bCSWStatus = CSW_COMMAND_FAILED_STATUS;
      msdState = MSD_STATUS_TRANSPORT;
      memcpy(msdDescr.dataTransferDescriptor->buffer, (uint8_t*)&csw, CSW_SIZE);
      HAL_UsbWrite(MSD_TRANSMIT_PIPE, msdDescr.dataTransferDescriptor->buffer, CSW_SIZE, msdTmtCallback, NULL);
      return;
    }
    /* check that no data in-out phase is needed */
    if (!scsiIsDataInOutPhaseNeeded(cbw->CBWCB))
    {
      csw.bCSWStatus = CSW_COMMAND_SUCCESS_STATUS;
      msdState = MSD_STATUS_TRANSPORT;
      memcpy(msdDescr.dataTransferDescriptor->buffer, (uint8_t*)&csw, CSW_SIZE);
      HAL_UsbWrite(MSD_TRANSMIT_PIPE, msdDescr.dataTransferDescriptor->buffer, CSW_SIZE, msdTmtCallback, NULL);
      return;
    }
    /* check that command is no read-write command */
    if (!scsiIsReadWriteCommand(cbw->CBWCB))
    {
      csw.bCSWStatus = CSW_COMMAND_SUCCESS_STATUS;
      msdState = MSD_SCSI_ANSWER;
      length = scsiGetCommandResponse(cbw->CBWCB, msdDescr.dataTransferDescriptor->buffer);
      HAL_UsbWrite(MSD_TRANSMIT_PIPE, msdDescr.dataTransferDescriptor->buffer, length, msdTmtCallback, NULL);
      return;
    }
    /* check that command is read command */
    rwControl.buffersToTransfer = scsiBlocksAmount(cbw->CBWCB) * (msdDescr.dataTransferDescriptor->length / STANDARD_BLOCK_LENGTH);
    rwControl.blockAddress = scsiGetBlockAddress(cbw->CBWCB);
    if (scsiIsReadCommand(cbw->CBWCB))
    {
      if (memorySuccessStatus != absMemRead(&msdDescr, rwControl.blockAddress, msdBufferReadCallback))
        msdCallback(MSD_READ_ERROR);
    }
    else
      HAL_UsbRead(MSD_RECEIVE_PIPE, msdDescr.dataTransferDescriptor->buffer,
                  BULK_SIZE, msdRcvCallback, NULL);
    msdState = MSD_DATA_INOUT;
    return;
  }
  if (MSD_DATA_INOUT == msdState)
  {
    /* increase internal write buffer offset*/
    rwControl.bufferOffset += transferred;
    /* if buffer is full perform write to flash */
    if (rwControl.bufferOffset == msdDescr.dataTransferDescriptor->length)
    {
      if (memorySuccessStatus !=  absMemWrite(&msdDescr, rwControl.blockAddress, msdBufferWriteCallback))
        msdCallback(MSD_WRITE_ERROR);
      /* wait for hsmci bus becomes free */
      rwControl.bufferOffset = 0;
      rwControl.buffersToTransfer--;
      rwControl.blockAddress += msdDescr.dataTransferDescriptor->length / STANDARD_BLOCK_LENGTH;
      return;
    }
    if (0 != rwControl.buffersToTransfer)
      HAL_UsbRead(MSD_RECEIVE_PIPE, msdDescr.dataTransferDescriptor->buffer + rwControl.bufferOffset,
                BULK_SIZE, msdRcvCallback, NULL);

  }
}

/**************************************************************************//**
\brief Opens mass storage device.
\param[in]
  callback - pointer to function to notify about MSD errors and transactions;
  responseBuffer - pointer to hsmci command response buffer; it should have
                   a size of four;
  buffer - pointer to buffer for hsmci data transfer; it should be
           a multiplier of 512;
  length - length of buffer for data transfer.
******************************************************************************/
void MSD_Open(MSD_Callback_t callback, uint32_t *responseBuffer, uint8_t *buffer, uint32_t length)
{
  uint32_t lastBlock;

  msdCallback = callback;

  msdDescr.dataTransferDescriptor = &dataTransferDescr;
  msdDescr.commandDescriptor = &commandDescr;
  msdDescr.dataTransferDescriptor->buffer = buffer;
  msdDescr.dataTransferDescriptor->length = length;
  msdDescr.commandDescriptor->responseBuffer = responseBuffer;
  msdDescr.dataTransferDescriptor->commandDescr = &commandDescr;

  msdPointDescr = & msdDescr;

  if (-1 == HAL_OpenHsmci(&msdDescr))
    if (msdCallback)
      msdCallback(MSD_INTERFACE_BUSY);

  if (memorySuccessStatus != absMemInit(&msdDescr))
    if (msdCallback)
      msdCallback(MSD_MEMORY_INIT_ERROR);

  if (memorySuccessStatus != absMemCapacity(&msdDescr, &lastBlock))
    if (msdCallback)
      msdCallback(MSD_READ_CAPACITY_ERROR);

  usbSetSerialNumber(memCardSerialNumber());

  scsiSetCapacity(lastBlock);

  rwControl.bufferOffset = 0;
  rwControl.buffersToTransfer = 0;
  msdState = MSD_COMMAND_TRANSPORT;
  csw.dCSWDataResidue = 0;

  HAL_RegisterEndOfBusResetHandler(usbBusResetAction);
  if (DEVICE_POWERED != HAL_GetState())
    HAL_UsbInit((uint8_t *)&request);
}

/**************************************************************************//**
\brief Closes mass storage device.
******************************************************************************/
void MSD_Close(void)
{
  if (-1 == HAL_CloseHsmci(&msdDescr))
    if (msdCallback)
      msdCallback(MSD_INTERFACE_BUSY);
}

/**************************************************************************//**
\brief Memory read callback function.
\param[in]
  status - status of memory read procedure.
******************************************************************************/
static void msdBufferReadCallback(MemoryStatus_t status)
{
  if (memorySuccessStatus != status)
    msdCallback(MSD_READ_ERROR);
  HAL_UsbWrite(MSD_TRANSMIT_PIPE, msdDescr.dataTransferDescriptor->buffer,
               msdDescr.dataTransferDescriptor->length, msdTmtCallback, NULL);
}

/**************************************************************************//**
\brief Memory write callback function.
\param[in]
  status - status of memory write procedure.
******************************************************************************/
static void msdBufferWriteCallback(MemoryStatus_t status)
{
  if (memorySuccessStatus != status)
    msdCallback(MSD_WRITE_ERROR);
  if (0 == rwControl.buffersToTransfer)
  {
    csw.bCSWStatus = CSW_COMMAND_SUCCESS_STATUS;
    msdState = MSD_STATUS_TRANSPORT;
    memcpy(msdDescr.dataTransferDescriptor->buffer, (uint8_t*)&csw, CSW_SIZE);
    HAL_UsbWrite(MSD_TRANSMIT_PIPE, msdDescr.dataTransferDescriptor->buffer, CSW_SIZE, msdTmtCallback, NULL);
  }
  else
    HAL_UsbRead(MSD_RECEIVE_PIPE, msdDescr.dataTransferDescriptor->buffer,
              BULK_SIZE, msdRcvCallback, NULL);
}

// eof massStorageDevice.c
