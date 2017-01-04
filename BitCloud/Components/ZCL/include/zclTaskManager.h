/************************************************************************//**
  \file zclTaskManager.h

  \brief
    The header file describes the public interface of ZCL Task Manager

    The file describes the public interface and types of ZCL Task Manager

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTMs).

  \internal
    History:
    01.12.08 A. Poptashov - Created.
******************************************************************************/

#ifndef _ZCLTASKMANAGER_H
#define _ZCLTASKMANAGER_H

/******************************************************************************
                   Includes section
******************************************************************************/
#include "sysTypes.h"

/******************************************************************************
                   Define(s) section
******************************************************************************/

/******************************************************************************
                   Types section
******************************************************************************/
/** Identifiers of ZCL task handlers. */
typedef enum _ZclTaskId_t
{
  ZCL_SUBTASK_ID,
  ZCL_PARSER_TASK_ID,
  ZCL_SECURITY_TASK_ID,
  ZCL_TASKS_SIZE
} ZclTaskId_t;

/** Bit array of ZCL task. If bit is '1' the task is posted. */
typedef uint8_t ZclTaskBitMask_t;

/** Type of iterator for task identifiers. */
typedef uint8_t ZclTaskIdIter_t;

/******************************************************************************
                   External variables section
******************************************************************************/

/******************************************************************************
                   Prototypes section
******************************************************************************/
/*************************************************************************//**
  \brief ZCL Task Manager post task function
  \param[in] taskId - Determines the task Id to be posted (ZclTaskId_t should be used)
*****************************************************************************/
void zclPostTask(ZclTaskId_t taskId);

/*************************************************************************//**
  \brief Task Manager task handler function
*****************************************************************************/
void ZCL_TaskHandler(void);

#endif  //#ifndef _ZCLTASKMANAGER_H

//eof zclTaskManager.h
