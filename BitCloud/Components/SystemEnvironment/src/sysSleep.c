/*****************************************************************************//**
  \file sysSleep.c

  \brief
    System sleep service.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    8/06/09 A. Khromykh - Created
**********************************************************************************/
/*********************************************************************************
                          Includes section.
**********************************************************************************/
#include <sysDbg.h>
#include <sysSleep.h>
#include <sysAssert.h>
#if defined(_SYS_LOG_ON_)
#include <usart.h>
#endif

/*********************************************************************************
                     External variables section
**********************************************************************************/
#if defined(_SYS_LOG_ON_)
  #if defined(_HAL_LOG_INTERFACE_UART0_) || defined(_HAL_LOG_INTERFACE_UART1_)
    extern HAL_UsartDescriptor_t sysUsartDescriptor;
  #endif
#endif

/*********************************************************************************
                     Global variables section
**********************************************************************************/
#if defined(_SYS_LOG_ON_)
  #if defined(_HAL_LOG_INTERFACE_UART0_) || defined(_HAL_LOG_INTERFACE_UART1_)
    HAL_Sleep_t localSleepParam;
  #endif
#endif

/*********************************************************************************
                            Implementation section.
**********************************************************************************/
#if defined(_SYS_LOG_ON_)
  #if defined(_HAL_LOG_INTERFACE_UART0_) || defined(_HAL_LOG_INTERFACE_UART1_)
    /**************************************************************************//**
    \brief Confirmation about the end of work log system.
    ******************************************************************************/
    static void sysLogTmtConfirm(void)
    {
      sysUsartDescriptor.txCallback = NULL;
      HAL_StartSystemSleep(&localSleepParam);
    }
  #endif
#endif


/**************************************************************************//**
\brief Prepares system for sleep.

\param[in]
  sleepParam - pointer to sleep structure.
******************************************************************************/
void SYS_Sleep(HAL_Sleep_t *sleepParam)
{
  int status;

  #if defined(_SYS_LOG_ON_)
    #if defined(_HAL_LOG_INTERFACE_UART0_) || defined(_HAL_LOG_INTERFACE_UART1_)
      if (1 == HAL_IsTxEmpty(&sysUsartDescriptor))
      {
        status = HAL_StartSystemSleep(sleepParam);
      }
      else
      {
        localSleepParam = *sleepParam;
        sysUsartDescriptor.txCallback = sysLogTmtConfirm;
      }
    #else
      status = HAL_StartSystemSleep(sleepParam);
    #endif
  #else
      status = HAL_StartSystemSleep(sleepParam);
  #endif

  if (status < 0)
  {
    SYS_E_ASSERT_WARN(false, SYS_ASSERT_ID_DEVICE_WAS_NOT_ABLE_TO_SLEEP);
  }
}
