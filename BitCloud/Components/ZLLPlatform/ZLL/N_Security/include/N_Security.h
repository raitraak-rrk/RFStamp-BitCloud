/*********************************************************************************************//**
\file  

\brief 

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal

$Id: N_Security.h 24615 2013-02-27 05:14:24Z arazinkov $

***************************************************************************************************/

#ifndef N_SECURITY_H
#define N_SECURITY_H

/***************************************************************************************************
* INCLUDE FILES
***************************************************************************************************/

#include "N_Types.h"

/***************************************************************************************************
* C++ DECLARATION WRAPPER
***************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************************************
* EXPORTED TYPES
***************************************************************************************************/

typedef uint8_t N_Security_Key_t[16];
typedef void (*N_Security_DoneCallback_t)(void);

/***************************************************************************************************
* EXPORTED MACROS AND CONSTANTS
***************************************************************************************************/

/***************************************************************************************************
* EXPORTED FUNCTIONS
***************************************************************************************************/

/** Fills a buffer with random data.
    \param[in] pBuffer Pointer to buffer to fill with random data
    \param[in] bufferLength Number of bytes to fill with random data
*/
void N_Security_GetRandomData(uint8_t* pBuffer, uint8_t bufferLength);

/** Gets a random 32 bits TransactionID.
    \returns The generated transaction ID.
*/
uint32_t N_Security_GetTransactionId(void);

/** Gets a random 32 bits ResponseID.
    \returns The generated response ID.
*/
uint32_t N_Security_GetResponseId(void);

/** Generates a random network key.
    \param[out] networkKey The new network key
*/
void N_Security_GenerateNewNetworkKey(N_Security_Key_t networkKey);

/** Encrypts a network key. The returned key is ready for transmission over Interpan.
    \param[in] keyIndex Key index
    \param[in] transactionId The Transaction ID.
    \param[in] responseId The Response ID.
    \param[in] unencKey The unencrypted network key
    \param[out] encKey Buffer to keep the encrypted NWK key
    \param[in] done Encrypt done callback
*/
void N_Security_EncryptNetworkKey(uint8_t keyIndex, uint32_t transactionId, uint32_t responseId,
                                  N_Security_Key_t unencKey, N_Security_Key_t encKey, N_Security_DoneCallback_t done);

/** Decrypts an encrypted network key received from Interpan. The returned key is ready to be used in the network.
    \param[in] keyIndex Key index
    \param[in] transactionId The Transaction ID.
    \param[in] responseId The Response ID.
    \param[in] encKey The encrypted network key
    \param[out] unencKey - Storage to put the decrypted network key into;
    \param[in] done Decrypt done callback
*/
void N_Security_DecryptNetworkKey(uint8_t keyIndex, uint32_t transactionId, uint32_t responseId,
                                  N_Security_Key_t encKey, N_Security_Key_t unencKey, N_Security_DoneCallback_t done);

/** Gets the Distributed TrustCenter Link Key (e.g. the ZLL LinkKey).
    \param[out] pKey Location to write the LinkKey to (16 bytes)
    When proper ZLL security material is available, the production key is returned, else the certification key is returned.
*/
void N_Security_GetDistributedTrustCenterLinkKey(uint8_t* pKey, N_Security_DoneCallback_t done);

/** Gets the mask of supported keys.
    \returns The key bit-mask of the device
*/
uint16_t N_Security_GetKeyBitMask(void);

/** Check if the given keymask is compatible with our keymask.
    \param othersBitMask Other key bit-mask
    \return TRUE if compatible
*/
bool N_Security_IsCompatibleWithKeyMask(uint16_t othersBitMask);

/** Find the shared key index between our keymask and another keymask.
    \param othersBitMask Other key bit-mask
    \return The shared key index
    \note Keys must be compatible, see \ref N_Security_IsCompatibleWithKeyMask .
*/
uint8_t N_Security_FindSharedKeyIndex(uint16_t othersBitMask);

/** En/decrypt 16-byte data using a secret and unknown key.
    \param pIn [in] 16-byte input data
    \param pOut [in] 16-byte output data
    \param done [in] Callback on operation completion
    \note pIn==pOut is allowed
*/
void N_Security_Crypt16(const uint8_t* pIn, uint8_t* pOut, N_Security_DoneCallback_t done);

/***************************************************************************************************
* END OF C++ DECLARATION WRAPPER
***************************************************************************************************/

#ifdef __cplusplus
}
#endif

#endif // N_SECURITY_H
