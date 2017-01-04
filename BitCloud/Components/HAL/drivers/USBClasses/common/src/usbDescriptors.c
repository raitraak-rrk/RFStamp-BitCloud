/****************************************************************************//**
  \file usbDescriptors.c

  \brief Virtual communication port descriptors.

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

/******************************************************************************
                   Define(s) section
******************************************************************************/
// Descriptor's type definitions.
#define DESCRIPTOR_TYPE_DEVICE                0x01
#define DESCRIPTOR_TYPE_CONFIGURATION         0x02
#define DESCRIPTOR_TYPE_STRING                0x03
#define DESCRIPTOR_TYPE_INTERFACE             0x04
#define DESCRIPTOR_TYPE_CS_INTERFACE          0x24
#define DESCRIPTOR_TYPE_ENDPOINT              0x05
#define DESCRIPTOR_TYPE_DEVICE_QUALIFIER      0x06
#define DESCRIPTOR_TYPE_OTHER_SPEED_CFG       0x07
#define DESCRIPTOR_TYPE_INTERFACE_POWER       0x08
#define DESCRIPTOR_TYPE_INTERFACE_ASSOCIATION 0x0B
#define DESCRIPTOR_TYPE_DFU_FUNCTIONAL        0x21

// String definitions' indexes.
#define MANUFACTURER_STRING_INDEX     0
#define PRODUCT_STRING_INDEX          0
#if MSD_SUPPORT == 1
  #define SERIAL_NUMBER_STRING_INDEX  1
#else
  #define SERIAL_NUMBER_STRING_INDEX  0
#endif
#define CONFIGURATION_STRING_INDEX    0
#define FIRST_INTERFACE_STRING_INDEX  0
#define SECOND_INTERFACE_STRING_INDEX 0
#define THIRD_INTERFACE_STRING_INDEX  0
#define FOURTH_INTERFACE_STRING_INDEX 0

#define VENDOR_ID       0x03EB    // Atmel
#if (APP_INTERFACE == APP_INTERFACE_VCP)
  #if (MSD_SUPPORT == 1)
    #if (DFU_SUPPORT == 1)
      #define PRODUCT_ID      0x6122
    #else
      #define PRODUCT_ID      0x6121
    #endif // (DFU_SUPPORT == 1)
  #else // (MSD_SUPPORT == 1)
    #if (DFU_SUPPORT == 1)
      #define PRODUCT_ID      0x6120
    #else
      #define PRODUCT_ID      0x6119
    #endif // (DFU_SUPPORT == 1)
  #endif // (MSD_SUPPORT == 1)
#else // (APP_INTERFACE == APP_INTERFACE_VCP)
  #if (MSD_SUPPORT == 1)
    #if (DFU_SUPPORT == 1)
      #define PRODUCT_ID      0x6124
    #else
      #define PRODUCT_ID      0x6123
    #endif // (DFU_SUPPORT == 1)
  #else // (MSD_SUPPORT == 1)
    #define PRODUCT_ID        0x6119
  #endif // (MSD_SUPPORT == 1)
#endif // (APP_INTERFACE == APP_INTERFACE_VCP)

#define DEVICE_RELEASE  0x0001
#define USB_BUS_RELEASE 0x0200

// Number of possible configurations for the device.
#define NUMBER_OF_CONFIGURATIONS     0x01

// Class specification parameters of communication device.
#define CDC_DEVICE_CLASS       0x02
#define CDC_DEVICE_SUBCLASS    0x00
#define CDC_DEVICE_PROTOCOL    0x00

// Class specification parameters of mass storage device.
#define MSC_DEVICE_CLASS       0x00
#define MSC_DEVICE_SUBCLASS    0x00
#define MSC_DEVICE_PROTOCOL    0x00

// Class specification parameters of composite device with CDC.
#define COMB_DEVICE_CLASS       0xef
#define COMB_DEVICE_SUBCLASS    0x02
#define COMB_DEVICE_PROTOCOL    0x01

// Endpoint definitions' sizes.
#if defined(AT91SAM7X256)
  #define SZ_CONTROL_ENDPOINT      0x08      // endpoint 0 is control pipe
#elif defined(AT90USB1287) || defined(AT91SAM3S4C) || defined(AT91SAM4S16C)
  #define SZ_CONTROL_ENDPOINT      0x40      // endpoint 0 is control pipe
#endif
#define SZ_ACM_INT_ENDPOINT      0x0008    // endpoint 3 is interrupt pipe for abstraction control model
#define SZ_CDC_BULKIN_ENDPOINT   BULK_SIZE // endpoint 2 is bulk pipe for input communication data
#define SZ_CDC_BULKOUT_ENDPOINT  BULK_SIZE // endpoint 1 is bulk pipe for output communication data
#define SZ_MSC_BULKIN_ENDPOINT   BULK_SIZE // endpoint 2 is bulk pipe for input communication data
#define SZ_MSC_BULKOUT_ENDPOINT  BULK_SIZE // endpoint 1 is bulk pipe for output communication data

// Configuration descriptor parameters.
#if (APP_INTERFACE == APP_INTERFACE_VCP)
  #if (MSD_SUPPORT == 1)
    #if (DFU_SUPPORT == 1)
      #define NUMBER_OF_INTERFACES      0x04
    #else
      #define NUMBER_OF_INTERFACES      0x03
    #endif
  #else
    #define NUMBER_OF_INTERFACES      0x02
  #endif
#else
  #if (DFU_SUPPORT == 1)
    #define NUMBER_OF_INTERFACES      0x02
  #else
    #define NUMBER_OF_INTERFACES      0x01
  #endif
#endif

#define CFG_SELECTING_VALUE       0x01
#define CFG_CHARACTERISTICS       0x80  // D7 is reserved and must be set to one for historical reasons.
#define MAXIMUM_POWER_CONSUMPTION 0x32  // Step is 2 mA.

// Parameters for interfaces descriptors
#define ALTERNATIVE_SETTING                0x00
#define NUMBER_USING_ENDPOINTS_FIRST_IFC   0x01
#define NUMBER_USING_ENDPOINTS_SECOND_IFC  0x02
#define NUMBER_USING_ENDPOINTS_THIRD_IFC   0x02
#define NUMBER_USING_ENDPOINTS_FOURTH_IFC  0x00
#define FIRST_IFC_CLASS        0x02
#define FIRST_IFC_SUBCLASS     0x02
#define FIRST_IFC_PROTOCOL     0x00
#define SECOND_IFC_CLASS       0x0A
#define SECOND_IFC_SUBCLASS    0x00
#define SECOND_IFC_PROTOCOL    0x00
#define THIRD_IFC_CLASS        0x08
#define THIRD_IFC_SUBCLASS     0x06
#define THIRD_IFC_PROTOCOL     0x50
#define FOURTH_IFC_CLASS       0xFE
#define FOURTH_IFC_SUBCLASS    0x01
#define FOURTH_IFC_PROTOCOL    0x01

// Header descriptor parameters.
#define HEADER_SUBTYPE        0x00
#define CDC_CLASS_DEFINITION  0x0110

// call management functional descriptor parameters.
#define CALL_MNGMT_SUBTYPE       0x01
#define CALL_MNGMT_CAPABILITIES  0x01  // See cdc specification.
#define NUMBER_OF_CALL_MNGMT     0x00

// abstract control management functional descriptor parameters.
#define ACM_SUBTYPE              0x02
#define ACM_CAPABILITIES         0x02  // see cdc specification

// Union functional descriptor parameters.
#define UNION_SUBTYPE            0x06
#define MASTER_IFC_TYPE          0x00
#define SLAVE0_IFC_TYPE          0x01

// Endpoints descriptor parameters.
#define ADDRESS_CDC_INTERRUPT_PIPE   0x83
#define ADDRESS_CDC_BULKIN_PIPE      0x82
#define ADDRESS_CDC_BULKOUT_PIPE     0x01
#define INTERRUPT_TYPE_PIPE          0x03
#define BULK_TYPE_PIPE               0x02
#define INTERRUPT_PIPE_POLLING       0x0A  // step is 1 ms
#define BULK_MAX_SPEED               0x00

// dfu functional descriptor parameters
#define DFU_ATTRIBUTES     0x01
#define DFU_DETACH_TIMEOUT 0x0064
#define DFU_TRANSFER_SIZE  SZ_CONTROL_ENDPOINT
#define DFU_VERSION        0x0101

// Language string descriptor parameters
#define LANG_DESCR_LENGTH   4
#define LANG_DESCR_LANGUAGE {0x09, 0x04}

/******************************************************************************
                   Constants section
******************************************************************************/
const DeviceDescriptor_t deviceDescr = {
  sizeof(DeviceDescriptor_t), // Size of this descriptor in bytes
  DESCRIPTOR_TYPE_DEVICE,     // Descriptor type
  USB_BUS_RELEASE,            // USB specification release number in BCD format
#if (APP_INTERFACE == APP_INTERFACE_VCP)
  #if (MSD_SUPPORT != 1) && (DFU_SUPPORT != 1)
  CDC_DEVICE_CLASS,           // Device class code
  CDC_DEVICE_SUBCLASS,        // Device subclass code
  CDC_DEVICE_PROTOCOL,        // Device protocol code
  #else // (MSD_SUPPORT != 1) && (DFU_SUPPORT != 1)
  COMB_DEVICE_CLASS,          // Device class code
  COMB_DEVICE_SUBCLASS,       // Device subclass code
  COMB_DEVICE_PROTOCOL,       // Device protocol code
  #endif // (MSD_SUPPORT != 1) && (DFU_SUPPORT != 1)
#else // (APP_INTERFACE == APP_INTERFACE_VCP)
  MSC_DEVICE_CLASS,           // Device class code
  MSC_DEVICE_SUBCLASS,        // Device subclass code
  MSC_DEVICE_PROTOCOL,        // Device protocol code
#endif // (APP_INTERFACE == APP_INTERFACE_VCP)
  SZ_CONTROL_ENDPOINT,        // Maximum packet size of endpoint 0 (in bytes)
  VENDOR_ID,                  // Vendor ID
  PRODUCT_ID,                 // Product ID
  DEVICE_RELEASE,             // Device release number in BCD format
  MANUFACTURER_STRING_INDEX,  // Index of the manufacturer string descriptor
  PRODUCT_STRING_INDEX,       // Index of the product string descriptor
  SERIAL_NUMBER_STRING_INDEX, // Index of the serial number string descriptor
  NUMBER_OF_CONFIGURATIONS    // Number of possible configurations for the device
};

const StringDescriptor_t langStringDescr = {
  LANG_DESCR_LENGTH,       // Size of the descriptor in bytes
  DESCRIPTOR_TYPE_STRING,  // Descriptor type
  LANG_DESCR_LANGUAGE      // Descriptor string
};

const ConfigurationFrameResponse_t usbConfigDescr = {
  { // configuration
    sizeof(ConfigurationDescriptor_t),    // Size of the descriptor in bytes
    DESCRIPTOR_TYPE_CONFIGURATION,        // Descriptor type
    sizeof(ConfigurationFrameResponse_t), // Length of all descriptors returned along with this configuration descriptor
    NUMBER_OF_INTERFACES,                 // Number of interfaces in this configuration
    CFG_SELECTING_VALUE,                  // Value for selecting this configuration
    CONFIGURATION_STRING_INDEX,           // Index of the configuration string descriptor
    CFG_CHARACTERISTICS,                  // Configuration characteristics
    MAXIMUM_POWER_CONSUMPTION             // Maximum power consumption of the device when in this configuration
  },
#if (APP_INTERFACE == APP_INTERFACE_VCP)
  #if (MSD_SUPPORT == 1) || (DFU_SUPPORT == 1)
  { // cdcIAD
    sizeof(InterfaceAssociationDescriptor_t), // Size of this descriptor in bytes
    DESCRIPTOR_TYPE_INTERFACE_ASSOCIATION,    // Descriptor type
    NUMBER_OF_FIRST_INTERFACE,                // Interface number of the first interface that is associated with this function
    2,                                        // Number of contiguous interfaces that are associated with this function
    CDC_DEVICE_CLASS,                         // Class code
    CDC_DEVICE_SUBCLASS,                      // Subclass code
    CDC_DEVICE_PROTOCOL,                      // Protocol code
    FIRST_INTERFACE_STRING_INDEX              //Index of string descriptor describing this function
  },
  #endif // (MSD_SUPPORT == 1) || (DFU_SUPPORT == 1)
  { // interface 1
    sizeof(InterfaceDescriptor_t),      // Size of the descriptor in bytes
    DESCRIPTOR_TYPE_INTERFACE,          // Descriptor type
    NUMBER_OF_FIRST_INTERFACE,          // Number of the interface in its configuration
    ALTERNATIVE_SETTING,                // Value to select this alternate interface setting
    NUMBER_USING_ENDPOINTS_FIRST_IFC,   // Number of endpoints used by the interface (excluding endpoint 0)
    FIRST_IFC_CLASS,                    // Interface class code
    FIRST_IFC_SUBCLASS,                 // Interface subclass code
    FIRST_IFC_PROTOCOL,                 // Interface protocol code
    FIRST_INTERFACE_STRING_INDEX        // Index of the interface string descriptor
  },
  { // header functional descriptor
    sizeof(HeaderFunctionalDescriptor_t),  // Size of the descriptor in bytes
    DESCRIPTOR_TYPE_CS_INTERFACE,          // Descriptor type
    HEADER_SUBTYPE,                        // Header functional descriptor subtype
    CDC_CLASS_DEFINITION                   // USB Class Definitions for Communication
  },
  { // Call Management Functional Descriptor
    sizeof(CallManagmentFunctionalDescriptor_t), // Size of the descriptor in bytes
    DESCRIPTOR_TYPE_CS_INTERFACE,                // Descriptor type
    CALL_MNGMT_SUBTYPE,                          // bDescriptor subtype: Call Management Func
    CALL_MNGMT_CAPABILITIES,                     // bmCapabilities: D1 + D0
    NUMBER_OF_CALL_MNGMT                         // bDataInterface: Data Class Interface 1
  },
  { // Abstract Control Management Functional Descriptor
    sizeof(AbstractControlManagmentDescriptor_t),  // Size of the descriptor in bytes
    DESCRIPTOR_TYPE_CS_INTERFACE,                  // Descriptor type
    ACM_SUBTYPE,                                   // Abstract Control Management Functional descriptor subtype
    ACM_CAPABILITIES                               // bmCapabilities: see cdc specification (support command type)
  },
  { // Union Functional Descriptor
    sizeof(UnionFunctionalDescriptor_t),  // Size of the descriptor in bytes
    DESCRIPTOR_TYPE_CS_INTERFACE,         // Descriptor type
    UNION_SUBTYPE,                        // Union Functional descriptor subtype
    MASTER_IFC_TYPE,                      // bMasterInterface: CDC Interface
    SLAVE0_IFC_TYPE                       // bSlaveInterface0: Data Class Interface
  },
  { // endpoint 3
    sizeof(HAL_UsbEndPointDescptr_t),   // Size of the descriptor in bytes
    DESCRIPTOR_TYPE_ENDPOINT,           // Descriptor type
    ADDRESS_CDC_INTERRUPT_PIPE,         // Address and direction of the endpoint
    INTERRUPT_TYPE_PIPE,                // Endpoint type and additional characteristics (for isochronous endpoints)
    SZ_ACM_INT_ENDPOINT,                // Maximum packet size (in bytes) of the endpoint
    INTERRUPT_PIPE_POLLING              // Polling rate of the endpoint
  },
  { // interface 2
    sizeof(InterfaceDescriptor_t),      // Size of the descriptor in bytes
    DESCRIPTOR_TYPE_INTERFACE,          // Descriptor type
    NUMBER_OF_SECOND_INTERFACE,         // Number of the interface in its configuration
    ALTERNATIVE_SETTING,                // Value to select this alternate interface setting
    NUMBER_USING_ENDPOINTS_SECOND_IFC,  // Number of endpoints used by the interface (excluding endpoint 0)
    SECOND_IFC_CLASS,                   // Interface class code
    SECOND_IFC_SUBCLASS,                // Interface subclass code
    SECOND_IFC_PROTOCOL,                // Interface protocol code
    SECOND_INTERFACE_STRING_INDEX       // Index of the interface string descriptor
  },
  {{ // endpoint 1
    sizeof(HAL_UsbEndPointDescptr_t),   // Size of the descriptor in bytes
    DESCRIPTOR_TYPE_ENDPOINT,           // Descriptor type
    ADDRESS_CDC_BULKOUT_PIPE,           // Address and direction of the endpoint
    BULK_TYPE_PIPE,                     // Endpoint type and additional characteristics (for isochronous endpoints)
    SZ_CDC_BULKOUT_ENDPOINT,            // Maximum packet size (in bytes) of the endpoint
    BULK_MAX_SPEED                      // Polling rate of the endpoint
  },
  { // endpoint 2
    sizeof(HAL_UsbEndPointDescptr_t),   // Size of the descriptor in bytes
    DESCRIPTOR_TYPE_ENDPOINT,           // Descriptor type
    ADDRESS_CDC_BULKIN_PIPE,            // Address and direction of the endpoint
    BULK_TYPE_PIPE,                     // Endpoint type and additional characteristics (for isochronous endpoints)
    SZ_CDC_BULKIN_ENDPOINT,             // Maximum packet size (in bytes) of the endpoint
    BULK_MAX_SPEED                      // Polling rate of the endpoint
  }},
#endif // (APP_INTERFACE == APP_INTERFACE_VCP)
#if (MSD_SUPPORT == 1)
  { // interface 3
    sizeof(InterfaceDescriptor_t),      // Size of the descriptor in bytes
    DESCRIPTOR_TYPE_INTERFACE,          // Descriptor type
#if (APP_INTERFACE == APP_INTERFACE_VCP)
    NUMBER_OF_THIRD_INTERFACE,          // Number of the interface in its configuration
#else
    NUMBER_OF_FIRST_INTERFACE,          // Number of the interface in its configuration
#endif
    ALTERNATIVE_SETTING,                // Value to select this alternate interface setting
    NUMBER_USING_ENDPOINTS_THIRD_IFC,   // Number of endpoints used by the interface (excluding endpoint 0)
    THIRD_IFC_CLASS,                    // Interface class code
    THIRD_IFC_SUBCLASS,                 // Interface subclass code
    THIRD_IFC_PROTOCOL,                 // Interface protocol code
    THIRD_INTERFACE_STRING_INDEX        // Index of the interface string descriptor
  },
    {{ // endpoint 6
    sizeof(HAL_UsbEndPointDescptr_t),   // Size of the descriptor in bytes
    DESCRIPTOR_TYPE_ENDPOINT,           // Descriptor type
    ADDRESS_MSC_BULKOUT_PIPE,           // Address and direction of the endpoint
    BULK_TYPE_PIPE,                     // Endpoint type and additional characteristics (for isochronous endpoints)
    SZ_MSC_BULKOUT_ENDPOINT,            // Maximum packet size (in bytes) of the endpoint
    BULK_MAX_SPEED                      // Polling rate of the endpoint
  },
  { // endpoint 5
    sizeof(HAL_UsbEndPointDescptr_t),   // Size of the descriptor in bytes
    DESCRIPTOR_TYPE_ENDPOINT,           // Descriptor type
    ADDRESS_MSC_BULKIN_PIPE,            // Address and direction of the endpoint
    BULK_TYPE_PIPE,                     // Endpoint type and additional characteristics (for isochronous endpoints)
    SZ_MSC_BULKIN_ENDPOINT,             // Maximum packet size (in bytes) of the endpoint
    BULK_MAX_SPEED                      // Polling rate of the endpoint
  }},
#endif // (MSD_SUPPORT == 1)
#if (DFU_SUPPORT == 1)
  { // interface 4
    sizeof(InterfaceDescriptor_t),      // Size of the descriptor in bytes
    DESCRIPTOR_TYPE_INTERFACE,          // Descriptor type
#if (APP_INTERFACE == APP_INTERFACE_VCP)
  #if (MSD_SUPPORT == 1)
    NUMBER_OF_FOURTH_INTERFACE,         // Number of the interface in its configuration
  #else
    NUMBER_OF_THIRD_INTERFACE,          // Number of the interface in its configuration
  #endif // (MSD_SUPPORT == 1)
#else
  #if (MSD_SUPPORT == 1)
    NUMBER_OF_SECOND_INTERFACE,         // Number of the interface in its configuration
  #else
    #error
  #endif // (MSD_SUPPORT == 1)
#endif // (APP_INTERFACE == APP_INTERFACE_VCP)
    ALTERNATIVE_SETTING,                // Value to select this alternate interface setting
    NUMBER_USING_ENDPOINTS_FOURTH_IFC,  // Number of endpoints used by the interface (excluding endpoint 0)
    FOURTH_IFC_CLASS,                   // Interface class code
    FOURTH_IFC_SUBCLASS,                // Interface subclass code
    FOURTH_IFC_PROTOCOL,                // Interface protocol code
    FOURTH_INTERFACE_STRING_INDEX       // Index of the interface string descriptor
  },
  { // Functional descriptor
    sizeof(DfuFunctionalDescriptor_t),  // Size of the descriptor in bytes
    DESCRIPTOR_TYPE_DFU_FUNCTIONAL,     // Descriptor type
    DFU_ATTRIBUTES,                     // Descriptor attributes
    DFU_DETACH_TIMEOUT,                 // Wait timeout of USB reset after DFU_DETACH request recieption
    DFU_TRANSFER_SIZE,                  // Maximum number of bytes that the device can accept per control-write transaction
    DFU_VERSION                         // DFU Version
  }
#endif // (DFU_SUPPORT == 1)
};

// eof usbDescriptors.c
