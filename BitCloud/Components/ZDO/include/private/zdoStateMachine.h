/**************************************************************************//**
  \file zdoStateMachine.h

  \brief Interface of global state machine of ZDO layer.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2009-10-28 A. Taradov - Created
    2012-10-31 Max Gekk - zdoManager.h was renamed to zdoStateMachine.h
                          Refactoring.
   Last change:
    $Id: zdoStateMachine.h 23429 2012-11-07 08:19:23Z mgekk $
******************************************************************************/
#if !defined _ZDO_STATE_MACHINE_H
#define _ZDO_STATE_MACHINE_H

/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <zdoMem.h>

/******************************************************************************
                                Types section
 ******************************************************************************/
/** Identifiers of modules that can be packed to groups. A combination of modules
   which activity is accepted forms global ZDO layer state. */
typedef enum _ZdoModuleId_t
{
  ZDO_START_NETWORK_MODULE_ID       = (1 << 0),
  ZDO_RESET_NETWORK_MODULE_ID       = (1 << 1),
  ZDO_LEAVE_NETWORK_MODULE_ID       = (1 << 2),
  ZDO_BINDING_MANAGER_MODULE_ID     = (1 << 3),
  ZDO_DISCOVERY_MANAGER_MODULE_ID   = (1 << 4),
  ZDO_NODE_MANAGER_MODULE_ID        = (1 << 5),
  ZDO_NWK_MANAGER_MODULE_ID         = (1 << 6),
  ZDO_POWER_MANAGER_MODULE_ID       = (1 << 7),
  ZDO_SECURITY_MANAGER_MODULE_ID    = (1 << 8),
  ZDO_ZDP_FRAME_PROCESSOR_MODULE_ID = (1 << 9),
  ZDO_ZDP_MANAGER_MODULE_ID         = (1 << 10),
  ZDO_NETWORK_STATUS_MODULE_ID      = (1 << 11),
  ZDO_ADDR_RESOLVING_MODULE_ID      = (1 << 12)
} ZdoModuleId_t;

/* Module permissions */
typedef enum _ZdoModuleSet_t
{
  ZDO_INITIAL_MODULE_SET        = 0,
  ZDO_ALL_DISABLED_MODULE_SET   = 0,
  ZDO_READY_MODULE_SET          = ZDO_START_NETWORK_MODULE_ID | ZDO_RESET_NETWORK_MODULE_ID |
                                  ZDO_POWER_MANAGER_MODULE_ID | ZDO_ZDP_MANAGER_MODULE_ID,
  ZDO_START_NETWORK_MODULE_SET  = ZDO_ZDP_MANAGER_MODULE_ID,
  ZDO_IN_NETWORK_MODULE_SET     = ZDO_BINDING_MANAGER_MODULE_ID | ZDO_DISCOVERY_MANAGER_MODULE_ID |
                                  ZDO_NODE_MANAGER_MODULE_ID | ZDO_NWK_MANAGER_MODULE_ID |
                                  ZDO_POWER_MANAGER_MODULE_ID | ZDO_SECURITY_MANAGER_MODULE_ID |
                                  ZDO_ZDP_FRAME_PROCESSOR_MODULE_ID | ZDO_ZDP_MANAGER_MODULE_ID |
                                  ZDO_NETWORK_STATUS_MODULE_ID | ZDO_RESET_NETWORK_MODULE_ID |
                                  ZDO_LEAVE_NETWORK_MODULE_ID | ZDO_ADDR_RESOLVING_MODULE_ID,
  ZDO_REJOIN_NETWORK_MODULE_SET = ZDO_ZDP_MANAGER_MODULE_ID,
  ZDO_LEAVING_MODULE_SET        = ZDO_LEAVE_NETWORK_MODULE_ID | ZDO_ZDP_MANAGER_MODULE_ID,
  ZDO_RESET_NETWORK_MODULE_SET  = 0,
  ZDO_REJOIN_DONE_MODULE_SET    = ZDO_ZDP_MANAGER_MODULE_ID | ZDO_NWK_MANAGER_MODULE_ID,
} ZdoModuleSet_t;

/******************************************************************************
                              Prototypes section
 ******************************************************************************/
/**************************************************************************//**
  \brief Enable certain modules

  \param[in] modules - modules to enable.
  \return None.
 ******************************************************************************/
INLINE void zdoEnable(uint16_t modules)
{
  zdoMem.enabledModules = modules;
}

/**************************************************************************//**
  \brief Check if certain module is enabled

  \param[in] module - module id to check.
  \return true  if the module is enabled otherwise false - the module is disabled.
 ******************************************************************************/
INLINE bool zdoEnabled(uint16_t module)
{
  return (zdoMem.enabledModules & module);
}

/**************************************************************************//**
  \brief Reset ZDO component
 ******************************************************************************/
ZDO_PRIVATE void zdoReset(void);

/**************************************************************************//**
  \brief Set 'Out of Network' flag for correct state restore after power failure.
 ******************************************************************************/
ZDO_PRIVATE void zdoSetOutOfNetworkFlag(void);

/**************************************************************************//**
  \brief Checks whether ZDO layer has any active transactions.
  \return true if any, false otherwise
 ******************************************************************************/
ZDO_PRIVATE bool zdoIsActiveTransaction(void);

#if defined _ENDDEVICE_ && defined _SLEEP_WHEN_IDLE_
/**************************************************************************//**
  \brief Checks whether ZDO layer has any active transactions that needs
    polling
  \return true if any, false otherwise
 ******************************************************************************/
ZDO_PRIVATE bool zdoIsPollRequired(void);
#endif

#endif /* _ZDO_STATE_MACHINE_H */
/** eof zdoStateMachine.h */
