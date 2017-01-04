/***************************************************************************//**
  \file zsiKeSerialization.c

  \brief ZAppSI Certicom primitives serialization.
         This file was automatically generated.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    29.11.2012  A. Khromykh - Created.
   Last change:
    $Id: zsiKeSerialization.c 24445 2013-02-07 14:52:15Z akhromykh $
 ******************************************************************************/

/******************************************************************************
                               Includes section
 ******************************************************************************/
#if CERTICOM_SUPPORT == 1
#include <zsiKeSerialization.h>
#include <zsiSerializer.h>
#include <zsiMemoryManager.h>
#include <sysUtils.h>
#include <zsiDbg.h>
#include <zclKeyEstablishmentCluster.h>

/******************************************************************************
                    Defines section
******************************************************************************/
#define YIELD_LEVEL           3ul

/******************************************************************************
                   External global variables section
******************************************************************************/
#if defined(ZAPPSI_HOST)
extern bool zsiAsyncCerticomWaiting;
#endif

/******************************************************************************
                   Local variables section
******************************************************************************/
#ifdef ZAPPSI_HOST

#elif defined(ZAPPSI_NP)
static uint8_t buffRndSeq[ZCL_KE_INITIATE_RANDOM_SEQ_SIZE];
SSP_KeyedHashMacReq_t zsiKeyedHashMacReq;
static ZsiCerticomMem_t zsiCerticomMem;
#endif /* ZAPPSI_NP */

/******************************************************************************
                               Implementation section
*******************************************************************************/
#ifdef ZAPPSI_HOST
/**************************************************************************//**
  \brief KE-ZSE_ECCGenerateKey primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeZSE_ECCGenerateKeyReq(const void *const req,
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
  ZsiEccGenerateKey_t *data = (ZsiEccGenerateKey_t *)req;
  uint16_t length = zsiZSE_ECCGenerateKeyReq(NULL);
  uint8_t sequenceNumber = zsiGetSequenceNumber();

  zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_SREQ_CMD, ZSI_CMD_KE,
                    ZSI_KE_ECC_GENERATE_KEY_REQ);

  zsiSerializeUint64(&serializer, &data->zsiKeRemoteExtAddr);

  return result;
}

/**************************************************************************//**
  \brief KE-ZSE_ECCKeyBitGenerate primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeZSE_ECCKeyBitGenerateReq(const void *const req,
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
  ZsiEccKeyBitGenerate_t *data = (ZsiEccKeyBitGenerate_t *)req;
  uint16_t length = zsiZSE_ECCKeyBitGenerateReq(NULL);
  uint8_t sequenceNumber = zsiGetSequenceNumber();

  zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_SREQ_CMD, ZSI_CMD_KE,
                    ZSI_KE_ECC_KEY_BIT_GENERATE_REQ);

  zsiSerializeData(&serializer, data->privateKey, SECT163K1_PRIVATE_KEY_SIZE);
  zsiSerializeData(&serializer, data->localEphemeralPrivateKey, SECT163K1_PRIVATE_KEY_SIZE);
  zsiSerializeData(&serializer, data->localEphemeralPublicKey, SECT163K1_COMPRESSED_PUBLIC_KEY_SIZE);
  zsiSerializeData(&serializer, data->remoteCertificate, SECT163K1_CERTIFICATE_SIZE);
  zsiSerializeData(&serializer, data->remoteEphemeralPublicKey, SECT163K1_COMPRESSED_PUBLIC_KEY_SIZE);
  zsiSerializeData(&serializer, data->publicKey, SECT163K1_COMPRESSED_PUBLIC_KEY_SIZE);
  return result;
}

/**************************************************************************//**
  \brief KE-SSP_BcbHashReq primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeSSP_BcbHashReq(const void *const req,
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

  uint16_t length = zsiSSP_BcbHashReq(req);
  uint8_t sequenceNumber = zsiGetSequenceNumber();

  zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_SREQ_CMD, ZSI_CMD_KE,
                    ZSI_KE_SSP_BCB_HASH_REQ);

  zsiSerializeUint32(&serializer, &((ZsiSspBcbHash_t *)req)->size);
  zsiSerializeData(&serializer, ((ZsiSspBcbHash_t *)req)->data, ((ZsiSspBcbHash_t *)req)->size);

  return result;
}

/**************************************************************************//**
  \brief KE-SSP_KeyedHashMacReq primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeSSP_KeyedHashMacReq(const void *const req,
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
  SSP_KeyedHashMacReq_t *data = (SSP_KeyedHashMacReq_t *)req;

  uint16_t length = zsiSSP_KeyedHashMacReq(req);
  uint8_t sequenceNumber = zsiGetSequenceNumber();

  zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_AREQ_CMD, ZSI_CMD_KE,
                    ZSI_KE_SSP_KEYED_HASH_MAC_REQ);

  zsiSerializeUint8(&serializer, data->textSize);
  // real text
  zsiSerializeData(&serializer, data->text, data->textSize);
  zsiSerializeData(&serializer, data->key, AES_MMO_HASH_SIZE);

  return result;
}

/**************************************************************************//**
  \brief KE-ZSE_ECCGenerateKey confirm primitive deserialization and
         processing routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
*******************************************************************************/
ZsiProcessingResult_t zsiDeserializeZSE_ECCGenerateKeyConf(void *memory,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepMemory = true,
    .keepCmdFrame = false
  };
  ZsiSerializer_t serializer =
  {
    .pow = cmdFrame->payload,
    .por = cmdFrame->payload
  };
  ZsiEccGenerateKey_t *data = (ZsiEccGenerateKey_t *)memory;

  zsiDeserializeData(&serializer, data->privateKey, SECT163K1_PRIVATE_KEY_SIZE);
  zsiDeserializeData(&serializer, data->publicKey, SECT163K1_COMPRESSED_PUBLIC_KEY_SIZE);

  return result;
}

/**************************************************************************//**
  \brief KE-ZSE_ECCKeyBitGenerate confirm primitive deserialization and
         processing routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
*******************************************************************************/
ZsiProcessingResult_t zsiDeserializeZSE_ECCKeyBitGenerateConf(void *memory,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepMemory = true,
    .keepCmdFrame = false
  };
  ZsiSerializer_t serializer =
  {
    .pow = cmdFrame->payload,
    .por = cmdFrame->payload
  };
  ZsiEccKeyBitGenerate_t *data = (ZsiEccKeyBitGenerate_t *)memory;

  zsiDeserializeData(&serializer, data->localKeyBits, SECT163K1_SHARED_SECRET_SIZE);

  return result;
}

/**************************************************************************//**
  \brief KE-SSP_BcbHash confirm primitive deserialization and
         processing routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
*******************************************************************************/
ZsiProcessingResult_t zsiDeserializeSSP_BcbHashConf(void *memory,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepMemory = true,
    .keepCmdFrame = false
  };
  ZsiSerializer_t serializer =
  {
    .pow = cmdFrame->payload,
    .por = cmdFrame->payload
  };

  zsiDeserializeData(&serializer, memory, AES_MMO_HASH_SIZE);

  return result;
}

/**************************************************************************//**
  \brief KE-SSP_KeyedHashMacConf confirm primitive deserialization and
         processing routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
*******************************************************************************/
ZsiProcessingResult_t zsiDeserializeSSP_KeyedHashMacConf(void *memory,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepMemory = true,
    .keepCmdFrame = false
  };
  ZsiSerializer_t serializer =
  {
    .pow = cmdFrame->payload,
    .por = cmdFrame->payload
  };

  // Obtain memory for confirm from bearing request
  SSP_KeyedHashMacReq_t *req = zsiDriverReturnBearingEntity(cmdFrame->sequenceNumber);

  zsiDeserializeData(&serializer, req->hash_i, AES_MMO_HASH_SIZE);
  req->SSP_KeyedHashMacConf();

  (void)memory;
  return result;
}

#elif defined(ZAPPSI_NP)
/**************************************************************************//**
  \brief This function copies size bytes of random data into buffer.

  \param: buffer - This is an unsigned char array of size at least sz to hold
   the random data.
  \param: size - The number of bytes of random data to compute and store.

  \return:  0 Indicates successful completion.
******************************************************************************/
static int zsiGetRndSeq(uint8_t *buffer, unsigned long size)
{
  ZsiEccGenerateKey_t *keys = &zsiCerticomMem.zsiEccGenerateKey;

  memcpy(buffer, buffRndSeq, size);
  if (MAC_IsOwnExtAddr(&keys->zsiKeRemoteExtAddr))
    SYS_GetRandomSequence(buffRndSeq, ZCL_KE_INITIATE_RANDOM_SEQ_SIZE);
  return 0;
}

/**************************************************************************//**
  \brief To accord Certicom API.
******************************************************************************/
static int yield(void)
{
  //Do needed processing here, ie. reset watchdog timers
  return MCE_SUCCESS;
}

/**************************************************************************//**
  \brief KE-ZSE_ECCGenerateKeyReq request primitive deserialization and
         processing routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
*******************************************************************************/
ZsiProcessingResult_t zsiDeserializeZSE_ECCGenerateKeyReq(void *memory,
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
  ZsiEccGenerateKey_t *keys = &zsiCerticomMem.zsiEccGenerateKey;

  // get remote extended address from host
  zsiDeserializeUint64(&serializer, &keys->zsiKeRemoteExtAddr);

  SYS_GetRandomSequence(buffRndSeq, ZCL_KE_INITIATE_RANDOM_SEQ_SIZE);
  //Generating the ephemeral public and private Key Pair
  ZSE_ECCGenerateKey(keys->privateKey, keys->publicKey, zsiGetRndSeq, yield, YIELD_LEVEL);

  serializer.pow = cmdFrame->payload;
  serializer.por = cmdFrame->payload;

  // Prepare and send confirm frame at the same buffer, as request
  {
    uint16_t length = zsiZSE_ECCGenerateKeyConf(NULL);
    uint8_t sequenceNumber = cmdFrame->sequenceNumber;

    zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_SRSP_CMD, ZSI_CMD_KE,
                      ZSI_KE_ECC_GENERATE_KEY_CONF);

    zsiSerializeData(&serializer, keys->privateKey, SECT163K1_PRIVATE_KEY_SIZE);
    zsiSerializeData(&serializer, keys->publicKey, SECT163K1_COMPRESSED_PUBLIC_KEY_SIZE);
  }

  return result;
}

/**************************************************************************//**
  \brief KE-ZSE_ECCKeyBitGenerateReq request primitive deserialization and
         processing routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
*******************************************************************************/
ZsiProcessingResult_t zsiDeserializeZSE_ECCKeyBitGenerateReq(void *memory,
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
  ZsiEccKeyBitGenerate_t *data = &zsiCerticomMem.zsiEccKeyBitGenerate;
  // deserialize keys
  zsiDeserializeData(&serializer, data->privateKey, SECT163K1_PRIVATE_KEY_SIZE);
  zsiDeserializeData(&serializer, data->localEphemeralPrivateKey, SECT163K1_PRIVATE_KEY_SIZE);
  zsiDeserializeData(&serializer, data->localEphemeralPublicKey, SECT163K1_COMPRESSED_PUBLIC_KEY_SIZE);
  zsiDeserializeData(&serializer, data->remoteCertificate, SECT163K1_CERTIFICATE_SIZE);
  zsiDeserializeData(&serializer, data->remoteEphemeralPublicKey, SECT163K1_COMPRESSED_PUBLIC_KEY_SIZE);
  zsiDeserializeData(&serializer, data->publicKey, SECT163K1_COMPRESSED_PUBLIC_KEY_SIZE);

  ZSE_ECCKeyBitGenerate(data->privateKey, data->localEphemeralPrivateKey,
                        data->localEphemeralPublicKey, data->remoteCertificate,
                        data->remoteEphemeralPublicKey, data->publicKey,
                        data->localKeyBits, SSP_BcbHash, yield, YIELD_LEVEL);

  serializer.pow = cmdFrame->payload;
  serializer.por = cmdFrame->payload;

  {
    uint16_t length = zsiZSE_ECCKeyBitGenerateConf(NULL);
    uint8_t sequenceNumber = cmdFrame->sequenceNumber;

    zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_SRSP_CMD, ZSI_CMD_KE,
                      ZSI_KE_ECC_KEY_BIT_GENERATE_CONF);

    zsiSerializeData(&serializer, data->localKeyBits, SECT163K1_SHARED_SECRET_SIZE);
  }
  return result;
}

/**************************************************************************//**
  \brief KE-SSP_BcbHashReq request primitive deserialization and
         processing routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
*******************************************************************************/
ZsiProcessingResult_t zsiDeserializeSSP_BcbHashReq(void *memory,
  ZsiCommandFrame_t *const cmdFrame)
{
  uint8_t *tmpDidgest = (uint8_t *)memory;
  uint32_t tmpSize;
  uint8_t *tmpData;
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

  zsiDeserializeData(&serializer, &tmpSize, sizeof(uint32_t));
  tmpData = (uint8_t *)serializer.por;

  SSP_BcbHash(tmpDidgest, tmpSize, tmpData);
  serializer.pow = cmdFrame->payload;
  serializer.por = cmdFrame->payload;

  // Prepare and send confirm frame at the same buffer, as request
  {
    uint16_t length = zsiSSP_BcbHashConf(NULL);
    uint8_t sequenceNumber = cmdFrame->sequenceNumber;

    zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_SRSP_CMD, ZSI_CMD_KE,
                      ZSI_KE_SSP_BCB_HASH_CONF);

    zsiSerializeData(&serializer, tmpDidgest, AES_MMO_HASH_SIZE);
  }

  return result;
}

/**************************************************************************//**
  \brief KE-SSP_KeyedHashMacConf primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeSSP_KeyedHashMacConf(const void *const req,
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
  uint16_t length = zsiSSP_KeyedHashMacConf(NULL);
  uint8_t sequenceNumber = zsiKeyedHashMacReq.service.sequenceNumber;

  zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_AREQ_CMD, ZSI_CMD_KE,
                    ZSI_KE_SSP_KEYED_HASH_MAC_CONF);

  zsiSerializeData(&serializer, zsiKeyedHashMacReq.hash_i, AES_MMO_HASH_SIZE);
  return result;
}

/**************************************************************************//**
  \brief SSP_KeyedHashMacReq request primitive deserialization and
         processing routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
*******************************************************************************/
ZsiProcessingResult_t zsiDeserializeSSP_KeyedHashMacReq(void *memory,
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

  ZsiKeMacBuffer_t *buffer = &zsiCerticomMem.zsiKeMacBuffer;
  zsiDeserializeUint8(&serializer, &zsiKeyedHashMacReq.textSize);
  zsiDeserializeData(&serializer, buffer->macData, zsiKeyedHashMacReq.textSize);
  zsiKeyedHashMacReq.text = buffer->macData;
  zsiDeserializeData(&serializer, buffer->digest, AES_MMO_HASH_SIZE);
  zsiKeyedHashMacReq.key = buffer->digest;
  zsiKeyedHashMacReq.hash_i = buffer->hash;
  zsiKeyedHashMacReq.service.sequenceNumber = cmdFrame->sequenceNumber;
  zsiKeyedHashMacReq.SSP_KeyedHashMacConf = zsiKeyedHashMacConf;
  SSP_KeyedHashMacReq(&zsiKeyedHashMacReq);

  return result;
}
#endif /* ZAPPSI_NP */

#endif // CERTICOM_SUPPORT == 1
/* eof zsiKeSerialization.c */
