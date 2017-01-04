/***************************************************************************//**
  \file zsiMemoryManager.c

  \brief ZAppSI memory manager implementation.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2011-08-31  A. Razinkov - Created.
   Last change:
    $Id: zsiMemoryManager.c 24441 2013-02-07 12:02:29Z akhromykh $
 ******************************************************************************/

/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <zsiMemoryManager.h>
#include <zsiMem.h>

/******************************************************************************
                              Implementations section
******************************************************************************/
/******************************************************************************
  \brief ZAppSI memory manager reset routine.
 ******************************************************************************/
void zsiResetMemoryManager(void)
{
  uint8_t it;

  memset(zsiMemoryManager(), 0x00, sizeof(ZsiMemoryManager_t));
  INIT_GUARDS(&zsiMemoryManager()->memoryPool);

  for (it = 0U; it < ZSI_MEMORY_BUFFERS_AMOUNT; it++)
  {
    INIT_GUARDS(&zsiMemoryManager()->memoryPool.buffers[it]);
  }
}

/******************************************************************************
  \brief Allocates memory for ZAppSI frames and BitCloud objects.

  \param[in] memory - allocated memory to free.

  \return None.

  \return Pointer to memory.
 ******************************************************************************/
void *zsiAllocateMemory(uint8_t memoryType)
{
  ZsiMemoryBuffer_t *buffers = zsiMemoryManager()->memoryPool.buffers;
  bool allocate = false;
  void *memory = NULL;
  uint8_t item;

  CHECK_GUARDS(&zsiMemoryManager()->memoryPool,
               ZSIMEMORYMANAGER_MEMORYCORRUPTION0);

  for (item = 0U; item < ZSI_MEMORY_BUFFERS_AMOUNT; item++)
  {
    CHECK_GUARDS(&buffers[item], ZSIMEMORYMANAGER_MEMORYCORRUPTION2);
  }

  if (ZSI_RX_ACK_MEMORY == memoryType)
    return &zsiMemoryManager()->memoryPool.ackRxFrame;

  if (ZSI_TX_ACK_MEMORY == memoryType)
    return &zsiMemoryManager()->memoryPool.ackTxFrame;

  /* Allocate memory for SREQ in reserved buffers. */
  if ((ZSI_SREQ_CMD == memoryType) || (ZSI_SRSP_CMD == memoryType))
  {
    /* We have duplicated SREQ or SRSP, if reserved buffers are busy */
    sysAssert(!buffers[ZSI_SYNC0_RESERVED_MEMORY_BUFFER].busy ||
           !buffers[ZSI_SYNC1_RESERVED_MEMORY_BUFFER].busy, ZSIMEMORYMANAGER_ZSIALLOCATEMORY0);
    allocate = true;
    item = ZSI_SYNC0_RESERVED_MEMORY_BUFFER;
    if (buffers[item].busy)
      item = ZSI_SYNC1_RESERVED_MEMORY_BUFFER;
  }
  else if (ZSI_AREQ_CMD == memoryType || ZSI_MUTUAL_MEMORY == memoryType)
  {
    for (item = ZSI_MUTUAL_MEMORY_BUFFERS_START; item < ZSI_MEMORY_BUFFERS_AMOUNT; item++)
    {
      if (!buffers[item].busy)
      {
        allocate = true;
        break;
      }
    }
  }

  if (allocate)
  {
    memory = &buffers[item].memory;
    buffers[item].busy = true;
  }

  return memory;
}

/******************************************************************************
  \brief Free memory allocated for ZAppSI command frames and BitCloud
         primitives.

  \param[in] memory - allocated memory to free.

  \return None.
 ******************************************************************************/
void zsiFreeMemory(const void *const memory)
{
  ZsiMemoryBuffer_t *buffers = zsiMemoryManager()->memoryPool.buffers;
  uint8_t it;

  CHECK_GUARDS(&zsiMemoryManager()->memoryPool,
               ZSIMEMORYMANAGER_MEMORYCORRUPTION1);

  if ((memory == &zsiMemoryManager()->memoryPool.ackTxFrame) || (memory == &zsiMemoryManager()->memoryPool.ackRxFrame))
    return;

  for (it = 0U; it < ZSI_MEMORY_BUFFERS_AMOUNT; it++)
  {
    CHECK_GUARDS(&buffers[it], ZSIMEMORYMANAGER_MEMORYCORRUPTION3);
    /* Locate and free memory buffer which keeps appropriate memory */
    if (((void *)&buffers[it] <= memory) && (memory < (void *)&buffers[it + 1U]))
    {
      buffers[it].busy = false;
      zsiMemoryAvailableNtfy();
      return;
    }
  }
  /* If we reach this point - invalid memory location was passed */
  sysAssert(0U, ZSIMEMORYMANAGER_ZSIFREEMEMORY0);
}

/******************************************************************************
  \brief Checks if free memory buffer is available.

  \return True, if free memory buffer is available, false - otherwise.
 ******************************************************************************/
bool zsiIsMemoryAvailable(void)
{
  ZsiMemoryBuffer_t *buffers = zsiMemoryManager()->memoryPool.buffers;
  uint8_t it;

  for (it = ZSI_MUTUAL_MEMORY_BUFFERS_START; it < ZSI_MEMORY_BUFFERS_AMOUNT; it++)
  {
    if (!buffers[it].busy)
    {
      return true;
    }
  }
  return false;
}

/* eof zsiMemoryManager.c */
