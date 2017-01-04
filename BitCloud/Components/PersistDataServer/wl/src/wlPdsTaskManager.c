/**************************************************************************//**
  \file wlPdsTaskManager.c

  \brief PDS task manager implementation.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    21.05.13 A. Fomin - Created.
******************************************************************************/

#ifdef _ENABLE_PERSISTENT_SERVER_
#if PDS_ENABLE_WEAR_LEVELING == 1
/******************************************************************************
                               Includes section
******************************************************************************/
#include <sysTaskManager.h>
#include <wlPdsTaskManager.h>

/******************************************************************************
                    Types section
******************************************************************************/
typedef uint8_t PdsTaskBitMask_t;
typedef void (*PdsTaskHandler_t)(void);

/******************************************************************************
                    Prototypes section
******************************************************************************/
void pdsStoreItemTaskHandler(void);

/******************************************************************************
                    Static variables section
******************************************************************************/
static PdsTaskHandler_t pdsTaskHandlers[PDS_TASKS_AMOUNT] =
{
  [PDS_STORE_ITEM_TASK_ID] = pdsStoreItemTaskHandler,
};

static PdsTaskBitMask_t pendingTasks;

/******************************************************************************
                   Implementation section
******************************************************************************/
/******************************************************************************
\brief Posts PDS task

\param[in] taskId - id of a task
******************************************************************************/
void pdsPostTask(PdsTaskId_t taskId)
{
  pendingTasks |= (1U << taskId);
  SYS_PostTask(PDS_TASK_ID);
}

/******************************************************************************
\brief General PDS task handler
******************************************************************************/
void PDS_TaskHandler(void)
{
  if (pendingTasks)
  {
    uint8_t taskId = 0U;

    while (!(pendingTasks & (1U << taskId)))
      taskId++;

    pendingTasks ^= (1U << taskId);
    pdsTaskHandlers[taskId]();
    if (pendingTasks)
      SYS_PostTask(PDS_TASK_ID);
  }
}

#endif /* _ENABLE_PERSISTENT_SERVER_ */
#endif /* PDS_ENABLE_WEAR_LEVELING == 1 */
/* eof wlPdsTaskManager.c */

