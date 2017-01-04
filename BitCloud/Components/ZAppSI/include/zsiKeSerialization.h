/******************************************************************************
  \file zsiKeSerialization.h

  \brief ZAppSI Certicom primitives serialization interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2010 , Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
  29.11.2012  A. Khromykh - Created.
  Last change:
  $Id: zsiKeSerialization.h 24619 2013-03-15 09:49:47Z aluzhetskiy $
******************************************************************************/

#ifndef _ZSIKESERIALIZATION_H_
#define _ZSIKESERIALIZATION_H_

/******************************************************************************
                    Includes section
******************************************************************************/
#include <aps.h>
#include <zsiFrames.h>
#include <zsiDriver.h>
#include <sspCommon.h>

#if CERTICOM_SUPPORT == 1
#include <sspHash.h>
#include <genericEcc.h>

/******************************************************************************
                    Defines section
******************************************************************************/
#define ZSI_KE_ADDITIONAL_MESSAGE_COMPONENT_SIZE sizeof(uint8_t)
#define ZSI_KE_MAC_DATA_SIZE ((2 * SECT163K1_COMPRESSED_PUBLIC_KEY_SIZE) + \
                              (2 * sizeof(ExtAddr_t)) + \
                               ZSI_KE_ADDITIONAL_MESSAGE_COMPONENT_SIZE) // 61 bytes

/******************************************************************************
                    Types section
******************************************************************************/
typedef struct _ZsiEccKeyBitGenerateAllocator_t
{
  uint8_t privateKey[SECT163K1_PRIVATE_KEY_SIZE];
  uint8_t localEphemeralPrivateKey[SECT163K1_PRIVATE_KEY_SIZE];
  uint8_t localEphemeralPublicKey[SECT163K1_COMPRESSED_PUBLIC_KEY_SIZE];
  uint8_t remoteCertificate[SECT163K1_CERTIFICATE_SIZE];
  uint8_t remoteEphemeralPublicKey[SECT163K1_COMPRESSED_PUBLIC_KEY_SIZE];
  uint8_t publicKey[SECT163K1_COMPRESSED_PUBLIC_KEY_SIZE];
  uint8_t localKeyBits[SECT163K1_SHARED_SECRET_SIZE];
} ZsiEccKeyBitGenerate_t;

typedef struct _ZsiEccGenerateKey_t
{
  ExtAddr_t zsiKeRemoteExtAddr;
  uint8_t privateKey[SECT163K1_PRIVATE_KEY_SIZE];
  uint8_t publicKey[SECT163K1_COMPRESSED_PUBLIC_KEY_SIZE];
} ZsiEccGenerateKey_t;

BEGIN_PACK
typedef struct PACK _ZsiKeMacBuffer_t
{
  volatile uint8_t keyExpander[SECURITY_KEY_SIZE];
  uint8_t macData[ZSI_KE_MAC_DATA_SIZE];
  volatile uint8_t bitsExpander[AES_MMO_HASH_SIZE];

  uint8_t digest[AES_MMO_HASH_SIZE];
  uint8_t hash[AES_MMO_HASH_SIZE];
} ZsiKeMacBuffer_t;
END_PACK

typedef union _ZsiCerticomMem_t
{
  ZsiEccKeyBitGenerate_t zsiEccKeyBitGenerate;
  ZsiEccGenerateKey_t zsiEccGenerateKey;
  ZsiKeMacBuffer_t zsiKeMacBuffer;
} ZsiCerticomMem_t;

typedef struct _ZsiSspBcbHash_t
{
  uint32_t size;
  uint8_t *data;
} ZsiSspBcbHash_t;

/******************************************************************************
                    Prototypes section
******************************************************************************/
/**************************************************************************//**
  \brief Finds routine for KE commands deserialization.

  \param[in] commandId - command identifier.

  \return Pointer to appropriate routine.
 ******************************************************************************/
ZsiProcessingRoutine_t zsiKeFindProcessingRoutine(uint8_t commandId);

#ifdef ZAPPSI_HOST
/**************************************************************************//**
  \brief KE-ZSE_ECCGenerateKeyReq frame size calculation routine.
         SOF and LENGTH fields are dismissed.

  \param[in] req - request parameters.

  \return KE-ZSE_ECCGenerateKeyReq request frame size.
 ******************************************************************************/
INLINE uint16_t zsiZSE_ECCGenerateKeyReq(const void *const req)
{
  (void)req;
  return ZSI_COMMAND_FRAME_OVERHEAD + sizeof(ExtAddr_t);
}

/**************************************************************************//**
  \brief KE-ZSE_ECCKeyBitGenerateReq frame size calculation routine.
         SOF and LENGTH fields are dismissed.

  \param[in] req - request parameters.

  \return KE-ZSE_ECCKeyBitGenerateReq request frame size.
 ******************************************************************************/
INLINE uint16_t zsiZSE_ECCKeyBitGenerateReq(const void *const req)
{
  (void)req;
  return ZSI_COMMAND_FRAME_OVERHEAD + SECT163K1_PRIVATE_KEY_SIZE +
         SECT163K1_PRIVATE_KEY_SIZE + SECT163K1_COMPRESSED_PUBLIC_KEY_SIZE +
         SECT163K1_CERTIFICATE_SIZE + SECT163K1_COMPRESSED_PUBLIC_KEY_SIZE +
         SECT163K1_COMPRESSED_PUBLIC_KEY_SIZE;
}

/**************************************************************************//**
  \brief KE-SSP_BcbHashReq frame size calculation routine.
         SOF and LENGTH fields are dismissed.

  \param[in] req - request parameters.

  \return KE-SSP_BcbHashReq request frame size.
 ******************************************************************************/
INLINE uint16_t zsiSSP_BcbHashReq(const void *const req)
{
  return ZSI_COMMAND_FRAME_OVERHEAD + sizeof(uint32_t) +
         ((ZsiSspBcbHash_t *)req)->size;
}

/**************************************************************************//**
  \brief KE-SSP_KeyedHashMacReq frame size calculation routine.
         SOF and LENGTH fields are dismissed.

  \param[in] req - request parameters.

  \return KE-SSP_KeyedHashMacReq request frame size.
 ******************************************************************************/
INLINE uint16_t zsiSSP_KeyedHashMacReq(const void *const req)
{
  return ZSI_COMMAND_FRAME_OVERHEAD + sizeof(uint8_t) +
         ((SSP_KeyedHashMacReq_t *)req)->textSize + AES_MMO_HASH_SIZE;
}

/**************************************************************************//**
  \brief KE-ZSE_ECCGenerateKey primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeZSE_ECCGenerateKeyReq(const void *const req,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief KE-ZSE_ECCKeyBitGenerate primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeZSE_ECCKeyBitGenerateReq(const void *const req,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief KE-SSP_BcbHashReq primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeSSP_BcbHashReq(const void *const req,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief KE-SSP_KeyedHashMacReq primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeSSP_KeyedHashMacReq(const void *const req,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief KE-ZSE_ECCGenerateKey confirm primitive deserialization and
         processing routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
*******************************************************************************/
ZsiProcessingResult_t zsiDeserializeZSE_ECCGenerateKeyConf(void *memory,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief KE-ZSE_ECCKeyBitGenerate confirm primitive deserialization and
         processing routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
*******************************************************************************/
ZsiProcessingResult_t zsiDeserializeZSE_ECCKeyBitGenerateConf(void *memory,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief KE-SSP_BcbHash confirm primitive deserialization and
         processing routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
*******************************************************************************/
ZsiProcessingResult_t zsiDeserializeSSP_BcbHashConf(void *memory,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief KE-SSP_KeyedHashMacConf confirm primitive deserialization and
         processing routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
*******************************************************************************/
ZsiProcessingResult_t zsiDeserializeSSP_KeyedHashMacConf(void *memory,
  ZsiCommandFrame_t *const cmdFrame);

#elif defined(ZAPPSI_NP)

/**************************************************************************//**
  \brief KE-ZSE_ECCGenerateKeyConf frame size calculation routine.
         SOF and LENGTH fields are dismissed.

  \param[in] req - request parameters.

  \return KE-ZSE_ECCGenerateKeyConf request frame size.
 ******************************************************************************/
INLINE uint16_t zsiZSE_ECCGenerateKeyConf(const void *const req)
{
  (void)req;
  return ZSI_COMMAND_FRAME_OVERHEAD + SECT163K1_PRIVATE_KEY_SIZE +
         SECT163K1_COMPRESSED_PUBLIC_KEY_SIZE;
}

/**************************************************************************//**
  \brief KE-ZSE_ECCKeyBitGenerateConf frame size calculation routine.
         SOF and LENGTH fields are dismissed.

  \param[in] req - request parameters.

  \return KE-ZSE_ECCGenerateKeyConf request frame size.
 ******************************************************************************/
INLINE uint16_t zsiZSE_ECCKeyBitGenerateConf(const void *const req)
{
  (void)req;
  return ZSI_COMMAND_FRAME_OVERHEAD + SECT163K1_SHARED_SECRET_SIZE;
}

/**************************************************************************//**
  \brief KE-SSP_BcbHashConf frame size calculation routine.
         SOF and LENGTH fields are dismissed.

  \param[in] req - request parameters.

  \return KE-SSP_BcbHashConf request frame size.
 ******************************************************************************/
INLINE uint16_t zsiSSP_BcbHashConf(const void *const req)
{
  (void)req;
  return ZSI_COMMAND_FRAME_OVERHEAD + AES_MMO_HASH_SIZE;
}

/**************************************************************************//**
  \brief KE-SSP_KeyedHashMacConf frame size calculation routine.
         SOF and LENGTH fields are dismissed.

  \param[in] req - request parameters.

  \return KE-SSP_KeyedHashMacConf request frame size.
 ******************************************************************************/
INLINE uint16_t zsiSSP_KeyedHashMacConf(const void *const req)
{
  (void)req;
  return ZSI_COMMAND_FRAME_OVERHEAD + AES_MMO_HASH_SIZE;
}

/**************************************************************************//**
  \brief KE-ZSE_ECCGenerateKeyReq request primitive deserialization and
         processing routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
*******************************************************************************/
ZsiProcessingResult_t zsiDeserializeZSE_ECCGenerateKeyReq(void *memory,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief KE-ZSE_ECCGenerateKeyConf primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeZSE_ECCGenerateKeyConf(const void *const req,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief KE-ZSE_ECCKeyBitGenerateReq request primitive deserialization and
         processing routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
*******************************************************************************/
ZsiProcessingResult_t zsiDeserializeZSE_ECCKeyBitGenerateReq(void *memory,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief KE-SSP_BcbHashReq request primitive deserialization and
         processing routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
*******************************************************************************/
ZsiProcessingResult_t zsiDeserializeSSP_BcbHashReq(void *memory,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief KE-SSP_KeyedHashMacConf primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeSSP_KeyedHashMacConf(const void *const req,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief SSP_KeyedHashMacReq request primitive deserialization and
         processing routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
*******************************************************************************/
ZsiProcessingResult_t zsiDeserializeSSP_KeyedHashMacReq(void *memory,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
\brief Confirm for SSP_KeyedHashMacReq.
******************************************************************************/
void zsiKeyedHashMacConf(void);

#endif /* ZAPPSI_NP */


#ifdef ZAPPSI_HOST
#define zsiDeserializeZSE_ECCGenerateKeyReq          NULL
#define zsiDeserializeZSE_ECCKeyBitGenerateReq       NULL
#define zsiDeserializeSSP_BcbHashReq                 NULL
#define zsiDeserializeSSP_KeyedHashMacReq            NULL

#elif defined(ZAPPSI_NP)
#define zsiDeserializeZSE_ECCGenerateKeyConf         NULL
#define zsiDeserializeZSE_ECCKeyBitGenerateConf      NULL
#define zsiDeserializeSSP_BcbHashConf                NULL
#define zsiDeserializeSSP_KeyedHashMacConf           NULL

#endif /* ZAPPSI_NP */

#endif // CERTICOM_SUPPORT == 1
#endif /* _ZSIKESERIALIZATION_H_ */
