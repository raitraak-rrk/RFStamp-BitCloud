/****************************************************************************//**
  \file vcpCdcProtocol.h

  \brief Implementation of communication device protocol command.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    11/09/08 A. Khromykh - Created
*******************************************************************************/
/******************************************************************************
                   Includes section
******************************************************************************/
#include <vcpCdcProtocol.h>
#include <usbEnumeration.h>
#include <usart.h>
#include <usb.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
// virtual uart speed
#define VU_1200   0x4B0
#define VU_2400   0x960
#define VU_4800   0x12C0
#define VU_9600   0x2580
#define VU_19200  0x4B00
#define VU_38400  0x9600
#define VU_57600  0xE100
#define VU_115200 0x1C200

// char size
#define VU_1STOPBIT    0
#define VU_1d5STOPBITS 1
#define VU_2STOPBITS   2

// parity
#define VU_NONE  0
#define VU_ODD   1
#define VU_EVEN  2
#define VU_MARK  3
#define VU_SPACE 4

// data bits number
#define VU_5DATABITS  5
#define VU_6DATABITS  6
#define VU_7DATABITS  7
#define VU_8DATABITS  8
#define VU_16DATABITS 16

/******************************************************************************
                   External global variables section
******************************************************************************/
extern HAL_UsartDescriptor_t *vcpPointDescrip;

/******************************************************************************
                   Global variables section
******************************************************************************/
UsbCdcRequest_t  request;
UsbCdcResponse_t response;

/******************************************************************************
                   Implementations section
******************************************************************************/
/******************************************************************************
Get baud rate meaning for cdc response.

Parameters:
  baudRate - virtual uart baudRate
******************************************************************************/
void vcpGetBaudRate(uint32_t baudRate)
{
  (void)baudRate;
  response.getLineCoding.dwDTERate = VU_115200;
}

/******************************************************************************
Get number of stop bits meaning for cdc response.

Parameters:
  stopBits - virtual uart stop bits
******************************************************************************/
void vcpGetStopBits(uint8_t stopBits)
{
  (void)stopBits;
  response.getLineCoding.bCharFormat = VU_1STOPBIT;
}

/******************************************************************************
Get parity meaning for cdc response.

Parameters:
  parity - virtual uart parity
******************************************************************************/
void vcpGetParity(uint8_t parity)
{
  (void)parity;
  response.getLineCoding.bParityType = VU_NONE;
}

/******************************************************************************
Get data length meaning for cdc response.

Parameters:
  dataLength - virtual uart data length
******************************************************************************/
void vcpGetDataLength(uint8_t dataLength)
{
  (void)dataLength;
  response.getLineCoding.bDataBits = VU_8DATABITS;
}

/******************************************************************************
Get virtual uart data and send answer to host.
******************************************************************************/
void vcpResponseGetLineCoding(void)
{
  vcpGetBaudRate(vcpPointDescrip->baudrate);
  vcpGetStopBits(vcpPointDescrip->stopbits);
  vcpGetParity(vcpPointDescrip->parity);
  vcpGetDataLength(vcpPointDescrip->dataLength);

  HAL_UsbWrite(0, (void *)&response, sizeof(GetLineCodingResponse_t), 0, 0);
}

/******************************************************************************
Set baud rate meaning to virtual port.

Parameters:
  baudRate - virtual uart baud rate
******************************************************************************/
void vcpSetBaudRate(uint32_t baudRate)
{
  (void)baudRate;
}

/******************************************************************************
Set number stop bits to virtual port.

Parameters:
  stopBits - virtual uart stop bits
******************************************************************************/
void vcpSetStopBits(uint8_t stopBits)
{
  (void)stopBits;
}

/******************************************************************************
Set parity meaning to virtual port.

Parameters:
  parity - virtual uart parity
******************************************************************************/
void vcpSetParity(uint8_t parity)
{
  (void)parity;
}

/******************************************************************************
Set data length to virtual port.

Parameters:
  dataLength - virtual uart data length
******************************************************************************/
void vcpSetDataLength(uint8_t dataLength)
{
  (void)dataLength;
}

/******************************************************************************
Set virtual uart data and send response to host.
******************************************************************************/
void vcpResponseSetLineCoding(void)
{
  vcpSetBaudRate(response.getLineCoding.dwDTERate);
  vcpSetStopBits(response.getLineCoding.bCharFormat);
  vcpSetParity(response.getLineCoding.bParityType);
  vcpSetDataLength(response.getLineCoding.bDataBits);

#if defined(AT91SAM7X256) || defined(AT91SAM3S4C)
  sendZLP();
#endif
}

/******************************************************************************
communication device request handler

Parameters:
  data - pointer to host's request
******************************************************************************/
void vcpRequestHandler(uint8_t *data)
{
  UsbCdcRequest_t *pRequest = NULL;

  pRequest = (UsbCdcRequest_t *)data;
  if (NULL == pRequest)
    return;

  // Check request code
  switch (pRequest->request.bRequest)
  {
    case SET_LINE_CODING:
        HAL_UsbRead(0, (void *)&response, sizeof(GetLineCodingResponse_t), (TransferCallback_t)vcpResponseSetLineCoding, 0);
      break;
    case GET_LINE_CODING:
        vcpResponseGetLineCoding();
      break;
    case SET_CONTROL_LINE_STATE:
        //vcpReadDataFromSetControlLineState(pRequest->wValue); // possible in the future
        #if defined(AT91SAM7X256) || defined(AT91SAM3S4C)
          sendZLP();
        #endif
      break;
    default:
        HAL_Stall(0);
      break;
  }
}

//eof vcpCdcProtocol.c
