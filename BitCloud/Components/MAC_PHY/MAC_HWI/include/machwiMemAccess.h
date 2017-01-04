/******************************************************************************
  \file machwiMemAccess.h

  \brief Provides access to MACHWI memory .

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      11/07/08 A. Mandychev - Created.
******************************************************************************/

#ifndef _MACHWIMEMACCESS_H
#define _MACHWIMEMACCESS_H

/******************************************************************************
                    Includes section
******************************************************************************/
#include <sysTypes.h>
#include <macMem.h>

/******************************************************************************
                         External variables section
******************************************************************************/
/** Declared in .../MAC_ENV/src/macMem.c */
extern MAC_Mem_t macMem;

/******************************************************************************
                        Inline static functions prototypes section.
******************************************************************************/
/******************************************************************************
  Gets address of MACHWI memory.
  Parameters: none.
  returns: Address of MACHWI memory.
******************************************************************************/
INLINE MachwiMem_t* machwiGetMem(void)
{
  return &macMem.hwi;
}

/******************************************************************************
  Stores the pointer to the current request.
  Parameters:
    req - pointer to the current request.
  returns:
    none.
******************************************************************************/
INLINE void machwiStoreReqParams(void *req)
{
  machwiGetMem()->machwiReq = req;
}

/******************************************************************************
  Gets the pointer to the current request.
  Parameters:
    none.
  returns:
    Pointer to the current request.
******************************************************************************/
INLINE void* machwiGetReqParams(void)
{
  return machwiGetMem()->machwiReq;
}

/**************************************************************************//**
 \brief  Allocates memory to execute request to MAC HWD sublayer.

 \return none.
******************************************************************************/
INLINE MachwiHwdReqDescr_t* machwiGetHwdReqMem(void)
{
  return &machwiGetMem()->machwdReq;
}

#endif /* _MACHWIMEM_H */

// eof machwiMemAccess.h
