/**************************************************************************//**
  \file wlPdsTaskManager.h

  \brief PDS task manager interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    21.05.13 N. Fomin  - Created.
******************************************************************************/

#ifndef _WLPDSTASKMANAGER_H_
#define _WLPDSTASKMANAGER_H_
/******************************************************************************
                    Includes section
******************************************************************************/
#include <sysTypes.h>

/******************************************************************************
                    Types section
******************************************************************************/
/* PDS task identifiers */
typedef enum _PdsTaskId_t
{
  PDS_STORE_ITEM_TASK_ID,
  PDS_TASKS_AMOUNT
} PdsTaskId_t;

/******************************************************************************
                    Prototypes section
******************************************************************************/
/******************************************************************************
\brief Posts PDS task

\param[in] taskId - id of a task
******************************************************************************/
void pdsPostTask(PdsTaskId_t taskId);

#endif /* _WLPDSTASKMANAGER_H_ */
/* eof wlPdsTaskManager.h */

