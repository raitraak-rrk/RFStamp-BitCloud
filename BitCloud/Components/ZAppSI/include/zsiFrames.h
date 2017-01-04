/**************************************************************************//**
  \file zsiFrames.h

  \brief ZAppSI frames format description

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2011-08-25  A. Razinkov - Created.
   Last change:
    $Id: zsiFrames.h 27025 2014-08-05 05:08:54Z unithra.c $
 ******************************************************************************/

#ifndef _ZSIFRAMES_H
#define _ZSIFRAMES_H

/*****************************************************************************
                              Includes section
******************************************************************************/
#include <sysTypes.h>
#include <zsiCommandId.h>

/*****************************************************************************
                              Definitions section
******************************************************************************/
/* ZAppSI start-of-frame sequence. */
#define ZSI_SOF_SEQUENCE 0x2AU
/* Maximum ZAppSI frame payload size */
#define ZSI_MAX_FRAME_PAYLOAD 256U  // Last measured value is 220 bytes for WSNDemo with STDLink security
/* ZAppSI command frame preamble size - SOF, FRAME_CONTROL and LENGTH fields. */
#define ZSI_COMMAND_FRAME_PREAMBLE_SIZE 4U
/* ZAppSI command frame fields size excluding SOF, LENGTH and PAYLOAD fields.
   FRAME_SEQ_NUM + COMMAND_HEADER + FCS */
#define ZSI_COMMAND_FRAME_OVERHEAD 4U

/* ZAppSI frame control field description.
   Bits 0-3 determine transmission status, bits 4-5 are reserved
   and bits 6-7 determine ZAppSI command frame types. */

/* Remote device received a corrupted frame. */
#define ZSI_INVALID_FCS_STATUS      (1U << 0U)
/* Remote device have no memory for current frame processing. */
#define ZSI_OVERFLOW_STATUS         (1U << 1U)
/* Received frame is a part of a larger one.  */
#define ZSI_FRAGMENTED_FRAME_STATUS (1U << 2U)
/* There is one more frame pending on the remote side. */
#define ZSI_FRAME_PENDING_STATUS    (1U << 3U)
/* Remote device received frame successfully */
#define ZSI_NO_ERROR_STATUS         ((0U << 0U) & (0U << 1U))

/* Bits 4-5 are reserved for future use. */

/* Command is a syncronous request, one which requires immediate response.
   For example function wich returnes int value.*/
#define ZSI_SREQ_CMD (1U << 6U)
/* Command is an asyncronous request, one which requires related callback, or
   function, returning void. */
#define ZSI_AREQ_CMD (1U << 7U)
/* Command is a syncronous response on relative syncronous request. */
#define ZSI_SRSP_CMD ((1U << 6U) | (1U << 7U))
/* Command is an acknowledgement on SREQ, AREQ or SRSP. */
#define ZSI_ACK_CMD ((0U << 6U) | (0U << 7U))

#define ZSI_CMD_TYPE_FIELD_MASK ((1U << 6U) | (1U << 7U))
#define ZSI_STATUS_FIELD_MASK   ((1U << 0U) | (1U << 1U))
#define IS_SREQ_CMD_FRAME(cmdFrame) \
  (((cmdFrame)->frameControl & ZSI_CMD_TYPE_FIELD_MASK) == ZSI_SREQ_CMD)
#define IS_AREQ_CMD_FRAME(cmdFrame) \
  (((cmdFrame)->frameControl & ZSI_CMD_TYPE_FIELD_MASK) == ZSI_AREQ_CMD)
#define IS_SRSP_CMD_FRAME(cmdFrame) \
  (((cmdFrame)->frameControl & ZSI_CMD_TYPE_FIELD_MASK) == ZSI_SRSP_CMD)
#define IS_ACK_CMD_FRAME(cmdFrame) \
  (((cmdFrame)->frameControl & ZSI_CMD_TYPE_FIELD_MASK) == ZSI_ACK_CMD)
#define IS_NO_ERROR_STATUS(ackFrame) \
  (((ackFrame)->frameControl & ZSI_STATUS_FIELD_MASK) == ZSI_NO_ERROR_STATUS)
#define IS_INVALID_FCS_STATUS(ackFrame) \
  (((ackFrame)->frameControl & ZSI_STATUS_FIELD_MASK) == ZSI_INVALID_FCS_STATUS)
#define IS_OVERFLOW_STATUS(ackFrame) \
  (((ackFrame)->frameControl & ZSI_STATUS_FIELD_MASK) == ZSI_OVERFLOW_STATUS)

/* ZAppSI command domains. */
#define ZSI_CMD_SYS 0x00U
#define ZSI_CMD_APS 0x01U
#define ZSI_CMD_ZDO 0x02U
#define ZSI_CMD_ZDP 0x03U
#define ZSI_CMD_NWK 0x04U
#define ZSI_CMD_MAC 0x05U
#define ZSI_CMD_HAL 0x06U
#define ZSI_CMD_SEC 0x07U
#define ZSI_CMD_KE  0x08U
#define ZSI_CMD_BSP 0x09U

/*****************************************************************************
                              Types section
******************************************************************************/
BEGIN_PACK

/* ZAppSI command information field format. */
typedef struct PACK _ZsiCommandHeader_t
{
  /* ZAppSI command domain. */
  uint8_t              domain;
  /* Command ID whithin the particular domain. */
  uint8_t              commandId;
} ZsiCommandHeader_t;

/* ZAppSI general command frame format. */
typedef struct PACK _ZsiCommandFrame_t
{
  /* Start-of-frame sequence. */
  uint8_t                   sof;
  /* Frame control field. Keeps channel layer statuses and control information. */
  uint8_t                   frameControl;
  /* Frame length in bytes. */
  uint16_t                  length;
  /* Frame sequence number for correct frame identification. */
  uint8_t                   sequenceNumber;
  /* Command header field. */
  ZsiCommandHeader_t        commandHeader;
  /* Frame payload. */
  uint8_t                   payload[ZSI_MAX_FRAME_PAYLOAD];
  /* Frame check sequence. Only for marking purposes, direct access denied. */
  uint8_t                   fcs;
} ZsiCommandFrame_t;

/* ZAppSI ACK frame format. */
typedef struct PACK _ZsiAckFrame_t
{
  /* Start-of-frame sequence. */
  uint8_t                   sof;
  /* Frame control field. Keeps channel layer statuses and control information. */
  uint8_t                   frameControl;
  /* Frame length in bytes. */
  uint16_t                  length;
  /* Frame sequence number for correct frame identification. */
  uint8_t                   sequenceNumber;
  /* Frame check sequence. */
  uint8_t                   fcs;
} ZsiAckFrame_t;

END_PACK

/******************************************************************************
                    Global functions section
******************************************************************************/
/******************************************************************************
  \brief Returns actual frame length.

  \param[in] cmdFrame - frame, which keeps serialized data.

  \return actual frame length.

 ******************************************************************************/
INLINE uint16_t zsiActualFrameLength(void *const frame)
{
  return ZSI_COMMAND_FRAME_PREAMBLE_SIZE +
         LE16_TO_CPU(((ZsiCommandFrame_t *)frame)->length);
}

#endif /* _ZSIFRAMES_H */
/* eof zsiFrames.h */
