/****************************************************************************//**
  \file usbEnumeration.c

  \brief Implementation of enumeration proccess.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    11/09/08 A. Khromykh - Created
    26/08/11 N. Fomin - Modified (MSD support)
*******************************************************************************/
/******************************************************************************
                   Includes section
******************************************************************************/
#include <usbDescriptors.h>
#if (APP_INTERFACE == APP_INTERFACE_VCP)
  #include <vcpVirtualUsart.h>
#endif // (APP_INTERFACE == APP_INTERFACE_VCP)
#if (MSD_SUPPORT == 1)
  #include <hsmci.h>
#endif // (MSD_SUPPORT == 1)
#include <usbSetupProcess.h>
#include <massStorageDevice.h>
#include <usbEnumeration.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
// request codes for runtime work
#define USB_GET_STATUS             0
#define USB_CLEAR_FEATURE          1
#define USB_SET_FEATURE            3
#define USB_SET_ADDRESS            5
#define USB_GET_DESCRIPTOR         6
#define USB_SET_DESCRIPTOR         7
#define USB_GET_CONFIGURATION      8
#define USB_SET_CONFIGURATION      9
#define USB_GET_INTERFACE          10
#define USB_SET_INTERFACE          11
#define USB_SYNCH_FRAME            12

// Descriptor type definitions.
#define DESCRIPTOR_TYPE_DEVICE           0x01
#define DESCRIPTOR_TYPE_CONFIGURATION    0x02
#define DESCRIPTOR_TYPE_STRING           0x03
#define DESCRIPTOR_TYPE_INTERFACE        0x04
#define DESCRIPTOR_TYPE_CS_INTERFACE     0x24
#define DESCRIPTOR_TYPE_ENDPOINT         0x05
#define DESCRIPTOR_TYPE_DEVICE_QUALIFIER 0x06
#define DESCRIPTOR_TYPE_OTHER_SPEED_CFG  0x07
#define DESCRIPTOR_TYPE_INTERFACE_POWER  0x08

// String definitions' indexes.
#define MANUFACTURER_STRING_INDEX     0
#define PRODUCT_STRING_INDEX          0
#define LANG_ID_STRING_INDEX          0
#define SERIAL_NUMBER_STRING_INDEX    1

/******************************************************************************
                   External global variables section
******************************************************************************/
extern const ConfigurationFrameResponse_t usbConfigDescr;
extern const StringDescriptor_t langStringDescr;
extern const DeviceDescriptor_t deviceDescr;
#if (APP_INTERFACE == APP_INTERFACE_VCP)
  extern HAL_UsartDescriptor_t *vcpPointDescrip;
#endif // (APP_INTERFACE == APP_INTERFACE_VCP)
#if (MSD_SUPPORT == 1)
  extern HAL_HsmciDescriptor_t *msdPointDescr;
#endif // (MSD_SUPPORT == 1)

/******************************************************************************
                   Prototypes section
******************************************************************************/
/******************************************************************************
usb bulk out receiving callbacks

Parameters:
  pArg   - argument
  status  - index of the requested descriptor
  length - maximum number of bytes to return
******************************************************************************/
void vcpRcvCallback(void *pArg, uint8_t status, uint16_t transferred, uint16_t remaining);
void msdRcvCallback(void *pArg, uint8_t status, uint16_t transferred, uint16_t remaining);

/******************************************************************************
                   Global variables section
******************************************************************************/
// virtual communication port address on usb
static uint16_t usbAddress = 0;
static StringDescriptor_t serialNumberStringDescr = {
  sizeof(StringDescriptor_t),
  DESCRIPTOR_TYPE_STRING,
  {0}
};

/******************************************************************************
                   Implementations section
******************************************************************************/
#if defined(AT91SAM7X256) || defined(AT91SAM3S4C) || defined(AT91SAM4S16C)
/******************************************************************************
send zero-length packet through control pipe
******************************************************************************/
void sendZLP(void)
{
  // Acknowledge the request
  HAL_UsbWrite(0, NULL, 0, NULL, NULL);
}
#endif

/******************************************************************************
Configures the device by setting it into the Configured state.

Parameters:
  cfgnum - configuration number to set
******************************************************************************/
void runtimeSetConfiguration(uint8_t cfgnum)
{
  // Set & save the desired configuration
  HAL_SetConfiguration(cfgnum);

  #if defined(AT91SAM7X256) || defined(AT91SAM3S4C) || defined(AT91SAM4S16C)
    // Acknowledge the request
    sendZLP();
  #endif
}

/******************************************************************************
runtime get descriptor command handler

Parameters:
  type   - type of the requested descriptor
  index  - index of the requested descriptor
  length - maximum number of bytes to return
******************************************************************************/
void runtimeGetDescriptorHandler(uint8_t type, uint8_t index, uint16_t length)
{
  (void)index;

  // Check the descriptor type
  switch (type)
  {
    case DESCRIPTOR_TYPE_DEVICE:
        // Adjust length and send descriptor
        if (length > deviceDescr.bLength)
          length = deviceDescr.bLength;
        HAL_UsbWrite(0, (void *)&deviceDescr, length, 0, 0);
      break;
    case DESCRIPTOR_TYPE_CONFIGURATION:
        // Adjust length and send descriptor
        if (length > usbConfigDescr.config.wTotalLength)
          length = usbConfigDescr.config.wTotalLength;
        HAL_UsbWrite(0, (void *)&usbConfigDescr, length, 0, 0);
      break;
    case DESCRIPTOR_TYPE_INTERFACE:
        HAL_Stall(0);
      break;
    case DESCRIPTOR_TYPE_DEVICE_QUALIFIER:
        HAL_Stall(0);
      break;
    case DESCRIPTOR_TYPE_OTHER_SPEED_CFG:
        HAL_Stall(0);
      break;
    case DESCRIPTOR_TYPE_ENDPOINT:
        HAL_Stall(0);
      break;
    case DESCRIPTOR_TYPE_STRING:
        if (SERIAL_NUMBER_STRING_INDEX == index)
        {
          if (length > serialNumberStringDescr.bLength)
            length = serialNumberStringDescr.bLength;
          HAL_UsbWrite(0, (void *)&serialNumberStringDescr, length, 0, 0);
        }
        //When string descriptor(s) presents, host also asks for string descriptor that describes used language.
        else if (LANG_ID_STRING_INDEX == index)
        {
          if (length > serialNumberStringDescr.bLength)
            length = langStringDescr.bLength;
          HAL_UsbWrite(0, (void *)&langStringDescr, length, 0, 0);
        }
        else
          HAL_Stall(0);
      break;
    default:
        HAL_Stall(0);
      break;
  }
}

/******************************************************************************
Standard usb request handler

Parameters:
  data - pointer to host's request
******************************************************************************/
void runtimeRequestHandler(uint8_t *data)
{
  UsbRequest_t *pRequest = NULL;

  pRequest = (UsbRequest_t *)data;
  if (NULL == pRequest)
    return;

  // Check request code
  switch (pRequest->bRequest)
  {
    case USB_GET_DESCRIPTOR:
        // Send the requested descriptor
        runtimeGetDescriptorHandler((uint8_t)(pRequest->wValue >> 8), (uint8_t)(pRequest->wValue), pRequest->wLength);
      break;
    case USB_SET_ADDRESS:
        usbAddress = pRequest->wValue & 0x7F;
        #if defined(AT91SAM7X256) || defined(AT91SAM3S4C) || defined(AT91SAM4S16C)
          HAL_UsbWrite(0, 0, 0, (TransferCallback_t) HAL_SetAddress, (void *)&usbAddress);
        #elif defined(AT90USB1287)
          HAL_SetAddress((uint8_t *)&usbAddress);
        #endif
      break;
    case USB_SET_CONFIGURATION:
        // Set the requested configuration
        runtimeSetConfiguration((uint8_t)pRequest->wValue);
        #if (APP_INTERFACE == APP_INTERFACE_VCP)
          HAL_ConfigureEndpoint((void *)&(usbConfigDescr.endpointIfc1));
          HAL_ConfigureEndpoint((void *)&(usbConfigDescr.endpointIfc2[0]));
          HAL_ConfigureEndpoint((void *)&(usbConfigDescr.endpointIfc2[1]));
          HAL_UsbRead(VCP_RECEIVE_PIPE, vcpPointDescrip->rxBuffer, BULK_SIZE, vcpRcvCallback, NULL);
        #endif // (APP_INTERFACE == APP_INTERFACE_VCP)
        #if (MSD_SUPPORT == 1)
          HAL_ConfigureEndpoint((void *)&(usbConfigDescr.endpointIfc3[0]));
          HAL_ConfigureEndpoint((void *)&(usbConfigDescr.endpointIfc3[1]));
          HAL_UsbRead(MSD_RECEIVE_PIPE, msdPointDescr->dataTransferDescriptor->buffer, BULK_SIZE, msdRcvCallback, NULL);
        #endif // (MSD_SUPPORT == 1)
      break;
    default:
      setupProcessRequestHandler(data);
      break;
  }
}

/******************************************************************************
Standard usb request handler

Parameters:
  data - pointer to host's request
******************************************************************************/
void usbBusResetAction(void)
{
  HAL_RegisterRequestHandler(runtimeRequestHandler);
}

/**************************************************************************//**
\brief Sets serial number for serial string descriptor.
******************************************************************************/
void usbSetSerialNumber(uint32_t sn)
{
  for(uint8_t i = 0; i < MAX_STRING_DESCRIPTOR_SIZE; i += 2)
  {
    serialNumberStringDescr.bString[i] = sn % 10 + '0';
    sn /= 10;
  }
}

// eof usbEnumeration.c
