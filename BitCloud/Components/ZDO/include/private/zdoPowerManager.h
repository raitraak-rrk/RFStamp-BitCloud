/*************************************************************************//**
  \file zdoPowerManager.h

  \brief The header file describes the constants, types and internal interface
  of ZDO power manager

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
******************************************************************************/

#ifndef _ZDOPOWERMANAGER_H
#define _ZDOPOWERMANAGER_H

/******************************************************************************
                        Includes section
******************************************************************************/
#include <nwk.h>
#include <zdo.h>

/******************************************************************************
                        Types section
******************************************************************************/
typedef struct _ZdoPowerManager_t
{
  ZDO_SleepReq_t         *sleepReq;
  ZDO_WakeUpReq_t        *wakeUpReq;
  bool                   sleeping;
  MAC_RxEnableReq_t      rxReq;
  bool                   rxOnWhenIdle;
#ifdef _ENDDEVICE_
  NWK_SyncReq_t          syncReq;
  HAL_AppTimer_t         syncTimer;
  uint8_t                syncFailCounter;
  bool                   waitingSyncConf;
  bool                   polling;
#if defined(_SLEEP_WHEN_IDLE_)
  uint32_t               lastPollTime;
#endif /* _SLEEP_WHEN_IDLE_ */
#endif // _ENDDEVICE_
} ZdoPowerManager_t;

/******************************************************************************
                        Prototypes section
******************************************************************************/

/**************************************************************************//**
  \brief Reset power manager
******************************************************************************/
ZDO_PRIVATE void zdoPowerManagerReset(void);

/******************************************************************************
  \brief Power Manager module task handler

    Note: always post ZDO_POWER_MANAGER_TASK_ID after performing actions that
    may result in device ability to sleep.
******************************************************************************/
ZDO_PRIVATE void zdoPowerManagerHandler(void);

#endif // _ZDOPOWERMANAGER_H_

// eof zdoPowerManager.h
