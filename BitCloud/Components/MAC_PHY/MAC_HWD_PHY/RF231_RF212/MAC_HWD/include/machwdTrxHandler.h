/******************************************************************************
  \file machwdTrxHandler.h

  \brief Prototypes of TRX handler.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    30/05/07 A. Mandychev - Created.
  Last change:
    $Id: machwdTrxHandler.h 27584 2015-01-09 14:45:42Z unithra.c $
******************************************************************************/

#ifndef _MACHWDTRXHANDLER_H
#define _MACHWDTRXHANDLER_H

/******************************************************************************
                    Includes section
******************************************************************************/
#include <inttypes.h>
#include <machwdSetTrx.h>
#include <machwdHandlerCtrl.h>

/******************************************************************************
                    Prototypes section
******************************************************************************/
/******************************************************************************
  TBD.
******************************************************************************/
MachwdHandlerResult_t machwdTrxReqHandler(MACHWD_SetTrxStateReq_t *request);

/******************************************************************************
  TBD.
******************************************************************************/
MachwdHandlerResult_t machwdTrxTaskHandler(void);

/******************************************************************************
  TBD.
******************************************************************************/
void machwdResetTrxHandler(void);

#endif /* MACHWDTRXHANDLER_H */

// eof machwdTrxHandler.h
