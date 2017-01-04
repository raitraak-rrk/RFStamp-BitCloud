/*********************************************************************************************//**
\file

\brief

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal

$Id: N_Zcl_Framework.c 24615 2013-02-27 05:14:24Z arazinkov $

***************************************************************************************************/

/***************************************************************************************************
* LOCAL INCLUDE FILES
***************************************************************************************************/

#include "N_Zcl_Framework_Bindings.h"
#include "N_Zcl_Framework.h"

/***************************************************************************************************
* EXTERNAL INCLUDE FILES
***************************************************************************************************/
#include "sysUtils.h"
#include "nwkCommon.h"

#include "N_ErrH.h"
#include "N_Log.h"
#include "N_Memory.h"
#include "N_PacketDistributor.h"
#include "N_Types.h"
#include "N_Util.h"
#include "N_Cmi.h"
#include "N_Binding_Bindings.h"
#include "N_Binding.h"
#include "N_Zcl_Framework.h"
#include "N_DeviceInfo_Bindings.h"
#include "N_DeviceInfo.h"

#include "N_Zcl_DataRequest.h"
#include "N_Binding.h"

/***************************************************************************************************
* LOCAL MACROS AND CONSTANTS
***************************************************************************************************/

#define COMPID "N_Zcl_Framework"

/** Maximum number of end-points that can have ZCL enabled */
#if !defined(N_ZCL_FRAMEWORK_MAX_ENDPOINTS)
#define N_ZCL_FRAMEWORK_MAX_ENDPOINTS 6u
#endif

/** Maximum number of foundation commands that can be registered */
#if !defined(N_ZCL_FRAMEWORK_MAX_FOUNDATION_COMMANDS)
#  define N_ZCL_FRAMEWORK_MAX_FOUNDATION_COMMANDS 9u
#endif

/** Maximum number of clusters that can be registered */
#if !defined(N_ZCL_FRAMEWORK_MAX_CLUSTERS)
#  define N_ZCL_FRAMEWORK_MAX_CLUSTERS 12u
#endif

#ifndef N_ZCL_FRAMEWORK_MAX_SUBSCRIBERS
  #define N_ZCL_FRAMEWORK_MAX_SUBSCRIBERS 1u
#endif

#define N_ZCL_DEFAULT_RADIUS 0u

/***************************************************************************************************
* LOCAL TYPES
***************************************************************************************************/

typedef struct FoundationCommandCallback_t
{
    uint8_t commandId;
    N_Zcl_Framework_ReceivedFoundationCommand_t pCallback;
} FoundationCommandCallback_t;

typedef struct ClusterCallback_t
{
    uint16_t clusterId;
    uint16_t manufacturerCode;
    uint8_t direction;
    N_Zcl_Framework_ReceivedClusterCommand_t pCallback;
} ClusterCallback_t;

BEGIN_PACK
typedef struct PACK _ZclMessageFrame_t
{
  // Auxilliary header (stack required)
  uint8_t *header;
  // Actually ZCL message
  uint8_t *msg;
  // Auxilliary footer (stack required)
  uint8_t *footer;
} ZclMessageFrame_t;
END_PACK

typedef struct _ZclApsBuffer_t
{
  ZclMessageFrame_t frame;
  APS_DataReq_t     dataReq;
  bool              busy;
} ZclApsBuffer_t;

/***************************************************************************************************
* LOCAL VARIABLES
***************************************************************************************************/

/** Registration of generic callback */
N_UTIL_CALLBACK_DECLARE(N_Zcl_Framework_Callback_t, s_subscribers, N_ZCL_FRAMEWORK_MAX_SUBSCRIBERS);

/** Registration of which end-points have already been enabled. */
static uint8_t s_enabledEndpoints[N_ZCL_FRAMEWORK_MAX_ENDPOINTS] = { 0u };

/** Registration of foundation commands. */
static FoundationCommandCallback_t s_registeredFoundationCommands[N_ZCL_FRAMEWORK_MAX_FOUNDATION_COMMANDS] = { {0u, NULL} };

/** Registration of clusters. */
static ClusterCallback_t s_registeredClusters[N_ZCL_FRAMEWORK_MAX_CLUSTERS] = { {0u, 0u, 0u, NULL} };

/** ZCL layer sequence number.

    Used by \ref N_Zcl_Framework_GetNextSequenceNumber.
*/
static uint8_t s_nextSequenceNumber = 0u;

/** The source address of the incoming command being processed.

    Used by \ref N_Zcl_Framework_GetSourceAddress.
*/
static N_Address_t* s_pSourceAddress;

/** The destination endpoint of the incoming command being processed.

    Used by \ref N_Zcl_Framework_GetDestinationEndpoint.
*/
static uint8_t s_destinationEndpoint;

/** The transaction sequence number of the incoming command being processed.

    Used by \ref N_Zcl_Framework_GetReceivedSequenceNumber.
*/
static uint8_t s_receivedSequenceNumber = 0u;

/** Whether the current incoming command was received as unicast.

    Used by \ref N_Zcl_Framework_ReceivedAsUnicast.
*/
static bool s_receivedAsUnicast = FALSE;

static uint16_t s_receivedClusterId;
static uint8_t s_receivedCommandId;
static uint16_t s_receivedManufacturerCode;

/** The transaction sequence number of last command that has been sent.

    Used by \ref N_Zcl_Framework_GetSentSequenceNumber.
*/
static uint8_t s_sentSequenceNumber = 0u;

static ZclApsBuffer_t zclApsBuffers[N_ZCL_APS_BUFFERS_AMOUNT];

/** Direction of the incoming packet being processed.

    Used by \ref N_Zcl_Framework_IsReceivedDirectionClientSide
*/
static bool s_receivedDirectionIsClientSide;

/***************************************************************************************************
* LOCAL FUNCTION DECLARATIONS
***************************************************************************************************/
static ZclApsBuffer_t* zclGetApsBuffer(size_t payloadLen);
static void zclApsDataConf(APS_DataConf_t* conf);
static void zclBcToPlatformAddressingConvert(N_Address_t *addressing, APS_DataInd_t *dataInd);
static void ProcessZclDataInd(APS_DataInd_t *dataInd);

/***************************************************************************************************
* LOCAL FUNCTIONS
***************************************************************************************************/

/***************************************************************************************************
Func: 
    zclGetApsBuffer - This function will utilize the zclApsBuffers if its available and allocate memory for frame, based on the 
        payloadLen parameter.
    The caller has to ensure, that the payload length is not more than "APS_MAX_TX_ASDU_SIZE" in case of fragmentation
    is not supported in APS. If the payload length is more than "APS_MAX_TX_ASDU_SIZE" and APS fragmentation is 
    supported then TX_Option should be set appropriately.
    
Params: 
    uint16_t payloadLen - Length of ASDU

Return: ZclApsBuffer_t*

***************************************************************************************************/
static ZclApsBuffer_t* zclGetApsBuffer(size_t payloadLen)
{
  ZclApsBuffer_t *apsBuffer;
  size_t bufLen;


  bufLen = APS_ASDU_OFFSET + payloadLen + (APS_AFFIX_LENGTH - APS_ASDU_OFFSET);  //Frame overhead + payload + footer
  
  for (uint8_t buffer = 0; buffer < N_ZCL_APS_BUFFERS_AMOUNT; buffer++)
  {
    apsBuffer = &zclApsBuffers[buffer];

    if (!apsBuffer->busy)
    {
      apsBuffer->frame.header = (uint8_t *) N_Memory_Alloc(bufLen);
      if (!apsBuffer->frame.header)
      {
        return(NULL);
      }
      apsBuffer->busy = true;
      apsBuffer->frame.msg = (uint8_t *)((uint8_t *)apsBuffer->frame.header + APS_ASDU_OFFSET); 
      apsBuffer->frame.footer = (uint8_t *)((uint8_t *)apsBuffer->frame.msg + payloadLen);

      apsBuffer->dataReq.asdu = apsBuffer->frame.msg;
      return apsBuffer;
    }
  }

  return NULL;
}

static void zclApsDataConf(APS_DataConf_t* conf)
{
  uint8_t transactionSeqNr;
  ZclApsBuffer_t *apsBuffer = GET_PARENT_BY_FIELD(ZclApsBuffer_t, dataReq.confirm, conf);

  N_Memory_Free((void*)apsBuffer->frame.header);
  apsBuffer->busy = false;
  transactionSeqNr = N_Zcl_Framework_GetSentSequenceNumber();
  N_UTIL_CALLBACK(N_Zcl_Framework_Callback_t, s_subscribers, DataConfirmation,
      (apsBuffer->dataReq.dstEndpoint, transactionSeqNr, conf->status));
}

/** Calculate the ZCL Frame Header size
    \param pZclFrameHeader Pointer to a filled frame header
    \returns TThe size of the header, taking into account the manufacturer specific bit
*/
static uint8_t CalculateZclFrameHeaderSize(const N_Zcl_Framework_Hdr_t* pZclFrameHeader)
{
    // frame control + sequence number + command id
    uint8_t size = 1u + 1u + 1u;

    // add the manfacturer code if the manufacturer specific bit is set
    if ( pZclFrameHeader->frameControl.manufacturerSpecific )
    {
        size += 2u;
    }

    return size;
}

/** Put the ZCL header into a transmit buffer
    \param pAfPayload Pointer to the transmit buffer
    \param pZclFrameHeader Pointer to a filled frame header
*/
static void FormatZclFrameHeader(uint8_t* pAfPayload, const N_Zcl_Framework_Hdr_t* pZclFrameHeader)
{
    // build the frame control field
    pAfPayload[0]  = pZclFrameHeader->frameControl.type;
    pAfPayload[0] |= pZclFrameHeader->frameControl.manufacturerSpecific << 2;
    pAfPayload[0] |= pZclFrameHeader->frameControl.direction << 3;
    pAfPayload[0] |= pZclFrameHeader->frameControl.disableDefaultResponse << 4;
    uint8_t index = 1u;

    // add the manfacturer code
    if ( pZclFrameHeader->frameControl.manufacturerSpecific )
    {
        pAfPayload[1] = N_Util_LowByteUint16(pZclFrameHeader->manufacturerCode);
        pAfPayload[2] = N_Util_HighByteUint16(pZclFrameHeader->manufacturerCode);
        index += 2u;
    }

    // add the sequence number
    pAfPayload[index] = pZclFrameHeader->transactionSequenceNumber;
    index++;

    // add the command id
    pAfPayload[index] = pZclFrameHeader->commandId;
}

/** Parse the received ZCL header
    \param pZclFrameHeader Pointer to a frame header structure that will be filled by this function
    \param afPayloadLength Length of the receive buffer
    \param pAfPayload Pointer to the receive buffer
    \returns The frame header length if the header was parsed successfully, 0 otherwise
*/
static uint8_t ParseZclFrameHeader(N_Zcl_Framework_Hdr_t* pZclFrameHeader, uint8_t afPayloadLength, const uint8_t* pAfPayload)
{
    memset(pZclFrameHeader, 0, sizeof(*pZclFrameHeader));

    // return error if the payload is too small for the frame control field,
    // sequence number and command id
    if ( afPayloadLength < 3u )
    {
        return 0u;
    }

    // build the frame control field
    pZclFrameHeader->frameControl.type = pAfPayload[0] & 0x03u;
    pZclFrameHeader->frameControl.manufacturerSpecific = (pAfPayload[0] & 0x04u) ? 1u : 0u;
    pZclFrameHeader->frameControl.direction = (pAfPayload[0] & 0x08u) ? 1u : 0u;
    pZclFrameHeader->frameControl.disableDefaultResponse = (pAfPayload[0] & 0x10u) ? 1u : 0u;
    uint8_t index = 1u;

    // parse the manfacturer code
    if ( pZclFrameHeader->frameControl.manufacturerSpecific )
    {
        // return error if the payload is too small for the extra manufacturer code
        if ( afPayloadLength < 5u )
        {
            return 0u;
        }

        pZclFrameHeader->manufacturerCode = N_Util_BuildUint16(pAfPayload[1], pAfPayload[2] );
        index += 2u;
    }

    // parse the sequence number
    pZclFrameHeader->transactionSequenceNumber = pAfPayload[index];
    index++;

    // parse the command id
    pZclFrameHeader->commandId = pAfPayload[index];
    index++;

    // return the length of the ZCL Frame Header
    return index;
}

static N_Zcl_Framework_ReceivedFoundationCommand_t FindFoundationCommandCallback(uint8_t commandId)
{
    N_Zcl_Framework_ReceivedFoundationCommand_t result = NULL;

    for ( uint8_t i = 0u; i < N_ZCL_FRAMEWORK_MAX_FOUNDATION_COMMANDS; i++ )
    {
        if ( s_registeredFoundationCommands[i].commandId == commandId )
        {
            result = s_registeredFoundationCommands[i].pCallback;
            break;
        }
        if ( s_registeredFoundationCommands[i].pCallback == NULL )
        {
            // end of list
            break;
        }
    }

    return result;
}

static N_Zcl_Framework_ReceivedClusterCommand_t FindClusterCallback(uint16_t clusterId, uint16_t manufacturerCode, uint8_t direction)
{
    N_Zcl_Framework_ReceivedClusterCommand_t result = NULL;

    for ( uint8_t i = 0u; i < N_ZCL_FRAMEWORK_MAX_CLUSTERS; i++ )
    {
        if (( s_registeredClusters[i].clusterId == clusterId ) &&
            ( s_registeredClusters[i].manufacturerCode == manufacturerCode ) &&
            ( s_registeredClusters[i].direction == direction ))
        {
            result = s_registeredClusters[i].pCallback;
            break;
        }
        if ( s_registeredClusters[i].pCallback == NULL )
        {
            // end of list
            break;
        }
    }

    return result;
}

static void zclBcToPlatformAddressingConvert(N_Address_t *addressing, APS_DataInd_t *dataInd)
{
  addressing->panId = 0;
  addressing->endPoint = dataInd->srcEndpoint;
  addressing->addrMode = N_Address_Mode_Short;
  addressing->address.shortAddress = dataInd->srcAddress.shortAddress;
}

/** Packet Distributor to ZCL data indication handler
 *  \param dataInd Pointer to data indication parameters */
static void ProcessZclDataInd(APS_DataInd_t *dataInd)
{
    N_Zcl_Status_t status;

    if ( dataInd->clusterId == N_ZCL_FRAMEWORK_INVALID_CLUSTER_ID )
    {
        // ignore packets with an invalid cluster id
        return;
    }

    ZLL_Endpoint_t* pEndpointDescription = N_PacketDistributor_FindEndpoint(dataInd->dstEndpoint);
    if ( pEndpointDescription == NULL )
    {
        // ignore packets for unknown destination end-point
        return;
    }

    N_Zcl_Framework_Incoming_t zclIncoming;
    zclIncoming.clusterId = dataInd->clusterId;
    zclIncoming.pData = NULL;
    zclIncoming.pDataLen = 0u;

    uint8_t zclFrameHeaderLength = ParseZclFrameHeader(&(zclIncoming.hdr),
                                                     (uint8_t)(dataInd->asduLength),
                                                     dataInd->asdu);
    if ( zclFrameHeaderLength == 0u )
    {
        // ignore packets with an invalid frame header
        return;
    }

    // store information for use by GetSourceAddress, GetDestinationEndpoint, GetReceivedSequenceNumber,ReceivedAsUnicast
    N_Address_t srcAddress;

    zclBcToPlatformAddressingConvert(&srcAddress, dataInd);
    s_pSourceAddress = (N_Address_t*)&srcAddress;

    s_destinationEndpoint = dataInd->dstEndpoint;
    s_receivedSequenceNumber = zclIncoming.hdr.transactionSequenceNumber;
    s_receivedAsUnicast = N_UTIL_BOOL(!(APS_SHORT_ADDRESS == dataInd->dstAddrMode && IS_BROADCAST_ADDR(dataInd->dstAddress.shortAddress)) &&
                                      !(APS_GROUP_ADDRESS == dataInd->dstAddrMode));
    s_receivedClusterId = zclIncoming.clusterId;
    s_receivedCommandId = zclIncoming.hdr.commandId;
    s_receivedManufacturerCode = zclIncoming.hdr.manufacturerCode;
    s_receivedDirectionIsClientSide = N_UTIL_BOOL(zclIncoming.hdr.frameControl.direction);

    // if the received ZCL command is manufacturer specific, it must be equal to the manufacturer
    // code. If not, a default response with status unsupported command (depending on the frametype) must be sent.
    if ( (zclIncoming.hdr.frameControl.manufacturerSpecific) && ( zclIncoming.hdr.manufacturerCode != N_ZCL_MANUFACTURER_CODE ) )
    {
        if ( zclIncoming.hdr.commandId == N_ZCL_CMD_DEFAULT_RSP )
        {
            // never send a default response when handling a default response
            return;
        }
        else
        {
            if ( zclIncoming.hdr.frameControl.type == N_ZCL_FRAMEWORK_FRAME_TYPE_PROFILE_CMD )
            {
                status = N_ZCL_STATUS_UNSUP_MANU_GENERAL_COMMAND;
            }
            else
            {
                status = N_ZCL_STATUS_UNSUP_MANU_CLUSTER_COMMAND;
            }

            if ( s_receivedAsUnicast )
            {
                // Construct a default response command
                uint8_t zclPayload[2];
                zclPayload[0] = zclIncoming.hdr.commandId;
                zclPayload[1] = (uint8_t)status;

                N_Zcl_Framework_Hdr_t zclFrameHeader;
                memset(&zclFrameHeader, 0, sizeof(zclFrameHeader));
                zclFrameHeader.frameControl.type = N_ZCL_FRAMEWORK_FRAME_TYPE_PROFILE_CMD;
                zclFrameHeader.frameControl.direction = N_ZCL_FRAMEWORK_SERVER_CLIENT_DIR;
                zclFrameHeader.frameControl.disableDefaultResponse = TRUE;
                zclFrameHeader.frameControl.manufacturerSpecific = 1u;
                zclFrameHeader.manufacturerCode = zclIncoming.hdr.manufacturerCode;
                zclFrameHeader.transactionSequenceNumber = zclIncoming.hdr.transactionSequenceNumber;
                zclFrameHeader.commandId = N_ZCL_CMD_DEFAULT_RSP;

                // store the sequence number for later use by the N_Zcl_Framework_GetSentSequenceNumber function
                s_sentSequenceNumber = zclIncoming.hdr.transactionSequenceNumber;

                uint8_t zclResponseFrameHeaderLength = CalculateZclFrameHeaderSize(&zclFrameHeader);
                uint8_t afPayloadLength = zclResponseFrameHeaderLength + (uint8_t)sizeof(zclPayload);

                if (afPayloadLength > APS_MAX_TX_ASDU_SIZE)
                {
                  return;
                }

                ZclApsBuffer_t *apsBuffer = zclGetApsBuffer((size_t)afPayloadLength);
                if ((apsBuffer == NULL) || (apsBuffer->frame.header == NULL))
                {
                    return;
                }

                FormatZclFrameHeader(apsBuffer->frame.msg, &zclFrameHeader);
                (void) memcpy(apsBuffer->frame.msg + zclResponseFrameHeaderLength, zclPayload, sizeof(zclPayload));

                apsBuffer->dataReq.dstAddrMode = dataInd->srcAddrMode;
                apsBuffer->dataReq.dstAddress = dataInd->srcAddress;
                apsBuffer->dataReq.dstEndpoint = dataInd->srcEndpoint;
                apsBuffer->dataReq.profileId = dataInd->profileId;
                apsBuffer->dataReq.clusterId = dataInd->clusterId;
                apsBuffer->dataReq.srcEndpoint = pEndpointDescription->simpleDescriptor->endpoint;
                apsBuffer->dataReq.asduLength = afPayloadLength;
                apsBuffer->dataReq.radius = N_ZCL_DEFAULT_RADIUS;
                apsBuffer->dataReq.APS_DataConf = zclApsDataConf;

                N_Zcl_DataRequest(&apsBuffer->dataReq);
            }

            return;
        }
    }
    else
    {
        zclIncoming.pData = dataInd->asdu + zclFrameHeaderLength;
        zclIncoming.pDataLen = (uint8_t)(dataInd->asduLength - zclFrameHeaderLength);

        if ( zclIncoming.hdr.frameControl.type == N_ZCL_FRAMEWORK_FRAME_TYPE_PROFILE_CMD )
        {
            // foundation command. find the registered foundation command
            N_Zcl_Framework_ReceivedFoundationCommand_t foundationCommandCallback;
            foundationCommandCallback = FindFoundationCommandCallback(zclIncoming.hdr.commandId);
            if ( foundationCommandCallback != NULL )
            {
                status = foundationCommandCallback(&zclIncoming);
            }
            else
            {
                if (zclIncoming.hdr.commandId == N_ZCL_CMD_DEFAULT_RSP)
                {
                    // never send a default response when handling a default response
                    return;
                }
                // unknown command. different status code for manufacturer specific
                if ( zclIncoming.hdr.frameControl.manufacturerSpecific )
                {
                    status = N_ZCL_STATUS_UNSUP_MANU_GENERAL_COMMAND;
                }
                else
                {
                    status = N_ZCL_STATUS_UNSUP_GENERAL_COMMAND;
                }
            }
        }
        else
        {
            // not a foundation command, must be a cluster specific command
            N_Zcl_Framework_ReceivedClusterCommand_t clusterCommandCallback;
            clusterCommandCallback = FindClusterCallback(dataInd->clusterId, zclIncoming.hdr.manufacturerCode, zclIncoming.hdr.frameControl.direction);
            if ( clusterCommandCallback != NULL )
            {
                // The return value of the plugin function will be
                //  \ref N_ZCL_STATUS_SUCCESS - Supported and need default response
                //  \ref N_ZCL_STATUS_FAILURE - Unsupported
                //  \ref N_ZCL_STATUS_CMD_HAS_RSP - Supported and do not need default rsp
                //  \ref N_ZCL_STATUS_INVALID_FIELD - Supported, but the incoming msg is wrong formatted
                //  \ref N_ZCL_STATUS_INVALID_VALUE - Supported, but the request not achievable by the h/w
                //  \ref N_ZCL_STATUS_SOFTWARE_FAILURE - Supported but memory allocation fails
                status = clusterCommandCallback(&zclIncoming);
            }
            else
            {
                // unknown command. different status code for manufacturer specific
                if ( zclIncoming.hdr.frameControl.manufacturerSpecific )
                {
                    status = N_ZCL_STATUS_UNSUP_MANU_CLUSTER_COMMAND;
                }
                else
                {
                    status = N_ZCL_STATUS_UNSUP_CLUSTER_COMMAND;
                }
            }
        }
    }

    N_UTIL_CALLBACK(N_Zcl_Framework_Callback_t, s_subscribers, MessageReceived, ());

    if ( s_receivedAsUnicast &&
         ( ( !zclIncoming.hdr.frameControl.disableDefaultResponse ) || ( status != N_ZCL_STATUS_SUCCESS ) ) &&
         ( status != N_ZCL_STATUS_CMD_HAS_RSP) )
    {
        // send a default response command
        uint8_t buffer[2];
        buffer[0] = zclIncoming.hdr.commandId;
        buffer[1] = (uint8_t)status;
        (void) N_Zcl_Framework_SendCommand(s_destinationEndpoint,
                                           dataInd->clusterId,
                                           N_ZCL_CMD_DEFAULT_RSP,
                                           N_ZCL_FRAMEWORK_FRAME_TYPE_PROFILE_CMD,
                                           N_ZCL_FRAMEWORK_SERVER_CLIENT_DIR,
                                           TRUE,
                                           zclIncoming.hdr.manufacturerCode,
                                           zclIncoming.hdr.transactionSequenceNumber,
                                           (uint8_t)(sizeof(buffer)),
                                           buffer,
                                           s_pSourceAddress);
    }

    // clear source address now the packet has been processed
    s_pSourceAddress = NULL;
    // clear destination endpoint now the packet has been processed
    s_destinationEndpoint = 0u;
}

/***************************************************************************************************
* EXPORTED FUNCTIONS
***************************************************************************************************/

/** Interface function, see \ref N_Zcl_Framework_Subscribe. */
void N_Zcl_Framework_Subscribe_Impl(const N_Zcl_Framework_Callback_t* pSubscriber)
{
    N_UTIL_CALLBACK_SUBSCRIBE(N_Zcl_Framework_Callback_t, s_subscribers, pSubscriber);
}

/** Interface function, see \ref N_Zcl_Framework_GetNextTransactionSequenceNumber. */
uint8_t N_Zcl_Framework_GetNextTransactionSequenceNumber_Impl(void)
{
    return s_nextSequenceNumber;
}

/** Interface function, see \ref N_Zcl_Framework_RegisterFoundationCommand. */
void N_Zcl_Framework_RegisterFoundationCommand_Impl(uint8_t commandId,
                                                    const N_Zcl_Framework_ReceivedFoundationCommand_t pCallback)
{
    N_ERRH_ASSERT_FATAL(pCallback != NULL);

    for ( uint8_t i = 0u; i < N_ZCL_FRAMEWORK_MAX_FOUNDATION_COMMANDS; i++ )
    {
        if ( s_registeredFoundationCommands[i].pCallback == NULL )
        {
            // add registration
            s_registeredFoundationCommands[i].commandId = commandId;
            s_registeredFoundationCommands[i].pCallback = pCallback;
            return;
        }
        else
        {
            // only one registration per command allowed
            N_ERRH_ASSERT_FATAL(commandId != s_registeredFoundationCommands[i].commandId);
        }
    }

    // maximum number of command registrations reached
    N_ERRH_FATAL();
}

/** Interface function, see \ref N_Zcl_Framework_RegisterCluster. */
void N_Zcl_Framework_RegisterCluster_Impl(uint16_t clusterId,
                                          uint16_t manufacturerCode,
                                          uint8_t direction,
                                          N_Zcl_Framework_ReceivedClusterCommand_t pCallback)
{
    N_ERRH_ASSERT_FATAL(pCallback != NULL);

    for ( uint8_t i = 0u; i < N_ZCL_FRAMEWORK_MAX_CLUSTERS; i++ )
    {
        if ( s_registeredClusters[i].pCallback == NULL )
        {
            // add registration
            s_registeredClusters[i].clusterId = clusterId;
            s_registeredClusters[i].manufacturerCode = manufacturerCode;
            s_registeredClusters[i].direction = direction;
            s_registeredClusters[i].pCallback = pCallback;
            return;
        }
        else
        {
            // only one registration per cluster allowed
            N_ERRH_ASSERT_FATAL(
                (s_registeredClusters[i].clusterId != clusterId) ||
                (s_registeredClusters[i].manufacturerCode != manufacturerCode) ||
                (s_registeredClusters[i].direction != direction));
        }
    }

    // maximum number of command registrations reached
    N_ERRH_FATAL();
}

/** Interface function, see \ref N_Zcl_Framework_EnableZclEndpoint. */
void N_Zcl_Framework_EnableZclEndpoint_Impl(uint8_t endpoint)
{
    // end-point 0 not supported
    N_ERRH_ASSERT_FATAL(endpoint != 0u);

    static const N_Zcl_DataInd_Callback_t s_N_Zcl_DataInd_Callback =
    {
        ProcessZclDataInd,
        -1
    };
    for ( uint8_t i = 0u; i < N_ZCL_FRAMEWORK_MAX_ENDPOINTS; i++ )
    {
        if ( s_enabledEndpoints[i] == endpoint )
        {
            // end-point already enabled
            return;
        }
        if ( s_enabledEndpoints[i] == 0u )
        {
            // enable ZCL for end-point by subscribing to incoming packets
            N_Zcl_DataInd_Subscribe(endpoint, &s_N_Zcl_DataInd_Callback);
            s_enabledEndpoints[i] = endpoint;
            return;
        }
    }

    // maximum number of enabled end-point already reached
    N_ERRH_FATAL();
}

/** Interface function, see \ref N_Zcl_Framework_AddSubscriber. */
void N_Zcl_Framework_AddSubscriber_Impl(uint8_t endpoint,
                                        const void* pCallback,
                                        N_Zcl_Framework_Subscriber_t* pSubscribers,
                                        uint8_t maxSubscribers)
{
    N_ERRH_ASSERT_FATAL(pCallback != NULL);

    // AddSubscriber is an implicit EnableZclForEndpoint
    N_Zcl_Framework_EnableZclEndpoint(endpoint);

    for ( uint8_t i = 0u; i < maxSubscribers; i++ )
    {
        if ( pSubscribers[i].pCallback == NULL )
        {
            // add subscription
            pSubscribers[i].endpoint = endpoint;
            pSubscribers[i].pCallback = pCallback;
            return;
        }
        else
        {
            // only one subscription per end-point allowed
            N_ERRH_ASSERT_FATAL(endpoint != pSubscribers[i].endpoint);
        }
    }

    // maximum number of subscribers reached
    N_ERRH_FATAL();
}

/** Interface function, see \ref N_Zcl_Framework_FindSubscriber. */
const void* N_Zcl_Framework_FindSubscriber_Impl(uint8_t endpoint,
                                                const N_Zcl_Framework_Subscriber_t* pSubscribers,
                                                uint8_t maxSubscribers)
{
    const void* result = NULL;

    for ( uint8_t i = 0u; i < maxSubscribers; i++ )
    {
        if ( pSubscribers[i].endpoint == endpoint )
        {
            result = pSubscribers[i].pCallback;
            break;
        }
        if ( pSubscribers[i].pCallback == NULL )
        {
            // end of list
            break;
        }
    }

    return result;
}

/** Interface function, see \ref N_Zcl_Framework_GetNextSequenceNumber. */
uint8_t N_Zcl_Framework_GetNextSequenceNumber_Impl(void)
{
    return s_nextSequenceNumber++;
}

/** Interface function, see \ref N_Zcl_Framework_SendCommand. */
N_Zcl_SendStatus_t N_Zcl_Framework_SendCommand_Impl(uint8_t sourceEndpoint,
                                           uint16_t clusterId,
                                           uint8_t commandId,
                                           uint8_t frameType,
                                           uint8_t direction,
                                           uint8_t disableDefaultResponse,
                                           uint16_t manufacturerCode,
                                           uint8_t sequenceNumber,
                                           uint8_t zclPayloadLength,
                                           const uint8_t* pZclPayload,
                                           N_Address_t* pDestinationAddress)
{
    uint8_t bindingRecordIndex;
    N_Binding_Record_t bindingRecord;
    N_ERRH_ASSERT_FATAL(sourceEndpoint != 0u); // Do not send from ZDP enpdoint
    ZLL_Endpoint_t* pEndpointDescription = N_PacketDistributor_FindEndpoint(sourceEndpoint);
    if ( pEndpointDescription == NULL )
    {
        return N_Zcl_SendStatus_Failure;
    }

    if ( clusterId == N_ZCL_FRAMEWORK_INVALID_CLUSTER_ID )
    {
        return N_Zcl_SendStatus_Failure;
    }

    if (N_Address_Mode_Binding == pDestinationAddress->addrMode)
    {
      bindingRecordIndex = N_Binding_FindBinding(sourceEndpoint, clusterId, 0u);
      if (INVALID_BINDING_RECORD_INDEX == bindingRecordIndex)
      {
          return N_Zcl_SendStatus_Failure;
      }
      N_Binding_GetBindingRecord(bindingRecordIndex,&bindingRecord);
      pDestinationAddress = &bindingRecord.destinationAddress;
    }

    N_Zcl_Framework_Hdr_t zclFrameHeader;
    memset(&zclFrameHeader, 0, sizeof(zclFrameHeader));
    zclFrameHeader.frameControl.type = frameType;
    zclFrameHeader.frameControl.direction = direction;
    zclFrameHeader.frameControl.disableDefaultResponse = disableDefaultResponse;
    if ( manufacturerCode != N_ZCL_MANUFACTURER_CODE_NONE )
    {
        zclFrameHeader.frameControl.manufacturerSpecific = 1u;
        zclFrameHeader.manufacturerCode = manufacturerCode;
    }
    zclFrameHeader.transactionSequenceNumber = sequenceNumber;
    zclFrameHeader.commandId = commandId;

    // store the sequence number for later use by the N_Zcl_Framework_GetSentSequenceNumber function
    s_sentSequenceNumber = sequenceNumber;

    uint8_t zclFrameHeaderLength = CalculateZclFrameHeaderSize(&zclFrameHeader);
    uint8_t afPayloadLength = zclFrameHeaderLength + zclPayloadLength;

    if (afPayloadLength > APS_MAX_TX_ASDU_SIZE)
    {
      return(N_Zcl_SendStatus_Failure);
    }

    ZclApsBuffer_t *apsBuffer = zclGetApsBuffer((size_t)afPayloadLength);
    if ((apsBuffer == NULL) || (apsBuffer->frame.header == NULL))
    {
        return N_Zcl_SendStatus_OutOfMemory;
    }

    FormatZclFrameHeader(apsBuffer->frame.msg, &zclFrameHeader);

    if ((pZclPayload != NULL) && (zclPayloadLength != 0u))
    {
        (void) memcpy(apsBuffer->frame.msg + zclFrameHeaderLength, pZclPayload, (size_t)zclPayloadLength);
    }

    apsBuffer->dataReq.dstAddrMode = N_Cmi_PlatformToBcAddressModeConvert(pDestinationAddress->addrMode);
    N_Cmi_PlatformToBcAddressingConvert(&apsBuffer->dataReq.dstAddress, pDestinationAddress);
    apsBuffer->dataReq.dstEndpoint = pDestinationAddress->endPoint;
    if (pEndpointDescription->simpleDescriptor->AppProfileId == PROFILE_ID_LIGHT_LINK)
    {
        apsBuffer->dataReq.profileId = PROFILE_ID_HOME_AUTOMATION;
    }
    else
    {
        apsBuffer->dataReq.profileId = pEndpointDescription->simpleDescriptor->AppProfileId;
    }
    apsBuffer->dataReq.clusterId = clusterId;
    apsBuffer->dataReq.srcEndpoint = sourceEndpoint;
    apsBuffer->dataReq.asduLength = afPayloadLength;

    apsBuffer->dataReq.radius = N_ZCL_DEFAULT_RADIUS;
    apsBuffer->dataReq.APS_DataConf = zclApsDataConf;

    N_Zcl_DataRequest(&apsBuffer->dataReq);

    return N_Zcl_SendStatus_Ok;
}

/** Interface function, see \ref N_Zcl_Framework_SendClusterSpecificRequest */
N_Zcl_SendStatus_t N_Zcl_Framework_SendClusterSpecificRequest_Impl(
    uint8_t sourceEndpoint,
    uint16_t clusterId,
    uint8_t commandId,
    uint16_t manufacturerCode,
    uint8_t options,
    uint8_t zclPayloadLength,
    uint8_t* pZclPayload,
    N_Address_t* pDestinationAddress)
{
    return N_Zcl_Framework_SendCommand(
        sourceEndpoint,
        clusterId,
        commandId,
        N_ZCL_FRAMEWORK_FRAME_TYPE_SPECIFIC_CMD,
        N_ZCL_FRAMEWORK_CLIENT_SERVER_DIR,
        (options & N_ZCL_COMMAND_OPTION_DEFAULT_RESPONSE_ONLY_ON_ERROR) ? TRUE : FALSE, // disableDefaultResponse
        manufacturerCode,
        N_Zcl_Framework_GetNextSequenceNumber(),
        zclPayloadLength,
        pZclPayload,
        pDestinationAddress);
}

/** Interface function, see \ref N_Zcl_Framework_SendRequest. */
N_Zcl_SendStatus_t N_Zcl_Framework_SendRequest_Impl(
    uint8_t sourceEndpoint,
    N_Zcl_Control_t* pRequest,
    N_Address_t* pDestinationAddress)
{
    N_ERRH_ASSERT_FATAL(sourceEndpoint != 0u); // Do not send from ZDP enpdoint
    ZLL_Endpoint_t *pEndpointDescription = N_PacketDistributor_FindEndpoint(sourceEndpoint);
    if ( pEndpointDescription == NULL )
    {
        return N_Zcl_SendStatus_Failure;
    }

    if ( pRequest->clusterId == N_ZCL_FRAMEWORK_INVALID_CLUSTER_ID )
    {
        return N_Zcl_SendStatus_Failure;
    }

   if (N_Address_Mode_Binding == pDestinationAddress->addrMode)
   {
     uint8_t bindingRecordIndex;
     N_Binding_Record_t bindingRecord;

     bindingRecordIndex = N_Binding_FindBinding(sourceEndpoint, pRequest->clusterId, 0u);
     if (INVALID_BINDING_RECORD_INDEX == bindingRecordIndex)
     {
         return N_Zcl_SendStatus_Failure;
     }
     N_Binding_GetBindingRecord(bindingRecordIndex,&bindingRecord);
     pDestinationAddress = &bindingRecord.destinationAddress;
   }

    uint16_t size = pRequest->payloadSize;

    // store the sequence number for later use by the N_Zcl_Framework_GetSentSequenceNumber function
    s_sentSequenceNumber = N_Zcl_Framework_GetNextSequenceNumber();

    if ( pRequest->frameControl.bits.manufacturerSpecific )
    {
        N_Zcl_ManufacturerCommand_t* pCommand = (N_Zcl_ManufacturerCommand_t*) pRequest;
        pCommand->sequenceNumber = s_sentSequenceNumber;
        size += 5u; // zcl header size for a manufacturer-specific command
    }
    else
    {
        N_Zcl_Command_t* pCommand = (N_Zcl_Command_t*) pRequest;
        pCommand->sequenceNumber = s_sentSequenceNumber;
        size += 3u; // zcl header size for a standard command
    }

    if (size > APS_MAX_TX_ASDU_SIZE)
    {
      return(N_Zcl_SendStatus_Failure);
    }

    ZclApsBuffer_t *apsBuffer = zclGetApsBuffer((size_t)size);
    if ( (apsBuffer == NULL) || (apsBuffer->frame.msg == NULL))
    {
        return N_Zcl_SendStatus_OutOfMemory;
    }
    
    memcpy(apsBuffer->frame.msg, &pRequest->frameControl, size);

    apsBuffer->dataReq.dstAddrMode = N_Cmi_PlatformToBcAddressModeConvert(pDestinationAddress->addrMode);
    N_Cmi_PlatformToBcAddressingConvert(&apsBuffer->dataReq.dstAddress, pDestinationAddress);
    apsBuffer->dataReq.dstEndpoint = pDestinationAddress->endPoint;
    if (pEndpointDescription->simpleDescriptor->AppProfileId == PROFILE_ID_LIGHT_LINK)
    {
        apsBuffer->dataReq.profileId = PROFILE_ID_HOME_AUTOMATION;
    }
    else
    {
        apsBuffer->dataReq.profileId = pEndpointDescription->simpleDescriptor->AppProfileId;
    }
    apsBuffer->dataReq.clusterId = pRequest->clusterId;
    apsBuffer->dataReq.srcEndpoint = sourceEndpoint;
    apsBuffer->dataReq.asduLength = size;

    apsBuffer->dataReq.radius = N_ZCL_DEFAULT_RADIUS;
    apsBuffer->dataReq.APS_DataConf = zclApsDataConf;

    N_Zcl_DataRequest(&apsBuffer->dataReq);

    return N_Zcl_SendStatus_Ok;
}

/** Interface function, see \ref N_Zcl_Framework_SendClusterSpecificResponse */
N_Zcl_SendStatus_t N_Zcl_Framework_SendClusterSpecificResponse_Impl(uint8_t zclPayloadLength, uint8_t* pZclPayload)
{
    return N_Zcl_Framework_SendCommand(
        s_destinationEndpoint,  // the destination endpoint of the command becomes the source endpoint of the response
        s_receivedClusterId,
        s_receivedCommandId,
        N_ZCL_FRAMEWORK_FRAME_TYPE_SPECIFIC_CMD,
        N_ZCL_FRAMEWORK_SERVER_CLIENT_DIR,
        N_ZCL_COMMAND_OPTION_DEFAULT_RESPONSE_ONLY_ON_ERROR,
        s_receivedManufacturerCode,
        s_receivedSequenceNumber,
        zclPayloadLength,
        pZclPayload,
        s_pSourceAddress);  // the source address becomes the destination address
}

/** Interface function, see \ref N_Zcl_Framework_GetSourceAddress */
N_Address_t* N_Zcl_Framework_GetSourceAddress_Impl(void)
{
    return s_pSourceAddress;
}

/** Interface function, see \ref N_Zcl_Framework_GetDestinationEndpoint */
uint8_t N_Zcl_Framework_GetDestinationEndpoint_Impl(void)
{
    return s_destinationEndpoint;
}

/** Interface function, see \ref N_Zcl_Framework_GetReceivedSequenceNumber */
uint8_t N_Zcl_Framework_GetReceivedSequenceNumber_Impl(void)
{
    return s_receivedSequenceNumber;
}

/** Interface function, see \ref N_Zcl_Framework_GetSentSequenceNumber */
uint8_t N_Zcl_Framework_GetSentSequenceNumber_Impl(void)
{
    return s_sentSequenceNumber;
}

/** Interface function, see \ref N_Zcl_Framework_ReceivedAsUnicast */
bool N_Zcl_Framework_ReceivedAsUnicast_Impl(void)
{
    return s_receivedAsUnicast;
}

/** Interface function, see \ref N_Zcl_Framework_IsReceivedDirectionClientSide */
bool N_Zcl_Framework_IsReceivedDirectionClientSide_Impl(void)
{
    return s_receivedDirectionIsClientSide;
}
