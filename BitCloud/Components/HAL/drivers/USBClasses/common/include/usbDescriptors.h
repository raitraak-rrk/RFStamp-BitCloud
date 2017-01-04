/****************************************************************************//**
  \file usbDescriptors.h

  \brief Declaration of descriptors structures.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    05/09/08 A. Khromykh - Created
    26/08/11 N. Fomin - Modified (MSD support)
*******************************************************************************/
#ifndef _USBDESCRIPTORS_H
#define _USBDESCRIPTORS_H

/******************************************************************************
                   Includes section
******************************************************************************/
#include <sysTypes.h>
#include <usb.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
#define VCP_INTERFACE_ENDPOINTS_NUMBER     2
#define MSD_INTERFACE_ENDPOINTS_NUMBER     2

#define NUMBER_OF_FIRST_INTERFACE          0x00
#define NUMBER_OF_SECOND_INTERFACE         0x01
#define NUMBER_OF_THIRD_INTERFACE          0x02
#define NUMBER_OF_FOURTH_INTERFACE         0x03

#define BULK_SIZE   0x0040

#define ADDRESS_MSC_BULKIN_PIPE      0x85
#define ADDRESS_MSC_BULKOUT_PIPE     0x06

#define MAX_STRING_DESCRIPTOR_SIZE   0x14

/******************************************************************************
                   Types section
******************************************************************************/
// USB standard device descriptor structure.
BEGIN_PACK
typedef struct PACK
{
  uint8_t  bLength;            // Size of this descriptor in bytes
  uint8_t  bDescriptorType;    // Descriptor type
  uint16_t bcdUSB;             // USB specification release number in BCD format
  uint8_t  bDeviceClass;       // Device class code
  uint8_t  bDeviceSubClass;    // Device subclass code
  uint8_t  bDeviceProtocol;    // Device protocol code
  uint8_t  bMaxPacketSize0;    // Maximum packet size of endpoint 0 (in bytes)
  uint16_t idVendor;           // Vendor ID
  uint16_t idProduct;          // Product ID
  uint16_t bcdDevice;          // Device release number in BCD format
  uint8_t  iManufacturer;      // Index of the manufacturer string descriptor
  uint8_t  iProduct;           // Index of the product string descriptor
  uint8_t  iSerialNumber;      // Index of the serial number string descriptor
  uint8_t  bNumConfigurations; // Number of possible configurations for the device
} DeviceDescriptor_t;

// USB string descriptor structure.
typedef struct PACK
{
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bString[MAX_STRING_DESCRIPTOR_SIZE];
} StringDescriptor_t;

// USB standard configuration descriptor structure.
typedef struct PACK
{
  uint8_t  bLength;             // Size of the descriptor in bytes
  uint8_t  bDescriptorType;     // Descriptor type
  uint16_t wTotalLength;        // Length of all descriptors returned along with this configuration descriptor
  uint8_t  bNumInterfaces;      // Number of interfaces in this configuration
  uint8_t  bConfigurationValue; // Value for selecting this configuration
  uint8_t  iConfiguration;      // Index of the configuration string descriptor
  uint8_t  bmAttributes;        // Configuration characteristics
  uint8_t  bMaxPower;           // Maximum power consumption of the device when in this configuration
} ConfigurationDescriptor_t;

// USB standard interface descriptor structure.
typedef struct PACK
{
  uint8_t bLength;            // Size of the descriptor in bytes
  uint8_t bDescriptorType;    // Descriptor type
  uint8_t bInterfaceNumber;   // Number of the interface in its configuration
  uint8_t bAlternateSetting;  // Value to select this alternate interface setting
  uint8_t bNumEndpoints;      // Number of endpoints used by the interface (excluding endpoint 0)
  uint8_t bInterfaceClass;    // Interface class code
  uint8_t bInterfaceSubClass; // Interface subclass code
  uint8_t bInterfaceProtocol; // Interface protocol code
  uint8_t iInterface;         // Index of the interface string descriptor
} InterfaceDescriptor_t;

// USB header functional descriptor.
typedef struct PACK
{
  uint8_t  bFunctionalLength;  // Size of the descriptor in bytes
  uint8_t  bDescriptorType;    // Descriptor type
  uint8_t  bDescriptorSubtype; // Header functional descriptor subtype
  uint16_t bcdCDC;             // USB Class Definitions for Communication
                               // Devices Specification release number in binary-coded decimal.
} HeaderFunctionalDescriptor_t;

// USB Abstract Control Management Functional Descriptor.
typedef struct PACK
{
  uint8_t bFunctionalLength;  // Size of the descriptor in bytes
  uint8_t bDescriptorType;    // Descriptor type
  uint8_t bDescriptorSubtype; // ACM descriptor subtype
  uint8_t bmCapabilities;     // The capabilities that this configuration supports
} AbstractControlManagmentDescriptor_t;

// USB Union Functional Descriptor.
typedef struct PACK
{
  uint8_t bFunctionalLength;  // Size of the descriptor in bytes
  uint8_t bDescriptorType;    // Descriptor type
  uint8_t bDescriptorSubtype; // Union functional descriptor subtype
  uint8_t bMasterInterface;   // The interface number of the Communication or Data Class interface, designated as the master or controlling interface for the union
  uint8_t bSlaveInterface0;   // Interface number of first slave or associated interface in the union
} UnionFunctionalDescriptor_t;

// USB Call Management Functional Descriptor.
typedef struct PACK
{
  uint8_t bFunctionalLength;  // Size of the descriptor in bytes
  uint8_t bDescriptorType;    // Descriptor type
  uint8_t bDescriptorSubtype; // Union functional descriptor subtype
  uint8_t bmCapabilities;     // The capabilities that this configuration supports
  uint8_t bDataInterface;     // Interface number of Data Class interface optionally used for call management
} CallManagmentFunctionalDescriptor_t;

// USB Interface Association Descriptor.
typedef struct PACK
{
  uint8_t bLength;            // Size of the descriptor in bytes
  uint8_t bDescriptorType;    // Descriptor type
  uint8_t bFirstInterface;    // Interface number of the first interface that is associated with this function
  uint8_t bInterfaceCount;    // Number of contiguous interfaces that are associated with this function
  uint8_t bFunctionClass;     // Class code
  uint8_t bFunctionSubClass;  // Subclass code
  uint8_t bFunctionProtocol;  // Protocol code
  uint8_t iFunction;          // Index of string descriptor describing this function
} InterfaceAssociationDescriptor_t;

typedef struct PACK
{
  uint8_t  bLength;           // Size of the descriptor in bytes
  uint8_t  bDescriptorType;   // Descriptor type
  uint8_t  bmAttributes;      // Descriptor attributes
  uint16_t wDetachTimeOut;    // Wait timeout of USB reset after DFU_DETACH request recieption
  uint16_t wTransferSize;     // Maximum number of bytes that the device can accept per control-write transaction
  uint16_t bcdDFUVersion;     // DFU Version
} DfuFunctionalDescriptor_t;

// USB frame for interface request.
typedef struct PACK
{
  ConfigurationDescriptor_t             config;
#if (APP_INTERFACE == APP_INTERFACE_VCP)
#if (MSD_SUPPORT == 1) || (DFU_SUPPORT == 1)
  InterfaceAssociationDescriptor_t      cdcIAD;
#endif // (MSD_SUPPORT == 1) || (DFU_SUPPORT == 1)
  InterfaceDescriptor_t                 interface1;
  HeaderFunctionalDescriptor_t          headerFunctional;
  CallManagmentFunctionalDescriptor_t   cmFunctionak;
  AbstractControlManagmentDescriptor_t  acmFunctional;
  UnionFunctionalDescriptor_t           unionFunctional;
  HAL_UsbEndPointDescptr_t              endpointIfc1;
  InterfaceDescriptor_t                 interface2;
  HAL_UsbEndPointDescptr_t              endpointIfc2[VCP_INTERFACE_ENDPOINTS_NUMBER];
#endif // (APP_INTERFACE == APP_INTERFACE_VCP)
#if (MSD_SUPPORT == 1)
  InterfaceDescriptor_t                 interface3;
  HAL_UsbEndPointDescptr_t              endpointIfc3[MSD_INTERFACE_ENDPOINTS_NUMBER];
#endif // (MSD_SUPPORT == 1)
#if (DFU_SUPPORT == 1)
  InterfaceDescriptor_t                 interface4;
  DfuFunctionalDescriptor_t             dfuFuncional;
#endif // (DFU_SUPPORT == 1)
} ConfigurationFrameResponse_t;
END_PACK

#endif /* _USBDESCRIPTORS_H */
// eof usbDescriptors.h