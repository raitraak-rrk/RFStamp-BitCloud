/**************************************************************************//**
  \file sspMem.h

  \brief Security Service Provider memory manager header file.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      06/12/2007 - ALuzhetsky created.
******************************************************************************/
#ifndef _SSPMEM_H
#define _SSPMEM_H

/******************************************************************************
                        Includes section.
******************************************************************************/
#include <sysQueue.h>
#include <sspManager.h>
#include <sspAuthenticHandler.h>
#include <sspSkkeHandler.h>
#include <sspSfpHandler.h>
#include <sspCcmHandler.h>

/******************************************************************************
                        Types section.
******************************************************************************/
typedef struct
{
  void            *sspReq; // Used by all modules which need to save requests' parameters.
  // For Manager needs.
  SspManagerMem_t managerMem;
  SspAesMem_t     aes;
  union
  {
#ifdef _LINK_SECURITY_
    SspAuthenticHandlerMem_t authenticHandlerMem;
    SspSkkeHandlerMem_t      skkeHandlerMem;
#endif // _LINK_SECURITY_
    SspSfpHandlerMem_t       sfpHandlerMem;
  };
  union
  {
#ifdef _LINK_SECURITY_
    SspHashHandlerMem_t hashHandlerMem;
#endif // _LINK_SECURITY_
    SspCcmHandlerMem_t  ccmHandlerMem;
  };
} SspMem_t;

/******************************************************************************
                        External variables.
******************************************************************************/
extern SspMem_t sspMem;

/******************************************************************************
                        Inline static functions prototypes section.
******************************************************************************/
/******************************************************************************
  Stores the pointer to the current request.
  Parameters:
    req - pointer to the current request.
  returns:
    none.
******************************************************************************/
INLINE void sspStoreReqParams(void *req)
{
  sspMem.sspReq = req;
}

/******************************************************************************
  Gets the pointer to the current request.
  Parameters:
    none.
  returns:
    Pointer to the current request.
******************************************************************************/
INLINE void* sspGetReqParams(void)
{
  return sspMem.sspReq;
}

/******************************************************************************
  Gets the current manager state.
  Parameters:
    none.
  returns:
    current manager state.
******************************************************************************/
/*INLINE SspState_t sspGetManagerState(void)
{
  return sspMem.managerMem.managerState;
}*/

/******************************************************************************
  Sets the current manager state.
  Parameters:
    new state.
  returns:
    none.
******************************************************************************/
/*INLINE void sspSetManagerState(SspState_t newState)
{
  sspMem.managerMem.managerState = newState;
}*/

/******************************************************************************
  Gets pointer to the queue descriptor.
  Parameters:
    none.
  returns:
    pointer to the queue descriptor.
******************************************************************************/
INLINE QueueDescriptor_t* sspGetQueueDescr(void)
{
  return &sspMem.managerMem.reqQueueDescr;
}

/******************************************************************************
  Gets pointer to the queue descriptor.
  Parameters:
    none.
  returns:
    pointer to the queue descriptor.
******************************************************************************/
INLINE SspTaskBitMask_t* sspGetTaskBitMask(void)
{
  return &sspMem.managerMem.taskBitMask;
}

/******************************************************************************
  Gets pointer to the current request.
  Parameters:
    none.
  returns:
    pointer to the current request.
******************************************************************************/
INLINE void* sspGetCurrentReq(void)
{
  return sspMem.sspReq;
}

/******************************************************************************
  Stores pointer to the current request.
  Parameters:
    pointer to the current request.
  returns:
    none.
******************************************************************************/
INLINE void sspSetCurrentReq(void* param)
{
  sspMem.sspReq = param;
}

#ifdef _LINK_SECURITY_
/******************************************************************************
  Gets pointer to the authentic handler memory.
  Parameters:
    none.
  returns:
    pointer to the authentic handler memory.
******************************************************************************/
INLINE SspAuthenticHandlerMem_t* sspGetAuthenticHandlerMem(void)
{
  return &sspMem.authenticHandlerMem;
}

/******************************************************************************
  Gets pointer to the skke handler memory.
  Parameters:
    none.
  returns:
    pointer to the skke handler memory.
******************************************************************************/
INLINE SspSkkeHandlerMem_t* sspGetSkkeHandlerMem(void)
{
  return &sspMem.skkeHandlerMem;
}

/******************************************************************************
  Gets pointer to the hash handler memory.
  Parameters:
    none.
  returns:
    pointer to the hash handler memory.
******************************************************************************/
INLINE SspHashHandlerMem_t* sspGetHashHandlerMem(void)
{
  return &sspMem.hashHandlerMem;
}
#endif // _LINK_SECURITY_

/******************************************************************************
  Gets pointer to the sfp handler memory.
  Parameters:
    none.
  returns:
    pointer to the sfp handler memory.
******************************************************************************/
INLINE SspSfpHandlerMem_t* sspGetSfpHandlerMem(void)
{
  return &sspMem.sfpHandlerMem;
}

/******************************************************************************
  Gets pointer to the memory for aes encryption procedure.
  Parameters:
    none.
  returns:
    pointer to the memory for aes encryption procedure.
******************************************************************************/
INLINE SspAesMem_t* sspGetAesMem(void)
{
  return &sspMem.aes;
}

/******************************************************************************
  Gets pointer to the ccm handler memory.
  Parameters:
    none.
  returns:
    pointer to the ccm handler memory.
******************************************************************************/
INLINE SspCcmHandlerMem_t* sspGetCcmHandlerMem(void)
{
  return &sspMem.ccmHandlerMem;
}


//#endif // _SSP_USE_STATIC_MEM_

#endif //_SSPMEM_H
// eof sspMem.h
