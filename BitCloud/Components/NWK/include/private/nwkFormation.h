/**************************************************************************//**
  \file nwkFormation.h

  \brief Formation header file.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2007-06-14 S. Vladykin - Created.
    2008-12-19 M. Gekk - Numbers are added to enum of state.
    2009-03-24 M. Gekk - Refactoring.
   Last change:
    $Id: nwkFormation.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#if !defined _NWK_FORMATION_H
#define _NWK_FORMATION_H

/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <nwkConfig.h>
#include <sysTypes.h>
#include <sysQueue.h>

/******************************************************************************
                                Types section
 ******************************************************************************/
/** Internal states of NLME-NETWORK-FORMATION component */
typedef enum _NwkFormationState_t
{
  NWK_FORMATION_IDLE_STATE = 0xB7,
  NWK_FORMATION_FIRST_STATE = 0x54,
  NWK_FORMATION_BEGIN_STATE = NWK_FORMATION_FIRST_STATE,
  NWK_FORMATION_MAC_REALIGNMENT_STATE = 0x55,
  NWK_FORMATION_MAC_ACTIVE_SCAN_STATE = 0x56,
  NWK_FORMATION_MAC_ED_SCAN_STATE = 0x57,
  NWK_FORMATION_CONFIRM_STATE = 0x58,
  NWK_FORMATION_SET_SHORT_ADDR_STATE = 0x59,
  NWK_FORMATION_SET_BEACON_PAYLOAD_LEN_STATE = 0x5A,
  NWK_FORMATION_SET_BEACON_PAYLOAD_STATE = 0x5B,
  NWK_FORMATION_SET_RX_ON_WHEN_IDLE_STATE = 0x5C,
  NWK_FORMATION_MAC_START_STATE = 0x5D,
  NWK_FORMATION_RX_ENABLE_STATE = 0x5E,
  NWK_FORMATION_SET_TRANSACTION_TIME_STATE = 0x5F,
  NWK_FORMATION_SET_EXT_ADDR_STATE = 0x60,
  NWK_FORMATION_LAST_STATE
} NwkFormationState_t;

/** Internal parameters of NLME-NETWORK-FORMATION component. */
typedef struct _NwkFormation_t
{
  QueueDescriptor_t queue; /**< Queue of requests from upper layer. */
  NwkFormationState_t state; /**< finite-state machine. */
  bool coordRealignment;
} NwkFormation_t;

/******************************************************************************
                               Prototypes section
 ******************************************************************************/
#if defined _COORDINATOR_
/**************************************************************************//**
  \brief Main task handler of NLME-NETWORK-FORMATION component.
 ******************************************************************************/
NWK_PRIVATE void nwkFormationTaskHandler(void);

/**************************************************************************//**
  \brief Reset NLME-NETWORK-FORMATION component.
 ******************************************************************************/
NWK_PRIVATE void nwkResetFormation(void);

/******************************************************************************
  \brief nwkFormation idle checking.

  \return true, if nwkFormation performs no activity, false - otherwise.
 ******************************************************************************/
NWK_PRIVATE bool nwkFormationIsIdle(void);

#else /* not defined _COORDINATOR_ */

#define nwkFormationTaskHandler NULL
#define nwkFormationIsIdle NULL
#define nwkResetFormation() (void)0

#endif /* _COORDINATOR_ */
#endif /* _NWK_FORMATION_H */
/** eof nwkFormation.h */

