/**************************************************************************//**
  \file nwkRouteTable.h

  \brief Interface of the routing table.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2013-11-21 Mahendran P - Created.
   Last change:
    $Id: nwkRoutingTable.h 22131 2013-11-21 08:58:09Z mahendranp $
 ******************************************************************************/
#if !defined _NWK_ROUTE_TABLE_H
#define _NWK_ROUTE_TABLE_H

/******************************************************************************
                               Includes section
 ******************************************************************************/

/******************************************************************************
                                Types section
 ******************************************************************************/

/** Type of routing rate. */
typedef uint8_t NwkRoutingRate_t;
/** Type of routing table size. */
typedef uint16_t NwkRoutingTableSize_t;
/** Type of routing entry index. */
typedef NwkRoutingTableSize_t NwkRoutingEntryIndex_t;

/** Routing table entry. ZigBee spec r17, Table 3.51. */
typedef struct _NwkRoutingTableEntry_t
{
  /** The 16-bit network address or Group ID of this route. If the destination
   * device is a ZigBee router, ZigBee coordinator, or an end device, and
   * nwkAddrAlloc has a value of 0x02, this field shall contain the actual
   * 16-bit address of that device. If the destination device is an end device
   * and nwkAddrAlloc has a value of 0x00, this field shall contain the 16-bit
   * network address of the device's parent. */
  ShortAddr_t dstAddr;
  /** The 16-bit network address of the next hop on the way to the destination.
   **/
  ShortAddr_t nextHopAddr;
  /** A flag indicating that the entry is active. */
  bool active              : 1;
  /** A flag indicating that the destination indicated by this address does
   * not store source routes. */
  bool noRouteCache        : 1;
  /** A flag indicating that the destination is a concentrator
   * that issued a many-to-one route request. */
  bool manyToOne           : 1;
  /** A flag indicating that a route record command frame should be sent to
   * the destination prior to the next data packet. */
  bool routeRecordRequired : 1;
  /** A flag indicating that the destination address is a Group ID. */
  bool groupId             : 1;
  /** Counter of the many-to-one discovery periods
   * without any source route packets. */
  NwkBitField_t noSourceRoutePeriods  : 2;
  /** Indicate to upper layer about new concentrator. */
  bool newConcentrator     : 1;
  NwkRoutingRate_t rate;
  /** Cost of route path to destination node. */
  NwkPathCost_t cost;
} NwkRoutingTableEntry_t;

/** The routing table. */
typedef struct _NwkRoutingTable_t
{
  /** Pointer to first entry of the routing table. */
  NwkRoutingTableEntry_t *table;
  /** Current size of the routing table. */
  NwkRoutingTableSize_t size;
  /* Number of failure transmission after that a routing entry will be deleted.
   **/
  uint8_t failOrder;
} NwkRoutingTable_t;

typedef NwkRoutingTable_t RouteTable_t;

/******************************************************************************
                              Prototypes section
******************************************************************************/
/**************************************************************************//**
  \brief Find a next active entry from the Routing table.

  \param[in] entry - pointer to an routing entry or NULL.

  \return first active entry if entry is NULL
          or next active entry,
          or NULL if next active entry is not found.
 ******************************************************************************/
NwkRoutingTableEntry_t* NWK_NextRoutingEntry(NwkRoutingTableEntry_t *entry);

#endif /* _NWK_ROUTE_TABLE_H */
/** eof nwkRouteTable.h */

