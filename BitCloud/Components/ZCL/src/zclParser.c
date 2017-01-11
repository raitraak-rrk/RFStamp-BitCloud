/************************************************************************//**
  \file zclParser.c

  \brief
    The ZCL Parser implementation

    The file implements the ZCL Parser

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    02.12.08 A. Potashov - Created.
******************************************************************************/
#if ZCL_SUPPORT == 1

/******************************************************************************
                   Includes section
******************************************************************************/
#include <zclDbg.h>
#include <sysUtils.h>
#include <sysQueue.h>
#include <zclTaskManager.h>
#include <zclParser.h>
#include <zcl.h>
#include <zclInt.h>
#include <zclMemoryManager.h>
#include <zclAttributes.h>
#include <sysAssert.h>

/******************************************************************************
                   Definitions section
******************************************************************************/
#define ZCL_DEFAULT_RESPONSE_PAYLOAD_SIZE   2

//! Cluster Security Descriptor
//! Defined the security type related to ClusterId
/******************************************************************************
                   Types section
******************************************************************************/
typedef struct
{
  uint8_t dataIndAmount;
  QueueDescriptor_t dataIndQueue;
} ZclParserMem_t;

/******************************************************************************
                   Static functions prototypes section
******************************************************************************/
static bool parseDataInd(APS_DataInd_t *ind);
static void processDataInd(void);
static void zclParserDataConf(APS_DataConf_t *conf);
static void sendDefaultResponse(ZclAuxParseData_t *auxData);
static void sendRelevantResponse(ZclFrameDescriptor_t *dsc, ZclMmBuffer_t *buf, APS_DataInd_t *ind, uint8_t cmd);
static bool zclReceivedDefaultResponseHandler(ZclAuxParseData_t *auxParseData);
static uint16_t formReadAttributesResponse(uint8_t *buf,
                                          uint16_t maxsize,
                                          const APS_DataInd_t *ind,
                                          const ZclFrameDescriptor_t *reqFrame);
static uint16_t formReadAttributesStructuredResponse(uint8_t *buf,
                                                    uint16_t maxsize,
                                                    const APS_DataInd_t *ind,
                                                    const ZclFrameDescriptor_t *reqFrame);
static uint8_t formWriteAttributeResponse(ZCL_WriteAttributeResp_t *respRec,
                                          const APS_DataInd_t *ind,
                                          const ZclFrameDescriptor_t *frameDescriptor,
                                          bool undivided);
static uint8_t formDiscoverAttributesResponse(ZCL_DiscoverAttributesResp_t *resp,
                                              const APS_DataInd_t *ind,
                                              const ZclFrameDescriptor_t *frameDescriptor);
static uint8_t formDiscoverCommandsReceivedResponse(ZCL_DiscoverCommandsReceivedResp_t *resp,
                                              const APS_DataInd_t *ind,
                                              const ZclFrameDescriptor_t *frameDescriptor);
static uint8_t formDiscoverCommandsGeneratedResponse(ZCL_DiscoverCommandsGeneratedResp_t *resp,
                                              const APS_DataInd_t *ind,
                                              const ZclFrameDescriptor_t *frameDescriptor);
static uint8_t formDiscoverAttributeExtendedsResponse(ZCL_DiscoverAttributesExtendedResp_t *resp,
                                             const APS_DataInd_t *ind,
                                             const ZclFrameDescriptor_t *frameDescriptor);
static void formServerDataReq(ZclMmBuffer_t *buffer, APS_DataInd_t *ind);

static bool zclDefaultResponseIsValid(ZclAuxParseData_t *auxData);
static inline bool isDefaultResponse(ZclFrameDescriptor_t *frameDescriptor);
static ZCL_Status_t validateIncomingCommand(const APS_DataInd_t *ind, const ZclFrameDescriptor_t *frameDescriptor);
static ZCL_Request_t * zclFindAppropriateRequest(ZclAuxParseData_t *auxParseData);
#if APP_ENABLE_CERTIFICATION_EXTENSION == 1
static bool isSupportedManufacturerSpecCode(uint16_t  manufacturerCode);
#endif // APP_ENABLE_CERTIFICATION_EXTENSION == 1

/******************************************************************************
                   Static variables section
******************************************************************************/
static ZclParserMem_t parserMem;

/******************************************************************************
                   Implementation section
******************************************************************************/
/*************************************************************************//**
  \brief Reset parser queue and clear memory.
*****************************************************************************/
void zclParserInit(void)
{
  memset(&parserMem, 0, sizeof(parserMem));
  resetQueue(&parserMem.dataIndQueue);
}

/**************************************************************************//**
\brief Check incoming ZCL frame header and fill frame descriptor

\param[out] frameDescriptor - frame descriptor
\param[in] ind - data indication
\return header size or 0 if error
******************************************************************************/
static uint8_t parseZclHeader(ZclFrameDescriptor_t *frameDescriptor, const APS_DataInd_t *ind)
{
  ZclFrameHeader_t *header = (ZclFrameHeader_t *) ind->asdu;
  uint8_t result = sizeof(ZclFrameHeader_t);

  //frame descriptor init
  memset(frameDescriptor, 0, sizeof(ZclFrameDescriptor_t));

  // frame type must be 00 or 01
  // reserved must have zero value
  // 0001.1101
  if (0x01 < (header->frameControl.uint8 & 0xe3))
    return 0;

  //parsing
  frameDescriptor->frameType            = header->frameControl.frameType;
  frameDescriptor->manufacturerSpecific = header->frameControl.manufacturerSpecific;
  frameDescriptor->direction            = header->frameControl.direction;
  frameDescriptor->defaultResponse      = header->frameControl.defaultResponse;

  if (ZCL_FRAME_CONTROL_MANUFACTURER_NONSPECIFIC == frameDescriptor->manufacturerSpecific)
  {
    header = (ZclFrameHeader_t *) (ind->asdu - sizeof(uint16_t));
    result -= sizeof(uint16_t);
  }
  frameDescriptor->manufacturerCode = header->manufacturerCode;
  frameDescriptor->sequenceNumber = header->sequenceNumber;
  frameDescriptor->commandId = header->commandId;

  frameDescriptor->header                     = ind->asdu;
  frameDescriptor->payload                    = ind->asdu + result;

  frameDescriptor->headerLength               = result;
  frameDescriptor->payloadLength              = ind->asduLength - result;
  return result;
}

/**************************************************************************//**
\brief Form header in supplied ZCL frame

\param[in, out] frameDescriptor - frame descriptor
******************************************************************************/
static void formZclHeader(ZclFrameDescriptor_t *frameDescriptor)
{
  ZclFrameHeader_t *header = (ZclFrameHeader_t *) frameDescriptor->header;

  header->frameControl.uint8                  = 0;
  header->frameControl.frameType              = frameDescriptor->frameType;
  header->frameControl.manufacturerSpecific   = frameDescriptor->manufacturerSpecific;
  header->frameControl.direction              = frameDescriptor->direction;
  header->frameControl.defaultResponse        = frameDescriptor->defaultResponse;
  header->frameControl.reserved               = 0;

  assert_static(ZCL_FRAME_HEADER_SIZE == sizeof(ZclFrameHeader_t));
  frameDescriptor->headerLength               = sizeof(ZclFrameHeader_t);

  if (ZCL_FRAME_CONTROL_MANUFACTURER_NONSPECIFIC == frameDescriptor->manufacturerSpecific)
  {
    header = (ZclFrameHeader_t *) (frameDescriptor->header - sizeof(uint16_t));
    frameDescriptor->headerLength -= sizeof(uint16_t);
  }
  else
    header->manufacturerCode                  = frameDescriptor->manufacturerCode;
  header->commandId                           = frameDescriptor->commandId;
  header->sequenceNumber                      = frameDescriptor->sequenceNumber;
}


/**************************************************************************//**
\brief Form server data request

\param[out] buffer - memory buffer to form request in
\param[in] ind - APS data indication with aux data to form request
******************************************************************************/
static void formServerDataReq(ZclMmBuffer_t *buffer, APS_DataInd_t *ind)
{
  // initialize APS_DataReq primitive
  buffer->primitive.apsDataReq.dstAddrMode = ind->srcAddrMode;
  if (APS_SHORT_ADDRESS == ind->srcAddrMode)
    buffer->primitive.apsDataReq.dstAddress.shortAddress = ind->srcAddress.shortAddress;
  if (APS_EXT_ADDRESS == ind->srcAddrMode)
    buffer->primitive.apsDataReq.dstAddress.extAddress = ind->srcAddress.extAddress;
  buffer->primitive.apsDataReq.srcEndpoint = ind->dstEndpoint;
  buffer->primitive.apsDataReq.dstEndpoint = ind->srcEndpoint;
  buffer->primitive.apsDataReq.clusterId = ind->clusterId;
  buffer->primitive.apsDataReq.profileId = ind->profileId;
  buffer->primitive.apsDataReq.APS_DataConf = zclParserDataConf;

  buffer->primitive.apsDataReq.txOptions.acknowledgedTransmission    = 1;
  buffer->primitive.apsDataReq.txOptions.doNotDecrypt                = 1;
  buffer->primitive.apsDataReq.txOptions.noRouteDiscovery            = 0;
  buffer->primitive.apsDataReq.radius                                = 0;
  buffer->primitive.apsDataReq.asdu = buffer->frame + getZclAsduOffset();
}

/**************************************************************************//**
\brief Callback on execution of server data request

\param[in] conf - confirmation data
******************************************************************************/
static void zclParserDataConf(APS_DataConf_t *conf)
{
  zclMmFreeMem((ZclMmBuffer_t *)GET_PARENT_BY_FIELD(APS_DataReq_t, confirm, conf));
}

/*************************************************************************//**
  \brief APS Data indication to ZCL layer

  \param[in] ind - buffer of data indication.
*****************************************************************************/
void zclDataInd(APS_DataInd_t *ind)
{
  ZclMmBuffer_t *indBuffer;
  uint8_t bufferAmount;
  uint8_t bufferSize;

  CS_ReadParameter(CS_ZCL_MEMORY_BUFFERS_AMOUNT_ID, (void *)&bufferAmount);
  CS_ReadParameter(CS_ZCL_BUFFER_SIZE_ID, (void *)&bufferSize);
  if ((bufferAmount - 1) > parserMem.dataIndAmount)
  {
    if (ind->asduLength > bufferSize)
    {
      SYS_E_ASSERT_ERROR(false, ZCL_BUFFER_SIZE_IS_TOO_LOW);
      return;
    }
    indBuffer = zclMmGetMem(ZCL_INPUT_DATA_BUFFER);

    if (indBuffer)
    {
      SYS_BYTE_MEMCPY(&indBuffer->primitive.apsDataInd, ind, sizeof(APS_DataInd_t));
      SYS_BYTE_MEMCPY(indBuffer->frame + getZclAsduOffset(), ind->asdu, ind->asduLength);
      indBuffer->primitive.apsDataInd.asdu = indBuffer->frame + getZclAsduOffset();
      putQueueElem(&parserMem.dataIndQueue, &indBuffer->primitive.apsDataInd);
      zclPostTask(ZCL_PARSER_TASK_ID);
      parserMem.dataIndAmount++;
    }
  }
  else
  {
    SYS_E_ASSERT_ERROR(false, ZCL_DATAIND_0);
    return;
  }
}

/**************************************************************************//**
\brief ZCL task handler
******************************************************************************/
void zclParserTaskHandler(void)
{
  if (parserMem.dataIndAmount)
    processDataInd();
  if (parserMem.dataIndAmount)
    zclPostTask(ZCL_PARSER_TASK_ID);
}

/**************************************************************************//**
\brief Get length of single payload element

\param[in] element - pointer to element
\param[in] formRequest - true if function is used to calculate request payload
  length, false if function is used to calculate response payload length
\return length
******************************************************************************/
static uint8_t getElementLength(const ZCL_NextElement_t *element, bool formRequest)
{
  uint8_t result;
  const uint8_t *payload = (formRequest ? element->content : element->payload);

  result = (formRequest ? ZCL_MAX_TX_ZSDU_SIZE : ZCL_MAX_RX_ZSDU_SIZE);
  result -= ZCL_FRAME_STANDARD_HEADER_SIZE;

  switch (element->id)
  {
    case ZCL_READ_ATTRIBUTES_COMMAND_ID:
      result = sizeof(ZCL_ReadAttributeReq_t);
      break;

    case ZCL_READ_ATTRIBUTES_STRUCTURED_COMMAND_ID:
      {
        const ZCL_ReadAttributesStructuredReq_t *req = (const ZCL_ReadAttributesStructuredReq_t *)payload;
        result = SLICE_SIZE(ZCL_ReadAttributesStructuredReq_t, id, selector.indicator) +
                req->selector.indicator * sizeof(req->selector.index);
      }
      break;

    case ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID:
      {
        const ZCL_ReadAttributeResp_t *resp = (const ZCL_ReadAttributeResp_t *) payload;
        if (ZCL_SUCCESS_STATUS == resp->status)
          result = SLICE_SIZE(ZCL_ReadAttributeResp_t, id, type) + ZCL_GetAttributeLength(resp->type, resp->value);
        else
          result = SLICE_SIZE(ZCL_ReadAttributeResp_t, id, status);
      }
      break;

    case ZCL_WRITE_ATTRIBUTES_COMMAND_ID:
      {
        const ZCL_WriteAttributeReq_t *req = (const ZCL_WriteAttributeReq_t *) payload;
        result = SLICE_SIZE(ZCL_WriteAttributeReq_t, id, type) + ZCL_GetAttributeLength(req->type, req->value);
      }
      break;

    case ZCL_WRITE_ATTRIBUTES_RESPONSE_COMMAND_ID:
      result = sizeof(ZCL_WriteAttributeResp_t);
      break;

    case ZCL_REPORT_ATTRIBUTES_COMMAND_ID:
      {
        const ZCL_Report_t *rep = (const ZCL_Report_t*) payload;
        result = SLICE_SIZE(ZCL_Report_t, id, type) + ZCL_GetAttributeLength(rep->type, rep->value);
      }
      break;

    case ZCL_CONFIGURE_REPORTING_COMMAND_ID:
      {
        const ZCL_ConfigureReportingReq_t *req = (const ZCL_ConfigureReportingReq_t *) payload;
        if (ZCL_FRAME_CONTROL_DIRECTION_SERVER_TO_CLIENT == req->direction)
        {
          // The timeout period field is included in the payload, and the attribute data type field,
          // the minimum reporting interval field, the maximum reporting imterval field,
          // reportable change field are omitted.
          result = SLICE_SIZE(ZCL_ConfigureReportingReq_t, direction, timeoutPeriod);
        }
        else
        {
          ZCL_DataTypeDescriptor_t descriptor;
          //Attribute data type field, the minimum reporting interval field, the maximum reporting imterval field
          //and reportable change field are included in frame payload. Timeout period field is omitted.
          result = SLICE_SIZE(ZCL_ConfigureReportingReq_t, direction,  maxReportingInterval);
          ZCL_GetDataTypeDescriptor(req->attributeType, req->reportableChange, &descriptor);
          if (ZCL_DATA_TYPE_ANALOG_KIND == descriptor.kind)
            result += descriptor.length;
        }
      }
      break;

    case ZCL_READ_REPORTING_CONFIGURATION_RESPONSE_COMMAND_ID:
      {
        const ZCL_ReadReportingConfigurationResp_t *resp = (const ZCL_ReadReportingConfigurationResp_t *) payload;
        if (resp->status != ZCL_SUCCESS_STATUS)
          break;

        if (ZCL_FRAME_CONTROL_DIRECTION_SERVER_TO_CLIENT == resp->direction)
        {
          // The timeout period field is included in the payload, and the attribute data type field,
          // the minimum reporting interval field, the maximum reporting imterval field,
          // reportable change field are omitted.
          result = SLICE_SIZE(ZCL_ReadReportingConfigurationResp_t, status, timeoutPeriod);
        }
        else
        {
          ZCL_DataTypeDescriptor_t descriptor;
          //Attribute data type field, the minimum reporting interval field, the maximum reporting imterval field
          //and reportable change field are included in frame payload. Timeout period field is omitted.
          result = SLICE_SIZE(ZCL_ReadReportingConfigurationResp_t, status, maxReportingInterval);
          ZCL_GetDataTypeDescriptor(resp->attributeType, resp->reportableChange, &descriptor);
          if (ZCL_DATA_TYPE_ANALOG_KIND == descriptor.kind)
            result += descriptor.length;
        }
      }
      break;

    case ZCL_CONFIGURE_REPORTING_RESPONSE_COMMAND_ID:
      result = sizeof(ZCL_ConfigureReportingResp_t);
      break;

    case ZCL_READ_REPORTING_CONFIGURATION_COMMAND_ID:
      result = sizeof(ZCL_ReadReportingConfigurationReq_t);
      break;

    case ZCL_DISCOVER_ATTRIBUTES_COMMAND_ID:
      result = sizeof(ZCL_DiscoverAttributesReq_t);
      break;

    case ZCL_DISCOVER_ATTRIBUTES_RESPONSE_COMMAND_ID:
      result = sizeof(ZCL_DiscoverAttributesRespRec_t);
      break;

    case ZCL_DISCOVER_COMMANDS_RECEIVED_COMMAND_ID:
      result = sizeof(ZCL_DiscoverCommandsReceivedReq_t);
      break;

    case ZCL_DISCOVER_COMMANDS_RECEIVED_RESPONSE_COMMAND_ID:
      result = sizeof(ZCL_CommandId_t);
      break; 

    case ZCL_DISCOVER_COMMANDS_GENERATED_COMMAND_ID:
      result = sizeof(ZCL_DiscoverCommandsGeneratedReq_t);
      break;

    case ZCL_DISCOVER_COMMANDS_GENERATED_RESPONSE_COMMAND_ID:
      result = sizeof(ZCL_CommandId_t);
      break;

    case ZCL_DISCOVER_ATTRIBUTES_EXTENDED_COMMAND_ID:
      result = sizeof(ZCL_DiscoverAttributesExtendedReq_t);
      break;

    case ZCL_DISCOVER_ATTRIBUTES_EXTENDED_RESPONSE_COMMAND_ID:
      result = sizeof(ZCL_DiscoverAttributesExtendedRespRec_t);
      break;

    default:
      break;
  }
  return result;
}

/**************************************************************************//**
\brief Put element to payload

\param[in] element - pointer to element
\return status of operation
******************************************************************************/
ZCL_Status_t ZCL_PutNextElement(ZCL_NextElement_t *element)
{
  ZCL_Status_t result = ZCL_SUCCESS_STATUS;
  uint8_t elementLength;
  uint16_t maxSize = ZCL_MAX_TX_ZSDU_SIZE;
  maxSize -= ZCL_FRAME_STANDARD_HEADER_SIZE;

  if (NULL == element)
  {
    result = ZCL_INVALID_PARAMETER_STATUS;
  }
  else
  {
    elementLength = getElementLength(element, true);
    if (maxSize >= element->payloadLength + elementLength)
    {
      SYS_BYTE_MEMCPY(element->payload, element->content, elementLength);
      element->payloadLength += elementLength;
      element->payload += elementLength;
    }
    else
    {
      result = ZCL_END_PAYLOAD_REACHED_STATUS;
    }
  }
  return result;
}

/**************************************************************************//**
\brief Get next element from payload

\param[in] element - pointer to element
\return status of operation
******************************************************************************/
ZCL_Status_t ZCL_GetNextElement(ZCL_NextElement_t *element)
{
  ZCL_Status_t result = ZCL_SUCCESS_STATUS;
  uint8_t elementLength;

  if (NULL == element || NULL == element->payload || 0 == element->payloadLength)
  {
    result = ZCL_INVALID_PARAMETER_STATUS;
  }
  else
  {
    elementLength = getElementLength(element, false);
    if (elementLength <= element->payloadLength)
    {
      if (NULL == element->content)
        element->content = element->payload;
      else
        SYS_BYTE_MEMCPY(element->content, element->payload, elementLength);
      element->payloadLength -= elementLength;
      element->payload += elementLength;
      if (0 == element->payloadLength)
        result = ZCL_END_PAYLOAD_REACHED_STATUS;
    }
    else
      result = ZCL_INVALID_VALUE_STATUS;
  }
  return result;
}

/**************************************************************************//**
\brief Form zcl header.

\param[in] - dataReq - pointer to data request;
\param[in] - reqType - type of the request;
\param[in] - direction - cluster\server side;
\param[in] - commandId - zcl command identification;
\param[in] - defaultResponse - presence or absence of the default responde;
\param[in] - manufacSpecCode - manufacturer specific code;
\param[in] - sequenceNumber - next sequence number;
\return header length
******************************************************************************/
uint8_t zclFormRequest(APS_DataReq_t *dataReq,
                       uint8_t reqType,
                       uint8_t direction,
                       uint8_t commandId,
                       uint8_t defaultResponse,
                       uint16_t manufacSpecCode,
                       uint8_t sequenceNumber)
{
  ZclFrameDescriptor_t frameDescriptor;

  //ZCL Frame Header
  memset(&frameDescriptor, 0, sizeof(ZclFrameDescriptor_t));

  frameDescriptor.commandId            = commandId;
  frameDescriptor.frameType            = reqType;
  frameDescriptor.direction            = direction;
  frameDescriptor.defaultResponse      = defaultResponse;
  frameDescriptor.manufacturerCode     = manufacSpecCode;
  if (!manufacSpecCode)
    frameDescriptor.manufacturerSpecific = ZCL_FRAME_CONTROL_MANUFACTURER_NONSPECIFIC;
  else
    frameDescriptor.manufacturerSpecific = ZCL_FRAME_CONTROL_MANUFACTURER_SPECIFIC;

#if APP_ENABLE_CERTIFICATION_EXTENSION == 1
  if (0xFFFF == manufacSpecCode)
  {
    frameDescriptor.manufacturerCode     = 0x0000;
    frameDescriptor.manufacturerSpecific = ZCL_FRAME_CONTROL_MANUFACTURER_SPECIFIC;
  }
#endif

  frameDescriptor.sequenceNumber       = sequenceNumber;
  frameDescriptor.header               = dataReq->asdu;
  formZclHeader(&frameDescriptor);

  return frameDescriptor.headerLength;
}

/**************************************************************************//**
\brief Forms response for write attribute command.

\param[out] - respRec - pointer to formed write attribute response;
\param[in]  - ind - pointer to received packet from APS layer;
\param[in]  - frameDescriptor - descriptor of ZCL frame;
\param[in]  - undivided - should write be atomic

\return response length
******************************************************************************/
static uint8_t formWriteAttributeResponse(ZCL_WriteAttributeResp_t *respRec,
                                          const APS_DataInd_t *ind,
                                          const ZclFrameDescriptor_t *frameDescriptor,
                                          bool undivided)
{
  uint8_t responseLength = 0;
  Endpoint_t endpoint = ind->dstEndpoint;
  ClusterId_t clusterId = ind->clusterId;
  ZCL_WriteAttributeReq_t *writeAttributeReq = (ZCL_WriteAttributeReq_t *)frameDescriptor->payload;
  uint8_t payloadLength = frameDescriptor->payloadLength;

  // If "undivided" request received - write attributes only after successful checking.
  // 2-phase commit should be performed in this case.
  uint8_t phase = undivided ? 2 : 1;
  while (phase)
  {
    phase--;
    while (payloadLength)
    {
      uint8_t recLen = ZCL_GetAttributeLength(writeAttributeReq->type, writeAttributeReq->value);
      recLen += sizeof(uint16_t) + sizeof(uint8_t);

      if (recLen > payloadLength)
      {
        SYS_E_ASSERT_ERROR(false, ZCLPARSER_FORMWRITEATTRIBUTERESPONSE0);
        return 0;
      }

      respRec->status = zclRemoteWriteAttributeValue(endpoint,
                                                     clusterId,
                                                     getOwnClusterSideByIncomingCommandDirection(frameDescriptor->direction),
                                                     writeAttributeReq->id,
                                                     writeAttributeReq->type,
                                                     writeAttributeReq->value,
                                                     phase);

      if (respRec->status)
      {
        respRec->id = writeAttributeReq->id;
        responseLength += sizeof(ZCL_WriteAttributeResp_t);
        respRec++;
        // Send response immediately, if error occured during checking phase
        if (phase)
          return responseLength;
      }
      // Indicate successfull writing
      else if (!phase)
        zclAttributeEventInd(ind, frameDescriptor, ZCL_WRITE_ATTRIBUTE_EVENT, writeAttributeReq->id);

      writeAttributeReq = (ZCL_WriteAttributeReq_t *) (((uint8_t *) writeAttributeReq) + recLen);
      payloadLength -= recLen;
    }
    // Checking complete - start write phase
    writeAttributeReq = (ZCL_WriteAttributeReq_t *)frameDescriptor->payload;
    payloadLength = frameDescriptor->payloadLength;
  }
  return (responseLength ? responseLength : 1);
}

/**************************************************************************//**
\brief Form response for read attribute command

\param[out] buf - buffer to store response
\param[in] maxsize - maximum buffer size
\param[in] ind - pointer to packet from APS layer
\param[in] reqFrame - descriptor of incoming ZCL frame
\return response length
******************************************************************************/
static uint16_t formReadAttributesResponse(uint8_t *buf,
                                          uint16_t maxsize,
                                          const APS_DataInd_t *ind,
                                          const ZclFrameDescriptor_t *reqFrame)
{
  uint16_t respLen = 0;

  const ZCL_ReadAttributeReq_t *req = (ZCL_ReadAttributeReq_t *)reqFrame->payload;

  for (uint8_t reqLen = reqFrame->payloadLength; reqLen; reqLen -= sizeof(ZCL_ReadAttributeReq_t), req++)
  {
    ZCL_ReadAttributeResp_t *outRec = (ZCL_ReadAttributeResp_t *)buf;
    uint8_t type = ZCL_NO_DATA_TYPE_ID;
    uint16_t recLen = 0;

    // At least id and status fields must fit to response
    if (respLen + SLICE_SIZE(ZCL_ReadAttributeResp_t, id, status) >= maxsize)
      break;

    // Ignore truncated request records
    if (reqLen < sizeof(ZCL_ReadAttributeReq_t))
    {
      SYS_E_ASSERT_ERROR(0, ZCLPARSER_FORMREADATTRIBUTERESPONSE0);
      return respLen;
    }

    outRec->id = req->id;
    // Ok, try to read attribute in a safe manner
    outRec->status = zclReadAttributeValueSafe(ind->dstEndpoint,
                                               ind->clusterId,
                                               getOwnClusterSideByIncomingCommandDirection(reqFrame->direction),
                                               req->id,
                                               &type,
                                               outRec->value,
                                               maxsize - respLen - sizeof(outRec->type)); // This much space is available for response

    // Ignore request if there is no space for response. Another attribute may fit though.
    // Use same slot in response.
    if (outRec->status == ZCL_INSUFFICIENT_SPACE_STATUS)
      continue;

    if (outRec->status == ZCL_UNSUPPORTED_ATTRIBUTE_STATUS)
    {
      // No such attribute. Omit type and value fields
      recLen = SLICE_SIZE(ZCL_ReadAttributeResp_t, id, status);
    }
    else
    {
      // Got attribute and copied it just fine
      outRec->type = type;
      recLen = SLICE_SIZE(ZCL_ReadAttributeResp_t, id, type) + ZCL_GetAttributeLength(type, outRec->value);
      zclAttributeEventInd(ind, reqFrame, ZCL_READ_ATTRIBUTE_EVENT, req->id);
    }

    respLen += recLen;
    buf += recLen;
  }
  return respLen;
}

/**************************************************************************//**
\brief Form response for read attribute structured command

\param[out] buf - buffer to store response
\param[in] maxsize - maximum buffer size
\param[in] ind - pointer to packet from APS layer
\param[in] reqFrame - descriptor of incoming ZCL frame
\return response length
******************************************************************************/
static uint16_t formReadAttributesStructuredResponse(uint8_t *buf,
                                                    uint16_t maxsize,
                                                    const APS_DataInd_t *ind,
                                                    const ZclFrameDescriptor_t *reqFrame)
{
  uint16_t respLen = 0;
  const uint8_t *reqPayload = reqFrame->payload;
  int16_t reqLen = reqFrame->payloadLength;

  while (reqLen > 0)
  {
    const ZCL_ReadAttributesStructuredReq_t *req = (const ZCL_ReadAttributesStructuredReq_t *)reqPayload;
    ZCL_ReadAttributeResp_t *outRec = (ZCL_ReadAttributeResp_t *)buf;
    uint8_t type = ZCL_NO_DATA_TYPE_ID;
    uint16_t reqItemLen;

    // At least id and status fields must fit to response
    if (respLen + SLICE_SIZE(ZCL_ReadAttributeResp_t, id, status) >= maxsize)
      break;

    outRec->id = req->id;
    outRec->status = zclReadIndexedAttribute(&req->selector,
                                             ind->dstEndpoint,
                                             ind->clusterId,
                                             getOwnClusterSideByIncomingCommandDirection(reqFrame->direction),
                                             req->id,
                                             &type,
                                             outRec->value,
                                             maxsize - respLen - sizeof(outRec->type)); // This much space is available for response

    if (outRec->status != ZCL_INSUFFICIENT_SPACE_STATUS)
    {
      uint16_t recLen;
      if (outRec->status != ZCL_SUCCESS_STATUS)
      {
        // Fail. Omit type and value fields
        recLen = SLICE_SIZE(ZCL_ReadAttributeResp_t, id, status);
      }
      else
      {
        // Got attribute and copied it just fine
        outRec->type = type;
        recLen = SLICE_SIZE(ZCL_ReadAttributeResp_t, id, type) + ZCL_GetAttributeLength(type, outRec->value);
        zclAttributeEventInd(ind, reqFrame, ZCL_READ_ATTRIBUTE_EVENT, req->id);
      }
      respLen += recLen;
      buf += recLen;
    }
    // Advance to the next attribute item in request
    reqItemLen = SLICE_SIZE(ZCL_ReadAttributesStructuredReq_t, id, selector.indicator) +
              req->selector.indicator * sizeof(req->selector.index);
    reqPayload += reqItemLen;
    reqLen -= reqItemLen;
  }
  return respLen;
}

/**************************************************************************//**
\brief Form response for discover attributes command

\param[out] resp - pointer to buffer for creating response
\param[in] ind - pointer to packet from APS layer
\param[in] frameDescriptor - descriptor of the incoming frame

\return response length
******************************************************************************/
static uint8_t formDiscoverAttributesResponse(ZCL_DiscoverAttributesResp_t *resp,
                                             const APS_DataInd_t *ind,
                                             const ZclFrameDescriptor_t *frameDescriptor)
{
  uint8_t result = 0;
  const Endpoint_t endpoint = ind->dstEndpoint;
  const ClusterId_t clusterId = ind->clusterId;
  ZclAttribute_t *attributeDescriptor = NULL;
  ZCL_DiscoverAttributesRespRec_t *rec = resp->attributeInfo;
  uint8_t counter = 0;
  ZCL_DiscoverAttributesReq_t *req = (ZCL_DiscoverAttributesReq_t *)frameDescriptor->payload;

  if (sizeof(ZCL_DiscoverAttributesReq_t) == frameDescriptor->payloadLength)
  {
    result = sizeof(uint8_t) /*discoveryComplete*/;
    resp->discoveryComplete = 0;

    do
    {
      attributeDescriptor = zclGetNextAttribute(endpoint,
                                                clusterId,
                                                getOwnClusterSideByIncomingCommandDirection(frameDescriptor->direction),
                                                attributeDescriptor);
      if (NULL == attributeDescriptor)
      {
        resp->discoveryComplete = 1;
      }
      else
      {
        if ((attributeDescriptor->id >= req->startAttributeId) &&
            (counter < req->maxAttributeIds))
        {
          rec->attributeId = attributeDescriptor->id;
          rec->typeId = attributeDescriptor->type;
          result += sizeof(ZCL_DiscoverAttributesRespRec_t);
          rec++;
          counter++;
        }
      }
    }
    while (NULL != attributeDescriptor);
  }

  return result;
}

/**************************************************************************//**
\brief Form response for discover commands received command

\param[out] resp - pointer to buffer for creating response
\param[in] ind - pointer to packet from APS layer
\param[in] frameDescriptor - descriptor of the incoming frame

\return response length
******************************************************************************/
static uint8_t formDiscoverCommandsReceivedResponse(ZCL_DiscoverCommandsReceivedResp_t *resp,
                                             const APS_DataInd_t *ind,
                                             const ZclFrameDescriptor_t *frameDescriptor)
{
  uint8_t result = 0;
  const Endpoint_t endpoint = ind->dstEndpoint;
  const ClusterId_t clusterId = ind->clusterId;
  ZCL_Cluster_t *cluster;
  ZclCommand_t  *command;
  uint8_t clusterCount = 0;
  uint8_t counter = 0;
  uint8_t clusterSide = getOwnClusterSideByIncomingCommandDirection(frameDescriptor->direction);

  ZCL_DiscoverCommandsReceivedReq_t *req = (ZCL_DiscoverCommandsReceivedReq_t *)frameDescriptor->payload;

  cluster = ZCL_GetCluster(endpoint,
                           clusterId,
                           clusterSide);

  if (sizeof(ZCL_DiscoverCommandsReceivedReq_t) == frameDescriptor->payloadLength)
  {
    result = sizeof(uint8_t) /*discoveryComplete*/;
    resp->discoveryComplete = 0;

    if (NULL == cluster)
    {
      resp->discoveryComplete = 1;
    }
    else
    {
      clusterCount =  cluster->commandsAmount;
      command = (ZclCommand_t*)cluster->commands;
      do
      {
         if ((command->id >= req->startCommandId) &&
              (counter < req->maxCommandIds) && clusterSide == command->options.direction)
          {
            resp->commandIds[counter] = ((ZclCommand_t*)command)->id ;
            result += sizeof(uint8_t);/*size of each command Id included*/
            counter++;
          }

        command = zclGetNextCommand(command);
        clusterCount--;

        if (counter == req->maxCommandIds)
          break;

      } while (clusterCount);
      
      if (!clusterCount)
        resp->discoveryComplete = 1;
    }
  }
  return result;
}
/**************************************************************************//**
\brief Form response for discover commands generated command

\param[out] resp - pointer to buffer for creating response
\param[in] ind - pointer to packet from APS layer
\param[in] frameDescriptor - descriptor of the incoming frame

\return response length
******************************************************************************/
static uint8_t formDiscoverCommandsGeneratedResponse(ZCL_DiscoverCommandsGeneratedResp_t *resp,
                                              const APS_DataInd_t *ind,
                                              const ZclFrameDescriptor_t *frameDescriptor)
{
  uint8_t result = 0;
  const Endpoint_t endpoint = ind->dstEndpoint;
  const ClusterId_t clusterId = ind->clusterId;
  ZCL_Cluster_t *cluster;
  ZclCommand_t  *command;
  uint8_t clusterCount = 0;
  uint8_t counter = 0;
  ZCL_DiscoverCommandsGeneratedReq_t *req = (ZCL_DiscoverCommandsGeneratedReq_t *)frameDescriptor->payload;
  uint8_t clusterSide = getOwnClusterSideByIncomingCommandDirection(frameDescriptor->direction);

  cluster = ZCL_GetCluster(endpoint,
                           clusterId,
                           clusterSide);

  if (sizeof(ZCL_DiscoverCommandsGeneratedReq_t) == frameDescriptor->payloadLength)
  {
    result = sizeof(uint8_t) /*discoveryComplete*/;
    resp->discoveryComplete = 0;

    if (NULL == cluster)
    {
      resp->discoveryComplete = 1;
    }
    else
    {
      clusterCount =  cluster->commandsAmount;
      command = (ZclCommand_t*)cluster->commands;
      do
      {
         if ((command->id >= req->startCommandId) &&
              (counter < req->maxCommandIds) && ((!clusterSide) == command->options.direction))
          {
            resp->commandIds[counter] = ((ZclCommand_t*)command)->id ;
            result += sizeof(uint8_t);/*size of each command Id included*/
            counter++;
          }

        command = zclGetNextCommand(command);
        clusterCount--;

        if (counter == req->maxCommandIds)
          break;

      } while (clusterCount);

      if (!clusterCount)
        resp->discoveryComplete = 1;
    }
  }
  return result;
}

/**************************************************************************//**
\brief Form response for discover attributes extended command

\param[out] resp - pointer to buffer for creating response
\param[in] ind - pointer to packet from APS layer
\param[in] frameDescriptor - descriptor of the incoming frame

\return response length
******************************************************************************/
static uint8_t formDiscoverAttributeExtendedsResponse(ZCL_DiscoverAttributesExtendedResp_t *resp,
                                             const APS_DataInd_t *ind,
                                             const ZclFrameDescriptor_t *frameDescriptor)
{
  uint8_t result = 0;
  const Endpoint_t endpoint = ind->dstEndpoint;
  const ClusterId_t clusterId = ind->clusterId;
  ZclAttribute_t *attributeDescriptor = NULL;
  ZCL_DiscoverAttributesExtendedRespRec_t *rec = resp->attributeInfo;
  uint8_t counter = 0;
  ZCL_DiscoverAttributesExtendedReq_t *req = (ZCL_DiscoverAttributesExtendedReq_t *)frameDescriptor->payload;

  if (sizeof(ZCL_DiscoverAttributesExtendedReq_t) == frameDescriptor->payloadLength)
  {
    result = sizeof(uint8_t) /*discoveryComplete*/;
    resp->discoveryComplete = 0;

    do
    {
      attributeDescriptor = zclGetNextAttribute(endpoint,
                                                clusterId,
                                                getOwnClusterSideByIncomingCommandDirection(frameDescriptor->direction),
                                                attributeDescriptor);
      if (NULL == attributeDescriptor)
      {
        resp->discoveryComplete = 1;
      }
      else
      {
        if ((attributeDescriptor->id >= req->startAttributeId) &&
            (counter < req->maxAttributeIds))
        {
          rec->attributeId = attributeDescriptor->id;
          rec->typeId = attributeDescriptor->type;
          rec->accessCtrrl.readable = 1;
          rec->accessCtrrl.writeable = (uint8_t)(attributeDescriptor->properties & ZCL_READONLY_ATTRIBUTE)? 0U : 1U;
          rec->accessCtrrl.reportable = (uint8_t)(attributeDescriptor->properties & ZCL_REPORTABLE_ATTRIBUTE)? 1U: 0U;
          result += sizeof(ZCL_DiscoverAttributesExtendedRespRec_t);
          rec++;
          counter++;
        }
        else if(counter == req->maxAttributeIds)
          break;
      }
    }
    while (NULL != attributeDescriptor);
  }
  return result;
}

/***************************************************************************//**
\brief Creates and sends default response if it is needed

\param[in] auxData - pointer to auxilliary structure with data for frame formation
******************************************************************************/
static void sendDefaultResponse(ZclAuxParseData_t *auxData)
{
  ZclFrameDescriptor_t *incomingFrameDesc = auxData->frameDescriptor;
  ZclFrameDescriptor_t outcomingFrameDesc;
  ZclMmBuffer_t *buf;
  uint8_t *payloadOut;

  if (!zclDefaultResponseIsValid(auxData))
    return;

  //get buffer for response
  buf = zclMmGetMem(ZCL_OUTPUT_RESPONSE_BUFFER);
  if (!buf)
    return;

  outcomingFrameDesc.headerLength = incomingFrameDesc->headerLength;
  payloadOut = buf->frame + getZclAsduOffset() + outcomingFrameDesc.headerLength;
  ((ZCL_DefaultResp_t*)payloadOut)->commandId = incomingFrameDesc->commandId;
  ((ZCL_DefaultResp_t*)payloadOut)->statusCode = auxData->commandStatus;
  // size of status code and command id and header length
  buf->primitive.apsDataReq.asduLength = sizeof(ZCL_DefaultResp_t) + outcomingFrameDesc.headerLength;
  outcomingFrameDesc.commandId = ZCL_DEFAULT_RESPONSE_COMMAND_ID;
  if (ZCL_FRAME_CONTROL_DIRECTION_SERVER_TO_CLIENT == incomingFrameDesc->direction)
    outcomingFrameDesc.direction = ZCL_FRAME_CONTROL_DIRECTION_CLIENT_TO_SERVER;
  else
    outcomingFrameDesc.direction = ZCL_FRAME_CONTROL_DIRECTION_SERVER_TO_CLIENT;
  outcomingFrameDesc.frameType = ZCL_FRAME_CONTROL_FRAME_TYPE_UNIVERSAL_COMMAND;
  outcomingFrameDesc.header = buf->frame + getZclAsduOffset();
  outcomingFrameDesc.payload = buf->frame + getZclAsduOffset() + outcomingFrameDesc.headerLength;
  outcomingFrameDesc.manufacturerSpecific = incomingFrameDesc->manufacturerSpecific;
  outcomingFrameDesc.manufacturerCode = incomingFrameDesc->manufacturerCode;
  outcomingFrameDesc.sequenceNumber = incomingFrameDesc->sequenceNumber;
  outcomingFrameDesc.defaultResponse = ZCL_FRAME_CONTROL_DISABLE_DEFAULT_RESPONSE;

  formZclHeader(&outcomingFrameDesc);
  formServerDataReq(buf, auxData->ind);
  zclApsDataReq(&buf->primitive.apsDataReq, auxData->apsSecurity);
}

/**************************************************************************//**
\brief Process incoming data indication
******************************************************************************/
static void processDataInd(void)
{
  APS_DataInd_t *apsDataInd = getQueueElem(&parserMem.dataIndQueue);

  if (!apsDataInd)
    return;

  if (parseDataInd(apsDataInd))
  {
    deleteHeadQueueElem(&parserMem.dataIndQueue);
    zclMmFreeMem((ZclMmBuffer_t *)apsDataInd);
    parserMem.dataIndAmount--;
  }
}

/*************************************************************************************//**
  \brief Incoming commands validation routine.

  \param[in] ind - pointer to APS data indication parameters.
  \param[in] frameDescriptor - parsed frame data.

  \return Command validation status.
******************************************************************************************/
static ZCL_Status_t validateIncomingCommand(const APS_DataInd_t *ind, const ZclFrameDescriptor_t *frameDescriptor)
{
  uint8_t securityType = zclGetSecurityTypeByClusterId(ind->clusterId, ind->dstEndpoint);
  (void)frameDescriptor;
#ifdef _APS_MULTICAST_
  if (APS_GROUP_ADDRESS == ind->dstAddrMode)
    securityType = ZCL_NETWORK_KEY_CLUSTER_SECURITY;
#endif

  if (ZCL_NETWORK_KEY_CLUSTER_SECURITY == securityType)
  {
    if (APS_UNSECURED_STATUS == ind->securityStatus)
    {
      // Accept commands which are compliant with the security policy
      return ZCL_SUCCESS_STATUS;
    }
    else if (APS_SECURED_WITH_ANY_LINK_KEY_STATUS(ind->securityStatus))
    {
#if CERTICOM_SUPPORT == 1
      if (!APS_AreKeysAuthorized(&ind->srcAddress.extAddress))
        /* The command was encrypted on the APS layer with a non authorized link key */
        return ZCL_NOT_AUTHORIZED_STATUS;
      else
#endif
        return ZCL_SUCCESS_STATUS;
    }
  }
  else if (ZCL_APPLICATION_LINK_KEY_CLUSTER_SECURITY == securityType &&
           APS_SECURED_WITH_ANY_LINK_KEY_STATUS(ind->securityStatus))
  {
#if CERTICOM_SUPPORT == 1
    if (!APS_AreKeysAuthorized(&ind->srcAddress.extAddress))
      /* The command was encrypted on the APS layer with a non authorized link key */
      return ZCL_NOT_AUTHORIZED_STATUS;
    else
#endif
      // Accept commands which are compliant with the security policy
      return ZCL_SUCCESS_STATUS;
  }

  /*
    AMI Profile Spec, r15, Paragraph 5.4.6, page 23.
    If incorrect key usage is found the application shall generate a ZCL
    Default Response, employing the Network Key, with a FAILURE (0x01) status code.
  */
  return ZCL_FAILURE_STATUS;
}

/**************************************************************************//**
\brief Send relevant (non-default) response to request

\param[in] dsc - frame descriptor
\param[in] buf - zcl memory buffer
\param[in] cmd - command to send
******************************************************************************/
static void sendRelevantResponse(ZclFrameDescriptor_t *dsc, ZclMmBuffer_t *buf, APS_DataInd_t *ind, uint8_t cmd)
{
  buf->primitive.apsDataReq.asduLength += dsc->headerLength;

  dsc->commandId = cmd;
  dsc->direction = getDstClusterSideByIncommingCommandDirection(dsc->direction);
  dsc->header = buf->frame + getZclAsduOffset();
  dsc->payload = buf->frame + getZclAsduOffset() + dsc->headerLength;
  //Disable Default Response
  dsc->defaultResponse = ZCL_FRAME_CONTROL_DISABLE_DEFAULT_RESPONSE;
  formZclHeader(dsc);
  formServerDataReq(buf, ind);
  zclApsDataReq(&buf->primitive.apsDataReq, APS_SECURED_WITH_ANY_LINK_KEY_STATUS(ind->securityStatus));
}

/**************************************************************************//**
\brief Checks if specified manufacturer code is supported

\param[in] manufacturerCode - code
******************************************************************************/
static bool isSupportedManufacturerSpecCode(uint16_t  manufacturerCode)
{
  uint16_t mfg_code;
  CS_ReadParameter(CS_MANUFACTURER_CODE_ID, &mfg_code);
  if (mfg_code == manufacturerCode)
    return true;

  return false;
}

/*************************************************************************************//**
  \brief The main incoming packets parse function. Performs the ZCL header checking and
  parses the ZCL command payload according to commandId.

  \param[in] ind - pointer to APS data indication parameters.
  \return true in case if data indication shall be deleted from queue, false otherwise.
******************************************************************************************/
static bool parseDataInd(APS_DataInd_t *ind)
{
  ZclMmBuffer_t *buf;
  uint8_t commandId;
  uint8_t *payloadOut;
  ZclFrameDescriptor_t frameDescriptor;
  ZclAuxParseData_t auxParseData = {.relevantResponse = false, .ind = ind, .frameDescriptor = &frameDescriptor};

  // Check frame and ignore invalid ones
  if (0 == parseZclHeader(&frameDescriptor, ind))
    return true;

  // Check security type and Cluster Usage of Security Keys.
  auxParseData.commandStatus = validateIncomingCommand(ind, &frameDescriptor);

  if (ZCL_SUCCESS_STATUS != auxParseData.commandStatus)
  {
    // Command is invalid or isn't supported - send a default response
    auxParseData.apsSecurity = false;
#if CERTICOM_SUPPORT == 1
    if (ZCL_NOT_AUTHORIZED_STATUS != auxParseData.commandStatus)
#endif
      sendDefaultResponse(&auxParseData);
    return true;
  }

  if (ZCL_FRAME_CONTROL_DIRECTION_CLIENT_TO_SERVER == frameDescriptor.direction &&
      ZCL_FRAME_CONTROL_MANUFACTURER_SPECIFIC == frameDescriptor.manufacturerSpecific &&
      !isSupportedManufacturerSpecCode(frameDescriptor.manufacturerCode))
  {
    auxParseData.commandStatus = (frameDescriptor.frameType != ZCL_FRAME_CONTROL_FRAME_TYPE_UNIVERSAL_COMMAND) ?
                                 ZCL_UNSUP_MANUF_CLUSTER_COMMAND :
                                 ZCL_UNSUP_MANUF_GENERAL_COMMAND_STATUS;
    auxParseData.apsSecurity = APS_SECURED_WITH_ANY_LINK_KEY_STATUS(ind->securityStatus);
    sendDefaultResponse(&auxParseData);
    return true;
  }

  if (frameDescriptor.frameType != ZCL_FRAME_CONTROL_FRAME_TYPE_UNIVERSAL_COMMAND)
  {
    if (ZCL_FRAME_CONTROL_DIRECTION_SERVER_TO_CLIENT == frameDescriptor.direction)
    { // additional checking for responses if confirm has been received already or not.
      ZCL_Request_t *zclReq = zclFindAppropriateRequest(&auxParseData);

      if (zclReq && (ZCL_REQ_FLAG_CONF_RCVD != (zclReq->service.statusflags & ZCL_REQ_RESP_CONF_MASK)))
        return false;
    }

    // Command is cluster specific, send default response if required
    zclCommandInd(&auxParseData);
    auxParseData.apsSecurity = APS_SECURED_WITH_ANY_LINK_KEY_STATUS(ind->securityStatus);
    sendDefaultResponse(&auxParseData);
    return true;
  }

  // Command is universal
  commandId = frameDescriptor.commandId;

  switch (commandId)
  {
    case ZCL_READ_ATTRIBUTES_COMMAND_ID:
    case ZCL_READ_ATTRIBUTES_STRUCTURED_COMMAND_ID:
      {
        uint8_t maxSize;

        CS_ReadParameter(CS_ZCL_BUFFER_SIZE_ID, (void *)&maxSize);

        buf = zclMmGetMem(ZCL_OUTPUT_RESPONSE_BUFFER);  // get buffer for response
        if (!buf)
          return false;
        payloadOut = buf->frame + getZclAsduOffset() + frameDescriptor.headerLength;
        // form response
        buf->primitive.apsDataReq.asduLength = (ZCL_READ_ATTRIBUTES_COMMAND_ID == commandId ?
                                                formReadAttributesResponse :
                                                formReadAttributesStructuredResponse)
                                                (payloadOut,
                                                 maxSize - sizeof(frameDescriptor.headerLength),
                                                 ind,
                                                 &frameDescriptor);
        sendRelevantResponse(&frameDescriptor, buf, ind, ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID);
      }
      break;

    case ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID:
    case ZCL_WRITE_ATTRIBUTES_RESPONSE_COMMAND_ID:
    case ZCL_CONFIGURE_REPORTING_RESPONSE_COMMAND_ID:
    case ZCL_READ_REPORTING_CONFIGURATION_RESPONSE_COMMAND_ID:
    case ZCL_DISCOVER_ATTRIBUTES_RESPONSE_COMMAND_ID:
    case ZCL_DISCOVER_COMMANDS_RECEIVED_RESPONSE_COMMAND_ID:
    case ZCL_DISCOVER_COMMANDS_GENERATED_RESPONSE_COMMAND_ID:
    case ZCL_DISCOVER_ATTRIBUTES_EXTENDED_RESPONSE_COMMAND_ID:
      {
        ZCL_Request_t *zclReq = zclFindAppropriateRequest(&auxParseData);

        if (zclReq && (ZCL_REQ_FLAG_CONF_RCVD != (zclReq->service.statusflags & ZCL_REQ_RESP_CONF_MASK)))
          return false;
        if (!zclReq)
          return true;

        zclResponseInd(&auxParseData, zclReq);
        auxParseData.apsSecurity = APS_SECURED_WITH_ANY_LINK_KEY_STATUS(ind->securityStatus);
        sendDefaultResponse(&auxParseData);
      }
      break;

    case ZCL_WRITE_ATTRIBUTES_NO_RESPONSE_COMMAND_ID:
    case ZCL_WRITE_ATTRIBUTES_COMMAND_ID:
    case ZCL_WRITE_ATTRIBUTES_UNDIVIDED_COMMAND_ID:
      {
        buf = zclMmGetMem(ZCL_OUTPUT_RESPONSE_BUFFER);
        if (!buf)
          return false;

        payloadOut = buf->frame + getZclAsduOffset() + frameDescriptor.headerLength;
        // form response
        buf->primitive.apsDataReq.asduLength = formWriteAttributeResponse((ZCL_WriteAttributeResp_t *)payloadOut,
                                                                           ind,
                                                                           &frameDescriptor,
                                                                           ZCL_WRITE_ATTRIBUTES_UNDIVIDED_COMMAND_ID == commandId);
        // Free allocated memory if no response required
        if (ZCL_WRITE_ATTRIBUTES_NO_RESPONSE_COMMAND_ID == commandId)
        {
          zclMmFreeMem(buf);
          sendDefaultResponse(&auxParseData);
        }
        else
          sendRelevantResponse(&frameDescriptor, buf, ind, ZCL_WRITE_ATTRIBUTES_RESPONSE_COMMAND_ID);
      }
      break;

    case ZCL_DISCOVER_ATTRIBUTES_COMMAND_ID:
      {
        buf = zclMmGetMem(ZCL_OUTPUT_RESPONSE_BUFFER);
        if (!buf)
          return false;
        payloadOut = buf->frame + getZclAsduOffset() + frameDescriptor.headerLength;
        //form response
        buf->primitive.apsDataReq.asduLength = formDiscoverAttributesResponse((ZCL_DiscoverAttributesResp_t *) payloadOut,
                                                                              ind,
                                                                              &frameDescriptor);
        sendRelevantResponse(&frameDescriptor, buf, ind, ZCL_DISCOVER_ATTRIBUTES_RESPONSE_COMMAND_ID);
      }
      break;

    case ZCL_REPORT_ATTRIBUTES_COMMAND_ID:
      zclReportInd(ind, &frameDescriptor);
      sendDefaultResponse(&auxParseData);
      break;

    case ZCL_READ_REPORTING_CONFIGURATION_COMMAND_ID:
      return zclReadReportingConfigurationHandler(ind, &frameDescriptor);

    case ZCL_CONFIGURE_REPORTING_COMMAND_ID:
      return zclConfigureReportingHandler(ind, &frameDescriptor);

    case ZCL_DISCOVER_COMMANDS_RECEIVED_COMMAND_ID:
      {
        buf = zclMmGetMem(ZCL_OUTPUT_RESPONSE_BUFFER);
        if (!buf)
          return false;
        payloadOut = buf->frame + getZclAsduOffset() + frameDescriptor.headerLength;
        //form response
        buf->primitive.apsDataReq.asduLength = formDiscoverCommandsReceivedResponse((ZCL_DiscoverCommandsReceivedResp_t *) payloadOut,
                                                                              ind,
                                                                              &frameDescriptor);
        sendRelevantResponse(&frameDescriptor, buf, ind, ZCL_DISCOVER_COMMANDS_RECEIVED_RESPONSE_COMMAND_ID);
      }
      break;

    case ZCL_DISCOVER_COMMANDS_GENERATED_COMMAND_ID:
      {
        buf = zclMmGetMem(ZCL_OUTPUT_RESPONSE_BUFFER);
        if (!buf)
          return false;
        payloadOut = buf->frame + getZclAsduOffset() + frameDescriptor.headerLength;
        //form response
        buf->primitive.apsDataReq.asduLength = formDiscoverCommandsGeneratedResponse((ZCL_DiscoverCommandsGeneratedResp_t *) payloadOut,
                                                                      ind,
                                                                      &frameDescriptor);
        sendRelevantResponse(&frameDescriptor, buf, ind, ZCL_DISCOVER_COMMANDS_GENERATED_RESPONSE_COMMAND_ID);
      }
      break;

    case ZCL_DISCOVER_ATTRIBUTES_EXTENDED_COMMAND_ID:
      {
        buf = zclMmGetMem(ZCL_OUTPUT_RESPONSE_BUFFER);
        if (!buf)
          return false;
        payloadOut = buf->frame + getZclAsduOffset() + frameDescriptor.headerLength;
        //form response
        buf->primitive.apsDataReq.asduLength = formDiscoverAttributeExtendedsResponse((ZCL_DiscoverAttributesExtendedResp_t *) payloadOut,
                                                                              ind,
                                                                              &frameDescriptor);
        sendRelevantResponse(&frameDescriptor, buf, ind, ZCL_DISCOVER_ATTRIBUTES_EXTENDED_RESPONSE_COMMAND_ID);
      }
      break;

    case ZCL_DEFAULT_RESPONSE_COMMAND_ID:
      return zclReceivedDefaultResponseHandler(&auxParseData);
      break;

    default:
      auxParseData.commandStatus = ZCL_UNSUP_GENERAL_COMMAND_STATUS;
      auxParseData.apsSecurity = APS_SECURED_WITH_ANY_LINK_KEY_STATUS(ind->securityStatus);;
      sendDefaultResponse(&auxParseData);
      break;
  }
  return true;
}

/***************************************************************************//**
\brief Checks, if received command is Default Response command.

\param[in] frameDescriptor - pointer to prepared frame descriptor.

\return True, if received command is Default Response command; false - otherwise.
******************************************************************************/
static inline bool isDefaultResponse(ZclFrameDescriptor_t *frameDescriptor)
{
  return ZCL_FRAME_CONTROL_FRAME_TYPE_UNIVERSAL_COMMAND == frameDescriptor->frameType
    && ZCL_DEFAULT_RESPONSE_COMMAND_ID == frameDescriptor->commandId;
}

/***************************************************************************//**
\brief Handler of received default responses

\param[in]
  auxParseData - pointer to data from recieved frame

\return true if received frame can be released, false - otherwise
******************************************************************************/
static bool zclReceivedDefaultResponseHandler(ZclAuxParseData_t *auxParseData)
{
  APS_DataInd_t *apsData = auxParseData->ind;
  ZclFrameDescriptor_t *frameDesc = auxParseData->frameDescriptor;
  ZCL_Cluster_t *cluster = NULL;

  ZCL_Request_t *zclReq = zclFindAppropriateRequest(auxParseData);

  if (zclReq)
  {
    if (ZCL_REQ_FLAG_CONF_RCVD != (zclReq->service.statusflags & ZCL_REQ_RESP_CONF_MASK))
      return false;
    if (zclReq->service.statusflags & ZCL_REQ_DEFAULT_RESP_RQRD)
    {
      if (!zclReq->ZCL_Notify)
        return true;
      zclReq->notify.id = ZCL_DEFAULT_RESPONSE_ID;
      zclReq->notify.status = ZCL_SUCCESS_STATUS;
      zclReq->ZCL_Notify(&zclReq->notify);
      return true;
    }
  }

  cluster = ZCL_GetCluster(apsData->dstEndpoint,
    apsData->clusterId,
    getOwnClusterSideByIncomingCommandDirection(frameDesc->direction));

  if (cluster)
  {
    ZCL_Addressing_t senderAddressing;

    senderAddressing.addrMode             = apsData->srcAddrMode;
    senderAddressing.addr                 = apsData->srcAddress;
    senderAddressing.profileId            = apsData->profileId;
    senderAddressing.endpointId           = apsData->srcEndpoint;
    senderAddressing.clusterId            = apsData->clusterId;
    senderAddressing.clusterSide          =
      getDstClusterSideByIncommingCommandDirection(frameDesc->direction);
    senderAddressing.sequenceNumber       = frameDesc->sequenceNumber;

    if (ZCL_FRAME_CONTROL_MANUFACTURER_NONSPECIFIC == frameDesc->manufacturerSpecific)
      senderAddressing.manufacturerSpecCode = 0;
    else
      senderAddressing.manufacturerSpecCode = frameDesc->manufacturerCode;

    if (cluster->ZCL_DefaultRespInd)
      cluster->ZCL_DefaultRespInd(&senderAddressing, sizeof(ZCL_DefaultResp_t),
        apsData->asdu + frameDesc->headerLength );
  }

  return true;
}

/***************************************************************************//**
\brief Checking if Default Response is valid for received frame

 2.4.12.2 ZCL specification
 The default response command is generated when a device receives a unicast
 command, there is no other relevant response specified for the command, and
 either an error results or the Disable default response bit of its Frame control field
 is set to 0.

 If a device receives a command in error through a broadcast or multicast
 transmission, the command shall be discarded and the default response command
 shall not be generated.

 The Default Response command should never been send responsing other Default Response
 command.

\param[in] auxData - pointer to auxilliary structure with data for frame formation
\return true in case if Defalt Response is valid for received frame, false otherwise.
******************************************************************************/
static bool zclDefaultResponseIsValid(ZclAuxParseData_t *auxData)
{
  ZclFrameDescriptor_t *incomingFrameDesc = auxData->frameDescriptor;
  APS_DataInd_t *ind = auxData->ind;

  /* Application forces to send a DefaultResponse */
  if (ZCL_SUCCESS_WITH_DEFAULT_RESPONSE_STATUS == auxData->commandStatus)
  {
    auxData->commandStatus = ZCL_SUCCESS_STATUS;
    return true;
  }
  /* No Default Resonse on other Default Response */
  if (isDefaultResponse(incomingFrameDesc))
    return false;
  /* Command received by error, as a result of broadcast or multicast transmission -
     no Default Response */
  if (APS_EXT_ADDRESS == ind->dstAddrMode ||
      APS_GROUP_ADDRESS == ind->dstAddrMode ||
      IS_BROADCAST_ADDR(ind->dstAddress.shortAddress))
    return false;
  /* Default Response SHOULD be sent if error occured during processing or on-demand request */
  if (ZCL_SUCCESS_STATUS != auxData->commandStatus)
    return true;
  /* No Default Response if relevant response existed */
  if (auxData->relevantResponse)
    return false;
  /* Default response if there is no relevant response, no errors and default response is allowed */
  if (ZCL_FRAME_CONTROL_ENABLE_DEFAULT_RESPONSE == incomingFrameDesc->defaultResponse)
    return true;
  /* Otherwise - false */
  return false;
}

/**************************************************************************//**
  \brief Looks for related request for incoming packet

  \param[in] auxParseData - pointer to structure with incoming packet

  \returns pointer to appropriate ZCL request
*****************************************************************************/
static ZCL_Request_t * zclFindAppropriateRequest(ZclAuxParseData_t *auxParseData)
{
  ZCL_Request_t *req;
  ZclMmBufferDescriptor_t *descr = NULL;

  while ((descr = zclMmGetNextOutputMemDescriptor(descr)))
  {
    req = descr->link;
    // check if this is a response
    if (req->dstAddressing.clusterSide != getOwnClusterSideByIncomingCommandDirection(auxParseData->frameDescriptor->direction))
      if (req->dstAddressing.sequenceNumber == auxParseData->frameDescriptor->sequenceNumber)
        if (req->dstAddressing.clusterId == auxParseData->ind->clusterId)
        {
          if (ZCL_REQ_FLAG_CONF_RCVD & req->service.statusflags)
          {
            zclMmFreeMem(&descr->buf);  // free memory located by sent request
            if (NULL == zclMmGetNextOutputMemDescriptor(NULL))
              zclStopResponseWaitTimer();
          }
          else
            req->service.statusflags |= ZCL_REQ_FLAG_RESP_RCVD;

          return req;
        }
  }

  return NULL;
}

#endif // ZCL_SUPPORT == 1
//eof zclParser.c
