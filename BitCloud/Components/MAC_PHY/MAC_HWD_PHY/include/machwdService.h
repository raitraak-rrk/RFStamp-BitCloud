/**************************************************************************//**
  \file machwdService.h

  \brief Describes types' declarations for internal needs.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      30/05/07 A. Mandychev - Created.
******************************************************************************/

#ifndef _MACHWDSERVISE_H
#define _MACHWDSERVISE_H

/******************************************************************************
                        Includes section.
******************************************************************************/
#include <sysTypes.h>
#include <macCommon.h>

/******************************************************************************
                        Types section.
******************************************************************************/
//! Set of MACHWD request identifiers.
enum
{
  MACHWD_TX_DATA_REQ_ID,
  MACHWD_SET_TRX_STATE_REQ_ID,
  MACHWD_SET_REQ_ID,
  MACHWD_ED_REQ_ID,
#ifdef _RF_AES_
  MACHWD_ENCRYPT_REQ_ID,
#endif /*_RF_AES_*/
#ifdef _RF_BAT_MON_
  MACHWD_BAT_MON_REQ_ID,
#endif /*_RF_BAT_MON_*/
#ifdef _RF_RND_
  MACHWD_RND_REQ_ID,
#endif /* _RF_RND_*/
#ifdef _RF_CALIBRATION_
  MACHWD_CALIBRATION_REQ_ID,
#endif
#ifdef _RF_REG_ACCESS_
  MACHWD_REG_WRITE_REQ_ID,
  MACHWD_REG_READ_REQ_ID,
#endif
  MACHWD_HANDLERS_NUMBER // Must be the last in the list
};

//! Inherited service structure.
typedef MAC_Service_t MACHWD_Service_t;

#endif /* _MACHWDSERVISE_H */

// eof machwdService.h
