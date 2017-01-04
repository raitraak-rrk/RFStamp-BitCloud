/******************************************************************************
  \file phyTxFrame.h

  \brief Frame transmission routines.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      04/06/07 ALuzhetsky - Created.
******************************************************************************/

#ifndef _PHYTXFRAME_H
#define _PHYTXFRAME_H

/******************************************************************************
                        Includes section.
******************************************************************************/
#include <sysTypes.h>
#include <at86rf2xx.h>
#include <phyCommon.h>
#include <phyRfSpiProto.h>
#include <phyDataStatus.h>
#include <phySetTrxState.h>
#include <machwdTxFrameHandler.h>
#include <macenvPib.h>
#include <phyMemAccess.h>
#include <phyRtimerIrqDispatcher.h>

/******************************************************************************
                    Types section
******************************************************************************/
#define PHY_RETRANSMISSION_DELAY_BASE         1000UL // [us]
#define PHY_RETRANSMISSION_DELAY_DISPERSION   7500UL // [us]

/******************************************************************************
                        External variables.
******************************************************************************/
extern uint8_t machwdFrameRetries;

/******************************************************************************
                        Prototypes section.
******************************************************************************/
/******************************************************************************
  Performs data transmission.
  Parameters: frame - pointer to the data to be sent.
  Parameters: trxCmd - command to be sent to the RF after transmission is
              done - the goal is to change the RF state.
  Returns:    PHY_IN_PROGRESS_REQ_STATUS.
******************************************************************************/
PHY_ReqStatus_t PHY_DataReq(uint8_t *frame, PHY_TrxCmd_t trxCmd);

/******************************************************************************
                 Inline static functions prototypes section.
******************************************************************************/
/******************************************************************************
  Finalize transmit operation.
  Parameters: none.
  Returns:    none
******************************************************************************/
INLINE void phyFinishTxFrame(void)
{
  RegTrxState_t txStatus;
  uint8_t retryCounter = machwdTxFrameHandlerRetryCounter;
  uint8_t frameRetries = csPIB.macAttr.maxFrameRetries;
  uint8_t tmpRegValue = phyReadRegister(TRX_STATUS_REG);

  if (TRX_STATUS_TX_ARET_ON == (*((RegTrxStatus_t *)&tmpRegValue)).trxStatus)
  {
  *((uint8_t *)&txStatus) = phyReadRegisterInline(TRX_STATE_REG);

  {
    PhyTrxState_t trxState = phyConvertTrxCmdToTrxState(*phyMemPhyTrxCmd());

    phyWriteRegisterInline(TRX_STATE_REG, *phyMemPhyTrxCmd());
    while (trxState != (phyReadRegisterInline(TRX_STATUS_REG) & REG_TRX_STATUS_TRX_STATUS_MASK));
    *phyMemPhyTrxState() = trxState;
  }
  *machwdMemDataStatus() = txStatus.tracStatus;
  if ((PHY_CHANNEL_ACCESS_FAIL_DATA_STATUS == txStatus.tracStatus ||
        PHY_NO_ACK_DATA_STATUS == txStatus.tracStatus) && retryCounter)
  {
    uint16_t waitPeriod = (uint16_t)((((uint32_t)machwdTxFrameHandlerRandomSeed *
                          (PHY_RETRANSMISSION_DELAY_DISPERSION - (frameRetries  - retryCounter) * 1000UL)) >> 16U) +
                          PHY_RETRANSMISSION_DELAY_BASE * (frameRetries  - retryCounter + 2U));
    HAL_StopRtimer();
    HAL_StartRtimer(HAL_RTIMER_ONE_SHOT_MODE, waitPeriod);
    phySetRTimerSource(HAL_RF_TX_RETRY_DELAY_ID);
  }
  else
  {
    machwdTxFrameHandlerSubState = MACHWD_IDLE_SUBSTATE;
    machwdSetHandlerId(MACHWD_TX_FRAME_HNDLR_ID);
    MACHWD_PostTask();
  }
  }
#ifdef _ZGPD_SPECIFIC_
  else if (PHY_PLL_ON_TRX_STATE == (*((RegTrxStatus_t *)&tmpRegValue)).trxStatus)
  {
    macHwdZgpFinishTxFrameHandling();
  }
#endif
  else if (TRX_STATUS_RX_AACK_ON == (*((RegTrxStatus_t *)&tmpRegValue)).trxStatus)
  {
    // Auto ACK transmission completed
#if defined(_FFD_) && defined(_CONTROL_FRAME_PENDING_BIT_)
    // Restore frame pending bit within RX_AACK_ON for AT86RF233
    tmpRegValue = phyReadRegister(CSMA_SEED_1_REG);
    phyWriteRegister(CSMA_SEED_1_REG, (REG_CSMA_SEED_1_AACK_SET_PD_MASK | tmpRegValue));
#endif
    phySetState(PHY_IDLE_STATE);
  }
}

#endif /* _PHYTXFRAME_H */

// eof phyTxFrame.h
