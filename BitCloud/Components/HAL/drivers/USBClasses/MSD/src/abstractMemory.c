/****************************************************************************//**
  \files abstractMemory.c

  \brief Implementation of amstract memory commands.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    30/08/11 N. Fomin - Created
*******************************************************************************/
/******************************************************************************
                   Includes section
******************************************************************************/
#include <mem.h>
#include <abstractMemory.h>
#include <driversDbg.h>
#include <sysAssert.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
#define RETRY_COUNT 5

/******************************************************************************
                   External global variables section
******************************************************************************/
/******************************************************************************
                   Types section
******************************************************************************/
typedef struct
{
  uint16_t transferLength;
  uint16_t blocksToTransfer;
  uint32_t logicalBlockAddress;
  HAL_HsmciDescriptor_t *hsmciDescriptor;
  uint8_t *dataBuffer;
} AbsMemInternal_t;

/******************************************************************************
                   Global variables section
******************************************************************************/
static void (*absMemCallback)(MemoryStatus_t status);
static AbsMemInternal_t apsMemInternal;

/******************************************************************************
                   Implementations section
******************************************************************************/
/**************************************************************************//**
\brief Current memory read callback.
\param[in]
  status - status of read operation.
******************************************************************************/
void absMemReadBlockCallback(MemStatus_t status)
{
  uint8_t i = RETRY_COUNT;
  apsMemInternal.blocksToTransfer--;
  uint16_t blocksRemaining = apsMemInternal.transferLength - apsMemInternal.blocksToTransfer;
  HAL_HsmciDescriptor_t *descriptor = apsMemInternal.hsmciDescriptor;

  if (memSuccess != status)
  {
    SYS_E_ASSERT_FATAL(absMemCallback, MSD_NULLCALLBACK0);
    absMemCallback(memoryErrorStatus);
  }
  if (0 == apsMemInternal.blocksToTransfer)
  {
    descriptor->dataTransferDescriptor->buffer = apsMemInternal.dataBuffer;
    SYS_E_ASSERT_FATAL(absMemCallback, MSD_NULLCALLBACK1);
    absMemCallback(memorySuccessStatus);
    return;
  }

  descriptor->dataTransferDescriptor->buffer += blocksRemaining * STANDARD_BLOCK_LENGTH;

  while(i--)
  {
    if (memSuccess == memReadBlock(descriptor, apsMemInternal.logicalBlockAddress + blocksRemaining, absMemReadBlockCallback))
      break;
  }
  if (0 == i)
  {
    SYS_E_ASSERT_FATAL(absMemCallback, MSD_NULLCALLBACK2);
    absMemCallback(memoryErrorStatus);
  }
}

/**************************************************************************//**
\brief Current memory write callback.
\param[in]
  status - status of write operation.
******************************************************************************/
void absMemWriteBlockCallback(MemStatus_t status)
{
  uint8_t i = RETRY_COUNT;
  apsMemInternal.blocksToTransfer--;
  uint16_t blocksRemaining = apsMemInternal.transferLength - apsMemInternal.blocksToTransfer;
  HAL_HsmciDescriptor_t *descriptor = apsMemInternal.hsmciDescriptor;

  if (memSuccess != status)
  {
    SYS_E_ASSERT_FATAL(absMemCallback, MSD_NULLCALLBACK3);
    absMemCallback(memoryErrorStatus);
  }
  if (0 == apsMemInternal.blocksToTransfer)
  {
    descriptor->dataTransferDescriptor->buffer = apsMemInternal.dataBuffer;
    SYS_E_ASSERT_FATAL(absMemCallback, MSD_NULLCALLBACK4);
    absMemCallback(memorySuccessStatus);
    return;
  }

  descriptor->dataTransferDescriptor->buffer += blocksRemaining * STANDARD_BLOCK_LENGTH;

  while(i--)
  {
    if (memSuccess == memWriteBlock(descriptor, apsMemInternal.logicalBlockAddress + blocksRemaining, absMemWriteBlockCallback))
      break;
  }
  if (0 == i)
  {
    SYS_E_ASSERT_FATAL(absMemCallback, MSD_NULLCALLBACK5);
    absMemCallback(memoryErrorStatus);
  }
}

/**************************************************************************//**
\brief Performs memory initialization.
\param[in]
  descriptor - pointer to hsmci descriptor.
\return
  status of initialization procedure.
******************************************************************************/
MemoryStatus_t absMemInit(HAL_HsmciDescriptor_t *descriptor)
{
  if (memSuccess != memInit(descriptor))
    return memoryErrorStatus;

  return memorySuccessStatus;
}

/**************************************************************************//**
\brief Reads memory capacity.
\param[in]
  descriptor - pointer to hsmci descriptor.
\param[out]
  lastBlockNumber - number of last accessible block of memory.
\return
  status of read capacity procedure.
******************************************************************************/
MemoryStatus_t absMemCapacity(HAL_HsmciDescriptor_t *descriptor, uint32_t *lastBlockNumber)
{
  MemCapacityInfo_t capInfo;
  uint8_t i = RETRY_COUNT;
  MemStatus_t status;

  while(i--)
  {
    status = memGetCapacityInfo(descriptor, &capInfo);
    if (memSuccess == status)
    {
      *lastBlockNumber = capInfo.lastLogicalBlock;
      return memorySuccessStatus;
    }
  }

  return memoryErrorStatus;
}

/**************************************************************************//**
\brief Writes data to memory at "address".
\param[in]
  descriptor - pointer to hsmci descriptor;
  address - address of blocks to write.
  callback - pointer to function to nofity upper layer about end of write
             procedure.
\return
  status of write procedure.
******************************************************************************/
MemoryStatus_t absMemWrite(HAL_HsmciDescriptor_t *descriptor, uint32_t address, void (*callback)(MemoryStatus_t))
{
  HAL_HsmciDataTransferDescriptor_t *dataDescr = descriptor->dataTransferDescriptor;
  uint8_t i;

  absMemCallback = callback;

  apsMemInternal.logicalBlockAddress = address;
  apsMemInternal.transferLength = dataDescr->length / STANDARD_BLOCK_LENGTH;
  apsMemInternal.blocksToTransfer = apsMemInternal.transferLength;
  apsMemInternal.dataBuffer = descriptor->dataTransferDescriptor->buffer;

  dataDescr->length = STANDARD_BLOCK_LENGTH;

  i = RETRY_COUNT;
  while(i--)
  {
    if (memSuccess == memWriteBlock(descriptor, address, absMemWriteBlockCallback))
      return memorySuccessStatus;
  }

  return memoryErrorStatus;
}

/**************************************************************************//**
\brief Reads data from memory at "address".
\param[in]
  descriptor - pointer to hsmci descriptor;
  address - address of blocks to read.
  callback - pointer to function to nofity upper layer about end of read
             procedure.
\return
  status of read procedure.
******************************************************************************/
MemoryStatus_t absMemRead(HAL_HsmciDescriptor_t *descriptor, uint32_t address, void (*callback)(MemoryStatus_t))
{
  HAL_HsmciDataTransferDescriptor_t *dataDescr = descriptor->dataTransferDescriptor;
  uint8_t i;

  absMemCallback = callback;

  apsMemInternal.logicalBlockAddress = address;
  apsMemInternal.transferLength = dataDescr->length / STANDARD_BLOCK_LENGTH;
  apsMemInternal.blocksToTransfer = apsMemInternal.transferLength;
  apsMemInternal.dataBuffer = descriptor->dataTransferDescriptor->buffer;

  dataDescr->length = STANDARD_BLOCK_LENGTH;

  i = RETRY_COUNT;
  while(i--)
  {
    if (memSuccess == memReadBlock(descriptor, address, absMemReadBlockCallback))
      return memorySuccessStatus;
  }

  return memoryErrorStatus;
}

//eof abstractMemory.c
