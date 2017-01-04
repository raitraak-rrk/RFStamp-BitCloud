/**************************************************************************//**
\file  halUsart.c

\brief Implementation of uart hardware-dependent module.

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2012, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal
  History:
    07/04/14 karthik.p_u - Created
*******************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/
#if defined(HAL_USE_USART)
/******************************************************************************
                   Includes section
******************************************************************************/

#include <usart.h>
#include <halInterrupt.h>
#include <halSleepTimerClock.h>

/******************************************************************************
                   Prototypes section
******************************************************************************/
void halPostUsartTask(HalUsartTaskId_t taskId);
void halUartSwrstSync(HAL_UsartDescriptor_t *descriptor);
void halUartEnableSync(HAL_UsartDescriptor_t *descriptor);
void halUartCtrlbSync(HAL_UsartDescriptor_t *descriptor);
void halSetUsartConfig(HAL_UsartDescriptor_t *descriptor);

/*****************************************************************************
                   Implementations section
******************************************************************************/

/**************************************************************************//**
  \brief Interrupt handler usart0

  \param none
  \return none.
******************************************************************************/
void usart0Handler(void)
{
  uint8_t intFlags;
  uint16_t status;
  uint32_t data;
  UsartChannel_t tty = USART_CHANNEL_0;

  /* get usart status register */
  status = tty->sercom->STATUS.reg;
  intFlags = tty->sercom->INTFLAG.reg;
  /* check overrun, frame or parity error */
  if (status & (SERCOM_USART_STATUS_BUFOVF | SERCOM_USART_STATUS_FERR | SERCOM_USART_STATUS_PERR))
  {
    tty->sercom->STATUS.reg = (SERCOM_USART_STATUS_BUFOVF | SERCOM_USART_STATUS_FERR | SERCOM_USART_STATUS_PERR);

    #if defined(_USE_USART_ERROR_EVENT_)
      halUsartSaveErrorReason(tty, status);
      halPostUsartTask((USART_CHANNEL_0 == tty) ? HAL_USART_TASK_USART0_ERR : HAL_USART_TASK_USART1_ERR);
    #endif
  } /* is any data received */
  else if (intFlags & SERCOM_USART_INTFLAG_RXC)
  {
    data =tty->sercom->DATA.reg;
    halWakeupFromIrq();
    halUsartRxBufferFiller(tty, data);
    halPostUsartTask(HAL_USART_TASK_USART0_RXC );
  }/* is data register empty */
  else if((intFlags & SERCOM_USART_INTFLAG_DRE) && (tty->sercom->INTENSET.bit.DRE == 1))
  {
    halDisableUsartDremInterrupt(tty);
    halWakeupFromIrq();
    halPostUsartTask(HAL_USART_TASK_USART0_DRE) ;
  } /* is transmission completed */ 
  else if ((intFlags & SERCOM_USART_INTFLAG_TXC)&& (tty->sercom->INTENSET.bit.TXC == 1))
  {
    // We must disable the interrupt because we must "break" context.
    halDisableUsartTxcInterrupt(tty);
    halPostUsartTask(HAL_USART_TASK_USART0_TXC );
  }
}

/**************************************************************************//**
  \brief Interrupt handler usart1

  \param none
  \return none.
******************************************************************************/
void usart1Handler(void)
{
  uint8_t intFlags;
  uint16_t status;
  uint32_t data;
  UsartChannel_t tty = USART_CHANNEL_1;

  /* get usart status register */
  status = tty->sercom->STATUS.reg;
  intFlags = tty->sercom->INTFLAG.reg;
  /* check overrun, frame or parity error */
  if (status & (SERCOM_USART_STATUS_BUFOVF | SERCOM_USART_STATUS_FERR | SERCOM_USART_STATUS_PERR))
  {
    tty->sercom->STATUS.reg = (SERCOM_USART_STATUS_BUFOVF | SERCOM_USART_STATUS_FERR | SERCOM_USART_STATUS_PERR);

    #if defined(_USE_USART_ERROR_EVENT_)
      halUsartSaveErrorReason(tty, status);
      halPostUsartTask((USART_CHANNEL_1 == tty) ? HAL_USART_TASK_USART1_ERR : HAL_USART_TASK_USART1_ERR);
    #endif
  } /* is any data received */
  else if (intFlags & SERCOM_USART_INTFLAG_RXC)
  {
    data =tty->sercom->DATA.reg;
    halWakeupFromIrq();
    halUsartRxBufferFiller(tty, data);
    halPostUsartTask(HAL_USART_TASK_USART1_RXC );
  }/* is data register empty */
  else if((intFlags & SERCOM_USART_INTFLAG_DRE) && (tty->sercom->INTENSET.bit.DRE == 1))
  {
    halDisableUsartDremInterrupt(tty);
    halWakeupFromIrq();
    halPostUsartTask(HAL_USART_TASK_USART1_DRE) ;
  } /* is transmission completed */ 
  else if ((intFlags & SERCOM_USART_INTFLAG_TXC)&& (tty->sercom->INTENSET.bit.TXC == 1))
  {
    // We must disable the interrupt because we must "break" context.
    halDisableUsartTxcInterrupt(tty);
    halPostUsartTask(HAL_USART_TASK_USART1_TXC );
  }
}

/**************************************************************************//**
  \brief Sets USART module parameters.

  \param descriptor - pointer to usart descriptor.
  \return none.
******************************************************************************/
void halFoundationsInit(HAL_UsartDescriptor_t *descriptor)
{
  uint8_t sercomNo;

  GPIO_make_out(&descriptor->tty->usartPinConfig[USART_TX_SIG]);
  GPIO_make_in(&descriptor->tty->usartPinConfig[USART_RX_SIG]);

  GPIO_pinfunc_config(&descriptor->tty->usartPinConfig[USART_TX_SIG]);
  GPIO_pinfunc_config(&descriptor->tty->usartPinConfig[USART_RX_SIG]);

  sercomNo = ((uint32_t)descriptor->tty->sercom - (uint32_t)&SC0_USART_CTRLA) / 0x400;

  /* clk settings */
  GCLK_CLKCTRL_s.id = 0x14 + sercomNo;
  GCLK_CLKCTRL_s.gen = 3;
  GCLK_CLKCTRL_s.clken = 1;

  /* enable the clock of USART */
  PM_APBCMASK |= (1 << (2 + sercomNo));

  if (USART_CHANNEL_0 == descriptor->tty)
    HAL_InstallInterruptVector(SERCOM0_IRQn + sercomNo, usart0Handler);
  else if (USART_CHANNEL_1 == descriptor->tty)
    HAL_InstallInterruptVector(SERCOM0_IRQn + sercomNo, usart1Handler);

  /* Clear & disable USART interrupt on NVIC */
  NVIC_DisableIRQ(SERCOM0_IRQn + sercomNo);
  NVIC_ClearPendingIRQ(SERCOM0_IRQn + sercomNo);
  /* set priority & enable USART interrupt on NVIC */
  NVIC_EnableIRQ(SERCOM0_IRQn + sercomNo);

}
/**************************************************************************//**
  \brief Sets USART module parameters.

  \param
    descriptor - USART module descriptor.
  \return
   none.
******************************************************************************/
void halSetUsartConfig(HAL_UsartDescriptor_t *descriptor)
{

  halFoundationsInit(descriptor);

  descriptor->tty->sercom->CTRLA.bit.SWRST = 1; //reset the USART 
  halUartSwrstSync(descriptor);

  descriptor->tty->sercom->CTRLB.reg = ((uint32_t)descriptor->parity | (uint32_t)descriptor->dataLength | (uint32_t)descriptor->stopbits);
  halUartCtrlbSync(descriptor);

  descriptor->tty->sercom->CTRLA.reg |= descriptor->mode ;
  descriptor->tty->sercom->CTRLA.bit.MODE = 0x01; //USART with internal clk
  descriptor->tty->sercom->CTRLA.bit.DORD = 0x01; //data order LSB first
  descriptor->tty->sercom->CTRLA.bit.RXPO = descriptor->tty->rxPadConfig; //PAD1			  
  descriptor->tty->sercom->CTRLA.bit.TXPO = descriptor->tty->txPadConfig; //PAD0

  if (USART_MODE_SYNC == descriptor->mode)
    descriptor->tty->sercom->CTRLA.reg  |= descriptor->syncMode;

  descriptor->tty->sercom->INTENCLR.reg = (uint8_t)ALL_PERIPHERIAL_INTERRUPT_DISABLE;

  /* Set the baud rate */
  descriptor->tty->sercom->BAUD.reg = descriptor->baudrate;

  /* Enable Transmitter and Receiver and set the data size */
  descriptor->tty->sercom->CTRLB.bit.TXEN  = 1;
  descriptor->tty->sercom->CTRLB.bit.RXEN  = 1;
  descriptor->tty->sercom->CTRLB.reg |= USART_DATA8;
  halUartCtrlbSync(descriptor);

  /* enable Rx interrupt */
  descriptor->tty->sercom->INTENSET.bit.RXC = 1;

  /* Enable the Usart */
  descriptor->tty->sercom->CTRLA.bit.ENABLE = 1;
  halUartEnableSync(descriptor);

}
/**************************************************************************//**
  \brief Checking for the Sync. flag for USART register access.

  \param
    descriptor - USART module descriptor.
  \return
   none.
******************************************************************************/
void halUartSwrstSync(HAL_UsartDescriptor_t *descriptor)
{
  while (descriptor->tty->sercom->SYNCBUSY.reg & SERCOM_USART_SYNCBUSY_SWRST);
}
/**************************************************************************//**
  \brief Checking for the Sync. flag for USART register access.

  \param
    descriptor - USART module descriptor.
  \return
   none.
******************************************************************************/
void halUartEnableSync(HAL_UsartDescriptor_t *descriptor)
{
  while (descriptor->tty->sercom->SYNCBUSY.reg & SERCOM_USART_SYNCBUSY_ENABLE);
}
/**************************************************************************//**
  \brief Checking for the Sync. flag for USART register access.

  \param
    descriptor - USART module descriptor.
  \return
   none.
******************************************************************************/
void halUartCtrlbSync(HAL_UsartDescriptor_t *descriptor)
{
  while (descriptor->tty->sercom->SYNCBUSY.reg & SERCOM_USART_SYNCBUSY_CTRLB);
}
#endif // defined(HAL_USE_USART)
// eof halUsart.c
