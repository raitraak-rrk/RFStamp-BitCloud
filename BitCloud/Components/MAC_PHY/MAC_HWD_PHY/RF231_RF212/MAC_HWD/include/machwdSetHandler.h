/******************************************************************************
  \file machwdSetHandler.h

  \brief Prototypes of Set handler.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    07/06/07 A. Mandychev - Created.
  Last change:
    $Id: machwdSetHandler.h 27584 2015-01-09 14:45:42Z unithra.c $
******************************************************************************/

#ifndef _MACHWDSETHANDLER_H
#define _MACHWDSETHANDLER_H

/******************************************************************************
                    Includes section
******************************************************************************/
#include <sysTypes.h>
#include <machwdSet.h>
#include <machwdHandlerCtrl.h>

/******************************************************************************
                    Prototypes section
******************************************************************************/
/******************************************************************************
  TBD.
******************************************************************************/
bool machwdIsBeaconRxMode(void);

/******************************************************************************
  TBD.
******************************************************************************/
MachwdHandlerResult_t machwdSetReqHandler(MACHWD_SetReq_t *request);

/******************************************************************************
  TBD.
******************************************************************************/
MachwdHandlerResult_t machwdSetTaskHandler(void);

/******************************************************************************
  TBD.
******************************************************************************/
void machwdResetSetHandler(void);

#endif /* _MACHWDSETHANDLER_H */

// eof machwdSetHandler.h
