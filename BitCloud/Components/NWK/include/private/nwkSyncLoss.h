/**************************************************************************//**
  \file nwkSyncLoss.h

  \brief SyncLoss header file.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2009-04-02 M. Gekk - Created.
   Last change:
    $Id: nwkSyncLoss.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#if !defined _NWKSYNCLOSS_H
#define _NWKSYNCLOSS_H

/******************************************************************************
                                 Includes section
 ******************************************************************************/
#include <nwkConfig.h>
#include <nlmeStartRouter.h>

/******************************************************************************
                                  Types section
 ******************************************************************************/
/** Internal states of SyncLoss component */
typedef enum _NwkSyncLossState_t
{
  NWK_SYNC_LOSS_IDLE_STATE = 0x00,
  NWK_REALIGNMENT_START_STATE = 0x01
} NwkSyncLossState_t;

/** Internal parameters of SyncLoss */
typedef struct _NwkSyncLoss_t
{
  NwkSyncLossState_t state;
  NWK_StartRouterReq_t startRouter;
} NwkSyncLoss_t;

/******************************************************************************
                                Prototypes section
 ******************************************************************************/
#if defined(_ROUTER_) && defined(NWK_COORD_REALIGNMENT)
/******************************************************************************
  \brief Reset SyncLoss component.
 ******************************************************************************/
NWK_PRIVATE void nwkResetSyncLoss(void);

#else /* _ROUTER_ && NWK_COORD_REALIGNMENT */

#define nwkResetSyncLoss() (void)0

#endif /* _ROUTER_ && NWK_COORD_REALIGNMENT */
#endif /* _NWKSYNCLOSS_H */
/** eof nwkSyncLoss.h */

