/**************************************************************************//**
  \file rfEncrypt.h

  \brief Prototypes of ecnryption functions and corresponding types.
    Powered by RF chip.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      18/01/08 A. Mandychev - Created.
      24/03/08 A. Mandychev - Renamed to rfEncrypt.h
******************************************************************************/

#ifndef _RFECNRYPT_H
#define _RFECNRYPT_H

/******************************************************************************
                    Includes section
******************************************************************************/
#include <sysTypes.h>
#include <macCommon.h>

/******************************************************************************
                    Define(s) section
******************************************************************************/

/******************************************************************************
                    Types section
******************************************************************************/
//! Set of AES command.
typedef enum
{
  RF_SET_KEY_AES_CMD = 0,
  RF_SET_INIT_VECT_AES_CMD,
  RF_ECB_ENCRYPT_AES_CMD,
  RF_CBC_ENCRYPT_AES_CMD,
  RF_ECB_DECRYPT_AES_CMD
} RF_AesCmd_t;

//! Encryption request structure.
typedef struct
{
  //! Service field - for internal needs.
  MAC_Service_t  service;
  //! AES command.
  RF_AesCmd_t aesCmd;
  //! Pointer to input data for AES command.
  uint8_t *text;
  //! Size of input data in blocks.
  uint8_t blockCount; // Block is 128-bit data.
  void (*RF_EncryptConf)(void);
} RF_EncryptReq_t;

/******************************************************************************
                    Prototypes section
******************************************************************************/
/**************************************************************************//**
  \brief Executes AES command. Valid for RF231 and RF212.
  \param reqParams - request parameters structure pointer.
  \return none.
******************************************************************************/
void RF_EncryptReq(RF_EncryptReq_t *reqParams);

#endif /*_RFECNRYPT_H*/

// eof rfEncrypt.h

