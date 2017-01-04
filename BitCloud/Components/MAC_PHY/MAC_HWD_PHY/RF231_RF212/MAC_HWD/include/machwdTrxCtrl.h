/******************************************************************************
  \file machwdTrxCtrl.h

  \brief Prototypes of TRX control module.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    18/06/07 A. Mandychev - Created.
  Last change:
    $Id: machwdTrxCtrl.h 27584 2015-01-09 14:45:42Z unithra.c $
******************************************************************************/

#ifndef _MACHWDTRXCTRL_H
#define _MACHWDTRXCTRL_H

/******************************************************************************
                        Includes section.
******************************************************************************/
#include <sysTypes.h>
#include <machwd.h>
#include <machwdManager.h>
#include <machwdHandlerCtrl.h>
#include <phySetTrxState.h>

/******************************************************************************
                    Types section
******************************************************************************/
typedef enum
{
  MACHWD_TX_ON_TRX_STATE = PHY_TX_ARET_ON_TRX_STATE,
  MACHWD_RX_ON_TRX_STATE = PHY_RX_AACK_ON_TRX_STATE,
  MACHWD_OFF_TRX_STATE   = PHY_OFF_TRX_STATE
} MachwdTrxState_t;

/******************************************************************************
                        Prototypes section.
******************************************************************************/
/**************************************************************************//**
 \brief Converts MACHWD Trx command into MACHWD Trx state.

 \param cmd - MACHWD Trx command.

 \return MACHWD Trx state
******************************************************************************/
MachwdTrxState_t machwdConvertTrxCmdToTrxState(MACHWD_TrxCmd_t cmd);

/**************************************************************************//**
 \brief Converts MACHWD Trx state into MACHWD Trx command.

 \param state - MACHWD Trx state.

 \return MACHWD Trx command
******************************************************************************/
MACHWD_TrxCmd_t machwdConvertTrxStateToTrxCmd(MachwdTrxState_t state);

/**************************************************************************//**
 \brief Converts MACHWD Trx command into PHY Trx command.

 \param cmd - MACHWD Trx command.

 \return PHY Trx command
******************************************************************************/
PHY_TrxCmd_t machwdGetPhyTrxCmd(MACHWD_TrxCmd_t cmd);

/******************************************************************************
                 Inline static functions prototypes section.
******************************************************************************/
/**************************************************************************//**
 \brief Returns current PHY Trx state.

 \return PHY Trx state
******************************************************************************/
INLINE MachwdTrxState_t machwdGetTrxState(void)
{
  return (MachwdTrxState_t) phyGetTrxState();
}

#endif /* _MACHWDTRXCTRL_H */

// eof machwdTrxCtrl.h
