/******************************************************************************
  \file phySetTrxState.h

  \brief Radio transceiver state control functions prototype.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      26/05/07 A. Mandychev - Created.
******************************************************************************/

#ifndef _PHYSETTRXSTATE_H
#define _PHYSETTRXSTATE_H

/******************************************************************************
                        Includes section.
******************************************************************************/
#include <at86rf2xx.h>
#include <halMacIsr.h>
#include <phyCommon.h>
#include <phyRfSpiProto.h>
#include <phyStateMachine.h>
#include <phyMemAccess.h>
#include <phyRxFrame.h>
#include <machwdHandlerCtrl.h>

/******************************************************************************
                        Prototypes section.
******************************************************************************/
/******************************************************************************
  Performs activities to switch to the necessery state.
  Parameters: cmd - command to switch.
  Returns: status of operation. PHY_BUSY_RX_REQ_STATUS,
           PHY_SUCCESS_REQ_STATUS, PHY_IN_PROGRESS_REQ_STATUS are all possible.
******************************************************************************/
PHY_ReqStatus_t PHY_SetTrxStateReq(PHY_TrxCmd_t cmd);

/******************************************************************************
                 Inline static functions prototypes section.
******************************************************************************/
/******************************************************************************
  Gets current TRX state.
  Parameters: none.
  Returns: current TRX state.
******************************************************************************/
INLINE PhyTrxState_t phyGetTrxState(void)
{
  return *phyMemPhyTrxState();
}

/******************************************************************************
  Set TRX OFF state.
******************************************************************************/
INLINE void phyResetTrxState(void)
{
  *phyMemPhyTrxState() = PHY_OFF_TRX_STATE;
}

/******************************************************************************
  RTimer event handler. It's used for state polling after receive procedure.
  To detect that acknowledgement was already sent.
  Parameters:
    none.
  Returns:
    none.
******************************************************************************/
INLINE void phyPollTrxStateHandler(void)
{
  uint8_t tmpRegValue;

  tmpRegValue = phyReadRegisterInline(TRX_STATUS_REG);
  if (TRX_STATUS_PLL_ON == (*((RegTrxStatus_t *)&tmpRegValue)).trxStatus)
  {
#if defined(_FFD_) && defined(_CONTROL_FRAME_PENDING_BIT_)
    // Set frame pending bit for AT86RF233
    tmpRegValue = phyReadRegister(CSMA_SEED_1_REG);
    phyWriteRegister(CSMA_SEED_1_REG, (REG_CSMA_SEED_1_AACK_SET_PD_MASK | tmpRegValue));
#endif
    HAL_StopRtimer();
    phyRxFinishedHandler();
  }
}

/******************************************************************************
  \brief RTimer event handler. It's used to check if radio is in ready to send state.
******************************************************************************/
INLINE void phyPollRTSTrxStateHandler(void)
{
  uint8_t tmpRegValue;

  if (PHY_IDLE_STATE != phyGetState())
    return;

  tmpRegValue = phyReadRegisterInline(TRX_STATUS_REG);
  if (TRX_STATUS_RX_AACK_ON == (*((RegTrxStatus_t *)&tmpRegValue)).trxStatus)
  {
    HAL_StopRtimer();
    MACHWD_PostTask();
  }
}

/******************************************************************************
  Converts command to switch to the state to switch.
  Parameters: cmd - command to convert.
  Returns: state originated from cmd.
******************************************************************************/
INLINE PhyTrxState_t phyConvertTrxCmdToTrxState(PHY_TrxCmd_t cmd)
{
  PhyTrxState_t state = (PhyTrxState_t) cmd;
  if (PHY_FORCE_TRX_OFF_CMD == cmd)
    state = PHY_OFF_TRX_STATE;
  return state;
}

#endif /* _PHYSETTRXSTATE_H */

// eof phySetTrxState.h
