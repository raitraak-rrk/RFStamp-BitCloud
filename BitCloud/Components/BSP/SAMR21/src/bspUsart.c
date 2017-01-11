/**************************************************************************//**
  \file   bspUart.c

  \brief  Implementation of board specific usart pin configuration

  \author
      Atmel Corporation: http://www.atmel.com \n
      Support email: avr@atmel.com

    Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
    Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      01/04/14 Agasthian - Created
 ******************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/
#if BSP_SUPPORT != BOARD_FAKE
/******************************************************************************
                   Includes section
******************************************************************************/
#include <usart.h>

/******************************************************************************
                   Implementations section
******************************************************************************/
/**************************************************************************//**
\brief Configure the usart pin based on the board
******************************************************************************/
void BSP_BoardSpecificUsartPinInit(UsartChannel_t tty)
{
  // configure uart
  if (tty == USART_CHANNEL_0)
  {
#if (BSP_SUPPORT == BOARD_SAMR21_XPRO)
    tty->sercom = &(SERCOM0->USART);
    tty->usartPinConfig[USART_TX_SIG].pinNum = 4;
    tty->usartPinConfig[USART_TX_SIG].portNum = PORT_A;
    tty->usartPinConfig[USART_TX_SIG].functionConfig = 3;
    tty->usartPinConfig[USART_RX_SIG].pinNum = 5;
    tty->usartPinConfig[USART_RX_SIG].portNum = PORT_A;
    tty->usartPinConfig[USART_RX_SIG].functionConfig = 3;
    tty->rxPadConfig = 1;
    tty->txPadConfig = 0;
#elif (BSP_SUPPORT == BOARD_SAMR21_ZLLEK)
    tty->sercom = &(SERCOM5->USART);
    tty->usartPinConfig[USART_TX_SIG].pinNum = 2;
    tty->usartPinConfig[USART_TX_SIG].portNum = PORT_B;
    tty->usartPinConfig[USART_TX_SIG].functionConfig = 3;
    tty->usartPinConfig[USART_RX_SIG].pinNum = 3;
    tty->usartPinConfig[USART_RX_SIG].portNum = PORT_B;
    tty->usartPinConfig[USART_RX_SIG].functionConfig = 3;
    tty->rxPadConfig = 1;
    tty->txPadConfig = 0;
#elif (BSP_SUPPORT == BOARD_SAMR21B18_MZ210PA_MODULE)
    tty->sercom = &(SERCOM2->USART);
    tty->usartPinConfig[USART_TX_SIG].pinNum = 14;
    tty->usartPinConfig[USART_TX_SIG].portNum = PORT_A;
    tty->usartPinConfig[USART_TX_SIG].functionConfig = 2;
    tty->usartPinConfig[USART_RX_SIG].pinNum = 15;
    tty->usartPinConfig[USART_RX_SIG].portNum = PORT_A;
    tty->usartPinConfig[USART_RX_SIG].functionConfig = 2;
    tty->rxPadConfig = 3;
    tty->txPadConfig = 1;
#elif (BSP_SUPPORT == BOARD_SAMR21_CUSTOM)
    tty->sercom = &(SERCOM2->USART);
    tty->usartPinConfig[USART_TX_SIG].pinNum = 14;
    tty->usartPinConfig[USART_TX_SIG].portNum = PORT_A;
    tty->usartPinConfig[USART_TX_SIG].functionConfig = 2;
    tty->usartPinConfig[USART_RX_SIG].pinNum = 15;
    tty->usartPinConfig[USART_RX_SIG].portNum = PORT_A;
    tty->usartPinConfig[USART_RX_SIG].functionConfig = 2;
    tty->rxPadConfig = 3;
    tty->txPadConfig = 1;
#elif (BSP_SUPPORT == BOARD_SAMR21G18_MR210UA_MODULE)
    tty->sercom = &(SERCOM0->USART);
    tty->usartPinConfig[USART_TX_SIG].pinNum = 8;
    tty->usartPinConfig[USART_TX_SIG].portNum = PORT_A;
    tty->usartPinConfig[USART_TX_SIG].functionConfig = 2;
    tty->usartPinConfig[USART_RX_SIG].pinNum = 7;
    tty->usartPinConfig[USART_RX_SIG].portNum = PORT_A;
    tty->usartPinConfig[USART_RX_SIG].functionConfig = 3;
    tty->rxPadConfig = 3;
    tty->txPadConfig = 0;
#elif (BSP_SUPPORT == BOARD_RFRINGQM)
    tty->sercom = &(SERCOM0->USART);
    tty->usartPinConfig[USART_TX_SIG].pinNum = 4;
    tty->usartPinConfig[USART_TX_SIG].portNum = PORT_A;
    tty->usartPinConfig[USART_TX_SIG].functionConfig = 3;
    tty->usartPinConfig[USART_RX_SIG].pinNum = 7;
    tty->usartPinConfig[USART_RX_SIG].portNum = PORT_A;
    tty->usartPinConfig[USART_RX_SIG].functionConfig = 3;
    tty->rxPadConfig = 3;
    tty->txPadConfig = 0;
#elif (BSP_SUPPORT == BOARD_RFRINGQT)
    tty->sercom = &(SERCOM1->USART);
    tty->usartPinConfig[USART_TX_SIG].pinNum = 16;
    tty->usartPinConfig[USART_TX_SIG].portNum = PORT_A;
    tty->usartPinConfig[USART_TX_SIG].functionConfig = 2;
    tty->usartPinConfig[USART_RX_SIG].pinNum = 17;
    tty->usartPinConfig[USART_RX_SIG].portNum = PORT_A;
    tty->usartPinConfig[USART_RX_SIG].functionConfig = 2;
    tty->rxPadConfig = 1;
    tty->txPadConfig = 0;
#elif (BSP_SUPPORT == BOARD_RFSTRIP)
    tty->sercom = &(SERCOM1->USART);
    tty->usartPinConfig[USART_TX_SIG].pinNum = 18;
    tty->usartPinConfig[USART_TX_SIG].portNum = PORT_A;
    tty->usartPinConfig[USART_TX_SIG].functionConfig = 2;
    tty->usartPinConfig[USART_RX_SIG].pinNum = 19;
    tty->usartPinConfig[USART_RX_SIG].portNum = PORT_A;
    tty->usartPinConfig[USART_RX_SIG].functionConfig = 2;
    tty->rxPadConfig = 3;
    tty->txPadConfig = 1;
#endif
  }
  else if (tty == USART_CHANNEL_1)
  {
#if BSP_SUPPORT == BOARD_SAMR21_ZLLEK
    tty->sercom = &(SERCOM3->USART);
    tty->usartPinConfig[USART_TX_SIG].pinNum = 27;
    tty->usartPinConfig[USART_TX_SIG].portNum = PORT_A;
    tty->usartPinConfig[USART_TX_SIG].functionConfig = 5;
    
    tty->usartPinConfig[USART_RX_SIG].pinNum = 28;
    tty->usartPinConfig[USART_RX_SIG].portNum = PORT_A;
    tty->usartPinConfig[USART_RX_SIG].functionConfig = 5;
    
    tty->usartPinConfig[USART_RTS_SIG].pinNum = 24;
    tty->usartPinConfig[USART_RTS_SIG].portNum = PORT_A;
    tty->usartPinConfig[USART_RTS_SIG].functionConfig = 2;
    
    tty->usartPinConfig[USART_CTS_SIG].pinNum = 25;
    tty->usartPinConfig[USART_CTS_SIG].portNum = PORT_A;
    tty->usartPinConfig[USART_CTS_SIG].functionConfig = 2;

    tty->rxPadConfig = 1;
    tty->txPadConfig = 2;
#elif BSP_SUPPORT == BOARD_SAMR21_XPRO
    tty->sercom = &(SERCOM0->USART);
    tty->usartPinConfig[USART_TX_SIG].pinNum = 6;
    tty->usartPinConfig[USART_TX_SIG].portNum = PORT_A;
    tty->usartPinConfig[USART_TX_SIG].functionConfig = 3;
    tty->usartPinConfig[USART_RX_SIG].pinNum = 7;
    tty->usartPinConfig[USART_RX_SIG].portNum = PORT_A;
    tty->usartPinConfig[USART_RX_SIG].functionConfig = 3;
    tty->rxPadConfig = 3;
    tty->txPadConfig = 1;
#elif (BSP_SUPPORT == BOARD_SAMR21_CUSTOM) || \
      (BSP_SUPPORT == BOARD_SAMR21B18_MZ210PA_MODULE)
    tty->sercom = &(SERCOM1->USART);
    tty->usartPinConfig[USART_TX_SIG].pinNum = 16;
    tty->usartPinConfig[USART_TX_SIG].portNum = PORT_A;
    tty->usartPinConfig[USART_TX_SIG].functionConfig = 2;
    tty->usartPinConfig[USART_RX_SIG].pinNum = 17;
    tty->usartPinConfig[USART_RX_SIG].portNum = PORT_A;
    tty->usartPinConfig[USART_RX_SIG].functionConfig = 2;
    tty->rxPadConfig = 1;
    tty->txPadConfig = 0;
#endif
  }
}
#endif // if BSP_SUPPORT != BOARD_FAKE
// eof bspUsart.c
