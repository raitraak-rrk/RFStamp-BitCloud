/**************************************************************************//**
  \file nlmeSyncLoss.h

  \brief NLME-SYNC-LOSS interface

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2009-04-12 Max Gekk - Created.
   Last change:
    $Id: nlmeSyncLoss.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#if !defined _NLME_SYNC_LOSS_H
#define _NLME_SYNC_LOSS_H

/******************************************************************************
                               Prototypes section
 ******************************************************************************/
/**************************************************************************//**
  \brief NLME-SYNC-LOSS indication primitive's prototype.

  \return None.
 ******************************************************************************/
extern void NWK_SyncLossInd(void);

#endif /* _NLME_SYNC_LOSS_H */
/** eof nlmeSyncLoss.h */

