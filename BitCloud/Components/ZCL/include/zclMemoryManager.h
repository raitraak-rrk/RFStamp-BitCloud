/************************************************************************//**
  \file zclMemoryManager.h

  \brief
    The header file describes the ZCL Memory Manager interface

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    27.11.08 A. Potashov - Created.
******************************************************************************/

#ifndef _ZCLMEMORYMANAGER_H
#define _ZCLMEMORYMANAGER_H

/******************************************************************************
                   Includes section
******************************************************************************/
#include <sysTypes.h>
#include <aps.h>
#include <appFramework.h>
#include <macAddr.h>
#include <zcl.h>
#include <dbg.h>

#ifdef ZAPPSI_HOST
#define ZCL_RESPONSE_SPACING 300U
#else // basic application
#define ZCL_RESPONSE_SPACING 0U
#endif
/******************************************************************************
                   Types section
******************************************************************************/
typedef enum
{
  /** Value for free buffer */
  ZCL_UNKNOWN_BUFFER = 0x0,
  /** Output packet from APL component. */
  ZCL_OUTPUT_DATA_BUFFER = 0x1,
  /** Input packet from APS-layer. */
  ZCL_INPUT_DATA_BUFFER = 0x2,
  /** Output packet from ZCL-layer(response). */
  ZCL_OUTPUT_RESPONSE_BUFFER = 0x3,
  /** Output packet from ZCL-layer (report). */
  ZCL_OUTPUT_REPORT_BUFFER = 0x4,
  ZCL_BUFFER_TYPE_LAST
} ZclBufferType_t;

typedef union
{
  APS_DataReq_t apsDataReq;
  APS_DataInd_t apsDataInd;
} ZclMmPrimitive_t;

typedef struct
{
  ZclMmPrimitive_t  primitive;
  uint8_t *frame;
} ZclMmBuffer_t;

typedef struct
{
  uint32_t        timeout;
  ZclBufferType_t type;
  ZCL_Request_t   *link;
  ZclMmBuffer_t   buf;
} ZclMmBufferDescriptor_t;

/******************************************************************************
                   Prototypes section
******************************************************************************/
/*************************************************************************//**
\brief Looks for and returns free zcl memory buffer

\param[in] type - the type of a buffer

\returns pointer to memory buffer or NULL if there is no free buffer
*****************************************************************************/
ZclMmBuffer_t *zclMmGetMem(ZclBufferType_t type);

/*************************************************************************//**
\brief Frees zcl buffer

\param[in] mem - pointer to used zcl buffer
*****************************************************************************/
void zclMmFreeMem(ZclMmBuffer_t *mem);

/*************************************************************************//**
\brief Gets next zcl memory buffer descriptor or
  the first one if mem is NULL with output request

\param[in] descr - pointer to current zcl buffer descriptor

\returns pointer to the next busy zcl memory buffer descriptor
  or NULL if no next busy zcl memory buffer descriptor is available
*****************************************************************************/
ZclMmBufferDescriptor_t *zclMmGetNextOutputMemDescriptor(ZclMmBufferDescriptor_t *descr);

#endif  //#ifndef _ZCLMEMORYMANAGER_H

//eof zclMemoryManager.h
