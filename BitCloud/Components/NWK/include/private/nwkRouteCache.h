/**************************************************************************//**
  \file nwkRouteCache.h

  \brief Interface of the route cache (route record table).

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2010-03-15 M. Gekk - Created.
   Last change:
    $Id: nwkRouteCache.h 27887 2015-05-27 10:55:43Z agasthian.s $
 ******************************************************************************/
#if !defined _NWK_ROUTE_CACHE_H
#define _NWK_ROUTE_CACHE_H
/** See format of the route cache record in ZigBee spec r18, Table 3.45. */

/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <nwkConfig.h>
#include <mac.h>
#include <nwkCommon.h>
#include <nwkRouteRecord.h>
#include <nwkRouteInfo.h>

/******************************************************************************
                                Types section
 ******************************************************************************/
/** Type of index in the route cache. */
typedef uint16_t NwkRouteCacheIndex_t;
/** Type of total size of the route cache. */
typedef NwkRouteCacheIndex_t NwkRouteCacheSize_t;

/** Format of a route cache record. ZigBee spec r18, page 350. */
typedef struct _NwkRouteCacheRecord_t
{
  /** The destination network address for this route record.*/
  ShortAddr_t dstAddr;
  /** The count of relay nodes from concentrator to the destination.
   * ZigBee spec r18, Table 3.45, page 350. */
  NWK_RelayCount_t relayCount;
  /** The set of network addresses that represent the route in order from
   * the concentrator to the destination. ZigBee spec r18, Table 3.45. */
  ShortAddr_t path[NWK_MAX_SOURCE_ROUTE];
} NwkRouteCacheRecord_t;

/* Internal variables and state of the route cache. */
typedef struct _NwkRouteCache_t
{
  /* A set of cached route records. */
  NwkRouteCacheRecord_t *record;
  /* Total number of route records. */
  NwkRouteCacheSize_t size;
  /* The index of oldest route record. */
  NwkRouteCacheIndex_t current;
} NwkRouteCache_t;

/******************************************************************************
                               Prototypes section
 ******************************************************************************/
#if defined _NWK_ROUTE_CACHE_
/**************************************************************************//**
  \brief Find a record in the route cache by a destination address.

  \param[in] dstAddr - destination short address. Address must be valid.
  \return Pointer to a record of the route cache if it is found
          otherwise 'false'.
 ******************************************************************************/
NWK_PRIVATE
NwkRouteCacheRecord_t* nwkFindRouteRecord(const ShortAddr_t dstAddr);

/**************************************************************************//**
  \brief Add new record into the route cache.

  Preconditions:
    - Size of relay list must be less NWK_MAX_SOURCE_ROUTE.
    - Destination address must be valid (range 0x0000-0xFFF7).

  \param[in] dstAddr - destination short address of new route record.
  \param[in] routeRecordPayload - pointer to payload of a received route record
                                  command packet.
  \return None.
 ******************************************************************************/
NWK_PRIVATE void nwkAddRouteRecord(const ShortAddr_t dstAddr,
  const NwkRouteRecordPayload_t *const routeRecordPayload);

/**************************************************************************//**
  \brief Delete a record from the route cache.

  \param[in] dstAddr - destination short address for a route record.
  \return None.
 ******************************************************************************/
NWK_PRIVATE void nwkDeleteRouteRecord(const ShortAddr_t dstAddr);

/**************************************************************************//**
  \brief Delete all records that include the given address.

  \param[in] shortAddr - the short address of removed node.
  \return None.
 ******************************************************************************/
NWK_PRIVATE void nwkDeleteFromRouteCache(const ShortAddr_t shortAddr);

/**************************************************************************//**
  \brief Reset the route cache.
  \return None.
 ******************************************************************************/
NWK_PRIVATE void nwkResetRouteCache(void);

#else /* _NWK_ROUTE_CACHE_ */

#define nwkResetRouteCache() (void)0
#define nwkDeleteFromRouteCache(shortAddr) (void)0

#endif /* _NWK_ROUTE_CACHE_ */
#endif /* _NWK_ROUTE_CACHE_H */
/** eof nwkRouteCache.h */

