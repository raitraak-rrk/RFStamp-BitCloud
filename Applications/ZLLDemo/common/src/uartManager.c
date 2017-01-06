/************************************************************************//**
  \file uartManager.c

  \brief
    ZclDevice: Uart Manager implementation.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    05.02.09 I. Fedina - Created.
******************************************************************************/
#if (APP_DEVICE_EVENTS_LOGGING == 1) || (APP_ENABLE_CONSOLE == 1)

/*******************************************************************************
                             Includes section
*******************************************************************************/
#include <uartManager.h>
#include <console.h>
#include <stdarg.h>
#include <stdio.h>
#include <rs232Controller.h>

/*******************************************************************************
                             Definitions section
*******************************************************************************/
#define TMP_STRING_BUFFER_SIZE              100

// UART Tx buffer size
#ifndef APP_UART_TX_BUFFER_SIZE
    #define APP_UART_TX_BUFFER_SIZE           300 // Based on the no. of strings that will be printed in synchronous
#endif

/*******************************************************************************
                             Local variables section
*******************************************************************************/

#if APP_INTERFACE == APP_INTERFACE_USBCDC
  typedef HAL_UsbHWDescriptor_t HAL_Descriptor_t;
   
  #if defined (USB_TEXT_MODE_SUPPORT)
    static uint8_t uartRxBuffer[USART_RX_BUFFER_LENGTH];  // UART Rx buffer
  #endif
#else
  typedef  HAL_UsartDescriptor_t HAL_Descriptor_t;
#endif

static uint8_t uartTxBuffer[APP_UART_TX_BUFFER_SIZE]; // UART Tx buffer
static uint8_t uartRxBuffer[USART_RX_BUFFER_LENGTH];  // UART Rx buffer
static HAL_Descriptor_t appDescriptor;

/*******************************************************************************
                             Prototypes section
*******************************************************************************/
#if APP_INTERFACE == APP_INTERFACE_USBCDC
  static void uartDataReceived(HAL_UsbRxIndCallback_t *);
#else
  static void uartDataReceived(uint16_t length);
#endif

/*******************************************************************************
                             Implementation section
*******************************************************************************/
/**************************************************************************//**
/brief Inits UART, register UART callbacks
*******************************************************************************/
void uartInit(void)
{
#if  APP_INTERFACE == APP_INTERFACE_USBCDC
#if  BSP_SUPPORT == BOARD_SAMR21_XPRO
  appDescriptor.usbVbusDetect = true;
#else
  appDescriptor.usbVbusDetect = false;
#endif
  appDescriptor.speed = USB_SPEED_FULL;
  appDescriptor.txBuffer        = uartTxBuffer;
  appDescriptor.txBufferLength  = APP_UART_TX_BUFFER_SIZE;;
  appDescriptor.txCallback      = NULL;
  appDescriptor.rxBuffer = uartRxBuffer;
  appDescriptor.rxBufferLength = USART_RX_BUFFER_LENGTH;
  appDescriptor.rxCallback = uartDataReceived;
  #if defined (USB_TEXT_MODE_SUPPORT)
    appDescriptor.oprMode = USB_TEXT_MODE;
  #else
    appDescriptor.oprMode = USB_BINARY_MODE;
  #endif
  OPEN_USART(&appDescriptor);
#else
appDescriptor.tty             = USART_CHANNEL;
appDescriptor.mode            = USART_MODE_ASYNC;
appDescriptor.baudrate        = USART_BAUDRATE_38400;
appDescriptor.dataLength      = USART_DATA8;
appDescriptor.parity          = USART_PARITY_NONE;
appDescriptor.stopbits        = USART_STOPBIT_1;
appDescriptor.rxBuffer        = uartRxBuffer;
appDescriptor.rxBufferLength  = USART_RX_BUFFER_LENGTH;
appDescriptor.txBuffer        = uartTxBuffer;
appDescriptor.txBufferLength  = APP_UART_TX_BUFFER_SIZE;
appDescriptor.rxCallback      = uartDataReceived;
appDescriptor.txCallback      = NULL;
appDescriptor.flowControl     = USART_FLOW_CONTROL_NONE;

#if BSP_ENABLE_RS232_CONTROL == 1 && !defined(OTAU_SERVER)
  BSP_EnableRs232();
#endif

OPEN_USART(&appDescriptor);

#endif
  
}
#if  APP_INTERFACE == APP_INTERFACE_USBCDC
void uartDeInit(void)
{
  CLOSE_USART(&appDescriptor);
}
#endif

/**************************************************************************//**
\brief Sends data to the Serial Interface

\param[in] data   - pointer to data to be sent;
\param[in] length - number of bytes (length) of data to be sent
******************************************************************************/
void sendDataToUart(uint8_t *data, uint8_t length)
{
  WRITE_USART(&appDescriptor, data, length);
}

/**************************************************************************//**
\brief Reads data from the Serial Interface

\params[out] data - pointer to receive buffer;
\params[in]  length - number of bytes (length) in receive buffer;

\returns amount of read bytes
******************************************************************************/
uint8_t readDataFromUart(uint8_t *data, uint8_t length)
{
  return READ_USART(&appDescriptor, data, length);
}

/**************************************************************************//**
\brief Callback on receiving data via Serial interface

\param[in] length - amount of received bytes
*******************************************************************************/
#if APP_INTERFACE == APP_INTERFACE_USBCDC
static void uartDataReceived( HAL_UsbRxIndCallback_t *usbRxIndication)
{
  #if APP_ENABLE_CONSOLE == 1
    #if defined (USB_TEXT_MODE_SUPPORT)
      if(appDescriptor.oprMode == USB_BINARY_MODE)
      {
    #endif
        processConsole(usbRxIndication->length);
    #if defined (USB_TEXT_MODE_SUPPORT)
      }
      else if (appDescriptor.oprMode == USB_TEXT_MODE)
      {
        processCommand((char *)appDescriptor.rxBuffer);
      }
    #endif /* USB_TEXT_MODE_SUPPORT */
  #endif // APP_ENABLE_CONSOLE == 1
  }
#else
static void uartDataReceived(uint16_t length)
{
#if APP_ENABLE_CONSOLE == 1
  processConsole(length);
#endif // APP_ENABLE_CONSOLE == 1
}

#endif

/******************************************************************************
\brief Outputs formated string to serial interface

param[in] formt - formated string pointer
******************************************************************************/
#if APP_DEVICE_EVENTS_LOGGING == 1
/******************************************************************************
\brief Outputs formated string to serial interface

param[in] formt - formated string pointer
******************************************************************************/
void appSnprintf(const char *formt, ...)
{
  uint8_t str[TMP_STRING_BUFFER_SIZE];
  uint8_t length;
  va_list ap;

  va_start(ap, formt);
  length = vsnprintf((char *) str, TMP_STRING_BUFFER_SIZE, formt, ap);
  sendDataToUart(str, length);
}
#endif // APP_DEVICE_EVENTS_LOGGING == 1
#endif // #if (APP_DEVICE_EVENTS_LOGGING == 1) || (APP_ENABLE_CONSOLE == 1)
// eof uartManager.c
