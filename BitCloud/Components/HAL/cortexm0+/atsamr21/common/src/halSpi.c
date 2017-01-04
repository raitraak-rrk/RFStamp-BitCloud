/**************************************************************************//**
\file  halSpi.c

\brief Implementation of USART SPI mode.

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2012, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal
  History:
    07/04/14 karthik.p_u - Created
******************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/
#if defined(HAL_USE_SPI)
/******************************************************************************
                   Includes section
******************************************************************************/
#include <spi.h>
#include <halDiagnostic.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/

/******************************************************************************
                   Global functions prototypes section
******************************************************************************/

/******************************************************************************
                   Global variables section
******************************************************************************/

/******************************************************************************
                   Implementations section
******************************************************************************/
/*****************************************************************************
Inline function to check the SPI CTRL B BUSY status
*****************************************************************************/
static inline bool halSpiCtrlBSyncing(HAL_SpiDescriptor_t *descriptor)
{
  return (descriptor->tty->sercom->SYNCBUSY.reg & SC0_SPI_SYNCBUSY_CTRLB);
}
/*****************************************************************************
Inline function to check the SPI ENABLE BUSY status
*****************************************************************************/
static inline bool halSpiEnableSyncing(HAL_SpiDescriptor_t *descriptor)
{
  return (descriptor->tty->sercom->SYNCBUSY.reg & SC0_SPI_SYNCBUSY_ENABLE);
}
/*****************************************************************************
Inline function to check the SPI ENABLE BUSY status
*****************************************************************************/
static inline bool halSpiSwrstSyncing(HAL_SpiDescriptor_t *descriptor)
{
  return (descriptor->tty->sercom->SYNCBUSY.reg & SC0_SPI_SYNCBUSY_SWRST);
}
/******************************************************************************
Set the parameters of the SPI.
Parameters:
  descriptor - pointer to the spi descriptor.
******************************************************************************/
static void halSpiFoundationInit(HAL_SpiDescriptor_t *descriptor)
{
  uint8_t sercomNo;
  /* Configure PIOs for SPI */
  GPIO_make_out(&descriptor->tty->spiPinConfig[SPI_CS_SIG]);
  GPIO_make_in(&descriptor->tty->spiPinConfig[SPI_MISO_SIG]);
  GPIO_make_out(&descriptor->tty->spiPinConfig[SPI_MOSI_SIG]);
  GPIO_make_out(&descriptor->tty->spiPinConfig[SPI_SCK_SIG]);

  /* Pin configurations and Configure SPI in peripheral multiplexing */
  GPIO_pinfunc_config(&descriptor->tty->spiPinConfig[SPI_MISO_SIG]);
  GPIO_pinfunc_config(&descriptor->tty->spiPinConfig[SPI_MOSI_SIG]);
  GPIO_pinfunc_config(&descriptor->tty->spiPinConfig[SPI_SCK_SIG]);

  sercomNo = ((uint32_t)descriptor->tty->sercom - (uint32_t)&SC0_SPI_CTRLA) / 0x400;

  /* clock settings */
  GCLK_CLKCTRL_s.id = 0x14 + sercomNo;
  GCLK_CLKCTRL_s.gen = 3;
  GCLK_CLKCTRL_s.clken = 1;

  /* enable the clock of SPI */
  PM_APBCMASK |= (1 << (2 + sercomNo));
}

/******************************************************************************
Set the parameters of the SPI in master mode.
Parameters:
  descriptor - pointer to the spi descriptor.
******************************************************************************/
void halSetSpiConfig(HAL_SpiDescriptor_t *descriptor)
{
  halSpiFoundationInit(descriptor);

  /* Reset */
  descriptor->tty->sercom->CTRLA.bit.SWRST = 1;
  while(halSpiSwrstSyncing(descriptor));

  descriptor->tty->sercom->CTRLB.reg  = SC0_SPI_CTRLB_RXEN;
  while(halSpiCtrlBSyncing(descriptor));

  descriptor->tty->sercom->BAUD.reg = descriptor->baudRate;

  /* setup spi mode master, pad & enable it */
  descriptor->tty->sercom->CTRLA.reg |= SC0_SPI_CTRLA_MODE(3) | SC0_SPI_CTRLA_DOPO(descriptor->tty->dopoConfig)
                                        | SC0_SPI_CTRLA_DIPO(descriptor->tty->dipoConfig) | SC0_SPI_CTRLA_ENABLE;
  while(halSpiEnableSyncing(descriptor));
}
/******************************************************************************
  Inline function (to use in critical sections)
  Writes/reads byte to/from SPI.
  parameters: value - byte to write.
  Returns:    the byte which was read.
******************************************************************************/
static uint32_t hal_WriteByteSpi(HAL_SpiDescriptor_t *descriptor, uint8_t value)
{
  ATOMIC_SECTION_ENTER
  descriptor->tty->sercom->DATA.reg = value;
  while (!(descriptor->tty->sercom->INTFLAG.reg & SC4_SPI_INTFLAG_RXC));
  ATOMIC_SECTION_LEAVE
  return descriptor->tty->sercom->DATA.reg;
}
/******************************************************************************
Write & read a length bytes to & from the SPI.
Parameters:
  tty    -  spi channel
  buffer -  pointer to application data buffer;
  length -  number bytes for transfer;
Returns:
  number of written & read bytes
******************************************************************************/
void halSyncSpiReadData(HAL_SpiDescriptor_t *descriptor, uint8_t *buffer, uint16_t length)
{
  uint16_t i;

  for (i = 0; i < length; i++)
    buffer[i] = hal_WriteByteSpi(descriptor, buffer[i]);

}
/******************************************************************************
Write & read a length bytes to & from the SPI.
Parameters:
  tty    -  spi channel
  buffer -  pointer to application data buffer;
  length -  number bytes for transfer;
Returns:
  number of written & read bytes
******************************************************************************/
void halSyncSpiWriteData(HAL_SpiDescriptor_t *descriptor, uint8_t *buffer, uint16_t length)
{
  uint16_t i;

  for (i = 0; i < length; i++)
    hal_WriteByteSpi(descriptor, buffer[i]);

}

/******************************************************************************
Disables SPI channel.
Parameters:
  tty  -  spi channel.
******************************************************************************/
void halClearSpi(HAL_SpiDescriptor_t *descriptor)
{
  descriptor->tty->sercom->CTRLA.bit.ENABLE = 0x00; // disable
}

#endif // defined(HAL_USE_SPI)

//end of halSpi.c
