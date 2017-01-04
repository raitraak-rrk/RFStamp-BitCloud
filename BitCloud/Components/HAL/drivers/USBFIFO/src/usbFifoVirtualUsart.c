/****************************************************************************//**
  \file usbFifoVirtualUart.c

  \brief Implementation of virtual uart API.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    11/09/08 A. Khromykh - Created
*******************************************************************************/
/******************************************************************************
                   Includes section
******************************************************************************/
#include <usbFifoVirtualUsart.h>
#include <usbFifoUsart.h>
#include <atomic.h>
#include <irq.h>

/******************************************************************************
                   External global variables section
******************************************************************************/
extern void (* extHandler)(void);

/******************************************************************************
                   Global variables section
******************************************************************************/
// pointer to application uart descriptor
HAL_UsartDescriptor_t *usbfifoPointDescrip = NULL;

/******************************************************************************
                   Implementations section
******************************************************************************/
/******************************************************************************
Open virtual com port and register uart's event handlers.

Parameters:
  descriptor - pointer to HAL_UartDescriptor_t structure

Returns:
  Returns positive uart descriptor on success or -1 in cases:
    - bad uart channel;
    - there are not enough resources;
    - receiving buffer is less bulk endpoint size;
******************************************************************************/
int USBFIFO_OpenUsart(HAL_UsartDescriptor_t *descriptor)
{
  if (NULL == descriptor)
    return -1;

  if (USART_CHANNEL_USBFIFO != descriptor->tty)
    return -1;

  if (NULL != usbfifoPointDescrip)
    return -1; /* source was opened */

  extHandler = usbfifoHandler;
  usbfifoPointDescrip = descriptor;

  usbfifoPointDescrip->service.rxPointOfRead = 0;
  usbfifoPointDescrip->service.rxPointOfWrite = 0;
  usbfifoPointDescrip->service.txPointOfRead = 0;
  usbfifoPointDescrip->service.txPointOfWrite = 0;
  if (0 != usbfifoInit())
    return -1;

  // enable receiver
  HAL_EnableIrq(IRQ_7);

  return (int)descriptor->tty;
}

/******************************************************************************
Frees the virtual uart channel.
Parameters:
  descriptor - the uart descriptor.
Returns:
  Returns 0 on success or -1 if bad descriptor.
******************************************************************************/
int USBFIFO_CloseUsart(HAL_UsartDescriptor_t *descriptor)
{
  if (NULL == descriptor)
    return -1;

  if (usbfifoPointDescrip != descriptor)
    return -1;

  usbfifoPointDescrip = NULL;
  extHandler = NULL;
  usbfifoUnInit();

  return 0;
}

/******************************************************************************
Writes a number of bytes to a virtual uart channel.
txCallback function will be used to notify when the transmission is finished.
Parameters:
  descriptor - pointer to HAL_UartDescriptor_t structure;
  buffer - pointer to the application data buffer;
  length - number of bytes to transfer;
Returns:
  -1 - bad descriptor;
   Number of bytes placed to the buffer - success.
******************************************************************************/
int USBFIFO_WriteUsart(HAL_UsartDescriptor_t *descriptor, uint8_t *buffer, uint16_t length)
{
  uint16_t           poW;
  uint16_t           poR;
  uint16_t           old;
  uint16_t           wasWrote = 0;
  HalUsartService_t *control;

  if (NULL == descriptor)
    return -1;

  if (usbfifoPointDescrip != descriptor)
    return -1;

  if (!buffer || !length)
    return -1;

  control = &descriptor->service;
  ATOMIC_SECTION_ENTER
    poW = control->txPointOfWrite;
    poR = control->txPointOfRead;
  ATOMIC_SECTION_LEAVE

  if (0 == descriptor->txBufferLength)
  { // Callback mode
    if (poW != poR)
      return -1; // there is unsent data
    descriptor->txBuffer = buffer;
    control->txPointOfWrite = length;
    control->txPointOfRead = 0;
    wasWrote = length;
  } // Callback mode.
  else
  { // Polling mode.
    while (wasWrote < length)
    {
      old = poW;

      if (++poW == descriptor->txBufferLength)
        poW = 0;

      if (poW == poR)
      { // Buffer full.
        poW = old;
        break;
      } // Buffer full.

      descriptor->txBuffer[old] = buffer[wasWrote++];
    }

    ATOMIC_SECTION_ENTER
      control->txPointOfWrite = poW;
    ATOMIC_SECTION_LEAVE
  } // Polling mode

  HAL_EnableIrq(IRQ_6);

  return wasWrote;
}

/*****************************************************************************
Reads length bytes from uart and places ones to buffer.
Parameters:
  descriptor - uart descriptor;
  buffer - pointer to a application buffer;
  length - the number of bytes which should be placed to buffer
Returns:
  -1 - bad descriptor, bad number to read;
  number of bytes that were placed to buffer.
*****************************************************************************/
int USBFIFO_ReadUsart(HAL_UsartDescriptor_t *descriptor, uint8_t *buffer, uint16_t length)
{
  uint16_t           wasRead = 0;
  uint16_t           poW;
  uint16_t           poR;
  HalUsartService_t *control;

  if (NULL == descriptor)
    return -1;

  if (!buffer || !length)
    return -1;

  if (descriptor != usbfifoPointDescrip)
    return -1; // Channel is not opened.

  control = &usbfifoPointDescrip->service;
  ATOMIC_SECTION_ENTER
    poW = control->rxPointOfWrite;
    poR = control->rxPointOfRead;
  ATOMIC_SECTION_LEAVE

  while ((poR != poW) && (wasRead < length))
  {
    buffer[wasRead] = descriptor->rxBuffer[poR];
    if (++poR == descriptor->rxBufferLength)
      poR = 0;
    wasRead++;
  }

  ATOMIC_SECTION_ENTER
    control->rxPointOfRead = poR;
    control->rxBytesInBuffer -= wasRead;
  ATOMIC_SECTION_LEAVE

  HAL_EnableIrq(IRQ_7);

  return wasRead;
}

/**************************************************************************//**
\brief Checks the status of tx buffer.

\param[in] descriptor - pointer to HAL_UsartDescriptor_t structure;
\return -1 - bad descriptor, no tx buffer; \n
         1 - tx buffer is empty; \n
         0 - tx buffer is not empty;
******************************************************************************/
int USBFIFO_IsTxEmpty(HAL_UsartDescriptor_t *descriptor)
{
  HalUsartService_t *control;
  uint16_t           poW;
  uint16_t           poR;

  if (NULL == descriptor)
    return -1;

  if (descriptor != usbfifoPointDescrip)
    return -1; // Channel is not opened.

  control = &usbfifoPointDescrip->service;
  ATOMIC_SECTION_ENTER
    poW = control->txPointOfWrite;
    poR = control->txPointOfRead;
  ATOMIC_SECTION_LEAVE
  if (poW == poR)
    return 1;

  return 0;
}

// eof usbFifoVirtualUart.c
