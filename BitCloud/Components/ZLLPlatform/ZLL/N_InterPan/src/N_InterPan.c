/*********************************************************************************************//**
\file

\brief

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal

$Id: N_InterPan.c 24662 2013-03-04 08:37:46Z arazinkov $

***************************************************************************************************/

/***************************************************************************************************
* LOCAL INCLUDE FILES
***************************************************************************************************/

#include "N_InterPan_Bindings.h"
#include "N_InterPan.h"
#include "N_InterPan_Init.h"

/***************************************************************************************************
* EXTERNAL INCLUDE FILES
***************************************************************************************************/

#include "N_Address.h"
#include "N_DeviceInfo.h"
#include "N_ErrH.h"
#include "N_PacketDistributor.h"
#include "N_Types.h"
#include "N_Util.h"
#include <intrpData.h>
#include <clusters.h>
#include <N_InterPanBuffers.h>

/***************************************************************************************************
* LOCAL MACROS AND CONSTANTS
***************************************************************************************************/

#define COMPID "N_InterPan"

/** Maximum number of subscribers to this component. */
#ifndef N_INTERPAN_MAX_SUBSCRIBERS
  #define N_INTERPAN_MAX_SUBSCRIBERS      4u
#endif

#define SCAN_REQUEST_COMMAND_ID                             0x00u
#define SCAN_RESPONSE_COMMAND_ID                            0x01u
#define DEVICE_INFO_REQUEST_COMMAND_ID                      0x02u
#define DEVICE_INFO_RESPONSE_COMMAND_ID                     0x03u
#define IDENTIFY_REQUEST_COMMAND_ID                         0x06u
#define RESET_TO_FACTORY_NEW_REQUEST_COMMAND_ID             0x07u
#define NETWORK_START_REQUEST_COMMAND_ID                    0x10u
#define NETWORK_START_RESPONSE_COMMAND_ID                   0x11u
#define NETWORK_JOIN_ROUTER_REQUEST_COMMAND_ID              0x12u
#define NETWORK_JOIN_ROUTER_RESPONSE_COMMAND_ID             0x13u
#define NETWORK_JOIN_END_DEVICE_REQUEST_COMMAND_ID          0x14u
#define NETWORK_JOIN_END_DEVICE_RESPONSE_COMMAND_ID         0x15u
#define NETWORK_UPDATE_REQUEST_COMMAND_ID                   0x16u

/** The endpoint used for Inter-PAN communication. */
#define INTERPAN_ENDPOINT               35u

/** The cluster ID used for Inter-PAN communication. */
#define INTERPAN_CLUSTER_ID             0x1000u

/** The profile ID for the Inter-PAN endpoint */
#define INTERPAN_PROFILE_ID             ((uint16_t)N_DeviceInfo_Profile_ZLL)

/** The device ID for the Inter-PAN endpoint (network configurator). */
#define INTERPAN_DEVICE_ID              0x1000u

/** Frame Type in Inter-PAN ZCL header */
#define FRAME_TYPE_PROFILE_CMD          0x00u
#define FRAME_TYPE_SPECIFIC_CMD         0x01u

/** Frame Client/Server Directions in Inter-PAN ZCL header */
#define CLIENT_SERVER_DIR               0x00u
#define SERVER_CLIENT_DIR               0x01u

/***************************************************************************************************
* LOCAL TYPES
***************************************************************************************************/

typedef void (*CommandHandler_t)(INTRP_DataInd_t *ind);

/***************************************************************************************************
* LOCAL VARIABLES
***************************************************************************************************/

/** Used for all responses until their corresponding commands have been sent. */
static void DummyCommandHandler(INTRP_DataInd_t *ind)
{
  // nop
  (void)ind;
}

N_UTIL_CALLBACK_DECLARE(N_InterPan_Callback_t, s_subscribers, N_INTERPAN_MAX_SUBSCRIBERS);

/** Sequence number used for Inter-PAN transaction sequence number and the AF sequence number. */
static uint8_t s_sequenceNumber = 0u;

/** Used to check for invalid extended address. */
static const N_Address_Extended_t s_invalidExtendedAddress = { 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu };
static const N_Address_Extended_t s_zeroExtendedAddress = { 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u };

/** To save code size, the response handlers are only installed when the corresponding command is sent. */
static CommandHandler_t s_ProcessReceivedScanResponse = DummyCommandHandler;
static CommandHandler_t s_ProcessReceivedDeviceInfoResponse = DummyCommandHandler;
static CommandHandler_t s_ProcessReceivedNetworkStartResponse = DummyCommandHandler;
static CommandHandler_t s_ProcessReceivedNetworkJoinRouterResponse = DummyCommandHandler;
static CommandHandler_t s_ProcessReceivedNetworkJoinEndDeviceResponse = DummyCommandHandler;

static bool s_initialised = FALSE;

/******************************************************************************
                    Prototypes section
******************************************************************************/
static void INTRP_DataConf(INTRP_DataConf_t *conf);

/***************************************************************************************************
* LOCAL FUNCTIONS
***************************************************************************************************/
/**************************************************************************//**
\brief Confirmation of interPan data sending

\param[in] conf - pointer to confirmation structure
******************************************************************************/
static void INTRP_DataConf(INTRP_DataConf_t *conf)
{
  INTRP_DataReq_t *req = GET_PARENT_BY_FIELD(INTRP_DataReq_t, confirm, conf);
  freeInterPanBuffer(req);
}

static void DataRequest(uint16_t dataLength, uint8_t* pData, N_Address_Extended_t* pDestinationAddress, uint8_t* pSequenceNumber)
{
  INTRP_DataReq_t *req = getFreeInterPanBuffer();
  uint8_t *asdu;

  if (!req)
    N_ERRH_FATAL();

  asdu = getDataBuffer(req);

  if (pDestinationAddress)
  {
    req->dstAddrMode    = APS_EXT_ADDRESS;
    memcpy(&req->dstAddress.ext, pDestinationAddress, sizeof(req->dstAddress.ext));
  }
  else
  {
    req->dstAddrMode   = APS_SHORT_ADDRESS;
    req->dstAddress.sh = ALL_DEVICES_IN_PAN_ADDR;
  }
  memcpy(asdu, pData, dataLength);
  req->dstPANID       = MAC_BROADCAST_PANID;
  req->profileId      = PROFILE_ID_LIGHT_LINK;
  req->clusterId      = ZLL_COMMISSIONING_CLUSTER_ID;
  req->asduLength     = dataLength;
  req->asdu           = asdu;
  req->asduHandle     = 0;
  req->INTRP_DataConf = INTRP_DataConf;

  INTRP_DataReq(req);
  (void)pSequenceNumber;
}

static void Send(uint8_t commandId, bool isResponse, uint16_t dataLength, uint8_t* data, N_Address_Extended_t* pDestinationAddress)
{
    // create header
    N_InterPan_FrameHeader_t* header = (N_InterPan_FrameHeader_t*) data;
    header->frameControl.type = FRAME_TYPE_SPECIFIC_CMD;
    header->frameControl.manufacturerSpecific = 0u;
    header->frameControl.direction = isResponse ? SERVER_CLIENT_DIR : CLIENT_SERVER_DIR;
    header->frameControl.disableDefaultResponse = 1u;
    header->frameControl.reserved = 0u;
    header->transactionSequenceNumber = isResponse ? header->transactionSequenceNumber : s_sequenceNumber;
    header->commandId = commandId;

    DataRequest(dataLength, data, pDestinationAddress, isResponse ? &header->transactionSequenceNumber : &s_sequenceNumber);
}

static void ProcessReceivedScanRequest(INTRP_DataInd_t *ind)
{
    N_InterPan_ScanRequest_t* pPayload = (N_InterPan_ScanRequest_t*)(ind->asdu);

    if ( pPayload->transactionId == 0uL )
    {
        // invalid value for transaction id
        return;
    }
    if ( ((pPayload->zigBeeInfo & 0x03u) == 0x03u) )
    {
        // invalid value for zigbeeInfo.LogicalType (b1-b0)
        return;
    }
    if ( ((pPayload->zigBeeInfo & 0x03u) != 0x02u) &&
         ((pPayload->zigBeeInfo & 0x04u) == 0x00u) )
    {
        // router/coordinator must have RxOnIdle set
        return;
    }

    /// \todo Put back the special case for a unicast ScanRequest in a development build

    N_UTIL_CALLBACK(N_InterPan_Callback_t, s_subscribers, ReceivedScanRequest,
        (pPayload, ind->rssi, &(ind->srcAddress.raw)));
}

static void ProcessReceivedScanResponse(INTRP_DataInd_t *ind)
{
    N_InterPan_ScanResponse_t* pPayload = (N_InterPan_ScanResponse_t*)(ind->asdu);

    if ( ind->asduLength < 32u )
    {
        // invalid size. do not try to read the number of sub-devices
        return;
    }
    else
    {
        if ( pPayload->numberSubDevices == 1u )
        {
            if ( ind->asduLength != 39u )
            {
                // invalid size. the command must contain the optional fields
                return;
            }
        }
        else
        {
            if ( ind->asduLength != 32u )
            {
                // invalid size. the command must NOT contain the optional fields
                return;
            }
        }
    }

    // Note: clipping pPayload->touchlinkRssiCorrection to at most 0x20u must be done in
    // N_LinkInitiator instead of in N_InterPan, to be able to remain compatible with
    // Louvre/Actilume factory tests. See #111.

    if ( pPayload->transactionId == 0uL )
    {
        // invalid value for transaction id
        return;
    }
    if ( ((pPayload->zigBeeInfo & 0x03u) == 0x03u) )
    {
        // invalid value for zigbeeInfo.LogicalType (b1-b0)
        return;
    }
    if ( ((pPayload->zigBeeInfo & 0x03u) != 0x02u) &&
         ((pPayload->zigBeeInfo & 0x04u) == 0x00u) )
    {
        // router/coordinator must have RxOnIdle set
        return;
    }
    if ( memcmp(pPayload->extendedPanId, s_invalidExtendedAddress, sizeof(s_invalidExtendedAddress)) == 0 )
    {
        // invalid extended pan id
        return;
    }
    if ( pPayload->panId == 0xFFFFu )
    {
        // invalid pan id
        return;
    }

    // Note that address 0x0000 (coordinator address) is allowed!
    // this is needed for interoperability with standard ZigBee networks

    if ( (pPayload->networkAddress >= 0xFFF8u) && (pPayload->networkAddress < 0xFFFFu) )
    {
        // invalid network address
        return;
    }
    if ( (pPayload->networkAddress == 0xFFFFu) && ((pPayload->zllInfo & 0x01u) == 0x00u) )
    {
        // invalid network address, 0xFFFF is only allowed if the device is factory new
        return;
    }

    if ( pPayload->numberSubDevices == 1u )
    {
        // only check the following field if they are present
        if ( (pPayload->endPoint == 0x00u) || (pPayload->endPoint > 0xF0u) )
        {
            // invalid endpoint
            return;
        }
    }

    N_UTIL_CALLBACK(N_InterPan_Callback_t, s_subscribers, ReceivedScanResponse,
        (pPayload, ind->rssi, &(ind->srcAddress.raw)));
}

static void ProcessReceivedDeviceInfoRequest(INTRP_DataInd_t *ind)
{
    N_InterPan_DeviceInfoRequest_t* pPayload = (N_InterPan_DeviceInfoRequest_t*)(ind->asdu);

    N_UTIL_CALLBACK(N_InterPan_Callback_t, s_subscribers, ReceivedDeviceInfoRequest,
        (pPayload, &(ind->srcAddress.raw)));
}

static void ProcessReceivedDeviceInfoResponse(INTRP_DataInd_t *ind)
{
    N_InterPan_DeviceInfoResponse_t* pPayload = (N_InterPan_DeviceInfoResponse_t*)(ind->asdu);

    if ( ind->asduLength < (uint16_t)offsetof(N_InterPan_DeviceInfoResponse_t, deviceInfo))
    {
        // invalid size. do not try to read the number of sub-devices
        return;
    }
    else
    {
        if ( pPayload->count > N_INTERPAN_DEVICE_INFO_RECORD_MAX )
        {
            // invalid count
            return;
        }

        uint16_t expectedPayloadLength = (uint16_t) sizeof(N_InterPan_DeviceInfoResponse_t) -
            ((N_INTERPAN_DEVICE_INFO_RECORD_MAX - (uint16_t) pPayload->count) * (uint16_t) sizeof(N_InterPan_DeviceInfo_t));

        if ( ind->asduLength != expectedPayloadLength )
        {
            // invalid size. the length must match the count field
            return;
        }
    }

    N_UTIL_CALLBACK(N_InterPan_Callback_t, s_subscribers, ReceivedDeviceInfoResponse,
        (pPayload, &(ind->srcAddress.raw)));
}

static void ProcessReceivedIdentifyRequest(INTRP_DataInd_t *ind)
{
    N_InterPan_IdentifyRequest_t* pPayload = (N_InterPan_IdentifyRequest_t*)(ind->asdu);

    N_UTIL_CALLBACK(N_InterPan_Callback_t, s_subscribers, ReceivedIdentifyRequest,
        (pPayload, &(ind->srcAddress.raw)));
}

static void ProcessReceivedResetToFactoryNewRequest(INTRP_DataInd_t *ind)
{
    N_InterPan_ResetToFactoryNewRequest_t* pPayload = (N_InterPan_ResetToFactoryNewRequest_t*)(ind->asdu);

    N_UTIL_CALLBACK(N_InterPan_Callback_t, s_subscribers, ReceivedResetToFactoryNewRequest,
        (pPayload, &(ind->srcAddress.raw)));
}

static void ProcessReceivedNetworkStartRequest(INTRP_DataInd_t *ind)
{
    N_InterPan_NetworkStartRequest_t* pPayload = (N_InterPan_NetworkStartRequest_t*)(ind->asdu);

    N_UTIL_CALLBACK(N_InterPan_Callback_t, s_subscribers, ReceivedNetworkStartRequest,
        (pPayload, &(ind->srcAddress.raw)));
}

static void ProcessReceivedNetworkStartResponse(INTRP_DataInd_t *ind)
{
    N_InterPan_NetworkStartResponse_t* pPayload = (N_InterPan_NetworkStartResponse_t*)(ind->asdu);

    if ( (pPayload->channel < 11u) || (pPayload->channel > 26u) )
    {
        // invalid channel.
        return;
    }
    if ( (memcmp(pPayload->extendedPanId, s_invalidExtendedAddress, sizeof(s_invalidExtendedAddress)) == 0) )
    {
        // invalid extended pan id
        return;
    }
    if ( (memcmp(pPayload->extendedPanId, s_zeroExtendedAddress, sizeof(s_zeroExtendedAddress)) == 0) )
    {
        // invalid extended pan id
        return;
    }
    if ( pPayload->panId == 0xFFFFu )
    {
        // invalid pan id
        return;
    }

    N_UTIL_CALLBACK(N_InterPan_Callback_t, s_subscribers, ReceivedNetworkStartResponse,
        (pPayload, &(ind->srcAddress.raw)));
}

static void ProcessReceivedNetworkJoinRouterRequest(INTRP_DataInd_t *ind)
{
    N_InterPan_NetworkJoinRequest_t* pPayload = (N_InterPan_NetworkJoinRequest_t*)(ind->asdu);

    N_UTIL_CALLBACK(N_InterPan_Callback_t, s_subscribers, ReceivedNetworkJoinRouterRequest,
        (pPayload, &(ind->srcAddress.raw)));
}

static void ProcessReceivedNetworkJoinRouterResponse(INTRP_DataInd_t *ind)
{
    N_InterPan_NetworkJoinResponse_t* pPayload = (N_InterPan_NetworkJoinResponse_t*)(ind->asdu);

    N_UTIL_CALLBACK(N_InterPan_Callback_t, s_subscribers, ReceivedNetworkJoinRouterResponse,
        (pPayload, &(ind->srcAddress.raw)));
}

static void ProcessReceivedNetworkJoinEndDeviceRequest(INTRP_DataInd_t *ind)
{
    N_InterPan_NetworkJoinRequest_t* pPayload = (N_InterPan_NetworkJoinRequest_t*)(ind->asdu);

    N_UTIL_CALLBACK(N_InterPan_Callback_t, s_subscribers, ReceivedNetworkJoinEndDeviceRequest,
        (pPayload, &(ind->srcAddress.raw)));
}

static void ProcessReceivedNetworkJoinEndDeviceResponse(INTRP_DataInd_t *ind)
{
    N_InterPan_NetworkJoinResponse_t* pPayload = (N_InterPan_NetworkJoinResponse_t*)(ind->asdu);

    N_UTIL_CALLBACK(N_InterPan_Callback_t, s_subscribers, ReceivedNetworkJoinEndDeviceResponse,
        (pPayload, &(ind->srcAddress.raw)));
}

static void ProcessReceivedNetworkUpdateRequest(INTRP_DataInd_t *ind)
{
    N_InterPan_NetworkUpdateRequest_t* pPayload = (N_InterPan_NetworkUpdateRequest_t*)(ind->asdu);

    N_UTIL_CALLBACK(N_InterPan_Callback_t, s_subscribers, ReceivedNetworkUpdateRequest,
        (pPayload, &(ind->srcAddress.raw)));
}

/**************************************************************************//**
\brief INTRP-DATA indication primitive's prototype

\param[in] ind - INTRP-DATA indication parameters' structure pointer
 ******************************************************************************/

void INTRP_DataInd(INTRP_DataInd_t *ind)
{
    N_InterPan_FrameHeader_t* pHeader = (N_InterPan_FrameHeader_t*)(ind->asdu);

    // stop if this is not an Inter-PAN message
    // not sure that this check is needed indeed
    /*if (!N_PacketDistributor_IsInterPan(pMsg->srcAddr.panId, pMsg->srcAddr.endPoint))
    {
        return;
    }*/

    // Only handle interpan cluster messages
    if (INTERPAN_CLUSTER_ID == ind->clusterId)
    {
        // Only handle non-manufacturer specific commands (skip otherwise)
        if (!pHeader->frameControl.manufacturerSpecific)
        {
            switch (pHeader->commandId)
            {
            case SCAN_REQUEST_COMMAND_ID:
                if (ind->asduLength == sizeof(N_InterPan_ScanRequest_t))
                {
                    ProcessReceivedScanRequest(ind);
                }
                break;

            case SCAN_RESPONSE_COMMAND_ID:
                // size is checked in ProcessReceivedScanResponse
                s_ProcessReceivedScanResponse(ind);
                break;

            case DEVICE_INFO_REQUEST_COMMAND_ID:
                if (ind->asduLength == sizeof(N_InterPan_DeviceInfoRequest_t))
                {
                    ProcessReceivedDeviceInfoRequest(ind);
                }
                break;

            case DEVICE_INFO_RESPONSE_COMMAND_ID:
                // size is checked in ProcessReceivedDeviceInfoResponse
                s_ProcessReceivedDeviceInfoResponse(ind);
                break;

            case IDENTIFY_REQUEST_COMMAND_ID:
                if (ind->asduLength == sizeof(N_InterPan_IdentifyRequest_t))
                {
                    ProcessReceivedIdentifyRequest(ind);
                }
                break;

            case RESET_TO_FACTORY_NEW_REQUEST_COMMAND_ID:
                if (ind->asduLength == sizeof(N_InterPan_ResetToFactoryNewRequest_t))
                {
                    ProcessReceivedResetToFactoryNewRequest(ind);
                }
                break;

            case NETWORK_START_REQUEST_COMMAND_ID:
                if (ind->asduLength == sizeof(N_InterPan_NetworkStartRequest_t))
                {
                    ProcessReceivedNetworkStartRequest(ind);
                }
                break;

            case NETWORK_START_RESPONSE_COMMAND_ID:
                if (ind->asduLength == sizeof(N_InterPan_NetworkStartResponse_t))
                {
                    s_ProcessReceivedNetworkStartResponse(ind);
                }
                break;

            case NETWORK_JOIN_ROUTER_REQUEST_COMMAND_ID:
                if (ind->asduLength == sizeof(N_InterPan_NetworkJoinRequest_t))
                {
                    ProcessReceivedNetworkJoinRouterRequest(ind);
                }
                break;

            case NETWORK_JOIN_ROUTER_RESPONSE_COMMAND_ID:
                if (ind->asduLength == sizeof(N_InterPan_NetworkJoinResponse_t))
                {
                    s_ProcessReceivedNetworkJoinRouterResponse(ind);
                }
                break;

            case NETWORK_JOIN_END_DEVICE_REQUEST_COMMAND_ID:
                if (ind->asduLength == sizeof(N_InterPan_NetworkJoinRequest_t))
                {
                    ProcessReceivedNetworkJoinEndDeviceRequest(ind);
                }
                break;

            case NETWORK_JOIN_END_DEVICE_RESPONSE_COMMAND_ID:
                if (ind->asduLength == sizeof(N_InterPan_NetworkJoinResponse_t))
                {
                    s_ProcessReceivedNetworkJoinEndDeviceResponse(ind);
                }
                break;

            case NETWORK_UPDATE_REQUEST_COMMAND_ID:
                if (ind->asduLength == sizeof(N_InterPan_NetworkUpdateRequest_t))
                {
                    ProcessReceivedNetworkUpdateRequest(ind);
                }
                break;

            default:
                // unknown Inter-PAN message. ignore
                break;
            }
        }
    }

    N_UTIL_CALLBACK(N_InterPan_Callback_t, s_subscribers, ReceivedInterPanCommand,
        (ind->asduLength,ind->asdu, ind->rssi, &(ind->srcAddress.raw)));
}

/***************************************************************************************************
* EXPORTED FUNCTIONS
***************************************************************************************************/

/** Initialises the component
*/
void N_InterPan_Init(void)
{
    s_initialised = TRUE;
    INTRP_DataIndRegisterCallback(INTRP_DataInd);
}

/** Interface function, see \ref N_InterPan_Subscribe. */
void N_InterPan_Subscribe(const N_InterPan_Callback_t* pCallback)
{
    N_ERRH_ASSERT_FATAL(s_initialised);
    N_UTIL_CALLBACK_SUBSCRIBE(N_InterPan_Callback_t, s_subscribers, pCallback);
}

/** Interface function, see \ref N_InterPan_BroadcastScanRequest. */
void N_InterPan_BroadcastScanRequest(N_InterPan_ScanRequest_t* pPayload)
{
    s_ProcessReceivedScanResponse = ProcessReceivedScanResponse;
    Send(SCAN_REQUEST_COMMAND_ID, FALSE, sizeof(N_InterPan_ScanRequest_t), (uint8_t*) pPayload, NULL);
}

/** Interface function, see \ref N_InterPan_UnicastScanRequest. */
void N_InterPan_UnicastScanRequest(N_InterPan_ScanRequest_t* pPayload, N_Address_Extended_t* pDestinationAddress)
{
    s_ProcessReceivedScanResponse = ProcessReceivedScanResponse;
    Send(SCAN_REQUEST_COMMAND_ID, FALSE, sizeof(N_InterPan_ScanRequest_t), (uint8_t*) pPayload, pDestinationAddress);
}

/** Interface function, see \ref N_InterPan_SendScanResponse. */
void N_InterPan_SendScanResponse(N_InterPan_ScanResponse_t* pPayload, N_Address_Extended_t* pDestinationAddress)
{
    uint16_t payloadLength = sizeof(N_InterPan_ScanResponse_t);
    if ( pPayload->numberSubDevices != 1u )
    {
        payloadLength -= 7u;
    }
    Send(SCAN_RESPONSE_COMMAND_ID, TRUE, payloadLength, (uint8_t*) pPayload, pDestinationAddress);
}

/** Interface function, see \ref N_InterPan_SendDeviceInfoRequest. */
void N_InterPan_SendDeviceInfoRequest(N_InterPan_DeviceInfoRequest_t* pPayload, N_Address_Extended_t* pDestinationAddress)
{
    s_ProcessReceivedDeviceInfoResponse = ProcessReceivedDeviceInfoResponse;
    Send(DEVICE_INFO_REQUEST_COMMAND_ID, FALSE, sizeof(N_InterPan_DeviceInfoRequest_t), (uint8_t*) pPayload, pDestinationAddress);
}

/** Interface function, see \ref N_InterPan_SendDeviceInfoResponse. */
void N_InterPan_SendDeviceInfoResponse(N_InterPan_DeviceInfoResponse_t* pPayload, N_Address_Extended_t* pDestinationAddress)
{
    N_ERRH_ASSERT_FATAL(pPayload->count <= N_INTERPAN_DEVICE_INFO_RECORD_MAX);

    uint16_t payloadLength = (uint16_t) sizeof(N_InterPan_DeviceInfoResponse_t) -
        ((N_INTERPAN_DEVICE_INFO_RECORD_MAX - (uint16_t) pPayload->count) * (uint16_t) sizeof(N_InterPan_DeviceInfo_t));

    Send(DEVICE_INFO_RESPONSE_COMMAND_ID, TRUE, payloadLength, (uint8_t*) pPayload, pDestinationAddress);
}

/** Interface function, see \ref N_InterPan_SendIdentifyRequest. */
void N_InterPan_SendIdentifyRequest(N_InterPan_IdentifyRequest_t* pPayload, N_Address_Extended_t* pDestinationAddress)
{
    Send(IDENTIFY_REQUEST_COMMAND_ID, FALSE, sizeof(N_InterPan_IdentifyRequest_t), (uint8_t*) pPayload, pDestinationAddress);
}

/** Interface function, see \ref N_InterPan_SendResetToFactoryNewRequest. */
void N_InterPan_SendResetToFactoryNewRequest(N_InterPan_ResetToFactoryNewRequest_t* pPayload, N_Address_Extended_t* pDestinationAddress)
{
    Send(RESET_TO_FACTORY_NEW_REQUEST_COMMAND_ID, FALSE, sizeof(N_InterPan_ResetToFactoryNewRequest_t), (uint8_t*) pPayload, pDestinationAddress);
}

/** Interface function, see \ref N_InterPan_SendNetworkStartRequest. */
void N_InterPan_SendNetworkStartRequest(N_InterPan_NetworkStartRequest_t* pPayload, N_Address_Extended_t* pDestinationAddress)
{
    s_ProcessReceivedNetworkStartResponse = ProcessReceivedNetworkStartResponse;
    Send(NETWORK_START_REQUEST_COMMAND_ID, FALSE, sizeof(N_InterPan_NetworkStartRequest_t), (uint8_t*) pPayload, pDestinationAddress);
}

/** Interface function, see \ref N_InterPan_SendNetworkStartResponse. */
void N_InterPan_SendNetworkStartResponse(N_InterPan_NetworkStartResponse_t* pPayload, N_Address_Extended_t* pDestinationAddress)
{
    Send(NETWORK_START_RESPONSE_COMMAND_ID, TRUE, sizeof(N_InterPan_NetworkStartResponse_t), (uint8_t*) pPayload, pDestinationAddress);
}

/** Interface function, see \ref N_InterPan_SendNetworkJoinRouterRequest. */
void N_InterPan_SendNetworkJoinRouterRequest(N_InterPan_NetworkJoinRequest_t* pPayload, N_Address_Extended_t* pDestinationAddress)
{
    s_ProcessReceivedNetworkJoinRouterResponse = ProcessReceivedNetworkJoinRouterResponse;
    Send(NETWORK_JOIN_ROUTER_REQUEST_COMMAND_ID, FALSE, sizeof(N_InterPan_NetworkJoinRequest_t), (uint8_t*) pPayload, pDestinationAddress);
}

/** Interface function, see \ref N_InterPan_SendNetworkJoinRouterResponse. */
void N_InterPan_SendNetworkJoinRouterResponse(N_InterPan_NetworkJoinResponse_t* pPayload, N_Address_Extended_t* pDestinationAddress)
{
    Send(NETWORK_JOIN_ROUTER_RESPONSE_COMMAND_ID, TRUE, sizeof(N_InterPan_NetworkJoinResponse_t), (uint8_t*) pPayload, pDestinationAddress);
}

/** Interface function, see \ref N_InterPan_SendNetworkJoinEndDeviceRequest. */
void N_InterPan_SendNetworkJoinEndDeviceRequest(N_InterPan_NetworkJoinRequest_t* pPayload, N_Address_Extended_t* pDestinationAddress)
{
    s_ProcessReceivedNetworkJoinEndDeviceResponse = ProcessReceivedNetworkJoinEndDeviceResponse;
    Send(NETWORK_JOIN_END_DEVICE_REQUEST_COMMAND_ID, FALSE, sizeof(N_InterPan_NetworkJoinRequest_t), (uint8_t*) pPayload, pDestinationAddress);
}

/** Interface function, see \ref N_InterPan_SendNetworkJoinEndDeviceResponse. */
void N_InterPan_SendNetworkJoinEndDeviceResponse(N_InterPan_NetworkJoinResponse_t* pPayload, N_Address_Extended_t* pDestinationAddress)
{
    Send(NETWORK_JOIN_END_DEVICE_RESPONSE_COMMAND_ID, TRUE, sizeof(N_InterPan_NetworkJoinResponse_t), (uint8_t*) pPayload, pDestinationAddress);
}

/** Interface function, see \ref N_InterPan_SendNetworkUpdateRequest. */
void N_InterPan_SendNetworkUpdateRequest(N_InterPan_NetworkUpdateRequest_t* pPayload, N_Address_Extended_t* pDestinationAddress)
{
    Send(NETWORK_UPDATE_REQUEST_COMMAND_ID, FALSE, sizeof(N_InterPan_NetworkUpdateRequest_t), (uint8_t*) pPayload, pDestinationAddress);
}

/** Interface function, see \ref N_InterPan_SendInterPanCommand. */
void N_InterPan_SendInterPanCommand(uint8_t payloadLength, uint8_t* pPayload, N_Address_Extended_t* pDestinationAddress)
{
    DataRequest((uint16_t)payloadLength, pPayload, pDestinationAddress, &s_sequenceNumber);
}
