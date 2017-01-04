/**************************************************************************//**
  \file nwkTaskManager.h

  \brief Header file of NWK task manager.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2007-06-14 V. Panov - Created.
    2008-08-21 Max Gekk - More fair planning of tasks.
    2009-06-12 Max Gekk - Refactoring.
   Last change:
    $Id: nwkTaskManager.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#if !defined _NWK_TASK_MANAGER_H
#define _NWK_TASK_MANAGER_H

/******************************************************************************
                             Includes section
 ******************************************************************************/
#include <sysTypes.h>

/******************************************************************************
                                Types section
 ******************************************************************************/
/** Identifiers of NWK task handlers. */
typedef enum _NwkTaskID_t
{
  NWK_TASK_RESET,
  NWK_TASK_SYNC,
  NWK_TASK_ADDR_CONFLICT,
  NWK_TASK_TX,
  NWK_TASK_ROUTE_DISCOVERY,
  NWK_TASK_ROUTE_DISCOVERY_TABLE,
  NWK_TASK_START_ROUTER,
  NWK_TASK_PERMIT_JOINING,
  NWK_TASK_DISCOVERY_REQ,
  NWK_TASK_FORMATION_REQ,
  NWK_TASK_JOIN_REQ,
  NWK_TASK_JOIN_IND,
  NWK_TASK_LEAVE,
  NWK_TASK_EDSCAN,
  NWK_TASK_DATA_CONF,
  NWK_TASK_LOOPBACK,
  NWK_TASK_ROUTE_RECORD,
  NWK_TASK_DIRECT_JOIN,
  NWK_TASK_ORPHAN,
  NWK_TASK_SILENT_JOIN,
  NWK_TASK_MANYTOONE,
  NWK_TASK_ALLOC_DATA_REQ,
  NWK_TASKS_SIZE
} NwkTaskID_t;

/** Bit array of NWK task. If bit is '1' the task is posted. */
typedef uint32_t NwkTaskBitMask_t;

/** Type of iterator for task identifiers. */
typedef uint8_t NwkTaskIdIter_t;

typedef struct _NwkTaskManager_t
{
  NwkTaskBitMask_t bitMap;
  NwkTaskIdIter_t current;
} NwkTaskManager_t;

/******************************************************************************
                              Prototypes section
 ******************************************************************************/
/******************************************************************************
  \brief post NWK task.

  \param taskID - identifier of NWK task.
  \return None.
 ******************************************************************************/
NWK_PRIVATE void nwkPostTask(const NwkTaskID_t taskID);

/******************************************************************************
  \brief Cancel NWK task.

  \param[in] mask - bit mask of NWK task dentifier.
  \return None.
 ******************************************************************************/
NWK_PRIVATE void nwkClearTaskBit(const NwkTaskBitMask_t mask);

/******************************************************************************
  \brief Reset the task manager.
 ******************************************************************************/
NWK_PRIVATE void nwkResetTaskManager(void);

#endif /* _NWK_TASK_MANAGER_H */
/** eof nwkTaskManager.h */

