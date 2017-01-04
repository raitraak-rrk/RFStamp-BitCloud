/**************************************************************************//**
  \file macPurge.h

  \brief Types', constants' and functions' declarations for IEEE 802.15.4-2006
     purge primitives.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      18/06/07 ALuzhetsky - Created.
******************************************************************************/

#ifndef _MACPURGE_H
#define _MACPURGE_H

/******************************************************************************
                        Includes section
******************************************************************************/
#include <sysTypes.h>
#include <macCommon.h>

/******************************************************************************
                        Defines section
******************************************************************************/

/******************************************************************************
                        Types section
******************************************************************************/
/**
 * \brief MCPS-PURGE confirm primitive's parameters structure declaration.
 * IEEE 802.15.4-2006 7.1.1.5 MCPS-PURGE.confirm.
 */
typedef struct
{
  //! The handle of the MSDU requested to be purge from the transaction queue.
  uint8_t msduHandle;
  //! The status of the request to be purged an MSDU from the transaction queue.
  MAC_Status_t status;
}  MAC_PurgeConf_t;

/**
 * \brief MCPS-PURGE request primitive's parameters structure declaration.
 * IEEE 802.15.4-2006 7.1.1.4 MCPS-PURGE.request.
 */
typedef struct
{
  //! Service field - for internal needs.
  MAC_Service_t service;
  //! The handle of the MSDU to be purged from the transaction queue.
  uint8_t       msduHandle;
  //! MCPS-PURGE confirm callback function pointer.
  void (*MAC_PurgeConf)(MAC_PurgeConf_t *conf);
  //! MCPS-PURGE confirm parameters' structure.
  MAC_PurgeConf_t confirm;
}  MAC_PurgeReq_t;

/******************************************************************************
                        Prototypes section
******************************************************************************/
/**************************************************************************//**
  \brief MCPS-PURGE request primitive's prototype.
  \param reqParams - MCPS-PURGE request parameters' structure pointer.
  \return none.
******************************************************************************/
void MAC_PurgeReq(MAC_PurgeReq_t *reqParams);

#endif /* _MACPURGE_H */

// eof macPurge.h
