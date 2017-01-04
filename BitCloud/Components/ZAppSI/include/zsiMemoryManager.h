/**************************************************************************//**
  \file zsiMemoryManager.h

  \brief ZAppSI memory manager interface

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2011-08-31  A. Razinkov - Created.
   Last change:
    $Id: zsiMemoryManager.h 24441 2013-02-07 12:02:29Z akhromykh $
 ******************************************************************************/

#ifndef _ZSIMEMORYMANAGER_H
#define _ZSIMEMORYMANAGER_H

/*****************************************************************************
                              Includes section
******************************************************************************/
#include <zsiFrames.h>
#include <zsiDbg.h>
#include <sysQueue.h>
#include <zdo.h>
#include <zsiSysSerialization.h>
#include <zsiKeSerialization.h>

/******************************************************************************
                              Defines section
******************************************************************************/
#ifndef BOARD_PC
#define ZSI_MEMORY_BUFFERS_AMOUNT 6U
#else
#define ZSI_MEMORY_BUFFERS_AMOUNT 128U
#endif
#if ZSI_MEMORY_BUFFERS_AMOUNT < 6U
#error "Not enough ZAppSI memory buffers: 6 or more required! "
#endif

/* Indexes of memory buffers reserved for SREQ/SRSP processing */
#define ZSI_SYNC0_RESERVED_MEMORY_BUFFER 0U
#define ZSI_SYNC1_RESERVED_MEMORY_BUFFER 1U
/* Mutual memory buffers start index */
#define ZSI_MUTUAL_MEMORY_BUFFERS_START  2U
/* Mutual memory marker */
#define ZSI_MUTUAL_MEMORY 0x2AU
#define ZSI_RX_ACK_MEMORY 0x2BU
#define ZSI_TX_ACK_MEMORY 0x2CU

/******************************************************************************
                              Types section
******************************************************************************/
typedef struct _ZsiMemoryBuffer_t
{
  QueueElement_t next;
  bool busy;
  TOP_GUARD
  union
  {
    uint8_t                         memory;
    ZsiCommandFrame_t               commandFrame;
    ZDO_StartNetworkReq_t           zdoStartNetworkReq;
    ZsiCsParameter_t                csParameter;
    ZDO_MgmtNwkUpdateNotf_t         zdoNwkUpdateNtfy;
    APS_DataInd_t                   apsDataInd;
    APS_DataReq_t                   apsDataReq;
    ZDO_ZdpReq_t                    zdpReq;
#if CERTICOM_SUPPORT == 1    
    ZsiEccKeyBitGenerate_t          zsiEccKeyBitGenerate;
    ZsiEccGenerateKey_t             zsiEccGenerateKey;
#endif    
  };
  BOTTOM_GUARD
} ZsiMemoryBuffer_t;

typedef struct _ZsiMemoryPool_t
{
  TOP_GUARD
  ZsiAckFrame_t     ackTxFrame;
  ZsiAckFrame_t     ackRxFrame;
  BOTTOM_GUARD
  ZsiMemoryBuffer_t buffers[ZSI_MEMORY_BUFFERS_AMOUNT];
} ZsiMemoryPool_t;

typedef struct _ZsiMemoryManager_t
{
  ZsiMemoryPool_t memoryPool;
} ZsiMemoryManager_t;

/******************************************************************************
                             Prototypes section
 ******************************************************************************/
/******************************************************************************
  \brief ZAppSI memory manager reset routine.
 ******************************************************************************/
void zsiResetMemoryManager(void);

/******************************************************************************
  \brief Allocates memory for ZAppSI frames and BitCloud primitives.

  \return Pointer to memory.
 ******************************************************************************/
void *zsiAllocateMemory(uint8_t memoryType);

/******************************************************************************
  \brief Free memory allocated for ZAppSI command frames and BitCloud
         primitives.

  \param[in] memory - allocated memory to free.

  \return None.
 ******************************************************************************/
void zsiFreeMemory(const void *const memory);

/******************************************************************************
  \brief Checks if free memory buffer is available.

  \return True, if free memory buffer is available, false - otherwise.
 ******************************************************************************/
bool zsiIsMemoryAvailable(void);

#endif /* _ZSIMEMORYMANAGER_H */
/* eof zsiMemoryManager.h */
