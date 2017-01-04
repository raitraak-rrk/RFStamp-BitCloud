/**************************************************************************//**
  \file  spi.c

  \brief Implementation of USART SPI mode, hardware-independent module.

  \author
      Atmel Corporation: http://www.atmel.com \n
      Support email: avr@atmel.com

    Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
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
                   Global functions prototypes section
******************************************************************************/

/******************************************************************************
                   External global variables section
******************************************************************************/
SpiChannelInfo_t spiChannel[NUMBER_OF_SPI_CHANNELS];

/******************************************************************************
                   Static functions prototypes section
******************************************************************************/
extern void halSyncSpiReadData(HAL_SpiDescriptor_t *descriptor, uint8_t *buffer, uint16_t length);
extern void halSyncSpiWriteData(HAL_SpiDescriptor_t *descriptor, uint8_t *buffer, uint16_t length);
extern void halSetSpiConfig(HAL_SpiDescriptor_t *descriptor);
extern void halClearSpi(HAL_SpiDescriptor_t *descriptor);
/******************************************************************************
                   Implementations section
******************************************************************************/
/**************************************************************************//**
\brief Open the SPI interface.
\param[in]
    descriptor - pointer to the spi descriptor.
\return
   -1 - there not are free resources.
    0 - SPI channel is ready.
******************************************************************************/
int HAL_OpenSpi(HAL_SpiDescriptor_t *descriptor)
{
  BSP_BoardSpecificSpiPinInit(descriptor->tty);
  halSetSpiConfig(descriptor);
  return 0;
}

/**************************************************************************//**
\brief Close the SPI channel and pins.
\param[in]
  descriptor - pointer to the spi descriptor.
\return
  Returns 0 on success or -1 if channel was not opened.
******************************************************************************/
int HAL_CloseSpi(HAL_SpiDescriptor_t *descriptor)
{
  halClearSpi(descriptor);
  return 0;
}

/**************************************************************************//**
\brief Writes a length bytes to the SPI. \n
 Callback function will be used to notify about the finishing transmitting.
 (only for master spi)
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
int HAL_WriteSpi(HAL_SpiDescriptor_t *descriptor, uint8_t *buffer, uint16_t length)
{
  if (!buffer || !length)
    return -1;

  halSyncSpiWriteData(descriptor, buffer, length);
  return 0;
}

/**************************************************************************//**
\brief For master : writes a number of bytes to the spi.\n
  Callback function will be used to notify when the activity is finished.\n
  The read data is placed to the buffer. \n
  For slave: reads a number of bytes from internal spi buffer and writes them \n
  to application buffer.
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
int HAL_ReadSpi(HAL_SpiDescriptor_t *descriptor, uint8_t *buffer, uint16_t length)
{
  halSyncSpiReadData(descriptor , buffer, length);
  return 0;
}

#endif // defined(HAL_USE_SPI)

// eof spi.c

