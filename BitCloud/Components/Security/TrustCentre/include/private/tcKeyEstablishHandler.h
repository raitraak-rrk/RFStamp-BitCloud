/**************************************************************************//**
  \file tcKeyEstablishHandler.h

  \brief Trust Centre end to end key establishment routine header file.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      2008.01.17 - ALuzhetsky created.
******************************************************************************/
#ifndef _TCKEYESTABLISHHANDLERH
#define _TCKEYESTABLISHHANDLERH

#ifdef _LINK_SECURITY_
/******************************************************************************
                             Includes section.
******************************************************************************/
#include <aps.h>
#include <sysTypes.h>
#include <tcKeyEstablish.h>

/******************************************************************************
                             Definitions section.
******************************************************************************/


/******************************************************************************
                             Types section.
******************************************************************************/
/**************************************************************************//**
  \brief TBD.

  TBD
******************************************************************************/
typedef enum
{
  TC_KEY_ESTABLISH_OBJ_STATE_IDLE,
  TC_KEY_ESTABLISH_OBJ_STATE_TRANSPORT_KEY_POSTED,
  TC_KEY_ESTABLISH_OBJ_STATE_FIRST_TRANSPORT_KEY_EXECUTING,
  TC_KEY_ESTABLISH_OBJ_STATE_SECOND_TRANSPORT_KEY_EXECUTING,
} TcKeyEstablishHandlerObjState_t;

/**************************************************************************//**
  \brief TBD.

  TBD
******************************************************************************/
typedef struct
{
  TC_KeyEstablishObj_t *keyEstablishObj;
  uint8_t              maxRequests;
} TcKeyEstablishHandlerMem_t;

/******************************************************************************
                             Functions prototypes section.
******************************************************************************/
/**************************************************************************//**
  \brief TBD.

  \param TBD.
  \return TBD.
******************************************************************************/
void tcKeyEstablishTaskHandler(void);

/**************************************************************************//**
  \brief TBD.

  \param TBD.
  \return TBD.
******************************************************************************/
void tcResetKeyEstablishHandler(void);

#endif // _LINK_SECURITY_

#endif //_TCKEYESTABLISHHANDLERH
// eof tcKeyEstablishHandler.h
