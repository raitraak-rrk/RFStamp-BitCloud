/**************************************************************************//**
  \file tcAuthenticHandler.h

  \brief Trust Centre authentic routine header file.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      2007.12 - ALuzhetsky created.
******************************************************************************/
#ifndef _TCAUTHENTICHANDLERH
#define _TCAUTHENTICHANDLERH

/******************************************************************************
                             Includes section.
******************************************************************************/
#include <aps.h>
#include <sysTypes.h>
#include <tcAuthentic.h>

/******************************************************************************
                             Types section.
******************************************************************************/
/**************************************************************************//**
  \brief TBD.

  TBD
******************************************************************************/
typedef struct
{
  TC_AuthenticObj_t *authenticObj;
  uint8_t           maxRequests;
} TcAuthenticHandlerMem_t;

/******************************************************************************
                             Functions prototypes section.
******************************************************************************/

/******************************************************************************
                             Functions prototypes section.
******************************************************************************/
/**************************************************************************//**
  \brief TBD.

  \param TBD.
  \return TBD.
******************************************************************************/
void tcUpdateDeviceIndToAuthenticHandler(APS_UpdateDeviceInd_t *indParam);


/**************************************************************************//**
  \brief TBD.

  \param TBD.
  \return TBD.
******************************************************************************/
void tcResetAuthenticHandler(void);

#endif //_TCAUTHENTICHANDLERH

// eof tcAuthenticHandler.h
