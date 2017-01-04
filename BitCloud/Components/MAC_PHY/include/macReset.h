/**************************************************************************//**
  \file macReset.h

  \brief Types', constants' and functions' declarations for IEEE 802.15.4-2006
    reset primitives.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      18/06/07 ALuzhetsky - Created.
******************************************************************************/

#ifndef _MACRESET_H
#define _MACRESET_H

/******************************************************************************
                        Includes section
******************************************************************************/
#include <macAddr.h>
#include <macCommon.h>

/******************************************************************************
                        Defines section
******************************************************************************/
/******************************************************************************
                        Types section
******************************************************************************/
/**
 * \brief MLME-RESET confirm primitive's parameters structure declaration.
 * IEEE 802.15.4-2006 7.1.9.2 MLME-RESET.confirm.
 */
typedef struct
{
  //! The result of the reset operation.
  MAC_Status_t status;
}  MAC_ResetConf_t;

/**
 * \brief MLME-RESET request primitive's parameters structure declaration.
 * IEEE 802.15.4-2006 7.1.9.1 MLME-RESET.request
 */
typedef struct
{
  //! Service field - for internal needs.
  MAC_Service_t service;
  //! If TRUE, the MAC sublayer is reset, and all MAC PIB attributes are set
  //! to their default values. It resets radio chip also.
  bool setDefaultPib;
  // MLME-RESET confirm callback function's pointer.
  void (*MAC_ResetConf)(MAC_ResetConf_t *conf);
  // MLME-RESET confirm parameters structure.
  MAC_ResetConf_t confirm;
}  MAC_ResetReq_t;

/******************************************************************************
                        Prototypes section
******************************************************************************/
/**************************************************************************//**
  \brief MLME-RESET request primitive's prototype.
  \param reqParams - MLME-RESET request parameters' structure pointer.
  \return none.
******************************************************************************/
void MAC_ResetReq(MAC_ResetReq_t *reqParams);

#endif /* _MACRESET_H */

// eof macReset.h
