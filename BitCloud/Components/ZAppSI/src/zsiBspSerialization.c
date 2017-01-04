/***************************************************************************//**
  \file zsiBspSerialization.c

  \brief ZAppSI BSP APIs serialization.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   Last change:
    $Id: zsiBspSerialization.c 24641 2013-03-22 14:26:58Z unithra.c $
 ******************************************************************************/

/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <zsiDriver.h>
#include <zsiBspSerialization.h>
#include <zsiSerializer.h>
#include <bspUid.h>

/******************************************************************************
                               Implementation section
 ******************************************************************************/
#ifdef ZAPPSI_HOST
/**************************************************************************//**
  \brief BSP read uid serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeBSP_ReadUidReq(const void *const req,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = true,
    .keepMemory = true
  };
  uint16_t length = zsiBSP_ReadUidReqLength();
  uint8_t sequenceNumber = zsiGetSequenceNumber();

  zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_SREQ_CMD, ZSI_CMD_BSP,
    ZSI_BSP_READ_UID_REQUEST);
  (void)req;
  return result;
}
/**************************************************************************//**
  \brief BSP_ReadUid confirm deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
******************************************************************************/
ZsiProcessingResult_t zsiDeserializeBSP_ReadUidConf(void *memory,ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = false,
    .keepMemory = true
  };
  ZsiSerializer_t serializer =
  {
    .pow = cmdFrame->payload,
    .por = cmdFrame->payload
  };

  zsiDeserializeUint64(&serializer, (uint64_t *)memory);

  return result;
}
#elif defined(ZAPPSI_NP)
/**************************************************************************//**
  \brief BSP_ReadUid Request deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

    \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeBSP_ReadUidReq(void *memory, ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = true,
    .keepMemory = false
  };
  ZsiSerializer_t serializer =
  {
    .pow = cmdFrame->payload,
    .por = cmdFrame->payload
  };

  ExtAddr_t uid;
  BSP_ReadUid(&uid); //Will read the UID from chip

  // Prepare and send confirm frame at the same buffer, as request
  {
    uint16_t length = zsiBSP_ReadUidConf(NULL);
    uint8_t sequenceNumber = cmdFrame->sequenceNumber;

    zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_SRSP_CMD, ZSI_CMD_BSP,
                      ZSI_BSP_READ_UID_CONFIRM);

    zsiSerializeUint64(&serializer, &uid);
  }

  (void)memory;

  return result;
}
#endif // ZAPPSI_HOST
/* eof zsiBspSerialization.c */