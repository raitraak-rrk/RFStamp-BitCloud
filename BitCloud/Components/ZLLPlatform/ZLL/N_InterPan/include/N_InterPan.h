/*********************************************************************************************//**
\file  

\brief 

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal

$Id: N_InterPan.h 24615 2013-02-27 05:14:24Z arazinkov $

Send/receive Inter-PAN commands. For detailed descriptions of the commands and their
parameters, see the specification (version 1.0).

Note that the current implementation only works on little-endian architectures.

***************************************************************************************************/

#ifndef N_INTERPAN_H
#define N_INTERPAN_H

/***************************************************************************************************
* INCLUDE FILES
***************************************************************************************************/

#include "N_Address.h"
#include "N_Types.h"

/***************************************************************************************************
* C++ DECLARATION WRAPPER
***************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************************************
* EXPORTED MACROS AND CONSTANTS
***************************************************************************************************/

#define NETWORK_KEY_LENGTH 16u

/** The maximum number of device info records in a device info response. */
#define N_INTERPAN_DEVICE_INFO_RECORD_MAX 5u

/* ZigBee info */
#define N_INTERPAN_ZIGBEE_INFO_LOGICAL_TYPE 0x03u
#define N_INTERPAN_ZIGBEE_INFO_COORDINATOR  0x00u
#define N_INTERPAN_ZIGBEE_INFO_ROUTER       0x01u
#define N_INTERPAN_ZIGBEE_INFO_END_DEVICE   0x02u

#define N_INTERPAN_ZIGBEE_INFO_RX_ON_IDLE   0x04u

/* ZLL info */
#define N_INTERPAN_ZLL_INFO_FACTORY_NEW                0x01u
#define N_INTERPAN_ZLL_INFO_ADDRESS_ASSIGNMENT         0x02u
#define N_INTERPAN_ZLL_INFO_TOUCHLINK_INITIATOR        0x10u
#define N_INTERPAN_ZLL_INFO_TOUCHLINK_PRIORITY_REQUEST 0x20u

/***************************************************************************************************
* EXPORTED TYPES
***************************************************************************************************/

typedef uint8_t NetworkKey_t[NETWORK_KEY_LENGTH];

// make sure that the Inter-PAN payload structures are packed
// ...assume that all compilers do support it
#pragma pack(1)

/** ZCL frame header - frame control field. */
typedef struct N_InterPan_FrameControl_t
{
    unsigned char type:2;
    unsigned char manufacturerSpecific:1;
    unsigned char direction:1;
    unsigned char disableDefaultResponse:1;
    unsigned char reserved:3;
} N_InterPan_FrameControl_t;

/** ZCL frame header.
    This field is filled in by N_InterPan before the frame is sent, so it can be left uninitialised.
*/
typedef struct N_InterPan_FrameHeader_t
{
    N_InterPan_FrameControl_t  frameControl;
    //uint16_t manufacturerCode;  // May be added later, but this requires additional code to skip it when not needed
    uint8_t                    transactionSequenceNumber;
    uint8_t                    commandId;
} N_InterPan_FrameHeader_t;

/** ZCL frame header with manufacturerCode.
    This field is filled in by N_InterPan before the frame is sent, so it can be left uninitialised.
*/
typedef struct N_InterPan_FrameHeader_ManufacturerSpecific_t
{
    N_InterPan_FrameControl_t  frameControl;
    uint16_t                   manufacturerCode;
    uint8_t                    transactionSequenceNumber;
    uint8_t                    commandId;
} N_InterPan_FrameHeader_ManufacturerSpecific_t;

/** Device (endpoint) info used in ScanResponse and DeviceInfoResponse commands */
typedef struct N_InterPan_DeviceInfo_t
{
    N_Address_Extended_t       ieeeAddress;
    uint8_t                    endPoint;
    uint16_t                   profileId;
    uint16_t                   deviceId;
    uint8_t                    version;
    uint8_t                    groupIds;
    uint8_t                    sort;
} N_InterPan_DeviceInfo_t;

/** ScanRequest command. */
typedef struct N_InterPan_ScanRequest_t
{
    N_InterPan_FrameHeader_t   header;
    uint32_t                   transactionId;
    uint8_t                    zigBeeInfo;
    uint8_t                    zllInfo;
} N_InterPan_ScanRequest_t;

/** ScanResponse command. */
typedef struct N_InterPan_ScanResponse_t
{
    N_InterPan_FrameHeader_t   header;
    uint32_t                   transactionId;
    uint8_t                    touchlinkRssiCorrection;
    uint8_t                    zigBeeInfo;
    uint8_t                    zllInfo;
    uint16_t                   keyBitMask;
    uint32_t                   responseId;
    N_Address_ExtendedPanId_t  extendedPanId;
    uint8_t                    networkUpdateId;
    uint8_t                    channel;
    uint16_t                   panId;
    uint16_t                   networkAddress;
    uint8_t                    numberSubDevices;
    uint8_t                    totalGroupIds;
    // optional fields only present when numberSubDevices == 1
    uint8_t                    endPoint;
    uint16_t                   profileId;
    uint16_t                   deviceId;
    uint8_t                    version;
    uint8_t                    groupIds;
} N_InterPan_ScanResponse_t;

/** DeviceInfoRequest command. */
typedef struct N_InterPan_DeviceInfoRequest_t
{
    N_InterPan_FrameHeader_t   header;
    uint32_t                   transactionId;
    uint8_t                    startIndex;
} N_InterPan_DeviceInfoRequest_t;

/** DeviceInfoResponse command. */
typedef struct N_InterPan_DeviceInfoResponse_t
{
    N_InterPan_FrameHeader_t   header;
    uint32_t                   transactionId;
    uint8_t                    numberSubDevices;
    uint8_t                    startIndex;
    uint8_t                    count;
    N_InterPan_DeviceInfo_t    deviceInfo[N_INTERPAN_DEVICE_INFO_RECORD_MAX];
} N_InterPan_DeviceInfoResponse_t;

/** IdentifyRequest command. */
typedef struct N_InterPan_IdentifyRequest_t
{
    N_InterPan_FrameHeader_t   header;
    uint32_t                   transactionId;
    uint16_t                   identifyTime;
} N_InterPan_IdentifyRequest_t;

/** NetworkStartRequest command. */
typedef struct N_InterPan_NetworkStartRequest_t
{
    N_InterPan_FrameHeader_t   header;
    uint32_t                   transactionId;
    N_Address_ExtendedPanId_t  extendedPanId;
    uint8_t                    keyIndex;
    NetworkKey_t               encryptedNetworkKey;
    uint8_t                    channel;
    uint16_t                   panId;
    uint16_t                   networkAddress;
    uint16_t                   groupIdsBegin;
    uint16_t                   groupIdsEnd;
    uint16_t                   freeNetworkAddressRangeBegin;
    uint16_t                   freeNetworkAddressRangeEnd;
    uint16_t                   freeGroupIdRangeBegin;
    uint16_t                   freeGroupIdRangeEnd;
    N_Address_Extended_t       endDeviceIeeeAddress;
    uint16_t                   endDeviceNetworkAddress;
} N_InterPan_NetworkStartRequest_t;

/** NetworkStartResponse command. */
typedef struct N_InterPan_NetworkStartResponse_t
{
    N_InterPan_FrameHeader_t   header;
    uint32_t                   transactionId;
    uint8_t                    status;
    N_Address_ExtendedPanId_t  extendedPanId;
    uint8_t                    networkUpdateId;
    uint8_t                    channel;
    uint16_t                   panId;
} N_InterPan_NetworkStartResponse_t;

/** NetworkJoinRouterRequest and NetworkJoinEndDeviceRequest commands (same payload). */
typedef struct N_InterPan_NetworkJoinRequest_t
{
    N_InterPan_FrameHeader_t   header;
    uint32_t                   transactionId;
    N_Address_ExtendedPanId_t  extendedPanId;
    uint8_t                    keyIndex;
    NetworkKey_t               encryptedNetworkKey;
    uint8_t                    networkUpdateId;
    uint8_t                    channel;
    uint16_t                   panId;
    uint16_t                   networkAddress;
    uint16_t                   groupIdsBegin;
    uint16_t                   groupIdsEnd;
    uint16_t                   freeNetworkAddressRangeBegin;
    uint16_t                   freeNetworkAddressRangeEnd;
    uint16_t                   freeGroupIdRangeBegin;
    uint16_t                   freeGroupIdRangeEnd;
} N_InterPan_NetworkJoinRequest_t;

/** NetworkJoinRouterResponse and NetworkJoinEndDeviceResponse commands (same payload). */
typedef struct N_InterPan_NetworkJoinResponse_t
{
    N_InterPan_FrameHeader_t   header;
    uint32_t                   transactionId;
    uint8_t                    status;
} N_InterPan_NetworkJoinResponse_t;

/** NetworkUpdateRequest command. */
typedef struct N_InterPan_NetworkUpdateRequest_t
{
    N_InterPan_FrameHeader_t   header;
    uint32_t                   transactionId;
    N_Address_ExtendedPanId_t  extendedPanId;
    uint8_t                    networkUpdateId;
    uint8_t                    channel;
    uint16_t                   panId;
    uint16_t                   networkAddress;
} N_InterPan_NetworkUpdateRequest_t;

/** ResetToFactoryDefaultsRequest command. */
typedef struct N_InterPan_ResetToFactoryNewRequest_t
{
    N_InterPan_FrameHeader_t   header;
    uint32_t                   transactionId;
} N_InterPan_ResetToFactoryNewRequest_t;

// back to the default packing
// ...assume that all compilers do support it (only tested with visual studio)
#pragma pack()

/** Callback structure. */
typedef struct N_InterPan_Callback_t
{
    /** Received a ScanRequest command. */
    void (*ReceivedScanRequest)(N_InterPan_ScanRequest_t* pPayload, int8_t rssi, N_Address_Extended_t* pSourceAddress);
    /** Received a ScanResponse command. */
    void (*ReceivedScanResponse)(N_InterPan_ScanResponse_t* pPayload, int8_t rssi, N_Address_Extended_t* pSourceAddress);
    /** Received a DeviceInfoRequest command. */
    void (*ReceivedDeviceInfoRequest)(N_InterPan_DeviceInfoRequest_t* pPayload, N_Address_Extended_t* pSourceAddress);
    /** Received a DeviceInfoResponse command. */
    void (*ReceivedDeviceInfoResponse)(N_InterPan_DeviceInfoResponse_t* pPayload, N_Address_Extended_t* pSourceAddress);
    /** Received a IdentifyRequest command. */
    void (*ReceivedIdentifyRequest)(N_InterPan_IdentifyRequest_t* pPayload, N_Address_Extended_t* pSourceAddress);
    /** Received a NetworkStartRequest command. */
    void (*ReceivedNetworkStartRequest)(N_InterPan_NetworkStartRequest_t* pPayload, N_Address_Extended_t* pSourceAddress);
    /** Received a NetworkStartResponse command. */
    void (*ReceivedNetworkStartResponse)(N_InterPan_NetworkStartResponse_t* pPayload, N_Address_Extended_t* pSourceAddress);
    /** Received a NetworkJoinRouterRequest command. */
    void (*ReceivedNetworkJoinRouterRequest)(N_InterPan_NetworkJoinRequest_t* pPayload, N_Address_Extended_t* pSourceAddress);
    /** Received a NetworkJoinRouterResponse command. */
    void (*ReceivedNetworkJoinRouterResponse)(N_InterPan_NetworkJoinResponse_t* pPayload, N_Address_Extended_t* pSourceAddress);
    /** Received a NetworkJoinEndDeviceRequest command. */
    void (*ReceivedNetworkJoinEndDeviceRequest)(N_InterPan_NetworkJoinRequest_t* pPayload, N_Address_Extended_t* pSourceAddress);
    /** Received a NetworkJoinEndDeviceResponse command. */
    void (*ReceivedNetworkJoinEndDeviceResponse)(N_InterPan_NetworkJoinResponse_t* pPayload, N_Address_Extended_t* pSourceAddress);
    /** Received a NetworkUpdateRequest command. */
    void (*ReceivedNetworkUpdateRequest)(N_InterPan_NetworkUpdateRequest_t* pPayload, N_Address_Extended_t* pSourceAddress);
    /** Received a ResetToFactoryNew command. */
    void (*ReceivedResetToFactoryNewRequest)(N_InterPan_ResetToFactoryNewRequest_t* pPayload, N_Address_Extended_t* pSourceAddress);

    /** Generic callback to handle an Inter-PAN command. Keep this callback last in the list.
        \param pPayload The payload data that has been received
        \param payloadLength The length of the payload data
        \param rssi The received signal strength indication
        \param pSourceAddress The IEEE address of the sender of the of the message
        \note The payload includes a ZCL header.
        \note This callback is also called for any of the 'known' InterPan commands.
    */
    void (*ReceivedInterPanCommand)(uint16_t payloadLength, uint8_t* pPayload, int8_t rssi, N_Address_Extended_t* pSourceAddress);

    /** Guard to ensure the initialiser contains all functions. */
    int8_t endOfList;
} N_InterPan_Callback_t;

/***************************************************************************************************
* EXPORTED FUNCTIONS
***************************************************************************************************/

/** Subscribe for callbacks from this component.
    \param pCallback Pointer to filled callback structure
*/
void N_InterPan_Subscribe(const N_InterPan_Callback_t* pCallback);

/** Broadcast ScanRequest command.
    \param pPayload Pointer to filled payload structure, the header should be uninitialized
*/
void N_InterPan_BroadcastScanRequest(N_InterPan_ScanRequest_t* pPayload);

/** Unicast ScanRequest command.
    \param pPayload Pointer to filled payload structure, the header should be uninitialized
    \param pDestinationAddress Destination IEEE address
*/
void N_InterPan_UnicastScanRequest(N_InterPan_ScanRequest_t* pPayload, N_Address_Extended_t* pDestinationAddress);

/** Send a ScanResponse command.
    \param pPayload Pointer to filled payload structure, the header should contain the same sequence number as the request, rest should be uninitialized
    \param pDestinationAddress Destination IEEE address
*/
void N_InterPan_SendScanResponse(N_InterPan_ScanResponse_t* pPayload, N_Address_Extended_t* pDestinationAddress);

/** Send a DeviceInfoRequest command.
    \param pPayload Pointer to filled payload structure, the header should be uninitialized
    \param pDestinationAddress Destination IEEE address
*/
void N_InterPan_SendDeviceInfoRequest(N_InterPan_DeviceInfoRequest_t* pPayload, N_Address_Extended_t* pDestinationAddress);

/** Send a DeviceInfoResponse command.
    \param pPayload Pointer to filled payload structure, the header should contain the same sequence number as the request, rest should be uninitialized
    \param pDestinationAddress Destination IEEE address
*/
void N_InterPan_SendDeviceInfoResponse(N_InterPan_DeviceInfoResponse_t* pPayload, N_Address_Extended_t* pDestinationAddress);

/** Send a IdentifyRequest command.
    \param pPayload Pointer to filled payload structure, the header should be uninitialized
    \param pDestinationAddress Destination IEEE address
*/
void N_InterPan_SendIdentifyRequest(N_InterPan_IdentifyRequest_t* pPayload, N_Address_Extended_t* pDestinationAddress);

/** Send a NetworkStartRequest command.
    \param pPayload Pointer to filled payload structure, the header should be uninitialized
    \param pDestinationAddress Destination IEEE address
*/
void N_InterPan_SendNetworkStartRequest(N_InterPan_NetworkStartRequest_t* pPayload, N_Address_Extended_t* pDestinationAddress);

/** Send a NetworkStartResponse command.
    \param pPayload Pointer to filled payload structure, the header should contain the same sequence number as the request, rest should be uninitialized
    \param pDestinationAddress Destination IEEE address
*/
void N_InterPan_SendNetworkStartResponse(N_InterPan_NetworkStartResponse_t* pPayload, N_Address_Extended_t* pDestinationAddress);

/** Send a NetworkJoinEndDeviceRequest command.
    \param pPayload Pointer to filled payload structure, the header should be uninitialized
    \param pDestinationAddress Destination IEEE address
*/
void N_InterPan_SendNetworkJoinRouterRequest(N_InterPan_NetworkJoinRequest_t* pPayload, N_Address_Extended_t* pDestinationAddress);

/** Send a NetworkJoinEndDeviceResponse command.
    \param pPayload Pointer to filled payload structure, the header should contain the same sequence number as the request, rest should be uninitialized
    \param pDestinationAddress Destination IEEE address
*/
void N_InterPan_SendNetworkJoinRouterResponse(N_InterPan_NetworkJoinResponse_t* pPayload, N_Address_Extended_t* pDestinationAddress);

/** Send a NetworkJoinEndDeviceRequest command.
    \param pPayload Pointer to filled payload structure, the header should be uninitialized
    \param pDestinationAddress Destination IEEE address
*/
void N_InterPan_SendNetworkJoinEndDeviceRequest(N_InterPan_NetworkJoinRequest_t* pPayload, N_Address_Extended_t* pDestinationAddress);

/** Send a NetworkJoinEndDeviceResponse command.
    \param pPayload Pointer to filled payload structure, the header should contain the same sequence number as the request, rest should be uninitialized
    \param pDestinationAddress Destination IEEE address
*/
void N_InterPan_SendNetworkJoinEndDeviceResponse(N_InterPan_NetworkJoinResponse_t* pPayload, N_Address_Extended_t* pDestinationAddress);

/** Send a NetworkUpdateRequest command.
    \param pPayload Pointer to filled payload structure, the header should be uninitialized
    \param pDestinationAddress Destination IEEE address
*/
void N_InterPan_SendNetworkUpdateRequest(N_InterPan_NetworkUpdateRequest_t* pPayload, N_Address_Extended_t* pDestinationAddress);

/** Send a ResetToFactoryNewRequest command.
    \param pPayload Pointer to filled payload structure, the header should be uninitialized
    \param pDestinationAddress Destination IEEE address
*/
void N_InterPan_SendResetToFactoryNewRequest(N_InterPan_ResetToFactoryNewRequest_t* pPayload, N_Address_Extended_t* pDestinationAddress);

/** Send an Inter-PAN command.
    \param payloadLength Length of the payload data (in bytes)
    \param pPayload Pointer to payload data
    \param pDestinationAddress Destination IEEE address, or NULL to broadcast the command.
    \note The payload should include a proper ZCL header.
*/
void N_InterPan_SendInterPanCommand(uint8_t payloadLength, uint8_t* pPayload, N_Address_Extended_t* pDestinationAddress);

/***************************************************************************************************
* END OF C++ DECLARATION WRAPPER
***************************************************************************************************/

#ifdef __cplusplus
}
#endif

#endif // N_INTERPAN_H
