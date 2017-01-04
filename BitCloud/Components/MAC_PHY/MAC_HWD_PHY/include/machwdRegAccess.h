/**************************************************************************//**
  \file machwdRegAccess.h

  \brief Radio registers access interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    2012-03-01 Dmitry Kolmakov - Created.
  Last change:
    $Id: machwdCalibration.h 20291 2012-03-01 12:59:34Z dkolmakov $
 ******************************************************************************/
#if !defined _MACHWDREGACCESS_H
#define _MACHWDREGACCESS_H

/******************************************************************************
                                Includes section
 ******************************************************************************/
#include <rfRegAccess.h>

/******************************************************************************
                               Definitions section
 ******************************************************************************/
/** Redefenition of RF register write request function name. **/
#define MACHWD_RegWriteReq RF_RegWriteReq

/** Redefenition of RF register read request function name. **/
#define MACHWD_RegReadReq RF_RegReadReq

/******************************************************************************
                                 Types section
 ******************************************************************************/
/** Redefenition of register access request structure. **/
typedef RF_RegAccessReq_t MACHWD_RegAccessReq_t;

/******************************************************************************
                               Prototypes section
 ******************************************************************************/
/**************************************************************************//**
  \brief Request to perform writing to the radio module memory.

  \param[in] reqParams - pointer to register access parameters - callback and
                         confirmation status.
  \return None.
 ******************************************************************************/
void MACHWD_RegWriteReq(MACHWD_RegAccessReq_t *reqParams);

/**************************************************************************//**
  \brief Request to perform reading from radio module memory.

  \param[in] reqParams - pointer to register access parameters - callback and
                         confirmation status.
  \return None.
 ******************************************************************************/
void MACHWD_RegReadReq(MACHWD_RegAccessReq_t *reqParams);

#endif /* _MACHWDREGACCESS_H */
/** eof machwdRegAccess.h */
