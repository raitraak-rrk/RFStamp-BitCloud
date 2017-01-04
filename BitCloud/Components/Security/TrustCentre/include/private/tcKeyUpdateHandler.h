/**************************************************************************//**
  \file tcKeyUpdateHandler.h

  \brief Trust Centre key update routine header file.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      2008.01.11 - ALuzhetsky created.
******************************************************************************/
#ifndef _TCKEYUPDATEHANDLERH
#define _TCKEYUPDATEHANDLERH

/******************************************************************************
                        Includes section.
******************************************************************************/
#include <sysQueue.h>
#include <appTimer.h>
#include <aps.h>

typedef struct
{
  QueueDescriptor_t reqQueueDescr;
  APS_KeyHandle_t  keyHandle;
  void              *currentReq;
  union
  {
    APS_TransportKeyReq_t transportKeyReq;
    APS_SwitchKeyReq_t    switchKeyReq;
    HAL_AppTimer_t        keyUpdateTimer;
  };
} TcKeyUpdateHandlerMem_t;

/******************************************************************************
                        Functions prototypes section.
******************************************************************************/
/**************************************************************************//**
  \brief TBD.

  \param TBD.
  \return TBD.
******************************************************************************/
void tcKeyUpdateTaskHandler(void);

/**************************************************************************//**
  \brief TBD.

  \param TBD.
  \return TBD.
******************************************************************************/
void tcResetKeyUpdateHandler(void);

#endif //_TCKEYUPDATEHANDLERH
// eof tcKeyUpdateHandler.h
