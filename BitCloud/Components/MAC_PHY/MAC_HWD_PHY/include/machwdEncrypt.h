/**************************************************************************//**
  \file machwdEncrypt.h

  \brief Prototypes of ecnryption functions and corresponding types.
    Note that encryption is supported only for RF231 and RF212.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    18/01/08 A. Mandychev - Created.
   Last change:
    $Id: machwdCalibration.h 18970 2011-10-21 12:48:41Z mgekk $
******************************************************************************/

#ifndef _MACHWDENCRYPT_H
#define _MACHWDENCRYPT_H

/******************************************************************************
                    Includes section
******************************************************************************/
#include <rfEncrypt.h>

/******************************************************************************
                    Define(s) section
******************************************************************************/
//! Redefenition of encryption request function name.
#define MACHWD_EncryptReq RF_EncryptReq

/******************************************************************************
                    Types section
******************************************************************************/
//! Redefenition of encryption request structure.
typedef RF_EncryptReq_t MACHWD_EncryptReq_t;

/******************************************************************************
                    Prototypes section
******************************************************************************/
/**************************************************************************//**
  \brief Executes AES command.
  \param reqParams - request parameters structure pointer.
  \return none.
******************************************************************************/
void MACHWD_EncryptReq(MACHWD_EncryptReq_t *reqParams);

#endif /*_MACHWDENCRYPT_H*/

// eof machwdEncrypt.h
