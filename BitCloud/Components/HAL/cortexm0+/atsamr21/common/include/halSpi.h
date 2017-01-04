/*****************************************************************************//**
\file  halSpi.h

\brief  Declarations of USART SPI mode.

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
#ifndef _HALSPI_H
#define _HALSPI_H

/******************************************************************************
                   Includes section
******************************************************************************/
#include <halClkCtrl.h>
#include <halTaskManager.h>
#include <sysTypes.h>
#include <gpio.h>
#include <spi.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
#define SPI_CHANNEL_0             &spiChannel[0]
#define SPI_CHANNEL_1             &spiChannel[1]
#define NUMBER_OF_SPI_CHANNELS    2

/******************************************************************************
                   Types section
******************************************************************************/

typedef enum
{
  SPI_CS_SIG,
  SPI_SCK_SIG,
  SPI_MOSI_SIG,
  SPI_MISO_SIG,
  TOTAL_NO_OF_SPI_SIGNALS
} spiSignalNum_t;

// spi channel
typedef struct
{
  SercomSpi* sercom;
  PinConfig_t spiPinConfig[TOTAL_NO_OF_SPI_SIGNALS];
  uint8_t dipoConfig;
  uint8_t dopoConfig;
} SpiChannelInfo_t;


// types of the clock mode
typedef enum
{
  // leading edge sample RX bit (rising), trailing edge setup TX bit (falling).
  SPI_CLOCK_MODE0,
  // leading edge setup TX bit (rising), trailing edge sample RX bit (falling).
  SPI_CLOCK_MODE1,
  // leading edge sample RX bit (falling), trailing edge setup TX bit (rising).
  SPI_CLOCK_MODE2,
  // leading edge setup TX bit (falling), trailing edge sample RX bit (rising).
  SPI_CLOCK_MODE3,
} SpiClockMode_t;

// clock rate
typedef enum
{
  SPI_CLOCK_RATE_62 =  ((F_PERI / (2 * 62500ul)) - 1),
  SPI_CLOCK_RATE_125 =  ((F_PERI / (2 * 125000ul)) - 1),
  SPI_CLOCK_RATE_250 =  ((F_PERI / (2 * 250000ul)) - 1),
  SPI_CLOCK_RATE_500 =  ((F_PERI / (2 * 500000ul)) - 1),
  SPI_CLOCK_RATE_1000 = ((F_PERI / (2 * 1000000ul)) - 1),
  SPI_CLOCK_RATE_2000 = ((F_PERI / (2 * 2000000ul)) - 1)
} SpiBaudRate_t;

// Data order
typedef enum
{
  SPI_DATA_MSB_FIRST, // data with MSB first
  SPI_DATA_LSB_FIRST  // data with LSB first
} SpiDataOrder_t;

typedef SpiChannelInfo_t *SpiChannel_t;
/******************************************************************************
                   Extern section
******************************************************************************/
extern SpiChannelInfo_t spiChannel[NUMBER_OF_SPI_CHANNELS];
/******************************************************************************
                   Prototypes section
******************************************************************************/
#endif

//eof halSpi.h
