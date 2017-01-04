/**************************************************************************//**
  \file zsiSerialController.h

  \brief Defines to abstract serial interfaces

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    12.09.2012  A. Khromykh - Created.
   Last change:
    $Id: zsiSerialInterface.h 24619 2013-03-15 09:49:47Z aluzhetskiy $
 ******************************************************************************/
#ifndef _ZSISERIALINTERFACE_H
#define _ZSISERIALINTERFACE_H

/******************************************************************************
                    Includes section
******************************************************************************/
#include <rs232Controller.h>
#if APP_ZAPPSI_INTERFACE == APP_INTERFACE_USBFIFO
  #include <usbFifoUsart.h>
#endif // APP_INTERFACE_USBFIFO

#if APP_ZAPPSI_INTERFACE == APP_INTERFACE_USART
  #include <usart.h>
#endif // APP_INTERFACE_USART

/******************************************************************************
                    Defines section
******************************************************************************/
#if APP_ZAPPSI_INTERFACE == APP_INTERFACE_USART
  INLINE int OPEN_ZAPPSI_INTERFACE(HAL_UsartDescriptor_t *descriptor)\
  {\
    BSP_EnableRs232();
    return HAL_OpenUsart(descriptor);\
  }
  INLINE int CLOSE_ZAPPSI_INTERFACE(HAL_UsartDescriptor_t *descriptor)\
  {\
    BSP_DisableRs232();
    return HAL_CloseUsart(descriptor);\
  }
  #define WRITE_ZAPPSI_INTERFACE           HAL_WriteUsart
  #define READ_ZAPPSI_INTERFACE            HAL_ReadUsart
  #define ZAPPSI_INTERFACE_TASK            HAL_TASK_USART
  #define HOLD_ADDITIONAL_ZAPPSI_INTERFACE_TASKS(TTY)  HAL_HoldOnOthersUsartTasks(TTY)
  #define RELEASE_ADDITIONAL_ZAPPSI_INTERFACE_TASKS()  HAL_ReleaseAllHeldUsartTasks()
#endif // APP_INTERFACE_USART

#if APP_ZAPPSI_INTERFACE == APP_INTERFACE_USBFIFO
  #define OPEN_ZAPPSI_INTERFACE            USBFIFO_OpenUsart
  #define CLOSE_ZAPPSI_INTERFACE           USBFIFO_CloseUsart
  #define WRITE_ZAPPSI_INTERFACE           USBFIFO_WriteUsart
  #define READ_ZAPPSI_INTERFACE            USBFIFO_ReadUsart
  #define ZAPPSI_INTERFACE_TASK            HAL_EXT_HANDLER
  #define HOLD_ADDITIONAL_ZAPPSI_INTERFACE_TASKS(TTY)
  #define RELEASE_ADDITIONAL_ZAPPSI_INTERFACE_TASKS()
#endif // APP_INTERFACE_USBFIFO
#endif // _ZSISERIALINTERFACE_H

// eof zsiSerialInterface.h
