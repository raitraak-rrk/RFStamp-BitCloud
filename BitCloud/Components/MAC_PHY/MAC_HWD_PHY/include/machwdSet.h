/**************************************************************************//**
  \file machwdSet.h

  \brief MACHWD set types and function prototypes.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      26/09/07 A. Mandychev - Created.
******************************************************************************/

#ifndef _MACHWDSET_H
#define _MACHWDSET_H

/******************************************************************************
                    Includes section
******************************************************************************/
#include <inttypes.h>
#include <machwdService.h>
#include <macphyPib.h>

/******************************************************************************
                    Define(s) section
******************************************************************************/
//! Start point for Atmel own identifier counting.
#define MACHWD_ATMEL_RESERVED_ID 0xF0

/******************************************************************************
                    Types section
******************************************************************************/
//! MACHWD PIB identifiers.
typedef enum
{
  // PHY id.
  MACHWD_PIB_CURRENT_CHANNEL_ID     = PHY_PIB_CURRENT_CHANNEL_ID,
  MACHWD_PIB_TRANSMIT_POWER_ID      = PHY_PIB_TRANSMIT_POWER_ID,
  MACHWD_PIB_CCA_MODE_ID            = PHY_PIB_CCA_MODE_ID,
  MACHWD_PIB_CURRENT_PAGE_ID        = PHY_PIB_CURRENT_PAGE_ID,
  // MAC id.
  MACHWD_PIB_MAX_CSMA_BACKOFFS_ID   = MAC_PIB_MAX_CSMA_BACKOFFS_ID,
  MACHWD_PIB_PANID_ID               = MAC_PIB_PANID_ID,
  MACHWD_PIB_SHORT_ADDR_ID          = MAC_PIB_SHORT_ADDR_ID,
  MACHWD_PIB_MIN_BE_ID              = MAC_PIB_MIN_BE_ID,
  MACHWD_PIB_MAX_FRAME_RETRIES_ID   = MAC_PIB_MAX_FRAME_RETRIES_ID,
  MACHWD_PIB_EXT_ADDR_ID            = MAC_PIB_EXT_ADDR_ID,
  // Additional id for software needs.
  MACHWD_PIB_RF_IRQ_DISABLE_ID      = MACHWD_ATMEL_RESERVED_ID,
  MACHWD_PIB_BEACON_RX_MODE_ID      = MACHWD_ATMEL_RESERVED_ID + 1,
  // Additional id for hardware needs.
  MACHWD_PIB_PAN_COORDINATOR_ID     = MACHWD_ATMEL_RESERVED_ID + 2,
  MACHWD_PIB_CCA_ED_THRES_ID        = MAC_PIB_CCA_ED_THRES,
} MACHWD_PibId_t;

//! MACHWD PIB attribute type.
typedef union
{
  MAC_PibAttr_t macPib;
  PHY_PibAttr_t phyPib;
  bool panCoordinator;
  bool rfIrqDisable;
  bool beaconRxMode;
} MACHWD_PibAttr_t;

//! MACHWD set request structure.
typedef struct
{
  //! Service field - for internal needs.
  MACHWD_Service_t  service;
  //! MACHWD PIB identifier.
  MACHWD_PibId_t    id;
  //! MACHWD PIB attribute.
  MACHWD_PibAttr_t  attr;
  // MACHWD set confirm callback function's pointer.
  void (*MACHWD_SetConf)(void);
} MACHWD_SetReq_t;

/******************************************************************************
                    Prototypes section
******************************************************************************/
/**************************************************************************//**
  \brief Sets MACHWD attributes.
  \param reqParams - MACHWD set request structure pointer.
  \return none.
******************************************************************************/
void MACHWD_SetReq(MACHWD_SetReq_t *reqParams);

#endif /* _MACHWDSET_H */

// eof machwdSet.h
