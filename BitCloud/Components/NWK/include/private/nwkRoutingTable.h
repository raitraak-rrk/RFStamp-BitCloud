/**************************************************************************//**
  \file nwkRoutingTable.h

  \brief Interface of the routing table.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2009-11-15 Max Gekk - Created.
   Last change:
    $Id: nwkRoutingTable.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#if !defined _NWK_ROUTING_TABLE_H
#define _NWK_ROUTING_TABLE_H

/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <nwkConfig.h>
#include <appFramework.h>
#include <nwkCommon.h>
#include <nwkSystem.h>
#include <nwkRouteTable.h>

/******************************************************************************
                                Types section
 ******************************************************************************/

/******************************************************************************
                              Prototypes section
 ******************************************************************************/
#if defined NWK_ROUTING_CAPACITY \
  && (defined _NWK_MESH_ROUTING_ || defined _NWK_MANY_TO_ONE_ROUTING_)
/**************************************************************************//**
  \brief Allocate a new entry in the routing table.

  \return pointer to an entry or NULL.
 ******************************************************************************/
NWK_PRIVATE NwkRoutingTableEntry_t* nwkAllocRoutingEntry(void);

/**************************************************************************//**
  \brief Free an entry of the routing table.

  \param[in] dstAddr - the short address of destination node.
  \param[in] isGroupId - 'true' if dstAddr is group id otherwise 'false'.
  \return None.
 ******************************************************************************/
NWK_PRIVATE void nwkFreeRoutingEntry(const ShortAddr_t dstAddr,
  const bool isGroupId);

/**************************************************************************//**
  \brief Free all entries with the given next hop.

  \param[in] nextHop - the short address of next hop node.
  \return None.
 ******************************************************************************/
NWK_PRIVATE void nwkDeleteNextHop(const ShortAddr_t nextHop);

/**************************************************************************//**
  \brief Delete the device from the routing table.

    Free all entries with next hop address or destination address are equal
  to the given short address.

  \param[in] shortAddr - the short address of removed node.
  \return None.
 ******************************************************************************/
INLINE void nwkDeleteFromRoutingTable(const ShortAddr_t shortAddr)
{
  nwkFreeRoutingEntry(shortAddr, false);
  nwkDeleteNextHop(shortAddr);
}

/**************************************************************************//**
  \brief Find a routing table entry by destination address.

  \param[in] dstAddr - a short address of destination node.
  \param[in] isGroupId - 'true' if dstAddr is group id otherwise 'false'.

  \return pointer to a routing table entry with given dstAddr.
 ******************************************************************************/
NWK_PRIVATE
NwkRoutingTableEntry_t* nwkFindRoutingEntry(const ShortAddr_t dstAddr,
  const bool isGroupId);

/******************************************************************************
 \brief Update information of the routing table entry after a packet transmission.

 \param[in] entry - pointer to an entry in the routing table.
 \param status - NWK status of packet transmission.
 ******************************************************************************/
NWK_PRIVATE void nwkUpdateRoutingEntry(NwkRoutingTableEntry_t *const entry,
  const NWK_Status_t status);

/**************************************************************************//**
  \brief Reset the routing table.
 ******************************************************************************/
NWK_PRIVATE void nwkResetRoutingTable(void);

#if defined _NWK_MESH_ROUTING_
/**************************************************************************//**
  \brief Setting the next hop address of the routing table entry.

  \param[in] dstAddr - a short address of destination node.
  \param[in] nextHopAddr - short address of next hop node.
  \param[in] isGroupId - 'true' if dstAddr is group id otherwise 'false'.
  \param[in] cost - cost of path to destination node.
  \return None.
 ******************************************************************************/
NWK_PRIVATE void nwkUpdateNextHop(const ShortAddr_t dstAddr,
  const bool isGroupId, const ShortAddr_t nextHopAddr,
  const NwkPathCost_t cost);

#else
#define nwkUpdateNextHop(dstAddr, isGroupId, nextHopAddr, cost) (void)0
#endif /* _NWK_MESH_ROUTING_ */
#else /* NWK_ROUTING_CAPACITY and (_NWK_MESH_ROUTING_ or _NWK_MANY_TO_ONE_ROUTING_) */

#define nwkResetRoutingTable() (void)0
#define nwkUpdateNextHop(dstAddr, isGroupId, nextHopAddr, cost) (void)0
#define nwkFreeRoutingEntry(dstAddr, isGroupId) (void)0
#define nwkRemoveNextHop(nextHop) (void)0
#define nwkDeleteFromRoutingTable(shortAddr) (void)0

#endif /* NWK_ROUTING_CAPACITY and (_NWK_MESH_ROUTING_ or _NWK_MANY_TO_ONE_ROUTING_) */
#endif /* _NWK_ROUTING_TABLE_H */
/** eof nwkRoutingTable.h */

