/**************************************************************************//**
  \file zdoLeaveNetwork.h

  \brief Interface for ZDO Leave Network routines

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    29.10.09 A. Taradov - Created
******************************************************************************/
#ifndef _ZDOLEAVENETWORK_H
#define _ZDOLEAVENETWORK_H

/******************************************************************************
                        Includes section
******************************************************************************/
#include <zdo.h>
#include <nwkCommon.h>

/******************************************************************************
                        Definitions section
******************************************************************************/

/******************************************************************************
                        Types section
******************************************************************************/
typedef enum _ZdoLeaveNetworkState_t
{
  ZDO_LEAVE_NETWORK_IDLE_STATE,
  ZDO_LEAVE_NETWORK_NWK_LEAVE_STATE,
  ZDO_LEAVE_NETWORK_APS_STOP_STATE,
  ZDO_LEAVE_NETWORK_REMOTE_STATE
} ZdoLeaveNetworkState_t;

typedef void (ZdoLeaveNetworkConf_t)(ZDO_Status_t status);

typedef struct _ZdoLeaveNetwork_t
{
  ZdoLeaveNetworkState_t state;
  ZdoLeaveNetworkConf_t  *confirm;
  bool                   rejoin;
  NWK_LeaveControl_t     leaveControl;
} ZdoLeaveNetwork_t;

/******************************************************************************
                        Prototypes section
******************************************************************************/

/******************************************************************************
  Network leave indication
  Parameters:
    rejoin - true if rejoin is required
  Return:
    none
******************************************************************************/
ZDO_PRIVATE void zdoLeaveNetworkInd(bool rejoin);

/******************************************************************************
  Leave device from network
  Parameters:
    removeChildren - remove or not children
    rejoin - perform or not rejoin after leaving
  Returns:
    None
******************************************************************************/
ZDO_PRIVATE void zdoLeaveNetworkReq(bool removeChildren, bool rejoin);

/******************************************************************************
  Leave device from network. Leave Network module must be allocated before
    calling this function.
  Parameters:
    removeChildren - remove or not children
    rejoin - perform or not rejoin after leaving
  Returns:
    None
******************************************************************************/
ZDO_PRIVATE void zdoLeaveNetwork(bool removeChildren, bool rejoin);

/******************************************************************************
  Allocate Leave Network module
  Parameters:
    leaveNetworkConf - confirmation callback
  Return:
    true - if module was alocated, false otherwise
******************************************************************************/
ZDO_PRIVATE bool zdoLeaveNetworkAlloc(ZdoLeaveNetworkConf_t *leaveNetworkConf);

/******************************************************************************
  Free Leave Network module
  Parameters:
    none
  Return:
    none
******************************************************************************/
ZDO_PRIVATE void zdoLeaveNetworkFree(void);

/******************************************************************************
  Reset Leave Network component.
  Parameters:
    none
  Return:
    none
******************************************************************************/
ZDO_PRIVATE void zdoLeaveNetworkReset(void);

#endif // _ZDOLEAVENETWORK_H

// eof zdoLeaveNetwork.h
