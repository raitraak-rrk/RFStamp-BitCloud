/************************************************************************//**
  \file zdoNwkManager.h

  \brief The header file describes the constants, types and internal interface
  of ZDO network manager

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    07/06/07 I. Kalganova - Modified
******************************************************************************/

#ifndef _ZDONWKMANAGER_H
#define _ZDONWKMANAGER_H

/******************************************************************************
                    Includes section
******************************************************************************/
#include <macAddr.h>
#include <nwk.h>
#include <zdo.h>
#include <zdoTaskManager.h>
#include <aps.h>
#include <zdoCommands.h>

/******************************************************************************
                    Types section
******************************************************************************/
/********************************************************************************//**
\brief States of ZDO NWKUpdateState functionality
***********************************************************************************/
typedef enum
{
  ZDO_IDLE_NWK_UPDATE_STATE,
  ZDO_INTERNAL_NWK_UPDATE_STATE,
  ZDO_EXTERNAL_NWK_UPDATE_STATE,
  ZDO_EXTERNAL_ERROR_NWK_UPDATE_STATE
} ZdoNWKUpdateState_t;

typedef struct _ZdoNwkManager_t
{
  uint8_t               statusIndCounter: 4;
  bool                  updateDeviceReqBusy: 1;
  bool                  updateDeviceReqPending: 1;
  MAC_SetReq_t          macSetReq;
  ZDO_SetTxPowerReq_t   *txPowerReq;
#ifdef _SECURITY_
  APS_UpdateDeviceReq_t apsUpdateDeviceReq;
#endif //_SECURITY_
  uint16_t              totalTransactions;
  uint16_t              failTransactions;
  bool                  badLink;
  HAL_AppTimer_t        updateNwkWaitTimer;
  uint8_t               transSeqNum;
} ZdoNwkManager_t;

/******************************************************************************
                    Prototypes section
******************************************************************************/
ZDO_PRIVATE void zdoNwkManagerReset(void);

/******************************************************************************
  Send Device Annce command
  Parameters:
    deviceAnnceConf - confirmation callback
  Return:
    none
******************************************************************************/
ZDO_PRIVATE void zdoSendDeviceAnnce(ZdoCommand_t *const zdoCommand,
  void (*deviceAnnceConf)(ZDO_ZdpResp_t *zdpResp));

#if defined _SECURITY_
#if defined(_COORDINATOR_) || defined(_ROUTER_) || defined(_LINK_SECURITY_)
/******************************************************************************
  Check if short address is available for given extended address and send
  appropriate request if not.
  Parameters:
    addr - extended address to check
  Return:
    true - short address already available
    false - short address currently is not available, request was sent
******************************************************************************/
ZDO_PRIVATE bool zdoDiscoverNwkAddress(const ExtAddr_t *const addr);
#endif
#endif

#endif // _ZDONWKMANAGER_H

// eof zdoNwkManager.h
