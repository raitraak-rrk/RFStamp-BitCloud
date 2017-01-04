/**************************************************************************//**
  \file N_Security.c

  \brief Implementation of ZLL NWK key encryption and decryption.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    27.08.12 A. Razinkov - created
******************************************************************************/

/******************************************************************************
                    Includes section
******************************************************************************/
#include <sysTypes.h>
#include <sspCommon.h>
#include <sysUtils.h>

#include <N_Security_Bindings.h>
#include <N_Security.h>
#include <N_Security_Calc.h>
#include <N_ErrH.h>
#include <N_Log.h>

#include <devconfig.h>

/******************************************************************************
                    Defines section
******************************************************************************/
#define COMPID "N_Security"

/******************************************************************************
                    Implementation section
******************************************************************************/
/** Fills a buffer with random data.
    \param[in] pBuffer Pointer to buffer to fill with random data
    \param[in] bufferLength Number of bytes to fill with random data
*/
void N_Security_GetRandomData_Impl(uint8_t* pBuffer, uint8_t bufferLength)
{
  N_Security_Calc_Random(bufferLength, pBuffer);
}

/** Encrypts a network key. The returned key is ready for transmission over Interpan.
    \param[in] keyIndex Key index
    \param[in] transactionId The Transaction ID.
    \param[in] responseId The Response ID.
    \param[in] unencKey The unencrypted network key
    \param[out] encKey Buffer to keep the encrypted NWK key
    \param[in] done Encrypt done callback
*/
void N_Security_EncryptNetworkKey_Impl(uint8_t keyIndex, uint32_t transactionId, uint32_t responseId,
                                  N_Security_Key_t unencKey, N_Security_Key_t encKey, N_Security_DoneCallback_t done)
{
  N_Security_Calc_Error_t err;
  N_ERRH_ASSERT_FATAL((transactionId != 0uL) && (responseId != 0uL) && (keyIndex < 16u));

  err = N_Security_Calc_Encrypt(keyIndex, transactionId, responseId, unencKey, encKey, done);
  N_ERRH_ASSERT_FATAL(err == N_Security_Calc_ErrorOk);
}

/** Decrypts an encrypted network key received from Interpan. The returned key is ready to be used in the network.
    \param[in] keyIndex Key index
    \param[in] transactionId The Transaction ID.
    \param[in] responseId The Response ID.
    \param[in] encKey The encrypted network key
    \param[out] unencKey - Storage to put the decrypted network key into;
    \param[in] done Decrypt done callback
*/
void N_Security_DecryptNetworkKey_Impl(uint8_t keyIndex, uint32_t transactionId, uint32_t responseId,
                                  N_Security_Key_t encKey, N_Security_Key_t unencKey, N_Security_DoneCallback_t done)
{
  N_Security_Calc_Error_t err;
  N_ERRH_ASSERT_FATAL((transactionId != 0uL) && (responseId != 0uL) && (keyIndex < 16u));

  err = N_Security_Calc_Decrypt(keyIndex, transactionId, responseId, encKey, unencKey, done);
  N_ERRH_ASSERT_FATAL(err == N_Security_Calc_ErrorOk);
}

/** Gets the Distributed TrustCenter Link Key (e.g. the ZLL LinkKey).
    \param[out] pKey Location to write the LinkKey to (16 bytes)
    When proper ZLL security material is available, the production key is returned, else the certification key is returned.
*/
void N_Security_GetDistributedTrustCenterLinkKey(uint8_t* pKey, N_Security_DoneCallback_t done)
{
  N_Security_Calc_GetDistributedTrustCenterLinkKey(pKey, done);
}

/** Generates a random network key.
    \param[out] networkKey The new network key
*/
void N_Security_GenerateNewNetworkKey_Impl(N_Security_Key_t networkKey)
{
#if defined(DEVCFG_FIXED_NWK_KEY)
    #warning USING FIXED NETWORK KEY!
    uint8_t fixedKey[] = DEVCFG_FIXED_NWK_KEY;
    memcpy(networkKey, fixedKey, sizeof(N_Security_Key_t));
#else
    N_Security_GetRandomData(networkKey, SECURITY_KEY_SIZE);
#endif
}

/** Interface function, see \ref N_Security_GetTransactionId */
uint32_t N_Security_GetTransactionId_Impl(void)
{
  uint32_t transactionId;

  do
  {
    N_Security_GetRandomData((uint8_t*)&transactionId, sizeof(uint32_t));
  } while (transactionId == 0uL);
  return transactionId;
}

/** Interface function, see \ref N_Security_GetResponseId */
uint32_t N_Security_GetResponseId_Impl(void)
{
  uint32_t responseId;

  do
  {
    N_Security_GetRandomData((uint8_t*)&responseId, sizeof(uint32_t));
  } while (responseId == 0uL);
  return responseId;
}

/** Interface function, see \ref N_Security_GetKeyBitMask */
uint16_t N_Security_GetKeyBitMask_Impl(void)
{
  return N_Security_Calc_GetKeyBitMask();
}

/** Interface function, see \ref N_Security_IsCompatibleWithKeyMask */
bool N_Security_IsCompatibleWithKeyMask_Impl(uint16_t othersBitMask)
{
  uint16_t sharedKeys = N_Security_Calc_GetKeyBitMask() & othersBitMask;
  return N_UTIL_BOOL(sharedKeys != 0u);
}

/** Interface function, see \ref N_Security_FindSharedKeyIndex */
uint8_t N_Security_FindSharedKeyIndex_Impl(uint16_t othersBitMask)
{
  uint16_t sharedKeys = N_Security_Calc_GetKeyBitMask() & othersBitMask;
  N_ERRH_ASSERT_FATAL(sharedKeys != 0u);    // should not happen by design (ScanResponses should be discarded already)

  uint16_t mask = 0x8000u;
  uint8_t idx = 15u;
  while ((mask != 0x0000u) && ((sharedKeys & mask) == 0u))
  {
    mask >>= 1;
    idx--;
  }
  N_ERRH_ASSERT_FATAL(idx <= 15u); // did not wrap?
  return idx;
}

/** En/decrypt 16-byte data using a secret and unknown key.
    \param pIn [in] 16-byte input data
    \param pOut [in] 16-byte output data
    \param done [in] Callback on operation completion
    \note pIn==pOut is allowed
*/
void N_Security_Crypt16_Impl(const uint8_t* pIn, uint8_t* pOut, N_Security_DoneCallback_t done)
{
  N_Security_Calc_Crypt16(pIn, pOut, done);
}

/** Initialises the component. */
void N_Security_Init(void)
{
  const char *lib_version;

  N_Security_Calc_Error_t err = N_Security_Calc_Init(&lib_version);
  N_LOG_ALWAYS(( lib_version ));
  N_ERRH_ASSERT_FATAL(err == N_Security_Calc_ErrorOk);

  N_LOG_ALWAYS(("KeyBitMask,0x%04X", N_Security_GetKeyBitMask()));
}

/* eof N_Security.c */

