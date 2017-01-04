/**************************************************************************//**
  \file machwdRnd.h

  \brief MACHWD random generation types and function prototypes.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      08/04/08 A. Mandychev - Created.
******************************************************************************/

#ifndef _MACHWDRND_H
#define _MACHWDRND_H

/******************************************************************************
                    Includes section
******************************************************************************/
#include <rfRandom.h>

/******************************************************************************
                    Define(s) section
******************************************************************************/
//! Redefenition of random generation request function name.
#define MACHWD_RndReq RF_RandomReq
//! Redefenition of random seq generation request function name .
#define MACHWD_RndSeqReq RF_RandomSeqReq
/******************************************************************************
                    Types section
******************************************************************************/
//! Redefenition of random generation request structure.
typedef RF_RandomReq_t MACHWD_RndReq_t;

/******************************************************************************
                    Prototypes section
******************************************************************************/
/**************************************************************************//**
  \brief Requests random value.
  \param reqParams - request parameters structure pointer.
  \return none.
******************************************************************************/
void MACHWD_RndReq(MACHWD_RndReq_t *reqParams);

/**************************************************************************//**
  \brief Requests random seq.
  \param reqParams - request parameters structure pointer.
  \return none.
******************************************************************************/
void MACHWD_RndSeqReq(MACHWD_RndReq_t *reqParams);

#endif /*_MACHWDRND_H*/

// eof machwdRnd.h
