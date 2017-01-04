/**************************************************************************//**
  \file nwkStartRouter.h

  \brief Start router header file.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2007-06-27 V. Panov - Created.
    2008-12-10 M. Gekk  - Reduction of usage of the RAM at separate compiling.
    2009-03-19 M. Gekk  - Refactoring.
   Last change:
    $Id: nwkStartRouter.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#if !defined _NWK_START_ROUTER_H
#define _NWK_START_ROUTER_H

/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <nwkConfig.h>
#include <sysQueue.h>

/******************************************************************************
                                 Types section
 ******************************************************************************/
/** Internal states of NLME-START-ROUTER component */
typedef enum _NwkStartRouterState_t
{
  NWK_START_ROUTER_IDLE_STATE = 0x9A,
  NWK_START_ROUTER_FIRST_STATE = 0x45,
  NWK_START_ROUTER_BEGIN_STATE = NWK_START_ROUTER_FIRST_STATE,
  NWK_START_ROUTER_MAC_REQ_STATE = 0x46,
  NWK_START_ROUTER_CONFIRM_STATE = 0x47,
  NWK_START_ROUTER_LAST_STATE
} NwkStartRouterState_t;

/** Internal parameters of NLME-START-ROUTER */
typedef struct _NwkStartRouter_t
{
  NwkStartRouterState_t state; /**< Finite-state machine */
  QueueDescriptor_t queue; /**< Queue of requests from upper layer */
  bool coordRealignment;
} NwkStartRouter_t;

/******************************************************************************
                              Prototypes section
 ******************************************************************************/
#if defined _ROUTER_
/**************************************************************************//**
  \brief Main task handler of NLME-START-ROUTER component
 ******************************************************************************/
NWK_PRIVATE void nwkStartRouterTaskHandler(void);

/**************************************************************************//**
  \brief Reset NLME-START-ROUTER component.
 ******************************************************************************/
NWK_PRIVATE void nwkResetStartRouter(void);

/******************************************************************************
  \brief nwkStartRouter idle checking.

  \return true, if nwkStartRouter performs no activity, false - otherwise.
 ******************************************************************************/
NWK_PRIVATE bool nwkStartRouterIsIdle(void);

#if defined NWK_COORD_REALIGNMENT
/**************************************************************************//**
  \brief Is network configuration realignment allowed or not?
 ******************************************************************************/
NWK_PRIVATE bool nwkRealignmentIsAllowed(void);
#endif /* NWK_COORD_REALIGNMENT */

#else /* _ROUTER_ */

#define nwkStartRouterTaskHandler NULL
#define nwkStartRouterIsIdle NULL
#define nwkResetStartRouter() (void)0

#endif /* _ROUTER_ */
#endif /* _NWKSTARTROUTER_H */
/** eof nwkStartRouter.h */

