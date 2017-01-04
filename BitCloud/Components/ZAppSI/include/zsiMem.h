/******************************************************************************
  \file zsiMem.h

  \brief
    ZAppSI memory interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2010 , Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2011-08-26  A. Razinkov - Created.
   Last change:
    $Id: zsiMem.h 24441 2013-02-07 12:02:29Z akhromykh $
******************************************************************************/

#ifndef _ZSIMEM_H_
#define _ZSIMEM_H_

/******************************************************************************
                    Includes section
******************************************************************************/
#include <sysTypes.h>
#include <zsiTaskManager.h>
#include <zsiMemoryManager.h>
#include <zsiSerialController.h>
#include <zsiDriver.h>

/******************************************************************************
                    Types section
******************************************************************************/
typedef struct _ZsiMem_t
{
  ZsiTaskManager_t      taskManager;
  ZsiMemoryManager_t    memoryManager;
  ZsiSerialController_t serialController;
  ZsiDriver_t           driver;
} ZsiMem_t;

/*****************************************************************************
                    External variables section
******************************************************************************/
extern ZsiMem_t zsiMem;

/******************************************************************************
                    Global functions section
******************************************************************************/
/******************************************************************************
  \brief ZAppSI task manager component getter routine.

  \return Pointer to ZAppSI task manager component.
 ******************************************************************************/
INLINE ZsiTaskManager_t *zsiTaskManager(void)
{
  return &zsiMem.taskManager;
}

/******************************************************************************
  \brief ZAppSI memory manager component getter routine.

  \return Pointer to ZAppSI memory manager component.
 ******************************************************************************/
INLINE ZsiMemoryManager_t *zsiMemoryManager(void)
{
  return &zsiMem.memoryManager;
}

/******************************************************************************
  \brief ZAppSI serial controller component getter routine.

  \return Pointer to ZAppSI serial controller component.
 ******************************************************************************/
INLINE ZsiSerialController_t *zsiSerial(void)
{
  return &zsiMem.serialController;
}

/******************************************************************************
  \brief ZAppSI driver component getter routine.

  \return Pointer to ZAppSI driver component.
 ******************************************************************************/
INLINE ZsiDriver_t *zsiDriver(void)
{
  return &zsiMem.driver;
}

#endif /* _ZSIMEM_H_ */
