/***************************************************************************//**
  \file zsiTaskManager.c

  \brief ZAppSI task manager implementation

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2011-08-26  A. Razinkov - Created.
   Last change:
    $Id: zsiTaskManager.c 24441 2013-02-07 12:02:29Z akhromykh $
 ******************************************************************************/

/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <sysTaskManager.h>
#include <zsiMem.h>
#include <zsiInit.h>

/*****************************************************************************
                              Local variables section
******************************************************************************/
ZsiTaskHandler_t zsiTaskHandlers[ZSI_TASKS_AMOUNT] =
{
  zsiInitTaskHandler,   /* ZSI_INIT_TASK_ID */
  zsiSerialTaskHandler, /* ZSI_SERIAL_TASK_ID */
  zsiDriverTaskHandler, /* ZSI_DRIVER_TASK_ID */
};

/******************************************************************************
                              Implementations section
******************************************************************************/
/******************************************************************************
  \brief Task manager reset routine.
 ******************************************************************************/
void zsiResetTaskManager(void)
{
  zsiTaskManager()->pendingTasks = 0U;
}

/******************************************************************************
  \brief Post ZAppSI task.
 ******************************************************************************/
void zsiPostTask(ZsiTaskId_t taskId)
{
  zsiTaskManager()->pendingTasks |= (1U << taskId);
  SYS_PostTask(ZSI_TASK_ID);
}

/******************************************************************************
  \brief General ZAppSI task handler.
 ******************************************************************************/
void ZSI_TaskHandler(void)
{
  ZsiTaskManager_t *manager = zsiTaskManager();

  if (manager->pendingTasks)
  {
    uint8_t taskId = 0U;

    while (!(manager->pendingTasks & (1 << taskId)))
      taskId++;

    manager->pendingTasks ^= (1 << taskId);
    zsiTaskHandlers[taskId]();
    if (manager->pendingTasks)
      SYS_PostTask(ZSI_TASK_ID);
  }
}

/* eof zsiTaskManager.c */
