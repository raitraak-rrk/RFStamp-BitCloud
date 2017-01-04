/**************************************************************************//**
  \file nlmeStartRouter.h

  \brief NLME-START-ROUTER interface

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2009-04-11 Max Gekk - Created.
   Last change:
    $Id: nlmeStartRouter.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#if !defined _NLME_START_ROUTER_H
#define _NLME_START_ROUTER_H

/******************************************************************************
                              Includes section
 ******************************************************************************/
#include <mac.h>
#include <nwkCommon.h>

/******************************************************************************
                               Types section
 ******************************************************************************/
/**//**
 * \brief NLME-START-ROUTER confirm primitive's parameters structure declaration.
 * ZigBee Specification r17, 3.2.2.7 NLME-START-ROUTER.confirm
 */
typedef struct _NWK_StartRouterConf_t
{
  /** The result of the attempt to initialize a ZigBee router.
   * Valid range: NWK_INVALID_REQUEST_STATUS or any status value returned from
   * the MAC_StartConf callback.*/
  NWK_Status_t status;
} NWK_StartRouterConf_t;

/**//**
 * \brief NLME-START-ROUTER request primitive's parameters structure declaration.
 * Zigbee Specification r17, 3.2.2.8 NLME-START-ROUTER.request.
 */
typedef struct _NWK_StartRouterReq_t
{
  /** Service fields - for internal needs. */
  struct
  {
    QueueElement_t qelem; /**< link used for queuing */
    MAC_StartReq_t macStart;
  } service;
  /* The parameter tunes joining. It defines the joining method. */
  NWK_JoinControl_t ctrl;
  /* NLME-START-ROUTER confirm callback function's pointer. */
  void (*NWK_StartRouterConf)(NWK_StartRouterConf_t *conf);
  /** NLME-START-ROUTER confirm primitive's parameters */
  NWK_StartRouterConf_t confirm;
} NWK_StartRouterReq_t;

/******************************************************************************
                              Prototypes section
 ******************************************************************************/
/**************************************************************************//**
  \brief NLME-START-ROUTER request primitive's prototype.

  \param[in] req - NLME-START-ROUTER request parameters' structure pointer.
  \return None.
 ******************************************************************************/
void NWK_StartRouterReq(NWK_StartRouterReq_t *const req);

#endif /* _NLME_START_ROUTER_H */
/** eof nlmeStartRouter.h */

