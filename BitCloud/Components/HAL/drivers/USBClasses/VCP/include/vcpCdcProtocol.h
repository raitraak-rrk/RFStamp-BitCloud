/****************************************************************************//**
  \file vcpCdcProtocol.h

  \brief Declaration of communication device protocol command.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    05/09/08 A. Khromykh - Created
*******************************************************************************/
#ifndef _VCPCDCPROTOCOL_H
#define _VCPCDCPROTOCOL_H

/******************************************************************************
                   Includes section
******************************************************************************/
#include <sysTypes.h>
#include <usbSetupProcess.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
// data size in request structure
#define CDC_REQUEST_DATA_SIZE       7
// data size in notification structure
#define NOTIFICATION_DATA_SIZE      2

// request codes for communication interface class
#define SEND_ENCAPSULATED_COMMAND                   0x00
#define GET_ENCAPSULATED_RESPONSE                   0x01
#define SET_COMM_FEATURE                            0x02
#define GET_COMM_FEATURE                            0x03
#define CLEAR_COMM_FEATURE                          0x04
#define SET_AUX_LINE_STATE                          0x10
#define SET_HOOK_STATE                              0x11
#define PULSE_SETUP                                 0x12
#define SEND_PULSE                                  0x13
#define SET_PULSE_TIME                              0x14
#define RING_AUX_JACK                               0x15
#define SET_LINE_CODING                             0x20
#define GET_LINE_CODING                             0x21
#define SET_CONTROL_LINE_STATE                      0x22
#define SEND_BREAK                                  0x23
#define SET_RINGER_PARMS                            0x30
#define GET_RINGER_PARMS                            0x31
#define SET_OPERATION_PARMS                         0x32
#define GET_OPERATION_PARMS                         0x33
#define SET_LINE_PARMS                              0x34
#define GET_LINE_PARMS                              0x35
#define DIAL_DIGITS                                 0x36
#define SET_UNIT_PARAMETER                          0x37
#define GET_UNIT_PARAMETER                          0x38
#define CLEAR_UNIT_PARAMETER                        0x39
#define GET_PROFILE                                 0x3A
#define SET_ETHERNET_MULTICAST_FILTERS              0x40
#define SET_ETHERNET_POWER_MANAGEMENT_PATTERNFILTER 0x41
#define GET_ETHERNET_POWER_MANAGEMENT_PATTERNFILTER 0x42
#define SET_ETHERNET_PACKET_FILTER                  0x43
#define GET_ETHERNET_STATISTIC                      0x44
#define SET_ATM_DATA_FORMAT                         0x50
#define GET_ATM_DEVICE_STATISTICS                   0x51
#define SET_ATM_DEFAULT_VC                          0x52
#define GET_ATM_VC_STATISTICS                       0x53

/******************************************************************************
                   Types section
******************************************************************************/
BEGIN_PACK
// Usb host request
typedef struct PACK
{
  UsbRequest_t request;
  uint8_t  bData[CDC_REQUEST_DATA_SIZE];
} UsbCdcRequest_t;

// Usb GetLineCoding device response
typedef struct PACK
{
  uint32_t dwDTERate;
  uint8_t  bCharFormat;
  uint8_t  bParityType;
  uint8_t  bDataBits;
} GetLineCodingResponse_t;

typedef union PACK
{
  GetLineCodingResponse_t getLineCoding;
} UsbCdcResponse_t;

// Usb VCP notification
typedef struct PACK
{
  uint8_t  bmRequestType;
  uint8_t  bNotification;
  uint16_t wValue;
  uint16_t wIndex;
  uint16_t wLength;
  uint8_t  bData[NOTIFICATION_DATA_SIZE];
} UsbNotification_t;
END_PACK

/******************************************************************************
                   Prototypes section
******************************************************************************/
/******************************************************************************
communication device request handler

Parameters:
  data - pointer to host's request
******************************************************************************/
void vcpRequestHandler(uint8_t *data);

#endif /* _VCPCDCPROTOCOL_H */
