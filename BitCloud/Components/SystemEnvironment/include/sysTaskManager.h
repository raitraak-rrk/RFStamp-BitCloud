/*************************************************************************//**
  \file sysTaskManager.h

  \brief The header file describes the public stack Task Manager interface,
         task handlers and tasks IDs of Task Manager

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    15/05/07 E. Ivanov - Created
*****************************************************************************/

#ifndef _TASKMANAGER_H
#define _TASKMANAGER_H
#include <sysTypes.h>
#include <atomic.h>

/*! The list of task IDs. The IDs are sorted according to descending
priority. For each task ID there is the corresponding task handler function. */
typedef enum
{
  MAC_PHY_HWD_TASK_ID = 1 << 0, //!< Task ID of the MAC-PHY-HWD layer
  HAL_TASK_ID         = 1 << 1, //!< Task ID of HAL (Hardware Abstraction Layer)
  MAC_HWI_TASK_ID     = 1 << 2, //!< Task ID of the MAC-HWI layer (a part of MAC independent of radio)
  NWK_TASK_ID         = 1 << 3, //!< Task ID of the NWK layer
  ZDO_TASK_ID         = 1 << 4, //!< Task ID of the ZDO layer
  APS_TASK_ID         = 1 << 5, //!< Task ID of the APS layer
  SSP_TASK_ID         = 1 << 6, //!< Task ID of Security Service Provider
  TC_TASK_ID          = 1 << 7, //!< Task ID of the Trust Center component
  BSP_TASK_ID         = 1 << 8, //!< Task ID of the Border Support Package layer
  ZSI_TASK_ID         = 1 << 9, //!< Task ID of the ZAppSI component
  ZCL_TASK_ID         = 1 << 0x0A, //!< Task ID of ZigBee Cluster Library
  ZLL_TASK_ID         = 1 << 0x0B, //!< Service Task ID
  APL_TASK_ID         = 1 << 0x0C, //!< Task ID of the application
  PDS_TASK_ID         = 1 << 0x0D, //!< Task ID of the Persistent Data Server component
  ZGP_TASK_ID         = 1 << 0x0E, //!< Task ID of the ZGP layer
} SYS_TaskId_t;

/*! \brief Possible results of system's initialization */
typedef enum
{
  INIT_SUCCES    = 1 << 0, //!< Initialization is a success
  RF_NOT_RESPOND = 1 << 1, //!< Radio has not responded
} SYS_InitStatus_t;

/*! This function is called to process a user application task.
 The function should be defined in the user application.
 Its very first call is intended just for setting stack parameters.
 For starting a network a new task should be posted. */
extern void APL_TaskHandler(void);
// \cond internal
//! This function is called for processing HAL task. Should be defined in HAL
extern void HAL_TaskHandler(void);
//! This function is called for processing BSP task. Should be defined in BSP
extern void BSP_TaskHandler(void);
//! This function is called for processing MAC_PHY_HWD task. Should be defined in MAC_PHY_HWD
extern void MAC_PHY_HWD_TaskHandler(void);
//! This function is called for processing MAC_HWI task. Should be defined in MAC_HWI
extern void MAC_HWI_TaskHandler(void);
//! This function is called for processing NWK task. Should be defined in NWK
extern void NWK_TaskHandler(void);
//! This function is called for processing ZDO task. Should be defined in ZDO
extern void ZDO_TaskHandler(void);
//! This function is called for processing APS task. Should be defined in APS
extern void APS_TaskHandler(void);
//! This function is called for processing SSP task. Should be defined in SSP
extern void SSP_TaskHandler(void);
//! This function is called for processing Trust Center task. Should be defined in Trust Center
extern void TC_TaskHandler(void);
//! This function is called for processing ZAppSI task. Should be defined in ZSI
extern void ZSI_TaskHandler(void);
//! This function is called for processing ZCL task. Should be defined in ZCL
extern void ZCL_TaskHandler(void);
//! This function is called for processing ZLL task. Should be defined in ZLL
extern void ZLL_TaskHandler(void);
//! Initializes radio chip
extern bool RF_Init(void);
//! This function is called for processing PDS task. Should be defined in PDS
extern void PDS_TaskHandler(void);

//! This function is called for processing MAC_PHY_HWD task. Should be defined in MAC_PHY_HWD
extern void MAC_PHY_HWD_ForceTaskHandler(void);
//! This function is called for processing MAC_HWI task. Should be defined in MAC_HWI
extern void MAC_HWI_ForceTaskHandler(void);
//! This function is called for processing ZGP task. Should be defined in ZGP
extern void ZGP_TaskHandler(void);
/* The bitmask of posted task with bits' corresponding to particular stack layers.
A bit becomes set to 1 in the bitmask when the SYS_PostTask() is called with the
task ID of the corresponding layer. A bit is dropped after calling the layer's task
handler function to process the posted task. */
extern volatile uint16_t SYS_taskFlag;
// \endcond

/* Mask of enabled system tasks. Default value - 0xFFFF (All tasks are enabled). */
extern uint16_t SYS_taskMask;

/**************************************************************************************//**
\brief Posts a task to the task manager, which is later processed by the task
handler of the corresponding stack layer.

\ingroup sys

A task is processed when the SYS_RunTask() function.

\param[in] taskId - ID of the posted task. The application's ID is ::APL_TASK_ID.
*********************************************************************************************/
/*
IDs of the tasks are listed in the SYS_TaskId enum. Each task has its own priority and is called
only if there is no any task with higher priority. A handler is called when respective task can be run.
Each task has its own task handler. Correspondence between tasks and handlers is listed below:  \n
HAL - HAL_TaskHandler()                 \n
BSP - BSP_TaskHandler()                 \n
MAC_PHY_HWD - MAC_PHY_HWD_TaskHandler() \n
MAC_HWI - MAC_HWI_TaskHandler()         \n
ZGP - ZGP_TaskHandler()                 \n
NWK - NWK_TaskHandler()                 \n
ZDO - ZDO_TaskHandler()                 \n
APS - APS_TaskHandler()                 \n
APL - APL_TaskHandler()                 \n
*/
INLINE void SYS_PostTask(SYS_TaskId_t taskId)
{
//\cond internal
  ATOMIC_SECTION_ENTER
   SYS_taskFlag |= taskId;
  ATOMIC_SECTION_LEAVE
//\endcond
}

/***************************************************************************//**
\brief Disables specified task.

\param[in] taskId - task to be disabled.
*******************************************************************************/
INLINE void SYS_DisableTask(SYS_TaskId_t taskId)
{
  SYS_taskMask &= ~taskId;
}

/***************************************************************************//**
\brief Enables specified task.

\param[in] taskId - task to be enabled.
*******************************************************************************/
INLINE void SYS_EnableTask(SYS_TaskId_t taskId)
{
  SYS_taskMask |= taskId;
}

/**************************************************************************************//**
\brief  This function is called by the stack or from the \c main() function to process tasks.

If several tasks have been posted by the moment of the function's call, they are executed
in order of layers' priority: a task of the layer with the highest priority is executed first.

\ingroup sys
***************************************************************************************/
bool SYS_RunTask(void);

//\cond internal
/**************************************************************************************//**
\brief To force runTask to help making sync calls

\ingroup sys
******************************************************************************************/
void SYS_ForceRunTask(void);

/**************************************************************************************//**
\brief To force Pdshandler to help making sync calls

\ingroup sys
******************************************************************************************/
void SYS_ForcePdsHandler(void);
// \endcond

/**************************************************************************//**
\brief Initializes the microconroller and the radio chip

\ingroup sys

This is the first stack function to be called by the application in the \c main()
function.

\param none
\return Initialization result
******************************************************************************/
SYS_InitStatus_t SYS_SysInit(void);
#endif
