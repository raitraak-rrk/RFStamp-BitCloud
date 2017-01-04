/***************************************************************************//**
  \file zsiZdoSerialization.c

  \brief ZAppSI ZDO primitives serialization. This file was automatically
         generated.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   Last change:
    $Id: zsiZdoSerialization.c 26492 2014-03-26 10:49:30Z karthik.p_u $
 ******************************************************************************/

/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <zsiZdoSerialization.h>
#include <zsiZdpSerialization.h>
#include <zsiSerializer.h>
#include <sysUtils.h>

/******************************************************************************
                               Implementation section
 ******************************************************************************/

#ifdef ZAPPSI_HOST
/**************************************************************************//**
  \brief ZDO-StartNetwork.Request primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeZDO_StartNetworkReq(const void *const req,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = true,
    .keepMemory = true
  };
  uint16_t length = zsiZDO_StartNetworkReqLength(NULL);
  uint8_t sequenceNumber = zsiGetSequenceNumber();

  (void)req;

  zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_AREQ_CMD, ZSI_CMD_ZDO,
    ZSI_ZDO_START_NETWORK_REQUEST);

  /* No special serialization needed */
  return result;
}

/**************************************************************************//**
  \brief ZDO-StartNetwork.Confirm primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeZDO_StartNetworkConf(void *memory,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = false,
    .keepMemory = false
  };
  ZDO_StartNetworkConf_t *confirm = NULL;
  /* Obtain memory for confirm from bearing request */
  ZDO_StartNetworkReq_t *req =
    zsiDriverReturnBearingEntity(cmdFrame->sequenceNumber);

  (void)memory;

  if (req)
  {
    ZsiSerializer_t serializer =
    {
      .por = cmdFrame->payload
    };

    confirm = &req->confirm;
    zsiDeserializeUint8(&serializer, &confirm->activeChannel);
    zsiDeserializeUint16(&serializer, &confirm->shortAddr);
    zsiDeserializeUint16(&serializer, &confirm->PANId);
    zsiDeserializeUint64(&serializer, &confirm->extPANId);
    zsiDeserializeUint16(&serializer, &confirm->parentAddr);
    zsiDeserializeUint8(&serializer, (uint8_t *)&confirm->status);
  }

  /* Callback calling */
  if (confirm)
  {
    req->service.zsi.callback = zsiZDO_StartNetworkConfCallback;
    zsiAreqProcessingComplete(req);
  }

  /* Free allocated memory, as it already exists in request */
  return result;
}

/**************************************************************************//**
  \brief ZDO-ResetNetwork.Request primitive serialization routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeZDO_ResetNetworkReq(const void *const req,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = true,
    .keepMemory = true
  };

  uint16_t length = zsiZDO_ResetNetworkReqLength(NULL);
  uint8_t sequenceNumber = zsiGetSequenceNumber();

  (void)req;

  zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_AREQ_CMD, ZSI_CMD_ZDO,
    ZSI_ZDO_RESET_NETWORK_REQUEST);

  return result;
}

/**************************************************************************//**
  \brief ZDO-ResetNetwork.Confirm primitive serialization routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeZDO_ResetNetworkConf(void *memory,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = false,
    .keepMemory = false
  };
  ZDO_ResetNetworkConf_t *confirm = NULL;
  /* Obtain memory for confirm from bearing request */
  ZDO_ResetNetworkReq_t *req =
    zsiDriverReturnBearingEntity(cmdFrame->sequenceNumber);

  (void)memory;

  if (req)
  {
    ZsiSerializer_t serializer =
    {
      .por = cmdFrame->payload
    };

    confirm = &req->confParams;
    zsiDeserializeUint8(&serializer, (uint8_t *)&confirm->status);
  }

  if (confirm)
  {
    req->service.zsi.callback = zsiZDO_ResetNetworkConfCallback;
    zsiAreqProcessingComplete(req);
  }

  /* Free allocated memory, as it already exists in request */
  return result;
}
/**************************************************************************//**
  \brief ZDO-Sleep.Request primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeZDO_SleepReq(const void *const req,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = true,
    .keepMemory = true
  };
  uint16_t length = zsiZDO_SleepReqLength(NULL);
  uint8_t sequenceNumber = zsiGetSequenceNumber();

  (void)req;

  zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_AREQ_CMD, ZSI_CMD_ZDO,
    ZSI_ZDO_SLEEP_REQUEST);

  /* No special serialization needed */
  return result;
}

/**************************************************************************//**
  \brief ZDO-Sleep.Confirm primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

    \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeZDO_SleepConf(void *memory,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = false,
    .keepMemory = false
  };
  ZDO_SleepConf_t *confirm = NULL;
  /* Obtain memory for confirm from bearing request */
  ZDO_SleepReq_t *request =
    zsiDriverReturnBearingEntity(cmdFrame->sequenceNumber);

  (void)memory;

  if (request)
  {
    ZsiSerializer_t serializer =
    {
      .por = cmdFrame->payload
    };

    confirm = &request->confirm;
    zsiDeserializeUint8(&serializer, (uint8_t *)&confirm->status);
  }

  /* Callback calling */
  if (confirm)
  {
    request->service.zsi.callback = zsiZDO_SleepConfCallback;
    zsiAreqProcessingComplete(request);
  }

  /* Free allocated memory, as it already exists in request */
  return result;
}

/**************************************************************************//**
  \brief ZDO-Wakeup.Indication primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeZDO_WakeupInd(void *memory,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = false,
    .keepMemory = false
  };

  (void)memory;
  (void)cmdFrame;

  ZDO_WakeUpInd();
  asm("nop");

  return result;
}
#ifdef _BINDING_
/**************************************************************************//**
  \brief ZDO-Bind.Indication primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeZDO_BindIndication(void *memory,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = false,
    .keepMemory = false
  };
  ZsiSerializer_t serializer =
  {
    .por = cmdFrame->payload
  };
  ZDO_BindInd_t *bindInd = (ZDO_BindInd_t *)memory;
  union
  {
    uint32_t u32;
    uint64_t u64;
  } value;

  zsiDeserializeUint64(&serializer, &value.u64);
  COPY_64BIT_VALUE(bindInd->srcAddr, value.u64);
  zsiDeserializeUint8(&serializer, (uint8_t *)&bindInd->srcEndpoint);
  zsiDeserializeUint16(&serializer, (uint16_t *)&bindInd->clusterId);
  zsiDeserializeUint8(&serializer, (uint8_t *)&bindInd->dstAddrMode);
  zsiDeserializeUint64(&serializer, &value.u64);
  COPY_64BIT_VALUE(bindInd->dstExtAddr, value.u64);
  zsiDeserializeUint8(&serializer, (uint8_t *)&bindInd->dstEndpoint);

  ZDO_BindIndication(bindInd);

  return result;
}

/**************************************************************************//**
  \brief ZDO-Unbind.Indication primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeZDO_UnbindIndication(void *memory,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = false,
    .keepMemory = false
  };
  ZsiSerializer_t serializer =
  {
    .por = cmdFrame->payload
  };
  ZDO_UnbindInd_t *unbindInd = (ZDO_UnbindInd_t *)memory;
  union
  {
    uint32_t u32;
    uint64_t u64;
  } value;

  zsiDeserializeUint64(&serializer, &value.u64);
  COPY_64BIT_VALUE(unbindInd->srcAddr, value.u64);
  zsiDeserializeUint8(&serializer, (uint8_t *)&unbindInd->srcEndpoint);
  zsiDeserializeUint16(&serializer, (uint16_t *)&unbindInd->clusterId);
  zsiDeserializeUint8(&serializer, (uint8_t *)&unbindInd->dstAddrMode);
  zsiDeserializeUint64(&serializer, &value.u64);
  COPY_64BIT_VALUE(unbindInd->dstExtAddr, value.u64);
  zsiDeserializeUint8(&serializer, (uint8_t *)&unbindInd->dstEndpoint);

  ZDO_UnbindIndication(unbindInd);

  return result;
}
#endif // _BINDING_
/**************************************************************************//**
  \brief ZDO-MgmtNwkUpdateNotf primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeZDO_MgmtNwkUpdateNotf(void *memory,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = false,
    .keepMemory = false
  };
  ZsiSerializer_t serializer =
  {
    .por = cmdFrame->payload
  };
  ZDO_MgmtNwkUpdateNotf_t *notify = (ZDO_MgmtNwkUpdateNotf_t *)memory;
  union
  {
    uint8_t  u8;
    uint16_t u16;
    uint32_t u32;
    uint64_t u64;
  } value;

  zsiDeserializeUint8(&serializer, &notify->status);

  switch (notify->status)
  {
    case ZDO_STATIC_ADDRESS_CONFLICT_STATUS:
      zsiDeserializeUint16(&serializer, &value.u16);
      notify->conflictAddress = value.u16;
      break;

    case ZDO_NWK_UPDATE_STATUS:
    case ZDO_NETWORK_STARTED_STATUS:
    case ZDO_NETWORK_LOST_STATUS:
    case ZDO_NETWORK_LEFT_STATUS:
      zsiDeserializeUint16(&serializer, &value.u16);
      notify->nwkUpdateInf.parentShortAddr = value.u16;
      zsiDeserializeUint16(&serializer, &value.u16);
      notify->nwkUpdateInf.panId = value.u16;
      zsiDeserializeUint8(&serializer, &notify->nwkUpdateInf.currentChannel);
      zsiDeserializeUint16(&serializer, &value.u16);
      notify->nwkUpdateInf.shortAddr = value.u16;
      break;

    case ZDO_DELETE_KEY_PAIR_STATUS:
    case ZDO_DELETE_LINK_KEY_STATUS:
      zsiDeserializeUint64(&serializer, &value.u64);
      COPY_64BIT_VALUE(notify->deviceExtAddr, value.u64);
      break;

    case ZDO_CHILD_REMOVED_STATUS:
      zsiDeserializeUint16(&serializer, &value.u16);
      notify->childAddr.shortAddr = value.u16;
      zsiDeserializeUint64(&serializer, &value.u64);
      COPY_64BIT_VALUE(notify->childAddr.extAddr, value.u64);
      break;

    case ZDO_CHILD_JOINED_STATUS:
    case ZDO_NO_KEY_PAIR_DESCRIPTOR_STATUS:
      zsiDeserializeUint16(&serializer, &value.u16);
      notify->childInfo.shortAddr = value.u16;
      zsiDeserializeUint64(&serializer, &value.u64);
      COPY_64BIT_VALUE(notify->childInfo.extAddr, value.u64);
      zsiDeserializeBitfield(&serializer, &value.u8);
      notify->childInfo.capabilityInfo.alternatePANCoordinator = value.u8;
      zsiDeserializeBitfield(&serializer, &value.u8);
      notify->childInfo.capabilityInfo.deviceType = value.u8;
      zsiDeserializeBitfield(&serializer, &value.u8);
      notify->childInfo.capabilityInfo.powerSource = value.u8;
      zsiDeserializeBitfield(&serializer, &value.u8);
      notify->childInfo.capabilityInfo.rxOnWhenIdle = value.u8;
      zsiDeserializeBitfield(&serializer, &value.u8);
      notify->childInfo.capabilityInfo.securityCapability = value.u8;
      zsiDeserializeBitfield(&serializer, &value.u8);
      notify->childInfo.capabilityInfo.allocateAddress = value.u8;
      break;

    case ZDO_SUCCESS_STATUS:
      zsiDeserializeUint32(&serializer, &value.u32);
      notify->scanResult.scannedChannels = value.u32;
      zsiDeserializeUint16(&serializer, &value.u16);
      notify->scanResult.totalTransmissions = value.u16;
      zsiDeserializeUint16(&serializer, &value.u16);
      notify->scanResult.transmissionsFailures = value.u16;
      zsiDeserializeUint8(&serializer, &notify->scanResult.scannedChannelsListCount);
      zsiDeserializeData(&serializer, notify->scanResult.energyValues, ED_SCAN_RESULTS_AMOUNT);
      break;

    default:
      break;
  }

  ZDO_MgmtNwkUpdateNotf(notify);

  return result;
}

/**************************************************************************//**
  \brief ZDO-GetLqiRssi request primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeZDO_GetLqiRssiReq(const void *const req,
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
  uint16_t length = zsiZDO_GetLqiRssiReqLength(NULL);
  uint8_t sequenceNumber = zsiGetSequenceNumber();
  ZDO_GetLqiRssi_t *request = (ZDO_GetLqiRssi_t *)req;

  zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_SREQ_CMD, ZSI_CMD_ZDO,
                    ZSI_ZDO_GET_LQI_RSSI_REQUEST);

  zsiSerializeUint16(&serializer, request->nodeAddr);

  return result;
}

/**************************************************************************//**
  \brief ZDO-GetLqiRssi confirm primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeZDO_GetLqiRssiConf(void *memory,
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
  ZDO_GetLqiRssi_t *request = (ZDO_GetLqiRssi_t *)memory;

  zsiDeserializeUint8(&serializer, &request->lqi);
  zsiDeserializeUint8(&serializer, (uint8_t *)&request->rssi);

  return result;
}

/**************************************************************************//**
  \brief ZDO-ZdpReq primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeZDO_ZdpReq(const void *const req,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZDO_ZdpReq_t *request = (ZDO_ZdpReq_t *)req;
  /* Find appropriate serialization routine */
  ZsiSerializeRoutine_t serialize =
    zsiZdpFindSerializationRoutine(request->reqCluster);

  sysAssert(serialize, 0xFFFF);

  return serialize(req, cmdFrame);
}

/**************************************************************************//**
  \brief ZDO-GetNwkStatus primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeZDO_GetNwkStatusReq(const void *const req,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = true,
    .keepMemory = true
  };
  uint16_t length = zsiSerializeZDO_GetNwkStatusReqLength(NULL);
  uint8_t sequenceNumber = zsiGetSequenceNumber();

  (void)req;

  zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_SREQ_CMD, ZSI_CMD_ZDO,
                    ZSI_ZDO_NETWORK_STATUS_REQUEST);

  return result;
}

/**************************************************************************//**
  \brief ZDO-GetNwkStatus confirm primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

    \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeZDO_GetNwkStatusConf(void *memory,
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
  uint8_t *status = (uint8_t *)memory;

  zsiDeserializeData(&serializer, status, sizeof(uint8_t));

  return result;
}

#ifdef _ENDDEVICE_
/**************************************************************************//**
  \brief ZDO_StartSyncReq or ZDO_StopSyncReq primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeZDO_StartStopSyncReq(const void *const req,
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
  bool *startReq = (bool *)req;
  uint8_t reqType = *startReq ? ZSI_ZDO_START_SYNC_REQUEST : ZSI_ZDO_STOP_SYNC_REQUEST;
  uint16_t length = zsiZDO_StartStopSyncReq(NULL);
  uint8_t sequenceNumber = zsiGetSequenceNumber();

  zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_SREQ_CMD, ZSI_CMD_ZDO, reqType);
  zsiSerializeUint8(&serializer, *startReq);

  return result;
}

/**************************************************************************//**
  \brief ZDO_StartSyncConf primitive deserialization and
         processing routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
*******************************************************************************/
ZsiProcessingResult_t zsiDeserializeZDO_StartStopSyncConf(void *memory,
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
  ZDO_Status_t *data = (ZDO_Status_t *)memory;

  zsiDeserializeData(&serializer, data, sizeof(ZDO_Status_t));

  return result;
}
#endif // _ENDDEVICE_

#elif defined(ZAPPSI_NP)
/**************************************************************************//**
  \brief ZDO-StartNetwork.Request primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeZDO_StartNetworkReq(void *memory,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = false,
    .keepMemory = true
  };
  ZDO_StartNetworkReq_t *req = memory;

  req->service.sequenceNumber = cmdFrame->sequenceNumber;
  req->ZDO_StartNetworkConf = zsiZDO_StartNetworkConf;

  /* Call BitCloud handler */
  ZDO_StartNetworkReq(req);

  return result;
}

/**************************************************************************//**
  \brief ZDO-ResetNetwork.Request primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeZDO_ResetNetworkReq(void *memory,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = false,
    .keepMemory = true
  };
  ZDO_ResetNetworkReq_t *req = memory;

  req->service.sequenceNumber = cmdFrame->sequenceNumber;
  req->ZDO_ResetNetworkConf = zsiZDO_ResetNetworkConf;

  /* Call BitCloud handler */
  ZDO_ResetNetworkReq(req);

  return result;
}
/**************************************************************************//**
  \brief ZDO-StartNetwork.Confirm primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeZDO_StartNetworkConf(const void *const req,
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
  ZDO_StartNetworkReq_t *request = (ZDO_StartNetworkReq_t *)req;
  ZDO_StartNetworkConf_t *const confirm = &request->confirm;
  uint16_t length = zsiZDO_StartNetworkConfLength(NULL);
  uint8_t sequenceNumber = request->service.sequenceNumber;

  zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_AREQ_CMD, ZSI_CMD_ZDO,
                    ZSI_ZDO_START_NETWORK_CONFIRM);

  zsiSerializeUint8(&serializer, confirm->activeChannel);
  zsiSerializeUint16(&serializer, confirm->shortAddr);
  zsiSerializeUint16(&serializer, confirm->PANId);
  zsiSerializeUint64(&serializer, &confirm->extPANId);
  zsiSerializeUint16(&serializer, confirm->parentAddr);
  zsiSerializeUint8(&serializer, confirm->status);

  return result;
}

ZsiProcessingResult_t zsiSerializeZDO_ResetNetworkConf(const void *const req,
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

  ZDO_ResetNetworkReq_t *request = (ZDO_ResetNetworkReq_t *)req;
  ZDO_ResetNetworkConf_t *const confirm = &request->confParams;

  uint16_t length = zsiZDO_ResetNetworkConfLength(NULL);
  uint8_t sequenceNumber = request->service.sequenceNumber;

  zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_AREQ_CMD, ZSI_CMD_ZDO,
                    ZSI_ZDO_RESET_NETWORK_CONFIRM);

  zsiSerializeUint8(&serializer, confirm->status);

  return result;
}

/**************************************************************************//**
  \brief ZDO-Sleep.Request primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeZDO_SleepReq(void *memory,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = false,
    .keepMemory = true
  };
  ZDO_SleepReq_t *request = memory;

  request->service.sequenceNumber = cmdFrame->sequenceNumber;
  request->ZDO_SleepConf = zsiZDO_SleepConf;

  /* Sleep mode isn't supported yet - so emulation used instead */
  zsiZDO_SleepReq(request);

  return result;
}

/**************************************************************************//**
  \brief ZDO-Sleep.Confirm primitive serialization routine.

  \param[in] request - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeZDO_SleepConf(const void *const req,
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

  ZDO_SleepReq_t *request = (ZDO_SleepReq_t *)req;
  ZDO_SleepConf_t *const confirm = &request->confirm;
  uint16_t length = zsiZDO_SleepConfLength(NULL);
  uint8_t sequenceNumber = request->service.sequenceNumber;

  zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_AREQ_CMD, ZSI_CMD_ZDO,
                    ZSI_ZDO_SLEEP_CONFIRM);

  zsiSerializeUint8(&serializer, confirm->status);

  return result;
}

/**************************************************************************//**
  \brief ZDO-Wakeup.Indication primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeZDO_WakeUpInd(const void *const req,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = true,
    .keepMemory = true
  };
  uint16_t length = zsiZDO_WakeUpIndLength(NULL);
  uint8_t sequenceNumber = zsiGetSequenceNumber();

  (void)req;

  zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_AREQ_CMD, ZSI_CMD_ZDO,
                    ZSI_ZDO_WAKEUP_INDICATION);

  return result;
}

/**************************************************************************//**
  \brief ZDO-MgmtNwkUpdateNotf frame size calculation routine.
         SOF and LENGTH fields are dismissed.

  \param[in] ntfy - notification parameters.

  \return ZDO-MgmtNwkUpdateNotf frame size.
 ******************************************************************************/
uint16_t
zsiZDO_MgmtNwkUpdateNotfLength(const ZDO_MgmtNwkUpdateNotf_t *const ntfy)
{
  uint16_t size = ZSI_COMMAND_FRAME_OVERHEAD +
                  sizeof(uint8_t); /* status */

  switch (ntfy->status)
  {
    case ZDO_STATIC_ADDRESS_CONFLICT_STATUS:
      size += sizeof(uint16_t); /* conflictAddress */
      break;

    case ZDO_NWK_UPDATE_STATUS:
    case ZDO_NETWORK_STARTED_STATUS:
    case ZDO_NETWORK_LOST_STATUS:
    case ZDO_NETWORK_LEFT_STATUS:
      size += sizeof(uint16_t) + /* nwkUpdateInf.parentShortAddr */
              sizeof(uint16_t) + /* nwkUpdateInf.panId */
              sizeof(uint8_t)  + /* nwkUpdateInf.currentChannel */
              sizeof(uint16_t);  /* nwkUpdateInf.shortAddr */

      break;

    case ZDO_DELETE_KEY_PAIR_STATUS:
    case ZDO_DELETE_LINK_KEY_STATUS:
      size += sizeof(uint64_t); /* deviceExtAddr */
      break;

    case ZDO_CHILD_REMOVED_STATUS:
      size += sizeof(uint16_t) + /* childAddr.shortAddr */
              sizeof(uint64_t);  /* childAddr.extAddr */
      break;

    case ZDO_CHILD_JOINED_STATUS:
    case ZDO_NO_KEY_PAIR_DESCRIPTOR_STATUS:
      size += sizeof(uint16_t) +  /* childInfo.shortAddr */
              sizeof(uint64_t) +  /* childInfo.extAddr */
              sizeof(uint8_t)  +  /* childInfo.capabilityInfo.alternatePANCoordinator */
              sizeof(uint8_t)  +  /* childInfo.capabilityInfo.deviceType */
              sizeof(uint8_t)  +  /* childInfo.capabilityInfo.powerSource */
              sizeof(uint8_t)  +  /* childInfo.capabilityInfo.rxOnWhenIdle */
              sizeof(uint8_t)  +  /* childInfo.capabilityInfo.securityCapability */
              sizeof(uint8_t);    /* childInfo.capabilityInfo.allocateAddress */
      break;

    case ZDO_SUCCESS_STATUS:
      size += sizeof(uint32_t) +      /* scanResult.scannedChannels */
              sizeof(uint16_t) +      /* scanResult.totalTransmissions */
              sizeof(uint16_t) +      /* scanResult.transmissionsFailures */
              sizeof(uint8_t)  +      /* scanResult.scannedChannelsListCount */
              ED_SCAN_RESULTS_AMOUNT; /* scanResult.energyValues */
      break;

    default:
      break;
  }
  return size;
}

/**************************************************************************//**
  \brief ZDO-MgmtNwkUpdateNotf primitive serialization routine.

  \param[in] ntfy - notification parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeZDO_MgmtNwkUpdateNotf(const void *const ntfy,
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
  ZDO_MgmtNwkUpdateNotf_t *notify = (ZDO_MgmtNwkUpdateNotf_t *)ntfy;
  uint16_t length = zsiZDO_MgmtNwkUpdateNotfLength(notify);
  uint8_t sequenceNumber = zsiGetSequenceNumber();
  /* Aligned union to deal with packed data */
  union
  {
    uint32_t u32;
    uint64_t u64;
  } value;

  zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_AREQ_CMD, ZSI_CMD_ZDO,
                    ZSI_ZDO_MGMT_NWK_UPDATE_NOTIFY);

  zsiSerializeUint8(&serializer, notify->status);
  switch (notify->status)
  {
    case ZDO_STATIC_ADDRESS_CONFLICT_STATUS:
      zsiSerializeUint16(&serializer, notify->conflictAddress);
      break;

    case ZDO_NWK_UPDATE_STATUS:
    case ZDO_NETWORK_STARTED_STATUS:
    case ZDO_NETWORK_LOST_STATUS:
    case ZDO_NETWORK_LEFT_STATUS:
      zsiSerializeUint16(&serializer, notify->nwkUpdateInf.parentShortAddr);
      zsiSerializeUint16(&serializer, notify->nwkUpdateInf.panId);
      zsiSerializeUint8(&serializer, notify->nwkUpdateInf.currentChannel);
      zsiSerializeUint16(&serializer, notify->nwkUpdateInf.shortAddr);
      break;

    case ZDO_DELETE_KEY_PAIR_STATUS:
    case ZDO_DELETE_LINK_KEY_STATUS:
      COPY_64BIT_VALUE(value.u64, notify->deviceExtAddr);
      zsiSerializeUint64(&serializer, &value.u64);
      break;

    case ZDO_CHILD_REMOVED_STATUS:
      zsiSerializeUint16(&serializer, notify->childAddr.shortAddr);
      COPY_64BIT_VALUE(value.u64, notify->childAddr.extAddr);
      zsiSerializeUint64(&serializer, &value.u64);
      break;

    case ZDO_CHILD_JOINED_STATUS:
    case ZDO_NO_KEY_PAIR_DESCRIPTOR_STATUS:
      zsiSerializeUint16(&serializer, notify->childInfo.shortAddr);
      COPY_64BIT_VALUE(value.u64, notify->childInfo.extAddr);
      zsiSerializeUint64(&serializer, &value.u64);
      zsiSerializeBitfield(&serializer, notify->childInfo.capabilityInfo.alternatePANCoordinator);
      zsiSerializeBitfield(&serializer, notify->childInfo.capabilityInfo.deviceType);
      zsiSerializeBitfield(&serializer, notify->childInfo.capabilityInfo.powerSource);
      zsiSerializeBitfield(&serializer, notify->childInfo.capabilityInfo.rxOnWhenIdle);
      zsiSerializeBitfield(&serializer, notify->childInfo.capabilityInfo.securityCapability);
      zsiSerializeBitfield(&serializer, notify->childInfo.capabilityInfo.allocateAddress);
      break;

    case ZDO_SUCCESS_STATUS:
      value.u32 = notify->scanResult.scannedChannels;
      zsiSerializeUint32(&serializer, &value.u32);
      zsiSerializeUint16(&serializer, notify->scanResult.totalTransmissions);
      zsiSerializeUint16(&serializer, notify->scanResult.transmissionsFailures);
      zsiSerializeUint8(&serializer, notify->scanResult.scannedChannelsListCount);
      zsiSerializeData(&serializer, notify->scanResult.energyValues, ED_SCAN_RESULTS_AMOUNT);
      break;

    default:
      break;
  }

  return result;
}

/**************************************************************************//**
  \brief ZDO-BindingInd primitive serialization routine.

  \param[in] ind - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeZDO_BindIndication(const void *const ind,
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

  ZDO_BindInd_t *bindInd = (ZDO_BindInd_t *)ind;
  uint16_t length = zsiZDO_BindIndicationLength(ind);
  uint8_t sequenceNumber = zsiGetSequenceNumber();

  zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_AREQ_CMD, ZSI_CMD_ZDO,
                    ZSI_ZDO_BIND_INDICATION);
  /* Aligned union to deal with packed data */
  union
  {
    uint32_t u32;
    uint64_t u64;
  } value;

  COPY_64BIT_VALUE(value.u64, bindInd->srcAddr);
  zsiSerializeUint64(&serializer, &value.u64);
  zsiSerializeUint8(&serializer, bindInd->srcEndpoint);
  zsiSerializeUint16(&serializer, bindInd->clusterId);
  zsiSerializeUint8(&serializer, bindInd->dstAddrMode);
  COPY_64BIT_VALUE(value.u64, bindInd->dstExtAddr);
  zsiSerializeUint64(&serializer, &value.u64);
  zsiSerializeUint8(&serializer, bindInd->dstEndpoint);

  return result;
}

/**************************************************************************//**
  \brief ZDO-UnbindingInd primitive serialization routine.

  \param[in] ind - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeZDO_UnbindIndication(const void *const ind,
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

  ZDO_UnbindInd_t *unbindInd = (ZDO_BindInd_t *)ind;
  uint16_t length = zsiZDO_UnbindIndicationLength(ind);
  uint8_t sequenceNumber = zsiGetSequenceNumber();

  zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_AREQ_CMD, ZSI_CMD_ZDO,
                    ZSI_ZDO_UNBIND_INDICATION);
  /* Aligned union to deal with packed data */
  union
  {
    uint32_t u32;
    uint64_t u64;
  } value;

  COPY_64BIT_VALUE(value.u64, unbindInd->srcAddr);
  zsiSerializeUint64(&serializer, &value.u64);
  zsiSerializeUint8(&serializer, unbindInd->srcEndpoint);
  zsiSerializeUint16(&serializer, unbindInd->clusterId);
  zsiSerializeUint8(&serializer, unbindInd->dstAddrMode);
  COPY_64BIT_VALUE(value.u64, unbindInd->dstExtAddr);
  zsiSerializeUint64(&serializer, &value.u64);
  zsiSerializeUint8(&serializer, unbindInd->dstEndpoint);

  return result;
}
/**************************************************************************//**
  \brief ZDO-GetLqiRssi request primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeZDO_GetLqiRssiReq(void *memory,
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
  ZDO_GetLqiRssi_t *request = (ZDO_GetLqiRssi_t *)memory;

  zsiDeserializeUint16(&serializer, &request->nodeAddr);

  ZDO_GetLqiRssi(request);

  /* Prepare and send confirm frame at the same buffer, as request */
  {
    uint16_t length = zsiZDO_GetLqiRssiConfLength(NULL);
    uint8_t sequenceNumber = cmdFrame->sequenceNumber;

    zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_SRSP_CMD, ZSI_CMD_ZDO,
                      ZSI_ZDO_GET_LQI_RSSI_CONFIRM);

    zsiSerializeUint8(&serializer, request->lqi);
    zsiSerializeUint8(&serializer, (uint8_t)request->rssi);
  }

  return result;
}

/**************************************************************************//**
  \brief ZDO-ZdpResp primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeZDO_ZdpResp(const void *const req,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZDO_ZdpReq_t *request = (ZDO_ZdpReq_t *)req;
  /* Find appropriate serialization routine */
  ZsiSerializeRoutine_t serialize =
    zsiZdpFindSerializationRoutine(request->reqCluster);

  sysAssert(serialize, 0xFFFF);

  return serialize(req, cmdFrame);
}

/**************************************************************************//**
  \brief ZDO-ZDO_GetNwkStatus request primitive deserialization and
         processing routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

    \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeZDO_GetNwkStatusReq(void *memory,
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
  ZDO_NwkStatus_t status = ZDO_GetNwkStatus();

  /* Prepare and send confirm frame at the same buffer, as request */
  {
    uint16_t length = zsiZDO_GetNwkStatusConfLength(NULL);
    uint8_t sequenceNumber = cmdFrame->sequenceNumber;

    zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_SRSP_CMD, ZSI_CMD_ZDO,
     ZSI_ZDO_NETWORK_STATUS_CONFIRM);

    zsiSerializeData(&serializer, &status, sizeof(ZDO_NwkStatus_t));
  }

  (void)memory;
  return result;
}

#ifdef _ENDDEVICE_
/**************************************************************************//**
  \brief ZDO_StartSyncReq or ZDO_StopSyncReq request primitive deserialization and
         processing routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
*******************************************************************************/
ZsiProcessingResult_t zsiDeserializeZDO_StartStopSyncReq(void *memory,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepMemory = false,
    .keepCmdFrame = true
  };
  ZsiSerializer_t serializer =
  {
    .pow = cmdFrame->payload,
    .por = cmdFrame->payload
  };
  ZDO_Status_t status;
  bool dataIn;
  uint8_t confType;
  (void)memory;

  zsiDeserializeData(&serializer, &dataIn, sizeof(bool));

  if (dataIn)
  {
    status = ZDO_StartSyncReq();
    confType = ZSI_ZDO_START_SYNC_CONFIRM;
  }
  else
  {
    status = ZDO_StopSyncReq();
    confType = ZSI_ZDO_STOP_SYNC_CONFIRM;
  }

  // Prepare and send confirm frame at the same buffer, as request
  {
    uint16_t length = zsiZDO_StartStopSyncConf(NULL);
    uint8_t sequenceNumber = cmdFrame->sequenceNumber;

    serializer.pow = cmdFrame->payload;
    serializer.por = cmdFrame->payload;

    zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_SRSP_CMD, ZSI_CMD_ZDO, confType);

    zsiSerializeData(&serializer, &status, sizeof(ZDO_Status_t));
  }

  return result;
}
#endif // _ENDDEVICE_

#endif /* ZAPPSI_NP */

/* eof zsiZdoSerialization.c */
