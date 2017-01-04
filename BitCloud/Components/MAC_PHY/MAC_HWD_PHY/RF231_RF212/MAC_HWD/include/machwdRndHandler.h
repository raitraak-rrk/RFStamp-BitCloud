/******************************************************************************
  \file machwdRndHandler.h

  \brief Prototypes of Random handler.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    08/04/08 A. Mandychev - Created.
  Last change:
    $Id: machwdRndHandler.h 27584 2015-01-09 14:45:42Z unithra.c $
******************************************************************************/

#ifndef _MACHWDRNDHANDLER_H
#define _MACHWDRNDHANDLER_H

/******************************************************************************
                    Includes section
******************************************************************************/
#include <sysTypes.h>
#include <machwdRnd.h>
#include <machwdHandlerCtrl.h>

/******************************************************************************
                    Prototypes section
******************************************************************************/
/******************************************************************************
  Handles random request.
  Parameters:
    *request - pointer to request structure.
  Returns: result of encryption request.
******************************************************************************/
MachwdHandlerResult_t machwdRndReqHandler(MACHWD_RndReq_t *request);

/******************************************************************************
  Handles random task. It means that random request has been started
  and must be proceeded again.
  Parameters:
    none.
  Returns: result of encryption request.
******************************************************************************/
MachwdHandlerResult_t machwdRndTaskHandler(void);

/******************************************************************************
  Resets random handler.
  Parameters:
    none.
  Return:
    none.
******************************************************************************/
void machwdResetRndHandler(void);

#endif /*_MACHWDRNDHANDLER_H*/

// eof machwdRndHandler.h
