/**************************************************************************//**
  \file macRxEnable.h

  \brief Types', constants' and functions' declarations for IEEE 802.15.4-2006
    rx enable primitives.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      18/06/07 ALuzhetsky - Created.
******************************************************************************/

#ifndef _MACRXENABLE_H
#define _MACRXENABLE_H

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
 * \brief MLME-RX_ENABLE confirm primitive's parameters structure declaration.
 * IEEE 802.15.4-2006 7.1.10.2 MLME-RX-ENABLE.confirm.
 */
typedef struct _MAC_RxEnableConf_t
{
  //! The result of the request to enable or disable the receiver.
  MAC_Status_t status;
}  MAC_RxEnableConf_t;

/**
 * \brief MLME-RX_ENABLE request primitive's parameters structure declaration.
 * IEEE 802.15.4-2006 7.1.10.1 MLME-RX-ENABLE.request.
 * deferPermit and rxOnTime  are NOT used - superframe functionality is not implemented.
 * rxOnDuration behaviour is changed. Non-standard addition.
 */
typedef struct _MAC_RxEnableReq_t
{
  //! Service field - for internal needs.
  MAC_Service_t service;
  //! Behaviour is changed. If rxOnDuration == 0 the receiver is switched off.
  //! If rxOnDuartion != 0 the receiver is switched on.
  uint32_t      rxOnDuration;
  //! MLME-RX-ENABLE callback pointer.
  void (*MAC_RxEnableConf)(MAC_RxEnableConf_t *conf);
  //! MLME-RX-ENABLE confirm parameters' structure.
  MAC_RxEnableConf_t confirm;
}  MAC_RxEnableReq_t;


/******************************************************************************
                        Prototypes section
******************************************************************************/
/**************************************************************************//**
  \brief MLME-RX_ENABLE request primitive's prototype.
  \param reqParams - MLME-RX_ENABLE request parameters' structure pointer.
  \return none.
******************************************************************************/
void MAC_RxEnableReq(MAC_RxEnableReq_t *reqParams);

#endif /* _MACRXENABLE_H */

// eof macRxEnable.h
