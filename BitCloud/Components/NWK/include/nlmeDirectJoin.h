/**************************************************************************//**
  \file nlmeDirectJoin.h

  \brief NLME-DIRECT-JOIN interface

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2009-04-12 Max Gekk - Created.
   Last change:
    $Id: nlmeDirectJoin.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#if !defined _NLME_DIRECT_JOIN_H
#define _NLME_DIRECT_JOIN_H

/******************************************************************************
                                Includes section
 ******************************************************************************/
#include <appFramework.h>
#include <nwkCommon.h>

/******************************************************************************
                                 Types section
 ******************************************************************************/
/**//**
 * \brief NLME-DIRECT-JOIN confirm primitive's parameters structure declaration.
 * ZigBee Specification r17, 3.2.2.15 NLME-DIRECT-JOIN.confirm, page 287.
 */
typedef struct _NWK_DirectJoinConf_t
{
  /** The status of the corresponding request. */
  NWK_Status_t status;
} NWK_DirectJoinConf_t;

/**//**
 * \brief NLME-DIRECT-JOIN request primitive's parameters structure declaration.
 * Zigbee Specification r17, 3.2.2.14 NLME-DIRECT-JOIN.request, page 286.
 */
typedef struct _NWK_DirectJoinReq_t
{
  /** Service fields - for internal needs. */
  struct
  {
    QueueElement_t qelem; /**< link used for queuing */
  } service;
  /** The IEEE address of the device to be directly joined.*/
  ExtAddr_t deviceAddress;
  /** Recommended network address. */
  ShortAddr_t networkAddress;
  /** The operating capabilities of the device being directly joined. */
  MAC_CapabilityInf_t capabilityInformation;
  /** NLME-DIRECT-JOIN confirm callback function's pointer. */
  void (*NWK_DirectJoinConf)(NWK_DirectJoinConf_t *conf);
  NWK_DirectJoinConf_t confirm;
} NWK_DirectJoinReq_t;

/******************************************************************************
                               Prototypes section
 ******************************************************************************/
/**************************************************************************//**
  \brief NLME-DIRECT-JOIN request primitive's prototype.

  \param[in] req - NLME-DIRECT-JOIN request parameters' structure pointer.
  \return None.
 ******************************************************************************/
void NWK_DirectJoinReq(NWK_DirectJoinReq_t *const req);

#endif /* _NLME_DIRECT_JOIN_H */
/** eof nlmeDirectJoin.h */

