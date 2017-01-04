/**************************************************************************//**
  \file tcMem.h

  \brief Security Trust Centre memory manager header file.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      2007.12.25 - ALuzhetsky created.
******************************************************************************/
#ifndef _TCMEM_H
#define _TCMEM_H

/******************************************************************************
                              Includes section.
******************************************************************************/
#include <sysQueue.h>
#include <tcTaskManager.h>
#include <tcAuthenticHandler.h>
#include <tcKeyUpdateHandler.h>
#include <tcKeyEstablishHandler.h>
#include <tcRemoveHandler.h>

/******************************************************************************
                        Types section.
******************************************************************************/
/**************************************************************************//**
  \brief TBD.

  TBD
******************************************************************************/
typedef struct
{
  TcTaskManagerMem_t         taskManagerMem;
  TcAuthenticHandlerMem_t    authenticHandlerMem;
  TcKeyUpdateHandlerMem_t    keyUpdateHandlerMem;
#ifdef _LINK_SECURITY_
  TcKeyEstablishHandlerMem_t keyEstablishHandlerMem;
#endif // _LINK_SECURITY_
  TcRemoveHandlerMem_t       removeHandlerMem;
} TcMem_t;

/******************************************************************************
                        External variables.
******************************************************************************/
extern TcMem_t tcMem;

/******************************************************************************
                        Inline functions prototypes section.
******************************************************************************/
/**************************************************************************//**
  \brief TBD.

  \param TBD.
  \return TBD.
******************************************************************************/
INLINE TcTaskManagerMem_t* tcGetTaskManagerMem(void)
{
  return &tcMem.taskManagerMem;
}

/**************************************************************************//**
  \brief TBD.

  \param TBD.
  \return TBD.
******************************************************************************/
INLINE TcAuthenticHandlerMem_t* tcGetAuthenticHandlerMem(void)
{
  return &tcMem.authenticHandlerMem;
}

/**************************************************************************//**
  \brief TBD.

  \param TBD.
  \return TBD.
******************************************************************************/
INLINE TcKeyUpdateHandlerMem_t* tcGetKeyUpdateHandlerMem(void)
{
  return &tcMem.keyUpdateHandlerMem;
}

/**************************************************************************//**
  \brief TBD.

  \param TBD.
  \return TBD.
******************************************************************************/
#ifdef _LINK_SECURITY_
INLINE TcKeyEstablishHandlerMem_t* tcGetKeyEstablishHandlerMem(void)
{
  return &tcMem.keyEstablishHandlerMem;
}
#endif // _LINK_SECURITY_

/**************************************************************************//**
  \brief TBD.

  \param TBD.
  \return TBD.
******************************************************************************/
INLINE TcRemoveHandlerMem_t* tcGetRemoveHandlerMem(void)
{
  return &tcMem.removeHandlerMem;
}

#endif //_TCMEM_H

// eof tcMem.h
