/****************************************************************************//**
  \file spi.h

  \brief The header file describes the USART SPI mode (for avr) and SPI1 bus (for arm).

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    29/05/08 A. Khromykh - Created
    07/04/14 karthik.p_u - Modified
 ******************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/

#ifndef _SPI_H
#define _SPI_H

/******************************************************************************
                   Includes section
******************************************************************************/
#if defined(AT91SAM7X256)
  #include <halMemSpi.h>
#elif defined(ATMEGA1281) || defined(ATMEGA2561) || defined(ATMEGA1284) || \
      defined(AT90USB1287) || defined(ATMEGA128RFA1) || defined(ATMEGA256RFR2) || \
      defined(ATXMEGA128A1) || defined(ATXMEGA256A3) || defined(ATXMEGA128B1) || \
      defined(ATXMEGA256D3) || defined(ATMEGA2564RFR2) || defined(ATSAMR21G18A) || \
      defined(ATSAMR21E18A) || defined(ATSAMR21E19A)
  #include <halSpi.h>
#endif

/******************************************************************************
                   Types section
******************************************************************************/
/** \brief Spi descriptor */
typedef struct
{
  /** \brief tty - SPI_CHANNEL_n to be used. "n" range depends on the platform.
  Take a look into halSpi.h platform specific file fore more details. */
  SpiChannel_t tty;
  /** \brief SpiClockMode_t clockMode - spi clock mode (set by user). Must be chosen from: \n
    SPI_CLOCK_MODE0 \n
    SPI_CLOCK_MODE1 \n
    SPI_CLOCK_MODE2 \n
    SPI_CLOCK_MODE3 \n */
  SpiClockMode_t clockMode;
#if defined(AT91SAM7X256)
  /** \brief parameters are valid only for arm: */
  /** \brief symbol size (bits) (set by user). Must be set: \n
    SPI_8BITS_SYMBOL  \n
    SPI_9BITS_SYMBOL  \n
    SPI_10BITS_SYMBOL \n
    SPI_11BITS_SYMBOL \n
    SPI_12BITS_SYMBOL \n
    SPI_13BITS_SYMBOL \n
    SPI_14BITS_SYMBOL \n
    SPI_15BITS_SYMBOL \n
    SPI_16BITS_SYMBOL \n */
  SpiSymbolSize_t symbolSize;
  /** \brief Must be use macros HAL_SPI_PACK_PARAMETER(freq_value, valid_delay_value, cons_delay_value). \n
  Where is: \n
    freq_value        - spi frequency (Hz) \n
    valid_delay_value - delay between setting CS and clock start (sec) \n
    cons_delay_value  - delay between consecutive transfers (sec) \n */
  uint32_t pack_parameter;
#elif defined(ATMEGA1281) || defined(ATMEGA2561) || defined(ATMEGA1284) || defined(AT90USB1287) || \
      defined(ATMEGA128RFA1) || defined(ATMEGA256RFR2) || defined(ATXMEGA128A1) || defined(ATXMEGA256A3) || \
      defined(ATXMEGA128B1) || defined(ATXMEGA256D3) || defined(ATMEGA2564RFR2)
  /** \brief parameters are valid only for avr:*/
  /** \brief spi data order (set by user). Must be chosen from: \n
    SPI_DATA_MSB_FIRST \n
    SPI_DATA_LSB_FIRST \n */
  SpiDataOrder_t dataOrder;
  /** \brief spi clock rate (set by user). Must be chosen from: \n
    for atmega         \n
    SPI_CLOCK_RATE_62  \n
    SPI_CLOCK_RATE_125 \n
    SPI_CLOCK_RATE_250 \n
    SPI_CLOCK_RATE_500 \n

    for xmega            mcu clock            \n
    SPI_CLOCK_RATE_125   (4 MHz)              \n
    SPI_CLOCK_RATE_250   (4, 8 MHz)           \n
    SPI_CLOCK_RATE_500   (4, 8, 16 MHz)       \n
    SPI_CLOCK_RATE_750   (12 MHz )            \n
    SPI_CLOCK_RATE_1000  (4, 8, 16, 32 MHz)   \n
    SPI_CLOCK_RATE_1500  (12 MHz)             \n
    SPI_CLOCK_RATE_2000  (4, 8, 16, 32 MHz)   \n
    SPI_CLOCK_RATE_3000  (12 MHz)             \n
    SPI_CLOCK_RATE_4000  (8, 16, 32 MHz)      \n
    SPI_CLOCK_RATE_6000  (12 MHz)             \n
    SPI_CLOCK_RATE_8000  (16, 32 MHz)         \n
    SPI_CLOCK_RATE_16000 (32 MHz) */
  SpiBaudRate_t  baudRate;
  /** \brief HAL USART service field - contains variables for HAL USART module
    internal needs */
  HAL_UsartDescriptor_t spiDescriptor;
#elif defined(ATSAMR21G18A) || defined(ATSAMR21E18A) || defined(ATSAMR21E19A)
    /** \brief parameters are valid only for avr:*/
  /** \brief spi data order (set by user). Must be chosen from: \n
    SPI_DATA_MSB_FIRST \n
    SPI_DATA_LSB_FIRST \n */
  SpiDataOrder_t dataOrder;
  /** \brief spi clock rate (set by user). Must be chosen from: \n
    for atmega         \n
    SPI_CLOCK_RATE_62  \n
    SPI_CLOCK_RATE_125 \n
    SPI_CLOCK_RATE_250 \n
    SPI_CLOCK_RATE_500 \n

    for xmega            mcu clock            \n
    SPI_CLOCK_RATE_125   (4 MHz)              \n
    SPI_CLOCK_RATE_250   (4, 8 MHz)           \n
    SPI_CLOCK_RATE_500   (4, 8, 16 MHz)       \n
    SPI_CLOCK_RATE_750   (12 MHz )            \n
    SPI_CLOCK_RATE_1000  (4, 8, 16, 32 MHz)   \n
    SPI_CLOCK_RATE_1500  (12 MHz)             \n
    SPI_CLOCK_RATE_2000  (4, 8, 16, 32 MHz)   \n
    SPI_CLOCK_RATE_3000  (12 MHz)             \n
    SPI_CLOCK_RATE_4000  (8, 16, 32 MHz)      \n
    SPI_CLOCK_RATE_6000  (12 MHz)             \n
    SPI_CLOCK_RATE_8000  (16, 32 MHz)         \n
    SPI_CLOCK_RATE_16000 (32 MHz) */
  SpiBaudRate_t  baudRate;
#endif
  union
  {
    /** \brief address of the function to notify the application when a transmission was completed. \n
               To use the synchronous method, set callback to NULL. \n */
    void (* callback)(void);
    /** \brief address of the function to notify the application when a byte was received. \n
               Slave mode uses only asynchronous method. However, the callback can be set to NULL. \n */
    void (* slave_callback)(uint16_t);
  };
} HAL_SpiDescriptor_t;

/**************************************************************************//**
\brief Open the SPI interface and configure pins.

\ingroup hal_spi

\param[in]
    descriptor - pointer to the spi descriptor.
\return
   -1 - there not are free resources.
    0 - SPI channel is ready.
******************************************************************************/
int HAL_OpenSpi(HAL_SpiDescriptor_t *descriptor);

/**************************************************************************//**
\brief Close the SPI channel and pins.

\ingroup hal_spi

\param[in]
  descriptor - pointer to the spi descriptor.
\return
  Returns 0 on success or -1 if channel was not opened.
******************************************************************************/
int HAL_CloseSpi(HAL_SpiDescriptor_t *descriptor);

/**************************************************************************//**
\brief Writes a length bytes to the SPI. \n
 Callback function will be used to notify about the finishing transmitting.
 (only for master spi)

\ingroup hal_spi

 \param[in]
  descriptor -  pointer to spi descriptor
\param[in]
  buffer     -  pointer to application data buffer;
\param[in]
  length     -  number bytes for transfer;
\return
  -1 - spi module was not opened, there is unsent data, pointer to the data or
       the length are zero; \n
   0 - on success or a number; \n
   Number of written bytes if the synchronous method is used(callback is NULL), \n
   only for master spi.
******************************************************************************/
int HAL_WriteSpi(HAL_SpiDescriptor_t *descriptor, uint8_t *buffer, uint16_t length);

/**************************************************************************//**
\brief For master : writes a number of bytes to the spi.\n
  Callback function will be used to notify when the activity is finished.\n
  The read data is placed to the buffer. \n
  For slave: reads a number of bytes from internal spi buffer and writes them \n
  to application buffer.

\ingroup hal_spi

\param[in]
  descriptor - pointer to HAL_SpiDescriptor_t structure
\param[in]
  buffer -  pointer to the application data buffer
\param[in]
  length -  number of bytes to transfer
\return
  -1 - spi module was not opened, or there is unsent data, or the pointer to
   data or the length are NULL; \n
   0 - success for master; \n
   Number of written bytes if the synchronous method is used(callback is NULL) for master \n
   or number of read bytes from internal buffer to the application buffer for slave.
******************************************************************************/
int HAL_ReadSpi(HAL_SpiDescriptor_t *descriptor, uint8_t *buffer, uint16_t length);

/**************************************************************************//**
\brief Configure the usart pin based on the board
******************************************************************************/
#if defined(ATSAMR21G18A) || defined(ATSAMR21E18A) || defined(ATSAMR21E19A)
void BSP_BoardSpecificSpiPinInit(SpiChannel_t tty);
#endif
#endif /* _SPI_H */
// eof spi.h
