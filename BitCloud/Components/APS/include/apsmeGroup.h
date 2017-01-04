/**************************************************************************//**
  \file apsmeGroup.h

  \brief Interface of APS Group table.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    24/07/07 A. Potashov - Created.
    21/07/10 M. Gekk     - Refactoring.
   Last change:
    $Id: apsmeGroup.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#if !defined _APSME_GROUP_H
#define _APSME_GROUP_H
/**//**
 *
 *  This interface allows the next higher layer to manage group membership
 * for endpoints on the current device by adding and removing entries in
 * the group table. See ZigBee spec. r18, 2.2.4.5, page 45.
 *
 **/

/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <apsCommon.h>

/******************************************************************************
                                Types section
 ******************************************************************************/
/**//**
 * \struct APS_AddGroupConf_t apsmeGroup.h "aps.h"
 *
 * \brief Parameters of the APSME-ADD-GROUP.confirm primitive.
 **/
typedef struct
{
  /** The status of the request to add a group. */
  APS_Status_t status;
} APS_AddGroupConf_t;

/**//**
 * \struct APS_AddGroupReq_t apsmeGroup.h "aps.h"
 *
 * \brief Parameters of the APS_AddGroupReq() function.
 **/
typedef struct
{
  /** \ref Endian "[LE]" The 16-bit address of the group being added. */
  GroupAddr_t groupAddress;
  /** The endpoint to which the given group is being added. */
  Endpoint_t endpoint;

  /** The confirm primitive as transaction result. */
  APS_AddGroupConf_t confirm;
} APS_AddGroupReq_t;

/**//**
 * \struct APS_RemoveGroupConf_t apsmeGroup.h "aps.h"
 *
 * \brief The primitive presenting the result of APS_RemoveAllGroupReq() function.
 **/
typedef struct
{
  /** The status of the request to remove a group. */
  APS_Status_t status;
} APS_RemoveGroupConf_t;

/**//**
 * \struct APS_RemoveGroupReq_t apsmeGroup.h "aps.h"
 *
 * \brief Parameters of the APS_RemoveGroupReq() function.
 **/
typedef struct
{
  /** \ref Endian "[LE]" The 16-bit address of the group being removed. */
  GroupAddr_t groupAddress;
  /** The endpoint at which the given group is being removed. */
  Endpoint_t endpoint;

  /** The confirm primitive containing the result of the function call. */
  APS_RemoveGroupConf_t confirm;
} APS_RemoveGroupReq_t;

/**//**
 * \struct APS_RemoveAllGroupsConf_t apsmeGroup.h "aps.h"
 *
 * \brief The structure presenting the result of APS_RemoveAllGroupReq() function.
 **/
typedef struct
{
  /** The status of the request to remove all groups. */
  APS_Status_t status;
} APS_RemoveAllGroupsConf_t;

/**//**
 * \struct APS_RemoveAllGroupsReq_t apsmeGroup.h "aps.h"
 *
 * \brief Parameters of the APS_RemoveAllGroupsReq() function.
 **/
typedef struct
{
  /** The endpoint from which the group is being removed. */
  Endpoint_t endpoint;

  /** The confirm primitive as transaction result. */
  APS_RemoveAllGroupsConf_t confirm;
} APS_RemoveAllGroupsReq_t;

/******************************************************************************
                               Prototypes section
 ******************************************************************************/
/**************************************************************************//**
  \brief Adds the current device to the members of a certain group.

  \ingroup aps_data

  The function makes the current device a member of the specified group. A call to the function is done
  synchronously since no requests are sent to the network. Instead, the function simply adds a new entry
  to the group table associating the group address and the endpoint ID specified within the parameters.
  The function can be called several times with the same group address and different endpoints which will
  cause inserting an entry for each distinct pair of the group address and an endpoint. When a data frame is
  sent to a group each group member upon reception of the frame fires indication callbacks for all endpoints
  associated with the group address specified in the request.

  The result of task execution is indicated by APS_AddGroupReq_t::confirm property inside the request
  parameters. The status field reports
  \li ::APS_SUCCESS_STATUS (0x00) if the device has been added to the group
  \li ::APS_TABLE_FULL_STATUS (0xAE) if the group table is full and a new entry can not be added.

  \param[in] req - pointer to parameters of the APSME-ADD-GROUP.request.
 *****************************************************************************/
void APS_AddGroupReq(APS_AddGroupReq_t *const req);

/**************************************************************************//**
  \brief Removes the current device from the members of a certain group.

  \ingroup aps_data

  The function removes the node from a group. A call to the function is done synchronously. The function drops
  all entries with the specified group and endpoint inside from the group table.

  The result of task execution is indicated by APS_AddGroupReq_t::confirm property inside the request
  parameters.  The status field reports
  \li ::APS_SUCCESS_STATUS (0x00) if the device has been removed from the group
  \li ::APS_INVALID_PARAMETER_STATUS (0xA6)  if requested entry has not been found in the group table

  \param[in] req - pointer to parameters of the APSME-REMOVE-GROUP.request.
 ******************************************************************************/
void APS_RemoveGroupReq(APS_RemoveGroupReq_t *const req);

/**************************************************************************//**
  \brief Removes the current device from the members of all groups for which a given
  endpoint if registered

  \ingroup aps_data

  The function unscribe the current device from the members of all groups for which
  a given endpoint is registered. To fullfill the task the function deletes all entries with a given
  endpoint inside from the group table. If 0xFF is specified for endpoint the node is removed
  from all groups.

  The result of task execution is indicated by APS_AddGroupReq_t::confirm field inside the request
  parameters.

  \param[in] req - pointer to parameters of the APSME-REMOVE-ALL-GROUPS.request.
 ******************************************************************************/
void APS_RemoveAllGroupsReq(APS_RemoveAllGroupsReq_t *const req);

#endif /* _APSME_GROUP_H */
/** eof apsmeGroup.h */

