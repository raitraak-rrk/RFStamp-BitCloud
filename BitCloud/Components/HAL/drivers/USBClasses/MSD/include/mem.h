/****************************************************************************//**
  \file mem.h

  \brief Declaration of memory commands.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    29/08/11 N. Fomin - Created
*******************************************************************************/
#ifndef _MEM_H
#define _MEM_H

/******************************************************************************
                   Includes section
******************************************************************************/
#include <sysTypes.h>
#include <hsmci.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
// block length
#define STANDARD_BLOCK_LENGTH 512

/******************************************************************************
                   Types section
******************************************************************************/
typedef enum
{
  memSuccess,
  hsmciError,
  voltageError,
  commandError,
  initTimeoutError,
  stateError,
  hsmciReadError,
  hsmciWriteError,
  setMaxFreqError
} MemStatus_t;

typedef struct
{
  uint32_t lastLogicalBlock;
  uint32_t logicalBlockLength;
} MemCapacityInfo_t;

/******************************************************************************
                   Prototypes section
******************************************************************************/
/**************************************************************************//**
\brief Writes one data block to memory at "address".
\param[in]
  descriptor - pointer to hsmci descriptor;
  address - address of block to write.
\return
  status of write procedure.
******************************************************************************/
MemStatus_t memWriteBlock(HAL_HsmciDescriptor_t *descriptor, uint32_t address, void (*callback)(MemStatus_t));

/**************************************************************************//**
\brief Reads one data block from memory at "address".
\param[in]
  descriptor - pointer to hsmci descriptor;
  address - address of block to read.
\return
  status of read procedure.
******************************************************************************/
MemStatus_t memReadBlock(HAL_HsmciDescriptor_t *descriptor, uint32_t address, void (*callback)(MemStatus_t));

/**************************************************************************//**
\brief Performs memory initialization.
\param[in]
  commandDescr - pointer to hsmci command descriptor.
\return
  status of initialization procedure.
******************************************************************************/
MemStatus_t memInit(HAL_HsmciDescriptor_t *commandDescr);

/**************************************************************************//**
\brief Reads memory capacity.
\param[in]
  descriptor - pointer to hsmci descriptor.
\param[out]
  capInfo - pointer to memory capacity structure.
\return
  status of read capacity procedure.
******************************************************************************/
MemStatus_t memGetCapacityInfo(HAL_HsmciDescriptor_t *descriptor, MemCapacityInfo_t *capInfo);

/**************************************************************************//**
\brief Checks if memory is ready for any data transfer.
\return
  false - memory is busy;
  true - memory is ready.
******************************************************************************/
bool memIsBusy(void);

/**************************************************************************//**
\brief Gets card unique serial number.
\return
  card serial number.
******************************************************************************/
uint32_t memCardSerialNumber(void);

#endif /* _MEM_H */
// eof mem.h
