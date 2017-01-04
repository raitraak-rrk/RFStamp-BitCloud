/***************************************************************************//**
  \file zsiMacSerialization.c

  \brief ZAppSI Mac primitives serialization.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    14.01.2013  A. Khromykh - Created.
   Last change:
    $Id: zsiMacSerialization.c 24443 2013-02-07 14:14:23Z akhromykh $
 ******************************************************************************/

/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <zsiMacSerialization.h>
#include <zsiSerializer.h>
#include <zsiMemoryManager.h>
#include <sysUtils.h>
#include <zsiDbg.h>

/******************************************************************************
                   Local variables section
******************************************************************************/
#ifdef ZAPPSI_HOST

#elif defined(ZAPPSI_NP)
RF_RandomReq_t *zsiRandomReq;
#endif /* ZAPPSI_NP */

/******************************************************************************
                               Implementation section
*******************************************************************************/
#ifdef ZAPPSI_HOST
/**************************************************************************//**
  \brief MAC_IsOwnExtAddrReq primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeMAC_IsOwnExtAddrReq(const void *const req,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = true,
    .keepMemory = true // It is not ZAppSi memory, keep for high layer
  };
  ZsiSerializer_t serializer =
  {
    .pow = cmdFrame->payload,
    .por = cmdFrame->payload
  };

  uint16_t length = zsiMAC_IsOwnExtAddrReq(req);
  uint8_t sequenceNumber = zsiGetSequenceNumber();

  zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_SREQ_CMD, ZSI_CMD_MAC,
                    ZSI_MAC_IS_OWN_EXT_ADDR_REQ);

  zsiSerializeUint64(&serializer, req);

  return result;
}

/**************************************************************************//**
  \brief MAC_IsOwnExtAddrConf confirm primitive deserialization and
         processing routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
*******************************************************************************/
ZsiProcessingResult_t zsiDeserializeMAC_IsOwnExtAddrConf(void *memory,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepMemory = true,
    .keepCmdFrame = false
  };
  ZsiSerializer_t serializer =
  {
    .por = cmdFrame->payload
  };

  zsiDeserializeUint8(&serializer, memory);

  return result;
}

/**************************************************************************//**
  \brief MAC_GetExtAddrReq primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeMAC_GetExtAddrReq(const void *const req,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = true,
    .keepMemory = true
  };

  uint16_t length = zsiMAC_GetExtAddrReq(req);
  uint8_t sequenceNumber = zsiGetSequenceNumber();

  zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_SREQ_CMD, ZSI_CMD_MAC,
                    ZSI_MAC_GET_EXT_ADDR_REQ);

  return result;
}

/**************************************************************************//**
  \brief MAC_GetExtAddrConf confirm primitive deserialization and
         processing routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
*******************************************************************************/
ZsiProcessingResult_t zsiDeserializeMAC_GetExtAddrConf(void *memory,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepMemory = true,
    .keepCmdFrame = false
  };
  ZsiSerializer_t serializer =
  {
    .por = cmdFrame->payload
  };

  zsiDeserializeUint64(&serializer, memory);

  return result;
}

/**************************************************************************//**
  \brief RF_RandomReq primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeRF_RandomReq(const void *const req,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = true,
    .keepMemory = true // It is not ZAppSi memory, keep for high layer
  };
  uint16_t length = zsiRF_RandomReq(req);
  uint8_t sequenceNumber = zsiGetSequenceNumber();

  zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_AREQ_CMD, ZSI_CMD_MAC,
                    ZSI_RF_RANDOM_REQ);
  return result;
}

/**************************************************************************//**
  \brief RF_RandomConf confirm primitive deserialization and
         processing routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
*******************************************************************************/
ZsiProcessingResult_t zsiDeserializeRF_RandomConf(void *memory,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepMemory = false,
    .keepCmdFrame = false
  };
  ZsiSerializer_t serializer =
  {
    .pow = cmdFrame->payload,
    .por = cmdFrame->payload
  };

  // Obtain memory for confirm from bearing request
  RF_RandomReq_t *req = zsiDriverReturnBearingEntity(cmdFrame->sequenceNumber);
  zsiDeserializeUint16(&serializer, &req->confirm.value);
  req->RF_RandomConf(&req->confirm);

  (void)memory;
  return result;
}

#elif defined(ZAPPSI_NP)
/**************************************************************************//**
  \brief MAC_IsOwnExtAddrReq request primitive deserialization and
         processing routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
*******************************************************************************/
ZsiProcessingResult_t zsiDeserializeMAC_IsOwnExtAddrReq(void *memory,
  ZsiCommandFrame_t *const cmdFrame)
{
  ExtAddr_t *dataIn = (ExtAddr_t *)memory;
  bool tmpValue;
  ZsiProcessingResult_t result =
  {
    .keepMemory = false,
    .keepCmdFrame = false
  };
  ZsiSerializer_t serializer =
  {
    .pow = cmdFrame->payload,
    .por = cmdFrame->payload
  };

  zsiDeserializeUint64(&serializer, dataIn);

  tmpValue = MAC_IsOwnExtAddr(dataIn);
  serializer.pow = cmdFrame->payload;
  serializer.por = cmdFrame->payload;

  // Prepare and send confirm frame at the same buffer, as request
  {
    uint16_t length = zsiMAC_IsOwnExtAddrConf(NULL);
    uint8_t sequenceNumber = cmdFrame->sequenceNumber;

    zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_SRSP_CMD, ZSI_CMD_MAC,
                      ZSI_MAC_IS_OWN_EXT_ADDR_CONF);

    zsiSerializeUint8(&serializer, tmpValue);
  }

  return result;
}

/**************************************************************************//**
  \brief MAC_GetExtAddrReq request primitive deserialization and
         processing routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
*******************************************************************************/
ZsiProcessingResult_t zsiDeserializeMAC_GetExtAddrReq(void *memory,
  ZsiCommandFrame_t *const cmdFrame)
{
  ExtAddr_t *extAddrPtr = (ExtAddr_t *)memory;

  ZsiProcessingResult_t result =
  {
    .keepMemory = true,
    .keepCmdFrame = true
  };
  ZsiSerializer_t serializer =
  {
    .pow = cmdFrame->payload,
    .por = cmdFrame->payload
  };

  extAddrPtr = (ExtAddr_t *)MAC_GetExtAddr();
  serializer.pow = cmdFrame->payload;
  serializer.por = cmdFrame->payload;

  // Prepare and send confirm frame at the same buffer, as request
  {
    uint16_t length = zsiMAC_GetExtAddrConf(NULL);
    uint8_t sequenceNumber = cmdFrame->sequenceNumber;

    zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_SRSP_CMD, ZSI_CMD_MAC,
                      ZSI_MAC_GET_EXT_ADDR_CONF);

    zsiSerializeUint64(&serializer, extAddrPtr);
  }

  return result;
}

/**************************************************************************//**
  \brief RF_RandomReq request primitive deserialization and
         processing routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
*******************************************************************************/
ZsiProcessingResult_t zsiDeserializeRF_RandomReq(void *memory,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepMemory = true,
    .keepCmdFrame = false
  };

  zsiRandomReq = (RF_RandomReq_t *)memory; // hold on pointer to remove memory in confirm
  zsiRandomReq->service.sequenceNumber = cmdFrame->sequenceNumber;
  zsiRandomReq->disableRx = false;
  zsiRandomReq->RF_RandomConf = zsiRfRandomConf;
  RF_RandomReq(zsiRandomReq);

  return result;
}

/**************************************************************************//**
  \brief RF_RandomConf primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeRF_RandomConf(const void *const req,
  ZsiCommandFrame_t *const cmdFrame)
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
  uint16_t length = zsiRF_RandomConf(req);
  zsiPrepareCommand(cmdFrame, length, zsiRandomReq->service.sequenceNumber, ZSI_AREQ_CMD, ZSI_CMD_MAC,
                    ZSI_RF_RANDOM_CONF);

  zsiSerializeUint16(&serializer, zsiRandomReq->confirm.value);

  return result;
}
#endif /* ZAPPSI_NP */

/* eof zsiMacSerialization.c */
