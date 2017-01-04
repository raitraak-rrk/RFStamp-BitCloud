/***************************************************************************//**
  \file zsiSysSerialization.c

  \brief ZAppSI system configuration primitives serialization.
         This file was automatically generated.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   Last change:
    $Id: zsiSysSerialization.c 26492 2014-03-26 10:49:30Z karthik.p_u $
 ******************************************************************************/

/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <zsiSysSerialization.h>
#include <zsiSerializer.h>
#include <sysUtils.h>

/******************************************************************************
                               Implementation section
 ******************************************************************************/

#ifdef ZAPPSI_HOST
/**************************************************************************//**
  \brief CS_ReadParameter request primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

      \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeCS_ReadParameterReq(const void *const req,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = true,
    .keepMemory = true
  };
  ZsiSerializer_t serializer =
  {
    .pow = cmdFrame->payload
  };
  uint16_t length = zsiCS_ReadParameterReqLength(NULL);
  uint8_t sequenceNumber = zsiGetSequenceNumber();
  CS_MemoryItemId_t parameterId = *(CS_MemoryItemId_t *)req;

  zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_SREQ_CMD, ZSI_CMD_SYS,
    ZSI_SYS_READ_PARAMETER_REQUEST);

  zsiSerializeUint16(&serializer, parameterId);

  return result;
}

/**************************************************************************//**
  \brief CS_ReadParameter confirm primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

    \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeCS_ReadParameterConf(void *memory,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = false,
    .keepMemory = true
  };
  ZsiSerializer_t serializer =
  {
    .por = cmdFrame->payload
  };
  extern ZsiCsParameter_t zsiCsParameter;

  zsiDeserializeUint16(&serializer, (uint16_t *)&zsiCsParameter.parameterId);
  zsiDeserializeUint16(&serializer, &zsiCsParameter.size);
  switch (zsiCsParameter.size)
  {
    case sizeof(uint8_t):
      zsiDeserializeUint8(&serializer, memory);
      break;

    case sizeof(uint16_t):
      zsiDeserializeUint16(&serializer, memory);
      break;

    case sizeof(uint32_t):
      zsiDeserializeUint32(&serializer, memory);
      break;

    case sizeof(uint64_t):
      zsiDeserializeUint64(&serializer, memory);
      break;

    default:
      zsiDeserializeData(&serializer, memory, zsiCsParameter.size);
      break;
  }

  return result;
}

/**************************************************************************//**
  \brief CS_WriteParameter request primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

      \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeCS_WriteParameterReq(const void *const req,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = true,
    .keepMemory = true
  };
  ZsiCsParameter_t *request = (ZsiCsParameter_t *)req;
  ZsiSerializer_t serializer =
  {
    .pow = cmdFrame->payload
  };
  uint16_t *valueUint16;
  uint16_t length = zsiCS_WriteParameterReqLength(request);
  uint8_t sequenceNumber = zsiGetSequenceNumber();

  zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_SREQ_CMD, ZSI_CMD_SYS,
    ZSI_SYS_WRITE_PARAMETER_REQUEST);

  zsiSerializeUint16(&serializer, request->parameterId);
  zsiSerializeUint16(&serializer, request->size);
  switch (request->size)
  {
    case sizeof(uint8_t):
      zsiSerializeUint8(&serializer, *((uint8_t *)request->payload));
      break;

    case sizeof(uint16_t):
      valueUint16 = (uint16_t *)request->payload;
      zsiSerializeUint16(&serializer, *valueUint16);
      break;

    case sizeof(uint32_t):
      zsiSerializeUint32(&serializer, (uint32_t *)request->payload);
      break;

    case sizeof(uint64_t):
      zsiSerializeUint64(&serializer, (uint64_t *)request->payload);
      break;

    default:
      zsiSerializeData(&serializer, &request->payload, request->size);
      break;
  }

  return result;
}

/**************************************************************************//**
  \brief CS_WriteParameter confirm primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

    \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeCS_WriteParameterConf(void *memory,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = false,
    .keepMemory = true
  };
  /* No actions should be performed */
  (void)memory;
  (void)cmdFrame;
  return result;
}
#elif defined(ZAPPSI_NP)
/**************************************************************************//**
  \brief CS_ReadParameter request primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

    \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeCS_ReadParameterReq(void *memory,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = false,
    .keepMemory = false
  };
  ZsiSerializer_t serializer =
  {
    .pow = cmdFrame->payload,
    .por = cmdFrame->payload
  };
  ZsiCsParameter_t *csParameter = (ZsiCsParameter_t *)memory;

  zsiDeserializeUint16(&serializer, (uint16_t *)&csParameter->parameterId);

  csParameter->size = CS_GetItemSize(csParameter->parameterId);
  CS_ReadParameter(csParameter->parameterId, csParameter->payload);

  /* Prepare and send confirm frame at the same buffer, as request */
  {
    uint16_t length = zsiCS_ReadParameterConfLength(csParameter);
    uint8_t sequenceNumber = cmdFrame->sequenceNumber;
    uint16_t *valueUint16;

    zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_SRSP_CMD, ZSI_CMD_SYS,
      ZSI_SYS_READ_PARAMETER_CONFIRM);

    zsiSerializeUint16(&serializer, csParameter->parameterId);
    zsiSerializeUint16(&serializer, csParameter->size);

    switch (csParameter->size)
    {
      case sizeof(uint8_t):
        zsiSerializeUint8(&serializer, *((uint8_t *)csParameter->payload));
        break;

      case sizeof(uint16_t):
        valueUint16 = (uint16_t *)csParameter->payload;
        zsiSerializeUint16(&serializer, *valueUint16);
        break;

      case sizeof(uint32_t):
        zsiSerializeUint32(&serializer, (uint32_t *)csParameter->payload);
        break;

      case sizeof(uint64_t):
        zsiSerializeUint64(&serializer, (uint64_t *)csParameter->payload);
        break;

      default:
        zsiSerializeData(&serializer, &csParameter->payload, csParameter->size);
        break;
    }
  }

  return result;
}

/**************************************************************************//**
  \brief CS_WriteParameter request primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

    \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeCS_WriteParameterReq(void *memory,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = true,
    .keepMemory = false
  };
  ZsiSerializer_t serializer =
  {
    .por = cmdFrame->payload
  };
  ZsiCsParameter_t *csParameter = (ZsiCsParameter_t *)memory;

  zsiDeserializeUint16(&serializer, (uint16_t *)&csParameter->parameterId);
  zsiDeserializeUint16(&serializer, &csParameter->size);

  switch (csParameter->size)
  {
    case sizeof(uint8_t):
      zsiDeserializeUint8(&serializer, csParameter->payload);
      break;

    case sizeof(uint16_t):
      zsiDeserializeUint16(&serializer, (uint16_t *)csParameter->payload);
      break;

    case sizeof(uint32_t):
      zsiDeserializeUint32(&serializer, (uint32_t *)csParameter->payload);
      break;

    case sizeof(uint64_t):
      zsiDeserializeUint64(&serializer, (uint64_t *)csParameter->payload);
      break;

    default:
      zsiDeserializeData(&serializer, csParameter->payload, csParameter->size);
      break;
  }

  CS_WriteParameter(csParameter->parameterId, csParameter->payload);

  /* Prepare and send confirm frame at the same buffer, as request */
  {
    uint16_t length = zsiCS_WriteParameterConfLength(NULL);
    uint8_t sequenceNumber = cmdFrame->sequenceNumber;

    zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_SRSP_CMD, ZSI_CMD_SYS,
                      ZSI_SYS_WRITE_PARAMETER_CONFIRM);

    /* No additional serialization required */
  }

  return result;
}

#endif /* ZAPPSI_NP */

/* eof zsiSysSerialization.c */
