/**************************************************************************//**
  \file nlmePermitJoining.h

  \brief NLME-PERMIT-JOINING interface

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2009-04-12 Max Gekk - Created.
   Last change:
    $Id: nlmePermitJoining.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#if !defined _NLME_PERMIT_JOINING_H
#define _NLME_PERMIT_JOINING_H

/******************************************************************************
                                Includes section
 ******************************************************************************/
#include <sysTypes.h>
#include <nwkCommon.h>

/******************************************************************************
                               Definitions section
 ******************************************************************************/
/** Permissions flags of network rejoin. */
#define NWK_ALLOW_REJOIN 0U
#define NWK_BAN_ED_REJOIN 1U  /* End Devices can't rejoin to this device. */
#define NWK_BAN_ROUTER_REJOIN 2U /* Routers can't rejoin to this device. */
#define NWK_BAN_REJOIN \
  ((NWK_RejoinPermit_t)(NWK_BAN_ED_REJOIN | NWK_BAN_ROUTER_REJOIN))

/******************************************************************************
                                 Types section
 ******************************************************************************/
/**//**
 * \brief NLME-PERMIT-JOINING confirm primitive's parameters structure.
 * ZigBee Specification r17, 3.2.2.6 NLME-PERMIT-JOINING.confirm, page 276.
 */
typedef struct _NWK_PermitJoiningConf_t
{
  /** The status of the corresponding request. Valid Range: NWK_SUCCESS_STATUS
   * or any status returned from the MLME-SET.confirm primitive. */
  NWK_Status_t status;
} NWK_PermitJoiningConf_t;

/**//**
 * \brief NLME-PERMIT-JOINING request primitive's parameters structure.
 * Zigbee Specification r17, 3.2.2.5 NLME-PERMIT-JOINING.request, page 274.
 */
typedef struct _NWK_PermitJoiningReq_t
{
 /** Service fields - for internal needs. */
  struct
  {
    QueueElement_t qelem; /**< link used for queuing */
  } service;
  /** The length of time in seconds during which the ZigBee coordinator or
   * router will allow associations. The value 0x00 and 0xff indicate that
   * permission is disabled or enabled, respectively,
   * without a specified time limit. */
  uint8_t permitDuration;
  /** NLME-PERMIT-JOINING confirm callback function's pointer. */
  void (*NWK_PermitJoiningConf)(NWK_PermitJoiningConf_t *conf);
  NWK_PermitJoiningConf_t confirm;
} NWK_PermitJoiningReq_t;

/** Type of rejoin permissions to the given devices. */
typedef uint8_t NWK_RejoinPermit_t;

/******************************************************************************
                               Prototypes section
 ******************************************************************************/
/**************************************************************************//**
  \brief NLME-PERMIT-JOINING request primitive's prototype.

  \param[in] req - NLME-PERMIT-JOINING request parameters' structure pointer.
  \return None.
 ******************************************************************************/
void NWK_PermitJoiningReq(NWK_PermitJoiningReq_t *const req);

/**************************************************************************//**
  \brief Enable or disable an end deivce or a router rejoin.

  \param[in] permit - rejoin permissions, allow an end device rejoin only
                      if NWK_BAN_ED_REJOIN bit isn't set, and allow a router
                      rejoin only if NWK_BAN_ROUTER_REJOIN flag isn't set.
  \return None.
 ******************************************************************************/
void NWK_SetRejoinPermit(const NWK_RejoinPermit_t permit);

#endif /* _NLME_PERMIT_JOINING_H */
/** eof nlmePermitJoining.h */

