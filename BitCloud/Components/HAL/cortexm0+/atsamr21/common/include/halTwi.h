/**************************************************************************//**
  \file  halTwi.h

  \brief Declaration of of Two wire interface interface module.

  \author
      Atmel Corporation: http://www.atmel.com \n
      Support email: avr@atmel.com

    Copyright (c) 2008-2012, Atmel Corporation. All rights reserved.
    Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
     28/02/14 Viswanadham Kotla - Created
 ******************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/

#ifndef _HALTWI_H
#define _HALTWI_H

/******************************************************************************
                   Includes section
******************************************************************************/
#include <gpio.h>
#include <halClkCtrl.h>
#include <i2cPacket.h>
#include <halDiagnostic.h>
#include <halTaskManager.h>
/******************************************************************************
                   Definitions section
******************************************************************************/
#define TWI_CHANNEL_0    &twiChannel[0]

#define I2C_MASTER_START_HOLD_TIME_300NS_600NS   SERCOM_I2CM_CTRLA_SDAHOLD(2)

#define RESPONSE_TIMEOUT 65535ul

/******************************************************************************
                   Types section
******************************************************************************/
// i2c channel
typedef enum
{
  I2C_SDA_SIG,
  I2C_SCK_SIG,
  TOTAL_NO_OF_I2C_SIGNALS
} i2cSignalNum_t;

typedef struct
{
  SercomI2cm* sercom;
  PinConfig_t i2cPinConfig[TOTAL_NO_OF_I2C_SIGNALS];
  uint8_t sdaPadConfig;
  uint8_t sckPadConfig;
} I2cChannelInfo_t;
 
typedef I2cChannelInfo_t *I2cChannel_t;

typedef enum 
{
	I2C_WRITE = 0,
	I2C_READ = 1,
}I2cTransferDirection_t;

/** \brief i2c baud rate */
typedef enum
{
	/** Baud rate at 100kHz (Standard-mode) */
	I2C_CLOCK_RATE_100 = (int32_t)((F_PERI / (2000*100)) - 5),
	/** Baud rate at 400kHz (Fast-mode) */
	I2C_CLOCK_RATE_400 = (int32_t)((F_PERI / (2000*400)) - 5),
	/** Baud rate at 1MHz (Fast-mode Plus) */
	I2C_CLOCK_RATE_1000 = (int32_t)((F_PERI / (2000*1000)) - 5),
	/** Baud rate at 3.4MHz (High-speed mode) */
	I2C_CLOCK_RATE_3400 = (int32_t)((F_PERI / (2000*3400)) - 5),

}I2cClockRate_t;

/** \brief internal address size */
typedef enum
{
  /* AT91C_TWI_IADRSZ_NO */
  HAL_NO_INTERNAL_ADDRESS = (0x0ul << 8),
  /* AT91C_TWI_IADRSZ_1_BYTE */
  HAL_ONE_BYTE_SIZE       = (0x1ul << 8),
  /* AT91C_TWI_IADRSZ_2_BYTE */
  HAL_TWO_BYTE_SIZE       = (0x2ul << 8),
  /* AT91C_TWI_IADRSZ_3_BYTE */
  HAL_THREE_BYTE_SIZE     = (0x3ul << 8)
} I2cInternalAddrSize_t;

// i2c virtual state
typedef enum
{
  I2C_PACKET_CLOSE = 0,
  I2C_PACKET_IDLE,
  I2C_PACKET_WRITE_ADDRESS,
  I2C_PACKET_WRITE_DATA,
  I2C_PACKET_READ_ADDRESS,
  I2C_PACKET_READ_DATA,
  I2C_PACKET_TRANSAC_SUCCESS,
  I2C_PACKET_TRANSAC_FAIL
} I2cPacketStates_t;

typedef struct
{
  volatile uint16_t index;             // current index of read/write byte
  volatile I2cPacketStates_t state;
} HalI2cPacketService_t;

extern I2cChannelInfo_t twiChannel[1];

/******************************************************************************
                   Prototypes section
******************************************************************************/
/**************************************************************************//**
\brief Inits TWI module. Setup the speed of TWI.

\param[in]
  rate - the speed of TWI.
******************************************************************************/
void halInitI2c(I2cClockRate_t rate);

/******************************************************************************
\brief start write transaction
******************************************************************************/
void halSendStartWriteI2c(void);

/******************************************************************************
\brief start read transaction
******************************************************************************/
void halSendStartReadI2c(void);

#endif /* _I2C_H*/
// eof i2c.h
