/*********************************************************************************************//**
\file

\brief D_XNv Drivers Implementation

This file implements a few basic 'static inline' functions that are intended to be used in a
certain way. This is as follows:

For any Flash command, the flash chip must first be selected using SelectFlash().
Each command ends with de-selection of the flash chip by calling DeselectFlash().
Within a command, a byte can be transferred over SPI to the flash chip using this sequence:
 - SpiTxByte(uint8_t b);  // initiates the transfer of a byte over SPI
 - SpiTxWaitReady();    // waits until the transfer is ready
When the flash chip sends back a byte (i.e. a response), a dummy byte is sent and the response is read:
 - SpiTxByte(0x00u);    // initiates the transfer of a dummy byte over SPI
 - SpiTxWaitReady();    // waits until the transfer is ready
 - byte = SpiRxByte();  // reads the byte
To optimize speed, any calculations should be done between SpiTxByte() and SpiTxWaitReady(), because
the SpiTxWaitReady() will be ready about 2 microseconds after the SpiTxByte().

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal

$Id: D_XNv-ATmega.c 24615 2013-02-27 05:14:24Z mahendranp $

***************************************************************************************************/

/***************************************************************************************************
* LOCAL INCLUDE FILES
***************************************************************************************************/

#include "D_XNv_Bindings.h"
#include "D_XNv_Init.h"
#include "D_XNv.h"
#include "N_Types.h"
#include <appTimer.h>

/***************************************************************************************************
* EXTERNAL INCLUDE FILES
***************************************************************************************************/

#include <ioavr.h>
#include <intrinsics.h>

#if defined(ENABLE_NV_USAGE_SIMULATION)
#include "N_Log.h"
#endif

/***************************************************************************************************
* LOCAL MACROS AND CONSTANTS
***************************************************************************************************/

#define COMPID "D_XNv"

#define D_XNV_PAGE_PROGRAM_CMD  0x02u
#define D_XNV_READ_CMD          0x03u
#define D_XNV_READ_STATUS_CMD   0x05u
#define D_XNV_WRITE_ENABLE_CMD  0x06u
#define D_XNV_SECTOR_ERASE_CMD  0xD8u

#define D_XNV_STATUS_WIP        0x01u

#define DDR_SPI                 DDRB

// Direction bits for SS, SCK and MOSI
#define DD_SS                   DDRB0
#define DD_SCK                  DDRB1
#define DD_MOSI                 DDRB2
#define DD_MISO                 DDRB3

#if !defined(D_XNV_PIN_CS)
#error D_XNV_PIN_CS must be defined in your config file. See D_XNv_Init.h
#endif

#if (D_XNV_PIN_CS & 0xF0u) == 0x00u
#   define D_XNV_CS_PORT    PORTA
#   define D_XNV_CS_DIR     DDRA
#elif (D_XNV_PIN_CS & 0xF0u) == 0x10u
#   define D_XNV_CS_PORT    PORTB
#   define D_XNV_CS_DIR     DDRB
#elif (D_XNV_PIN_CS & 0xF0u) == 0x20u
#   define D_XNV_CS_PORT    PORTC
#   define D_XNV_CS_DIR     DDRC
#elif (D_XNV_PIN_CS & 0xF0u) == 0x40u
#   define D_XNV_CS_PORT    PORTD
#   define D_XNV_CS_DIR     DDRD
#else
#error
#endif

#define D_XVN_CS_VALUE  (uint8_t)(1u << (D_XNV_PIN_CS & 0x07u))

/***************************************************************************************************
* LOCAL VARIABLES
***************************************************************************************************/
// The default s_pfSystemCheckCallback must be NULL - app to specify callback 
//to handle sys integrity checks
static D_XNv_SystemCheckCallback_t s_pfSystemCheckCallback = NULL;

/***************************************************************************************************
* LOCAL FUNCTIONS
***************************************************************************************************/

static inline void Delay100ns(void)
{
    // At 16 MHz, 2 nops is just above 100ns.
    asm("nop");
    asm("nop");
}

static inline void SelectFlash(void)
{
#if !defined BOOTLOADER
    // Let's check the system integrity before acessing the flash chip to prevent NV corruption
    if (s_pfSystemCheckCallback != NULL)
    {
       s_pfSystemCheckCallback();
    }
#else
   (void)s_pfSystemCheckCallback;
#endif

    Delay100ns();   // for tSHSL (a.k.a. tCPH or tCEH): >= 100ns

    // Make CSn low
    D_XNV_CS_PORT &= ~D_XVN_CS_VALUE;
}

static void DeselectFlash(void)
{
    // Make CSn high
    D_XNV_CS_PORT |= D_XVN_CS_VALUE;
}

static inline void SpiTxByte(uint8_t value)
{
    SPDR = value;
}

static inline void SpiTxWaitReady(void)
{
    // Wait for transmission complete
    while ( !(SPSR & (1 << SPIF)) )
    {
        // no action
    }
}

static inline void SpiTxByteAndWait(uint8_t value)
{
    SpiTxByte(value);
    SpiTxWaitReady();
}

/** Receive a byte that was transferred during a transmit */
static inline uint8_t SpiRxByte(void)
{
    return SPDR;
}

/** Wait unit a previous write or sector erase has finished. This can take really long! */
static void WaitUntilWriteFinished(void)
{
	uint8_t status;
	do
	{
		SelectFlash();
		SpiTxByteAndWait(D_XNV_READ_STATUS_CMD);
		SpiTxByteAndWait(0u);
		status = SpiRxByte();
		DeselectFlash();
	}
	while ( status & D_XNV_STATUS_WIP );
}

static inline uint8_t ReadSingleByte(void)
{
    SpiTxByteAndWait(0u);
    return SpiRxByte();
}

/***************************************************************************************************
* EXPORTED FUNCTIONS
***************************************************************************************************/
void D_XNv_Init(D_XNv_SystemCheckCallback_t pfCallback)
{
    // Set MOSI and SCK output, all others input
    DDR_SPI |= (1 << DD_MOSI) | (1 << DD_SCK) | (1 << DD_SS);
    DDR_SPI &= ~(1 << DD_MISO);

    // Enable SPI, Master, set clock rate fck/2
    SPCR = (1 << SPE) | (1 << MSTR);

    // Enable Double SPI Speed
    SPSR = (1 << SPI2X);

    // Configure the CS pin as output
    D_XNV_CS_DIR |= D_XVN_CS_VALUE;

    DeselectFlash();

#if !defined BOOTLOADER
    /* Set callback function */
    if (pfCallback != NULL)
      s_pfSystemCheckCallback = pfCallback;
#else
    (void)s_pfSystemCheckCallback;
#endif
}

void D_XNv_Read_Impl(uint8_t sector, uint16_t offset, uint8_t *pBuffer, D_XNv_Size_t numberOfBytes)
{
    WaitUntilWriteFinished();

    SelectFlash();
    SpiTxByteAndWait(D_XNV_READ_CMD);
    SpiTxByteAndWait(sector);
    SpiTxByteAndWait((uint8_t)(offset >> 8));
    SpiTxByteAndWait((uint8_t)offset);

    while ( numberOfBytes != 0u )
    {
        *pBuffer = ReadSingleByte();
        pBuffer++;

        numberOfBytes--;
    }

    DeselectFlash();
}

void D_XNv_Write_Impl(uint8_t sector, uint16_t offset, const uint8_t *pBuffer, D_XNv_Size_t numberOfBytes)
{
    while (numberOfBytes != 0u)
    {
        WaitUntilWriteFinished();

        SelectFlash();
        SpiTxByteAndWait(D_XNV_WRITE_ENABLE_CMD);
        DeselectFlash();

        SelectFlash();
        SpiTxByteAndWait(D_XNV_PAGE_PROGRAM_CMD);
        SpiTxByteAndWait(sector);
        SpiTxByteAndWait((uint8_t)(offset >> 8));
        SpiTxByteAndWait((uint8_t)offset);
        do
        {
            SpiTxByteAndWait(*pBuffer);
            pBuffer++;
            numberOfBytes--;
            offset++;
        }
        while ( ( numberOfBytes != 0u ) && ( (offset & 0x00FFu) != 0x0000u ) );
        DeselectFlash();
    }
}

void D_XNv_EraseSector_Impl(uint8_t sector)
{
#if defined(ENABLE_NV_USAGE_SIMULATION)
    N_LOG_ALWAYS(("EraseSector,%hu", sector));
#endif
    WaitUntilWriteFinished();

    SelectFlash();
    SpiTxByteAndWait(D_XNV_WRITE_ENABLE_CMD);
    DeselectFlash();

    SelectFlash();
    SpiTxByteAndWait(D_XNV_SECTOR_ERASE_CMD);
    SpiTxByteAndWait(sector);
    SpiTxByteAndWait(0x00u);
    SpiTxByteAndWait(0x00u);
    DeselectFlash();
}

bool D_XNv_IsEmpty_Impl(uint8_t sector, uint16_t offset, D_XNv_Size_t numberOfBytes)
{
    WaitUntilWriteFinished();

    SelectFlash();
    SpiTxByteAndWait(D_XNV_READ_CMD);
    SpiTxByteAndWait(sector);
    SpiTxByteAndWait((uint8_t)(offset >> 8));
    SpiTxByteAndWait((uint8_t)offset);

    uint8_t andedContent = 0xFFu;
    do
    {
        andedContent &= ReadSingleByte();
        numberOfBytes--; // will wrap from 0 to 0xFFFF when numberOfBytes equals 0 on entry
    }
    while ((andedContent == 0xFFu) && (numberOfBytes != 0u));

    DeselectFlash();

    return (andedContent == 0xFFu);
}

bool D_XNv_IsEqual_Impl(uint8_t sector, uint16_t offset, uint8_t *pBuffer, D_XNv_Size_t numberOfBytes)
{
    bool isEqual = TRUE;

    WaitUntilWriteFinished();

    SelectFlash();
    SpiTxByteAndWait(D_XNV_READ_CMD);
    SpiTxByteAndWait(sector);
    SpiTxByteAndWait((uint8_t)(offset >> 8));
    SpiTxByteAndWait((uint8_t)offset);

    while ((isEqual) && (numberOfBytes != 0u))
    {
        isEqual = (*pBuffer == ReadSingleByte());
        pBuffer++;

        numberOfBytes--;
    }

    DeselectFlash();

    return isEqual;
}
