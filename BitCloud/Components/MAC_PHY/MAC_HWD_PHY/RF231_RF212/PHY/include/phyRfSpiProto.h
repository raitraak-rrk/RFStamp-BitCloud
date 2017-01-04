/******************************************************************************
  \file phyRfSpiProto.h

  \brief SPI protocol header.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      03/06/07 ALuzhetsky - Created.
      18/01/08 A. Mandychev - Security is supported.
******************************************************************************/
#ifndef _PHYRFSPIPROTO_H
#define _PHYRFSPIPROTO_H

/******************************************************************************
                        Includes section
******************************************************************************/
#include <inttypes.h>
#include <sysTypes.h>
#include <at86rf2xx.h>
#include <halRfSpi.h>
#ifdef _RF_AES_
#include <halRfCtrl.h>
#include <phyEncrypt.h>
#endif

/******************************************************************************
                        Define(s) section
******************************************************************************/
#define phyStopReadFrame() phyStopReadFrameInline()
#define phyStopReadSram() phyStopReadSramInline()

/******************************************************************************
                        Prototypes section
******************************************************************************/
/******************************************************************************
  Writes AT86RF2XX register.
  parameters: addr - register address
              value - data to be written.
******************************************************************************/
void phyWriteRegister(uint8_t addr, uint8_t value);

/******************************************************************************
  Reads AT86RF2XX register.
  parameters: addr - register address.
  returnes:   value which was read.
******************************************************************************/
uint8_t phyReadRegister(uint8_t addr);

/******************************************************************************
  Reads data from AT86RF23X buffer.
  Parameters: buf - pointer to the data buffer.
              length - number of bytes to be read out.
******************************************************************************/
void phyWriteFrame(uint8_t* buf, uint8_t length);

/******************************************************************************
  Reads data from AT86RF23X buffer.
  Parameters: buf - pointer to the data buffer.
              length - number of bytes to be read out.
******************************************************************************/
void phyReadFrame(uint8_t* buf, uint8_t length);

/******************************************************************************
  Starts SPI operation and sends frame read command to the RF chip.
  Parameters: none.
  Returns: none.
******************************************************************************/
void phyStartReadFrame(void);

/******************************************************************************
  Writes data to AT86RF23X SRAM.
  Parameters: buf - pointer to the data buffer.
              address - location of SRAM to start writing to
              length - number of bytes to be read out.
******************************************************************************/
void phyWriteSram(uint8_t* buf, uint8_t address, uint8_t length);

/******************************************************************************
  Reads data from AT86RF23X SRAM.
  Parameters: buf - pointer to the data buffer.
              address - location of SRAM to start reading from
              length - number of bytes to be read out.
******************************************************************************/
void phyReadSram(uint8_t* buf, uint8_t address, uint8_t length);

/******************************************************************************
  Starts SPI operation and sends SRAM read command to the RF chip.
  Parameters: none.
  Returns: none.
******************************************************************************/
void phyStartReadSram(void);

/******************************************************************************
  Does encryption over SPI.
  Parameters:
    request - pointer to request structure.
  Returns: status of operation. PHY_BUSY_RX_REQ_STATUS and
    PHY_SUCCESS_REQ_STATUS are all possible.
******************************************************************************/
#ifdef _RF_AES_
void phyEncryptOverSpi(PHY_EncryptReq_t *req);
#endif

/******************************************************************************
                 Inline static functions prototypes section
******************************************************************************/
/******************************************************************************
  Writes AT86RF2XX registers. Inline realization.
  params: reg - register address and data to be written.
******************************************************************************/
INLINE void phyWriteRegisterInline(uint8_t addr, uint8_t value)
{
  HAL_SelectRfSpi();
  HAL_WriteByteInlineRfSpi(addr | RF_CMD_RW);
  HAL_WriteByteInlineRfSpi(value);
  HAL_DeselectRfSpi();
}

/******************************************************************************
  Reads AT86RF2XX register (inline realization).
  parameters: addr - register address.
  returnes:   value which was read.
******************************************************************************/
INLINE uint8_t phyReadRegisterInline(uint8_t addr)
{
  uint8_t value;

  HAL_SelectRfSpi();
  HAL_WriteByteInlineRfSpi((addr & RF_RADDRM) | RF_CMD_RR);
  value = HAL_WriteByteInlineRfSpi((addr & RF_RADDRM) | RF_CMD_RR);
  HAL_DeselectRfSpi();
  return value;
}

/******************************************************************************
  Reads data from AT86RF23X buffer (inline realization).
  Parameters: buf - pointer to the data buffer.
              length - number of bytes to be read out.
******************************************************************************/
INLINE void phyReadFrameInline(uint8_t* buf, uint8_t length)
{
  while (length--)
  {
    *(buf++) = HAL_WriteByteInlineRfSpi(RF_CMD_FR); //dummy write (to read)
  }
}

/******************************************************************************
  Starts SPI operation and sends frame read command to the RF chip (inline
  realization).
  Parameters: none.
  Returns: none.
******************************************************************************/
INLINE void phyStartReadFrameInline(void)
{
  HAL_SelectRfSpi();
  HAL_WriteByteInlineRfSpi(RF_CMD_FR);
}

/******************************************************************************
  Finishes SPI operation.
  Parameters: none.
  Returns: none.
******************************************************************************/
INLINE void phyStopReadFrameInline(void)
{
  HAL_DeselectRfSpi();
}

/******************************************************************************
  Reads data from AT86RF23X SRAM (inline realization).
  Parameters: buf - pointer to the data buffer.
              address - location of SRAM to start reading from
              length - number of bytes to be read out.
******************************************************************************/
INLINE void phyReadSramInline(uint8_t* buf, uint8_t address, uint8_t length)
{
  HAL_WriteByteInlineRfSpi(address);
  while (length--)
  {
    *(buf++) = HAL_WriteByteInlineRfSpi(RF_CMD_SR); //dummy write (to read)
  }
}

/******************************************************************************
  Starts SPI operation and sends SRAM read command to the RF chip (inline
  realization).
  Parameters: none.
  Returns: none.
******************************************************************************/
INLINE void phyStartReadSramInline(void)
{
  HAL_SelectRfSpi();
  HAL_WriteByteInlineRfSpi(RF_CMD_SR);
}

/******************************************************************************
  Finishes SPI operation.
  Parameters: none.
  Returns: none.
******************************************************************************/
INLINE void phyStopReadSramInline(void)
{
  HAL_DeselectRfSpi();
}

#endif /* _PHYRFSPIPROTO_H */

//eof phyRfSpiProto.h
