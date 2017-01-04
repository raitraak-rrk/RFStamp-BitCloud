/**************************************************************************//**
  \file sspAuthentic.h

  \brief Mutual Symmetric-Key Entity Authentication routine header file.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      29/11/07 ALuzhetsky - Created.
******************************************************************************/
#ifndef _SSPAUTHENTIC_H
#define _SSPAUTHENTIC_H

#ifdef _LINK_SECURITY_
/******************************************************************************
                        Includes section.
******************************************************************************/
#include <sysTypes.h>
#include <sspCommon.h>
#include <sspChallengeGen.h>

/******************************************************************************
                        Definitions section.
******************************************************************************/
#define HASH_SIZE 16

/******************************************************************************
                        Types' definition section.
******************************************************************************/

/**************************************************************************//**
  \brief Declaration of CalculateAuthenticParamsConf primitive parameters structure.
******************************************************************************/
typedef struct
{
  //! Calculated MacTag1 or MacTag2.
  uint8_t macTag[HASH_SIZE];
} SSP_CalculateAuthenticParamsConf_t;

/**************************************************************************//**
  \brief Declaration of CalculateAuthenticParamsConf primitive parameters structure.
******************************************************************************/
typedef struct
{
  //! Service field - for internal needs.
  SSP_Service_t service;
  //! Extended address of the device which initiated authnetication procedure.
  const uint64_t *initiatorIEEEAddr;
  //! Extended address of the device which should response.
  const uint64_t *responderIEEEAddr;
  //! Initiator or responder device frame counter.
  uint32_t frameCounter;
  //! Preset key which should be known by both devices (initiator and responder).
  const uint8_t *networkKey/*[SECURITY_KEY_SIZE]*/;
  //! Initiator challenge sequence (just 128-bit random number).
  uint8_t *qeu;
  //! Responder challenge sequence (just 128-bit random number).
  uint8_t *qev;
  //! Security Service Provider CalculateAuthenticParams confirm callback function's pointer.
  void (*SSP_CalculateAuthenticParamsConf)(SSP_CalculateAuthenticParamsConf_t *conf);
  //! Security Service Provider CalculateAuthenticParams confirm parameters' structure.
  SSP_CalculateAuthenticParamsConf_t confirm;
} SSP_CalculateAuthenticParamsReq_t;

/******************************************************************************
                        Prototypes section.
******************************************************************************/
/*******************************************************************************
  Performs calculation of the parameters for Mutual Symmetric-Key Entity
  Authentication procedure (MatTag1).
  Parameters:
    param - procedure parameters (for detailed description look at
             SSP_CalculateAuthenticParamsReq_t declaration).
  Return:
    none.
********************************************************************************/
void SSP_CalculateAuthenticMacTag1Req(SSP_CalculateAuthenticParamsReq_t *param);

/*******************************************************************************
  Performs calculation of the parameters for Mutual Symmetric-Key Entity
  Authentication procedure (MatTag2).
  Parameters:
    param - procedure parameters (for detailed description look at
             SSP_CalculateAuthenticParamsReq_t declaration).
  Return:
    none.
********************************************************************************/
void SSP_CalculateAuthenticMacTag2Req(SSP_CalculateAuthenticParamsReq_t *param);

#endif // _LINK_SECURITY_

#endif //_SSPAUTHENTIC_H

// eof sspAuthentic.h

