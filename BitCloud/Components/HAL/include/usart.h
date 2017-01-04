/****************************************************************************//**
  \file  usart.h

  \brief The header file describes the usart interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    22/08/07 A. Khromykh - Created
*******************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/
#ifndef _USART_H
#define _USART_H

/******************************************************************************
                   Includes section
******************************************************************************/
// \cond
#include <halUsart.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
#define USART_FLOW_CONTROL_NONE 0
#define USART_FLOW_CONTROL_HARDWARE (1 << 0)
#define USART_DTR_CONTROL (1 << 1)

#if defined(ATMEGA1281) || defined(ATMEGA2561) || defined(ATMEGA1284) || \
    defined(AT90USB1287) || defined(ATMEGA128RFA1) || defined(ATXMEGA128A1) || \
    defined(ATXMEGA256A3) || defined(ATXMEGA128B1) || defined(ATXMEGA256D3) || \
    defined(ATMEGA256RFR2) || defined(ATMEGA2564RFR2)
  // this define is used only by HAL.
  #define USART_SPI_READ_MODE  (1 << 4)
  #define USART_SPI_WRITE_MODE (1 << 3)
#endif
// \endcond

/******************************************************************************
                   Types section
******************************************************************************/
/** \brief Usart descriptor*/
typedef struct
{
  /** \brief HAL USART service field - contains variables for HAL USART module
  internal needs */
  HalUsartService_t service;
  /** \brief tty - USART_CHANNEL_n to be used. "n" range depends on the platform.
  Take a look into halUsart.h platform specific file fore more details. */
  UsartChannel_t tty;
  /** \brief Sets synchronous or asynchronous routine. \n
  Must be chosen from: \n
    USART_MODE_ASYNC \n
    USART_MODE_RS485 (only for arm)\n
    USART_MODE_SYNC  \n */
  UsartMode_t mode;
  /** \brief baudrate - USART baud rate. Must be chosen from: \n
    USART_BAUDRATE_1200  \n
    USART_BAUDRATE_2400  \n
    USART_BAUDRATE_4800  \n
    USART_BAUDRATE_9600 \n
    USART_BAUDRATE_19200 \n
    USART_BAUDRATE_38400 \n
    USART_SYNC_BAUDRATE_1200 \n
    USART_SYNC_BAUDRATE_2400 \n
    USART_SYNC_BAUDRATE_4800 \n
    USART_SYNC_BAUDRATE_9600 \n
    USART_SYNC_BAUDRATE_38400 \n
    USART_SYNC_BAUDRATE_57600 \n
    USART_SYNC_BAUDRATE_115200  \n */
  UsartBaudRate_t baudrate;
  /** \brief data -  USART data length. Must be chosen from: \n
    USART_DATA5 \n
    USART_DATA6 \n
    USART_DATA7 \n
    USART_DATA8 \n */
  UsartData_t dataLength;
  /** \brief parity -  USART parity mode. Must be chosen from: \n
    USART_PARITY_NONE \n
    USART_PARITY_EVEN \n
    USART_PARITY_ODD \n  */
  UsartParity_t parity;
  /** \brief stopbits - USART stop bits number. Must be chosen from: \n
    USART_STOPBIT_1 \n
    USART_STOPBIT_2 \n */
  UsartStopBits_t stopbits;
  /** \brief edge - data received edge (only for usart). Must be chosen from: \n
    USART_EDGE_MODE_FALLING \n
    USART_EDGE_MODE_RISING  \n */
  UsartEdgeMode_t edge;
  /** \brief master or slave on usart (only for usart). Must be chosen from:
    USART_CLK_MODE_MASTER \n
    USART_CLK_MODE_SLAVE  \n */
  UsartClkMode_t syncMode;
  /** \brief It's pointer to receive buffer. \n
   If rxBuffer is NULL then transactions are discarded. \n
   Size of buffer depends on user application. */
  uint8_t *rxBuffer;
  /** \brief length of receive buffer */
  uint16_t rxBufferLength;
  /** \brief It's pointer to transmit buffer. \n
   If txBuffer is NULL then callback method is used. \n
   If txBuffer isn't NULL then polling method is used. */
  uint8_t *txBuffer;
  /** \brief length of transmit buffer */
  uint16_t txBufferLength;
  /** \brief It's receive usart callback. \n
   If rxCallback is NULL then polling method is used. \n
   If rxCallback isn't NULL then callback method is used.*/
  void (*rxCallback)(uint16_t);
  /** \brief It's transmitting was completed usart callback. \n
   If txBuffer isn't NULL then txCallback notify about end of bytes sending.  */
  void (*txCallback)(void);
  #if defined(_USE_USART_ERROR_EVENT_)
    /** \brief It's error was occurred usart callback. \n
    If receiver error is issued then errCallback notify about issue with reason. \n
    Reason must be: \n
    FRAME_ERROR  \n
    DATA_OVERRUN \n
    PARITY_ERROR */
    void (*errCallback)(UsartErrorReason_t);
  #endif
  /** \brief flow control of usart. One of the defines \n
   USART_FLOW_CONTROL_NONE, \n
   USART_FLOW_CONTROL_HARDWARE, USART_DTR_CONTROL, it is possible to combine by logical OR. \n
   RS485 mode needs for USART_FLOW_CONTROL_NONE. \n
   The RTS pin is driven high when the transmitter is operating. */
  uint8_t flowControl;
} HAL_UsartDescriptor_t;

/******************************************************************************
                   Prototypes section
******************************************************************************/
/**************************************************************************//**
\brief Registers usart's event handlers. Performs configuration
of usart registers. Performs configuration of RTS, CTS and DTR pins.

\ingroup hal_usart

\param[in]
  descriptor - pointer to HAL_UsartDescriptor_t structure

\return
  Returns positive usart descriptor on success or -1 in cases: \n
    - bad usart channel; \n
    - there are not enough resources; \n
******************************************************************************/
int HAL_OpenUsart(HAL_UsartDescriptor_t *descriptor);

/*************************************************************************//**
\brief Releases the usart channel and pins, if hardware flow control was used.

\ingroup hal_usart

\param[in]
  descriptor - pointer to HAL_UsartDescriptor_t structure

\return
 -1 - bad descriptor or channel is already closed; \n
  0 - success.
*****************************************************************************/
int HAL_CloseUsart(HAL_UsartDescriptor_t *descriptor);

/**************************************************************************//**
\brief Writes a number of bytes to usart channel.
txCallback function will be used to notify when the transmission is finished.
If hardware flow control is used for transmitting then RTS and DTR pins will
be tested during transmission.

\ingroup hal_usart

\param[in]
  descriptor - pointer to HAL_UsartDescriptor_t structure;

\param[in]
  buffer - pointer to the application data buffer;

\param[in]
  length - number of bytes to transfer;

\return
  -1 - bad descriptor; \n
   Number of bytes placed to the buffer - success.
******************************************************************************/
int HAL_WriteUsart(HAL_UsartDescriptor_t *descriptor, uint8_t *buffer, uint16_t length);

/*************************************************************************//**
\brief Reads a number of bytes from usart and places them to the buffer.

\ingroup hal_usart

\param[in]
  descriptor - pointer to HAL_UsartDescriptor_t structure;

\param[in]
  buffer - pointer to the application buffer;

\param[in]
  length - number of bytes to be placed to the buffer;

\return
  -1 - bad descriptor, or bad number of bytes to read; \n
  Number of bytes placed to the buffer - success.
*****************************************************************************/
int HAL_ReadUsart(HAL_UsartDescriptor_t *descriptor, uint8_t *buffer, uint16_t length);

/**************************************************************************//**
\brief Forbids the host to transmit data.
Only USART_CHANNEL_1 can be used for hardware flow control for avr.

\ingroup hal_usart

\param[in]
  descriptor - pointer to HAL_UsartDescriptor_t structure;

\return
  -1 - bad descriptor, bad usart, or unsupported mode; \n
   0 - success.
******************************************************************************/
int HAL_OnUsartCts(HAL_UsartDescriptor_t *descriptor);

/**************************************************************************//**
\brief Allows the host to transmit data.
Only USART_CHANNEL_1 can be used for hardware flow control for avr.

\ingroup hal_usart

\param[in]
  descriptor - pointer to HAL_UsartDescriptor_t structure;

\return
  -1 - bad descriptor, bad usart, or unsupported mode; \n
   0 - success.
******************************************************************************/
int HAL_OffUsartCts(HAL_UsartDescriptor_t *descriptor);

/**************************************************************************//**
\brief Reads RTS pin state.
Only USART_CHANNEL_1 can be used for hardware flow control for avr.

\ingroup hal_usart

\param[in]
  descriptor - pointer to HAL_UsartDescriptor_t structure;

\return
  -1 - bad descriptor, bad usart, or unsupported mode; \n
   0 - RTS is low level; \n
   1 - RTS is high level;
******************************************************************************/
int HAL_ReadUsartRts(HAL_UsartDescriptor_t *descriptor);

/**************************************************************************//**
\brief Reads DTR pin state.
Only USART_CHANNEL_1 can be used for hardware flow control for avr.

\ingroup hal_usart

\param[in]
  descriptor - pointer to HAL_UsartDescriptor_t structure;

\return
  -1 - bad descriptor, bad usart, or unsupported mode; \n
   0 - DTR is low level; \n
   1 - DTR is high level;
******************************************************************************/
int HAL_ReadUsartDtr(HAL_UsartDescriptor_t *descriptor);

/**************************************************************************//**
\brief Checks the status of tx buffer.

\ingroup hal_usart

\param[in]
  descriptor - pointer to HAL_UsartDescriptor_t structure;

\return
  -1 - bad descriptor, no tx buffer; \n
   1 - tx buffer is empty; \n
   0 - tx buffer is not empty;
******************************************************************************/
int HAL_IsTxEmpty(HAL_UsartDescriptor_t *descriptor);

/**************************************************************************//**
\brief Holds execution of all tasks except related to reqiured channel.

\param[in] tty - channel to accept tasks for
******************************************************************************/
void HAL_HoldOnOthersUsartTasks(UsartChannel_t tty);

/**************************************************************************//**
\brief Accepts execution of previously holded tasks.
******************************************************************************/
void HAL_ReleaseAllHeldUsartTasks(void);

#if defined(ATMEGA1281) || defined(ATMEGA2561) || defined(ATMEGA1284) || defined(AT90USB1287) || defined(ATMEGA128RFA1) \
 || defined(ATMEGA256RFR2) || defined(ATMEGA2564RFR2)
/**************************************************************************//**
\brief Enables DTR wake up.

\param[in]
  callback - pointer to callback method;
******************************************************************************/
void HAL_EnableDtrWakeUp(void (* callback)(void));

/**************************************************************************//**
\brief Disables DTR wake up.
******************************************************************************/
void HAL_DisableDtrWakeUp(void);
#endif

/**************************************************************************//**
\brief Configure the usart pin based on the board
******************************************************************************/
#if defined(ATSAMR21G18A) || defined(ATSAMR21E18A) || defined(ATSAMR21E19A)
void BSP_BoardSpecificUsartPinInit(UsartChannel_t tty);
#endif

#endif /* _USART_H */
// eof usart.h
