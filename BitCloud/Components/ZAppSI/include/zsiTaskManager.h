/******************************************************************************
  \file zsiTaskManager.h

  \brief
    ZAppSI task manager interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2010 , Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2011-08-26  A. Razinkov - Created.
   Last change:
    $Id: zsiTaskManager.h 24441 2013-02-07 12:02:29Z akhromykh $
******************************************************************************/

#ifndef _ZSITASKMANAGER_H_
#define _ZSITASKMANAGER_H_

/******************************************************************************
                    Includes section
******************************************************************************/
#include <sysTypes.h>

/******************************************************************************
                    Types section
******************************************************************************/

/* ZAppSI task identifiers */
typedef enum _ZsiTaskId_t
{
  ZSI_INIT_TASK_ID,
  ZSI_SERIAL_TASK_ID,
  ZSI_DRIVER_TASK_ID,
  ZSI_TASKS_AMOUNT,
} ZsiTaskId_t;

/* ZAppSI posted task bit mask */
typedef uint8_t ZsiTaskBitMask_t;
/* ZAppSI task handler */
typedef void (*ZsiTaskHandler_t)(void);
/* ZAppSI Task Manager description */
typedef struct _ZsiTaskManager_t
{
  ZsiTaskBitMask_t pendingTasks;
} ZsiTaskManager_t;
/******************************************************************************
                    Prototypes section
******************************************************************************/
/******************************************************************************
  \brief Post ZAppSI component task.

  \param[in] taskId - component task identifier.

  \return None.
 ******************************************************************************/
void zsiPostTask(ZsiTaskId_t taskId);

/******************************************************************************
  \brief Task manager reset routine.
 ******************************************************************************/
void zsiResetTaskManager(void);

#endif /* _ZSITASKMANAGER_H_ */
