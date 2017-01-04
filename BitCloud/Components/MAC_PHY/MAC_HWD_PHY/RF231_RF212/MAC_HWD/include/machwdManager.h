/******************************************************************************
  \file machwdManager.h

  \brief Manager's module prototypes and types.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    28/05/07 A. Mandychev - Created.
  Last change:
    $Id: machwdManager.h 27584 2015-01-09 14:45:42Z unithra.c $
******************************************************************************/

#ifndef _MACHWDMANAGER_H
#define _MACHWDMANAGER_H

/******************************************************************************
                        Includes section.
******************************************************************************/
#include <sysQueue.h>
#include <atomic.h>
#include <sysTaskManager.h>
#include <macDbg.h>
#include <halDiagnostic.h>
#include <machwdData.h>

/******************************************************************************
                           External variables section
******************************************************************************/
/** machwdQueueDescr is defined in machwdManager.c */
extern QueueDescriptor_t machwdQueueDescr;
/* Do not use it in other files. */
extern bool __wasPostTask;

/******************************************************************************
                        Prototypes section.
******************************************************************************/
/******************************************************************************
 TBD.
******************************************************************************/
void MAC_PHY_HWD_TaskHandler(void);

/**************************************************************************//**
  \brief Sends confirmation to the upper layer.
******************************************************************************/
void machwdSendConf2UpperLayer(void);

/**************************************************************************//**
  \brief Sends Data Indication to the upper layer.
******************************************************************************/
void machwdSendDataInd2UpperLayer(MACHWD_DataInd_t *indParams);

/******************************************************************************
                        Inline static functions prototypes section.
******************************************************************************/

/**************************************************************************//**
  \brief Returnes the head element of the MACHWD requests queue.

  \return The head element of the MACHWD requests queue.
 ******************************************************************************/
INLINE void* getHeadMachwdRequestsQueueElement(void)
{
  return getQueueElem(&machwdQueueDescr);
}

/******************************************************************************
 \brief Posts the MACHWD task.
******************************************************************************/
INLINE void MACHWD_PostTask(void)
{
  ATOMIC_SECTION_ENTER
  if (!__wasPostTask)
  {
    SYS_PostTask(MAC_PHY_HWD_TASK_ID);
    __wasPostTask = true;
  }
  ATOMIC_SECTION_LEAVE
}
#endif /* _MACHWDMANAGER_H */

// eof machwdManager.h
