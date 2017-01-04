/**************************************************************************//**
  \file nwkRouteInfo.h

  \brief Public route information.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2010-05-07 Max Gekk - Created.
   Last change:
    $Id: nwkRouteInfo.h 27898 2015-06-02 09:50:16Z agasthian.s $
 ******************************************************************************/
#if !defined _NWK_ROUTE_INFO_H
#define _NWK_ROUTE_INFO_H

/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <appFramework.h>

/******************************************************************************
                              Definitions section
 ******************************************************************************/
/* Invalid concentrator descriptor. */
#define NWK_INVALID_CONCENTRATOR (UINT16_MAX)
/* Macro return true if concentrator descriptor is valid. */
#define IS_VALID_CONCENTRATOR(descr) (NWK_INVALID_CONCENTRATOR != (descr))

/******************************************************************************
                                Types section
 ******************************************************************************/
/* Type of concentrator descriptor. */
typedef uint16_t NWK_Concentrator_t;

/* Type of concentrator indication structure.
   Deprecated. Use generic subscription to event BC_EVENT_NEW_CONCENTRATOR instead. */
typedef struct _NWK_ConcentratorInd_t
{
  /** Service fields - for internal needs (event receiver for compatibility with new event system). */
  SYS_EventService_t service;
  /* This function is called when new concentrator is found.
     New argument 'id' was added as a transition to generic event system.
     Please update you software and ignore this argument for a moment. */
  void (* NWK_ConcentratorInd)(SYS_EventId_t id, SYS_EventData_t data);
} NWK_ConcentratorInd_t;

/** Type of the relay count field. Valid range 0-0xffff.
 * See Zigbee spec r18, Table 3.45. */
typedef uint16_t NWK_RelayCount_t;

#if defined (_NWK_ROUTE_CACHE_ ) || defined (_NWK_MANY_TO_ONE_ROUTING_)
/* Type of Source routing status. */
typedef enum _NWK_SourceRouting_Status_t
{
  NWK_SourceRouting_Success,
  NWK_SourceRouting_Failure,
  NWK_SourceRouting_OutOfMemory,
  NWK_SourceRouting_BufferFull,
  NWK_SourceRouting_DiscoveryTableFull,
  NWK_SourceRouting_RelayListTooLong,
} NWK_SourceRouting_Status_t;

/* Type of Source routing event. */
typedef enum _NWK_SourceRouting_EventType_t
{
  NWK_RouteRecordReceived,
  NWK_SourceRouteFailed,
  NWK_ManyToOneRouteFailed,
} NWK_SourceRouting_EventType_t;

/**//**
 * \brief Route record received event indicator.
 */
typedef struct PACK
{
  __packed ShortAddr_t srcAddr;
  uint8_t relayCount;
  ShortAddr_t relayList[NWK_MAX_SOURCE_ROUTE];
}NWK_RouteRecordInfo_t;

BEGIN_PACK
/**//**
 * \brief Source route failed event indicator.
 */
typedef struct PACK
{
  ShortAddr_t originalDestinationAddress;
  ShortAddr_t reporterAddress;
}NWK_SourceRouteFailureStatus_t;
END_PACK

typedef struct PACK
{
  NWK_SourceRouting_EventType_t type;
  union
  {
    __packed ShortAddr_t reporterAddr;
    NWK_RouteRecordInfo_t routeRecordData;
    NWK_SourceRouteFailureStatus_t sourceRouteFailureStatus;
  } sourceRouteEventData;
}NWK_SourceRouteEvent_t;
#endif /* _NWK_ROUTE_CACHE_ || _NWK_MANY_TO_ONE_ROUTING_ */

/******************************************************************************
                              Prototypes section
 ******************************************************************************/
/**************************************************************************//**
  \brief This function return the 16-bit network address of the next hop on
     the way to the destination.

  \return The network address of the next hop.
 ******************************************************************************/
ShortAddr_t NWK_GetNextHop(ShortAddr_t dstAddr);

/**************************************************************************//**
  \brief Get descriptor of next concentrator.

  Example:
    NWK_Concentrator_t c = NWK_INVALID_CONCENTRATOR;

    while (IS_VALID_CONCENTRATOR(c = NWK_NextConcentrator(c)))
    {
      cAddr = NWK_ShortAddrOfConcentrator(c);
      if (MASTER_CONCENTRATOR_ADDR == cAddr)
        ...
      ...
    }

  \param[in] prevConcentrator - descriptor of previous concentrator or
    NWK_INVALID_CONCENTRATOR if it's initial call.

  \return if next concentrator is found then return valid descriptor otherwise
    return NWK_INVALID_CONCENTRATOR.
 ******************************************************************************/
NWK_Concentrator_t NWK_NextConcentrator(const NWK_Concentrator_t prevConcentrator);

/**************************************************************************//**
  \brief Get short address of given concentrator.

    Note: Call this function and NWK_NextConcentrator() in same task.

  \param[in] concentrator - descriptor of concentrator (see NWK_NextConcentrator).
  \return Short address of concentrator.
 ******************************************************************************/
ShortAddr_t NWK_ShortAddrOfConcentrator(const NWK_Concentrator_t concentrator);

/**************************************************************************//**
  \brief Get short address of next hop node for given concentrator.

    Note: Call this function and NWK_NextConcentrator() in same task.

  \param[in] concentrator - descriptor of concentrator (see NWK_NextConcentrator).
  \return Short address of next hop.
 ******************************************************************************/
ShortAddr_t NWK_NextHopToConcentrator(const NWK_Concentrator_t concentrator);

/**************************************************************************//**
  \brief Subscribe to indication about new concentrators.

  Deprecated. Subscribe to generic BC_EVENT_NEW_CONCENTRATOR instead.

  \param[in] ind - pointer to concentrator indication structure.
  \return None.
 ******************************************************************************/
/*DEPRECATED*/ void NWK_SubscribeToConcentratorInfo(NWK_ConcentratorInd_t *const ind);

/**************************************************************************//**
  \brief Unsubscribe from indication about new concentrators.

  Deprecated. Unsubscribe from generic BC_EVENT_NEW_CONCENTRATOR instead.

  \param[in] ind - pointer to concentrator indication structure.
  \return None.
 ******************************************************************************/
/*DEPRECATED*/ void NWK_UnsubscribeFromConcentratorInfo(NWK_ConcentratorInd_t *const ind);

#if defined _NWK_ROUTE_CACHE_
/**************************************************************************//**
  \brief Clear the relay list for the given destination address.

   Relay list to the destination device from the routeCache is cleared.

  \param[in] dstAddr - The dst address for which this relayList should be cleared.
  \return None.
 ******************************************************************************/
void NWK_ClearRelayList(const ShortAddr_t dstAddr);

/**************************************************************************//**
  \brief Set the relay list for the given destination address.

   Relay list to use if a message is sent to the destination address.

  \param[in] dstAddr - The dst address for which this relayList should be cleared.
  \param[in] relayCount - The dst address for which this relayList should be cleared.
  \param[in] path - The list of network addresses by which the packet is relayed through the network.
  \return Status code indicating the status of the operation.
 ******************************************************************************/
NWK_SourceRouting_Status_t NWK_SetRelayList(const ShortAddr_t dstAddr, const NWK_RelayCount_t relayCount, ShortAddr_t *path);

#endif /* _NWK_ROUTE_CACHE_ */

#if defined _CUSTOM_4_SECURITY_MODE_
/**************************************************************************//**
  \brief Add a known concentrator information.

  \param[in] shortAddr - short address of the concentrator.
  \param[in] nextHopAddr - next hop address on a rote to the concentrator.
  \param[in] noRouteCache - noRouteCahce flag of the concentrator.
  \return None.
 ******************************************************************************/
void NWK_AddConcentrator(const ShortAddr_t shortAddr, const ShortAddr_t nextHopAddr,
  const bool noRouteCache);
#endif /* _CUSTOM_4_SECURITY_MODE_ */

#endif /* _NWK_ROUTE_INFO_H */
/** eof nwkRouteInfo.h */

