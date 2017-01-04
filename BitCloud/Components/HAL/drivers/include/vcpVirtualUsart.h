/****************************************************************************//**
  \file vcpVirtualUsart.h

  \brief The header file describes the interface of the virtual uart based on USB

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    05/09/08 A. Khromykh - Created
*******************************************************************************/
#ifndef _VCPVIRTUALUART_H
#define _VCPVIRTUALUART_H

/******************************************************************************
                   Includes section
******************************************************************************/
#include <sysTypes.h>
#include <usart.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
#define USART_CHANNEL_VCP  ((UsartChannel_t)5)
#define VCP_TRANSMIT_PIPE      2
#define VCP_RECEIVE_PIPE       1

/******************************************************************************
                   Prototypes section
******************************************************************************/
/**************************************************************************//**
\brief Registers uart's event handlers.

\param[in]
  descriptor - pointer to HAL_UartDescriptor_t structure

\return
  Returns positive uart descriptor on success or -1 in cases: \n
    - bad uart channel; \n
    - there are not enough resources; \n
    - receiving buffer is less bulk endpoint size;
******************************************************************************/
int VCP_OpenUsart(HAL_UsartDescriptor_t *descriptor);

/*************************************************************************//**
\brief Releases the uart channel.

\param[in]
  descriptor - pointer to HAL_UartDescriptor_t structure

\return
 -1 - bad descriptor; \n
  0 - success.
*****************************************************************************/
int VCP_CloseUsart(HAL_UsartDescriptor_t *descriptor);

/**************************************************************************//**
\brief Writes a number of bytes to uart channel.
txCallback function will be used to notify when the transmission is finished.

\param[in]
  descriptor - pointer to HAL_UartDescriptor_t structure;

\param[in]
  buffer - pointer to the application data buffer;

\param[in]
  length - number of bytes to transfer;

\return
  -1 - bad descriptor; \n
   Number of bytes placed to the buffer - success.
******************************************************************************/
int VCP_WriteUsart(HAL_UsartDescriptor_t *descriptor, uint8_t *buffer, uint16_t length);

/*************************************************************************//**
\brief Reads a number of bytes from uart and places them to the buffer.

\param[in]
  descriptor - pointer to HAL_UartDescriptor_t structure;

\param[in]
  buffer - pointer to the application buffer;

\param[in]
  length - number of bytes to be placed to the buffer;

\return
  -1 - bad descriptor, or bad number of bytes to read; \n
  Number of bytes placed to the buffer - success.
*****************************************************************************/
int VCP_ReadUsart(HAL_UsartDescriptor_t *descriptor, uint8_t *buffer, uint16_t length);

/**************************************************************************//**
\brief Checks the status of tx buffer (for polling mode).

\param[in]
  descriptor - pointer to HAL_UsartDescriptor_t structure;
\return
  -1 - bad descriptor, no tx buffer; \n
   1 - tx buffer is empty; \n
   0 - tx buffer is not empty;
******************************************************************************/
int VCP_IsTxEmpty(HAL_UsartDescriptor_t *descriptor);

#endif /* _VCPVIRTUALUART_H */
