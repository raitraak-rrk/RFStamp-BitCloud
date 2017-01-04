/**************************************************************************//**
  \file N_PacketDistributor.c

  \brief
    N_PacketDistributor implementation.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    2808.2012 N.Fomin - Created
******************************************************************************/
/******************************************************************************
                    Includes section
******************************************************************************/
#include <N_PacketDistributor.h>
#include <N_ErrH.h>
#include <sysUtils.h>
#include <N_Zcl_Framework.h>
#include <N_DeviceInfo.h>

/******************************************************************************
                    Definitions section
******************************************************************************/
#define COMPID "N_PacketDistributor"

#ifndef MAX_ZCL_ENDPOINTS
  #define MAX_ZCL_ENDPOINTS  10u
#endif

#ifndef N_PACKET_DISTRIBUTER_MAX_SUBSCRIBERS
  #define N_PACKET_DISTRIBUTER_MAX_SUBSCRIBERS    (MAX_ZCL_ENDPOINTS + 2u)  // One extra for ZDP, one extra for InterPan
#endif

/******************************************************************************
                    Types section
******************************************************************************/

typedef struct N_PacketDistributor_Subscriber_t
{
    uint8_t endpoint;
    const N_Zcl_DataInd_Callback_t* pCallback;
} N_PacketDistributor_Subscriber_t;

typedef struct N_PacketDistributor_Registration_t
{
    uint8_t endpoint;
    uint8_t groupCount;
} N_PacketDistributor_Registration_t;

/******************************************************************************
                    Prototypes section
******************************************************************************/
static void N_PacketDistributor_DataInd(APS_DataInd_t *dataInd);
static void nHacObserver(SYS_EventId_t eventId, SYS_EventData_t data);
static void nHacRegisterEndpoint(ZLL_Endpoint_t* zllEp);

/******************************************************************************
                    Static variables section
******************************************************************************/
static N_PacketDistributor_Subscriber_t s_N_PacketDistributor_Subscribers[N_PACKET_DISTRIBUTER_MAX_SUBSCRIBERS] = { {0u, NULL} };
static ZLL_Endpoint_t* zllEndpoints[MAX_ZCL_ENDPOINTS];
/* BitCloud events receiver */
static SYS_EventReceiver_t bcEventReceiver = { .func = nHacObserver};

static uint8_t freeEndpointCell = 0;

/******************************************************************************
                    Implementation section
******************************************************************************/
/** Register a ZigBee end-point.
    \param ZLL_Endpoint_t Pointer to a filled structure describing
        the end-point. The memory must stay available after the call to
        this function
    \param groupCount The number of groups used by the the endpoint (see the note below)

    \note The groups count should include number of the groups that the endpoint uses as client of
        the ZCL groups cluster, NOT the number of groups used as server of the cluster: e.g. a
        remote control endpoint uses one group (groupCount == 1) to control some dimmable
        light endpoints (groupCount == 0).
*/
void N_PacketDistributor_RegisterEndpoint_Impl(ZLL_Endpoint_t* zllEp, uint8_t groupCount)
{
  zllEp->groupsAmount = groupCount;

  zllEp->service.registerEndpointReq.APS_DataInd = N_PacketDistributor_DataInd;
  zllEp->service.registerEndpointReq.simpleDescriptor = zllEp->simpleDescriptor;

  APS_RegisterEndpointReq(&zllEp->service.registerEndpointReq);
  
  if((PROFILE_ID_HOME_AUTOMATION == zllEp->simpleDescriptor->AppProfileId) || (PROFILE_ID_LIGHT_LINK == zllEp->simpleDescriptor->AppProfileId))
  {
    nHacRegisterEndpoint(zllEp);
    
    /* Subscribe to frames with unexpected ProfileId indication events */
    SYS_SubscribeToEvent(BC_EVENT_ACCESS_REQUEST, &bcEventReceiver);
  }
}

/** Returns the number of Zcl EndPoints (so excluding ZDO and InterPan endpoints)
*/
uint8_t N_PacketDistributor_GetNumberOfZclEndpoints_Impl(void)
{
  APS_RegisterEndpointReq_t *epReq = NULL;
  uint8_t epAmount = 0;

  while(epReq = APS_NextEndpoint(epReq))
    epAmount++;

  return epAmount;
}

/** Returns the zcl endpoint number for an index (0..NumberOfZclEndpoints-1)
*/
uint8_t N_PacketDistributor_GetZclEndpointFromIndex_Impl(uint8_t index)
{
  APS_RegisterEndpointReq_t *epReq = NULL;
  uint8_t epAmount = 0;


  while((epReq = APS_NextEndpoint(epReq)) && (epAmount < index))
    epAmount++;

  if (!epReq)
  {
    N_ERRH_FATAL();
    return 0;
  }
  return epReq->simpleDescriptor->endpoint;
}

/** Get the group count registered with \ref N_PacketDistributor_RegisterEndpoint.
    \param endpoint The endpoint number for which to get the group count
    \returns The group count
*/
uint8_t N_PacketDistributor_GetGroupCount_Impl(uint8_t endpoint)
{
  APS_RegisterEndpointReq_t *epReq = NULL;
  ZLL_Endpoint_t *zllEp;

  while(epReq = APS_NextEndpoint(epReq))
    if (epReq->simpleDescriptor->endpoint == endpoint)
      break;

  if (!epReq)
  {
    N_ERRH_FATAL();
    return 0;
  }
  zllEp = GET_PARENT_BY_FIELD(ZLL_Endpoint_t, service.registerEndpointReq, epReq);
  return zllEp->groupsAmount;
}

/** Get the sum of the group counts registered with \ref N_PacketDistributor_RegisterEndpoint for all endpoints.
    \returns The total number of groups
*/
uint8_t N_PacketDistributor_GetTotalGroupCount_Impl(void)
{
  APS_RegisterEndpointReq_t *epReq = NULL;
  ZLL_Endpoint_t *zllEp;
  uint8_t totalCount = 0;

  while(epReq = APS_NextEndpoint(epReq))
  {
    zllEp = GET_PARENT_BY_FIELD(ZLL_Endpoint_t, service.registerEndpointReq, epReq);
    totalCount += zllEp->groupsAmount;
  }
  return totalCount;
}

/** Find a registered endpoint.
    \param endpoint Endpoint number
    \returns A pointer to the endpoint description or NULL if the endpoint could not be found.
             Do not free the memory
*/
ZLL_Endpoint_t* N_PacketDistributor_FindEndpoint_Impl(uint8_t endpoint)
{
  APS_RegisterEndpointReq_t *epReq = NULL;

  while(epReq = APS_NextEndpoint(epReq))
    if (epReq->simpleDescriptor->endpoint == endpoint)
      break;

  if (!epReq)
    return NULL;
  return GET_PARENT_BY_FIELD(ZLL_Endpoint_t, service.registerEndpointReq, epReq);
}

/** Returns the number of Zll EndPoints
*/
uint8_t N_PacketDistributor_GetNumberOfZllEndpoints_Impl(void)
{
  APS_RegisterEndpointReq_t *epReq = NULL;
  uint8_t epAmount = 0;

  while(epReq = APS_NextEndpoint(epReq))
  {
    if ((N_DeviceInfo_Profile_ZLL == epReq->simpleDescriptor->AppProfileId) ||
        ((N_DeviceInfo_Profile_ZHA == epReq->simpleDescriptor->AppProfileId)&&
          (epReq->simpleDescriptor->AppDeviceVersion >= 1)))
      epAmount++;
  }

  return epAmount;
}

/** Get the sum of the group counts for zll endpoints.
    \returns The total number of groups
*/
uint8_t N_PacketDistributor_GetTotalGroupCountZll_Impl(void)
{
  APS_RegisterEndpointReq_t *epReq = NULL;
  ZLL_Endpoint_t *zllEp;
  uint8_t totalCount = 0;

  while(epReq = APS_NextEndpoint(epReq))
  {
    if ((N_DeviceInfo_Profile_ZLL == epReq->simpleDescriptor->AppProfileId) ||
        ((N_DeviceInfo_Profile_ZHA == epReq->simpleDescriptor->AppProfileId) &&
          (epReq->simpleDescriptor->AppDeviceVersion >= 1)))
    {
      zllEp = GET_PARENT_BY_FIELD(ZLL_Endpoint_t, service.registerEndpointReq, epReq);
      totalCount += zllEp->groupsAmount;
    }
  }
  return totalCount;
}

/** Returns the zll endpoint number for an index (0..NumberOfZllEndpoints-1)
*/
uint8_t N_PacketDistributor_GetZllEndpointFromIndex_Impl(uint8_t index)
{
  APS_RegisterEndpointReq_t *epReq = NULL;
  uint8_t epAmount = 0;

  while(epReq = APS_NextEndpoint(epReq))
  {
    if(((N_DeviceInfo_Profile_ZLL == epReq->simpleDescriptor->AppProfileId) ||
           ((N_DeviceInfo_Profile_ZHA == epReq->simpleDescriptor->AppProfileId) && 
             (epReq->simpleDescriptor->AppDeviceVersion >=1))))
    {
      if(epAmount++ == index)
        break;
    }
  }

  if (!epReq)
  {
    N_ERRH_FATAL();
    return 0;
  }
  return epReq->simpleDescriptor->endpoint;
}

/** Interface function, see \ref N_Zcl_DataInd_Subscribe */
void N_PacketDistributor_Subscribe_Impl(uint8_t endpoint, const N_Zcl_DataInd_Callback_t* pCallback)
{
    N_ERRH_ASSERT_FATAL((pCallback               != NULL) &&
                      (pCallback->DataInd != NULL) &&
                      (pCallback->endOfList    == -1  ) );

    for ( uint8_t i = 0u; i < N_PACKET_DISTRIBUTER_MAX_SUBSCRIBERS; i++ )
    {
        if ( s_N_PacketDistributor_Subscribers[i].pCallback == NULL )
        {
            // add subscription
            s_N_PacketDistributor_Subscribers[i].endpoint = endpoint;
            s_N_PacketDistributor_Subscribers[i].pCallback = pCallback;
            return;
        }
        else
        {
            // only one subscription per end-point allowed
            N_ERRH_ASSERT_FATAL(endpoint != s_N_PacketDistributor_Subscribers[i].endpoint);
        }
    }

    // maximum number of subscribers reached
    N_ERRH_FATAL();
}

/** BitCloud APS to ZigBee Platform data indication handler
 *  \param dataInd Pointer to data indication parameters */
static void N_PacketDistributor_DataInd(APS_DataInd_t *dataInd)
{
    // find subscriber for end-point
    uint8_t endpoint = dataInd->dstEndpoint;

    for ( uint8_t i = 0u; i < N_PACKET_DISTRIBUTER_MAX_SUBSCRIBERS; i++ )
    {
        if ( s_N_PacketDistributor_Subscribers[i].pCallback == NULL )
        {
            // end of list
            break;
        }

        if ( s_N_PacketDistributor_Subscribers[i].endpoint == endpoint )
        {
            s_N_PacketDistributor_Subscribers[i].pCallback->DataInd(dataInd);
            break;
        }
    }
}

/** Handle interception of frames with ProfileId different to destination endpoint
 *
 *  \param eventId Event ID
 *  \param data Data associated with event occured
 */
static void nHacObserver(SYS_EventId_t eventId, SYS_EventData_t data)
{
  if (BC_EVENT_ACCESS_REQUEST == eventId)
  {
    BcAccessReq_t *const accessReq = (BcAccessReq_t*)data;

    if (BC_FRAME_FILTERING_BY_PROFILE_ID_ACTION == accessReq->action)
    {
      APS_FrameFilteringByProfileIdContext_t *frameFilteringContext =
        (APS_FrameFilteringByProfileIdContext_t*)accessReq->context;
      ZLL_Endpoint_t *hacEndpoint;

      /* Deny reception by default */
      accessReq->denied = true;

      for (uint8_t i = 0; i < MAX_ZCL_ENDPOINTS; i++)
      {
        hacEndpoint = zllEndpoints[i];
        if (!hacEndpoint)
          return;

        /* Accept HAP frames to endpoints registered in nHacRegisterEndpoint */
        if ((hacEndpoint->simpleDescriptor->endpoint ==
             frameFilteringContext->endpoint->simpleDescriptor->endpoint) &&
            ((PROFILE_ID_HOME_AUTOMATION == frameFilteringContext->profileId) ||
            (APS_WILDCARD_PROFILE_ID == frameFilteringContext->profileId)))
        {
          accessReq->denied = false;
          return;
        }
      }
    }
  }
}

static void nHacRegisterEndpoint(ZLL_Endpoint_t* zllEp)
{
  N_ERRH_ASSERT_FATAL(freeEndpointCell < MAX_ZCL_ENDPOINTS); // Not enough memory to register Endpoint

  zllEndpoints[freeEndpointCell++] = zllEp;
}
