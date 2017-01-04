/**************************************************************************//**
  \file nwkRouteDiscovery.h

  \brief Network route discovery header file.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2007-09-05 V. Panov - Created.
    2008-09-06 M. Gekk  - Modified to support parallel processing.
    2010-02-06 M. Gekk  - Refactoring.
   Last change:
    $Id: nwkRouteDiscovery.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#if !defined _NWK_ROUTE_DISCOVERY_H
#define _NWK_ROUTE_DISCOVERY_H

/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <nwkConfig.h>
#include <sysQueue.h>
#include <nlmeRouteDiscovery.h>
#include <nwkCommon.h>

/******************************************************************************
                                 Types section
 ******************************************************************************/
/** Internal states of the route discovery component. */
typedef enum _NwkRouteDiscoveryState_t
{
  NWK_ROUTE_DISCOVERY_IDLE_STATE = 0x99,
  NWK_ROUTE_DISCOVERY_PROCESS_STATE = 0x38
} NwkRouteDiscoveryState_t;

/** Internal state and variables of route discovery component. */
typedef struct _NwkRouteDiscovery_t
{
  QueueDescriptor_t queue;  /**< Queue of requests from other components. */
  /** Low part of the identifier of initial route request. */
  uint8_t routeRequestIdLow;
#if defined _SYS_ASSERT_ON_
  /** Finite-state machine. */
  NwkRouteDiscoveryState_t state;
#endif /* _SYS_ASSERT_ON_ */
} NwkRouteDiscovery_t;

/******************************************************************************
                                Prototypes section
 ******************************************************************************/
#if defined NWK_ROUTING_CAPACITY \
  && (defined _NWK_MESH_ROUTING_ || defined _NWK_CONCENTRATOR_)
/**************************************************************************//**
  \brief Task handler of the route discovery component.
 ******************************************************************************/
NWK_PRIVATE void nwkRouteDiscoveryTaskHandler(void);

/******************************************************************************
  \brief Reset the route discovery component.
 ******************************************************************************/
NWK_PRIVATE void nwkResetRouteDiscovery(void);

/******************************************************************************
  \brief Flush all route discovery requests.
 ******************************************************************************/
NWK_PRIVATE void nwkFlushRouteDiscovery(void);

/******************************************************************************
  \brief nwkRouteDiscovery idle checking.

  \return true, if nwkRouteDiscovery performs no activity, false - otherwise.
 ******************************************************************************/
NWK_PRIVATE bool nwkRouteDiscoveryIsIdle(void);

#else /* no NWK_ROUTING_CAPACITY */

#define nwkRouteDiscoveryTaskHandler NULL
#define nwkRouteDiscoveryIsIdle NULL
#define nwkResetRouteDiscovery() (void)0
#define nwkFlushRouteDiscovery() (void)0

#endif /* NWK_ROUTING_CAPACITY and (_NWK_MESH_ROUTING_ or _NWK_CONCENTRATOR_) */
#endif /* _NWK_ROUTE_DISCOVERY_H */
/** eof nwkRouteDiscovery.h */

