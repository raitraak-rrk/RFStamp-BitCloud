/******************************************************************************
  \file machwdBatMonHandler.h

  \brief Prototypes of battery monitor handler.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    16/04/08 A. Mandychev - Created.
  Last change:
    $Id: machwdBatMonHandler.h 27584 2015-01-09 14:45:42Z unithra.c $
******************************************************************************/

#ifndef _MACHWDBATMONHANDLER_H
#define _MACHWDBATMONHANDLER_H

/******************************************************************************
                    Includes section
******************************************************************************/
#include <machwdBatMon.h>
#include <machwdHandlerCtrl.h>
#include <machwdManager.h>

/******************************************************************************
                    Types section
******************************************************************************/
typedef enum
{
  MACHWD_BAT_MON_IDLE_SUBSTATE,
  MACHWD_BAT_MON_IND_SUBSTATE
} BatMonHandlerSubState_t;

/******************************************************************************
                    External variables
******************************************************************************/
extern BatMonHandlerSubState_t batMonSubState;

/******************************************************************************
                    Prototypes section
******************************************************************************/
/******************************************************************************
  Handles battery monitor request.
  Parameters:
    *request - pointer to request structure.
  Returns: result of battery monitor request.
******************************************************************************/
MachwdHandlerResult_t machwdBatMonReqHandler(MACHWD_BatMonReq_t *request);

/******************************************************************************
  Handles battery monitor task. It means that battery monitor request has been
  started and must be proceeded again.
  Parameters:
    none.
  Returns: result of battery monitor request.
******************************************************************************/
MachwdHandlerResult_t machwdBatMonTaskHandler(void);

/******************************************************************************
  Resets encrypt handler.
  Parameters:
    none.
  Return:
    none.
******************************************************************************/
void machwdResetBatMonHandler(void);

/******************************************************************************
                    Inline static functions section
******************************************************************************/
/******************************************************************************
  Inidicates that supply voltage drop below the configured threshold.
  Parameters: none.
  Returns:    none
******************************************************************************/
INLINE void MACHWD_BatMonInd(void)
{
  batMonSubState = MACHWD_BAT_MON_IND_SUBSTATE;
  machwdSetHandlerId(MACHWD_BAT_MON_HNDLR_ID);
  MACHWD_PostTask();
}

#endif /*_MACHWDBATMONHANDLER_H*/

// eof machwdBatMonHandler.h
