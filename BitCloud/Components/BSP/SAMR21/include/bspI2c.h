/**************************************************************************//**
  \file   bspI2c.h

  \brief  Declaration of board specific i2c pin configuration

  \author
      Atmel Corporation: http://www.atmel.com \n
      Support email: avr@atmel.com

    Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
    Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      05/05/14 Viswanadham kotla - Created
 ******************************************************************************/

#ifndef _BSPI2C_H
#define _BSPI2C_H
 
#if BSP_SUPPORT != BOARD_FAKE
#if BSP_SUPPORT == BOARD_SAMR21_XPRO
/******************************************************************************
                   Includes section
******************************************************************************/
#include <i2cPacket.h>
/******************************************************************************
                   Prototype section
******************************************************************************/
/**************************************************************************//**
\brief Configure the usart pin based on the board
******************************************************************************/
void BSP_BoardSpecificI2cPinInit(I2cChannel_t tty);

#endif // BSP_SUPPORT == BOARD_SAMR21_XPRO
#endif // if BSP_SUPPORT != BOARD_FAKE
#endif //_BSPI2C_H
// eof bspI2c.h