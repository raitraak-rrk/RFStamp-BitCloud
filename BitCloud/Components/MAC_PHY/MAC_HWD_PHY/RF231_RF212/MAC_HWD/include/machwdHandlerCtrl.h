/******************************************************************************
  \file machwdHandlerCtrl.h

  \brief Prototypes of handler control.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    28/05/07 A. Mandychev - Created.
  Last change:
    $Id: machwdHandlerCtrl.h 27584 2015-01-09 14:45:42Z unithra.c $
******************************************************************************/

#ifndef _MACHWDHANDLERCTRL_H
#define _MACHWDHANDLERCTRL_H

/******************************************************************************
                        Includes section.
******************************************************************************/
#include <sysTypes.h>
#include <machwdMemAccess.h>

/******************************************************************************
                        Types section.
******************************************************************************/
typedef enum
{
  MACHWD_SUCCESS_HNDLR_RESULT      = PHY_SUCCESS_REQ_STATUS,
  MACHWD_BUSY_RX_HNDLR_RESULT      = PHY_BUSY_RX_REQ_STATUS,
  MACHWD_IN_PROGRESS_HNDLR_RESULT  = PHY_IN_PROGRESS_REQ_STATUS
} MachwdHandlerResult_t;

typedef enum
{
  MACHWD_IDLE_HNDLR_STATE        = MACHWD_SUCCESS_HNDLR_RESULT,
  MACHWD_BUSY_RX_HNDLR_STATE     = MACHWD_BUSY_RX_HNDLR_RESULT,
  MACHWD_IN_PROGRESS_HNDLR_STATE = MACHWD_IN_PROGRESS_HNDLR_RESULT
} MachwdHandlerState_t;

typedef enum
{
  MACHWD_DATA_REQ_IND_HNDLR_ID = 0,  // Maximum priority.
  MACHWD_RX_FRAME_IND_HNDLR_ID,
  MACHWD_TX_FRAME_HNDLR_ID,
  MACHWD_RX_FRAME_FINISHED_HNDLR_ID,
  MACHWD_BAT_MON_HNDLR_ID,
  MACHWD_TRX_HNDLR_ID,
  MACHWD_SET_HNDLR_ID,
  MACHWD_ED_HNDLR_ID,
  MACHWD_ENCRYPT_HNDLR_ID,
  MACHWD_RND_HNDLR_ID,
  MACHWD_CALIBRATION_HNDLR_ID,
  MACHWD_NEW_MAC_REQ_HNDLR_ID,
  MACHWD_REG_ACCESS_HNDLR_ID,	 // Minimum priority.
  MACHWD_HANDLER_COUNT           // Handlers counter
} MachwdHandlerId_t;

/******************************************************************************
                        External variables.
******************************************************************************/
// Do not use it in other files.
extern uint16_t __handlers;
extern uint16_t __handlersMask;
extern void *__macRequest;

/******************************************************************************
                        Prototypes section.
******************************************************************************/
/**************************************************************************//**
  \brief Returns posted task identifier.

  \param[out] handler identifier

  \return BC_SUCCESS in case if posted task is found, BC_FAIL otherwise.
******************************************************************************/
result_t machwdGetHandlerId(MachwdHandlerId_t *handlerId);

/******************************************************************************
                        Inline static functions prototypes section.
******************************************************************************/
/******************************************************************************
  TBD
  Parameters:
  TBD - TBD.
  Returns:
  TBD.
******************************************************************************/
INLINE void machwdStoreMacRequest(void *request)
{
  __macRequest = request;
}

/******************************************************************************
   TBD.
******************************************************************************/
INLINE void *machwdLoadMacRequest(void)
{
  return __macRequest;
}

/******************************************************************************
 TBD
 Parameters:
   TBD - TBD.
 Returns:
   TBD.
******************************************************************************/
INLINE void machwdSetHandlerId(MachwdHandlerId_t handlerId)
{
  __handlers |= (1U << handlerId);
}

/******************************************************************************
 TBD
 Parameters:
   TBD - TBD.
 Returns:
   TBD.
******************************************************************************/
INLINE void machwdDeleteHandlerId(MachwdHandlerId_t handlerId)
{
  __handlers &= ~(1U << handlerId);
}

/**************************************************************************//**
 \brief Delete all machwd handlers.
******************************************************************************/
INLINE void machwdDeleteAllHandlerIds(void)
{
  __handlers = 0U;
}

/**************************************************************************//**
 \brief Disables all machwd handlers.
******************************************************************************/
INLINE void machwdDisableAllHandlers(void)
{
  __handlersMask = 0U;
}

/**************************************************************************//**
 \brief Enables all machwd handlers.
******************************************************************************/
INLINE void machwdEnableAllHandlers(void)
{
  __handlersMask = ((uint16_t)~0U);
}

/**************************************************************************//**
 \brief Enables specified machwd handler.
 \param[in] handlerId - handler identifier to be enabled
******************************************************************************/
INLINE void machwdEnableHandler(MachwdHandlerId_t handlerId)
{
  __handlersMask |= (1U << handlerId);
}

#endif /* _MACHWDHANDLERCTRL_H */

// eof machwdHandlerCtrl.h
