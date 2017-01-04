/**************************************************************************//**
  \file pdsTimer.c

  \brief Persistent Data Server timer implementation.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2012-03-14 A. Razinkov  - Created.
   Last change:
    $Id: pdsTimer.c 19835 2012-01-24 16:51:08Z arazinkov $
 ******************************************************************************/

#if PDS_ENABLE_WEAR_LEVELING != 1
#ifdef _ENABLE_PERSISTENT_SERVER_
/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <pdsDataServer.h>
#include <stdPdsDbg.h>
#include <stdPdsMem.h>
#include <sysTaskManager.h>

/******************************************************************************
                    Types section
******************************************************************************/

/******************************************************************************
                    Prototypes section
******************************************************************************/
static void pdsTimerFired(void);

/******************************************************************************
                    Static variables section
******************************************************************************/

/******************************************************************************
                   Implementation section
******************************************************************************/
/******************************************************************************
\brief PDS timer initialization routine.
******************************************************************************/
void pdsTimerInit(void)
{
  pdsMemory()->periodicCommitTimer.mode = TIMER_REPEAT_MODE;
  pdsMemory()->periodicCommitTimer.callback = pdsTimerFired;
}

/***************************************************************************//**
\brief Enables periodic commitment of specified persistent items to a
       non-volatile storage.

\ingroup pds

\param[in] interval - period of time between successive commitments
\param[in] memoryId - an identifier of PDS file or directory to be stored
                      in non-volatile memory.
******************************************************************************/
void PDS_StoreByTimer(uint32_t interval, PDS_MemId_t memoryId)
{
  HAL_StopAppTimer(&pdsMemory()->periodicCommitTimer);

  /* Disable periodic commitment, if zero passed as interval */
  if (!interval)
  {
    pdsMemory()->memIdToStoreByTimer = PDS_NO_MEMORY_SPECIFIED;
    return;
  }

  /* Reconfigure and start periodic commitment */
  pdsMemory()->periodicCommitTimer.interval = interval;
  pdsMemory()->memIdToStoreByTimer = memoryId;
  HAL_StartAppTimer(&pdsMemory()->periodicCommitTimer);
}

/******************************************************************************
\brief PDS timer expiration callback.
******************************************************************************/
static void pdsTimerFired(void)
{
  PDS_Store(pdsMemory()->memIdToStoreByTimer);
}

/***************************************************************************//**
\brief Stops the Persistent Data Server. This doesn't affect a policy created
       through PDS_StoreByEvent() of  PDS_StoreByTimer() API.
       After calling PDS_Run() function PDS continues it's work, as before the
       stopping.
*******************************************************************************/
void PDS_Stop(void)
{
  HAL_StopAppTimer(&pdsMemory()->periodicCommitTimer);

  while (pdsMemory()->status & PDS_WRITING_INPROGRESS_FLAG)
  {
    SYS_ForceRunTask();
  }

  pdsMemory()->status |= PDS_STOPPED_FLAG;
}

/***************************************************************************//**
\brief Continue Persistent Data Server operation the same way, as before a
       stopping.
*******************************************************************************/
void PDS_Run(void)
{
  if (pdsMemory()->status & PDS_STOPPED_FLAG)
  {
    HAL_StartAppTimer(&pdsMemory()->periodicCommitTimer);
    pdsMemory()->status ^= PDS_STOPPED_FLAG;
  }
}

#endif /* _ENABLE_PERSISTENT_SERVER_ */
#endif /* PDS_ENABLE_WEAR_LEVELING != 1 */
/* eof pdsTimer.c */

