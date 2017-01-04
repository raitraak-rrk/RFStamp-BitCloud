/*****************************************************************************//**
\file  isdConsoleTunneling.h

\brief Interface for tunneling functionality.

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal
  History:
    30.01.13 S. Dmitriev - Created
**********************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/

#ifndef _ISDCONSOLETUNNELING_H
#define _ISDCONSOLETUNNELING_H

/******************************************************************************
                   Prototypes section
******************************************************************************/
/**************************************************************************//**
\brief Start sending char from tx tunneling buffer through usart.
******************************************************************************/
void isdSendTunnelingDate(void);

/**************************************************************************//**
\brief Gets char from tx tunneling buffer.

\return read char
******************************************************************************/
uint8_t getCharFromTunnel(void);

/**************************************************************************//**
\brief Puts char in rx tunneling buffer.

\param[in] char to write

\return true if char was written correctly, false otherwise.
******************************************************************************/
bool putCharInTunnel(uint8_t ch);

/**************************************************************************//**
\brief Check is allocated buffers for tunneling or not.

\return true if tunnel was initialized, false otherwise.
******************************************************************************/
bool isTunnelInitialized(void);

/**************************************************************************//**
\brief Check there is something to tunneling from upper layer.

\return true if buffer of transmit isn't empty, false otherwise.
******************************************************************************/
bool isTunnelingDateToSend(void);

/**************************************************************************//**
\brief Raised callback to upper layer.
******************************************************************************/
void raiseTunnelRxCallback(void);

#endif // _ISDCONSOLETUNNELING_H