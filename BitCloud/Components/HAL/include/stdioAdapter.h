/**************************************************************************//**
  \file  stdioAdapter.h

  \brief The header file describes adapter to stdio stream

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    16/01/13 S. Dmitriev - Created
 ******************************************************************************/
#ifndef _STDIO_ADAPTER_H
#define _STDIO_ADAPTER_H

/******************************************************************************
                        Includes section
******************************************************************************/
#include <usart.h>

/******************************************************************************
                    Functions prototypes section
******************************************************************************/
 /**************************************************************************//**
\brief Init stdio adapter

\param[in]
  descriptor - pointer to HAL_UsartDescriptor_t structure

\returns positive usart descriptor on success or -1 in cases: \n
    - the channel was already opened. \n
    - pointer to descriptor is equal to NULL. \n
******************************************************************************/
int HAL_OpenStdio(HAL_UsartDescriptor_t *descriptor);

/*************************************************************************//**
\brief Releases the usart channel and pins, if hardware flow control was used.

\ingroup hal_usart

\param[in]
  descriptor - pointer to HAL_UsartDescriptor_t structure

\return
 -1 - bad descriptor or channel is already closed; \n
  0 - success.
*****************************************************************************/
int HAL_CloseStdio(HAL_UsartDescriptor_t *descriptor);

/*************************************************************************//**
\brief Reads a number of bytes from stdio and places them to the buffer.

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
int HAL_ReadStdin(HAL_UsartDescriptor_t *descriptor, uint8_t *buffer, uint16_t length);

/**************************************************************************//**
\brief Writes a number of bytes to stdout.

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
int HAL_WriteStdout(HAL_UsartDescriptor_t *descriptor, uint8_t *buffer, uint16_t length);

#endif // _STDIO_ADAPTER_H
// eof stdioAdapter.h