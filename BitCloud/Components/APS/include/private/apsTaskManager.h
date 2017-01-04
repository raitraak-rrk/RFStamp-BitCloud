/**************************************************************************//**
  \file apsTaskManager.h

  \brief Header file of APS task manager.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2010-12-12 Max Gekk - Created.
   Last change:
    $Id: apsTaskManager.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#if !defined _APS_TASK_MANAGER_H
#define  _APS_TASK_MANAGER_H

/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <sysTypes.h>

/******************************************************************************
                                Types section
 ******************************************************************************/
/** Identifiers of APS task handlers. */
typedef enum  _ApsTaskID_t
{
  APS_TASK_STATE_MACHINE,
  APS_TASK_DATA,
  APS_TASK_SECURITY,
  APS_TASK_COMMAND_REQ,
  APS_TASK_AUTH_REQ,
  APS_TASK_SKKE_REQ,
  APS_TASK_SKKE_RESP,
  APS_TASK_TRANSPORT_KEY,
  APS_TASKS_SIZE
} ApsTaskID_t;

/** Bit array of APS task. If bit is '1' the task is posted. */
typedef uint8_t ApsTaskBitMask_t;

/** Type of iterator for task identifiers. */
typedef uint8_t ApsTaskIdIter_t;

/******************************************************************************
                               Prototypes section
 ******************************************************************************/
/******************************************************************************
  \brief post APS task.

  \param taskID - identifier of APS task.
  \return None.
 ******************************************************************************/
APS_PRIVATE void apsPostTask(const ApsTaskID_t taskID);

/******************************************************************************
  \brief Reset the APS task manager.
 ******************************************************************************/
APS_PRIVATE void apsResetTaskManager(void);

#endif /* _APS_TASK_MANAGER_H */
/** eof apsTaskManager.h */

