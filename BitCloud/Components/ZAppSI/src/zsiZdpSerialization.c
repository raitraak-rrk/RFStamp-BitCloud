/***************************************************************************//**
  \file zsiZdpSerialization.c

  \brief ZAppSI ZDP primitives serialization. This file was automatically
         generated.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   Last change:
    $Id: zsiZdpSerialization.c 28222 2015-07-24 11:38:43Z unithra.c $
 ******************************************************************************/

/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <zsiZdpSerialization.h>
#include <zsiZdoSerialization.h>
#include <zsiSerializer.h>
#include <sysUtils.h>

/******************************************************************************
                               Prototypes section
 ******************************************************************************/
#ifdef ZAPPSI_HOST
uint16_t zsiSerializeZdpReqCommon(ZsiSerializer_t *const serializer,
  const ZDO_ZdpReq_t *const req, ZsiCommandFrame_t *const cmdFrame);
void zsiDeserializeZdpRespCommon(ZsiSerializer_t *const serializer,
  ZDO_ZdpResp_t *const resp);
#else
void zsiDeserializeZdpReqCommon(ZsiSerializer_t *const serializer,
  ZDO_ZdpReq_t *const req);
uint16_t zsiSerializeZdpRespCommon(ZsiSerializer_t *const serializer,
  const ZDO_ZdpResp_t *const resp, ZsiCommandFrame_t *const cmdFrame);
#endif /* ZAPPSI_HOST */

/******************************************************************************
                               Implementation section
 ******************************************************************************/

#ifdef ZAPPSI_HOST
/**************************************************************************//**
  \brief Routine to serialize common part of all ZDP requests.

  \param[out] serializer - pointer to serializer.
  \param[in] req - ZDP request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Serialized part length.
 ******************************************************************************/
uint16_t zsiSerializeZdpReqCommon(ZsiSerializer_t *const serializer,
  const ZDO_ZdpReq_t *const req, ZsiCommandFrame_t *const cmdFrame)
{
  uint8_t *startingPoint = serializer->pow;

  (void)cmdFrame;
  zsiSerializeUint16(serializer, req->reqCluster);
  zsiSerializeUint8(serializer, req->dstAddrMode);
  if (APS_EXT_ADDRESS == req->dstAddrMode)
    zsiSerializeUint64(serializer, &req->dstAddress.extAddress);
  else
    zsiSerializeUint16(serializer, req->dstAddress.shortAddress);

  return serializer->pow - startingPoint;
}

/**************************************************************************//**
  \brief Routine to deserialize common part of all ZDP responses.

  \param[out] serializer - pointer to serializer.
  \param[in] resp - pointer to ZDP response.

  \return None.
 ******************************************************************************/
void zsiDeserializeZdpRespCommon(ZsiSerializer_t *const serializer,
  ZDO_ZdpResp_t *const resp)
{
  union
  {
    uint8_t  u8;
    uint16_t u16;
    uint64_t u64;
  } value;

  zsiDeserializeUint8(serializer, &value.u8);
  resp->srcAddrMode = value.u8;
  if (APS_EXT_ADDRESS == resp->srcAddrMode)
  {
    zsiDeserializeUint64(serializer, &value.u64);
    COPY_64BIT_VALUE(resp->srcAddress.extAddress, value.u64);
  }
  else
  {
    zsiDeserializeUint16(serializer, &value.u16);
    resp->srcAddress.shortAddress = value.u16;
  }

  zsiDeserializeUint8(serializer, &value.u8);
  resp->respPayload.seqNum = value.u8;
  zsiDeserializeUint8(serializer, &value.u8);
  resp->respPayload.status = value.u8;
}

/**************************************************************************//**
  \brief ZDP-IeeeAddress.Reqest primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeZdpIeeeAddressReq(const void *const req,
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
  ZDO_ZdpReq_t *zdpRequest = (ZDO_ZdpReq_t *)req;
  ZDO_IeeeAddrReq_t *ieeeRequest = &zdpRequest->req.reqPayload.ieeeAddrReq;
  uint8_t sequenceNumber = zsiGetSequenceNumber();
  uint16_t length = zsiSerializeZdpReqCommon(&serializer, zdpRequest, cmdFrame);

  length += zsiZdpIeeeAddressReqLength(NULL);

  zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_AREQ_CMD, ZSI_CMD_ZDP,
    ZSI_ZDP_IEEE_ADDRESS_REQUEST);

  zsiSerializeUint16(&serializer, ieeeRequest->nwkAddrOfInterest);
  zsiSerializeUint8(&serializer, ieeeRequest->reqType);
  zsiSerializeUint8(&serializer, ieeeRequest->startIndex);

  return result;
}

/**************************************************************************//**
  \brief ZDP-IeeeAddress.Response primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeZdpIeeeAddressResp(void *memory,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = false,
    .keepMemory = false
  };
  ZDO_IeeeAddrResp_t *ieeeResponse = NULL;
  /* Obtain memory for confirm from bearing request */
  ZDO_ZdpReq_t *request =
    zsiDriverReturnBearingEntity(cmdFrame->sequenceNumber);

  (void)memory;

  if (request)
  {
    ZsiSerializer_t serializer =
    {
      .por = cmdFrame->payload
    };
    union
    {
      uint8_t u8;
      uint16_t u16;
      uint64_t u64;
    } value;
    uint8_t it;

    ieeeResponse = &request->resp.respPayload.ieeeAddrResp;
    zsiDeserializeZdpRespCommon(&serializer, &request->resp);
    zsiDeserializeUint64(&serializer, &value.u64);
    ieeeResponse->ieeeAddrRemote = value.u64;
    zsiDeserializeUint16(&serializer, &value.u16);
    ieeeResponse->nwkAddrRemote = value.u16;
    zsiDeserializeUint8(&serializer, &value.u8);
    ieeeResponse->numAssocDev = value.u8;
    zsiDeserializeUint8(&serializer, &value.u8);
    ieeeResponse->startIndex = value.u8;
    for (it = 0U; it < ieeeResponse->numAssocDev; it++)
    {
      zsiDeserializeUint16(&serializer, &value.u16);
      ieeeResponse->nwkAddrAssocDevList[it] = value.u16;
    }
  }

  /* Callback calling */
  if (ieeeResponse)
  {
    request->service.zsi.callback = zsiZDO_ZdpRespCallback;
    zsiAreqProcessingComplete(request);
  }

  return result;
}

/**************************************************************************//**
  \brief ZDP-PermitJoining.Request primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeZdpPermitJoiningReq(const void *const req,
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
  ZDO_ZdpReq_t *zdpRequest = (ZDO_ZdpReq_t *)req;
  ZDO_MgmtPermitJoiningReq_t *permitJoining = &zdpRequest->req.reqPayload.mgmtPermitJoiningReq;
  uint8_t sequenceNumber = zsiGetSequenceNumber();
  uint16_t length = zsiSerializeZdpReqCommon(&serializer, zdpRequest, cmdFrame);

  length += zsiZdpPermitJoiningReqLength(NULL);

  zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_AREQ_CMD, ZSI_CMD_ZDP,
    ZSI_ZDP_PERMIT_JOINING_REQUEST);

  zsiSerializeUint8(&serializer, permitJoining->permitDuration);
  zsiSerializeUint8(&serializer, permitJoining->tcSignificance);

  return result;
}

/**************************************************************************//**
  \brief ZDP-PermitJoining.Response primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeZdpPermitJoiningResp(void *memory,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = false,
    .keepMemory = false
  };
  /* Obtain memory for confirm from bearing request */
  ZDO_ZdpReq_t *request =
    zsiDriverReturnBearingEntity(cmdFrame->sequenceNumber);

  (void)memory;

  if (request)
  {
    ZsiSerializer_t serializer =
    {
      .por = cmdFrame->payload
    };

    zsiDeserializeZdpRespCommon(&serializer, &request->resp);

    request->service.zsi.callback = zsiZDO_ZdpRespCallback;
    zsiAreqProcessingComplete(request);
  }

  return result;
}

/**************************************************************************//**
  \brief ZDP-SimpleDescriptor.Request primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeZdpSimpleDescriptorReq(const void *const req,
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
  ZDO_ZdpReq_t *zdpRequest = (ZDO_ZdpReq_t *)req;
  ZDO_SimpleDescReq_t *simpleDescRequest = &zdpRequest->req.reqPayload.simpleDescReq;
  uint8_t sequenceNumber = zsiGetSequenceNumber();
  uint16_t length = zsiSerializeZdpReqCommon(&serializer, zdpRequest, cmdFrame);

  length += zsiZdpSimpleDescriptorReqLength();

  zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_AREQ_CMD, ZSI_CMD_ZDP,
    ZSI_ZDP_SIMPLE_DESCRIPTOR_REQUEST);

  zsiSerializeUint16(&serializer, simpleDescRequest->nwkAddrOfInterest);
  zsiSerializeUint8(&serializer, simpleDescRequest->endpoint);

  return result;
}

/**************************************************************************//**
  \brief ZDP-SimpleDescriptorResp primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeZdpSimpleDescriptorResp(void *memory,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = false,
    .keepMemory = false
  };
  ZDO_SimpleDescResp_t *simpleDescResponse = NULL;
  /* Obtain memory for confirm from bearing request */
  ZDO_ZdpReq_t *request =
    zsiDriverReturnBearingEntity(cmdFrame->sequenceNumber);

  (void)memory;

  if (request)
  {
    ZsiSerializer_t serializer =
    {
      .por = cmdFrame->payload
    };
    union
    {
      uint8_t u8;
      uint16_t u16;
    } value;
    uint8_t it;

    simpleDescResponse = &request->resp.respPayload.simpleDescResp;
    zsiDeserializeZdpRespCommon(&serializer, &request->resp);
    zsiDeserializeUint16(&serializer, &value.u16);
    simpleDescResponse->nwkAddrOfInterest = value.u16;
    zsiDeserializeUint8(&serializer, &value.u8);
    simpleDescResponse->length = value.u8;
    zsiDeserializeUint8(&serializer, &value.u8);
    simpleDescResponse->simpleDescriptor.endpoint = value.u8;
    zsiDeserializeUint16(&serializer, &value.u16);
    simpleDescResponse->simpleDescriptor.AppProfileId = value.u16;
    zsiDeserializeUint16(&serializer, &value.u16);
    simpleDescResponse->simpleDescriptor.AppDeviceId = value.u16;
    zsiDeserializeUint8(&serializer, &value.u8);
    simpleDescResponse->simpleDescriptor.AppDeviceVersion = value.u8;
    zsiDeserializeUint8(&serializer, &value.u8);
    simpleDescResponse->simpleDescriptor.AppInClustersCount = value.u8;
    for (it = 0U; it < simpleDescResponse->simpleDescriptor.AppInClustersCount; it++)
    {
      zsiDeserializeUint16(&serializer, &value.u16);
      simpleDescResponse->simpleDescriptor.AppInClustersList[it] = value.u16;
    }
    zsiDeserializeUint8(&serializer, &value.u8);
    simpleDescResponse->simpleDescriptor.AppOutClustersCount = value.u8;
    for (it = 0U; it < simpleDescResponse->simpleDescriptor.AppOutClustersCount; it++)
    {
      zsiDeserializeUint16(&serializer, &value.u16);
      simpleDescResponse->simpleDescriptor.AppOutClustersList[it] = value.u16;
    }
  }

  /* Callback calling */
  if (simpleDescResponse)
  {
    request->service.zsi.callback = zsiZDO_ZdpRespCallback;
    zsiAreqProcessingComplete(request);
  }

  return result;
}

#ifdef _BINDING_
/**************************************************************************//**
  \brief ZDP-MatchDescriptor.Request primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeZdpMatchDescriptorReq(const void *const req,
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
  ZDO_ZdpReq_t *zdpRequest = (ZDO_ZdpReq_t *)req;
  ZDO_MatchDescReq_t *matchRequest = &zdpRequest->req.reqPayload.matchDescReq;
  uint8_t sequenceNumber = zsiGetSequenceNumber();
  uint16_t length = zsiSerializeZdpReqCommon(&serializer, zdpRequest, cmdFrame);
  uint8_t i;

  length += zsiZdpMatchDescriptorReqLength(matchRequest);

  zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_AREQ_CMD, ZSI_CMD_ZDP,
    ZSI_ZDP_MATCH_DESCRIPTOR_REQUEST);

  zsiSerializeUint16(&serializer, matchRequest->nwkAddrOfInterest);
  zsiSerializeUint16(&serializer, matchRequest->profileId);
  zsiSerializeUint8(&serializer, matchRequest->numInClusters);
  for (i = 0U; i < matchRequest->numInClusters; i++)
  {
    zsiSerializeUint16(&serializer, matchRequest->inClusterList[i]);
  }
  zsiSerializeUint8(&serializer, matchRequest->numOutClusters);
  for (i = 0U; i < matchRequest->numOutClusters; i++)
  {
    zsiSerializeUint16(&serializer, matchRequest->outClusterList[i]);
  }

  return result;
}

/**************************************************************************//**
  \brief ZDP-MatchDescriptorResp primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeZdpMatchDescriptorResp(void *memory,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = false,
    .keepMemory = false
  };
  ZDO_MatchDescResp_t *matchResponse = NULL;
  /* Obtain memory for confirm from bearing request */
  ZDO_ZdpReq_t *request =
    zsiDriverReturnBearingEntity(cmdFrame->sequenceNumber);

  (void)memory;

  if (request)
  {
    ZsiSerializer_t serializer =
    {
      .por = cmdFrame->payload
    };
    union
    {
      uint8_t u8;
      uint16_t u16;
    } value;
    uint8_t it;

    matchResponse = &request->resp.respPayload.matchDescResp;
    zsiDeserializeZdpRespCommon(&serializer, &request->resp);
    zsiDeserializeUint16(&serializer, &value.u16);
    matchResponse->nwkAddrOfInterest = value.u16;
    zsiDeserializeUint8(&serializer, &value.u8);
    matchResponse->matchLength = value.u8;
    for (it = 0U; it < matchResponse->matchLength; it++)
    {
      zsiDeserializeUint8(&serializer, &value.u8);
      matchResponse->matchList[it] = value.u8;
    }
  }

  /* Callback calling */
  if (matchResponse)
  {
    request->service.zsi.callback = zsiZDO_ZdpRespCallback;
    zsiAreqProcessingComplete(request);
  }

  /* Keep request context until all matches received. */
  if (ZDO_CMD_COMPLETED_STATUS != request->resp.respPayload.status)
    zsiDriverKeepBearingEntity(request, cmdFrame->sequenceNumber);

  return result;
}

/**************************************************************************//**
  \brief ZDP-Bind.Reqest primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeZdpBindReq(const void *const req,
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
  ZDO_ZdpReq_t *zdpRequest = (ZDO_ZdpReq_t *)req;
  ZDO_BindReq_t *bindRequest = &zdpRequest->req.reqPayload.bindReq;
  uint8_t sequenceNumber = zsiGetSequenceNumber();
  uint16_t length = zsiSerializeZdpReqCommon(&serializer, zdpRequest, cmdFrame);
  uint64_t u64;

  length += zsiZdpBindReqLength(bindRequest);

  zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_AREQ_CMD, ZSI_CMD_ZDP,
                    ZSI_ZDP_BIND_REQUEST);

  COPY_64BIT_VALUE(u64, bindRequest->srcAddr);
  zsiSerializeUint64(&serializer, &u64);
  zsiSerializeUint8(&serializer, bindRequest->srcEndpoint);
  zsiSerializeUint16(&serializer, bindRequest->clusterId);
  zsiSerializeUint8(&serializer, bindRequest->dstAddrMode);
  if (APS_EXT_ADDRESS == bindRequest->dstAddrMode)
  {
    COPY_64BIT_VALUE(u64, bindRequest->dstExtAddr);
    zsiSerializeUint64(&serializer, &u64);
    zsiSerializeUint8(&serializer, bindRequest->dstEndpoint);
  }
  else
    zsiSerializeUint16(&serializer, bindRequest->dstGroupAddr);

  return result;
}

/**************************************************************************//**
  \brief ZDP-Bind.Response primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeZdpBindResp(void *memory,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = false,
    .keepMemory = false
  };
  ZDO_BindResp_t *bindResponse = NULL;
  /* Obtain memory for confirm from bearing request */
  ZDO_ZdpReq_t *request =
    zsiDriverReturnBearingEntity(cmdFrame->sequenceNumber);

  (void)memory;

  if (request)
  {
    ZsiSerializer_t serializer =
    {
      .por = cmdFrame->payload
    };
    uint8_t u8;

    bindResponse = &request->resp.respPayload.bindResp;
    zsiDeserializeZdpRespCommon(&serializer, &request->resp);
    zsiDeserializeUint8(&serializer, &u8);
    bindResponse->status = u8;
  }

  /* Callback calling */
  if (bindResponse)
  {
    request->service.zsi.callback = zsiZDO_ZdpRespCallback;
    zsiAreqProcessingComplete(request);
  }

  return result;
}

#endif /* _BINDING_ */

#elif defined(ZAPPSI_NP)
/**************************************************************************//**
  \brief Routine to deserialize common part of all ZDP requests.

  \param[out] serializer - pointer to serializer.
  \param[in] req - pointer to ZDP request.

  \return None.
 ******************************************************************************/
void zsiDeserializeZdpReqCommon(ZsiSerializer_t *const serializer,
  ZDO_ZdpReq_t *const req)
{
  zsiDeserializeUint16(serializer, &req->reqCluster);
  zsiDeserializeUint8(serializer, (uint8_t *)&req->dstAddrMode);

  if (APS_EXT_ADDRESS == req->dstAddrMode)
    zsiDeserializeUint64(serializer, &req->dstAddress.extAddress);
  else
    zsiDeserializeUint16(serializer, &req->dstAddress.shortAddress);
}

/**************************************************************************//**
  \brief Routine to serialize common part of all ZDP responses.

  \param[out] serializer - pointer to serializer.
  \param[in] req - ZDP request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Serialized part length.
 ******************************************************************************/
uint16_t zsiSerializeZdpRespCommon(ZsiSerializer_t *const serializer,
  const ZDO_ZdpResp_t *const resp, ZsiCommandFrame_t *const cmdFrame)
{
  uint8_t *startingPoint = serializer->pow;

  (void)cmdFrame;

  zsiSerializeUint8(serializer, resp->srcAddrMode);
  if (APS_EXT_ADDRESS == resp->srcAddrMode)
    zsiSerializeUint64(serializer, &resp->srcAddress.extAddress);
  else
    zsiSerializeUint16(serializer, resp->srcAddress.shortAddress);
  zsiSerializeUint8(serializer, resp->respPayload.seqNum);
  zsiSerializeUint8(serializer, resp->respPayload.status);

  return serializer->pow - startingPoint;
}

/**************************************************************************//**
  \brief ZDP-IeeeAddressReq primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeZdpIeeeAddressReq(void *memory,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = false,
    .keepMemory = true
  };
  ZsiSerializer_t serializer =
  {
    .por = cmdFrame->payload
  };
  ZDO_ZdpReq_t *zdpRequest = memory;
  ZDO_IeeeAddrReq_t *ieeeRequest = &zdpRequest->req.reqPayload.ieeeAddrReq;
  union
  {
    uint8_t  u8;
    uint16_t u16;
  } value;

  zsiDeserializeZdpReqCommon(&serializer, zdpRequest);
  zsiDeserializeUint16(&serializer, &value.u16);
  ieeeRequest->nwkAddrOfInterest = value.u16;
  zsiDeserializeUint8(&serializer, &value.u8);
  ieeeRequest->reqType = value.u8;
  zsiDeserializeUint8(&serializer, &value.u8);
  ieeeRequest->startIndex = value.u8;

  zdpRequest->service.sequenceNumber = cmdFrame->sequenceNumber;
  zdpRequest->ZDO_ZdpResp = zsiZDO_ZdpResp;

  /* Call BitCloud handler */
  ZDO_ZdpReq(zdpRequest);

  return result;
}

/**************************************************************************//**
  \brief ZDP-IeeeAddress.Response primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeZdpIeeeAddressResp(const void *const req,
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
  ZDO_ZdpReq_t *request = (ZDO_ZdpReq_t *)req;
  ZDO_IeeeAddrResp_t *const ieeeResponse = &request->resp.respPayload.ieeeAddrResp;
  uint8_t sequenceNumber = request->service.sequenceNumber;
  uint16_t length = zsiSerializeZdpRespCommon(&serializer, &request->resp, cmdFrame);
  union
  {
    uint8_t it;
    uint64_t u64;
  } stuff;

  length += zsiZdpIeeeAddressRespLength(ieeeResponse);

  zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_AREQ_CMD, ZSI_CMD_ZDP,
                    ZSI_ZDP_IEEE_ADDRESS_RESPONSE);

  COPY_64BIT_VALUE(stuff.u64, ieeeResponse->ieeeAddrRemote);
  zsiSerializeUint64(&serializer, &stuff.u64);
  zsiSerializeUint16(&serializer, ieeeResponse->nwkAddrRemote);
  zsiSerializeUint8(&serializer, ieeeResponse->numAssocDev);
  zsiSerializeUint8(&serializer, ieeeResponse->startIndex);
  for (stuff.it = 0U; stuff.it < ieeeResponse->numAssocDev; stuff.it++)
  {
    zsiSerializeUint16(&serializer, ieeeResponse->nwkAddrAssocDevList[stuff.it]);
  }

  return result;
}

/**************************************************************************//**
  \brief ZDP-PermitJoining.Request primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeZdpPermitJoiningReq(void *memory,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = false,
    .keepMemory = true
  };
  ZsiSerializer_t serializer =
  {
    .por = cmdFrame->payload
  };
  ZDO_ZdpReq_t *zdpRequest = memory;
  ZDO_MgmtPermitJoiningReq_t *permitJoining = &zdpRequest->req.reqPayload.mgmtPermitJoiningReq;
  uint8_t value;

  zsiDeserializeZdpReqCommon(&serializer, zdpRequest);
  zsiDeserializeUint8(&serializer, &value);
  permitJoining->permitDuration = value;
  zsiDeserializeUint8(&serializer, &value);
  permitJoining->tcSignificance = value;

  zdpRequest->service.sequenceNumber = cmdFrame->sequenceNumber;
  zdpRequest->ZDO_ZdpResp = zsiZDO_ZdpResp;

  /* Call BitCloud handler */
  ZDO_ZdpReq(zdpRequest);

  return result;
}

/**************************************************************************//**
  \brief ZDP-PermitJoining.Response primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeZdpPermitJoiningResp(const void *const req,
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
  ZDO_ZdpReq_t *request = (ZDO_ZdpReq_t *)req;
  uint8_t sequenceNumber = request->service.sequenceNumber;
  uint16_t length = zsiSerializeZdpRespCommon(&serializer, &request->resp, cmdFrame);

  length += zsiZdpPermitJoiningRespLength(NULL);

  zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_AREQ_CMD, ZSI_CMD_ZDP,
                    ZSI_ZDP_PERMIT_JOINING_RESPONSE);

  return result;
}

/**************************************************************************//**
  \brief ZDP-SimpleDescriptorReq primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeZdpSimpleDescriptorReq(void *memory,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = false,
    .keepMemory = true
  };
  ZsiSerializer_t serializer =
  {
    .por = cmdFrame->payload
  };
  ZDO_ZdpReq_t *zdpRequest = memory;
  ZDO_SimpleDescReq_t *simpleRequest = &zdpRequest->req.reqPayload.simpleDescReq;
  union
  {
    uint8_t  u8;
    uint16_t u16;
  } value;

  zsiDeserializeZdpReqCommon(&serializer, zdpRequest);
  zsiDeserializeUint16(&serializer, &value.u16);
  simpleRequest->nwkAddrOfInterest = value.u16;
  zsiDeserializeUint8(&serializer, &value.u8);
  simpleRequest->endpoint = value.u8;

  zdpRequest->service.sequenceNumber = cmdFrame->sequenceNumber;
  zdpRequest->ZDO_ZdpResp = zsiZDO_ZdpResp;

  /* Call BitCloud handler */
  ZDO_ZdpReq(zdpRequest);

  return result;
}

/**************************************************************************//**
  \brief ZDP-SimpleDescriptor.Response primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeZdpSimpleDescriptorResp(const void *const req,
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
  ZDO_ZdpReq_t *request = (ZDO_ZdpReq_t *)req;
  ZDO_SimpleDescResp_t *const simpleDescResponse = &request->resp.respPayload.simpleDescResp;
  uint8_t sequenceNumber = request->service.sequenceNumber;
  uint16_t length = zsiSerializeZdpRespCommon(&serializer, &request->resp, cmdFrame);
  uint8_t it;

  length += zsiZdpSimpleDescriptorRespLength(simpleDescResponse);

  zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_AREQ_CMD, ZSI_CMD_ZDP,
                    ZSI_ZDP_SIMPLE_DESCRIPTOR_RESPONSE);

  zsiSerializeUint16(&serializer, simpleDescResponse->nwkAddrOfInterest);
  zsiSerializeUint8(&serializer, simpleDescResponse->length);
  zsiSerializeUint8(&serializer, simpleDescResponse->simpleDescriptor.endpoint);
  zsiSerializeUint16(&serializer, simpleDescResponse->simpleDescriptor.AppProfileId);
  zsiSerializeUint16(&serializer, simpleDescResponse->simpleDescriptor.AppDeviceId);
  zsiSerializeUint8(&serializer, simpleDescResponse->simpleDescriptor.AppDeviceVersion);
  zsiSerializeUint8(&serializer, simpleDescResponse->simpleDescriptor.AppInClustersCount);
  for (it = 0U; it < simpleDescResponse->simpleDescriptor.AppInClustersCount; it++)
  {
    zsiSerializeUint16(&serializer, simpleDescResponse->simpleDescriptor.AppInClustersList[it]);
  }
  zsiSerializeUint8(&serializer, simpleDescResponse->simpleDescriptor.AppOutClustersCount);
  for (it = 0U; it < simpleDescResponse->simpleDescriptor.AppOutClustersCount; it++)
  {
    zsiSerializeUint16(&serializer, simpleDescResponse->simpleDescriptor.AppOutClustersList[it]);
  }

  return result;
}

#ifdef _BINDING_
/**************************************************************************//**
  \brief ZDP-MatchDescriptorReq primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeZdpMatchDescriptorReq(void *memory,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = false,
    .keepMemory = true
  };
  ZsiSerializer_t serializer =
  {
    .por = cmdFrame->payload
  };
  ZDO_ZdpReq_t *zdpRequest = memory;
  ZDO_MatchDescReq_t *matchRequest = &zdpRequest->req.reqPayload.matchDescReq;
  union
  {
    uint8_t  u8;
    uint16_t u16;
  } value;
  uint8_t i;

  zsiDeserializeZdpReqCommon(&serializer, zdpRequest);
  zsiDeserializeUint16(&serializer, &value.u16);
  matchRequest->nwkAddrOfInterest = value.u16;
  zsiDeserializeUint16(&serializer, &value.u16);
  matchRequest->profileId = value.u16;
  zsiDeserializeUint8(&serializer, &value.u8);
  matchRequest->numInClusters = value.u8;
  for (i = 0U; i < matchRequest->numInClusters; i++)
  {
    zsiDeserializeUint16(&serializer, &value.u16);
    matchRequest->inClusterList[i] = value.u16;
  }

  zsiDeserializeUint8(&serializer, &value.u8);
  matchRequest->numOutClusters = value.u8;
  for (i = 0U; i < matchRequest->numOutClusters; i++)
  {
    zsiDeserializeUint16(&serializer, &value.u16);
    matchRequest->outClusterList[i] = value.u16;
  }

  zdpRequest->service.sequenceNumber = cmdFrame->sequenceNumber;
  zdpRequest->ZDO_ZdpResp = zsiZDO_ZdpResp;

  /* Call BitCloud handler */
  ZDO_ZdpReq(zdpRequest);

  return result;
}

/**************************************************************************//**
  \brief ZDP-MatchDescriptor.Response primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeZdpMatchDescriptorResp(const void *const req,
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
  ZDO_ZdpReq_t *request = (ZDO_ZdpReq_t *)req;
  ZDO_MatchDescResp_t *const matchResponse = &request->resp.respPayload.matchDescResp;
  uint8_t sequenceNumber = request->service.sequenceNumber;
  uint16_t length = zsiSerializeZdpRespCommon(&serializer, &request->resp, cmdFrame);
  uint8_t it;

  length += zsiZdpMatchDescriptorRespLength(matchResponse);

  zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_AREQ_CMD, ZSI_CMD_ZDP,
                    ZSI_ZDP_MATCH_DESCRIPTOR_RESPONSE);

  zsiSerializeUint16(&serializer, matchResponse->nwkAddrOfInterest);
  zsiSerializeUint8(&serializer, matchResponse->matchLength);
  for (it = 0U; it < matchResponse->matchLength; it++)
  {
    zsiSerializeUint8(&serializer, matchResponse->matchList[it]);
  }

  /* Free allocated memory, if all MatchDescriptorResponses reseived */
  if (ZDO_CMD_COMPLETED_STATUS == request->resp.respPayload.status)
    result.keepMemory = false;

  return result;
}

/**************************************************************************//**
  \brief ZDP-Bind.Request primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeZdpBindReq(void *memory,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = false,
    .keepMemory = true
  };
  ZsiSerializer_t serializer =
  {
    .por = cmdFrame->payload
  };
  ZDO_ZdpReq_t *zdpRequest = memory;
  ZDO_BindReq_t *bindRequest = &zdpRequest->req.reqPayload.bindReq;
  union
  {
    uint8_t  u8;
    uint16_t u16;
    uint64_t u64;
  } value;

  zsiDeserializeZdpReqCommon(&serializer, zdpRequest);
  zsiDeserializeUint64(&serializer, &value.u64);
  COPY_64BIT_VALUE(bindRequest->srcAddr, value.u64);
  zsiDeserializeUint8(&serializer, &value.u8);
  bindRequest->srcEndpoint = value.u8;
  zsiDeserializeUint16(&serializer, &value.u16);
  bindRequest->clusterId = value.u16;
  zsiDeserializeUint8(&serializer, &value.u8);
  bindRequest->dstAddrMode = value.u8;
  if (APS_EXT_ADDRESS == bindRequest->dstAddrMode)
  {
    zsiDeserializeUint64(&serializer, &value.u64);
    COPY_64BIT_VALUE(bindRequest->dstExtAddr, value.u64);
    zsiDeserializeUint8(&serializer, &value.u8);
    bindRequest->dstEndpoint = value.u8;
  }
  else
  {
    zsiDeserializeUint16(&serializer, &value.u16);
    bindRequest->dstGroupAddr = value.u16;
  }

  zdpRequest->service.sequenceNumber = cmdFrame->sequenceNumber;
  zdpRequest->ZDO_ZdpResp = zsiZDO_ZdpResp;

  /* Call BitCloud handler */
  ZDO_ZdpReq(zdpRequest);

  return result;
}

/**************************************************************************//**
  \brief ZDP-Bind.Response primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeZdpBindResp(const void *const req,
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
  ZDO_ZdpReq_t *request = (ZDO_ZdpReq_t *)req;
  ZDO_BindResp_t *const bindResponse = &request->resp.respPayload.bindResp;
  uint8_t sequenceNumber = request->service.sequenceNumber;
  uint16_t length = zsiSerializeZdpRespCommon(&serializer, &request->resp, cmdFrame);

  length += zsiZdpBindRespLength(NULL);

  zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_AREQ_CMD, ZSI_CMD_ZDP,
                    ZSI_ZDP_BIND_RESPONSE);

  zsiSerializeUint8(&serializer, bindResponse->status);

  return result;
}

#endif /* _BINDING_ */

#endif /* ZAPPSI_NP */

/* eof zsiZdpSerialization.c */
