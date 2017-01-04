/******************************************************************************
  \file machwdEncryptHandler.h

  \brief Prototype of ecnryption handler.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      18/01/08 A. Mandychev - Created.
******************************************************************************/

#ifndef _MACHWDENCRYPTHANDLER_H
#define _MACHWDENCRYPTHANDLER_H

/******************************************************************************
                    Includes section
******************************************************************************/
#include <sysTypes.h>
#include <phyEncrypt.h>
#include <machwdEncrypt.h>
#include <machwdHandlerCtrl.h>

/******************************************************************************
                    Prototypes section
******************************************************************************/
/******************************************************************************
  Handles encryption request.
  Parameters:
    *request - pointer to request structure.
  Returns: result of encryption request.
******************************************************************************/
MachwdHandlerResult_t machwdEncryptReqHandler(MACHWD_EncryptReq_t *request);

/******************************************************************************
  Handles encryption task. It means that encryption request has been started
  and must be proceeded again.
  Parameters:
    none.
  Returns: result of encryption request.
******************************************************************************/
MachwdHandlerResult_t machwdEncryptTaskHandler(void);

/******************************************************************************
  Resets encrypt handler.
  Parameters:
    none.
  Return:
    none.
******************************************************************************/
void machwdResetEncryptHandler(void);

#endif /*_MACHWDENCRYPTHANDLER_H*/

// eof machwdEncryptHandler.h
