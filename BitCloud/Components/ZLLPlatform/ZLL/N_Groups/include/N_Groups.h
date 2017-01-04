/*********************************************************************************************//**
\file  

\brief 

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal

$Id: N_Groups.h 24615 2013-02-27 05:14:24Z arazinkov $

***************************************************************************************************/

#ifndef N_GROUPS_H
#define N_GROUPS_H

/***************************************************************************************************
* INCLUDE FILES
***************************************************************************************************/

#include "N_Types.h"

/***************************************************************************************************
* C++ DECLARATION WRAPPER
***************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************************************
* EXPORTED TYPES
***************************************************************************************************/

typedef enum N_Groups_Status_t
{
    N_Groups_Status_Ok = 0,             //< Everything went OK
    N_Groups_Status_AlreadyMember = 1,  //< The endpoint was already member of the group
    N_Groups_Status_NotMember = 2,      //< The endpoint was not a member of the group
    N_Groups_Status_NoCapacity = 3,     //< There is no more group capacity to perform the action
    N_Groups_Status_Failed = 4,         //< The operation failed e.g. due to lack of memory
} N_Groups_Status_t;

typedef struct N_Groups_Subscriber_t
{
    /** An endpoint was added to a group.
        \param endpoint The endpoint that was added to the group
        \param groupId The group ID to which the endpoint was added
    */
    void (*AddedToGroup)(uint8_t endpoint, uint16_t groupId);

    /** An endpoint was removed from a group.
        \param endpoint The endpoint that was removed from the group
        \param groupId The group ID from which the endpoint was removed
        \note Is called multiple times if all groups are removed from an endpoint
    */
    void (*RemovedFromGroup)(uint8_t endpoint, uint16_t groupId);

    /** Guard. Must be -1. */
    int8_t endOfList;

} N_Groups_Subscriber_t;

/***************************************************************************************************
* EXPORTED MACROS AND CONSTANTS
***************************************************************************************************/


/***************************************************************************************************
* EXPORTED FUNCTIONS
***************************************************************************************************/

/** Add the application endpoint to this group.
    \param endpoint The endpoint to add to the group
    \param groupId The group to add the endpoint to
    \returns N_Groups_Status_Ok when successfull, else N_Groups_Status_AlreadyMember or N_Groups_Status_NoCapacity
*/
N_Groups_Status_t N_Groups_AddToGroup(uint8_t endpoint, uint16_t groupId);

/** Remove the application endpoint from this group.
    \param endpoint The endpoint to remove from the group
    \param groupId The group to remove the endpoint from
    \returns N_Groups_Status_Ok when successfull, else N_Groups_Status_NotMember.
*/
N_Groups_Status_t N_Groups_RemoveFromGroup(uint8_t endpoint, uint16_t groupId);

/** Remove the application endpoint from all groups.
    \param endpoint The endpoint to remove from all groups
*/
void N_Groups_RemoveFromAllGroups(uint8_t endpoint);

/** Checks membership of an endpoint to a group.
    \param endpoint The endpoint to check
    \param groupId The group ID to check
    \returns TRUE if the endpoint is member of the group, otherwise FALSE.
*/
bool N_Groups_IsMemberOfGroup(uint8_t endpoint, uint16_t groupId);

/** Gets the total group list capacity
    \returns The group list capacity
*/
uint8_t N_Groups_GetTotalCapacity(void);

/** Gets the remaining group list capacity
    \returns The remaining group list capacity
*/
uint8_t N_Groups_GetRemainingCapacity(void);

/** Retrieves the groupId on the given index of the group list
    \param endpoint The endpoint to check
    \param index The index within the group list
    \param pGroup The place to store the groupId retrieved from the list
    \returns N_Groups_Status_Ok when pGroup is valid, otherwise N_Groups_Status_Failed.
*/
N_Groups_Status_t N_Groups_GetGroup(uint8_t endpoint, uint8_t index, uint16_t* pGroup);

/** Subscribe to this component's callbacks.
    \param pCallback Pointer to a callback structure filled with functions to be called.
*/
void N_Groups_Subscribe(const N_Groups_Subscriber_t* pSubscriber);

/***************************************************************************************************
* END OF C++ DECLARATION WRAPPER
***************************************************************************************************/

#ifdef __cplusplus
}
#endif

#endif // N_GROUPS_H
