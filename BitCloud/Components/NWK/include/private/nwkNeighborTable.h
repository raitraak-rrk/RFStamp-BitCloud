/**************************************************************************//**
  \file nwkNeighborTable.h

  \brief Interface of the neighbor table..

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2009-05-12 M. Gekk - Created.
   Last change:
    $Id: nwkNeighborTable.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#if !defined _NWK_NEIGHBOR_TABLE_H
#define _NWK_NEIGHBOR_TABLE_H

/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <mac.h>
#include <nwkNeighbor.h>
#include <nwkMem.h>

/******************************************************************************
                               Define(s) section
 ******************************************************************************/
#define IS_EMPTY(ntEntry) (RELATIONSHIP_EMPTY == ntEntry->relationship)
#define IS_AUTHENTICATED_CHILD(ntEntry) \
   (RELATIONSHIP_CHILD == ntEntry->relationship)
#define IS_UNAUTHENTICATED_CHILD(ntEntry) \
   (RELATIONSHIP_UNAUTHENTICATED_CHILD == ntEntry->relationship)
#define IS_END_DEVICE(ntEntry) \
   (DEVICE_TYPE_END_DEVICE == ntEntry->deviceType)
#define IS_CHILD(ntEntry) \
   (IS_AUTHENTICATED_CHILD(ntEntry) || IS_UNAUTHENTICATED_CHILD(ntEntry))
#define IS_PARENT(ntEntry) (RELATIONSHIP_PARENT == ntEntry->relationship)
#define IS_RX_ON_WHEN_IDLE(ntEntry) (ntEntry->rxOnWhenIdle)
#define FREE_NEIGHBOR(neighbor) neighbor->relationship = RELATIONSHIP_EMPTY
/** Getting mutable part of neighbor entry. */
#define MUTABLE_PART(neighbor) nwkMutablePart(nwkMemNeighborTable(), neighbor)
/** Minimum value of a cost metric. */
#define NWK_MIN_LINK_COST 1

/******************************************************************************
                 Inline static functions prototypes section
 ******************************************************************************/
/**************************************************************************//**
  \brief First entry of the neighbor table.
 ******************************************************************************/
INLINE NwkNeighbor_t* nwkNtBegin(void)
{
  return nwkMemNeighborTable()->table;
}

/**************************************************************************//**
  \brief Function returns a pointer to memory area after last entry of the
    neighbor table.
 ******************************************************************************/
INLINE NwkNeighbor_t* nwkNtEnd(void)
{
  return nwkMemNeighborTable()->end;
}

/**************************************************************************//**
  \brief Function return a total size of the neighbor table.
 ******************************************************************************/
INLINE NwkSizeOfNeighborTable_t nwkNtSize(void)
{
  return nwkMemNeighborTable()->size;
}

/**************************************************************************//**
  \brief Getting the mutable part of given neighbor entry.
 ******************************************************************************/
INLINE NwkMutablePartOfNeighbor_t* nwkMutablePart(
  NwkNeighborTable_t const *const neighborTable,
  NwkNeighbor_t const *const neighbor)
{
  return neighborTable->mutableTable + (neighbor - neighborTable->table);
}

/******************************************************************************
                              Prototypes section
 ******************************************************************************/
/**************************************************************************//**
  \brief Reset the neighbor table.

  \return None.
 ******************************************************************************/
NWK_PRIVATE void nwkResetNeighborTable(void);

/**************************************************************************//**
  \brief Clear all entries in the neighbor table.

  \param[in] removeParent - remove parent's entry.
  \return None.
 ******************************************************************************/
NWK_PRIVATE void nwkClearNeighborTable(const bool removeParent);

/**************************************************************************//**
  \brief Free a entry in the neighbor table.

  \param[in] neighbor - pointer to a neighbor.
  \return None.
 ******************************************************************************/
NWK_PRIVATE void nwkFreeNeighbor(NwkNeighbor_t *const neighbor);

/**************************************************************************//**
  \brief Update of time of a life of the neighbor.

  \param[in] neighbor - pointer to a neighbor.
  \return None.
 ******************************************************************************/
NWK_PRIVATE void nwkSetLifeTimeOfNeighbor(NwkNeighbor_t *const neighbor);

#if defined NWK_ROUTING_CAPACITY && defined _NWK_MESH_ROUTING_
/**************************************************************************//**
  \brief Checking of that the given device is destination node for the
    short address.

  \param[in] shortAddr - unicast or multicast short address.
  \param[in] group - if 'true' then shortAddress is a multicast address.
  \return 'true' if this node is destination else 'false'.
 ******************************************************************************/
NWK_PRIVATE bool nwkIsRouteDestination(const ShortAddr_t shortAddr,
  const bool group);
#endif /* NWK_ROUTING_CAPACITY and _NWK_MESH_ROUTING_ */

/**************************************************************************//**
  \brief Recalculation LQI and costs for the set neighbor.

  \param[in] neighbor - pointer to a neighbor
  \param[in] linkQuality - the link quality indicator of a received packet.
  \param[in] rssi - received signal strength indication.
  \return None.
 ******************************************************************************/
NWK_PRIVATE void nwkUpdateLqiAndCostOfNeighbor(NwkNeighbor_t *const neighbor,
  const Lqi_t linkQuality, const Rssi_t rssi);

/**************************************************************************//**
  \brief Initialize RSSI, LQI and cost of a neighbor.

  \param[in] neighbor - pointer to neighbor in the neighbor table.
  \param[in] linkQuality - the link quality indicator.
  \param[in] rssi - received signal strength indication.

  \return None.
 ******************************************************************************/
NWK_PRIVATE void nwkInitLqiAndCostOfNeighbor(NwkNeighbor_t *const neighbor,
  const Lqi_t linkQuality, const Rssi_t rssi);

/**************************************************************************//**
  \brief Recalculation LQI and costs for a neighbor with given short address.

  \param[in] addr - short address of a neighbor.
  \param[in] linkQuality - the link quality indicator of a received packet.
  \param[in] rssi - received signal strength indication.
  \return None.
 ******************************************************************************/
NWK_PRIVATE void nwkUpdateLqiAndLifeTime(const ShortAddr_t addr,
  const Lqi_t linkQuality, const Rssi_t rssi);

/**************************************************************************//**
  \brief Is there a free place in the neighbor table.

  \param endDeviceCount - current number of end device children.
  \param routerCount - current number of router children.

  \return 'true' if the neighbor table is full otherwise return 'false'.
 ******************************************************************************/
NWK_PRIVATE bool nwkIsNeighborTableFull(const NwkChildCount_t endDeviceCount,
  const NwkChildCount_t routerCount);

/**************************************************************************//**
  \brief Gets value for the end device capacity bit in the beacon payload.

  \param endDeviceCount - current number of end device children.

  \return 1U if a end device can rejoin to this device otherwise return 0U.
 ******************************************************************************/
NWK_PRIVATE uint8_t nwkGetEdCapacity(const NwkChildCount_t endDeviceCount);

/**************************************************************************//**
  \brief Gets value for the router capacity bit in the beacon payload.

  \param routerCount - current number of router children.

  \return 1U if a router can rejoin to this device otherwise return 0U.
 ******************************************************************************/
NWK_PRIVATE uint8_t nwkGetRouterCapacity(const NwkChildCount_t routerCount);

/**************************************************************************//**
  \brief Calculation of quantity of children and updating of capacity bits.
 ******************************************************************************/
NWK_PRIVATE void nwkUpdateCapacityBitsInBeacon(void);

/**************************************************************************//**
  \brief Updating capacity bits in beacon by end devices and routers count.

  \param[in] endDeviceCount - current number of end device children.
  \param[in] routerCount - current number of router children.
  \return None.
 ******************************************************************************/
NWK_PRIVATE void nwkUpdateCapacityBits(const NwkChildCount_t endDeviceCount,
  const NwkChildCount_t routerCount);

#if defined _RESOLVE_ADDR_CONFLICT_
/******************************************************************************
  \brief Find a neighbor with address conflict.

    This function reset the conflict flag.

  \return Pointer to neighbor's entry in the neighbor table.
******************************************************************************/
NWK_PRIVATE NwkNeighbor_t* nwkFindAddrConflictNeighbor(void);
#endif /* _RESOLVE_ADDR_CONFLICT_ */

#if defined _ROUTER_ || defined _COORDINATOR_
/******************************************************************************
  \brief Find a end device child with given short address.

  \param[in] shortAddr - short address of end device child.
  \return Pointer to neighbor's entry in the neighbor table.
******************************************************************************/
NWK_PRIVATE NwkNeighbor_t* nwkFindEndDeviceChild(const ShortAddr_t shortAddr);
#endif /* _ROUTER_ or _COORDINATOR_ */

#if defined _ROUTER_ || defined _COORDINATOR_
/**************************************************************************//**
  \brief Start the age timer of the neighbor table.
 ******************************************************************************/
NWK_PRIVATE void nwkStartAgeTimerOfNeighborTable(void);

/**************************************************************************//**
  \brief Stop the age timer of the neighbor table.
 ******************************************************************************/
NWK_PRIVATE void nwkStopAgeTimerOfNeighborTable(void);

/**************************************************************************//**
  \brief Search of the following child after given with a flag rxOnWhenIdle
    in 'false' and that can take indirect packet.

  \param[in] neighbor - the neighbor with which search should begin.
  \param[in] exceptShortAddr - the address of the child which should be passed.
  \return Pointer to sleeping child's entry in the neighbor table.
 ******************************************************************************/
NWK_PRIVATE NwkNeighbor_t* nwkNextSleepingChildForIndirectTx(
  NwkNeighbor_t* neighbor, const ShortAddr_t exceptShortAddr);

/**************************************************************************//**
  \brief Gets the current value of outgoing cost for the neighbor.

  \param[in] neighbor - pointer to a neighbor

  \return Maximum cost (7) if outgoing cost is unknown otherwise value from
          the last received link status from the neighbor.
 ******************************************************************************/
NWK_PRIVATE NwkLinkCost_t nwkGetOutgoingCost(const NwkNeighbor_t *const neighbor);

#else
#define nwkStartAgeTimerOfNeighborTable() (void)0
#define nwkStopAgeTimerOfNeighborTable() (void)0
#define nwkGetOutgoingCost(neighbor) 0U

#endif /* _ROUTER_ or _COORDINATOR_ */

#endif /* _NWK_NEIGHBOR_TABLE_H */
/** eof nwkNeighborTable.h */

