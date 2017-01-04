/**************************************************************************//**
  \file tcAuthentic.h

  \brief Security Trust Centre authentic primitive header file.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      2007.12 - ALuzhetsky created.
******************************************************************************/
#ifndef _TCAUTHENTICH
#define _TCAUTHENTICH

/******************************************************************************
                             Include section.
******************************************************************************/
#include <aps.h>

/******************************************************************************
                             Types section.
******************************************************************************/
typedef enum _TcAuthObjState_t
{
  TC_AUTHENTIC_OBJ_STATE_IDLE,
  TC_AUTHENTIC_OBJ_STATE_TRANSPORTING_NETWORK_KEY,
  TC_AUTHENTIC_OBJ_STATE_TRANSPORTING_MASTER_KEY,
  TC_AUTHENTIC_OBJ_STATE_ESTABLISHING_KEY,
  TC_AUTHENTIC_OBJ_STATE_REMOVING_DEVICE,
} TcAuthObjState_t;

/**************************************************************************//**
  \brief Authentic handler memory object.

  This struct declares memory wichshould be reserved for the Trust Centre
  Authentication proceure.
******************************************************************************/
typedef struct
{
  union
  { //! Memory for APS_TransportKeyReq.
    APS_TransportKeyReq_t transportKey;
    //! Memory for APS_RemoveDeviceReq.
    APS_RemoveDeviceReq_t removeDevice;
#if defined _LINK_SECURITY_ && defined _HIGH_SECURITY_
    //! Memory for APS_EstablishKeyReq.
    APS_EstablishKeyReq_t establishKey;
#endif
  } buffer;
  //! Memory to store source address of APS_UpdateDeviceInd command.
  ExtAddr_t updateIndSrcAddr;
  //! Current state for Trust Centre authentic handler.
  TcAuthObjState_t state;
} TC_AuthenticObj_t;

#endif // _TCAUTHENTICH

// eof tcAuthentic.h
