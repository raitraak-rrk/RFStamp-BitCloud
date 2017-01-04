/************************************************************************//**
  \file zdoMem.h

  \brief The header file describes the internal ZDO memory

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
******************************************************************************/

#ifndef _ZDOMEM_H
#define _ZDOMEM_H

#include <zdo.h>
#include <zdoNwkManager.h>
#include <zdoPowerManager.h>
#include <zdoStartNetwork.h>
#include <zdoLeaveNetwork.h>
#include <zdoZdpReq.h>
#include <zdoSecurityManager.h>
#include <zdoEndDeviceBinding.h>
#include <zdoPermissionsTable.h>
#include <zdoAddrResolvingInt.h>
#include <zdoMgmtNwkUpdate.h>

typedef union _ZdoStackReq_t
{
  // Requests from ZDO to NLME
#if defined(_COORDINATOR_)
  NWK_NetworkFormationReq_t nwkFormationReq;
#endif

#if defined(_ROUTER_) || defined(_ENDDEVICE_)
  NWK_JoinReq_t             nwkJoinReq;
#endif

#if defined(_COORDINATOR_) || defined(_ROUTER_)
  NWK_PermitJoiningReq_t    nwkPermitJoiningReq;
#endif

#if defined(_ROUTER_)
  NWK_StartRouterReq_t      nwkStartRouterReq;
#endif

  NWK_LeaveReq_t            nwkLeaveReq;

  // Requests from ZDO to APSME
  APS_ResetReq_t            apsResetReq;
  APS_StartReq_t            apsStartReq;
  APS_StopReq_t             apsStopReq;
} ZdoStackReq_t;

typedef struct _ZdoMem_t
{
  ZdoStackReq_t             stackReq;
  ZdoStartNetwork_t         zdoStartNetwork;
  ZdoLeaveNetwork_t         zdoLeaveNetwork;
  ZdoPowerManager_t         powerManager;
  ZdoNwkManager_t           nwkManager;
  ZdoZdpReq_t               zdoZdpReq;
#ifdef _SECURITY_
  ZdoSecurityManager_t      zdoSecurityManager;
#endif
  uint16_t                  enabledModules;
#ifdef _END_DEVICE_BINDING_
  ZdoEndDeviceBinding_t     endDevBinding;
#endif
#ifdef _PERMISSIONS_
  ZdoPermissions_t          permissions;
#endif
  ZdoAddrResolving_t        zdoAddrResolving;
  ZdoMgmtNwkUpdate_t        mgmtNwkUpdate;
} ZdoMem_t;

extern ZdoMem_t zdoMem;

static inline ZdoStackReq_t * zdoMemStackRequest(void)
{
  return &zdoMem.stackReq;
}

static inline ZdoPowerManager_t * zdoMemPowerManager(void)
{
  return &zdoMem.powerManager;
}

static inline ZdoNwkManager_t * zdoMemNwkManager(void)
{
  return &zdoMem.nwkManager;
}

static inline ZdoZdpReq_t * zdoMemZdpReq(void)
{
  return &zdoMem.zdoZdpReq;
}

static inline ZdoStartNetwork_t * zdoMemStartNetwork(void)
{
  return &zdoMem.zdoStartNetwork;
}

static inline ZdoLeaveNetwork_t * zdoMemLeaveNetwork(void)
{
  return &zdoMem.zdoLeaveNetwork;
}

#ifdef _SECURITY_
static inline ZdoSecurityManager_t * zdoMemSecurityManager(void)
{
  return &zdoMem.zdoSecurityManager;
}
#endif

#ifdef _END_DEVICE_BINDING_
static inline ZdoEndDeviceBinding_t * zdoMemEndDeviceBinding(void)
{
  return &zdoMem.endDevBinding;
}
#endif

#ifdef _PERMISSIONS_
static inline ZdoPermissions_t * zdoMemPermissions(void)
{
  return &zdoMem.permissions;
}
#endif

static inline ZdoAddrResolving_t * zdoMemAddrResolving(void)
{
  return &zdoMem.zdoAddrResolving;
}

static inline ZdoMgmtNwkUpdate_t * zdoMemMgmtNwkUpdate(void)
{
  return &zdoMem.mgmtNwkUpdate;
}

#endif // _ZDOMEM_H

