/**************************************************************************//**
\file  halSpi.c

\brief SPI interface routine.

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

/******************************************************************************
                   Includes section
******************************************************************************/
#include <halRfSpi.h>
#include <halRfPio.h>
#include <halAppClock.h>
#include <halTaskManager.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/

/******************************************************************************
                   Implementations section
******************************************************************************/
/******************************************************************************
  Performs hardware initialization of SPI0 interface.
******************************************************************************/
void HAL_InitRfSpi(void)
{
  /* Configure PIOs for SPI */
  GPIO_RF_SLP_TR_make_out();
  GPIO_RF_SLP_TR_clr();
  GPIO_RF_RST_make_out();
  GPIO_RF_IRQ_make_in();
  GPIO_RF_CS_make_out();
  GPIO_RF_MISO_make_in();
  GPIO_RF_MOSI_make_out();
  GPIO_RF_SCK_make_out();
  GPIO_RF_MISO_make_pullup();

  /* samR21 custom board */
  /* pin configurations */
  PORTB_PINCFG30 |= PORTA_PINCFG30_PMUXEN; //MOSI  B30
  PORTC_PINCFG19 |= PORTC_PINCFG19_PMUXEN; //MISO  C19
  PORTC_PINCFG18 |= PORTC_PINCFG18_PMUXEN; //SCK   C18
  PORTB_PINCFG0 |= PORTB_PINCFG0_PMUXEN; //IRQ     B0

  /* Configure SPI in peripheral multiplexing */
  PORTB_PMUX15 = PORTB_PMUX15_PMUXE(5);
  PORTC_PMUX9 = PORTC_PMUX9_PMUXE(5) | PORTC_PMUX9_PMUXO(5);

  /* Enable the clock of SPI */
  PM_APBCMASK |= PM_APBCMASK_SERCOM4;

  /* Configure the generic clk settings */
  GCLK_CLKCTRL_s.id = 0x18;  // enabling clock for SERCOM0
  GCLK_CLKCTRL_s.gen = 3;
  GCLK_CLKCTRL_s.clken = 1;

  /* Reset */
  SC4_SPI_CTRLA = SC4_SPI_CTRLA_SWRST;
  while(halRfSpiSwrstSyncing());

  SC4_SPI_CTRLB  =/*|= SPI_CHARACTER_SIZE_8BIT |*/ SC4_SPI_CTRLB_RXEN;
  while(halRfSpiCtrlBSyncing());

  /* setup spi mode master, pad & enable it */
  SC4_SPI_CTRLA |=  SC4_SPI_CTRLA_MODE(3) | SC4_SPI_CTRLA_DOPO(1) | SC4_SPI_CTRLA_ENABLE;
  while(halRfSpiEnableSyncing());
}

/******************************************************************************
  Writes/reads byte to/from SPI.
  parameters: value - byte to write.
  Returns:    the data which was read.
******************************************************************************/
uint32_t HAL_WriteByteRfSpi(uint8_t value)
{
  return HAL_WriteByteInlineRfSpi(value);
}

/******************************************************************************
  Deselects a slave device.
******************************************************************************/
void HAL_DeselectRfSpi(void)
{
  GPIO_RF_CS_set();
}

/******************************************************************************
  Selects a slave device.
******************************************************************************/
void HAL_SelectRfSpi(void)
{
  GPIO_RF_CS_clr();
}
// eof halRfSpi.c
