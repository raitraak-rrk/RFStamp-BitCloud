/**************************************************************************//**
  \file macBuffers.h

  \brief MAC and PHY buffers' description.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    09/10/07 A. Luzhetsky - Created.
******************************************************************************/

#ifndef _MACBUFFERS_H
#define _MACBUFFERS_H

/******************************************************************************
                    Includes section
******************************************************************************/
#include <inttypes.h>
#include <sysTypes.h>
#include <phy.h>
#include <macphyPib.h>

/******************************************************************************
                    Define(s) section
******************************************************************************/
//! Size of internal reception buffer.
#define FRAME_RX_BUFFER_SIZE (PHY_FRAME_LENGTH_FIELD_SIZE + \
                              PHY_MAX_PACKET_SIZE + \
                              PHY_LQI_FIELD_SIZE + \
                              PHY_RSSI_FIELD_SIZE)

/******************************************************************************
                    Types section
******************************************************************************/
typedef enum
{
  FRAME_RX_BUFFER_TYPE_DATA,
#if defined(_FFD_)
  FRAME_RX_BUFFER_TYPE_DATA_REQ,
#endif // defined(_FFD_)
  FRAME_RX_BUFFER_TYPES_AMOUNT
} FrameRxBufferType_t;

//! Rx frame buffer state descriptor.
typedef struct
{
  uint8_t *buffer;
  uint8_t *reserved;
  uint8_t *gap1b;
  uint8_t *gap1e;
  uint8_t *gap2b;
  uint8_t *gap2e;
} FrameRxBufferState_t;

#endif // _MACBUFFERS_H

// eof macBuffers.h
