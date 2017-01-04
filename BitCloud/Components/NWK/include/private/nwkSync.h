/**************************************************************************//**
  \file nwkSync.h

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2007-06-27 V. Panov - Created.
    2008-12-10 M. Gekk  - Reduction of usage of the RAM at separate compiling.
    2009-03-10 M. Gekk  - Refactoring.
   Last change:
    $Id: nwkSync.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#if !defined _NWK_SYNC_H
#define _NWK_SYNC_H

/*****************************************************************************
                               Includes section
 *****************************************************************************/
#include <nwkConfig.h>
#include <sysQueue.h>
#include <mac.h>

/******************************************************************************
                              Definitions section
 ******************************************************************************/
/** Maximum synchronization attempts. */
#define MAX_SYNC_FAIL_COUNTER 8U

/******************************************************************************
                                Types section
 ******************************************************************************/
/** Internal states of NLME-SYNC component */
typedef enum _NwkSyncState_t
{
  NWK_SYNC_IDLE_STATE = 0x31,
  NWK_SYNC_FIRST_STATE = 0xA3,
  NWK_SYNC_BEGIN_STATE = NWK_SYNC_FIRST_STATE,
  NWK_SYNC_MAC_POLL_STATE = 0xA4,
  NWK_SYNC_CONFIRM_STATE = 0xA5,
  NWK_SYNC_LAST_STATE
} NwkSyncState_t;

/** Internal parameters of NLME-SYNC component. */
typedef struct _NwkSync_t
{
  /** Internal component's state*/
  NwkSyncState_t state;
  /** Counter of synchronization attempts */
  uint8_t counter;
  /** Queue of NLME-SYNC.request from an upper layer */
  QueueDescriptor_t queue;
  /** MLME-POLL request primitive's parameters */
  MAC_PollReq_t macReq; /**< macReq must be no in union */
} NwkSync_t;

/*****************************************************************************
                            Prototypes section
 *****************************************************************************/
#if defined _ENDDEVICE_
/*************************************************************************//**
  \brief Main task handler of NLME-SYNC component
 *****************************************************************************/
NWK_PRIVATE void nwkSyncTaskHandler(void);

/*************************************************************************//**
  \brief Reset NLME-SYNC component.
 *****************************************************************************/
NWK_PRIVATE void nwkResetSync(void);

/******************************************************************************
  \brief nwkSync idle checking.

  \return true, if nwkSync performs no activity, false - otherwise.
 ******************************************************************************/
NWK_PRIVATE bool nwkSyncIsIdle(void);

#else /* _ENDDEVICE_ */

#define nwkSyncTaskHandler NULL
#define nwkSyncIsIdle NULL
#define nwkResetSync() (void)0

#endif /* _ENDDEVICE_ */
#endif /* _NWK_SYNC_H */
/** eof nwkSync.h */

