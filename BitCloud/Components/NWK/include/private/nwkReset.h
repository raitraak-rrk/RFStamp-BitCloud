/**************************************************************************//**
  \file nwkReset.h

  \brief NWK reset header file.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2007-06-20 V. Panov - Created.
    2009-03-07 M. Gekk  - Refactoring.
   Last change:
    $Id: nwkReset.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#if !defined _NWK_RESET_H
#define _NWK_RESET_H

/******************************************************************************
                                Includes section
 ******************************************************************************/
#include <nwkConfig.h>
#include <sysQueue.h>
#include <mac.h>

/******************************************************************************
                                 Types section
 ******************************************************************************/
/** Internal states of the NLME-RESET component. */
typedef enum _NwkResetState_t
{
  NWK_RESET_IDLE_STATE = 0x19,
  NWK_RESET_FIRST_STATE = 0xC1,
  NWK_RESET_BEGIN_STATE = NWK_RESET_FIRST_STATE,
  NWK_RESET_CONFIRM_STATE = 0xC2,
  NWK_RESET_MAC_STATE = 0xC3,
  NWK_RESET_RX_ENABLE_STATE = 0xC4,
  NWK_RESET_SET_SHORT_ADDRESS_STATE = 0xC5,
  NWK_RESET_SET_COORD_SHORT_ADDR_STATE = 0xC6,
  NWK_RESET_SET_PANID_STATE = 0xC7,
  NWK_RESET_SET_COORD_EXT_ADDR_STATE = 0xC8,
  NWK_RESET_SET_ASSOC_PAN_COORD_STATE = 0xC9,
  NWK_RESET_SET_CHANNEL_STATE = 0xCA,
  NWK_RESET_SET_RX_ON_WHEN_IDLE_STATE = 0xCB,
  NWK_RESET_SET_BEACON_PAYLOAD_STATE = 0xCC,
  NWK_RESET_SET_BEACON_PAYLOAD_LEN_STATE = 0xCD,
  NWK_RESET_SET_ASSOC_PERMIT_STATE = 0xCE,
  NWK_RESET_MAC_START_STATE = 0xCF,
  NWK_RESET_SET_TRANSACTION_TIME_STATE = 0xD0,
  NWK_RESET_LAST_STATE
} NwkResetState_t;

/** Internal parameters of NLME-RESET component. */
typedef struct _NwkReset_t
{
  /** Finite-state machine */
  NwkResetState_t state;
  /** Queue of requests from upper layer */
  QueueDescriptor_t queue;
  union
  {
    MAC_ResetReq_t reset;
    MAC_SetReq_t set;
    MAC_RxEnableReq_t rxEnable;
    MAC_StartReq_t start;
  } mac;
} NwkReset_t;

/** Type of reset function. */
typedef void (* nwkResetFunction_t)(void);

/******************************************************************************
                             Prototypes section
 ******************************************************************************/
/**************************************************************************//**
  \brief Task handler of reset component.
 ******************************************************************************/
NWK_PRIVATE void nwkResetTaskHandler(void);

/******************************************************************************
  \brief nwkReset idle checking.

  \return true, if nwkReset performs no activity, false - otherwise.
 ******************************************************************************/
NWK_PRIVATE bool nwkResetIsIdle(void);

/******************************************************************************
  \brief NWK layer reset phase completion checker.

  \return true, if NWK reset done, false - otherwise.
 ******************************************************************************/
NWK_PRIVATE bool nwkResetCompleted(void);

#endif /* _NWK_RESET_H */
/** eof nwkReset.h */

