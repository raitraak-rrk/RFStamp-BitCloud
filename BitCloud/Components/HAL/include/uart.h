/************************************************************************//**
  \file uart.h

  \brief
    UART interface

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    17.11.2010 D. Loskutnikov - Created.
******************************************************************************/
#ifndef _UART_H
#define _UART_H

/******************************************************************************
                        Includes section
******************************************************************************/
#include <usart.h>
#include <halUart.h>

/******************************************************************************
                    Functions prototypes section
******************************************************************************/
/**
\defgroup hal_uart HAL UART controlling functions

\ingroup hal

\brief Functions used to control UART

These functions are available only for those platforms where there is a separate
interface that cannot be used for communication over USART (synchronous receive-transmit)
but for UART (asynchronous receive-transmit) only.
*/
/**************************************************************************//**
\brief Open UART

\ingroup hal_uart

\param[in] desc UART descriptor
\return 1 if success, -1 if error
******************************************************************************/
int HAL_OpenUart(HAL_UartDescriptor_t *descriptor);

/**************************************************************************//**
\brief Close UART

\ingroup hal_uart

\param[in] desc UART descriptor
\return
******************************************************************************/
int HAL_CloseUart(HAL_UartDescriptor_t *descriptor);

/**************************************************************************//**
\brief Send contents of buffer over UART

\ingroup hal_uart

\param[in] desc UART descriptor
\param[in] buffer buffer to be sent
\param[in] length buffer length
\return number of sent bytes or -1 if error
******************************************************************************/
int HAL_WriteUart(HAL_UartDescriptor_t *descriptor, uint8_t *buffer, uint16_t length);

/**************************************************************************//**
\brief Copy received by UART bytes to user-supplied buffer

\ingroup hal_uart

\param[in] desc UART descriptor
\param[in] buffer buffer to store data
\param[in] length maximum length of buffer
\return number of actually copied bytes or -1 if error
******************************************************************************/
int HAL_ReadUart(HAL_UartDescriptor_t *descriptor, uint8_t *buffer, uint16_t length);

/**************************************************************************//**
\brief Check if any bytes are pending for transmission over UART

\ingroup hal_uart

\param[in] desc descriptor
\return -1 if error, 0 if not, 1 if yes
******************************************************************************/
int HAL_IsUartTxEmpty(HAL_UartDescriptor_t *descriptor);

/**************************************************************************//**
\brief Store UART error conditions

\param[in] desc UART descriptor
\param[in] err error condition
******************************************************************************/
void halUartStoreError(HAL_UartDescriptor_t *desc, uint8_t err);

/**************************************************************************//**
\brief Send byte from FIFO over UART

\param[in] desc UART descriptor
******************************************************************************/
void halUartTx(HAL_UartDescriptor_t *desc);

/**************************************************************************//**
\brief Store received byte to FIFO

\param[in] desc UART descriptor
******************************************************************************/
void halUartStoreRx(HAL_UartDescriptor_t *desc);

/**************************************************************************//**
\brief UART transmission complete handler

\param[in] desc UART descriptor
******************************************************************************/
void halSigUartTransmissionComplete(HAL_UartDescriptor_t *desc);

/**************************************************************************//**
\brief UART reception complete handler

\param[in] desc UART descriptor
******************************************************************************/
void halSigUartReceptionComplete(HAL_UartDescriptor_t *desc);

/**************************************************************************//**
\brief UART error handler

\param[in] desc UART descriptor
******************************************************************************/
void halSigUartErrorOccurred(HAL_UartDescriptor_t *desc);

#endif /* _UART_H */
// eof uart.h
