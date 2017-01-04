/*****************************************************************************
  \file  sysDbg.h

  \brief

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    13.10.2009 ivagulin - Created
******************************************************************************/


#ifndef _SYSDBG_H_
#define _SYSDBG_H_

#include <dbg.h>

typedef enum
{
  SYS_ASSERT_ID_DOUBLE_QUEUE_PUT             = 0x8000,
  SYS_ASSERT_ID_DRT_SIZE_TOO_BIG             = 0x8001,
  SYS_ASSERT_ID_DEVICE_WAS_NOT_ABLE_TO_SLEEP = 0x8002,
  SYS_ASSERT_WRONG_EVENT_SUBSCRIBE           = 0x8003,
  SYS_ASSERT_WRONG_EVENT_POST                = 0x8004,
  SYS_POSTEVENT_NULLCALLBACK0                = 0x8005,
  SYS_TASKHANDLER_NULLCALLBACK0              = 0x8006,
  SYSMUTEX_MUTEXLOCK0                        = 0x8007,
  SYSMUTEX_MUTEXUNLOCK0                      = 0x8008,
  SYSMUTEX_MUTEXUNLOCK1                      = 0x8009,
  SYSMUTEX_MUTEXUNLOCK2                      = 0x800A,
  SYSMUTEX_MUTEXUNLOCK3                      = 0x800B,
  SYSMUTEX_ISMUTEXLOCKED0                    = 0x800C
} SysAssertId_t;

#endif /* _SYSDBG_H_ */
