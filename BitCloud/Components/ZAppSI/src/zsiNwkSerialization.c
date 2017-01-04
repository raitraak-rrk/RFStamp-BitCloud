/***************************************************************************//**
  \file zsiNwkSerialization.c

  \brief ZAppSI NWK primitives serialization. This file was automatically
         generated.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   Last change:
    $Id: zsiNwkSerialization.c 27088 2014-08-19 05:54:45Z mukesh.basrani $
 ******************************************************************************/

/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <zsiNwkSerialization.h>
#include <zsiSerializer.h>
#include <sysUtils.h>

/******************************************************************************
                               Implementation section
 ******************************************************************************/

#ifdef ZAPPSI_HOST
/**************************************************************************//**
  \brief NWK-GetNextHop request primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

      \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeNWK_GetNextHopReq(const void *const req,
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
  uint16_t length = zsiNWK_GetNextHopReqLength(NULL);
  uint8_t sequenceNumber = zsiGetSequenceNumber();
  ShortAddr_t *shortAddr = (ShortAddr_t *)req;

  zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_SREQ_CMD, ZSI_CMD_NWK,
                    ZSI_NWK_GET_NEXT_HOP_REQUEST);

  zsiSerializeUint16(&serializer, *shortAddr);

  return result;
}

/**************************************************************************//**
  \brief NWK-GetNextHop confirm primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeNWK_GetNextHopConf(void *memory,
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

  zsiDeserializeUint16(&serializer, (uint16_t *)memory);

  return result;
}

/**************************************************************************//**
  \brief NWK-GetExtByShortAddress request primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
******************************************************************************/
ZsiProcessingResult_t zsiSerializeNWK_GetExtByShortAddressReq(const void *const req,
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
  uint16_t length = zsiNWK_GetExtByShortAddressReqLength(NULL);
  uint8_t sequenceNumber = zsiGetSequenceNumber();
  ShortAddr_t *shortAddr = (ShortAddr_t *)req;

  zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_SREQ_CMD, ZSI_CMD_NWK,
    ZSI_NWK_GET_EXT_BY_SHORT_ADDRESS_REQUEST);

  zsiSerializeUint16(&serializer, *shortAddr);

  return result;
}

/**************************************************************************//**
  \brief NWK-GetExtByShortAddress confirm primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
******************************************************************************/
ZsiProcessingResult_t zsiDeserializeNWK_GetExtByShortAddressConf(void *memory,
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

  zsiDeserializeUint64(&serializer, (uint64_t *)memory);

  return result;
}

/**************************************************************************//**
  \brief NWK-GetShortByExtAddress request primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
******************************************************************************/
ZsiProcessingResult_t zsiSerializeNWK_GetShortByExtAddressReq(const void *const req,
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
  uint16_t length = zsiNWK_GetShortByExtAddressReqLength(NULL);
  uint8_t sequenceNumber = zsiGetSequenceNumber();
  ExtAddr_t *extAddr = (ExtAddr_t *)req;

  zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_SREQ_CMD, ZSI_CMD_NWK,
    ZSI_NWK_GET_SHORT_BY_EXT_ADDRESS_REQUEST);

  zsiSerializeUint64(&serializer, extAddr);

  return result;
}

/**************************************************************************//**
  \brief NWK-GetShortByExtAddress confirm primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

    \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeNWK_GetShortByExtAddressConf(void *memory,
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

  zsiDeserializeUint16(&serializer, (uint16_t *)memory);

  return result;
}

/**************************************************************************//**
  \brief NWK-GetUnicastDeliveryTime primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeNWK_GetUnicastDeliveryTimeReq(const void *const req,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = true,
    .keepMemory = true
  };
  uint16_t length = zsiNWK_GetUnicastDeliveryTimeReqLength(NULL);
  uint8_t sequenceNumber = zsiGetSequenceNumber();

  (void)req;

  zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_SREQ_CMD, ZSI_CMD_NWK,
                    ZSI_NWK_GET_UNICAST_DELIVERY_TIME_REQUEST);

  return result;
}

/**************************************************************************//**
  \brief NWK-GetUnicastDeliveryTime confirm primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

    \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeNWK_GetUnicastDeliveryTimeConf(void *memory,
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

  zsiDeserializeUint32(&serializer, (uint32_t *)memory);

  return result;
}

/**************************************************************************//**
  \brief NWK-GetShortAddr primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeNWK_GetShortAddrReq(const void *const req,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = true,
    .keepMemory = true
  };
  uint16_t length = zsiSerializeNWK_GetShortAddrReqLength(NULL);
  uint8_t sequenceNumber = zsiGetSequenceNumber();

  (void)req;

  zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_SREQ_CMD, ZSI_CMD_NWK,
                    ZSI_NWK_GET_SHORT_ADDRESS_REQUEST);

  return result;
}

/**************************************************************************//**
  \brief NWK-GetShortAddr confirm primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

    \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeNWK_GetShortAddrConf(void *memory,
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

  zsiDeserializeUint16(&serializer, (uint16_t *)memory);

  return result;
}
#ifdef _SECURITY_
/**************************************************************************//**
  \brief NWK-SetKey primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeNWK_SetKeyReq(const void *const req,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = true,
    .keepMemory = true
  };
  ZsiSerializer_t serializer =
  {
    .pow = cmdFrame->payload,
    .por = cmdFrame->payload
  };
  ZsiSetKey_t *nwkSetKey = (ZsiSetKey_t *)req;

  uint16_t length = zsiSerializeNWK_SetKeyReqLength(NULL);
  uint8_t sequenceNumber = zsiGetSequenceNumber();

  zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_SREQ_CMD, ZSI_CMD_NWK,
                    ZSI_NWK_SET_KEY_REQUEST);

  zsiSerializeData(&serializer, nwkSetKey->key, SECURITY_KEY_SIZE);
  zsiSerializeUint8(&serializer, nwkSetKey->keySeqNum);

  return result;  
}
/**************************************************************************//**
  \brief NWK-SetKey confirm primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeNWK_SetKeyConf(void *memory,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = false,
    .keepMemory = true
  };

  (void)memory;
  (void)cmdFrame;
  return result; 
}
/**************************************************************************//**
  \brief NWK-ActivateKey primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeNWK_ActivateKeyReq(const void *const req,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = true,
    .keepMemory = true
  };
  ZsiSerializer_t serializer =
  {
    .pow = cmdFrame->payload,
    .por = cmdFrame->payload
  };
  uint8_t *nwkKeySeqNum = (uint8_t *)req;

  uint16_t length = zsiSerializeNWK_ActivateKeyReqLength(NULL);
  uint8_t sequenceNumber = zsiGetSequenceNumber();

  zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_SREQ_CMD, ZSI_CMD_NWK,
                    ZSI_NWK_ACTIVATE_KEY_REQUEST);

  zsiSerializeUint8(&serializer, *nwkKeySeqNum);

  return result;  
}
/**************************************************************************//**
  \brief NWK-ActivateKey confirm primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

    \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeNWK_ActivateKeyConf(void *memory,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = false,
    .keepMemory = true
  };

  (void)memory;
  (void)cmdFrame;
  return result; 
}
#endif /* _SECURITY_ */
#elif defined(ZAPPSI_NP)
/**************************************************************************//**
  \brief NWK-GetNextHop request primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeNWK_GetNextHopReq(void *memory,
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
  ShortAddr_t *targetShortAddr = (ShortAddr_t *)memory;
  ShortAddr_t shortAddr;

  zsiDeserializeUint16(&serializer, targetShortAddr);

  shortAddr = NWK_GetNextHop(*targetShortAddr);

  /* Prepare and send confirm frame at the same buffer, as request */
  {
    uint16_t length = zsiNWK_GetNextHopConfLength(NULL);
    uint8_t sequenceNumber = cmdFrame->sequenceNumber;

    zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_SRSP_CMD, ZSI_CMD_NWK,
     ZSI_NWK_GET_NEXT_HOP_CONFIRM);

    zsiSerializeUint16(&serializer, shortAddr);
  }

  return result;
}

/**************************************************************************//**
  \brief NWK-GetExtByShortAddress request primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
******************************************************************************/
ZsiProcessingResult_t zsiDeserializeNWK_GetExtByShortAddressReq(void *memory,
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
  ShortAddr_t *shortAddr = (ShortAddr_t *)memory;
  const ExtAddr_t *extAddr;

  zsiDeserializeUint16(&serializer, shortAddr);

  extAddr = NWK_GetExtByShortAddress(*shortAddr);

  /* Prepare and send confirm frame at the same buffer, as request */
  {
    uint16_t length = zsiNWK_GetExtByShortAddressConfLength(NULL);
    uint8_t sequenceNumber = cmdFrame->sequenceNumber;

    zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_SRSP_CMD, ZSI_CMD_NWK,
     ZSI_NWK_GET_EXT_BY_SHORT_ADDRESS_CONFIRM);

    zsiSerializeUint64(&serializer, extAddr);
  }

  return result;
}

/**************************************************************************//**
  \brief NWK-GetShortByExtAddress request primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

    \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeNWK_GetShortByExtAddressReq(void *memory,
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
  ExtAddr_t *extAddr = (ExtAddr_t *)memory;
  const ShortAddr_t *shortAddr;

  zsiDeserializeUint64(&serializer, extAddr);

  shortAddr = NWK_GetShortByExtAddress(extAddr);

  /* Prepare and send confirm frame at the same buffer, as request */
  {
    uint16_t length = zsiNWK_GetShortByExtAddressConfLength(NULL);
    uint8_t sequenceNumber = cmdFrame->sequenceNumber;

    zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_SRSP_CMD, ZSI_CMD_NWK,
     ZSI_NWK_GET_SHORT_BY_EXT_ADDRESS_CONFIRM);

    zsiSerializeUint16(&serializer, *shortAddr);
  }

  return result;
}

/**************************************************************************//**
  \brief NWK-GetUnicastDeliveryTime request primitive deserialization and
         processing routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

    \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeNWK_GetUnicastDeliveryTimeReq(void *memory,
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
  uint32_t deliveryTime = NWK_GetUnicastDeliveryTime();

  /* Prepare and send confirm frame at the same buffer, as request */
  {
    uint16_t length = zsiNWK_GetUnicastDeliveryTimeConfLength(NULL);
    uint8_t sequenceNumber = cmdFrame->sequenceNumber;

    zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_SRSP_CMD, ZSI_CMD_NWK,
     ZSI_NWK_GET_UNICAST_DELIVERY_TIME_CONFIRM);

    zsiSerializeUint32(&serializer, &deliveryTime);
  }

  (void)memory;
  return result;
}

/**************************************************************************//**
  \brief NWK-NWK_GetShortAdd request primitive deserialization and
         processing routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

    \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeNWK_GetShortAddReq(void *memory,
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
  ShortAddr_t addr = NWK_GetShortAddr();

  /* Prepare and send confirm frame at the same buffer, as request */
  {
    uint16_t length = zsiNWK_GetShortAddConfLength(NULL);
    uint8_t sequenceNumber = cmdFrame->sequenceNumber;

    zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_SRSP_CMD, ZSI_CMD_NWK,
     ZSI_NWK_GET_SHORT_ADDRESS_CONFIRM);

    zsiSerializeUint16(&serializer, addr);
  }

  (void)memory;
  return result;
}
/**************************************************************************//**
  \brief NWK-SetKey request primitive deserialization and
         processing routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
 ******************************************************************************/
#ifdef _SECURITY_
ZsiProcessingResult_t zsiDeserializeNWK_SetKeyReq(void *memory,
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
  ZsiSetKey_t *nwkSetKey = (ZsiSetKey_t *)memory;

  zsiDeserializeData(&serializer, nwkSetKey->key, SECURITY_KEY_SIZE);
  zsiDeserializeUint8(&serializer, (uint8_t *)&nwkSetKey->keySeqNum);

  NWK_SetKey(nwkSetKey->key, nwkSetKey->keySeqNum);

  /* Prepare and send confirm frame at the same buffer, as request */
  {
    uint16_t length = zsiNWK_SetKeyConfLength(NULL);
    uint8_t sequenceNumber = cmdFrame->sequenceNumber;

    zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_SRSP_CMD, ZSI_CMD_NWK,
     ZSI_NWK_SET_KEY_CONFIRM);

  }

  (void)memory;
  return result;  
}
/**************************************************************************//**
  \brief NWK-ActivateKey request primitive deserialization and
         processing routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeNWK_ActivateKeyReq(void *memory,
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
  uint8_t *nwkKeySeqNum = (uint8_t *)memory;
  zsiDeserializeUint8(&serializer, (uint8_t *)nwkKeySeqNum);

  NWK_ActivateKey(*nwkKeySeqNum);

  /* Prepare and send confirm frame at the same buffer, as request */
  {
    uint16_t length = zsiNWK_ActivateKeyConfLength(NULL);
    uint8_t sequenceNumber = cmdFrame->sequenceNumber;

    zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_SRSP_CMD, ZSI_CMD_NWK,
     ZSI_NWK_ACTIVATE_KEY_CONFIRM);

  }

  (void)memory;
  return result;
}
#endif /* _SECURITY_ */
#endif /* ZAPPSI_NP */

/* eof zsiNwkSerialization.c */
