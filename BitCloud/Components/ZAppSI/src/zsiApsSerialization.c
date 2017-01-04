/***************************************************************************//**
  \file zsiApsSerialization.c

  \brief ZAppSI APS primitives serialization.
         This file was automatically generated.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   Last change:
    $Id: zsiApsSerialization.c 26492 2014-03-26 10:49:30Z karthik.p_u $
 ******************************************************************************/

/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <zsiApsSerialization.h>
#include <zsiSerializer.h>
#include <sysUtils.h>
#include <zsiDbg.h>

/******************************************************************************
                               Implementation section
 ******************************************************************************/

#ifdef ZAPPSI_HOST

/**************************************************************************//**
  \brief APS-Data.Confirm callback wrapper routine.

  \param[in] req - request parameters.

  \return None.
 ******************************************************************************/
void zsiAPS_DataConfCallback(void *const req)
{
  APS_DataReq_t *request = (APS_DataReq_t *)req;
  request->APS_DataConf(&request->confirm);
}

/**************************************************************************//**
  \brief APS-RegisterEndpoint request primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeAPS_RegisterEndpointReq(const void *const req,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = true,
    .keepMemory = true
  };
  ZsiSerializer_t serializer =
  {
    .pow = cmdFrame->payload,
    .por = cmdFrame->payload
  };
  APS_RegisterEndpointReq_t *const request = (APS_RegisterEndpointReq_t *)req;
  uint16_t length = zsiAPS_RegisterEndpointReqLength(request);
  uint8_t sequenceNumber = zsiGetSequenceNumber();
  uint8_t element;

  zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_SREQ_CMD, ZSI_CMD_APS,
    ZSI_APS_REGISTER_ENDPOINT_REQUEST);

  zsiSerializeUint8(&serializer, request->simpleDescriptor->endpoint);
  zsiSerializeUint16(&serializer, request->simpleDescriptor->AppProfileId);
  zsiSerializeUint16(&serializer, request->simpleDescriptor->AppDeviceId);
  zsiSerializeBitfield(&serializer, request->simpleDescriptor->AppDeviceVersion);
  zsiSerializeUint8(&serializer, request->simpleDescriptor->AppInClustersCount);
  for (element = 0U; element < request->simpleDescriptor->AppInClustersCount; element++)
  {
    zsiSerializeUint16(&serializer, request->simpleDescriptor->AppInClustersList[element]);
  }
  zsiSerializeUint8(&serializer, request->simpleDescriptor->AppOutClustersCount);
  for (element = 0U; element < request->simpleDescriptor->AppOutClustersCount; element++)
  {
    zsiSerializeUint16(&serializer, request->simpleDescriptor->AppOutClustersList[element]);
  }

  return result;
}

/**************************************************************************//**
  \brief APS-RegisterEndpoint confirm primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeAPS_RegisterEndpointConf(void *memory,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = false,
    .keepMemory = true
  };
  ZsiSerializer_t serializer =
  {
    .pow = cmdFrame->payload,
    .por = cmdFrame->payload
  };

  zsiDeserializeUint8(&serializer, memory);

  return result;
}

/**************************************************************************//**
  \brief APS-NextBindingEntry request primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeAPS_NextBindingEntryReq(const void *const req,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = true,
    .keepMemory = true
  };
  ZsiSerializer_t serializer =
  {
    .pow = cmdFrame->payload,
    .por = cmdFrame->payload
  };
  
  ApsBindingEntry_t *const request = (ApsBindingEntry_t *)req;
  uint16_t length = zsiAPS_NextBindingEntryReqLength(request);
  uint8_t sequenceNumber = zsiGetSequenceNumber();

  zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_SREQ_CMD, ZSI_CMD_APS,
    ZSI_APS_NEXT_BINDING_ENTRY_REQUEST);

  zsiSerializeUint64(&serializer, &request->srcAddr);
  zsiSerializeUint8(&serializer, request->srcEndpoint);
  zsiSerializeUint16(&serializer, request->clusterId);
  zsiSerializeUint8(&serializer, request->dstAddrMode);
  if(APS_EXT_ADDRESS == request->dstAddrMode)
  {
    zsiSerializeUint64(&serializer, &request->dst.unicast.extAddr);
    zsiSerializeUint8(&serializer, request->dst.unicast.endpoint);
  }
  else
  {
    zsiSerializeUint16(&serializer, request->dst.group);
  }
  
  return result;  
}

/**************************************************************************//**
  \brief APS-NextBindingEntry confirm primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeAPS_NextBindingEntryConf(void *memory,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = false,
    .keepMemory = true
  };
  ZsiSerializer_t serializer =
  {
    .pow = cmdFrame->payload,
    .por = cmdFrame->payload
  };

  zsiDeserializeUint8(&serializer, memory);

  return result;
}
/**************************************************************************//**
  \brief APS-Data.Indication primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeAPS_DataInd(void *memory,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = false,
    .keepMemory = false
  };
  ZsiSerializer_t serializer =
  {
    .pow = cmdFrame->payload,
    .por = cmdFrame->payload
  };
  APS_RegisterEndpointReq_t *descriptor = NULL;
  APS_DataInd_t *indication = (APS_DataInd_t *)memory;

  indication->dstAddrMode = (APS_AddrMode_t)0;
  indication->srcAddrMode = (APS_AddrMode_t)0;
  indication->status = (APS_Status_t)0;
  indication->securityStatus = (APS_Status_t)0;
  zsiDeserializeUint8(&serializer, (uint8_t *)&indication->dstAddrMode);
  zsiDeserializeUint16(&serializer, &indication->dstAddress.shortAddress);
  zsiDeserializeUint64(&serializer, &indication->dstAddress.extAddress);
  zsiDeserializeUint8(&serializer, &indication->dstEndpoint);
  zsiDeserializeUint8(&serializer, (uint8_t *)&indication->srcAddrMode);
  zsiDeserializeUint16(&serializer, &indication->srcAddress.shortAddress);
  zsiDeserializeUint64(&serializer, &indication->srcAddress.extAddress);
  zsiDeserializeUint16(&serializer, &indication->prevHopAddr);
  zsiDeserializeUint8(&serializer, &indication->srcEndpoint);
  zsiDeserializeUint16(&serializer, &indication->profileId);
  zsiDeserializeUint16(&serializer, &indication->clusterId);
  zsiDeserializeUint16(&serializer, &indication->asduLength);
  zsiDeserializeToPointer(&serializer, &indication->asdu, indication->asduLength);
  zsiDeserializeUint8(&serializer, (uint8_t *)&indication->status);
  zsiDeserializeUint8(&serializer, (uint8_t *)&indication->securityStatus);
  zsiDeserializeUint8(&serializer, (uint8_t *)&indication->nwkSecurityStatus);
  zsiDeserializeUint8(&serializer, &indication->linkQuality);
  zsiDeserializeUint32(&serializer, &indication->rxTime);
  zsiDeserializeUint8(&serializer, (uint8_t *)&indication->rssi);

  /* Indicate to appropriate endpoint */
  descriptor = zsiApsFindEndpoint(indication->dstEndpoint);

  if (descriptor && descriptor->APS_DataInd)
  {
    descriptor->APS_DataInd(indication);
  }

  return result;
}

/**************************************************************************//**
  \brief APS-Data.Request primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeAPS_DataReq(const void *const req,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = true,
    .keepMemory = true
  };
  ZsiSerializer_t serializer =
  {
    .pow = cmdFrame->payload,
    .por = cmdFrame->payload
  };
  APS_DataReq_t *const request = (APS_DataReq_t *)req;
  uint16_t length = zsiAPS_DataReqLength(request);
  uint8_t sequenceNumber = zsiGetSequenceNumber();

  zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_AREQ_CMD, ZSI_CMD_APS,
    ZSI_APS_DATA_REQUEST);

  zsiSerializeUint8(&serializer, request->dstAddrMode);
  zsiSerializeUint16(&serializer, request->dstAddress.shortAddress);
  zsiSerializeUint64(&serializer, &request->dstAddress.extAddress);
  zsiSerializeUint8(&serializer, request->dstEndpoint);
  zsiSerializeUint16(&serializer, request->profileId);
  zsiSerializeUint16(&serializer, request->clusterId);
  zsiSerializeUint8(&serializer, request->srcEndpoint);
  zsiSerializeUint16(&serializer, request->asduLength);
  zsiSerializeData(&serializer, request->asdu, request->asduLength);
  zsiSerializeBitfield(&serializer, request->txOptions.securityEnabledTransmission);
  zsiSerializeBitfield(&serializer, request->txOptions.useNwkKey);
  zsiSerializeBitfield(&serializer, request->txOptions.acknowledgedTransmission);
  zsiSerializeBitfield(&serializer, request->txOptions.fragmentationPermitted);
  zsiSerializeBitfield(&serializer, request->txOptions.includeExtendedNonce);
  zsiSerializeBitfield(&serializer, request->txOptions.doNotDecrypt);
  zsiSerializeBitfield(&serializer, request->txOptions.indicateBroadcasts);
  zsiSerializeUint8(&serializer, request->radius);

  return result;
}

/**************************************************************************//**
  \brief APS-Data.Confirm primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

    \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeAPS_DataConf(void *memory,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = false,
    .keepMemory = false
  };
  APS_DataConf_t *confirm = NULL;
  /* Obtain memory for confirm from bearing request */
  APS_DataReq_t *req =
    zsiDriverReturnBearingEntity(cmdFrame->sequenceNumber);

  (void)memory;

  if (req)
  {
    ZsiSerializer_t serializer =
    {
      .pow = cmdFrame->payload,
      .por = cmdFrame->payload
    };

    confirm = &req->confirm;
    zsiDeserializeUint8(&serializer, (uint8_t *)&confirm->status);
    zsiDeserializeUint32(&serializer, &confirm->txTime);
  }

  /* Callback calling */
  if (confirm)
  {
    req->service.zsi.callback = zsiAPS_DataConfCallback;
    zsiAreqProcessingComplete(req);
  }

  /* Free allocated memory, as it already exists in request */
  return result;
}

#ifdef _BINDING_
/**************************************************************************//**
  \brief APS-Bind.Request primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeAPS_BindReq(const void *const req,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = true,
    .keepMemory = true
  };
  ZsiSerializer_t serializer =
  {
    .pow = cmdFrame->payload,
    .por = cmdFrame->payload
  };
  APS_BindReq_t *const request = (APS_BindReq_t *)req;
  uint16_t length = zsiAPS_BindReqLength(request);
  uint8_t sequenceNumber = zsiGetSequenceNumber();

  zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_SREQ_CMD, ZSI_CMD_APS,
                    ZSI_APS_BIND_REQUEST);

  zsiSerializeUint64(&serializer, &request->srcAddr);
  zsiSerializeUint8(&serializer, request->srcEndpoint);
  zsiSerializeUint16(&serializer, request->clusterId);
  zsiSerializeUint8(&serializer, request->dstAddrMode);
  if (APS_EXT_ADDRESS == request->dstAddrMode)
  {
    zsiSerializeUint64(&serializer, &request->dst.unicast.extAddr);
    zsiSerializeUint8(&serializer, request->dst.unicast.endpoint);
  }
  else
    zsiSerializeUint16(&serializer, request->dst.group);

  return result;
}

/**************************************************************************//**
  \brief APS-Bind.Confirm primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeAPS_BindConf(void *memory,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = false,
    .keepMemory = true
  };
  ZsiSerializer_t serializer =
  {
    .pow = cmdFrame->payload,
    .por = cmdFrame->payload
  };

  zsiDeserializeUint8(&serializer, memory);

  return result;
}

#endif /* _BINDING_ */

#ifdef _LINK_SECURITY_
/**************************************************************************//**
  \brief APS-SetLinkKey request primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

      \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeAPS_SetLinkKeyReq(const void *const req,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = true,
    .keepMemory = true
  };
  ZsiSerializer_t serializer =
  {
    .pow = cmdFrame->payload,
    .por = cmdFrame->payload
  };
  ZsiKeyDescriptor_t *descriptor = (ZsiKeyDescriptor_t *)req;
  uint16_t length = zsiAPS_SetLinkKeyReqLength(NULL);
  uint8_t sequenceNumber = zsiGetSequenceNumber();

  zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_SREQ_CMD, ZSI_CMD_APS,
                    ZSI_APS_SET_LINK_KEY_REQUEST);

  zsiSerializeUint64(&serializer, descriptor->extAddress);
  zsiSerializeData(&serializer, descriptor->key, SECURITY_KEY_SIZE);

  return result;
}

/**************************************************************************//**
  \brief APS-SetLinkKey confirm primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

    \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeAPS_SetLinkKeyConf(void *memory,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = false,
    .keepMemory = true
  };
  ZsiSerializer_t serializer =
  {
    .pow = cmdFrame->payload,
    .por = cmdFrame->payload
  };

  zsiDeserializeUint16(&serializer, memory);

  return result;
}

/**************************************************************************//**
  \brief APS-GetLinkKey request primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

      \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeAPS_GetLinkKeyReq(const void *const req,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = true,
    .keepMemory = true
  };
  ZsiSerializer_t serializer =
  {
    .pow = cmdFrame->payload,
    .por = cmdFrame->payload
  };
  APS_KeyHandle_t *handle = (APS_KeyHandle_t *)req;
  uint16_t length = zsiAPS_SetLinkKeyReqLength(NULL);
  uint8_t sequenceNumber = zsiGetSequenceNumber();
  uint16_t handleU16 = (((uint16_t)handle->flags << 8) & 0xFF00) | handle->idx;

  zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_SREQ_CMD, ZSI_CMD_APS,
                    ZSI_APS_GET_LINK_KEY_REQUEST);

  zsiSerializeUint16(&serializer, handleU16);

  return result;
}

/**************************************************************************//**
  \brief APS-GetLinkKey confirm primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

    \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeAPS_GetLinkKeyConf(void *memory,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = false,
    .keepMemory = true
  };
  ZsiSerializer_t serializer =
  {
    .pow = cmdFrame->payload,
    .por = cmdFrame->payload
  };
  ZsiKeyDescriptor_t *descriptor = (ZsiKeyDescriptor_t *)memory;

  zsiDeserializeUint8(&serializer, &descriptor->keyFond);
  if (descriptor->keyFond)
  {
    zsiDeserializeData(&serializer, descriptor->key, SECURITY_KEY_SIZE);
  }

  return result;
}

/**************************************************************************//**
  \brief APS-FindKeysReq request primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

      \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeAPS_FindKeysReq(const void *const req,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = true,
    .keepMemory = true
  };
  ZsiSerializer_t serializer =
  {
    .pow = cmdFrame->payload,
    .por = cmdFrame->payload
  };
  ExtAddr_t *extAddr = (ExtAddr_t *)req;
  uint16_t length = zsiAPS_FindKeysReqLength(NULL);
  uint8_t sequenceNumber = zsiGetSequenceNumber();

  zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_SREQ_CMD, ZSI_CMD_APS,
                    ZSI_APS_FIND_KEYS_REQUEST);

  zsiSerializeUint64(&serializer, extAddr);

  return result;
}

/**************************************************************************//**
  \brief APS-FindKeys confirm primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

    \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeAPS_FindKeysConf(void *memory,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = false,
    .keepMemory = true
  };
  ZsiSerializer_t serializer =
  {
    .pow = cmdFrame->payload,
    .por = cmdFrame->payload
  };

  zsiDeserializeUint16(&serializer, memory);

  return result;
}

/**************************************************************************//**
  \brief APS-GetKeyPairDeviceAddressReq request primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
******************************************************************************/
ZsiProcessingResult_t zsiSerializeAPS_GetKeyPairDeviceAddressReq(const void *const req,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = true,
    .keepMemory = true
  };
  ZsiSerializer_t serializer =
  {
    .pow = cmdFrame->payload,
    .por = cmdFrame->payload
  };
  APS_KeyHandle_t *handle = (APS_KeyHandle_t *)req;
  uint16_t length = zsiAPS_GetKeyPairDeviceAddressReqLength(NULL);
  uint8_t sequenceNumber = zsiGetSequenceNumber();

  zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_SREQ_CMD, ZSI_CMD_APS,
                    ZSI_APS_GET_KEY_PAIR_DEV_ADDR_REQUEST);

  zsiSerializeUint8(&serializer, handle->flags);
  zsiSerializeUint16(&serializer, handle->idx);

  return result;
}

/**************************************************************************//**
  \brief APS-GetKeyPairDeviceAddress confirm primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
******************************************************************************/
ZsiProcessingResult_t zsiDeserializeAPS_GetKeyPairDeviceAddressConf(void *memory,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = false,
    .keepMemory = true
  };
  ZsiSerializer_t serializer =
  {
    .pow = cmdFrame->payload,
    .por = cmdFrame->payload
  };

  zsiDeserializeUint64(&serializer, (uint64_t *)memory);

  return result;
}

/**************************************************************************//**
  \brief APS-NextKeys request primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
******************************************************************************/
ZsiProcessingResult_t zsiSerializeAPS_NextKeysReq(const void *const req,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = true,
    .keepMemory = true
  };
  ZsiSerializer_t serializer =
  {
    .pow = cmdFrame->payload,
    .por = cmdFrame->payload
  };
  APS_KeyHandle_t *handle = (APS_KeyHandle_t *)req;
  uint16_t length = zsiAPS_NextKeysReqLength(NULL);
  uint8_t sequenceNumber = zsiGetSequenceNumber();

  zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_SREQ_CMD, ZSI_CMD_APS,
                    ZSI_APS_NEXT_KEYS_REQUEST);

  zsiSerializeUint8(&serializer, handle->flags);
  zsiSerializeUint16(&serializer, handle->idx);

  return result;
}

/**************************************************************************//**
  \brief APS-NextKeys confirm primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
******************************************************************************/
ZsiProcessingResult_t zsiDeserializeAPS_NextKeysConf(void *memory,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = false,
    .keepMemory = true
  };
  ZsiSerializer_t serializer =
  {
    .por = cmdFrame->payload,
    .pow = cmdFrame->payload
  };
  APS_KeyHandle_t *handle = (APS_KeyHandle_t *)memory;

  zsiDeserializeUint8(&serializer, &handle->flags);
  zsiDeserializeUint16(&serializer, (uint16_t *)&handle->idx);

  return result;
}

/**************************************************************************//**
  \brief APS-MarkGlobalKeyAsTCKey primitive serialization and
         processing routine.

  \param[in] hndl - pointer to valid key handle.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeAPS_MarkGlobalKeyAsTCKeyReq(const void *const hndl,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = true,
    .keepMemory = true
  };
  ZsiSerializer_t serializer =
  {
    .pow = cmdFrame->payload,
    .por = cmdFrame->payload
  };

  APS_KeyHandle_t *handle = (APS_KeyHandle_t *)hndl;
  uint16_t length = zsiAPS_MarkGlobalKeyAsTCKeyReqLength(handle);
  uint8_t sequenceNumber = zsiGetSequenceNumber();

  zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_SREQ_CMD, ZSI_CMD_APS,
    ZSI_APS_MARK_TC_KEY_REQUEST);

  zsiSerializeUint16(&serializer, handle->idx);
  zsiSerializeUint8(&serializer, handle->flags);

  return result;
}

/**************************************************************************//**
  \brief APS-MarkGlobalKeyAsTCKey confirm primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

    \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeAPS_MarkGlobalKeyAsTCkeyConf(void *memory,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = false,
    .keepMemory = true
  };

  (void)memory;
  (void)cmdFrame;
  return result;
}

/**************************************************************************//**
  \brief APS_SetAuthorizedStatusReq request primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
******************************************************************************/
ZsiProcessingResult_t zsiSerializeAPS_SetAuthorizedStatusReq(const void *const req,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = true,
    .keepMemory = true
  };
  ZsiSerializer_t serializer =
  {
    .pow = cmdFrame->payload,
    .por = cmdFrame->payload
  };

  ZsiSetAuthorizedStatus_t *dataIn = (ZsiSetAuthorizedStatus_t *)req;

  uint16_t length = zsiAPS_SetAuthorizedStatusReq(NULL);
  uint8_t sequenceNumber = zsiGetSequenceNumber();

  zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_SREQ_CMD, ZSI_CMD_APS,
                    ZSI_APS_SET_AUTHORIZED_STATUS_REQUEST);

  zsiSerializeUint64(&serializer, dataIn->extAddress);
  zsiSerializeUint8(&serializer, dataIn->status);

  return result;
}

/**************************************************************************//**
  \brief APS_SetAuthorizedStatus confirm primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
******************************************************************************/
ZsiProcessingResult_t zsiDeserializeAPS_SetAuthorizedStatusConf(void *memory,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = false,
    .keepMemory = true
  };

  (void)memory;
  (void)cmdFrame;
  return result;
}

/**************************************************************************//**
  \brief APS_SetDefaultKeyPairStatusReq request primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
******************************************************************************/
ZsiProcessingResult_t zsiSerializeAPS_SetDefaultKeyPairStatusReq(const void *const req,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = true,
    .keepMemory = true
  };
  ZsiSerializer_t serializer =
  {
    .pow = cmdFrame->payload,
    .por = cmdFrame->payload
  };

  uint16_t length = zsiAPS_SetDefaultKeyPairStatusReq(NULL);
  uint8_t sequenceNumber = zsiGetSequenceNumber();

  zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_SREQ_CMD, ZSI_CMD_APS,
                    ZSI_APS_SET_DEFAULT_KEY_PAIR_STATUS_REQUEST);

  zsiSerializeUint8(&serializer, *(uint8_t *)req);

  return result;
}

/**************************************************************************//**
  \brief APS_SetAuthorizedStatus confirm primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
******************************************************************************/
ZsiProcessingResult_t zsiDeserializeAPS_SetDefaultKeyPairStatusConf(void *memory,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = false,
    .keepMemory = true
  };

  (void)memory;
  (void)cmdFrame;
  return result;
}

/**************************************************************************//**
  \brief APS-AreKeysAuthorizedReq primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeAPS_AreKeysAuthorizedReq(const void *const req,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = true,
    .keepMemory = true // It is not ZAppSi memory, keep for high layer
  };
  ZsiSerializer_t serializer =
  {
    .pow = cmdFrame->payload,
    .por = cmdFrame->payload
  };

  uint16_t length = zsiAPS_AreKeysAuthorizedReq(req);
  uint8_t sequenceNumber = zsiGetSequenceNumber();

  zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_SREQ_CMD, ZSI_CMD_APS,
                    ZSI_APS_ARE_KEYS_AUTHORIZED_REQUEST);

  zsiSerializeUint64(&serializer, req);

  return result;
}

/**************************************************************************//**
  \brief KE-MAC_IsOwnExtAddrConf confirm primitive deserialization and
         processing routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
*******************************************************************************/
ZsiProcessingResult_t zsiDeserializeAPS_AreKeysAuthorizedConf(void *memory,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepMemory = true,
    .keepCmdFrame = false
  };
  ZsiSerializer_t serializer =
  {
    .pow = cmdFrame->payload,
    .por = cmdFrame->payload
  };

  zsiDeserializeUint8(&serializer, memory);

  return result;
}

/**************************************************************************//**
  \brief APS_GetTrustCenterAddressReq primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeAPS_GetTrustCenterAddressReq(const void *const req,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = true,
    .keepMemory = true
  };

  uint16_t length = zsiAPS_GetTrustCenterAddressReq(req);
  uint8_t sequenceNumber = zsiGetSequenceNumber();

  zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_SREQ_CMD, ZSI_CMD_APS,
                    ZSI_APS_GET_TRUST_CENTRE_ADDRESS_REQUEST);


  return result;
}

/**************************************************************************//**
  \brief APS_GetTrustCenterAddress confirm primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
******************************************************************************/
ZsiProcessingResult_t zsiDeserializeAPS_GetTrustCenterAddressConf(void *memory,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = false,
    .keepMemory = true
  };
  ZsiSerializer_t serializer =
  {
    .pow = cmdFrame->payload,
    .por = cmdFrame->payload
  };

  zsiDeserializeUint64(&serializer, (uint64_t *)memory);

  return result;
}

#endif /* _LINK_SECURITY_ */

#elif defined(ZAPPSI_NP)
/**************************************************************************//**
  \brief APS-Data.Indication primitive serialization routine.

  \param[in] ind - indication parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

      \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeAPS_DataInd(const void *const ind,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = true,
    .keepMemory = true
  };
  ZsiSerializer_t serializer =
  {
    .pow = cmdFrame->payload
  };
  APS_DataInd_t *indication = (APS_DataInd_t *)ind;
  uint16_t length = zsiAPS_DataIndLength(indication);
  uint8_t sequenceNumber = zsiGetSequenceNumber();

  zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_AREQ_CMD, ZSI_CMD_APS,
                    ZSI_APS_DATA_INDICATION);

  zsiSerializeUint8(&serializer, indication->dstAddrMode);
  zsiSerializeUint16(&serializer, indication->dstAddress.shortAddress);
  zsiSerializeUint64(&serializer, &indication->dstAddress.extAddress);
  zsiSerializeUint8(&serializer, indication->dstEndpoint);
  zsiSerializeUint8(&serializer, indication->srcAddrMode);
  zsiSerializeUint16(&serializer, indication->srcAddress.shortAddress);
  zsiSerializeUint64(&serializer, &indication->srcAddress.extAddress);
  zsiSerializeUint16(&serializer, indication->prevHopAddr);
  zsiSerializeUint8(&serializer, indication->srcEndpoint);
  zsiSerializeUint16(&serializer, indication->profileId);
  zsiSerializeUint16(&serializer, indication->clusterId);
  zsiSerializeUint16(&serializer, indication->asduLength);
  zsiSerializeData(&serializer, indication->asdu, indication->asduLength);
  zsiSerializeUint8(&serializer, indication->status);
  zsiSerializeUint8(&serializer, indication->securityStatus);
  zsiSerializeUint8(&serializer, indication->nwkSecurityStatus);
  zsiSerializeUint8(&serializer, indication->linkQuality);
  zsiSerializeUint32(&serializer, &indication->rxTime);
  zsiSerializeUint8(&serializer, (uint8_t)indication->rssi);

  return result;
}

/**************************************************************************//**
  \brief APS-Data.Request primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

    \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeAPS_DataReq(void *memory,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = false,
    .keepMemory = true
  };
  ZsiSerializer_t serializer =
  {
    .por = cmdFrame->payload,
  };
  APS_DataReq_t *request = (APS_DataReq_t *)memory;
  union
  {
    uint8_t u8;
    uint8_t *u8Ptr;
  } value;

  zsiDeserializeUint8(&serializer, (uint8_t *)&request->dstAddrMode);
  zsiDeserializeUint16(&serializer, &request->dstAddress.shortAddress);
  zsiDeserializeUint64(&serializer, &request->dstAddress.extAddress);
  zsiDeserializeUint8(&serializer, &request->dstEndpoint);
  zsiDeserializeUint16(&serializer, &request->profileId);
  zsiDeserializeUint16(&serializer, &request->clusterId);
  zsiDeserializeUint8(&serializer, &request->srcEndpoint);
  zsiDeserializeUint16(&serializer, &request->asduLength);
  zsiDeserializeToPointer(&serializer, &request->asdu, request->asduLength);
  zsiDeserializeBitfield(&serializer, &value.u8);
  request->txOptions.securityEnabledTransmission = value.u8;
  zsiDeserializeBitfield(&serializer, &value.u8);
  request->txOptions.useNwkKey = value.u8;
  zsiDeserializeBitfield(&serializer, &value.u8);
  request->txOptions.acknowledgedTransmission = value.u8;
  zsiDeserializeBitfield(&serializer, &value.u8);
  request->txOptions.fragmentationPermitted = value.u8;
  zsiDeserializeBitfield(&serializer, &value.u8);
  request->txOptions.includeExtendedNonce = value.u8;
  zsiDeserializeBitfield(&serializer, &value.u8);
  request->txOptions.doNotDecrypt = value.u8;
  zsiDeserializeBitfield(&serializer, &value.u8);
  request->txOptions.indicateBroadcasts = value.u8;
  request->txOptions.noRouteDiscovery = 0U;
  zsiDeserializeUint8(&serializer, &request->radius);

  /* Put asdu to the end of the request. Reserve place for low level headers */
  value.u8Ptr = (uint8_t *)memory + sizeof(APS_DataReq_t) + APS_ASDU_OFFSET;
  memcpy(value.u8Ptr, request->asdu, request->asduLength);
  request->asdu = value.u8Ptr;
  value.u8Ptr += request->asduLength + (APS_AFFIX_LENGTH - APS_ASDU_OFFSET);
  /* Payload doesn't fit allocated memory */
  sysAssert((uint16_t)(value.u8Ptr - (uint8_t *)memory) < ZSI_MAX_FRAME_PAYLOAD, ZSIAPSSERIALIZATION_ZSIDESERIALIZEAPSDATAREQ0);

  request->APS_DataConf = zsiAPS_DataConf;
  request->service.sequenceNumber = cmdFrame->sequenceNumber;
  APS_DataReq(request);

  return result;
}

/**************************************************************************//**
  \brief APS-Data.Confirm primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

      \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeAPS_DataConf(const void *const req,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = true,
    .keepMemory = false
  };
  ZsiSerializer_t serializer =
  {
    .pow = cmdFrame->payload
  };
  APS_DataReq_t *request = (APS_DataReq_t *)req;
  APS_DataConf_t *confirm = &request->confirm;
  uint16_t length = zsiAPS_DataConfLength(NULL);
  uint8_t sequenceNumber = request->service.sequenceNumber;

  zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_AREQ_CMD, ZSI_CMD_APS,
    ZSI_APS_DATA_CONFIRM);

  zsiSerializeUint8(&serializer, confirm->status);
  zsiSerializeUint32(&serializer, &confirm->txTime);

  return result;
}

/**************************************************************************//**
  \brief APS-RegisterEndpoint request primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

    \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeAPS_RegisterEndpointReq(void *memory,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = true,
    .keepMemory = false
  };
  APS_Status_t status = APS_INVALID_PARAMETER_STATUS;
  ZsiSerializer_t serializer =
  {
    .por = cmdFrame->payload,
    .pow = cmdFrame->payload,
  };
  /* SimpleDescriptor_t is a packed structure, so we can't pass addresses of its
     fields to functions directly. Aligned items used instead. */
  union
  {
    uint8_t u8;
    uint16_t u16;
  } value;
  ZsiEndpointDescriptor_t *descriptor = zsiApsAllocateEndpointMemory();

  (void)memory;

  if (descriptor)
  {
    SimpleDescriptor_t *simpleDescriptor = &descriptor->simpleDescriptor;
    ClusterId_t *inClusterList = descriptor->inClusterList;
    ClusterId_t *outClusterList = descriptor->outClusterList;
    APS_RegisterEndpointReq_t *const request = &descriptor->registerEndpointReq;
    uint8_t element;

    zsiDeserializeUint8(&serializer, &value.u8);
    simpleDescriptor->endpoint = value.u8;
    zsiDeserializeUint16(&serializer, &value.u16);
    simpleDescriptor->AppProfileId = value.u16;
    zsiDeserializeUint16(&serializer, &value.u16);
    simpleDescriptor->AppDeviceId = value.u16;
    zsiDeserializeBitfield(&serializer, &value.u8);
    simpleDescriptor->AppDeviceVersion = value.u8;
    simpleDescriptor->Reserved = 0U;
    zsiDeserializeUint8(&serializer, &value.u8);
    simpleDescriptor->AppInClustersCount = value.u8;

    for (element = 0U; element < simpleDescriptor->AppInClustersCount; element++)
    {
      zsiDeserializeUint16(&serializer, &value.u16);
      inClusterList[element] = value.u16;
    }

    zsiDeserializeUint8(&serializer, &value.u8);
    simpleDescriptor->AppOutClustersCount = value.u8;

    for (element = 0U; element < simpleDescriptor->AppOutClustersCount; element++)
    {
      zsiDeserializeUint16(&serializer, &value.u16);
      outClusterList[element] = value.u16;
    }

    request->APS_DataInd = APS_DataInd;
    request->simpleDescriptor = simpleDescriptor;
    request->simpleDescriptor->AppInClustersList = inClusterList;
    request->simpleDescriptor->AppOutClustersList = outClusterList;

    APS_RegisterEndpointReq(request);

    status = request->status;
  }

  /* Prepare and send confirm at the same buffer as request */
  {
    uint16_t length = zsiAPS_RegisterEndpointConfLength(NULL);
    uint8_t sequenceNumber = cmdFrame->sequenceNumber;

    zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_SRSP_CMD, ZSI_CMD_APS,
                      ZSI_APS_REGISTER_ENDPOINT_CONFIRM);

    zsiSerializeUint8(&serializer, (uint8_t)status);
  }

  return result;
}

/**************************************************************************//**
  \brief APS-NextBindingEntry request primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeAPS_NextBindingEntryReq(void *memory,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = true,
    .keepMemory = false
  };
  ZsiSerializer_t serializer =
  {
    .pow = cmdFrame->payload,
    .por = cmdFrame->payload
  };
  ApsBindingEntry_t *request = (ApsBindingEntry_t *)memory;
  ApsBindingEntry_t *nextEntry = NULL;

  zsiDeserializeUint64(&serializer, &request->srcAddr);
  zsiDeserializeUint8(&serializer, &request->srcEndpoint);
  zsiDeserializeUint16(&serializer, &request->clusterId);
  zsiDeserializeUint8(&serializer, (uint8_t *)&request->dstAddrMode);

  if (APS_EXT_ADDRESS == request->dstAddrMode)
  {
    zsiDeserializeUint64(&serializer, &request->dst.unicast.extAddr);
    zsiDeserializeUint8(&serializer, &request->dst.unicast.endpoint);
  }
  else
    zsiDeserializeUint16(&serializer, &request->dst.group);

  nextEntry = APS_NextBindingEntry(request);

  /* Prepare and send confirm frame at the same buffer, as request */
  {
    uint16_t length = zsiAPS_NextBindingEntryConfLength(NULL);
    uint8_t sequenceNumber = cmdFrame->sequenceNumber;

    zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_SRSP_CMD, ZSI_CMD_APS,
      ZSI_APS_NEXT_BINDING_ENTRY_CONFIRM);
   
    zsiSerializeUint64(&serializer, &nextEntry->srcAddr);
    zsiSerializeUint8(&serializer, nextEntry->srcEndpoint);
    zsiSerializeUint16(&serializer, nextEntry->clusterId);
    zsiSerializeUint8(&serializer, request->dstAddrMode);

    if (APS_EXT_ADDRESS == request->dstAddrMode)
    {
      zsiSerializeUint64(&serializer, &request->dst.unicast.extAddr);
      zsiSerializeUint8(&serializer, request->dst.unicast.endpoint);
    }
    else
      zsiSerializeUint16(&serializer, request->dst.group);
  }
  
  return result; 
}


#ifdef _BINDING_
/**************************************************************************//**
  \brief APS-Bind.Request primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeAPS_BindReq(void *memory,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = true,
    .keepMemory = false
  };
  ZsiSerializer_t serializer =
  {
    .pow = cmdFrame->payload,
    .por = cmdFrame->payload
  };
  APS_BindReq_t *request = (APS_BindReq_t *)memory;

  zsiDeserializeUint64(&serializer, &request->srcAddr);
  zsiDeserializeUint8(&serializer, &request->srcEndpoint);
  zsiDeserializeUint16(&serializer, &request->clusterId);
  zsiDeserializeUint8(&serializer, (uint8_t *)&request->dstAddrMode);

  if (APS_EXT_ADDRESS == request->dstAddrMode)
  {
    zsiDeserializeUint64(&serializer, &request->dst.unicast.extAddr);
    zsiDeserializeUint8(&serializer, &request->dst.unicast.endpoint);
  }
  else
    zsiDeserializeUint16(&serializer, &request->dst.group);

  APS_BindReq(request);

  /* Prepare and send confirm frame at the same buffer, as request */
  {
    uint16_t length = zsiAPS_BindConfLength(NULL);
    uint8_t sequenceNumber = cmdFrame->sequenceNumber;

    zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_SRSP_CMD, ZSI_CMD_APS,
      ZSI_APS_BIND_CONFIRM);

    zsiSerializeUint8(&serializer, request->confirm.status);
  }

  return result;
}

#endif /* _BINDING_ */

#ifdef _LINK_SECURITY_
/**************************************************************************//**
  \brief APS-SetLinkKey request primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

    \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeAPS_SetLinkKeyReq(void *memory,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = true,
    .keepMemory = false
  };
  ZsiSerializer_t serializer =
  {
    .por = cmdFrame->payload,
    .pow = cmdFrame->payload,
  };
  union
  {
    uint64_t u64;
  } value;
  APS_KeyHandle_t handle;

  zsiDeserializeUint64(&serializer, &value.u64);
  zsiDeserializeData(&serializer, memory, SECURITY_KEY_SIZE);

  handle = APS_SetLinkKey(&value.u64, memory);

  /* Prepare and send confirm at the same buffer as request */
  {
    uint16_t length = zsiAPS_SetLinkKeyConfLength(NULL);
    uint8_t sequenceNumber = cmdFrame->sequenceNumber;
    uint16_t handleU16 = (((uint16_t)handle.flags << 8) & 0xFF00) | handle.idx;

    zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_SRSP_CMD, ZSI_CMD_APS,
      ZSI_APS_SET_LINK_KEY_CONFIRM);

    zsiSerializeUint16(&serializer, handleU16);
  }

  return result;
}

/**************************************************************************//**
  \brief APS-GetLinkKey request primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

    \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeAPS_GetLinkKeyReq(void *memory,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = true,
    .keepMemory = false
  };
  ZsiSerializer_t serializer =
  {
    .por = cmdFrame->payload,
    .pow = cmdFrame->payload,
  };
  uint8_t *key = NULL;
  union
  {
    APS_KeyHandle_t handle;
    uint8_t keyFond;
  } stuff;

  (void)memory;

  zsiDeserializeUint16(&serializer, (uint16_t *)&stuff.handle);

  key = APS_GetLinkKey(stuff.handle);

  /* Prepare and send confirm at the same buffer as request */
  {
    uint16_t length = zsiAPS_GetLinkKeyConfLength(NULL);
    uint8_t sequenceNumber = cmdFrame->sequenceNumber;

    stuff.keyFond = 0U;
    if (key)
      stuff.keyFond = 1U;

    zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_SRSP_CMD, ZSI_CMD_APS,
      ZSI_APS_GET_LINK_KEY_CONFIRM);

    zsiSerializeUint8(&serializer, stuff.keyFond);
    if (key)
    {
      zsiSerializeData(&serializer, key, SECURITY_KEY_SIZE);
    }
  }

  return result;
}

/**************************************************************************//**
  \brief APS-FindKeys request primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

    \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeAPS_FindKeysReq(void *memory,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = true,
    .keepMemory = false
  };
  ZsiSerializer_t serializer =
  {
    .por = cmdFrame->payload,
    .pow = cmdFrame->payload,
  };
  ExtAddr_t *extAddr = (ExtAddr_t *)memory;
  APS_KeyHandle_t handle;

  zsiDeserializeUint64(&serializer, extAddr);

  handle = APS_FindKeys(extAddr);

  /* Prepare and send confirm at the same buffer as request */
  {
    uint16_t length = zsiAPS_FindKeysConfLength(NULL);
    uint8_t sequenceNumber = cmdFrame->sequenceNumber;
    uint16_t handleU16 = (((uint16_t)handle.flags << 8) & 0xFF00) | handle.idx;

    zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_SRSP_CMD, ZSI_CMD_APS,
      ZSI_APS_FIND_KEYS_CONFIRM);

    zsiSerializeUint16(&serializer, handleU16);
  }

  return result;
}

/**************************************************************************//**
  \brief APS-GetKeyPairDeviceAddress request primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
******************************************************************************/
ZsiProcessingResult_t zsiDeserializeAPS_GetKeyPairDeviceAddressReq(void *memory,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = true,
    .keepMemory = false
  };
  ZsiSerializer_t serializer =
  {
    .por = cmdFrame->payload,
    .pow = cmdFrame->payload,
  };
  APS_KeyHandle_t *handle = (APS_KeyHandle_t *)memory;
  ExtAddr_t *extAddr;

  zsiDeserializeUint8(&serializer, &handle->flags);
  zsiDeserializeUint16(&serializer, (uint16_t *)&handle->idx);

  extAddr = APS_GetKeyPairDeviceAddress(*handle);

  /* Prepare and send confirm at the same buffer as request */
  {
    uint16_t length = zsiAPS_GetKeyPairDeviceAddressConfLength(NULL);
    uint8_t sequenceNumber = cmdFrame->sequenceNumber;

    zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_SRSP_CMD, ZSI_CMD_APS,
      ZSI_APS_GET_KEY_PAIR_DEV_ADDR_CONFIRM);

    zsiSerializeUint64(&serializer, extAddr);
  }

  return result;
}

/**************************************************************************//**
  \brief APS-NextKeys request primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
******************************************************************************/
ZsiProcessingResult_t zsiDeserializeAPS_NextKeysReq(void *memory,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = true,
    .keepMemory = false
  };
  ZsiSerializer_t serializer =
  {
    .por = cmdFrame->payload,
    .pow = cmdFrame->payload,
  };
  APS_KeyHandle_t *handle = (APS_KeyHandle_t *)memory;

  zsiDeserializeUint8(&serializer, &handle->flags);
  zsiDeserializeUint16(&serializer, (uint16_t *)&handle->idx);

  *handle = APS_NextKeys(*handle);

  /* Prepare and send confirm at the same buffer as request */
  {
    uint16_t length = zsiAPS_NextKeysConfLength(NULL);
    uint8_t sequenceNumber = cmdFrame->sequenceNumber;

    zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_SRSP_CMD, ZSI_CMD_APS,
      ZSI_APS_NEXT_KEYS_CONFIRM);

    zsiSerializeUint8(&serializer, handle->flags);
    zsiSerializeUint16(&serializer, handle->idx);
  }

  return result;
}

/**************************************************************************//**
  \brief Request to mark specified global key as 'TC key', deserialization and
         processing routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeAPS_MarkGlobalKeyAsTCKeyReq(void *memory,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = false,
    .keepMemory = false
  };
  ZsiSerializer_t serializer =
  {
    .por = cmdFrame->payload,
  };

  APS_KeyHandle_t *handle = (APS_KeyHandle_t *)memory;

  zsiDeserializeUint16(&serializer, (uint16_t *)&handle->idx);
  zsiDeserializeUint8(&serializer, &handle->flags);

  APS_MarkGlobalKeyAsTCKey(*handle);

  {
    uint16_t length = zsiAPS_MarkGlobalKeyAsTCKeyConfLength();
    uint8_t sequenceNumber = cmdFrame->sequenceNumber;

    zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_SRSP_CMD, ZSI_CMD_APS,
      ZSI_APS_MARK_TC_KEY_CONFIRM);
  }

  return result;
}

/**************************************************************************//**
  \brief APS_SetAuthorizedStatus request primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
******************************************************************************/
ZsiProcessingResult_t zsiDeserializeAPS_SetAuthorizedStatusReq(void *memory,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = false,
    .keepMemory = false
  };
  ZsiSerializer_t serializer =
  {
    .por = cmdFrame->payload,
    .pow = cmdFrame->payload,
  };
  ZsiSetAuthorizedStatus_t *data = (ZsiSetAuthorizedStatus_t *)memory;

  zsiDeserializeUint64(&serializer, (uint64_t *)data->extAddress);
  zsiDeserializeUint8(&serializer, (uint8_t *)&data->status);

  APS_SetAuthorizedStatus(data->extAddress, data->status);

  /* Prepare and send confirm at the same buffer as request */
  {
    uint16_t length = zsiAPS_SetAuthorizedStatusConf(NULL);
    uint8_t sequenceNumber = cmdFrame->sequenceNumber;

    zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_SRSP_CMD, ZSI_CMD_APS,
                      ZSI_APS_SET_AUTHORIZED_STATUS_CONFIRM);
  }

  return result;
}

/**************************************************************************//**
  \brief APS_SetAuthorizedStatus request primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
******************************************************************************/
ZsiProcessingResult_t zsiDeserializeAPS_SetDefaultKeyPairStatusReq(void *memory,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = false,
    .keepMemory = false
  };
  ZsiSerializer_t serializer =
  {
    .por = cmdFrame->payload,
    .pow = cmdFrame->payload,
  };

  zsiDeserializeUint8(&serializer, (uint8_t *)memory);
  APS_SetDefaultKeyPairStatus(*(APS_KeyPairFlags_t *)memory);

  /* Prepare and send confirm at the same buffer as request */
  {
    uint16_t length = zsiAPS_SetAuthorizedStatusConf(NULL);
    uint8_t sequenceNumber = cmdFrame->sequenceNumber;

    zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_SRSP_CMD, ZSI_CMD_APS,
                      ZSI_APS_SET_DEFAULT_KEY_PAIR_STATUS_CONFIRM);
  }

  return result;
}

/**************************************************************************//**
  \brief APS_AreKeysAuthorizedReq request primitive deserialization and
         processing routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
*******************************************************************************/
ZsiProcessingResult_t zsiDeserializeAPS_AreKeysAuthorizedReq(void *memory,
  ZsiCommandFrame_t *const cmdFrame)
{
  ExtAddr_t *dataIn = (ExtAddr_t *)memory;
  bool tmpValue;
  ZsiProcessingResult_t result =
  {
    .keepMemory = false,
    .keepCmdFrame = false
  };
  ZsiSerializer_t serializer =
  {
    .pow = cmdFrame->payload,
    .por = cmdFrame->payload
  };

  zsiDeserializeUint64(&serializer, dataIn);

  tmpValue = APS_AreKeysAuthorized(dataIn);
  serializer.pow = cmdFrame->payload;
  serializer.por = cmdFrame->payload;

  // Prepare and send confirm frame at the same buffer, as request
  {
    uint16_t length = zsiAPS_AreKeysAuthorizedConf(NULL);
    uint8_t sequenceNumber = cmdFrame->sequenceNumber;

    zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_SRSP_CMD, ZSI_CMD_APS,
                      ZSI_APS_ARE_KEYS_AUTHORIZED_CONFIRM);

    zsiSerializeUint8(&serializer, tmpValue);
  }

  return result;
}

/**************************************************************************//**
  \brief APS_GetTrustCenterAddressReq request primitive deserialization and
         processing routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
*******************************************************************************/
ZsiProcessingResult_t zsiDeserializeAPS_GetTrustCenterAddressReq(void *memory,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepMemory = false,
    .keepCmdFrame = false
  };
  ZsiSerializer_t serializer =
  {
    .pow = cmdFrame->payload,
    .por = cmdFrame->payload
  };

  ExtAddr_t *tcAddr = APS_GetTrustCenterAddress();

  // Prepare and send confirm frame at the same buffer, as request
  {
    uint16_t length = zsiAPS_GetTrustCenterAddressConf(NULL);
    uint8_t sequenceNumber = cmdFrame->sequenceNumber;

    zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_SRSP_CMD, ZSI_CMD_APS,
                      ZSI_APS_GET_TRUST_CENTRE_ADDRESS_CONFIRM);

    zsiSerializeUint64(&serializer, tcAddr);
  }
  
  (void)memory;

  return result;
}

#endif /* _LINK_SECURITY_ */

#endif /* ZAPPSI_NP */

/* eof zsiApsSerialization.c */
