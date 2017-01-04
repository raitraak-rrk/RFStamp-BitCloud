/************************************************************************//**
  \file zclTaskManager.c

  \brief
    Implementation of ZCL Task Manager

    The file implements the ZCL Task Manager

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    01.12.08 A. Potashov - Created.
******************************************************************************/
#if ZCL_SUPPORT == 1
/******************************************************************************
                   Includes section
******************************************************************************/
#include <zclTaskManager.h>
#include <sysTaskManager.h>
#include <zclDbg.h>
#include <sysAssert.h>

/******************************************************************************
                            Definitions section
 ******************************************************************************/
#define HANDLERS_GET(A, I) memcpy_P(A, &zclHandlers[I], sizeof(ZclTask_t))

/******************************************************************************
                              Types section
 ******************************************************************************/
/** Pointer to a ZCL task handler. */
typedef  void (* ZclTask_t)(void);

/******************************************************************************
                          Prototypes section
******************************************************************************/
extern void zclTaskHandler(void);
extern void zclParserTaskHandler(void);
extern void zclSecurityTaskHandler(void);

/******************************************************************************
                             Constants section
 ******************************************************************************/
/** Array of ZCL task handlers. */
static PROGMEM_DECLARE(ZclTask_t zclHandlers[ZCL_TASKS_SIZE]) =
{
  [ZCL_SUBTASK_ID]          = zclTaskHandler,
  [ZCL_PARSER_TASK_ID]      = zclParserTaskHandler,
  [ZCL_SECURITY_TASK_ID]    = zclSecurityTaskHandler,
};

/******************************************************************************
                          Global variables section
 ******************************************************************************/
static ZclTaskBitMask_t zclTaskBitMap;
static ZclTaskIdIter_t  zclCurrentTask;

/******************************************************************************
                           Implementations section
******************************************************************************/
/*****************************************************************************
  Task Manager post task function
*****************************************************************************/
void zclPostTask(ZclTaskId_t taskId)
{
  zclTaskBitMap |= (1U << taskId);
  SYS_PostTask(ZCL_TASK_ID);
}

/**************************************************************************//**
  \brief Global task handler of ZCL layer.
******************************************************************************/
void ZCL_TaskHandler(void)
{
  const ZclTaskIdIter_t last = zclCurrentTask;

  do
  {
    if (ZCL_TASKS_SIZE <= ++zclCurrentTask)
      zclCurrentTask = 0U;

    if (zclTaskBitMap & (1U << zclCurrentTask))
    {
      ZclTask_t handler;

      zclTaskBitMap ^= 1U << zclCurrentTask; //&=~
      HANDLERS_GET(&handler, zclCurrentTask);
      SYS_E_ASSERT_FATAL(handler, ZCLTASKMANAGER_TASKHANDLER_0);
      handler();
      break;
    }
  } while (last != zclCurrentTask);

  if (zclTaskBitMap)
    SYS_PostTask(ZCL_TASK_ID);
}
#endif // ZCL_SUPPORT == 1
//eof zclTaskManager.c
