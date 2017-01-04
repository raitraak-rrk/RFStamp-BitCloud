/*****************************************************************************//**
\file  halRfSpi.h

\brief SPI interface routines header.

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal
  History:
    07/04/14 karthik.p_u - Created
**********************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/

#ifndef _HALRFSPI_H
#define _HALRFSPI_H

/******************************************************************************
                   Includes section
******************************************************************************/
#include <sysTypes.h>
#include <atsamr21.h>
#include <atomic.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/

#define SC1_SPI_MODE_SPI_MASTER  SC1_SPI_CTRLA_MODE(0x03)
#define GCLK_GENERATOR_0         (0)

enum spiDataOrder {
	/** The LSB of the data is transmitted first */
	SPI_DATA_ORDER_LSB      = SC1_SPI_CTRLA_DORD,
	/** The MSB of the data is transmitted first */
	SPI_DATA_ORDER_MSB      = 0,
};

enum spiCharacterSize {
	/** 8 bit character */
	SPI_CHARACTER_SIZE_8BIT = SC1_SPI_CTRLB_CHSIZE(0),
	/** 9 bit character */
	SPI_CHARACTER_SIZE_9BIT = SC1_SPI_CTRLB_CHSIZE(1),
};

/******************************************************************************
                   Prototypes section
******************************************************************************/
/******************************************************************************
Performs hwi initialization of SPI(RF) interface.
******************************************************************************/
void HAL_InitRfSpi(void);

/******************************************************************************
  Writes/reads byte to/from SPI.
  parameters: value - byte to write.
  Returns:    the byte which was read.
******************************************************************************/
uint32_t HAL_WriteByteRfSpi(uint8_t value);

/******************************************************************************
  Deselects a slave device.
******************************************************************************/
void HAL_DeselectRfSpi(void);

/******************************************************************************
  Selects a slave device.
******************************************************************************/
void HAL_SelectRfSpi(void);

/******************************************************************************
                   Inline static functions section
******************************************************************************/
/******************************************************************************
  Inline function (to use in critical sections)
  Writes/reads byte to/from SPI.
  parameters: value - byte to write.
  Returns:    the byte which was read.
******************************************************************************/
static inline uint32_t HAL_WriteByteInlineRfSpi(uint8_t value)
{
  ATOMIC_SECTION_ENTER
  SC4_SPI_DATA = value;
  while (!(SC4_SPI_INTFLAG & SC4_SPI_INTFLAG_RXC));
  ATOMIC_SECTION_LEAVE
  return SC4_SPI_DATA;
}
/*****************************************************************************
Inline function to check the SPI CTRL B BUSY status
*****************************************************************************/
static inline bool halRfSpiCtrlBSyncing()
{
  return (SC4_SPI_SYNCBUSY & SC4_SPI_SYNCBUSY_CTRLB);
}
/*****************************************************************************
Inline function to check the SPI ENABLE BUSY status
*****************************************************************************/
static inline bool halRfSpiEnableSyncing()
{
  return (SC4_SPI_SYNCBUSY & SC4_SPI_SYNCBUSY_ENABLE);
}
/*****************************************************************************
Inline function to check the SPI ENABLE BUSY status
*****************************************************************************/
static inline bool halRfSpiSwrstSyncing()
{
  return (SC4_SPI_SYNCBUSY & SC4_SPI_SYNCBUSY_SWRST);
}
#endif /* _HALRFSPI_H */

//eof halRfSpi.h
