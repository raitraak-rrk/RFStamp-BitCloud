/******************************************************************************
  \file phyCommon.h

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      21/09/07 A. Luzhetsky - Created.
******************************************************************************/

#ifndef _PHYCOMMON_H
#define _PHYCOMMON_H

/******************************************************************************
                    Includes section
******************************************************************************/
#include <at86rf2xx.h>

/******************************************************************************
                    Types section
******************************************************************************/
typedef enum
{
  PHY_SUCCESS_REQ_STATUS,
  PHY_BUSY_RX_REQ_STATUS,
  PHY_IN_PROGRESS_REQ_STATUS
} PHY_ReqStatus_t;

typedef enum
{
  HAL_ED_DELAY_ID,
  HAL_RF_STATE_POLL_DELAY_ID,
  HAL_RF_RTS_STATE_POLL_DELAY_ID,
  HAL_RF_TX_RETRY_DELAY_ID,
#ifdef _ZGPD_SPECIFIC_
  HAL_IDENTICAL_FRAME_DELAY_ID,
  HAL_RX_AFTER_TX_INTERVAL_ID,
  HAL_RX_OFFSET_ID
#endif /* End of _ZGPD_SPECIFIC_ */
} HAL_RTimerSource_t;

typedef enum
{
  PHY_FORCE_TRX_OFF_CMD = TRX_CMD_FORCE_TRX_OFF,
  PHY_RX_ON_CMD         = TRX_CMD_RX_ON,
  PHY_TRX_OFF_CMD       = TRX_CMD_TRX_OFF,
  PHY_RX_AACK_ON_CMD    = TRX_CMD_RX_AACK_ON,
  PHY_TX_ARET_ON_CMD    = TRX_CMD_TX_ARET_ON,
  PHY_PLL_ON_CMD        = TRX_CMD_PLL_ON
} PHY_TrxCmd_t;

typedef enum
{
  PHY_RX_ON_TRX_STATE       = TRX_STATUS_RX_ON,
  PHY_OFF_TRX_STATE         = TRX_STATUS_TRX_OFF,
  PHY_RX_AACK_ON_TRX_STATE  = TRX_STATUS_RX_AACK_ON,
  PHY_TX_ARET_ON_TRX_STATE  = TRX_STATUS_TX_ARET_ON,
  PHY_PLL_ON_TRX_STATE      = TRX_STATUS_PLL_ON
} PhyTrxState_t;

typedef enum
{
  PHY_IDLE_STATE,
  PHY_BUSY_RX_STATE
} PhyState_t;

#endif /* _PHYCOMMON_H */

// eof phyCommon.h
