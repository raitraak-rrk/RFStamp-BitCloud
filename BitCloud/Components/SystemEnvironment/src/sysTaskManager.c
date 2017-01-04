/**************************************************************************//**
  \file sysTaskManager.c

  \brief

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    15/05/07 E. Ivanov - Created
******************************************************************************/
/******************************************************************************
                             Includes section
******************************************************************************/
#include <sysTaskManager.h>
#include <sysDbg.h>
#include <sysSleep.h>
#include <sysEvents.h>
#include <sysIdleHandler.h>
#include <sysAssert.h>

#if defined(_USE_KF_MAC_)
#include <mac_api.h>
#else  // defined(_USE_KF_MAC_)
#include <halInit.h>
#endif // defined(_USE_KF_MAC_)

#ifdef SIMULATOR
  #include <halMedium.h>
#endif
#ifdef ZAPPSI_HOST
#include <zsiMem.h>
#include <sysInit.h>
#endif
/******************************************************************************
                             Prototypes section
******************************************************************************/
static void processManagerTask(void);

/******************************************************************************
                             External variables section
******************************************************************************/
volatile uint16_t SYS_taskFlag = 0;
uint16_t SYS_taskMask = 0xFFFF;

/******************************************************************************
                             Static variables section
******************************************************************************/
static void (*taskHandlers[])() = {
#if defined(_SYS_MAC_PHY_HWD_TASK_) && !defined(ZAPPSI_HOST)
  MAC_PHY_HWD_TaskHandler,
#else
  NULL,
#endif

#ifdef _SYS_HAL_TASK_
  HAL_TaskHandler,
#else
  NULL,
#endif

#if defined(_SYS_MAC_HWI_TASK_) && !defined(ZAPPSI_HOST)
  MAC_HWI_TaskHandler,
#else
  NULL,
#endif

#if defined(_SYS_NWK_TASK_) && !defined(ZAPPSI_HOST)
  NWK_TaskHandler,
#else
  NULL,
#endif

#if defined(_SYS_ZDO_TASK_) && !defined(ZAPPSI_HOST)
  ZDO_TaskHandler,
#else
  NULL,
#endif

#if defined(_SYS_APS_TASK_) && !defined(ZAPPSI_HOST)
  APS_TaskHandler,
#else
  NULL,
#endif

#if defined(_SYS_SSP_TASK_) && !defined(ZAPPSI_HOST)
  SSP_TaskHandler,
#else
  NULL,
#endif

#if defined(_SYS_TC_TASK_) && !defined(ZAPPSI_HOST)
  TC_TaskHandler,
#else
  NULL,
#endif

#ifdef _SYS_BSP_TASK_
  BSP_TaskHandler,
#else
  NULL,
#endif

#if defined(ZAPPSI_NP) || defined(ZAPPSI_HOST)
  ZSI_TaskHandler,
#else
  NULL,
#endif

#if ZCL_SUPPORT == 1
  ZCL_TaskHandler,
#else
  NULL,
#endif

#ifdef _SYS_ZLL_TASK_
  ZLL_TaskHandler,
#else
  NULL,
#endif

#ifdef _SYS_APL_TASK_
  APL_TaskHandler,
#else
  NULL,
#endif

#ifdef _SYS_PDS_TASK_
  PDS_TaskHandler,
#else
  NULL,
#endif

#ifdef _SYS_ZGP_TASK_
  ZGP_TaskHandler,
#else
  NULL,
#endif
};

/******************************************************************************
                             Implementation section
******************************************************************************/
/***************************************************************************//**
\brief System initialization routine
*******************************************************************************/
SYS_InitStatus_t SYS_SysInit(void)
{
#if defined(_USE_KF_MAC_)
  /* Initialize the MAC layer and its underlying layers, like PAL, TAL, BMM. */
  if (wpan_init() != MAC_SUCCESS)
  {
      /*
       * Stay here; we need a valid IEEE address.
       * Check kit documentation how to create an IEEE address
       * and to store it into the EEPROM.
       */
      pal_alert();
  }
#else // defined(_USE_KF_MAC_)

  HAL_Init();    // It must be first
  SYS_InitLog(); // It initializes the system logger
#if defined(_SYS_MAC_PHY_HWD_TASK_) && !defined(ZAPPSI_HOST)
  if (!RF_Init())     // It must be after HAL_Init()
  {
    return RF_NOT_RESPOND;
  }
#endif // _SYS_MAC_PHY_HWD_TASK_

#endif // defined(_USE_KF_MAC_)

#if defined(_SYS_ZDO_TASK_) && !defined(ZAPPSI_HOST)
  SYS_PostTask(ZDO_TASK_ID); // ZDO task must be started first to initialize the stack
#elif defined(ZAPPSI_HOST)
  SYS_InitZsiMem();
  SYS_PostTask(ZSI_TASK_ID);
#elif defined(_SYS_ZGP_TASK_)
  SYS_PostTask(ZGP_TASK_ID);
#else
  SYS_PostTask(APL_TASK_ID); // If there isn't ZDO e.g. HAL SE target is used
#endif /* _SYS_ZDO_TAKS_ */

#ifdef SIMULATOR
  HAL_MediumStart();
#endif
  return INIT_SUCCES;
}

/***************************************************************************//**
\brief General BitCloud task manager entry point
*******************************************************************************/
bool SYS_RunTask(void)
{
  processManagerTask();
  SYS_PostEvent(SYS_EVENT_TASK_PROCESSED, 0);
  // enter the mcu's idle mode
  if (0 == SYS_taskFlag)
    SYS_IdleHandler();

  return (SYS_taskFlag != 0);
}

/***************************************************************************//**
\brief Force task manager procedure
*******************************************************************************/
void SYS_ForceRunTask(void)
{
  processManagerTask();
}

/***************************************************************************//**
\brief Force PDS handler procedure
*******************************************************************************/
void SYS_ForcePdsHandler(void)
{
#if defined(_SYS_MAC_PHY_HWD_TASK_) && !defined(ZAPPSI_HOST)
  MAC_PHY_HWD_ForceTaskHandler();
#endif

#if defined(_SYS_MAC_HWI_TASK_) && !defined(ZAPPSI_HOST)
  MAC_HWI_ForceTaskHandler();
#endif

#if defined(_SYS_SSP_TASK_) && !defined(ZAPPSI_HOST)
  SSP_TaskHandler();
#endif

#ifdef _SYS_PDS_TASK_
  PDS_TaskHandler();
#endif

  SYS_PostEvent(SYS_EVENT_TASK_PROCESSED, 0);
}

/***************************************************************************//**
\brief Task processing handler
*******************************************************************************/
static void processManagerTask(void)
{
  uint8_t taskId;

  for (taskId = 0; taskId < (sizeof(taskHandlers) / sizeof(taskHandlers[0])); taskId++)
  {
#if defined(_USE_KF_MAC_)
    wpan_task();
#endif // defined(_USE_KF_MAC_)
    SYS_INFINITY_LOOP_MONITORING
    if ((1 << taskId) & (SYS_taskFlag & SYS_taskMask))
    {
      ATOMIC_SECTION_ENTER
        SYS_taskFlag &= ~(1 << taskId);
      ATOMIC_SECTION_LEAVE

      SYS_E_ASSERT_FATAL(taskHandlers[taskId], SYS_TASKHANDLER_NULLCALLBACK0);
      taskHandlers[taskId]();

      break;
    }
  }
}

#if defined(_USE_KF_MAC_)
/***************************************************************************//**
\brief Stub for BSP task handler.
*******************************************************************************/
void BSP_TaskHandler(void){}
#endif // defined(_USE_KF_MAC_)

/* eof sysTaskManager.c */
