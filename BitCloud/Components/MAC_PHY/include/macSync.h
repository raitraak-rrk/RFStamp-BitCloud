/**************************************************************************//**
  \file macSync.h

  \brief Types', constants' and functions' declarations for IEEE 802.15.4-2006
    sync primitives.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      18/06/07 ALuzhetsky - Created.
******************************************************************************/

#ifndef _MACSYNC_H
#define _MACSYNC_H

/******************************************************************************
                        Includes section
******************************************************************************/

/******************************************************************************
                        Defines section
******************************************************************************/

/******************************************************************************
                        Types section
******************************************************************************/
//! Set of loss indication reasons. IEEE 802.15.4-2006 Table 75.
typedef enum
{
  MAC_PAN_ID_CONFLICT = 0,
  MAC_REALIGNMENT     = 1,
  MAC_BEACON_LOSS     = 2
} MAC_SyncLossReason_t;

//! MLME-SYNC_LOSS indication structure. IEEE 802.15.4-2006 7.1.15.2
typedef struct
{
  //! The reason that synchronization was lost.
  MAC_SyncLossReason_t reason;
  //! The PAN identifier with which the device lost synchronization or to which
  //! it was realigned.
  PanId_t              panId;
  //! The logical channel on which the device lost synchronization or to which it
  //! was realigned.
  uint8_t              logicalChannel;
  //! The channel page on which the device lost synchronization or to which it
  //! was realigned.
  uint8_t              channelPage;
}  MAC_SyncLossInd_t;


/******************************************************************************
                        Prototypes section
******************************************************************************/
/**************************************************************************//**
  \brief MLME-SYNC_LOSS indication primitive's prototype.
  \param indParams - MLME-SYNC_LOSS indication parameters' structure pointer.
  \return none.
******************************************************************************/
extern void MAC_SyncLossInd(MAC_SyncLossInd_t *indParams);

#endif /* _MACSYNC_H */

// eof macSync.h
