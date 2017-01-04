/***************************************************************************//**
  \file zsiKe.c

  \brief ZAppSI Certicom API wrapper.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    29.11.2012  A. Khromykh - Created.
   Last change:
   $Id: zsiKe.c 26492 2014-03-26 10:49:30Z karthik.p_u $
 ******************************************************************************/

/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <apsdeEndpoint.h>
#include <zsiDriver.h>
#include <zsiDbg.h>
#include <sysQueue.h>
#include <sysTaskManager.h>
#include <zsiKeSerialization.h>

#if CERTICOM_SUPPORT == 1
/******************************************************************************
                            Definitions section.
******************************************************************************/
#define MAX_YIELD_LEVEL  10

/******************************************************************************
                   External global variables section
******************************************************************************/
#if defined(ZAPPSI_HOST)
extern ExtAddr_t keRemoteExtAddr;
#elif  defined(ZAPPSI_NP)
extern SSP_KeyedHashMacReq_t zsiKeyedHashMacReq;
#endif

/******************************************************************************
                              Static variables section
******************************************************************************/
#if defined(ZAPPSI_HOST)
static ZsiCerticomMem_t certicomMem;
#endif

static PROGMEM_DECLARE(ZsiProcessingRoutine_t zsiKeProcessingRoutines[]) =
{
  [ZSI_KE_ECC_GENERATE_KEY_REQ]      = zsiDeserializeZSE_ECCGenerateKeyReq,
  [ZSI_KE_ECC_GENERATE_KEY_CONF]     = zsiDeserializeZSE_ECCGenerateKeyConf,
  [ZSI_KE_ECC_KEY_BIT_GENERATE_REQ]  = zsiDeserializeZSE_ECCKeyBitGenerateReq,
  [ZSI_KE_ECC_KEY_BIT_GENERATE_CONF] = zsiDeserializeZSE_ECCKeyBitGenerateConf,
  [ZSI_KE_SSP_BCB_HASH_REQ]          = zsiDeserializeSSP_BcbHashReq,
  [ZSI_KE_SSP_BCB_HASH_CONF]         = zsiDeserializeSSP_BcbHashConf,
  [ZSI_KE_SSP_KEYED_HASH_MAC_REQ]    = zsiDeserializeSSP_KeyedHashMacReq,
  [ZSI_KE_SSP_KEYED_HASH_MAC_CONF]   = zsiDeserializeSSP_KeyedHashMacConf,
};


/******************************************************************************
                               Implementation section
 ******************************************************************************/
/**************************************************************************//**
  \brief Finds routine for KE commands deserialization.

  \param[in] commandId - command identifier.

  \return Pointer to appropriate routine.
 ******************************************************************************/
ZsiProcessingRoutine_t zsiKeFindProcessingRoutine(uint8_t commandId)
{
  ZsiProcessingRoutine_t routine = NULL;

  if (commandId < ARRAY_SIZE(zsiKeProcessingRoutines))
  {
    memcpy_P(&routine, &zsiKeProcessingRoutines[commandId],
             sizeof(routine));
  }

  return routine;
}

#ifdef ZAPPSI_HOST
/**************************************************************************//**
\brief  Generates an ephemeral key pair using the specified random data
        generation function.

\param[out] privateKey   The generated private key. This is an unsigned char
                         buffer of size SECT163K1_PRIVATE_KEY_SIZE.
\param[out] publicKey    The generated public key. This is an unsigned char
                         buffer of size
                         SECT163K1_COMPRESSED_PUBLIC_KEY_SIZE.
\param[in] GetRandomData Pointer to a function to get random data for
                         generating the ephemeral key pair.
\param[in] yieldLevel The yield level determines how often the user defined yield
                      function will be called. This is a number from 0 to 10.
                      0  will never yield.
                      1  will yield the most often.
                      10 will yield the least often.
\param[in] YieldFunc  Pointer to a function to allow user defined yielding.
                      YieldFunc may be NULL if yieldLevel is 0.

\return MCE_ERR_NULL_OUTPUT_BUF    privateKey or publicKey
                                   are NULL.
        MCE_ERR_NULL_FUNC_PTR      GetRandomData is NULL or
                                   YieldFunc is NULL and
                                   YieldLevel is not 0.
        MCE_ERR_BAD_INPUT          YieldLevel is greater than 10.
        MCE_SUCCESS                Success.

******************************************************************************/
int ZSE_ECCGenerateKey(unsigned char *privateKey,
                       unsigned char *publicKey,
                       GetRandomDataFunc *GetRandomData,
                       YieldFunc *yield,
                       unsigned long yieldLevel)
{
  ZsiEccGenerateKey_t *zsiEccGenerateKey = &certicomMem.zsiEccGenerateKey;
  (void)GetRandomData;

  sysAssert((privateKey && publicKey), ZSIKESERIALIZATION_ZSEECCGENERATEKEY);

  zsiEccGenerateKey->zsiKeRemoteExtAddr = keRemoteExtAddr;
  zsiProcessCommand(ZSI_SREQ_CMD, (void *const)zsiEccGenerateKey, zsiSerializeZSE_ECCGenerateKeyReq, zsiEccGenerateKey);

  memcpy(privateKey, zsiEccGenerateKey->privateKey, SECT163K1_PRIVATE_KEY_SIZE);
  memcpy(publicKey, zsiEccGenerateKey->publicKey, SECT163K1_COMPRESSED_PUBLIC_KEY_SIZE);

  (void)yield;
  (void)yieldLevel;
  return MCE_SUCCESS;
}

/**************************************************************************//**
\brief  Derives a shared secret using the ECMQV algorithm. The public key
        of the  remote party is reconstructed using its implicit certificate
        and the CA public key.

\param[in] privateKey  The static private key of the local entity. This is an
                       unsigned char buffer of size
                       SECT163K1_PRIVATE_KEY_SIZE.
\param[in] ephemeralPrivateKey   The ephemeral private key of the local entity.
                                 It should be generated using a previous call
                                 to the function ECCGenerateKey. An
                                 unsigned char buffer of size
                                 SECT163K1_PRIVATE_KEY_SIZE.
\param[in] ephemeralPublicKey    The ephemeral public key of the local entity.
                                 It should be generated using a previous call
                                 to the function ECCGenerateKey. An
                                 unsigned char buffer of size
                                 SECT163K1_COMPRESSED_PUBLIC_KEY_SIZE.
\param[in] remoteCertificate     Implicit certificate of the remote party.
                                 This is an unsigned char buffer of size
                                 SECT163K1_CERTIFICATE_SIZE. The
                                 static public key of the remote party is
                                 derived from the certificate using the CA's
                                 public key.
\param[in] remoteEphemeralPublicKey  Ephemeral public key received from the
                                     remote party. This is an unsigned char
                                     buffer of size
                                 SECT163K1_COMPRESSED_PUBLIC_KEY_SIZE.
\param[in] caPublicKey Public key of the certificate authority. The static
                       public key for the remote party is derived from the
                       certificate using the CA's public key.
\param[out] keyBits    The derived shared secret. This is an unsigned char
                       buffer of size SECT163K1_SHARED_SECRET_SIZE.
\param[in] Hash        Pointer to a function to hash the certificate data.
\param[in] yieldLevel The yield level determines how often the user defined yield
                      function will be called. This is a number from 0 to 10.
                      0  will never yield.
                      1  will  yield the most often.
                      10 will yield the least often.
\param[in] YieldFunc  Pointer to a function to allow user defined yielding.
                      YieldFunc may be NULL if yieldLevel is 0.

\return MCE_ERR_NULL_PRIVATE_KEY      privateKey is NULL.
        MCE_ERR_NULL_EPHEM_PRI_KEY    ephemeralPrivateKey is
                                      NULL
        MCE_ERR_NULL_EPHEM_PUB_KEY    ephemeralPublicKey or
                                      remoteEphemeralPublicKey are
                                      NULL.
        MCE_ERR_NULL_INPUT_BUF        remoteCertificate is
                                      NULL.
        MCE_ERR_NULL_PUBLIC_KEY       caPublicKey is NULL.
        MCE_ERR_NULL_OUTPUT_BUF       keyBits is NULL.
        MCE_ERR_NULL_FUNC_PTR         Hash is NULL or
                                      YieldFunc is NULL and
                                      YieldLevel is not 0.
        MCE_ERR_BAD_INPUT             YieldLevel is greater than 10.
        MCE_SUCCESS                   Success.
******************************************************************************/
int ZSE_ECCKeyBitGenerate(unsigned char *privateKey,
                          unsigned char *ephemeralPrivateKey,
                          unsigned char *ephemeralPublicKey,
                          unsigned char *remoteCertificate,
                          unsigned char *remoteEphemeralPublicKey,
                          unsigned char *caPublicKey,
                          unsigned char *keyBits,
                          HashFunc *Hash,
                          YieldFunc *yield,
                          unsigned long yieldLevel)
{
  ZsiEccKeyBitGenerate_t *zsiEccKeyBitGenerate = &certicomMem.zsiEccKeyBitGenerate;
  (void)Hash;

  sysAssert((privateKey && ephemeralPrivateKey && ephemeralPublicKey &&
             remoteEphemeralPublicKey && remoteCertificate &&
             caPublicKey && keyBits), ZSIKESERIALIZATION_ZSEECCKEYBITGENERATE);

  memcpy(zsiEccKeyBitGenerate->privateKey, privateKey, SECT163K1_PRIVATE_KEY_SIZE);
  memcpy(zsiEccKeyBitGenerate->localEphemeralPrivateKey, ephemeralPrivateKey, SECT163K1_PRIVATE_KEY_SIZE);
  memcpy(zsiEccKeyBitGenerate->localEphemeralPublicKey, ephemeralPublicKey, SECT163K1_COMPRESSED_PUBLIC_KEY_SIZE);
  memcpy(zsiEccKeyBitGenerate->remoteCertificate, remoteCertificate, SECT163K1_CERTIFICATE_SIZE);
  memcpy(zsiEccKeyBitGenerate->remoteEphemeralPublicKey, remoteEphemeralPublicKey, SECT163K1_COMPRESSED_PUBLIC_KEY_SIZE);
  memcpy(zsiEccKeyBitGenerate->publicKey, caPublicKey, SECT163K1_COMPRESSED_PUBLIC_KEY_SIZE);

  zsiProcessCommand(ZSI_SREQ_CMD, (void *const)zsiEccKeyBitGenerate, zsiSerializeZSE_ECCKeyBitGenerateReq, zsiEccKeyBitGenerate);

  memcpy(keyBits, zsiEccKeyBitGenerate->localKeyBits, SECT163K1_SHARED_SECRET_SIZE);

  (void)yield;
  (void)yieldLevel;
  return MCE_SUCCESS;
}

/**************************************************************************\\**
  \brief This function computes the AES MMO digest of the data parameter of
  length <tt>sz</tt>, and stores the result in.

  \param[out] digest  This is an unsigned char buffer to hold the message digest.
                      The length of the digest must be AES_MMO_HASH_SIZE bytes.
  \param[in] sz       The size in bytes of the message to be hashed.
  \param[in] data     This is an unsigned char buffer of data to be hashed.

  \return MCE_ERR_NULL_OUTPUT_BUF  digest is NULL
          MCE_ERR_NULL_INPUT_BUF   data is NULL
          MCE_SUCCESS              indicates successful completion.

******************************************************************************/
int SSP_BcbHash(unsigned char *digest, unsigned long sz, unsigned char *data)
{
  ZsiSspBcbHash_t bcbHashReq;

  sysAssert((digest && sz && data), ZSIKESERIALIZATION_SSPBCBHASH);

  bcbHashReq.size = sz;
  bcbHashReq.data = data;
  zsiProcessCommand(ZSI_SREQ_CMD, (void *const)&bcbHashReq, zsiSerializeSSP_BcbHashReq, digest);

  return 0;
}

/**************************************************************************//**
  \brief SSP_KeyedHashMacReq handler wrapper routine.

  \param[in] req - primitive pointer.
 ******************************************************************************/
void SSP_KeyedHashMacReq(SSP_KeyedHashMacReq_t *req)
{
  zsiProcessCommand(ZSI_AREQ_CMD, req, zsiSerializeSSP_KeyedHashMacReq, req);
}

#elif defined(ZAPPSI_NP)

/**************************************************************************//**
\brief Confirm for SSP_KeyedHashMacReq.
******************************************************************************/
void zsiKeyedHashMacConf(void)
{
  zsiProcessCommand(ZSI_AREQ_CMD, &zsiKeyedHashMacReq, zsiSerializeSSP_KeyedHashMacConf, NULL);
}
#endif /* ZAPPSI_NP */

#endif // CERTICOM_SUPPORT == 1
/* eof zsiKe.c */
