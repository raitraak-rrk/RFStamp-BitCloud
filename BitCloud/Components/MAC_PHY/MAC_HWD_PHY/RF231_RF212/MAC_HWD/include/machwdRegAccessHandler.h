/**************************************************************************//**
  \file machwdRegAccessHandler.h

  \brief MAC HWD interface for accessing to RF registers.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2012-03-01 Dmitry Kolmakov - Created.
   Last change:
    $Id: machwdCalibHandler.h 20291 2012-03-01 12:59:34Z dkolmakov $
 ******************************************************************************/
#ifndef _MACHWDREGACCESSHANDLER_H
#define _MACHWDREGACCESSHANDLER_H

/******************************************************************************
                                Includes section
 ******************************************************************************/
#include <machwdRegAccess.h>
#include <machwdHandlerCtrl.h>
#include <machwdManager.h>

/******************************************************************************
                               Prototypes section
 ******************************************************************************/
/******************************************************************************
  \brief Try to perform writing to or reading from RF module memory.

  \param[in] request - pointer to paramters of the original register
                       access request.
  \return result of register access procedure.
 *****************************************************************************/
MachwdHandlerResult_t machwdRegAccessReqHandler(MACHWD_RegAccessReq_t *request);

/******************************************************************************
  \brief Start r/w procedure if PHY is idle otherwise postpone the task.

  \return result of register access procedure.
 *****************************************************************************/
MachwdHandlerResult_t machwdRegAccessTaskHandler(void);

/******************************************************************************
  \brief Reset internal state and clear variables of the reg access module.
 *****************************************************************************/
void machwdResetRegAccessHandler(void);

#endif /* _MACHWDREGACCESSHANDLER_H */
/** eof machwdRegAccessHandler.h */
