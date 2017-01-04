/**************************************************************************//**
  \file nlmeLeave.h

  \brief NLME-LEAVE interface

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2009-04-13 Max Gekk - Created.
   Last change:
    $Id: nlmeLeave.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#if !defined _NLME_LEAVE_H
#define _NLME_LEAVE_H

/******************************************************************************
                                 Includes section
 ******************************************************************************/
#include <appFramework.h>
#include <nwkCommon.h>

/******************************************************************************
                                  Types section
 ******************************************************************************/
/**//**
 * \brief NLME-LEAVE confirm primitive's parameters structure declaration.
 * ZigBee Specification r17, 3.2.2.18 NLME-LEAVE.confirm, page 291.
 */
typedef struct _NWK_LeaveConf_t
{
  /** The status of the corresponding request.*/
  NWK_Status_t status;
} NWK_LeaveConf_t;

/**//**
 * \brief NLME-LEAVE request primitive's parameters structure declaration.
 * Zigbee Specification r17, 3.2.2.16 NLME-LEAVE.request, page 288.
 */
typedef struct _NWK_LeaveReq_t
{
  /** Service fields - for internal needs. */
  struct
  {
    QueueElement_t qelem; /**< link used for queuing */
  } service;
  /** The 64-bit IEEE address of the entity to be removed from the network
   * or NULL if the device removes itself from the network. */
  ExtAddr_t deviceAddress;
  /** Bit field to control of cleaning after leave */
  NWK_LeaveControl_t leaveControl;
  /** This parameter has a value of TRUE if the device being asked to leave the
   * network is also being asked to remove its child devices, if any. Otherwise,
   * it has a value of FALSE. */
  bool removeChildren;
  /** This parameter has a value of TRUE if the device being asked to leave from
   * the current parent is requested to rejoin the network.
   * Otherwise, the parameter has a value of FALSE. */
  bool rejoin;
  /** NLME-LEAVE confirm callback function's pointer. */
  void (*NWK_LeaveConf)(NWK_LeaveConf_t *conf);
  NWK_LeaveConf_t confirm;
} NWK_LeaveReq_t;

/**//**
 * \brief NLME-LEAVE indication primitive's parameters structure declaration.
 * Zigbee Specification r17, 3.2.2.17 NLME-LEAVE.indication, page 290.
 */
 typedef struct _NWK_LeaveInd_t
{
  /** The 64-bit IEEE address of an entity that has removed itself from
   * the network or NULL in the case that the device issuing the primitive has
   * been removed from the network by its parent. */
  ExtAddr_t deviceAddress;
  /** This parameter has a value of TRUE if the device being asked to leave the
   * current parent is requested to rejoin the network. Otherwise, this parameter
   * has a value of FALSE. */
  bool rejoin;
} NWK_LeaveInd_t;

/******************************************************************************
                               Prototypes section
 ******************************************************************************/
/**************************************************************************//**
  \brief NLME-LEAVE request primitive's prototype.

  \param[in] req - NLME-LEAVE request parameters' structure pointer.
  \return None.
 ******************************************************************************/
void NWK_LeaveReq(NWK_LeaveReq_t *const req);

/**************************************************************************//**
  \brief NLME-LEAVE indication primitive's prototype.

  \param[in] ind - NLME-LEAVE indication parameters' structure pointer.
  \return None.
 ******************************************************************************/
extern void NWK_LeaveInd(NWK_LeaveInd_t *ind);

#endif /* _NLME_LEAVE_H */
/** eof nlmeLeave.h */

