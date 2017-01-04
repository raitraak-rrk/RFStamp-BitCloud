/**************************************************************************//**
  \file zdoStartNetwork.h

  \brief Interface for ZDO Start Network request routines

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    17.09.09 A. Taradov - Created
******************************************************************************/
#ifndef _ZDOSTARTNETWORK_H
#define _ZDOSTARTNETWORK_H

/******************************************************************************
                        Includes section
******************************************************************************/
#include <zdo.h>
#include <sysQueue.h>
#include <nwk.h>

/******************************************************************************
                        Definitions section
******************************************************************************/

/******************************************************************************
                        Types section
******************************************************************************/
typedef enum _ZdoStartNetworkState_t
{
  ZDO_START_NETWORK_IDLE_STATE,
  ZDO_START_NETWORK_BEGIN_STATE,
  ZDO_START_NETWORK_APS_RESET_STATE,
  ZDO_START_NETWORK_FORMATION_STATE,
  ZDO_START_NETWORK_JOIN_STATE,
  ZDO_START_NETWORK_REJOIN_STATE,
  ZDO_START_NETWORK_JOINED_STATE,
  ZDO_START_NETWORK_START_ROUTER_STATE,
  ZDO_START_NETWORK_PERMIT_JOIN_STATE,
  ZDO_START_NETWORK_APS_START_STATE,
  ZDO_START_NETWORK_ANNCE_STATE,
  ZDO_START_NETWORK_CONFIRM_STATE
} ZdoStartNetworkState_t;

typedef void (ZdoStartNetworkConf_t)(ZDO_Status_t status);

typedef struct _ZdoStartNetwork_t
{
  QueueDescriptor_t        queue;
  ZdoStartNetworkState_t   state;
  NWK_JoinControl_t        joinCtrl;
  ZdoStartNetworkConf_t    *confirm;
#if defined(_ROUTER_) || defined(_ENDDEVICE_)
  uint8_t                  joinAttempts;
  HAL_AppTimer_t           joinTimer;
#endif
} ZdoStartNetwork_t;

/******************************************************************************
                        Prototypes section
******************************************************************************/

/******************************************************************************
  Reset Start Network component.
  Parameters:
    none
  Return:
    none
******************************************************************************/
ZDO_PRIVATE void zdoStartNetworkReset(void);

/******************************************************************************
  Start Network request task handler.
  Parameters:
    none
  Return:
    none
******************************************************************************/
ZDO_PRIVATE void zdoStartNetworkTaskHandler(void);

/******************************************************************************
  Allocate Start Network module
  Parameters:
    startNetworkConf - confirmation callback
  Return:
    true - if module was alocated, false otherwise
******************************************************************************/
ZDO_PRIVATE bool zdoStartNetworkAlloc(ZdoStartNetworkConf_t *startNetworkConf);

/******************************************************************************
  Free Start Network module
  Parameters:
    none
  Return:
    none
******************************************************************************/
ZDO_PRIVATE void zdoStartNetworkFree(void);

#ifdef _COORDINATOR_
/******************************************************************************
  NLME-NETWORK-FORMATION.request preparing and sending.
  Parameters:
    none
  Return:
    none
******************************************************************************/
ZDO_PRIVATE void zdoNetworkFormationReq(void);
#endif /* _COORDINATOR_ */

#if defined _ROUTER_ || defined _ENDDEVICE_
/**************************************************************************//**
  \brief Rejoin network
 ******************************************************************************/
ZDO_PRIVATE void zdoRejoinNetwork(void);
#endif

#if !defined _LIGHT_LINK_PROFILE_ && (defined _ROUTER_ || defined _ENDDEVICE_)
/******************************************************************************
  \brief Start Intra-PAN portability procedure. Stop activity and try to rejoin.
 ******************************************************************************/
ZDO_PRIVATE void zdoPerformIntraPanPortability(void);

#else
#define zdoPerformIntraPanPortability() (void)0
#endif

#endif // _ZDOSTARTNETWORK_H

// eof zdoStartNetwork.h
