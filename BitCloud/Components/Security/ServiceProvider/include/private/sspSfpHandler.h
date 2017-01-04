/**************************************************************************//**
  \file sspSfpHandler.h

  \brief SFP routine header file.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      2006 - VGribanovsky created.
      29/11/2007 - ALuzhetsky API corrected.
******************************************************************************/

#ifndef _SSPSFPHANDLER_H
#define _SSPSFPHANDLER_H

/******************************************************************************
                        Includes section.
******************************************************************************/
#include <sysEndian.h>
#include <sspManager.h>
#include <sspSfp.h>
#include <sspAesHandler.h>
#include <sspCcmHandler.h>

/******************************************************************************
                        Types definition section.
******************************************************************************/
/**************************************************************************//**
  \brief TBD
******************************************************************************/
typedef enum
{
  SFP_STATE_IDLE,
  SFP_STATE_ENCRYPTION_KEY_TRANSFORMING,
  SFP_STATE_ENCRYPTION_AUTHENTIC,
  SFP_STATE_ENCRYPTION_ENCRYPT,
  SFP_STATE_DECRYPTION_KEY_TRANSFORMING,
  SFP_STATE_DECRYPTION_AUTHENTIC,
  SFP_STATE_DECRYPTION_ENCRYPT,
  SFP_STATE_DECRYPTION_MIC_CHECK,
  SFP_STATE_CCM_ENCRYPTION,
  SFP_STATE_CCM_ENCRYPTION_AUTHENTIC,
  SFP_STATE_CCM_DECRYPTION,
  SFP_STATE_CCM_DECRYPTION_MIC_CHECK,
  SFP_STATE_CCM_DECRYPTION_AUTHENTIC
} SspSfpHandlerState_t;

/**************************************************************************//**
  \brief TBD
******************************************************************************/
typedef struct
{
  SspSfpHandlerState_t sfpState;
  uint8_t              nonce[CCM_NONCE_SIZE];
  uint8_t              transformedKey[SECURITY_KEY_SIZE];
  uint8_t              mic[CCM_MIC_MAX_SIZE];
  uint8_t              auxiliaryHeaderLength;
  union
  {
    CcmReq_t             ccmReq;
#ifdef _LINK_SECURITY_
    SspKeyedHashMacReq_t hashMacReq;
#endif // _LINK_SECURITY_
  };
  BEGIN_PACK
  struct PACK
  {
    volatile uint8_t keyExpander[16];
    uint8_t text;
    volatile uint8_t bitsExpander[16];
  }; // For hash function
  END_PACK
} SspSfpHandlerMem_t;

/**************************************************************************//**
  \brief Security Control field format.
******************************************************************************/
BEGIN_PACK
typedef struct PACK
{
  LITTLE_ENDIAN_OCTET(4,(
    uint8_t securityLevel : 3,
    uint8_t keyIdentifier : 2,
    uint8_t extendedNonce : 1,
    uint8_t reserved      : 2
  ))
} SecurityCtrlField_t;
END_PACK

/******************************************************************************
                        Prototypes section.
******************************************************************************/
/******************************************************************************
  Encrypts frame. Encoding is based on incoming param which are extracted from
  SSP_EncryptFrameReq param.
  Parameters:
    none.
  Return:
    none.
******************************************************************************/
void sspEncryptFrameReqHandler(void);

/******************************************************************************
  Decrypts frame. Decrypting is based on incoming param which are extracted from
  SSP_DecryptFrameReq param.
  Parameters:
    none.
  Return:
    none.
******************************************************************************/
void sspDecryptFrameReqHandler(void);

/******************************************************************************
  Does CCM operation. CCM operation is based on incoming param which are extracted from
  SSP_CCMReq param.
  Parameters:
    none.
  Return:
    none.
******************************************************************************/
void sspCcmReqHandler(void);
/******************************************************************************
  Encrypts frame. Encrypting is based on incoming param which are extracted from
  SSP_ZgpEncryptFrameReq param.
  Parameters:
    none.
  Return:
    none.
******************************************************************************/
#ifdef ZGP_SECURITY_ENABLE
void sspZgpEncryptFrameReqHandler(void);

/******************************************************************************
  Decrypts frame. Decrypting is based on incoming param which are extracted from
  SSP_ZgpDecryptFrameReq param.
  Parameters:
    none.
  Return:
    none.
******************************************************************************/
void sspZgpDecryptFrameReqHandler(void);
#endif /* ZGP_SECURITY_ENABLE */
/******************************************************************************
  Parses auxiliary frame header.
  Parameters:
    param - contains pointer to the frame and parameters to be initialized.
  Return:
    Auxiliary header length.
******************************************************************************/
uint8_t sspParseAuxiliaryHeaderHandler(SSP_ParseAuxiliaryHeader_t *param);

/******************************************************************************
  Security Frame Processor Handler's task.
  Parameters:
    none
  Return:
    none.
******************************************************************************/
void sspSfpTaskHandler(void);

/**************************************************************************//**
  \brief Resets Security frame processor
 ******************************************************************************/
void sspResetSfp(void);

#endif //_SSPSFPHANDLER_H

// eof sspSfpHandler.h
