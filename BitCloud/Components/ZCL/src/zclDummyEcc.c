/**************************************************************************//**
  \file zclDummyEcc.c

  \brief Stubs of Certicom library routines - always return the
         very same key

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      21/06/2010 D.Loskutnikov - Created
******************************************************************************/
#if ZCL_SUPPORT == 1
/******************************************************************************
                            Includes section.
******************************************************************************/
#include <stdint.h>
#include <string.h>
#include <genericEcc.h>
#include <sspHash.h>

/******************************************************************************
                            Definitions section.
******************************************************************************/

// answer to the ultimate question of life, the universe, and everything
#define DUMMY_FILL      42

#define MAX_YIELD_LEVEL 10

/******************************************************************************
                            Implementation section.
******************************************************************************/

/**************************************************************************//**
\brief  Creates an ECDSA signature of a message digest.
        The outputs are the r and s components of the signature.

\param[in] privateKey The private key. This is an unsigned char buffer of size
                      SECT163K1_PRIVATE_KEY_SIZE.
\param[in] msgDigest  The hash of the message to be signed. This is an unsigned
                      char buffer of size AES_MMO_HASH_SIZE.
\param[in] GetRandomData Pointer to a function to get random data for
                         generating ephemeral keys.
\param[in] yieldLevel The yield level determines how often the user defined yield
                      function will be called. This is a number from 0 to 10.
                      0  will never yield.
                      1  will  yield the most often.
                      10 will yield the least often.
\param[in] YieldFunc  Pointer to a function to allow user defined yielding.
\param[out] r The r component of the signature. This is an unsigned char buffer
              of size SECT163K1_POINT_ORDER_SIZE.
\param[out] s The s component of the signature. This is an unsigned char buffer
              of size SECT163K1_POINT_ORDER_SIZE.

\return MCE_ERR_NULL_PRIVATE_KEY    privateKey is NULL.
        MCE_ERR_NULL_OUTPUT_BUF     msgDigest, r or
                                    s are NULL.
        MCE_ERR_NULL_FUNC_PTR       GetRandomData is NULL or
                                    YieldFunc is NULL and
                                    YieldLevel is not 0.
        MCE_ERR_BAD_INPUT           YieldLevel is greater than 10.
        MCE_SUCCESS                 Success.
******************************************************************************/
int ZSE_ECDSASign(unsigned char *privateKey,
                  unsigned char *msgDigest,
                  GetRandomDataFunc *GetRandomData,
                  unsigned char *r,
                  unsigned char *s,
                  YieldFunc *yield,
                  unsigned long yieldLevel )
{
  if (!privateKey)
    return MCE_ERR_NULL_PRIVATE_KEY;

  if (!msgDigest || !r || !s)
    return MCE_ERR_NULL_OUTPUT_BUF;

  if (!GetRandomData)
    return MCE_ERR_NULL_FUNC_PTR;

  if (yieldLevel)
  {
    if (yieldLevel > MAX_YIELD_LEVEL)
      return MCE_ERR_BAD_INPUT;

    if (!yield)
      return MCE_ERR_NULL_FUNC_PTR;

    // Call just once, ignoring yieldLevel - just to catch possible errors with yield callback
    yield();
  }

  GetRandomData(r, SECT163K1_POINT_ORDER_SIZE);
  GetRandomData(s, SECT163K1_POINT_ORDER_SIZE);

  return MCE_SUCCESS;
}


/**************************************************************************//**
\brief  Verifies an ECDSA signature created using a private signing key by using
        the associated public key, the digest and the signature components.

\param[in] publicKey The public key. This is an unsigned char buffer of size
                     SECT163K1_COMPRESSED_PUBLIC_KEY_SIZE.
\param[in] msgDigest The hash of the message to be verified.  This is an
                     unsigned char buffer of size AES_MMO_HASH_SIZE.
\param[in] r         The r component of the signature. This is an unsigned char
                     buffer of size SECT163K1_POINT_ORDER_SIZE.
\param[in] s         The s component of the signature. This is an unsigned char
                     buffer of size SECT163K1_POINT_ORDER_SIZE.
\param[in] yieldLevel The yield level determines how often the user defined yield
                      function will be called. This is a number from 0 to 10.
                      0  will never yield.
                      1  will  yield the most often.
                      10 will yield the least often.
\param[in] YieldFunc  Pointer to a function to allow user defined yielding.
                      YieldFunc may be NULL if yieldLevel is 0.

\return MCE_ERR_FAIL_VERIFY        The signature verification failed.
        MCE_ERR_NULL_PUBLIC_KEY    publicKey is NULL.
        MCE_ERR_NULL_INPUT_BUF     msgDigest, r or
                                  s are NULL.
        MCE_ERR_NULL_FUNC_PTR      YieldFunc is NULL and
                                  YieldLevel is not 0.
        MCE_ERR_BAD_INPUT          YieldLevel is greater than 10.
        MCE_SUCCESS                Success.
******************************************************************************/
int ZSE_ECDSAVerify(unsigned char *publicKey,
                    unsigned char *msgDigest,
                    unsigned char *r,
                    unsigned char *s,
                    YieldFunc *yield,
                    unsigned long yieldLevel)
{
  if (!publicKey)
    return MCE_ERR_NULL_PUBLIC_KEY;

  if (!msgDigest || !r || !s)
    return MCE_ERR_NULL_INPUT_BUF;

  if (yieldLevel)
  {
    if (yieldLevel > MAX_YIELD_LEVEL)
      return MCE_ERR_BAD_INPUT;

    if (!yield)
      return MCE_ERR_NULL_FUNC_PTR;

    yield();
  }

  // Never return MCE_ERR_FAIL_VERIFY, since dummy library don't do any verify at all
  //return MCE_ERR_FAIL_VERIFY;

  return MCE_SUCCESS;
}

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
  if (!privateKey || !publicKey)
    return MCE_ERR_NULL_OUTPUT_BUF;

  if (!GetRandomData)
    return MCE_ERR_NULL_FUNC_PTR;

  if (yieldLevel)
  {
    if (yieldLevel > MAX_YIELD_LEVEL)
      return MCE_ERR_BAD_INPUT;

    if (!yield)
      return MCE_ERR_NULL_FUNC_PTR;

    // Call just once, ignoring yieldLevel - just to catch possible errors with yield callbacks
    yield();
  }

  // Just fill with random. Check of random generator call is a bonus
  GetRandomData(privateKey, SECT163K1_PRIVATE_KEY_SIZE);
  GetRandomData(publicKey, SECT163K1_COMPRESSED_PUBLIC_KEY_SIZE);

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
  uint8_t dummy[AES_MMO_HASH_SIZE];

  if (!privateKey)
    return MCE_ERR_NULL_PRIVATE_KEY;

  if (!ephemeralPrivateKey)
    return MCE_ERR_NULL_EPHEM_PRI_KEY;

  if (!ephemeralPublicKey || !remoteEphemeralPublicKey)
    return MCE_ERR_NULL_EPHEM_PUB_KEY;

  if (!remoteCertificate)
    return MCE_ERR_NULL_INPUT_BUF;

  if (!caPublicKey)
    return MCE_ERR_NULL_PUBLIC_KEY;

  if (!keyBits)
    return MCE_ERR_NULL_OUTPUT_BUF;

  if (yieldLevel)
  {
    if (yieldLevel > MAX_YIELD_LEVEL)
      return MCE_ERR_BAD_INPUT;

    if (!yield)
      return MCE_ERR_NULL_FUNC_PTR;

    yield();
  }

  // Call Hash to check callback
  Hash(dummy, SECT163K1_PRIVATE_KEY_SIZE, privateKey);

  memset(keyBits, DUMMY_FILL, SECT163K1_SHARED_SECRET_SIZE);

  return MCE_SUCCESS;
}

/**************************************************************************//**
\brief  Reconstructs the remote party's public key using its implicit
        certificate and the CA public key.

\param[in] certificate  Implicit certificate of the remote party. This is an
                        unsigned char buffer of size SECT163K1_CERTIFICATE_SIZE.
                        The static public key of the remote party is derived from
                        the certificate using the CA's public key.
\param[in] caPublicKey  Public key of the certificate authority. The static
                        public key of the remote party is derived from the
                        certificate using the CA's public key.
\param[out] publicKey   The derived public key. This is an unsigned char buffer
                        of size
                        SECT163K1_COMPRESSED_PUBLIC_KEY_SIZE.
\param[in] Hash         Pointer to a function to hash the certificate data.
\param[in] yieldLevel   The yield level determines how often the user defined
                        yield function will be called. This is a number from
                        0 to 10.
                        0  will never yield.
                        1  will  yield the most often.
                        10 will yield the least often.
\param[in] YieldFunc    Pointer to a function to allow user defined yielding.
                        YieldFunc may be NULL if
                        yieldLevel is 0.

\return MCE_ERR_NULL_INPUT_BUF        certificate is NULL.
        MCE_ERR_NULL_PUBLIC_KEY       caPublicKey is NULL.
        MCE_ERR_NULL_OUTPUT_BUF       publicKey is NULL.
        MCE_ERR_NULL_FUNC_PTR         Hash is NULL or
                                      YieldFunc is NULL and
                                      YieldLevel is not 0.
        MCE_ERR_BAD_INPUT             YieldLevel is greater than 10.
******************************************************************************/
int ZSE_ECQVReconstructPublicKey(unsigned char* certificate,
                                 unsigned char* caPublicKey,
                                 unsigned char* publicKey,
                                 HashFunc *Hash,
                                 YieldFunc *yield,
                                 unsigned long yieldLevel)
{
  uint8_t dummy[AES_MMO_HASH_SIZE];

  if (!certificate)
    return MCE_ERR_NULL_INPUT_BUF;

  if (!caPublicKey)
    return MCE_ERR_NULL_PUBLIC_KEY;

  if (!publicKey)
    return MCE_ERR_NULL_OUTPUT_BUF;

  if (yieldLevel)
  {
    if (yieldLevel > MAX_YIELD_LEVEL)
      return MCE_ERR_BAD_INPUT;

    if (!yield)
      return MCE_ERR_NULL_FUNC_PTR;

    yield();
  }

  // Call Hash to check callback
  Hash(dummy, SECT163K1_CERTIFICATE_SIZE, certificate);

  memset(publicKey, DUMMY_FILL, SECT163K1_COMPRESSED_PUBLIC_KEY_SIZE);

  // Guess we should return MCE_SUCCESS if all ok (not stated in doc)
  return MCE_SUCCESS;
}
#endif // ZCL_SUPPORT == 1
//eof zclDummyEcc.c
