/**************************************************************************//**
  \file zdoTaskManager.h

  \brief The header file describes the constants, types and internal interface
         of ZDO task manager.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2012-11-06 Max Gekk - Created.
   Last change:
    $Id: zdoTaskManager.h 23844 2012-12-13 13:58:00Z mgekk $
 ******************************************************************************/
#if !defined _ZDO_TASK_MANAGER_H
#define  _ZDO_TASK_MANAGER_H

/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <sysTypes.h>

/******************************************************************************
                                Types section
 ******************************************************************************/
/** Identifiers of ZDO task handlers. */
typedef enum  _ZdoTaskID_t
{
  ZDO_SECURITYMANAGER_TASK_ID,
  ZDO_ZDP_MANAGER_TASK_ID,
  ZDO_POWER_MANAGER_TASK_ID,
  ZDO_START_NETWORK_TASK_ID,
  ZDO_ADDRESS_RESOLVING_TASK_ID,
  ZDO_INIT_TASK_ID,
  ZDO_TASKS_SIZE
} ZdoTaskID_t;

/** Bit array of ZDO task. If bit is '1' the task is posted. */
typedef uint8_t ZdoTaskBitMask_t;

/** Type of iterator for task identifiers. */
typedef uint8_t ZdoTaskIdIter_t;

/******************************************************************************
                               Prototypes section
 ******************************************************************************/
/******************************************************************************
  \brief Post ZDO task.

  \param taskID - identifier of ZDO task.
  \return None.
 ******************************************************************************/
ZDO_PRIVATE void zdoPostTask(const ZdoTaskID_t taskID);

/******************************************************************************
  \brief Remove ZDO subtask from list of posted tasks.

  \param[in] taskID - identifier of a task which must be unpost.
  \return None.
 ******************************************************************************/
ZDO_PRIVATE void zdoUnpostTask(ZdoTaskID_t taskID);

#endif /* _ZDO_TASK_MANAGER_H */
/** eof zdoTaskManager.h */
