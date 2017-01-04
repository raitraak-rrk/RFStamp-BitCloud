/**************************************************************************//**
  \file N_Security_Calc.c

  \brief implementation of ZLL NWK key encryption/decryption.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    05.09.12 N. Fomin - created
    23.11.12 M. Gekk - Support AES Engine mutex.
******************************************************************************/
/******************************************************************************
                    Include section
******************************************************************************/
#include <N_Security_Calc.h>
#include <sspAesHandler.h>
#include <sysMutex.h>
#include <sysUtils.h>

#include <N_ErrH.h>

/******************************************************************************
                    Definition section
******************************************************************************/
#if !(defined(_HAL_HW_AES_) || defined(_MAC_HW_AES_))
  #error AES encryption/decryption implementation is required
#endif // !defined(_HAL_HW_AES_)

#define COMPID "N_Security_Calc"

/*! Certification key. Used for ZLL transport key obtaining */
#define ZLL_SECURITY_KEY "\xC0\xC1\xC2\xC3\xC4\xC5\xC6\xC7\xC8\xC9\xCA\xCB\xCC\xCD\xCE\xCF"

#define ZLL_SUPPORTED_SECURITY_KEYS_BITMASK ((1u << ZLL_CERTIFICATION_KEY_INDEX) | (1u << ZLL_DEVELOPMENT_KEY_INDEX))

// /*! NWK key */
// #define ZLL_NWK_KEY      "\x11\x22\x33\x44\x55\x66\x77\x88\x99\xAA\xBB\xCC\xDD\xEE\xFF\x00"
// /*! Certification pre-installed link key. To encrypt NWK key in case of non-ZLL device joining */
#define ZLL_PREINSTALLED_LINK_KEY "\xD0\xD1\xD2\xD3\xD4\xD5\xD6\xD7\xD8\xD9\xDA\xDB\xDC\xDD\xDE\xDF"
// /*! HA Network default TC link key. To used inside the HA network */
// #define ZLL_DEFAULT_HA_TC_LINK_KEY "\x5a\x69\x67\x42\x65\x65\x41\x6c\x6c\x69\x61\x6e\x63\x65\x30\x39"

#define N_SECURITY_CALC_SECURITY_BLOCK_SIZE 16u

#define LIBVERSION "Lib.Atmel.Dev"

#define SECURITY_MEMORY_POOL_SIZE 2u

/******************************************************************************
                    Types section
******************************************************************************/
typedef struct
{
  AES_ENCRYPT_REQ_T    encryptReq;
  N_Security_Calc_DoneCallback_t cb;
  bool                 encrypt;
  uint32_t             rId;
  uint32_t             tId;
  uint8_t              tempBuffer[SECURITY_KEY_SIZE];
  uint8_t              key[SECURITY_KEY_SIZE];
  SYS_MutexOwner_t     mutexOwner;
  uint8_t              keyIndex;
  uint8_t              *resultKey;
  bool                 busy;
} SecurityMem_t;

/******************************************************************************
                    Static variables section
******************************************************************************/
static SecurityMem_t securityMemPool[SECURITY_MEMORY_POOL_SIZE];
static SecurityMem_t *actualSecurityMem;
DECLARE_MUTEX(aesEngineMutex);

/******************************************************************************
                    Prototypes section
******************************************************************************/
static void nSecuritySetCryptionKeyForTransportKey(SecurityMem_t *securityMem);
static void nSecuritySetCryptionKeyForTransportKeyConf(void);
static void nSecurityCalculateTransportKeyConf(void);
static void nSecuritySetCryptionKeyForNetworkKeyConf(void);
static void nSecurityEncryptOrDecryptNetworkKeyConf(void);
static void nSecuritySetCryptionKeyForNetworkKey(SecurityMem_t *securityMem);
static void nSecurityMutexLockConf(SYS_Mutex_t *const mutex, void *context);

/******************************************************************************
                    Implementation section
******************************************************************************/
static inline SecurityMem_t* getSecurityMem(void)
{
  for (uint8_t mem = 0; mem < SECURITY_MEMORY_POOL_SIZE; mem++)
  {
    if (!securityMemPool[mem].busy)
    {
      securityMemPool[mem].busy = true;
      return &securityMemPool[mem];
    }
  }

  N_ERRH_ASSERT_FATAL(0);    // Not enough memory for security
  return NULL;
}

static inline void freeSecurityMem(SecurityMem_t* mem)
{
  mem->busy = false;
}

/**************************************************************************//**
\brief Initializes the security library

\param[out] version - The security version string.
******************************************************************************/
N_Security_Calc_Error_t N_Security_Calc_Init(const char** version)
{
  for (uint8_t mem = 0; mem < SECURITY_MEMORY_POOL_SIZE; mem++)
  {
    securityMemPool[mem].mutexOwner.SYS_MutexLockConf = nSecurityMutexLockConf;
    securityMemPool[mem].mutexOwner.context = &securityMemPool[mem];
  }

  *version = LIBVERSION;
  return N_Security_Calc_ErrorOk;
}

/**************************************************************************//**
\brief Returns supported security key bit mask.

\return supported key bit mask.
******************************************************************************/
uint16_t N_Security_Calc_GetKeyBitMask(void)
{
  return ZLL_SUPPORTED_SECURITY_KEYS_BITMASK;
}

/**************************************************************************//**
\brief Returns distributed trust center key.

\param[out] pKey - pointer to the memory where key shall be placed.
******************************************************************************/
void N_Security_Calc_GetDistributedTrustCenterLinkKey(uint8_t* pKey, N_Security_Calc_DoneCallback_t callback)
{
  char const zllLinkKey[] = ZLL_PREINSTALLED_LINK_KEY;

  memcpy(pKey, zllLinkKey, SECURITY_KEY_SIZE);
  callback();
}

/**************************************************************************//**
\brief Encrypt security key

\param[in] keyIndex - index of ZLL key
\param[in] transactionID - transaction id to be used to calculate the transport key;
\param[in] responseID - esponse id to be used to calculate the transport key;
\param[in] pNwkKey - key to be encrypted;
\param[out] pEncNwkKey - Storage to put the encrypted network key into;
\param[in] callback - function to be called after encryption is finished
******************************************************************************/
N_Security_Calc_Error_t N_Security_Calc_Encrypt(uint8_t keyIndex,
                                           uint32_t transactionID,
                                           uint32_t responseID,
                                           uint8_t* pNwkKey,
                                           uint8_t* pEncNwkKey,
                                           N_Security_Calc_DoneCallback_t callback)
{
  SecurityMem_t *securityMem = getSecurityMem();

  memcpy(securityMem->key, pNwkKey, SECURITY_KEY_SIZE);
  securityMem->encrypt         = true;
  securityMem->cb              = callback;
  securityMem->rId             = responseID;
  securityMem->tId             = transactionID;
  securityMem->keyIndex        = keyIndex;
  securityMem->resultKey       = pEncNwkKey;
  /* calculation of the transport key using transaction and response ids as
     data to be encrypted and ZLL certification key as key */
  if (SYS_MutexLock(&aesEngineMutex, &securityMem->mutexOwner))
    nSecurityMutexLockConf(&aesEngineMutex, securityMem);

  return N_Security_Calc_ErrorOk;
}

/**************************************************************************//**
\brief Decrypt security key

\param[in] keyIndex - index of ZLL key
\param[in] transactionID - transaction id to be used to calculate the transport key;
\param[in] responseID - esponse id to be used to calculate the transport key;
\param[in] pEncNwkKey - key to be decrypted;
\param[out] pNwkKey - Storage to put the decrypted network key into;
\param[in] callback - function to be called after encryption is finished
******************************************************************************/
N_Security_Calc_Error_t N_Security_Calc_Decrypt(uint8_t keyIndex,
                                           uint32_t transactionID,
                                           uint32_t responseID,
                                           uint8_t* pEncNwkKey,
                                           uint8_t* pNwkKey,
                                           N_Security_Calc_DoneCallback_t callback)
{
  SecurityMem_t *securityMem = getSecurityMem();

  memcpy(securityMem->key, pEncNwkKey, SECURITY_KEY_SIZE);
  securityMem->encrypt         = false;
  securityMem->cb              = callback;
  securityMem->rId             = responseID;
  securityMem->tId             = transactionID;
  securityMem->keyIndex        = keyIndex;
  securityMem->resultKey       = pNwkKey;
  /* calculation of the transport key using transaction and response ids as
     data to be encrypted and ZLL certification key as key */
  if (SYS_MutexLock(&aesEngineMutex, &securityMem->mutexOwner))
    nSecurityMutexLockConf(&aesEngineMutex, securityMem);

  return N_Security_Calc_ErrorOk;
}

/**************************************************************************//**
\brief Returns random bytes usable for security purposes

\param[in] bytes - the number of bytes to return
\param[out] pKey - pointer to the memory where random bytes shall be placed.

\note This function's execution time is quite high (est. 4us per byte).
******************************************************************************/
void N_Security_Calc_Random(uint8_t bytes, uint8_t* random)
{
  SYS_GetRandomSequence(random, bytes);
};

/**************************************************************************//**
\brief En/decrypt 16-byte data using a secret and unknown key. pIn==pOut is allowed

\param[in] pIn - 16-byte input data
\param[in] pOut - 16-byte output data
\param[in] done - callback on operation completion
******************************************************************************/
void N_Security_Calc_Crypt16(const uint8_t* pIn, uint8_t* pOut, N_Security_Calc_DoneCallback_t done)
{
  for (uint8_t i = 0; i < N_SECURITY_CALC_SECURITY_BLOCK_SIZE; i++)
  {
    pOut[i] = pIn[i] ^ 0x2a;
  }
  done();
}

/**************************************************************************//**
  \brief AES Engine is ready.

  \param[in] mutex - the pointer to the AES Engine mutex.
  \param[in] context - the pointer of owner's context.
 ******************************************************************************/
static void nSecurityMutexLockConf(SYS_Mutex_t *const mutex, void *context)
{
  SecurityMem_t *securityMem = (SecurityMem_t*)context;

  if (ZLL_DEVELOPMENT_KEY_INDEX == securityMem->keyIndex)
    nSecuritySetCryptionKeyForNetworkKey(securityMem);
  else
    nSecuritySetCryptionKeyForTransportKey(securityMem);

  (void)mutex;
}

/**************************************************************************//**
\brief Sets cryption key for calculation of the transport key

\param[in] securityMem - related security memory, keeps operation context.
******************************************************************************/
static void nSecuritySetCryptionKeyForTransportKey(SecurityMem_t *securityMem)
{
  char const zllKey[] = ZLL_SECURITY_KEY;

  memcpy(securityMem->tempBuffer, zllKey, SECURITY_KEY_SIZE);

  securityMem->encryptReq.text              = securityMem->tempBuffer;
#ifdef _HAL_HW_AES_
  securityMem->encryptReq.command           = SM_SET_KEY_COMMAND;
  securityMem->encryptReq.HAL_SmEncryptConf = nSecuritySetCryptionKeyForTransportKeyConf;
#endif
#ifdef _MAC_HW_AES_
  securityMem->encryptReq.aesCmd            = RF_SET_KEY_AES_CMD;
  securityMem->encryptReq.RF_EncryptConf    = nSecuritySetCryptionKeyForTransportKeyConf;
#endif

  actualSecurityMem = securityMem;

  AES_SET_KEY_REQ(&securityMem->encryptReq);
}

/**************************************************************************//**
\brief Confirmation about cryption key setting; creates request to calculate
       the transport key
******************************************************************************/
static void nSecuritySetCryptionKeyForTransportKeyConf(void)
{
  actualSecurityMem->tempBuffer[0]  = actualSecurityMem->tempBuffer[4]  = (actualSecurityMem->tId>> 24)  & 0xff;
  actualSecurityMem->tempBuffer[1]  = actualSecurityMem->tempBuffer[5]  = (actualSecurityMem->tId >> 16) & 0xff;
  actualSecurityMem->tempBuffer[2]  = actualSecurityMem->tempBuffer[6]  = (actualSecurityMem->tId >> 8)  & 0xff;
  actualSecurityMem->tempBuffer[3]  = actualSecurityMem->tempBuffer[7]  = (actualSecurityMem->tId >> 0)  & 0xff;
  actualSecurityMem->tempBuffer[8]  = actualSecurityMem->tempBuffer[12] = (actualSecurityMem->rId >> 24) & 0xff;
  actualSecurityMem->tempBuffer[9]  = actualSecurityMem->tempBuffer[13] = (actualSecurityMem->rId >> 16) & 0xff;
  actualSecurityMem->tempBuffer[10] = actualSecurityMem->tempBuffer[14] = (actualSecurityMem->rId >> 8)  & 0xff;
  actualSecurityMem->tempBuffer[11] = actualSecurityMem->tempBuffer[15] = (actualSecurityMem->rId >> 0)  & 0xff;

#ifdef _HAL_HW_AES_
  actualSecurityMem->encryptReq.command           = SM_ECB_ENCRYPT_COMMAND;
  actualSecurityMem->encryptReq.HAL_SmEncryptConf = nSecurityCalculateTransportKeyConf;
#endif
#ifdef _MAC_HW_AES_
  actualSecurityMem->encryptReq.aesCmd            = RF_ECB_ENCRYPT_AES_CMD;
  actualSecurityMem->encryptReq.RF_EncryptConf    = nSecurityCalculateTransportKeyConf;
#endif
  actualSecurityMem->encryptReq.text              = actualSecurityMem->tempBuffer;
  actualSecurityMem->encryptReq.blockCount        = 1;

  AES_ENCRYPT_REQ(&actualSecurityMem->encryptReq);
}

/**************************************************************************//**
\brief Confirmation about calculation of the transport key; sets key to
       encrypt/decrypt network key
******************************************************************************/
static void nSecurityCalculateTransportKeyConf(void)
{
  // Previously encrypted text is used as a key here
#ifdef _HAL_HW_AES_
  actualSecurityMem->encryptReq.command           = SM_SET_KEY_COMMAND;
  actualSecurityMem->encryptReq.HAL_SmEncryptConf = nSecuritySetCryptionKeyForNetworkKeyConf;
#endif
#ifdef _MAC_HW_AES_
  actualSecurityMem->encryptReq.aesCmd            = RF_SET_KEY_AES_CMD;
  actualSecurityMem->encryptReq.RF_EncryptConf    = nSecuritySetCryptionKeyForNetworkKeyConf;
#endif

  AES_ENCRYPT_REQ(&actualSecurityMem->encryptReq);
}

/**************************************************************************//**
\brief Confirmation about setting cryption key; creates request to
       encrypt/decrypt network key
******************************************************************************/
static void nSecuritySetCryptionKeyForNetworkKeyConf(void)
{
#ifdef _HAL_HW_AES_
  if (actualSecurityMem->encrypt)
    actualSecurityMem->encryptReq.command = SM_ECB_ENCRYPT_COMMAND;
  else
    actualSecurityMem->encryptReq.command = SM_ECB_DECRYPT_COMMAND;
  actualSecurityMem->encryptReq.HAL_SmEncryptConf = nSecurityEncryptOrDecryptNetworkKeyConf;
#endif
#ifdef _MAC_HW_AES_
  if (actualSecurityMem->encrypt)
    actualSecurityMem->encryptReq.aesCmd  = RF_ECB_ENCRYPT_AES_CMD;
  else
    actualSecurityMem->encryptReq.aesCmd  = RF_ECB_DECRYPT_AES_CMD;
  actualSecurityMem->encryptReq.RF_EncryptConf = nSecurityEncryptOrDecryptNetworkKeyConf;
#endif
  
  actualSecurityMem->encryptReq.text              = actualSecurityMem->key;
  actualSecurityMem->encryptReq.blockCount        = 1;

  AES_ENCRYPT_REQ(&actualSecurityMem->encryptReq);
}

/**************************************************************************//**
\brief Confirmation about encription/decryption of network key completion
******************************************************************************/
static void nSecurityEncryptOrDecryptNetworkKeyConf(void)
{
  SecurityMem_t *ptr = actualSecurityMem;

  memcpy(actualSecurityMem->resultKey, actualSecurityMem->encryptReq.text, SECURITY_KEY_SIZE);
  actualSecurityMem->cb();
  SYS_MutexUnlock(&aesEngineMutex, &ptr->mutexOwner);
  freeSecurityMem(ptr);
}

/**************************************************************************//**
\brief Sets cryption key to encrypt/decrypt development key

\param[in] securityMem - related security memory, keeps operation context.
******************************************************************************/
static void nSecuritySetCryptionKeyForNetworkKey(SecurityMem_t *securityMem)
{
  securityMem->tempBuffer[0]  = 0x50;
  securityMem->tempBuffer[1]  = 0x68;
  securityMem->tempBuffer[2]  = 0x4C;
  securityMem->tempBuffer[3]  = 0x69;
  securityMem->tempBuffer[4]  = (securityMem->tId>> 24)  & 0xff;
  securityMem->tempBuffer[5]  = (securityMem->tId >> 16) & 0xff;
  securityMem->tempBuffer[6]  = (securityMem->tId >> 8)  & 0xff;
  securityMem->tempBuffer[7]  = (securityMem->tId >> 0)  & 0xff;
  securityMem->tempBuffer[8]  = 0x43;
  securityMem->tempBuffer[9]  = 0x4C;
  securityMem->tempBuffer[10] = 0x53;
  securityMem->tempBuffer[11] = 0x4E;
  securityMem->tempBuffer[12] = (securityMem->rId >> 24) & 0xff;
  securityMem->tempBuffer[13] = (securityMem->rId >> 16) & 0xff;
  securityMem->tempBuffer[14] = (securityMem->rId >> 8)  & 0xff;
  securityMem->tempBuffer[15] = (securityMem->rId >> 0)  & 0xff;

#ifdef _HAL_HW_AES_
  securityMem->encryptReq.command           = SM_SET_KEY_COMMAND;
  securityMem->encryptReq.HAL_SmEncryptConf = nSecuritySetCryptionKeyForNetworkKeyConf;
#endif 
#ifdef _MAC_HW_AES_
  securityMem->encryptReq.aesCmd            = RF_SET_KEY_AES_CMD;
  securityMem->encryptReq.RF_EncryptConf    = nSecuritySetCryptionKeyForNetworkKeyConf;
#endif
  securityMem->encryptReq.text              = securityMem->tempBuffer;
  securityMem->encryptReq.blockCount        = 1;

  actualSecurityMem = securityMem;

  AES_ENCRYPT_REQ(&securityMem->encryptReq);
}

//eof N_Security_Calc.c
