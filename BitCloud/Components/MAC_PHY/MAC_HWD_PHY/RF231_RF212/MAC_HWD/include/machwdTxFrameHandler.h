/******************************************************************************
  \file machwdTxFrameHandler.h

  \brief Prototypes of Tx Frame handler.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    09/06/07 A. Mandychev - Created.
  Last change:
    $Id: machwdTxFrameHandler.h 27584 2015-01-09 14:45:42Z unithra.c $
******************************************************************************/

#ifndef _MACHWDTXFRAMEHANDLER_H
#define _MACHWDTXFRAMEHANDLER_H

/******************************************************************************
                    Includes section
******************************************************************************/
#include <sysTypes.h>
#include <machwdMem.h>
#include <phyDataStatus.h>
#include <machwdData.h>
#include <machwdManager.h>
#include <machwdHandlerCtrl.h>

/******************************************************************************
                    Types section
******************************************************************************/
typedef enum
{
  MACHWD_IDLE_SUBSTATE,
  MACHWD_SETTXONSTATE_SUBSTATE,
  MACHWD_TX_IN_PROGRESS_SUBSTATE
} TxFrameHandlerSubState_t;

/*****************************************************************************
                              External variables section
******************************************************************************/
extern TxFrameHandlerSubState_t machwdTxFrameHandlerSubState;
extern uint8_t machwdTxFrameHandlerRetryCounter;
extern uint16_t machwdTxFrameHandlerRandomSeed;

/******************************************************************************
                    Prototypes section
******************************************************************************/
/**************************************************************************\\**
  \brief Handles Tx Frame request.

  \param[in] request - pointer to request structure with confirm structure.
******************************************************************************/
MachwdHandlerResult_t machwdTxFrameReqHandler(MACHWD_DataReq_t *request);

/**************************************************************************//**
  \brief Handles Tx frame task. It means that Tx Frame request has
         been initiated already.
 *****************************************************************************/
MachwdHandlerResult_t machwdTxFrameTaskHandler(void);

/**************************************************************************//**
  \brief Resets Tx frame handler.
******************************************************************************/
void machwdResetTxFrameHandler(void);

/******************************************************************************
  \brief Tx frame retry handler.
******************************************************************************/
void machwdTxRetryHandler(void);

/******************************************************************************
                    Inline static functions section
******************************************************************************/
/**************************************************************************//**
  \brief Saves PHY data status and posts new task. This routine is invoked
         from IRQ handler.

  \param[in] status - PHY data status.
******************************************************************************/
// concurrency (asynchronous) process
INLINE void PHY_DataConf(PHY_DataStatus_t status)
{
  *machwdMemDataStatus() = status;
  machwdTxFrameHandlerSubState = MACHWD_IDLE_SUBSTATE;
  machwdSetHandlerId(MACHWD_TX_FRAME_HNDLR_ID);
  SYS_PostTask(MAC_PHY_HWD_TASK_ID);
}

#endif /* _MACHWDTXFRAMEHANDLER_H */

// eof machwdTxFrameHandler.h
