/**************************************************************************//**
  \file nwkAddressConflict.h

  \brief Interface of address conflict resolver.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2009-06-23  Max Gekk - Created.
    2010-07-04  Max Gekk - Redesign.
   Last change:
    $Id: nwkAddressConflict.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#if !defined _NWK_ADDRESS_CONFLICT_H
#define _NWK_ADDRESS_CONFLICT_H

/******************************************************************************
                              Includes section
 ******************************************************************************/
#include <nwkConfig.h>
#include <sysTypes.h>
#include <sysQueue.h>
#include <mac.h>
#include <nwkCommon.h>
#include <nlmeJoin.h>
#include <nwkStatusReq.h>

/******************************************************************************
                               Types section
 ******************************************************************************/
/** Internal states of the address conflict component. */
typedef enum _NwkAddrConflictState_t
{
  NWK_ADDR_CONFLICT_IDLE_STATE = 0xAA,
  NWK_ADDR_CONFLICT_FIRST_STATE = 0x43,
  NWK_ADDR_CONFLICT_BEGIN_STATE = NWK_ADDR_CONFLICT_FIRST_STATE,
  NWK_ADDR_CONFLICT_MAC_SET_STATE = 0x44,
  NWK_ADDR_CONFLICT_REJOIN_STATE = 0x45,
  NWK_ADDR_CONFLICT_SEND_STATUS_STATE = 0x46,
  NWK_ADDR_CONFLICT_RESOLVING_STATE = 0x47,
  NWK_ADDR_CONFLICT_LAST_STATE
} NwkAddrConflictState_t;

/** Internal variables of the network manager component. */
typedef struct _NwkAddrConflict_t
{
  /** Finite-state machine */
  NwkAddrConflictState_t state;
  /** Current conflict address. */
  ShortAddr_t shortAddr;
  bool sendNwkStatus;
  union
  {
    MAC_SetReq_t macSet;
    NWK_JoinReq_t rejoin;
    NwkStatusReq_t nwkStatus;
  } req;
} NwkAddrConflict_t;

/******************************************************************************
                              Prototypes section
 ******************************************************************************/
#if defined _RESOLVE_ADDR_CONFLICT_
/**************************************************************************//**
  \brief Reset address conflict component.
 ******************************************************************************/
NWK_PRIVATE void nwkResetAddressConflict(void);

/**************************************************************************//**
  \brief Main task handler of address conflict component.
 ******************************************************************************/
NWK_PRIVATE void nwkAddressConflictTaskHandler(void);

/**************************************************************************//**
  \brief Resolve address conflict for given short address.

  \param[in] shortAddr - conflicted short address.
  \return None.
******************************************************************************/
NWK_PRIVATE void nwkResolveAddressConflict(const ShortAddr_t shortAddr,
  const bool sendNwkStatus);

/******************************************************************************
  \brief nwkAddressConflict idle checking.

  \return true, if nwkAddressConflict performs no activity, false - otherwise.
 ******************************************************************************/
NWK_PRIVATE bool nwkAddressConflictIsIdle(void);

#else /* _RESOLVE_ADDR_CONFLICT_ */

#define nwkResetAddressConflict() (void)0
#define nwkAddressConflictTaskHandler NULL
#define nwkAddressConflictIsIdle NULL
#define nwkResolveAddressConflict(shortAddr, sendNwkStatus) ((void)0)

#endif /* _RESOLVE_ADDR_CONFLICT_ */
#endif /* _NWK_ADDRESS_CONFLICT_H */
/** eof nwkAddressConflict.h */

