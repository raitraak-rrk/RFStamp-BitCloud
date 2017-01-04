/*********************************************************************************************//**
\file

\brief

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal

$Id: N_Zcl_Framework.h 12382 2015-01-13 10:09:34Z francisco.vidal@philips.com $

***************************************************************************************************/

#ifndef N_ZCL_FRAMEWORK_H
#define N_ZCL_FRAMEWORK_H

/***************************************************************************************************
* INCLUDE FILES
***************************************************************************************************/

#include "N_Address.h"
#include "N_Types.h"
#include <apsdeData.h>

/***************************************************************************************************
* C++ DECLARATION WRAPPER
***************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************************************
* EXPORTED TYPES
***************************************************************************************************/

#if !defined(N_ZCL_APS_BUFFERS_AMOUNT) 
/* The maximum possible ammount of concurrent ZCL requests. */
#define N_ZCL_APS_BUFFERS_AMOUNT 5u
#endif

/** Common return value for sending ZCL commands.
*/
typedef enum N_Zcl_SendStatus_t
{
    N_Zcl_SendStatus_Ok = 0x00u,
    N_Zcl_SendStatus_Failure = 0x01u,
    N_Zcl_SendStatus_OutOfMemory = 0x89u,
} N_Zcl_SendStatus_t;

/** ZCL status codes from ZigBee Cluster Library 20100426, table 2.16.
*/
typedef enum N_Zcl_Status_t
{
    N_ZCL_STATUS_SUCCESS =                             (uint8_t)0x00u,
    N_ZCL_STATUS_FAILURE =                             (uint8_t)0x01u,

    N_ZCL_STATUS_NOT_AUTHORIZED =                      (uint8_t)0x7Eu,
    N_ZCL_STATUS_RESERVED_FIELD_NOT_ZERO =             (uint8_t)0x7Fu,
    N_ZCL_STATUS_MALFORMED_COMMAND =                   (uint8_t)0x80u,
    N_ZCL_STATUS_UNSUP_CLUSTER_COMMAND =               (uint8_t)0x81u,
    N_ZCL_STATUS_UNSUP_GENERAL_COMMAND =               (uint8_t)0x82u,
    N_ZCL_STATUS_UNSUP_MANU_CLUSTER_COMMAND =          (uint8_t)0x83u,
    N_ZCL_STATUS_UNSUP_MANU_GENERAL_COMMAND =          (uint8_t)0x84u,
    N_ZCL_STATUS_INVALID_FIELD =                       (uint8_t)0x85u,
    N_ZCL_STATUS_UNSUPPORTED_ATTRIBUTE =               (uint8_t)0x86u,
    N_ZCL_STATUS_INVALID_VALUE =                       (uint8_t)0x87u,
    N_ZCL_STATUS_READ_ONLY =                           (uint8_t)0x88u,
    N_ZCL_STATUS_INSUFFICIENT_SPACE =                  (uint8_t)0x89u,
    N_ZCL_STATUS_DUPLICATE_EXISTS =                    (uint8_t)0x8Au,
    N_ZCL_STATUS_NOT_FOUND =                           (uint8_t)0x8Bu,
    N_ZCL_STATUS_UNREPORTABLE_ATTRIBUTE =              (uint8_t)0x8Cu,
    N_ZCL_STATUS_INVALID_DATA_TYPE =                   (uint8_t)0x8Du,
    N_ZCL_STATUS_INVALID_SELECTOR =                    (uint8_t)0x8Eu,
    N_ZCL_STATUS_WRITE_ONLY =                          (uint8_t)0x8Fu,
    N_ZCL_STATUS_INCONSISTENT_STARTUP_STATE =          (uint8_t)0x90u,
    N_ZCL_STATUS_DEFINED_OUT_OF_BAND =                 (uint8_t)0x91u,
    N_ZCL_STATUS_INCONSISTENT =                        (uint8_t)0x92u,
    N_ZCL_STATUS_ACTION_DENIED =                       (uint8_t)0x93u,
    N_ZCL_STATUS_TIMEOUT =                             (uint8_t)0x94u,
    N_ZCL_STATUS_ABORT =                               (uint8_t)0x95u,
    N_ZCL_STATUS_INVALID_IMAGE =                       (uint8_t)0x96u,
    N_ZCL_STATUS_WAIT_FOR_DATA =                       (uint8_t)0x97u,
    N_ZCL_STATUS_NO_IMAGE_AVAILABLE =                  (uint8_t)0x98u,
    N_ZCL_STATUS_REQUIRE_MORE_IMAGE =                  (uint8_t)0x99u,

    N_ZCL_STATUS_HARDWARE_FAILURE =                    (uint8_t)0xC0u,
    N_ZCL_STATUS_SOFTWARE_FAILURE =                    (uint8_t)0xC1u,
    N_ZCL_STATUS_CALIBRATION_ERROR =                   (uint8_t)0xC2u,

    /** This special value is internally used to indicate that no default response should be
        generated. It is never sent over the air. */
    N_ZCL_STATUS_CMD_HAS_RSP =                         (uint8_t)0xFFu
} N_Zcl_Status_t;

/** Frame control field from the ZCL header. */
typedef struct
{
    unsigned int type:2;
    unsigned int manufacturerSpecific:1;
    unsigned int direction:1;
    unsigned int disableDefaultResponse:1;
} N_Zcl_Framework_Control_t;

/** Parsed ZCL header. */
typedef struct
{
    N_Zcl_Framework_Control_t frameControl;
    uint16_t manufacturerCode;  /**< Use \ref N_ZCL_MANUFACTURER_CODE_NONE if not manufacturer specific. */
    uint8_t transactionSequenceNumber;
    uint8_t commandId;
} N_Zcl_Framework_Hdr_t;

/** Incoming ZCL command. */
typedef struct
{
    /** Parsed ZCL header. */
    N_Zcl_Framework_Hdr_t hdr;

    /** The cluster ID of the incoming message. */
    uint16_t clusterId;

    /** Pointer to ZCL payload. */
    uint8_t *pData;

    /** Length of ZCL payload. */
    uint8_t pDataLen;
} N_Zcl_Framework_Incoming_t;

/** Callback function for received foundation messages.
    \param pIncoming The incoming Zcl message
    \returns Status code. If the status is anything other than \ref N_ZCL_STATUS_CMD_HAS_RSP, and the
        command was unicast, the framework will generate a default response.
*/
typedef N_Zcl_Status_t (*N_Zcl_Framework_ReceivedFoundationCommand_t)(const N_Zcl_Framework_Incoming_t* pIncoming);

/** Callback function for received cluster messages.
    \param pIncoming The incoming Zcl message
    \returns Status code. If the status is anything other than \ref N_ZCL_STATUS_CMD_HAS_RSP, and the
        command was unicast, the framework will generate a default response.
*/
typedef N_Zcl_Status_t (*N_Zcl_Framework_ReceivedClusterCommand_t)(const N_Zcl_Framework_Incoming_t* pIncoming);

/** Generic per-endpoint subscriber.

    Used by the \ref N_Zcl_Framework_AddSubscriber and \ref N_Zcl_Framework_FindSubscriber
    helper functions.
*/
typedef struct N_Zcl_Framework_Subscriber_t
{
    uint8_t endpoint;
    const void* pCallback;
} N_Zcl_Framework_Subscriber_t;

/** Status codes returned by the DataConfirmation callback function. All status codes returned are according
    to the ZigBee (r20) and the IEEE 802.15.4-2003 specifications.
 */
typedef enum N_Zcl_DataConfirmationStatus_t
{
    N_Zcl_DataConfirmationStatus_Success,
    N_Zcl_DataConfirmationStatus_Failure,

    /* See ZigBee r20 specification, table 3.57 (NWK layer status values) */
    N_Zcl_DataConfirmationStatus_NwkInvalidParameter     = 0xC1,
    N_Zcl_DataConfirmationStatus_NwkInvalidRequest       = 0xC2,
    N_Zcl_DataConfirmationStatus_NwkNotPermitted         = 0xC3,
    N_Zcl_DataConfirmationStatus_NwkStartupFailure       = 0xC4,
    N_Zcl_DataConfirmationStatus_NwkAlreadyPresent       = 0xC5,
    N_Zcl_DataConfirmationStatus_NwkSyncFail             = 0xC6,
    N_Zcl_DataConfirmationStatus_NwkNeibTableFull        = 0xC7,
    N_Zcl_DataConfirmationStatus_NwkUnknownDevice        = 0xC8,
    N_Zcl_DataConfirmationStatus_NwkUnsupportedAttribute = 0xC9,
    N_Zcl_DataConfirmationStatus_NwkNoNetwork            = 0xCA,
    N_Zcl_DataConfirmationStatus_NwkMaxFrmCounter        = 0xCC,
    N_Zcl_DataConfirmationStatus_NwkNoKey                = 0xCD,
    N_Zcl_DataConfirmationStatus_NwkBadCcmOutput         = 0xCE,
    N_Zcl_DataConfirmationStatus_NwkNoRoute              = 0xCF,
    N_Zcl_DataConfirmationStatus_NwkRouteDiscoveryFailed = 0xD0,
    N_Zcl_DataConfirmationStatus_NwkRouteError           = 0xD1,
    N_Zcl_DataConfirmationStatus_NwkBTTableFull          = 0xD2,
    N_Zcl_DataConfirmationStatus_NwkFrameNotBuffered     = 0xD3,
    
    /* See IEEE 802.15.4-2003 specification, table 64 (MAC enumerations) */
    N_Zcl_DataConfirmationStatus_MacBeaconLoss           = 0xE0,
    N_Zcl_DataConfirmationStatus_MacChannelAccessFailure = 0xE1,
    N_Zcl_DataConfirmationStatus_MacDenied               = 0xE2,
    N_Zcl_DataConfirmationStatus_MacDisableTrxFailure    = 0xE3,
    N_Zcl_DataConfirmationStatus_MacFailedSecurityCheck  = 0xE4,
    N_Zcl_DataConfirmationStatus_MacFrameTooLong         = 0xE5,
    N_Zcl_DataConfirmationStatus_MacInvalidGTS           = 0xE6,
    N_Zcl_DataConfirmationStatus_MacInvalidHandle        = 0xE7,
    N_Zcl_DataConfirmationStatus_MacInvalidParameter     = 0xE8,
    N_Zcl_DataConfirmationStatus_MacNoAck                = 0xE9,
    N_Zcl_DataConfirmationStatus_MacNoBeacon             = 0xEA,
    N_Zcl_DataConfirmationStatus_MacNoData               = 0xEB,
    N_Zcl_DataConfirmationStatus_MacNoShortAddress       = 0xEC,
    N_Zcl_DataConfirmationStatus_MacOutOfCap             = 0xED,
    N_Zcl_DataConfirmationStatus_MacPanIdConflict        = 0xEE,
    N_Zcl_DataConfirmationStatus_MacRealignment          = 0xEF,
    N_Zcl_DataConfirmationStatus_MacTranscationExpired   = 0xF0,
    N_Zcl_DataConfirmationStatus_MacTransactionOverflow  = 0xF1,
    N_Zcl_DataConfirmationStatus_MacTxActive             = 0xF2,
    N_Zcl_DataConfirmationStatus_MacUnAvailableKey       = 0xF3,
    N_Zcl_DataConfirmationStatus_MacUnsupportedAttribute = 0xF4
} N_Zcl_DataConfirmationStatus_t;

/** Generic callback structure
    Used by \ref N_Zcl_Framework_Subscribe function.
*/
typedef struct N_Zcl_Framework_Callback_t
{
    /** Called when a Zcl command is received */
    void (*MessageReceived)(void);

    /** Called to confirm the message was sent successfully or has failed to send.
        \param endpoint The endpoint for which the data confirmation is
        \param transactionSeqNr Identification of the command for which the confirmation is. 
            See \ref N_Zcl_Framework_GetNextTransactionSequenceNumber
        \param status The send status.
    */
    void (*DataConfirmation)(uint8_t endpoint, uint8_t transactionSeqNr, N_Zcl_DataConfirmationStatus_t status);

    int8_t endOfList; // guard, must be initialized to -1
} N_Zcl_Framework_Callback_t;

// make sure that the payload structures are packed
// ...assume that all compilers do support it
#pragma pack(1)

typedef struct N_Zcl_Control_t
{
    uint16_t clusterId;
    uint8_t payloadSize;
    union
    {
        uint8_t field;
        struct
        {
            uint8_t frameType:2;
            uint8_t manufacturerSpecific:1;
            uint8_t direction:1;
            uint8_t disableDefaultResponse:1;
        } bits;
    } frameControl;
} N_Zcl_Control_t;

typedef struct N_Zcl_Command_t
{
    N_Zcl_Control_t control;
    uint8_t sequenceNumber;
    uint8_t commandId;
} N_Zcl_Command_t;

typedef struct N_Zcl_ManufacturerCommand_t
{
    N_Zcl_Control_t control;
    uint16_t manufacturerCode;
    uint8_t sequenceNumber;
    uint8_t commandId;
} N_Zcl_ManufacturerCommand_t;

// back to the default packing
// ...assume that all compilers do support it (only tested with visual studio)
#pragma pack()

/***************************************************************************************************
* EXPORTED MACROS AND CONSTANTS
***************************************************************************************************/

/** Manufacturer code for non-manufacturer specific clusters/commands/attributes. */
#define N_ZCL_MANUFACTURER_CODE_NONE 0x0000u

#define N_ZCL_FRAMEWORK_INVALID_CLUSTER_ID 0xFFFFu

/** No options for the ZCL request command.
    This means that the "disable default response" is NOT set.
*/
#define N_ZCL_COMMAND_OPTION_NONE 0x00u

/** Send the ZCL request command with the "disable default response" field set.
*/
#define N_ZCL_COMMAND_OPTION_DEFAULT_RESPONSE_ONLY_ON_ERROR 0x01u

/*** Frame Types ***/
#define N_ZCL_FRAMEWORK_FRAME_TYPE_PROFILE_CMD 0x00u
#define N_ZCL_FRAMEWORK_FRAME_TYPE_SPECIFIC_CMD 0x01u

/*** Frame Client/Server Directions ***/
#define N_ZCL_FRAMEWORK_CLIENT_SERVER_DIR 0x00u
#define N_ZCL_FRAMEWORK_SERVER_CLIENT_DIR 0x01u

#define N_ZCL_CMD_DEFAULT_RSP 0x0Bu

#define N_ZCL_FRAMEWORK_FRAME_CONTROL_TYPE_PROFILE_CMD      0x00u
#define N_ZCL_FRAMEWORK_FRAME_CONTROL_TYPE_SPECIFIC_CMD     0x01u
#define N_ZCL_FRAMEWORK_FRAME_CONTROL_MANUFACTURER_SPECIFIC 0x04u
#define N_ZCL_FRAMEWORK_FRAME_CONTROL_CLIENT_SERVER_DIR     0x00u
#define N_ZCL_FRAMEWORK_FRAME_CONTROL_SERVER_CLIENT_DIR     0x08u
#define N_ZCL_FRAMEWORK_FRAME_CONTROL_DISABLE_DEFAULT_RSP   0x10u

#define N_ZCL_FRAMEWORK_FRAME_CONTROL_CLUSTER_REQUEST \
    (N_ZCL_FRAMEWORK_FRAME_CONTROL_TYPE_SPECIFIC_CMD | N_ZCL_FRAMEWORK_FRAME_CONTROL_CLIENT_SERVER_DIR)

/***************************************************************************************************
* EXPORTED FUNCTIONS for SENDING ZCL MESSAGES
***************************************************************************************************/

/** Send a ZCL command
    \param sourceEndpoint The source endpoint
    \param clusterId The cluster id
    \param commandId The command id
    \param frameType The frame type
    \param direction The direction
    \param disableDefaultResponse Disable the default response?
    \param manufacturerCode The manufacturer code
    \param sequenceNumber The sequence number
    \param zclPayloadLength The length of the payload
    \param pZclPayload The payload
    \param pDestinationAddress The destination address
    \returns success status
*/
N_Zcl_SendStatus_t N_Zcl_Framework_SendCommand(uint8_t sourceEndpoint,
                                      uint16_t clusterId,
                                      uint8_t commandId,
                                      uint8_t frameType,
                                      uint8_t direction,
                                      uint8_t disableDefaultResponse,
                                      uint16_t manufacturerCode,
                                      uint8_t sequenceNumber,
                                      uint8_t zclPayloadLength,
                                      const uint8_t *pZclPayload,
                                      N_Address_t *pDestinationAddress);

/** Wrapper for \ref N_Zcl_Framework_SendCommand to send a cluster specific request command.
    \param sourceEndpoint The source endpoint
    \param clusterId The cluster id
    \param commandId The command id
    \param manufacturerCode The manufacturer code
    \param options Combination of N_Zcl_Command_Option_... values
    \param zclPayloadLength The length of the payload
    \param pZclPayload The payload
    \param pDestinationAddress The destination address
    \returns Success status

    The options paramter can be used to set the "disable default response"
    field in the ZCL header.
    If the \ref N_ZCL_COMMAND_OPTION_DEFAULT_RESPONSE_ONLY_ON_ERROR option
    is supplied, the "disable default response" field is set and a
    default response will only be returned for an error. If the option
    is not supplied, a default response will also be returned in case of
    success.
    Note that a default response will never be sent if the command was
    send as broadcast or groupcast and that the option only influences
    default response commands, not specific responses.

    The sequence number that is put in the ZCL header of the request
    can be retreived by the application with with the
    \ref N_Zcl_Framework_GetSentSequenceNumber function.
*/
N_Zcl_SendStatus_t N_Zcl_Framework_SendClusterSpecificRequest(
    uint8_t sourceEndpoint,
    uint16_t clusterId,
    uint8_t commandId,
    uint16_t manufacturerCode,
    uint8_t options,
    uint8_t zclPayloadLength,
    uint8_t* pZclPayload,
    N_Address_t* pDestinationAddress);

/** Wrapper for \ref N_Zcl_Framework_SendCommand to send a cluster specific response command.
    \param zclPayloadLength The length of the payload
    \param pZclPayload The payload
    \returns Success status
    \note May only be called from within a received command callback function. Other parameters are deduced from the received command.
*/
N_Zcl_SendStatus_t N_Zcl_Framework_SendClusterSpecificResponse(
    uint8_t zclPayloadLength,
    uint8_t* pZclPayload);

/** Send a ZCL request.
    \param sourceEndpoint The source endpoint
    \param pRequest Pointer to the request structure
    \param pDestinationAddress The destination address
    \returns Success status

    The request structure must contain cluster id, ZCL header, ZCL payload and payload length.

    The sequence number that is put in the ZCL header of the request
    can be retreived by the application with with the
    \ref N_Zcl_Framework_GetSentSequenceNumber function.
*/
N_Zcl_SendStatus_t N_Zcl_Framework_SendRequest(
    uint8_t sourceEndpoint,
    N_Zcl_Control_t* pRequest,
    N_Address_t* pDestinationAddress);

/** Return a sequence number for use in a SendCommand call
    \returns The sequence number
*/
uint8_t N_Zcl_Framework_GetNextSequenceNumber(void);

/** Get the (ZCL) sequence number of the ZCL command just sent.
    \returns The sequence number

    \note Only valid just after sending a ZCL command, before any other ZCL
          command is sent (this includes e.g. default response commands).

    The application can use this function to get the sequence number of the
    ZCL request command it has just sent via a call to one of the N_Zcl
    cluster components.
    Together with \ref N_Zcl_Framework_GetReceivedSequenceNumber this can
    be used to match the request to the received response (e.g. a default
    response command).

    The sequence number should be enough to uniquely identify a response
    command, the application does not have to look at any other fields
    (e.g. source address, or response command id)
*/
uint8_t N_Zcl_Framework_GetSentSequenceNumber(void);

/***************************************************************************************************
* EXPORTED FUNCTIONS for RECEIVING ZCL MESSAGES
***************************************************************************************************/

/** Subscribe to all incoming ZCL messages of a certain cluster.
    \param clusterId The cluster id
    \param manufacturerCode The manufacturer code
    \param direction The direction (client-to-server or server-to-client)
    \param pCallback Pointer to a callback function that will be called for incoming messages
*/
void N_Zcl_Framework_RegisterCluster(uint16_t clusterId,
                                     uint16_t manufacturerCode,
                                     uint8_t direction,
                                     N_Zcl_Framework_ReceivedClusterCommand_t pCallback);

/** Subscribe to all incoming foundation commands of a certain type
    \param commandId The command id of the foundation messages to subscribe to.
    \param pCallback Pointer to a callback function that will be called for incoming commands
*/
void N_Zcl_Framework_RegisterFoundationCommand(uint8_t commandId,
                                               N_Zcl_Framework_ReceivedFoundationCommand_t pCallback);

/** Tell the framework that it should allow incoming messages for the endpoint.
    \param endpoint The endpoint which should be enabled.

    \note May be called multiple times for the same endpoint.
*/
void N_Zcl_Framework_EnableZclEndpoint(uint8_t endpoint);

/** Get the source address of the received ZCL message.
    \returns The source address

    \note Only use from a ZCL callback function, otherwise the result is undefined.
*/
N_Address_t* N_Zcl_Framework_GetSourceAddress(void);

/** Get the destination endpoint of the received ZCL message.
    \returns The destination endpoint

    \note Only use from a ZCL callback function, otherwise the result is undefined.
*/
uint8_t N_Zcl_Framework_GetDestinationEndpoint(void);

/** Get the (ZCL) sequence number of the received ZCL command.
    \returns The sequence number

    \note Only use from a ZCL callback function, otherwise the result is undefined.

    For an explanation of how to use the function, see
    \ref N_Zcl_Framework_GetReceivedSequenceNumber.
*/
uint8_t N_Zcl_Framework_GetReceivedSequenceNumber(void);

/** Whether the direction bit from the last ZCL received message is Client-Side.
    \returns TRUE when last received message is Client-Side

    \note Only use from a ZCL callback function, otherwise the result is undefined.
*/
bool N_Zcl_Framework_IsReceivedDirectionClientSide(void);

/** Whether the incoming message was received as a unicast.
    \returns TRUE when the message was unicast, FALSE when it was either broadcast or groupcast.
*/
bool N_Zcl_Framework_ReceivedAsUnicast(void);

/** Utility function for cluster components and foundation command components.
    \param endpoint The endpoint to register a subscriber for
    \param pCallback The callback function pointer to subscribe
    \param pSubscribers Pointer to an array to put the subscription in
    \param maxSubscribers Array size of pSubscribers
    \note This function is used by server-side clients of N_Zcl_Framework to offload a part of their administration.
    \note This function does an implicit N_Zcl_Framework_EnableZclEndpoint().
*/
void N_Zcl_Framework_AddSubscriber(uint8_t endpoint,
                                   const void* pCallback,
                                   N_Zcl_Framework_Subscriber_t* pSubscribers,
                                   uint8_t maxSubscribers);

/** Utility function for cluster components and foundation command components.
    \param endpoint The endpoint to register a subscriber for
    \param pSubscribers Pointer to an array to put the subscription in
    \param maxSubscribers Array size of pSubscribers
    \returns The registered callback function pointer, or NULL.
    \note This function is used by server-side clients of N_Zcl_Framework to offload a part of their administration.
*/
const void* N_Zcl_Framework_FindSubscriber(uint8_t endpoint,
                                           const N_Zcl_Framework_Subscriber_t* pSubscribers,
                                           uint8_t maxSubscribers);

/** Subscribe to N_Zcl_Framework callbacks.
*/
void N_Zcl_Framework_Subscribe(const N_Zcl_Framework_Callback_t* pSubscriber);

/** Returns the next transaction sequence number that will be used in the AF layer
    when sending a ZCL command.
    \returns The next transaction sequence number.
*/
uint8_t N_Zcl_Framework_GetNextTransactionSequenceNumber(void);

/***************************************************************************************************
* END OF C++ DECLARATION WRAPPER
***************************************************************************************************/

#ifdef __cplusplus
}
#endif

#endif // N_ZCL_FRAMEWORK_H
