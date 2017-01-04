/**************************************************************************//**
  \file nlmeReset.h

  \brief NLME-RESET interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2009-04-13 Max Gekk - Created.
   Last change:
    $Id: nlmeReset.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#if !defined _NLME_RESET_H
#define _NLME_RESET_H

/******************************************************************************
                                Includes section
 ******************************************************************************/
#include <nwkCommon.h>

/******************************************************************************
                                 Types section
 ******************************************************************************/
/**//**
 * \brief NLME-RESET confirm primitive's parameters structure declaration.
 * ZigBee Specification r17, 3.2.2.20 NLME-RESET.confirm, page 293.
 */
typedef struct _NWK_ResetConf_t
{
  /** The result of the reset operation. */
  NWK_Status_t status;
} NWK_ResetConf_t;

/**//**
 * \brief NLME-RESET request primitive's parameters structure declaration.
 * Zigbee Specification r17, 3.2.2.19 NLME-RESET.request, page 292.
 */
typedef struct _NWK_ResetReq_t
{
  /** Service fields - for internal needs. */
  struct
  {
    QueueElement_t qelem; /**< link used for queuing */
  } service;
  /** NLME-RESET confirm callback function's pointer. */
  void (*NWK_ResetConf)(NWK_ResetConf_t *conf);
  NWK_ResetConf_t confirm;
} NWK_ResetReq_t;

/******************************************************************************
                               Prototypes section
 ******************************************************************************/
/**************************************************************************//**
  \brief NLME-RESET request primitive's prototype.

  \param[in] req - NLME-RESET request parameters' structure pointer.
  \return None.
******************************************************************************/
void NWK_ResetReq(NWK_ResetReq_t *const req);

#endif  /* _NLME_RESET_H */
/** eof nlmeReset.h */

