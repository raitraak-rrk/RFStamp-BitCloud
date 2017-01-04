/**************************************************************************//**
  \file N_Groups.c

  \brief Implementation of wrapper for BitCloud stack groups management API.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    01.11.12 D. Kolmakov - Created
******************************************************************************/

/******************************************************************************
                    Includes section
******************************************************************************/
#include <nwk.h>
#include <aps.h>
#include <configServer.h>
#include <N_Groups_Bindings.h>
#include <N_Groups.h>
#include <N_ErrH.h>

/******************************************************************************
                    Defines section
******************************************************************************/
#define COMPID "N_Groups"

/* See ZigBee spec. r18, 2.1.2, page 18. */
#define IS_VALID_ENDPOINT(endpoint) ((APS_ZDO_ENDPOINT < (endpoint)) && ((endpoint) < APS_MIN_RESERVED_ENDPOINT)) || \
                                    (APS_BROADCAST_ENDPOINT == (endpoint)) || (APS_ZGP_RESERVED_ENDPOINT == (endpoint))

/** Maximum number of subscribers to this component */
#if (!defined(N_GROUP_MAX_SUBSCRIBERS))
  #define N_GROUP_MAX_SUBSCRIBERS   1u
#endif

N_UTIL_CALLBACK_DECLARE(N_Groups_Subscriber_t, s_subscribers, N_GROUP_MAX_SUBSCRIBERS);

static void N_Groups_RemovedFromAllGroups(SYS_EventId_t eventId, SYS_EventData_t data);

static SYS_EventReceiver_t eventReceiver = {
  .func = N_Groups_RemovedFromAllGroups
};

/******************************************************************************
                    Implementation section
******************************************************************************/
/** Add the application endpoint to this group.
    \param endpoint The endpoint to add to the group
    \param groupId The group to add the endpoint to
    \returns N_Groups_Status_Ok when successfull, else N_Groups_Status_AlreadyMember or N_Groups_Status_NoCapacity
*/
N_Groups_Status_t N_Groups_AddToGroup_Impl(uint8_t endpoint, uint16_t groupId)
{
  N_ERRH_ASSERT_FATAL(IS_VALID_ENDPOINT(endpoint));

  if (NWK_IsGroupMember(groupId, endpoint))
    return N_Groups_Status_AlreadyMember;

  if (NWK_AddGroup(groupId, endpoint))
  {
    N_UTIL_CALLBACK(N_Groups_Subscriber_t, s_subscribers, AddedToGroup, (endpoint, groupId));
    return N_Groups_Status_Ok;
  }
  else
    return N_Groups_Status_NoCapacity;
}

/** Remove the application endpoint from this group.
    \param endpoint The endpoint to remove from the group
    \param groupId The group to remove the endpoint from
    \returns N_Groups_Status_Ok when successfull, else N_Groups_Status_NotMember.
*/
N_Groups_Status_t N_Groups_RemoveFromGroup_Impl(uint8_t endpoint, uint16_t groupId)
{
  N_ERRH_ASSERT_FATAL(IS_VALID_ENDPOINT(endpoint));

  if(NWK_RemoveGroup(groupId, endpoint))
  {
    N_UTIL_CALLBACK(N_Groups_Subscriber_t, s_subscribers, RemovedFromGroup, (endpoint, groupId));
    return N_Groups_Status_Ok;
  }
  else
    return N_Groups_Status_NotMember;
}

/** Remove the application endpoint from all groups.
    \param endpoint The endpoint to remove from all groups
*/
void N_Groups_RemoveFromAllGroups_Impl(uint8_t endpoint)
{
  N_ERRH_ASSERT_FATAL(IS_VALID_ENDPOINT(endpoint));
  NWK_RemoveAllGroups(endpoint);
}

/** Checks membership of an endpoint to a group.
    \param endpoint The endpoint to check
    \param groupId The group ID to check
    \returns TRUE if the endpoint is member of the group, otherwise FALSE.
*/
bool N_Groups_IsMemberOfGroup_Impl(uint8_t endpoint, uint16_t groupId)
{
  N_ERRH_ASSERT_FATAL(IS_VALID_ENDPOINT(endpoint));

  return NWK_IsGroupMember(groupId, endpoint);
}

/** Gets the total group list capacity
    \returns The group list capacity
*/
uint8_t N_Groups_GetTotalCapacity_Impl(void)
{
  NWK_GroupTableSize_t totalCapacity;

  CS_ReadParameter(CS_GROUP_TABLE_SIZE_ID, &totalCapacity);

  return totalCapacity;
}

/** Gets the remaining group list capacity
    \returns The remaining group list capacity
*/
uint8_t N_Groups_GetRemainingCapacity_Impl(void)
{
  return NWK_GroupCapacity();
}

/** Retrieves the groupId on the given index of the group list
    \param endpoint The endpoint to check
    \param index The index within the group list
    \param pGroup The place to store the groupId retrieved from the list
    \returns N_Groups_Status_Ok when pGroup is valid, otherwise N_Groups_Status_Failed.
*/
N_Groups_Status_t N_Groups_GetGroup_Impl(uint8_t endpoint, uint8_t index, uint16_t* pGroup)
{
  NWK_GroupTableEntry_t *groupEntry = NULL;

  while (NULL != (groupEntry = NWK_NextGroup(groupEntry)))
    if (groupEntry->data == endpoint && !index--)
    {
      *pGroup = groupEntry->addr;
      return N_Groups_Status_Ok;
    }

  return N_Groups_Status_Failed;
}

/** Subscribe to this component's callbacks.
    \param pCallback Pointer to a callback structure filled with functions to be called.
*/
void N_Groups_Subscribe_Impl(const N_Groups_Subscriber_t* pSubscriber)
{
   SYS_SubscribeToEvent(BC_EVENT_GROUPS_REMOVED, &eventReceiver);
   N_UTIL_CALLBACK_SUBSCRIBE(N_Groups_Subscriber_t, s_subscribers, pSubscriber);
}

static void N_Groups_RemovedFromAllGroups(SYS_EventId_t eventId, SYS_EventData_t data)
{
  NWK_GroupTableEntry_t *groupEntry = (NWK_GroupTableEntry_t *)data;
  N_UTIL_CALLBACK(N_Groups_Subscriber_t, s_subscribers, RemovedFromGroup, 
                  (groupEntry->data, groupEntry->addr));
}

/* eof N_Groups.c */
