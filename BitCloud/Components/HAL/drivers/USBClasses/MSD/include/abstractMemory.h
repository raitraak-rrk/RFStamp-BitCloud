/****************************************************************************//**
  \file abstactMemory.h

  \brief Declaration of abstract memory commands.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    29/08/11 N. Fomin - Created
*******************************************************************************/
#ifndef _ABSTRACT_MEMORY_H
#define _ABSTRACT_MEMORY_H

/******************************************************************************
                   Includes section
******************************************************************************/
#include <sysTypes.h>
#include <hsmci.h>

/******************************************************************************
                   Types section
******************************************************************************/
typedef enum
{
  memorySuccessStatus,
  memoryErrorStatus
} MemoryStatus_t;

/******************************************************************************
                   Prototypes section
******************************************************************************/
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
MemoryStatus_t absMemWrite(HAL_HsmciDescriptor_t *descriptor, uint32_t address, void (*callback)(MemoryStatus_t));

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
MemoryStatus_t absMemRead(HAL_HsmciDescriptor_t *descriptor, uint32_t address, void (*callback)(MemoryStatus_t));

/**************************************************************************//**
\brief Performs memory initialization.
\param[in]
  descriptor - pointer to hsmci descriptor.
\return
  status of initialization procedure.
******************************************************************************/
MemoryStatus_t absMemInit(HAL_HsmciDescriptor_t *descriptor);

/**************************************************************************//**
\brief Reads memory capacity.
\param[in]
  descriptor - pointer to hsmci descriptor.
\param[out]
  lastBlockNumber - number of last accessible block of memory.
\return
  status of read capacity procedure.
******************************************************************************/
MemoryStatus_t absMemCapacity(HAL_HsmciDescriptor_t *descriptor, uint32_t *lastBlockNumber);

#endif /* _ABSTRACT_MEMORY_H */
// eof abstactMemory.h
