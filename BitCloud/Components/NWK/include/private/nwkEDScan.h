/**************************************************************************//**
  \file nwkEDScan.h

  \brief EDScan header file.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2007-06-14 S. Vladykin - Created.
    2008-12-10 M. Gekk     - Reduction of the used RAM.
    2009-03-15 M. Gekk     - Refactoring.
   Last change:
    $Id: nwkEDScan.h 27584 2015-01-09 14:45:42Z unithra.c $
 *****************************************************************************/
#if !defined _NWK_EDSCAN_H
#define _NWK_EDSCAN_H

/*****************************************************************************
                              Includes section
 *****************************************************************************/
#include <nwkConfig.h>
#include <sysQueue.h>
#include <nwkCommon.h>

/*****************************************************************************
                              Types section
 *****************************************************************************/
/** Internal states of NLME-ED-SCAN component */
typedef enum _NwkEDScanState_t
{
  NWK_EDSCAN_IDLE_STATE = 0x13,
  NWK_EDSCAN_FIRST_STATE = 0xE7,
  NWK_EDSCAN_BEGIN_STATE = NWK_EDSCAN_FIRST_STATE,
  NWK_EDSCAN_MAC_SCAN_STATE = 0xE8,
  NWK_EDSCAN_CONFIRM_STATE = 0xE9,
  NWK_EDSCAN_LAST_STATE
} NwkEDScanState_t;

/** Internal parameters of NLME-ED-SCAN */
typedef struct _NwkEDScan_t
{
  QueueDescriptor_t queue; /**< Queue of requests from upper layer */
  NwkEDScanState_t state; /**< finite-state machine */
  NwkState_t savedStateMachine;
} NwkEDScan_t;

/*****************************************************************************
                             Prototypes section
 *****************************************************************************/
/*************************************************************************//**
  \brief Reset NLME-ED-SCAN component.
 *****************************************************************************/
NWK_PRIVATE void nwkResetEDScan(void);

/******************************************************************************
  \brief nwkEDScan idle checking.

  \return true, if nwkEDScan performs no activity, false - otherwise.
 ******************************************************************************/
NWK_PRIVATE bool nwkEDScanIsIdle(void);

/*************************************************************************//**
  \brief Main task handler of NLME-ED-SCAN
 *****************************************************************************/
NWK_PRIVATE void nwkEDScanTaskHandler(void);

#endif /* _NWK_EDSCAN_H */
/** eof nwkEDScan.h */

