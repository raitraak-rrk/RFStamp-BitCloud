/******************************************************************************
  \file machwdEdHandler.h

  \brief Prototypes of Energy Detect handler

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).


  \internal
  History:
    28/05/07 A. Mandychev - Created.
  Last change:
    $Id: machwdEdHandler.h 27584 2015-01-09 14:45:42Z unithra.c $
******************************************************************************/

#ifndef _MACHWDEDHANDLER_H
#define _MACHWDEDHANDLER_H

/******************************************************************************
                    Includes section
******************************************************************************/
#include <inttypes.h>
#include <machwdMem.h>
#include <machwdEd.h>
#include <machwdManager.h>
#include <machwdHandlerCtrl.h>

/******************************************************************************
                    External variables
******************************************************************************/
//  Do not use it in other files.
extern PHY_EnergyLevel_t __energyLevel;

/******************************************************************************
                    Prototypes section
******************************************************************************/
/******************************************************************************
  Handles ED request.
  Parameters:
    *request - pointer to request structure with confirm structure
  Returns: result of ED request.
******************************************************************************/
MachwdHandlerResult_t machwdEdReqHandler(MACHWD_EdReq_t *request);

/******************************************************************************
  TBD.
******************************************************************************/
MachwdHandlerResult_t machwdEdTaskHandler(void);

/******************************************************************************
  TBD.
******************************************************************************/
void machwdResetEdHandler(void);

/******************************************************************************
                    Inline static functions section
******************************************************************************/
/******************************************************************************
  Saves ED level and posts new task. This routine is invoked from IRQ handler.
  Parameters:
    energyLevel - ED level for current channel.
******************************************************************************/
// concurrency (asynchronous) process
INLINE void PHY_EdConf(uint8_t energyLevel)
{
  __energyLevel = energyLevel;
  machwdSetHandlerId(MACHWD_ED_HNDLR_ID);
  MACHWD_PostTask();
}

#endif /* _MACHWDEDHANDLER_H */

// eof machwdEdHandler.h
