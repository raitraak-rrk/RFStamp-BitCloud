/**************************************************************************//**
  \file   bspI2c.c

  \brief  Implementation of board specific i2c pin configuration

  \author
      Atmel Corporation: http://www.atmel.com \n
      Support email: avr@atmel.com

    Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
    Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      01/04/14 Viswanadham kotla - Created
 ******************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/
#if BSP_SUPPORT != BOARD_FAKE
#if BSP_SUPPORT == BOARD_SAMR21_XPRO
/******************************************************************************
                   Includes section
******************************************************************************/
#include <i2cPacket.h>
/******************************************************************************
                   Implementations section
******************************************************************************/
/**************************************************************************//**
\brief Configure the usart pin based on the board
******************************************************************************/
void BSP_BoardSpecificI2cPinInit(I2cChannel_t tty)
{
  // configure uart
  if (tty == TWI_CHANNEL_0)
  {
    tty->sercom = &(SERCOM1->I2CM);
    tty->i2cPinConfig[I2C_SDA_SIG].pinNum = 16;
    tty->i2cPinConfig[I2C_SDA_SIG].portNum = PORT_A;
    tty->i2cPinConfig[I2C_SDA_SIG].functionConfig = 2;/* peripheral func C */
    tty->i2cPinConfig[I2C_SCK_SIG].pinNum = 17;
    tty->i2cPinConfig[I2C_SCK_SIG].portNum = PORT_A;
    tty->i2cPinConfig[I2C_SCK_SIG].functionConfig = 2; /* peripheral func C */
    tty->sdaPadConfig = 0;
    tty->sckPadConfig = 1;
  }
}

#endif // if BSP_SUPPORT == BOARD_SAMR21_XPRO
#endif // if BSP_SUPPORT != BOARD_FAKE
// eof bspI2c.c