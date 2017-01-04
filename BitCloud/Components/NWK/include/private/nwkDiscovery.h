/**************************************************************************//**
  \file nwkDiscovery.h

  \brief Discovery header file.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2007-06-14 S. Vladykin - Created.
    2008-12-10 M. Gekk     - Reduction of the used RAM.
    2009-04-06 M. Gekk     - Refactoring.
   Last change:
    $Id: nwkDiscovery.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#if !defined _NWK_DISCOVERY_H
#define _NWK_DISCOVERY_H

/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <nwkConfig.h>
#include <sysQueue.h>

/******************************************************************************
                                Types section
 ******************************************************************************/
/** Internal states of NLME-NETWORK-DISCOVERY component */
typedef enum _NwkDiscoveryState_t
{
  NWK_DISCOVERY_IDLE_STATE = 0xD6,
  NWK_DISCOVERY_FIRST_STATE = 0x70,
  NWK_DISCOVERY_BEGIN_STATE = NWK_DISCOVERY_FIRST_STATE,
  NWK_DISCOVERY_MAC_SCAN_STATE = 0x71,
  NWK_DISCOVERY_CONFIRM_STATE = 0x72,
  NWK_DISCOVERY_LAST_STATE
} NwkDiscoveryState_t;

/** Internal parameters of NLME-NETWORK-DISCOVERY */
typedef struct _NwkDiscovery_t
{
  QueueDescriptor_t queue; /**< Queue of requests from upper layer */
  NwkDiscoveryState_t state; /**< finite-state machine */
} NwkDiscovery_t;

/******************************************************************************
                               Prototypes section
 ******************************************************************************/
/**************************************************************************//**
  \brief Main task handler of NLME-NETWORK-DISCOVERY component
 ******************************************************************************/
NWK_PRIVATE void nwkDiscoveryTaskHandler(void);

/**************************************************************************//**
  \brief Reset NLME-NETWORK-DISCOVERY component
 ******************************************************************************/
NWK_PRIVATE void nwkResetDiscovery(void);

/******************************************************************************
  \brief nwkDiscovery idle checking.

  \return true, if nwkDiscovery performs no activity, false - otherwise.
 ******************************************************************************/
NWK_PRIVATE bool nwkDiscoveryIsIdle(void);

#endif /* _NWK_DISCOVERY_H */
/** eof nwkDiscovery.h */

