/**************************************************************************//**
  \file N_SourceRouting.c

  \brief
    N_SourceRouting implementation.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    06.04.15 Sankar Mohan - Created
******************************************************************************/

/******************************************************************************
                    Includes section
******************************************************************************/
#include <N_SourceRouting.h>
#include <nlmeRouteDiscovery.h>
#include <nwkRouteInfo.h>
#include "N_ErrH.h"

#if defined(_NWK_MANY_TO_ONE_ROUTING_) || defined(_NWK_ROUTE_CACHE_)
/******************************************************************************
                    Defines section
******************************************************************************/
#define COMPID "N_SourceRouting"

#ifndef N_SOURCEROUTING_MAX_SUBSCRIBERS
  #define N_SOURCEROUTING_MAX_SUBSCRIBERS 3u
#endif

/******************************************************************************
                    Types section
******************************************************************************/
typedef struct _N_ManyToOne_RouteReq_t
{
  NWK_RouteDiscoveryReq_t rdiscReq;
  bool busy;
} N_ManyToOne_RouteReq_t;

/******************************************************************************
                    Prototypes section
******************************************************************************/
#if defined _NWK_ROUTE_CACHE_
static NWK_RouteDiscoveryReq_t *getFreeManyToOneRouteReq(void);
static void freeManyToOneRouteReq(NWK_RouteDiscoveryConf_t *confirm);
static void nSendManyToOneRouteReqConf(NWK_RouteDiscoveryConf_t *conf);
#endif /* _NWK_ROUTE_CACHE_ */
static void nSourceRouteObserver(SYS_EventId_t eventId, SYS_EventData_t data);
/******************************************************************************
                    Static section
******************************************************************************/
static N_ManyToOne_RouteReq_t manyToOneRouteReq[N_MANYTOONEROUTE_REQUESTS_POOL_SIZE];
/* BitCloud events receiver */
static SYS_EventReceiver_t nSourceRouteEventReceiver = {.func = nSourceRouteObserver};
N_UTIL_CALLBACK_DECLARE(N_SourceRouting_Callback_t, s_N_SourceRouting_Subscribers, N_SOURCEROUTING_MAX_SUBSCRIBERS);

/******************************************************************************
                    Implementation section
******************************************************************************/
/** Initialises the component
*/
void N_SourceRouting_Init(void)
{
  memset(manyToOneRouteReq, 0x00, sizeof(manyToOneRouteReq));
  SYS_SubscribeToEvent(BC_EVENT_SOURCE_ROUTING_STATUS_INDICATION, &nSourceRouteEventReceiver);
}

/** Subscribe for callbacks from this component.
    \param pCallback Pointer to filled callback structure
*/
void N_SourceRouting_Subscribe_Impl(const N_SourceRouting_Callback_t* pCallback)
{
  N_UTIL_CALLBACK_SUBSCRIBE(N_SourceRouting_Callback_t, s_N_SourceRouting_Subscribers, pCallback);
}

#if defined _NWK_ROUTE_CACHE_
/** Send a many to one route request, see ZigBee 2007 Specification 3.2.2.31.
    \param noRouteCache TRUE if no route record table should be established. Otherwise FALSE.
    \returns N_SourceRouting_Status_Success if the call succeeded.
*/
N_SourceRouting_Status_t N_SourceRouting_SendManyToOneRouteRequest_Impl(bool noRouteCache)
{
  NWK_RouteDiscoveryReq_t *routeReq = getFreeManyToOneRouteReq();
  
  if (NULL == routeReq)
    return N_SourceRouting_Status_Failure;

  routeReq->dstAddrMode = NWK_DSTADDRMODE_NOADDR;
  routeReq->radius = NWK_MAX_SOURCE_ROUTE;
  routeReq->noRouteCache = noRouteCache;
  routeReq->NWK_RouteDiscoveryConf = nSendManyToOneRouteReqConf;
  NWK_RouteDiscoveryReq(routeReq);
  return N_SourceRouting_Status_Success;
}

/** Many to One route request confirmation callback function.
    \param[in] conf - pointer to NLME-ROUTE-DISCOVERY confirm primitive's parameters structure.
    \return None.
*/
static void nSendManyToOneRouteReqConf(NWK_RouteDiscoveryConf_t *conf)
{
  freeManyToOneRouteReq(conf);
}

/** Set a relay list to use if a message is sent to the destination address.
    \param destinationAddress The destination address for which this relayList is used.
    \param relayListLength The length of the relayList.
    \param relayList A list of network addresses by which a unicast command to the destinationAddress is relayed through the network.
    \returns N_SourceRouting_Status_Success if the call succeeded.
*/
N_SourceRouting_Status_t N_SourceRouting_SetRelayList_Impl(uint16_t destinationAddress, uint8_t relayListLength, uint16_t relayList[])
{
  if(relayListLength == 0U)
    return N_SourceRouting_Status_Failure;

  return NWK_SetRelayList(destinationAddress, relayListLength, relayList);
}

/** Clear a relay list for this destination address.
    \param destinationAddress The destination address for which this relayList should be cleared.
*/
void N_SourceRouting_ClearRelayList_Impl(uint16_t destinationAddress)
{
  NWK_ClearRelayList(destinationAddress);
}

/** Gather free buffer for Many To One Route request

    \return Pointer to free manyToOneRouteReq request structure, or NULL if no one exists.
*/
static NWK_RouteDiscoveryReq_t *getFreeManyToOneRouteReq(void)
{
  for (uint8_t item = 0; item < N_MANYTOONEROUTE_REQUESTS_POOL_SIZE; item++)
  {
    if (!manyToOneRouteReq[item].busy)
    {
      manyToOneRouteReq[item].busy = true;
      return &manyToOneRouteReq[item].rdiscReq;
    }
  }
  return NULL;
}

/** Release occupied manyToOneRouteReq request buffer
    \param resp manyToOneRouteReq response pointer
*/
static void freeManyToOneRouteReq(NWK_RouteDiscoveryConf_t *confirm)
{
  N_ManyToOne_RouteReq_t *nMtorReq = GET_PARENT_BY_FIELD(N_ManyToOne_RouteReq_t, rdiscReq.confirm, confirm);

  N_ERRH_ASSERT_FATAL(nMtorReq->busy == true);

  nMtorReq->rdiscReq.NWK_RouteDiscoveryConf = NULL;
  nMtorReq->busy = false;
}
#endif /* _NWK_ROUTE_CACHE_ */

/**************************************************************************//**
  \brief Processes Source route event

  \param[in] eventId - id of raised event;
  \param[in] data    - event's data.
 ******************************************************************************/
static void nSourceRouteObserver(SYS_EventId_t eventId, SYS_EventData_t data)
{
  NWK_SourceRouteEvent_t *const sourceRouteData = (NWK_SourceRouteEvent_t *)data;
  if(sourceRouteData->type == NWK_ManyToOneRouteFailed)
  {
    N_UTIL_CALLBACK(N_SourceRouting_Callback_t, s_N_SourceRouting_Subscribers, ManyToOneRouteFailed,
      (sourceRouteData->sourceRouteEventData.reporterAddr));
  }
#if defined _NWK_ROUTE_CACHE_
  else if (sourceRouteData->type == NWK_RouteRecordReceived)
  {
    N_UTIL_CALLBACK(N_SourceRouting_Callback_t, s_N_SourceRouting_Subscribers, ReceivedRouteRecord,
      (sourceRouteData->sourceRouteEventData.routeRecordData.srcAddr, 
       sourceRouteData->sourceRouteEventData.routeRecordData.relayCount,
       sourceRouteData->sourceRouteEventData.routeRecordData.relayList));
  }
  else if(sourceRouteData->type == NWK_SourceRouteFailed)
  {
    N_UTIL_CALLBACK(N_SourceRouting_Callback_t, s_N_SourceRouting_Subscribers, SourceRouteFailed,
      (sourceRouteData->sourceRouteEventData.sourceRouteFailureStatus.originalDestinationAddress,
         sourceRouteData->sourceRouteEventData.sourceRouteFailureStatus.reporterAddress));
  }
#endif /* _NWK_ROUTE_CACHE_ */
}

#endif /* _NWK_MANY_TO_ONE_ROUTING_ || _NWK_ROUTE_CACHE_ */
