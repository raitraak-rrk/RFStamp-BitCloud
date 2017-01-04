/**************************************************************************//**
  \file sspSkke.h

  \brief Security-Key Key Esteblishment routine header file.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      29/11/07 ALuzhetsky - Created.
******************************************************************************/
#ifndef _SSPSKKE_H
#define _SSPSKKE_H

#ifdef _LINK_SECURITY_
/******************************************************************************
                        Includes section.
******************************************************************************/
#include <sysTypes.h>
#include <sspCommon.h>
#include <sspAuthentic.h>
#include <sspChallengeGen.h>

/******************************************************************************
                        Definitions section.
******************************************************************************/

/******************************************************************************
                        Types' definition section.
******************************************************************************/

/**************************************************************************//**
  \brief Declaration of CalculateSkkeParamsConf primitive parameters structure.
******************************************************************************/
typedef struct
{
  //! Calculated MacTag1.
  uint8_t macTag1[HASH_SIZE];
  //! Calculated MacTag2.
  uint8_t macTag2[HASH_SIZE];
  //! Calculated keyData.
  uint8_t keyData[HASH_SIZE];
} SSP_CalculateSkkeParamsConf_t;

/**************************************************************************//**
  \brief Declaration of CalculateSkkeParamsReq primitive parameters structure.
******************************************************************************/
typedef struct
{
  //! Service field - for internal needs.
  SSP_Service_t service;
  //! Extended address of the device which initiated SKKE procedure.
  const uint64_t *initiatorIEEEAddr;
  //! Extended address of the device which should response.
  const uint64_t *responderIEEEAddr;
  //! Preset key which should be known by both devices (initiator and responder).
  uint8_t *masterKey/*[SECURITY_KEY_SIZE]*/;
  //! Initiator challenge sequence (just 128-bit random number).
  uint8_t qeu[CHALLENGE_SEQUENCE_SIZE];
  //! Responder challenge sequence (just 128-bit random number).
  uint8_t qev[CHALLENGE_SEQUENCE_SIZE];
  //! Security Service Provider CalculateSkkeParams confirm callback function's pointer.
  void (*SSP_CalculateSkkeParamsConf)(SSP_CalculateSkkeParamsConf_t *conf);
  //! Security Service Provider CalculateSkkeParams confirm parameters' structure.
  SSP_CalculateSkkeParamsConf_t confirm;
} SSP_CalculateSkkeParamsReq_t;

/******************************************************************************
                        Prototypes section.
******************************************************************************/
/*******************************************************************************
  Performs calculation of the parameters for Symmetric key key establishment
  procedure.
  Parameters:
    param - SKKE procedure parameters (for detailed description look at
             SSP_CalculateSkkeParamsReq_t declaration).
  Return:
    none.
********************************************************************************/
void SSP_CalculateSkkeParamsReq(SSP_CalculateSkkeParamsReq_t *param);

#endif // _LINK_SECURITY_

#endif //_SSPSKKE_H

// eof sspSkke.h

