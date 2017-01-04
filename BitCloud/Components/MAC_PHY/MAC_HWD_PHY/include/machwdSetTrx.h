/**************************************************************************//**
  \file machwdSetTrx.h

  \brief MACHWD transcivier control types and function prototypes.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      26/09/07 A. Mandychev - Created.
******************************************************************************/

#ifndef _MACHWDSETTRX_H
#define _MACHWDSETTRX_H

/******************************************************************************
                    Includes section
******************************************************************************/
#include <sysTypes.h>
#include <machwdService.h>

/******************************************************************************
                    Define(s) section
******************************************************************************/

/******************************************************************************
                    Types section
******************************************************************************/
//! Set of commands to control radio transcivier.
typedef enum
{
  MACHWD_TX_ON_CMD,
  MACHWD_RX_ON_CMD,
  MACHWD_TRX_OFF_CMD,
  MACHWD_PLL_ON_CMD
} MACHWD_TrxCmd_t;

//! Set TRX state request structure.
typedef struct
{
  //! Service field - for internal needs.
  MACHWD_Service_t         service;
  //! Command to chnage the trancivier current state.
  MACHWD_TrxCmd_t          trxCmd;
  // MACHWD set trx state confirm callback function's pointer.
  void (*MACHWD_SetTrxStateConf)(void);
} MACHWD_SetTrxStateReq_t;

/******************************************************************************
                    Prototypes section
******************************************************************************/
/**************************************************************************//**
  \brief Set trx state.
  \param reqParams - MACHWD set trx state request structure pointer.
  \return none.
******************************************************************************/
void MACHWD_SetTrxStateReq(MACHWD_SetTrxStateReq_t *reqParams);

#endif /* _MACHWDSETTRX_H */

// eof machwdSetTrx.h
