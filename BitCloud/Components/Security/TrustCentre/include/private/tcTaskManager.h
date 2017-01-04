/**************************************************************************//**
  \file tcTaskManager.h

  \brief Security Trust Centre task manager header file.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      2007.12.25 - ALuzhetsky created.
******************************************************************************/
#ifndef _TCTASKMANAGER_H
#define _TCTASKMANAGER_H

/******************************************************************************
                            Includes section.
******************************************************************************/
#include <sysTypes.h>

/******************************************************************************
                             Types section.
******************************************************************************/
/**************************************************************************//**
  \brief TBD.

  TBD
******************************************************************************/
typedef enum
{
  //TC_TASK_RESET,
  TC_TASK_KEY_UPDATE,
#ifdef _LINK_SECURITY_
  TC_TASK_KEY_ESTABLISH,
#endif // _LINK_SECURITY_
  TC_TASK_REMOVE,
} tcTaskID_t;

/**************************************************************************//**
  \brief TBD.

  TBD
******************************************************************************/
typedef uint8_t TcTaskBitMask_t;

/**************************************************************************//**
  \brief TBD.

  TBD
******************************************************************************/
typedef struct
{
  TcTaskBitMask_t taskBitMask;
} TcTaskManagerMem_t;

/******************************************************************************
                        Functions prototypes section.
******************************************************************************/
/**************************************************************************//**
  \brief TBD.

  \param TBD.
  \return TBD.
******************************************************************************/
void tcPostTask(tcTaskID_t taskID);

/**************************************************************************//**
  \brief TBD.

  \param TBD.
  \return TBD.
******************************************************************************/
void tcResetTaskManager(void);

#endif // _TCTASKMANAGER_H

// eof tcTaskManager.h
