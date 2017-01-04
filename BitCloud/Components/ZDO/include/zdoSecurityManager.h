/************************************************************************//**
  \file zdoSecurityManager.h

  \brief The header file describes the constants, types and internal interface
  of ZDO security manager

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
******************************************************************************/

#ifndef _ZDOSECURITYMANAGER_H
#define _ZDOSECURITYMANAGER_H

/******************************************************************************
                    Includes section
******************************************************************************/
#include <appTimer.h>
#include <zdo.h>

/******************************************************************************
                    Types section
******************************************************************************/
typedef enum
{
  ZDO_IDLE_SECURITY_STATE,
  ZDO_NORMAL_OPERATING_SECURITY_STATE,
  ZDO_WAITING_ESTABLISH_KEY_SECURITY_STATE,
  ZDO_WAITING_TRANSPORT_KEY_SECURITY_STATE,
  ZDO_WAITING_PARENT_AUTHENTICATION_SECURITY_STATE,
  ZDO_WAITING_CHILD_AUTHENTICATION_SECURITY_STATE,
  ZDO_APP_KEY_ESTABLISH_RESPONDER_SECURITY_STATE,
  ZDO_APP_KEY_ESTABLISH_INITIATOR_SECURITY_STATE,
  ZDO_STARTING_NORMAL_OPERATING_SECURITY_STATE,
  ZDO_FAIL_CONFIRM_SECURITY_STATE,
  ZDO_CONFIRM_APP_KEY_SECURITY_STATE,
  ZDO_WAITING_APP_KEY_SECURITY_STATE,
  ZDO_WAITING_TC_LINK_KEY_SECURITY_STATE,
} ZdoSecurityManagerState_t;

typedef struct
{
  ZdoSecurityManagerState_t state;
#ifdef _SECURITY_
  HAL_AppTimer_t             waitTimer;
  HAL_AppTimer_t             delayTimer;
  uint8_t                    newKeySeqNum;
#ifdef _LINK_SECURITY_
  union
  {
    APS_AuthenticateReq_t    apsAuthenticateReq;
#ifdef _HIGH_SECURITY_
    APS_EstablishKeyReq_t    apsEstablishKeyReq;
#endif // _HIGH_SECURITY_
#ifdef _CUSTOM_4_SECURITY_MODE_
    APS_RequestKeyReq_t      apsRequestKeyReq;
#endif // _CUSTOM_4_SECURITY_MODE_
  };
#ifdef _HIGH_SECURITY_
  APS_EstablishKeyResp_t     apsEstablishKeyResp;
#endif // _HIGH_SECURITY_
  ZDO_EndToEndAppKeyReq_t   *appKeyReq;
#endif // _LINK_SECURITY_
#endif // _SECURITY_
} ZdoSecurityManager_t;

/******************************************************************************
                    Prototypes section
******************************************************************************/
void zdoResetSecurityManager(void);
void zdoAuthenticationStart(void);
void zdoAuthenticationConf(ZDO_Status_t status);
uint8_t ZDO_GetPreconfiguredStatus(void);
void zdoAuthenticateChild(const ExtAddr_t *childAddr);
void zdoSecurityManagerHandler(void);

#endif // _ZDOSECURITYMANAGER_H

// eof zdoSecurityManager.h
