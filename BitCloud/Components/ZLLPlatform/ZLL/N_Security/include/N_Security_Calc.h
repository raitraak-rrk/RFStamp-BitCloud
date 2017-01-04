/*********************************************************************************************//**
\file  

\brief 

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal

$Id: N_Security_Calc.h 24615 2013-02-27 05:14:24Z arazinkov $

***************************************************************************************************/

#ifndef N_SECURITY_CALC_H
#define N_SECURITY_CALC_H

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

/******************************************************************************
                    Defines section
******************************************************************************/
#define ZLL_DEVELOPMENT_KEY_INDEX        0
#define ZLL_MASTER_KEY_INDEX             4
#define ZLL_CERTIFICATION_KEY_INDEX      15

/***************************************************************************************************
* EXPORTED TYPES
***************************************************************************************************/

typedef enum
{
    N_Security_Calc_ErrorOk          = 0,    //< All went well
    N_Security_Calc_Reserved         = 1,    //<
    N_Security_Calc_NoKeyAvailable   = 2,    //<
    N_Security_Calc_SystemError      = 3     //<
} N_Security_Calc_Error_t;

typedef void (*N_Security_Calc_DoneCallback_t)(void);

/***************************************************************************************************
* EXPORTED FUNCTIONS
***************************************************************************************************/

/**************************************************************************//**
\brief Initializes the security library

\param[out] version - The security version string.
******************************************************************************/
N_Security_Calc_Error_t N_Security_Calc_Init(const char** version);

/**************************************************************************//**
\brief Returns supported security key bit mask.

\return supported key bit mask.
******************************************************************************/
uint16_t N_Security_Calc_GetKeyBitMask(void);

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
                                           N_Security_Calc_DoneCallback_t callback);

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
                                           N_Security_Calc_DoneCallback_t callback);

/**************************************************************************//**
\brief Returns distributed trust center key.

\param[out] pKey - pointer to the memory where key shall be placed.
******************************************************************************/
void N_Security_Calc_GetDistributedTrustCenterLinkKey(uint8_t* pKey, N_Security_Calc_DoneCallback_t callback);

/**************************************************************************//**
\brief Returns random bytes usable for security purposes

\param[in] bytes - the number of bytes to return
\param[out] pKey - pointer to the memory where random bytes shall be placed.

\note This function's execution time is quite high (est. 4us per byte).
******************************************************************************/
void N_Security_Calc_Random(uint8_t bytes, uint8_t* random);

/**************************************************************************//**
\brief En/decrypt 16-byte data using a secret and unknown key. pIn==pOut is allowed

\param[in] pIn - 16-byte input data
\param[in] pOut - 16-byte output data
\param[in] done - callback on operation completion
******************************************************************************/
void N_Security_Calc_Crypt16(const uint8_t* pIn, uint8_t* pOut, N_Security_Calc_DoneCallback_t done);

/***************************************************************************************************
* END OF C++ DECLARATION WRAPPER
***************************************************************************************************/

#ifdef __cplusplus
}
#endif

#endif // N_SECURITY_CALC_H
