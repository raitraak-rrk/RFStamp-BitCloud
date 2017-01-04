/**************************************************************************//**
  \file tcKeyEstablish.h

  \brief Security Trust Centre End to End Key Establish primitive header file.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      2008.01.17 - ALuzhetsky created.
******************************************************************************/
#ifndef _TCKEYESTABLISH
#define _TCKEYESTABLISH

/******************************************************************************
                             Types section.
******************************************************************************/
/**************************************************************************//**
  \brief TBD.

  TBD
******************************************************************************/
typedef struct
{
  union
  {
    APS_TransportKeyReq_t transportKeyReq;
  };
  //ExtAddr_t updateIndSrcAddr;
  uint8_t state;
} TC_KeyEstablishObj_t;

/******************************************************************************
                             Functions prototypes section.
******************************************************************************/
/**************************************************************************//**
  \brief TBD.

  \param TBD.
  \return TBD.
******************************************************************************/
#ifdef _LINK_SECURITY_
void APS_RequestKeyInd(APS_RequestKeyInd_t *indParams);
#endif // _LINK_SECURITY_

#endif // _LINK_SECURITY_

// eof tcKeyEstablish.h
