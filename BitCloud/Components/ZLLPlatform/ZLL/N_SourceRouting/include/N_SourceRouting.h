/*********************************************************************************************//**
\file  

\brief 

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal

$Id:

***************************************************************************************************/

#ifndef N_SOURCEROUTING_H
#define N_SOURCEROUTING_H

/***************************************************************************************************
* INCLUDE FILES
***************************************************************************************************/

#include "N_Types.h"
#if defined(_NWK_MANY_TO_ONE_ROUTING_) || defined(_NWK_ROUTE_CACHE_)
/***************************************************************************************************
* C++ DECLARATION WRAPPER
***************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************************************
* EXPORTED TYPES
***************************************************************************************************/

/* The maximum possible ammount of concurrent many to one route request requests. */
#ifndef N_MANYTOONEROUTE_REQUESTS_POOL_SIZE
  #define N_MANYTOONEROUTE_REQUESTS_POOL_SIZE 3u
#endif /* N_MANYTOONEROUTE_REQUESTS_POOL_SIZE */

typedef enum N_SourceRouting_Status_t
{
    N_SourceRouting_Status_Success,
    N_SourceRouting_Status_Failure,
    N_SourceRouting_Status_OutOfMemory,
    N_SourceRouting_Status_BufferFull,
    N_SourceRouting_Status_DiscoveryTableFull,
    N_SourceRouting_Status_RelayListTooLong,
} N_SourceRouting_Status_t;

typedef struct N_SourceRouting_Callback_t
{
#if defined _NWK_ROUTE_CACHE_
    /** Received a RouteRecord command.
        \param sourceAddress The network address from which the RouteRecord is received.
        \param relayListLength The length of the relayList.
        \param relayList A list of network addresses by which a unicast command to sourceAddress can be relayed.
    */
    void (*ReceivedRouteRecord)(uint16_t sourceAddress, uint8_t relayListLength, uint16_t relayList[]);

    /** Received a Network Status command with error code 'source route failure' (0x0B).
        \param originalDestinationAddress The destination address of the message which caused the failure.
        \param reporterAddress The network address from the device that reported the failure.
    */
    void (*SourceRouteFailed)(uint16_t originalDestinationAddress, uint16_t reporterAddress);
#endif /* _NWK_ROUTE_CACHE_ */

    /** Received a Network Status command with error code 'many to one route error' (0x0C).
        \param reporterAddress The network address from the device that reported the failure.
    */
    void (*ManyToOneRouteFailed)(uint16_t reporterAddress);

    /** Guard to ensure the initialiser contains all functions. */
    int8_t endOfList;

} N_SourceRouting_Callback_t;

/***************************************************************************************************
* EXPORTED MACROS AND CONSTANTS
***************************************************************************************************/


/***************************************************************************************************
* EXPORTED FUNCTIONS
***************************************************************************************************/

/** Initialise the event.
*/
void N_SourceRouting_Init(void);

/** Subscribe for callbacks from this component.
    \param pCallback Pointer to filled callback structure
*/
void N_SourceRouting_Subscribe(const N_SourceRouting_Callback_t* pCallback);

#if defined _NWK_ROUTE_CACHE_
/** Send a many to one route request, see ZigBee 2007 Specification 3.2.2.31.
    \param noRouteCache TRUE if no route record table should be established. Otherwise FALSE.
    \returns N_SourceRouting_Status_Success if the call succeeded.
*/
N_SourceRouting_Status_t N_SourceRouting_SendManyToOneRouteRequest(bool noRouteCache);

/** Set a relay list to use if a message is sent to the destination address.
    \param destinationAddress The destination address for which this relayList is used.
    \param relayListLength The length of the relayList.
    \param relayList A list of network addresses by which a unicast command to the destinationAddress is relayed through the network.
    \returns N_SourceRouting_Status_Success if the call succeeded.
*/
N_SourceRouting_Status_t N_SourceRouting_SetRelayList(uint16_t destinationAddress, uint8_t relayListLength, uint16_t relayList[]);

/** Clear a relay list for this destination address.
    \param destinationAddress The destination address for which this relayList should be cleared.
*/
void N_SourceRouting_ClearRelayList(uint16_t destinationAddress);
#endif /* _NWK_ROUTE_CACHE_ */

/***************************************************************************************************
* END OF C++ DECLARATION WRAPPER
***************************************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _NWK_MANY_TO_ONE_ROUTING_ || _NWK_ROUTE_CACHE_ */
#endif // N_SOURCEROUTING_H
