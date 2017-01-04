/**************************************************************************//**
  \file sspCcmHandler.h

  \brief CCM routine header file.

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
#ifndef _SSPCCMHANDLER_H
#define _SSPCCMHANDLER_H


/******************************************************************************
                        Includes section.
******************************************************************************/
#include <sysTypes.h>


/******************************************************************************
                        Definitions section.
******************************************************************************/
#define CCM_MIC_MAX_SIZE 16
#define CCM_NONCE_SIZE   13

/******************************************************************************
                        Types' definitions section.
******************************************************************************/
/**************************************************************************//**
  TBD
******************************************************************************/
typedef enum
{
  CCM_REQ_TYPE_CRYPT,
  CCM_REQ_TYPE_AUTH,
  CCM_REQ_TYPE_CHECK_MIC
} CcmReqType_t;

/**************************************************************************//**
  TBD
******************************************************************************/
typedef enum
{
  CCM_STATE_IDLE,
  CCM_STATE_SET_KEY_WHILE_AUTH_OR_MIC_CHECK,
  CCM_STATE_SET_KEY_WHILE_CRYPT,
  CCM_STATE_CALC_X1_FOR_HDR_WHILE_AUTH_OR_MIC_CHECK,
  CCM_STATE_CALC_XN_FOR_HDR_WHILE_AUTH_OR_MIC_CHECK,
  CCM_STATE_CALC_XN_FOR_PLD_WHILE_CRYPT_OR_MIC_CHECK,
  CCM_STATE_CALC_LAST_X_FOR_HDR_WHILE_MIC_CHECK,
  CCM_STATE_CALC_LAST_X_FOR_HDR_WHILE_AUTH,
  CCM_STATE_CALC_LAST_X_FOR_PLD_WHILE_CRYPT_OR_MIC_CHECK,
  CCM_STATE_CRYPT_NEXT_PLD_BLOCK_WHILE_CRYPT_OR_MIC_CHECK,
  CCM_STATE_CALC_XN_FOR_PLD_WHILE_AUTH,
  CCM_STATE_CALC_LAST_X_FOR_PLD_WHILE_AUTH,
  CCM_STATE_CRYPT_MIC,
} SspCcmState_t;

/**************************************************************************//**
  \brief Input param structure for CCM authentification & encrypt
  OR decode & authentification operations.
******************************************************************************/
typedef struct
{
  //!< key
  uint8_t *key/*[SECURITY_KEY_SIZE]*/;
  //!< nonce
  uint8_t *nonce/*[CCM_NONCE_SIZE]*/;
  //!< header
  uint8_t *a/*[len_a]*/;
  //!< pdu
  uint8_t *m/*[len_m]*/;
  //!< MIC length (or MAC - message authentication code).
  //!< Limitations: does not work correctly if MIC length >= 0xfeff.
  uint8_t M;
  //!< header length
  uint8_t len_a;
  //!< pdu length
  uint8_t len_m;
  //!< Pointer to MIC memory - real MIC value will be XORed to this memory.
  //!< Set to zero MIC byte values if you want the new MIC to be generated.
  //!< To verify MIC check that it's value is all zeroes after authentication.
  uint8_t *mic/*[M]*/;
  //!< Confirm callback function
  void (*ccmConfirm)(void);
} CcmReq_t;

/******************************************************************************
  \brief Cryptographic context. Used for only for MIC generation and checking.
******************************************************************************/
typedef struct
{
  uint8_t textSize;
  uint8_t *text/*[textSize]*/;
  uint8_t x_i[SECURITY_BLOCK_SIZE];
} CcmAuthCtx_t;

/**************************************************************************//**
 \brief CCM handler module static memory structure.
******************************************************************************/
typedef struct
{
  //!< Current CCM request pointer
  CcmReq_t *ccmReq;
  //!< Current CCM request type
  CcmReqType_t reqType;
  //!< Current CCM module state
  SspCcmState_t ccmState;
  //!< Buffer is used to save intermidiate decryption results while MIC
  //!< calculating procedure and to form B1 while authentication.
  uint8_t tmpCcmBuf[SECURITY_BLOCK_SIZE];
  //!< Payload block counter
  uint8_t pldBlockCounter;
  CcmAuthCtx_t  ccmAuthCtx;
} SspCcmHandlerMem_t;


/******************************************************************************
                        Prototypes section.
******************************************************************************/
/******************************************************************************
  Calculate the CCM* authentication tag (CBC-MAC in RFC 3610 terms).
  Arguments:
    key - AES-128 key;
    nonce_block - 16-byte block containing nonce in bytes 1..13
    (bytes 0,14,15 are arbitrary);
    frame - ptr to data to be protected by the authentication tag;
    len_auth - data that have to be authentified only;
    len_crypt - data that have to be authentified and encrypted;
    auth - buffer to place the authentication tag;
    M - MIC length,
    temp - 176-byte array used by AES encryption algorithm.
    If M=0 (which is permitted by ZigBee Standard but not by RFC 3610)
    the function returns immediately.
  Limitations: does not work correctly if len_auth >= 0xfeff.
******************************************************************************/
void ccmAuthReq(CcmReq_t *ccmParam);


/******************************************************************************
  Encrypt the buffer in place by the CCM*. Arguments:
    key - AES-128 key;
    nonce_block - 16-byte block containing nonce in bytes 1..13
    (bytes 0,14,15 are arbitrary);
    payload - buffer to be encrypted;
    len_payload - length of this buffer;
    auth - MIC which have to be encrypted additionally (also in place);
    M - its length;
    temp - 176-byte array used by AES encryption algorithm.
******************************************************************************/
void ccmCryptReq(CcmReq_t *);

/**************************************************************************//**
 \brief Performs encrypted frame MIC check procedure.

 \param checMecReq - request parametrs structure pointer. Take a look at the
  CcmReq_t type declaration for the detailes.
******************************************************************************/
void ccmCheckMicReq(CcmReq_t *checkMicReq);

/**************************************************************************//**
 \brief Resets CCM
******************************************************************************/
void sspResetCcm(void);

#endif //_SSPCCMHANDLER_H

//eof sspCcmHandler.h
