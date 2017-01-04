/************************************************************************//**
  \file  usb.h

  \brief The header file describes the usb interface

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    19.02.15 Parthasarathy G - Created.
******************************************************************************/
//#if APP_INTERFACE == APP_INTERFACE_USBFIFO

#ifndef _USB_H
#define _USB_H

/******************************************************************************
                   Includes section
******************************************************************************/
// \cond
#include <halUsb.h>
#include <halClkCtrl.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/

/******************************************************************************
                   Types section
******************************************************************************/

typedef enum
{
  RX_COMPLETE,
  RX_BUFFER_FULL,
  RX_DATA_AVAILABLE
}HAL_UsbRxCallBackStatus_t;

typedef enum
{
  USB_BINARY_MODE,
  USB_TEXT_MODE 
}HAL_UsbCdcOperatingMode_t;

typedef struct
{
  uint16_t length;
#if defined (USB_TEXT_MODE_SUPPORT)
  HAL_UsbRxCallBackStatus_t status;
#endif
}HAL_UsbRxIndCallback_t;

/** \brief USB descriptor*/
typedef struct
{
   /** \brief It's pointer to receive buffer. \n
   If rxBuffer is NULL then transactions are discarded. \n
   Size of buffer depends on user application. */
  uint8_t *rxBuffer;
  /** \brief length(size) of receive buffer */
  uint16_t rxBufferLength;
  /** \brief It's pointer to transmit buffer. \n
   If txBuffer is NULL then transactions are discarded. \n
   Size of buffer depends on user application. */
  uint8_t *txBuffer;
  /** \brief length(size) of transmit buffer */
  uint16_t txBufferLength;
  /** \brief It's receive usb callback. \n
   If rxCallback is NULL then polling method shall used. \n
   If rxCallback isn't NULL then callback method is used.*/
  void (*rxCallback)(HAL_UsbRxIndCallback_t *);

  /** \brief It's transmitting was completed usart callback. \n
   If txBuffer isn't NULL then txCallback notify about end of bytes sending.  */
  void (*txCallback)(void);
  /** \brief Operation speed for USB */
  UsbSpeed_t speed;
  /** 
    Set this variable to true ,
    If VBUS pin is used as external interrupt for hot plug detection
  */
  bool usbVbusDetect;
   
  HAL_UsbCdcOperatingMode_t oprMode;
}HAL_UsbHWDescriptor_t;

/******************************************************************************
                   Prototypes section
******************************************************************************/

/**************************************************************************//**
\brief Performs configuration of USB registers and CDC class, HW pins \n
       Initializes the USB and CDC class. Device will get enumerated \n
        automatically once the V-BUS voltage is detected.
        
\ingroup hal_usb

\param[in]
  descriptor - pointer to HAL_UsbCdcDescriptor_t structure

\return
  Returns positive value on success or -1 in cases: \n
    - bad usb descriptor or; \n
    - there are not enough resources; \n  
******************************************************************************/
int HAL_OpenUsb(HAL_UsbHWDescriptor_t *descriptor);


/**************************************************************************//**
\brief Sends data to the USB-CDC Interface

\ingroup hal_usb

\param[in] data   - pointer to data to be sent;
\param[in] length - number of bytes (length) of data to be sent

\return
  -1 -on failure to transmit; \n
   Number of bytes placed to the buffer - success.
******************************************************************************/
int HAL_WriteUsb(uint8_t *data, uint8_t length);

/**************************************************************************//**
\brief Reads data from the USB-CDC Interface

\ingroup hal_usb

\params[out] data - pointer to receive buffer;
\params[in]  length - number of bytes (length) to be read;

\return
  -1 - on failure or bad number of bytes to read; \n
  Number of bytes placed to the buffer - success.
******************************************************************************/
int HAL_ReadUsb(uint8_t *data, uint8_t length);

/*************************************************************************//**
\brief Releases the USB module and pins

\ingroup hal_usb

\param[in]
  descriptor - pointer to HAL_UsbCdcDescriptor_t structure

\return
 -1 - bad descriptor or is already closed; \n
  0 - success.
*****************************************************************************/
int HAL_CloseUsb(void);

#if defined(ATSAMR21G18A) || defined(ATSAMR21E18A)
/**************************************************************************//**
\brief Configure the USB Vbus detect pin as external interrupt
******************************************************************************/
void BSP_BoardSpecificUsbVbusInit(void);

/**************************************************************************//**
\brief Reads the status of the VBUS pin
\returns True if pin is logic high otherwise false
******************************************************************************/
uint8_t BSP_isUsbVbusHigh (void);
#endif

#endif
