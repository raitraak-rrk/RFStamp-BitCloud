/**************************************************************************//**
 \file nwkNeighbor.h

 \brief Neighbor table interface.

 \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2009-04-14 Max Gekk - Created.
   Last change:
    $Id: nwkNeighbor.h 27751 2015-04-07 11:02:46Z Mahendran.P $
 ******************************************************************************/
/**//**
 *  The neighbor table of a device shall contain information on every device
 * within transmission range. ZigBee Spec r17, 3.6.1.5, page 366.
 **/
#if !defined _NWK_NEIGHBOR_H
#define _NWK_NEIGHBOR_H

/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <appFramework.h>
#include <appTimer.h>
#include <nwkCommon.h>

/******************************************************************************
                                 Types section
 ******************************************************************************/
/** The relationship between the neighbor and the current device */
typedef enum _Relationship_t
{
  RELATIONSHIP_PARENT                = 0x00, /**< neighbor is the parent */
  RELATIONSHIP_CHILD                 = 0x01, /**< neighbor is a child */
  RELATIONSHIP_SIBLING               = 0x02, /**< neighbor is a sibling */
  RELATIONSHIP_NONE_OF_ABOVE         = 0x03, /**< none of the above */
  RELATIONSHIP_PREVIOUS_CHILD        = 0x04, /**< previous child */
  RELATIONSHIP_UNAUTHENTICATED_CHILD = 0x05, /**< unauthenticated child */
  RELATIONSHIP_EMPTY
} Relationship_t;

/** The neighbor table item. */
typedef struct _NwkNeighbor_t
{
  Relationship_t relationship;
  /** The type of neighbor device. */
  DeviceType_t deviceType;
  /** IEEE 802.15.4-2006 7.3.1.2 Capability Information field. */
  MAC_CapabilityInf_t capability;
  /** The logical channel on which the network is operating. */
  Channel_t logicalChannel;
  /** The 16-bit network address of the neighboring device. */
  ShortAddr_t networkAddr;
  PanId_t panId;
  /** 64-bit IEEE address that is unique to every device. */
  ExtAddr_t extAddr;
  /** The 64-bit unique identifier of the network to which the device belongs.*/
  ExtPanId_t extPanId;
  /** The tree depth of the neighbor device. */
  NwkDepth_t depth;
  /** The value identifying a snapshot of the network settings with which this
   * node is operating with.*/
  NwkUpdateId_t updateId;
  /** Indicates if neighbor's receiver is enabled during idle periods. */
  bool rxOnWhenIdle       :1;
  /** An indication of whether the device is accepting joining requests. */
  bool permitJoining      :1;
  /** An indication of whether the device has been
   * ruled out as a potential parent. */
  bool potentialParent    :1;
  /** Network address of the neighbor is conflict with other address in network.
   **/
  bool isAddressConflict  :1;
  /** Upper layer knowns about this child (true) or not (false).*/
  bool isKnownChild       :1;
  unsigned reserved1      :3;
} NwkNeighbor_t;

/** Type of life time of neighbors in ticks. */
typedef uint16_t NwkLifeTime_t;
/** The bit map of passive acks, each bit is matched to a broadcast that
 * waits passive acknowledgements. */
typedef uint8_t NwkPassiveAckMap_t;

/** Mutable fields of a entry in the neighbor table. */
typedef struct _NwkMutablePartOfNeighbor_t
{
  /** The time of life of a neighbor entry. */
  NwkLifeTime_t lifeTime;
  /** The estimated link quality for RF transmissions from this device. */
  Lqi_t lqi;
  Rssi_t rssi;
  /** The cost of an outgoing link as measured by the neighbor. */
  unsigned outgoingCost  :3;
  /** The cost of an incoming link as measured by this device. */
  unsigned incomingCost  :3;
  unsigned reserved1     :2;
#if defined _NWK_PASSIVE_ACK_
  NwkPassiveAckMap_t passiveAckMap;
#endif /* _NWK_PASSIVE_ACK_ */
} NwkMutablePartOfNeighbor_t;

/** Type of size of the neighbor table. */
typedef uint8_t NwkSizeOfNeighborTable_t;

/** Type of the neighbor table. */
typedef struct _NwkNeighborTable_t
{
 /** The array of a neighbor entry. */
  NwkNeighbor_t *table;
  /** Pointer to memory area after last entry of the neighbor table. */
  NwkNeighbor_t *end;
  /** The array of a mutable neighbor entry. */
  NwkMutablePartOfNeighbor_t *mutableTable;
  /** Current size of the neighbor table. */
  NwkSizeOfNeighborTable_t size;
#if defined _ROUTER_ || defined _COORDINATOR_
  /** Timer for internal use. */
  HAL_AppTimer_t ageTimer;
#endif /* _ROUTER_ or _COORDINATOR_ */
} NwkNeighborTable_t;

/** For backward compatibility */
typedef NwkNeighbor_t Neib_t;
typedef NwkMutablePartOfNeighbor_t MutableNeib_t;
typedef NwkNeighborTable_t NeibTable_t;

/******************************************************************************
                               Prototypes section
 ******************************************************************************/
/**************************************************************************//**
  \brief Access function to the neighbor table.

  \return Pointer to the neighbor table.
 ******************************************************************************/
NwkNeighborTable_t* NWK_GetNeighborTable(void);

/**************************************************************************//**
  \brief Searching a neighbor entry by extended address.

  \param[in] extAddr - extended IEEE address of neighbor.
  \return NULL if no records found, or entry with extAddr otherwise
 ******************************************************************************/
NwkNeighbor_t* NWK_FindNeighborByExtAddr(const ExtAddr_t extAddr);

/**************************************************************************//**
  \brief Searching a neighbor entry by short address.

  \param[in] shortAddr - network address of neighbor.
  \return NULL if no records found, or entry with shortAddr otherwise
 ******************************************************************************/
NwkNeighbor_t* NWK_FindNeighborByShortAddr(const ShortAddr_t shortAddr);

/**************************************************************************//**
  \brief Remove a neighbor from the neighbor table with leave indication.

  \param[in] neighbor - pointer to a entry in the neighbor table.
  \param[in] cleanAddressMap - Remove records from address map if existed.
  \return None.
 ******************************************************************************/
void NWK_RemoveNeighbor(NwkNeighbor_t *const neighbor, bool cleanAddressMap);

/**************************************************************************//**
  \brief Link quality indication for given neighbor.

  \param[in] neighbor - pointer to a entry of neighbor in the neighbor table.
  \return LQI of received frames from a neighbor
 ******************************************************************************/
Lqi_t NWK_GetNeighborsLqi(NwkNeighbor_t const *const neighbor);

/**************************************************************************//**
  \brief RSSI for given neighbor.

  \param[in] neighbor - pointer to a entry of neighbor in the neighbor table.
  \return RSSI of received frames from a neighbor
 ******************************************************************************/
Rssi_t NWK_GetNeighborsRssi(NwkNeighbor_t const *const neighbor);

/**************************************************************************//**
  \brief Is given neighbor known child.

  \param[in] extAddr - extended address of child.
  \param[in] setKnownFlag - change the known flag to 'true' if this parameter
                              is equal 'true'.

  \return Current value of known flag.
 ******************************************************************************/
bool NWK_IsKnownChild(const ExtAddr_t extAddr, const bool setKnownFlag);

/**************************************************************************//**
  \brief Is given neighbor is child.

  \param[in] neighbor - pointer to a entry of neighbor in the neighbor table.

  \return True, given neighbor is child.; otherwise - false.
 ******************************************************************************/
INLINE bool NWK_IsChild(Neib_t const *const neighbor)
{
  return (RELATIONSHIP_CHILD == neighbor->relationship)
      || (RELATIONSHIP_UNAUTHENTICATED_CHILD == neighbor->relationship);
}

/**************************************************************************//**
  \brief Is given neighbor is unauthenticated child.

  \param[in] neighbor - pointer to a entry of neighbor in the neighbor table.

  \return True, given neighbor is unauthenticated child.; otherwise - false.
 ******************************************************************************/
INLINE bool NWK_IsUnauthenticatedChild(Neib_t const *const neighbor)
{
  return (RELATIONSHIP_UNAUTHENTICATED_CHILD == neighbor->relationship);
}

/**************************************************************************//**
  \brief Authenticate a child node.

  \param[in] extAddr - pointer to the extended address of child.
  \return None.
 ******************************************************************************/
bool NWK_AuthenticateNeighbor(const ExtAddr_t *const extAddr);

/*****************************************************************************
  \brief This function adds neighbor table record for a known neighbor.

  \param[in] nwkAddress - neighbor network address;
  \param[in] extAddr    - pointer to extended address of a node.
  \param[in] replaceEntry - replace existing entry with relationship-noneOfabove 
                                        if neighbor table is full  
  \return pointer to neighbor.
 *****************************************************************************/
NwkNeighbor_t *NWK_AddKnownNeighbor(const ShortAddr_t nwkAddress, ExtAddr_t *extAddr, bool replaceEntry);

/**************************************************************************//**
  \brief Linear search of an EndDevice child.

  \param[in] exceptShortAddr - to ignore entries with this address.
  \param[in] neighbor - begin search after this neighbor.
  \return Pointer to EndDevice child in the neighbor table.
 ******************************************************************************/
NwkNeighbor_t* NWK_NextEDChild(NwkNeighbor_t* neighbor,
  const ShortAddr_t exceptShortAddr);

/**************************************************************************//**
  \brief Calculate an incoming cost by LQI.

  \param[in] lqi - link quality indicator.
  \return None.
 ******************************************************************************/
NwkPathCost_t NWK_CostFromLqi(const Lqi_t lqi);

/******************************************************************************
  \brief Updating the outgoing cost and age field of the neighbor.

  \param[in] neighbor - pointer to a neighbor in the neighbor table.
  \param[in] outgoingCost - new outgoing cost.

  \return None.
 ******************************************************************************/
void nwkUpdateOutgoingCostAndLifeTime(NwkNeighbor_t *const neighbor,
  const NwkPathCost_t outgoingCost);

#if defined(_ROUTER_) || defined(_COORDINATOR_)
#if defined(_NWK_KEEP_STALE_NEIGHBOR_ENTRY_)
/******************************************************************************
\brief Check whether the neighbor is stale

\param[in] neighbor - neighbor that needs to be checked

\return TRUE if neighbor is stale, FALSE otherwise
******************************************************************************/
bool NWK_IsNeighborStale(NwkNeighbor_t* const neighbor);

/******************************************************************************
\brief Find the stale entry in neighbor table

\return Stale entry (NwkNeighbor_t*) if available, NULL otherwise
******************************************************************************/
NwkNeighbor_t* NWK_RemoveStaleNeighbor(void);

#endif //#if defined(_NWK_KEEP_STALE_NEIGHBOR_ENTRY_)
#endif //#if defined(_ROUTER_) || defined(_COORDINATOR_)

#endif /* _NWK_NEIGHBOR_H */
/** eof nwkNeighbor.h */

