/**************************************************************************//**
  \file nlmeSync.h

  \brief NLME-SYNC interface

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2009-04-12 Max Gekk - Created.
   Last change:
    $Id: nlmeSync.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#if !defined _NLME_SYNC_H
#define _NLME_SYNC_H

/******************************************************************************
                                Includes section
 ******************************************************************************/
#include <nwkCommon.h>

/******************************************************************************
                                 Types section
 ******************************************************************************/
/**//**
 * \brief NLME-SYNC confirm primitive's parameters structure declaration.
 * ZigBee Specification r17, 3.2.2.24 NLME-SYNC.confirm, page 296.
 */
typedef struct _NWK_SyncConf_t
{
  /** The result of the request to synchronize with the ZigBee coordinator or
   * router. Valid Range: NWK_SUCCESS_STATUS, NWK_INVALID_PARAMETER_STATUS,
   * or any status value returned from the MLME_POLL.confirm primitive. */
  NWK_Status_t status;
} NWK_SyncConf_t;

/**//**
 * \brief NLME-SYNC request primitive's parameters structure declaration.
 * Zigbee Specification r17, 3.2.2.22 NLME-SYNC.request, page 294.
 */
typedef struct _NWK_SyncReq_t
{
  /** Service fields - for internal needs. */
  struct
  {
    QueueElement_t qelem; /**< link used for queuing */
  } service;
  /** Whether or not the synchronization should be maintained for future beacons.
   **/
  bool track;
  /** NLME-SYNC confirm callback function's pointer. Must not be set to NULL. */
  void (*NWK_SyncConf)(NWK_SyncConf_t *conf);
  NWK_SyncConf_t confirm;
} NWK_SyncReq_t;

/******************************************************************************
                               Prototypes section
 ******************************************************************************/
/**************************************************************************//**
  \brief NLME-SYNC request primitive's prototype.

  The function sends a data request to the parent polling for buffered data. The confirmation callback is raised with NWK_SUCCESS_STATUS, if the
  request has been executed successfully and a response from the parents has been received. If the current node's parent is storing data frames
  for the child, then after sending response to the polling request it starts sending the data. When the child receives the data the standard callback
  idicating data frame reception is executed.

  \param[in] req - NLME-SYNC request parameters' structure pointer.
  \return None.
 ******************************************************************************/
void NWK_SyncReq(NWK_SyncReq_t *const req);

#endif /* _NLME_SYNC_H */
/** eof nlmeSync.h */

