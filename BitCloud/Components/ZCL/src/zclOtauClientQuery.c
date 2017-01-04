/************************************************************************//**
  \file zclOtauClientQuery.c

  \brief
    The file implements the OTAU client querying for image part

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    16.10.14 Karthik.P_u - Created.
  Last change:
    $Id: zclOtauClient.c 27261 2014-10-15 11:19:50Z karthik.p_u $
******************************************************************************/

#if (ZCL_SUPPORT == 1) && (APP_USE_OTAU == 1)

/******************************************************************************
                   Includes section
******************************************************************************/
#include <zclOtauManager.h>
#include <zclOtauClient.h>
#include <sysUtils.h>

/******************************************************************************
                   External variables section
******************************************************************************/
extern ZclOtauClientStateMachine_t stateMachine;
extern ZCL_OtauClusterClientAttributes_t otauClientAttributes;
extern ExtAddr_t serverExtAddr;
extern ExtAddr_t otauServerExtAddr;
extern uint32_t otauImageSize, otauImageVersion, otauNextOffset;
extern uint8_t otauRunningChecksum;
extern uint32_t otauInternalLength, otauImageRemainder;
extern uint8_t otauInternalAddrStatus;
extern OtauImageAuxVar_t recoveryLoading;
extern ExtAddr_t otauUnauthorizedServers[OTAU_MAX_UNAUTHORIZED_SERVERS];
extern ZclOtauDiscoveryResult_t *actvServer;
extern uint8_t retryCount, otauMaxRetryCount;
extern ZCL_Status_t otauUpgradeEndStatus;
#if APP_SUPPORT_OTAU_RECOVERY == 1
extern uint32_t otauFlashWriteOffset;
extern uint32_t otauNextPdsWriteOffset;
extern uint16_t otauPdsWriteFreqInBytes;
#endif

/******************************************************************************
                   Global variables section
******************************************************************************/
ZclOtauImageNotifyParams_t imgNtfyServer = 
{
  .addr = {
    .service.next       = NULL,
    .busy               = false,
    .serverEndpoint     = 0,
    .serverShortAddress = BROADCAST_ADDR_RX_ON_WHEN_IDLE,
    .serverExtAddress   = COMMON_SERVER_EXT_ADDRESS
   },  
  .ver.memAlloc = 0
};

/******************************************************************************
                   Implementation section
******************************************************************************/

/***************************************************************************//**
\brief Start to query the server for image
******************************************************************************/
void otauStartQuery(void)
{
  if (!OTAU_CHECK_STATE(stateMachine, OTAU_QUERY_FOR_IMAGE_STATE))
  {
    SYS_E_ASSERT_ERROR(false, ZCL_OTAU_INVALID_STATE_START_QUERY);
    return;
  }

  OTAU_SET_STATE(stateMachine, OTAU_QUERY_FOR_IMAGE_STATE);
  retryCount = otauMaxRetryCount;
  otauQueryNextImageReq();
}

/***************************************************************************//**
\brief Send query next image request
******************************************************************************/
void otauQueryNextImageReq(void)
{
  uint16_t csManufacturerId;
  CS_ReadParameter(CS_MANUFACTURER_CODE_ID, &csManufacturerId);
#if (USE_IMAGE_SECURITY == 1)
  otauReadImgTypeFromEeprom();
#else
  ZCL_OtauClientMem_t *clientMem = zclGetOtauClientMem();
  ZCL_OtauQueryNextImageReq_t *tmpOtauReq = &clientMem->zclReqMem.uQueryNextImageReq;
  ZCL_Request_t *tmpZclReq = &clientMem->reqMem.zclCommandReq;

  if (!isOtauBusy)
  {
    zclRaiseCustomMessage(OTAU_QUERY_NEXT_IMAGE_INITIATED);
    isOtauBusy = true;
  }

  zclOtauFillOutgoingZclRequest(QUERY_NEXT_IMAGE_REQUEST_ID, sizeof(ZCL_OtauQueryNextImageReq_t), (uint8_t *)tmpOtauReq);

  tmpOtauReq->controlField.hardwareVersionPresent = 0;
  tmpOtauReq->controlField.reserved               = 0;
  tmpOtauReq->manufacturerId                      = csManufacturerId;
  tmpOtauReq->imageType                           = OTAU_SPECIFIC_IMAGE_TYPE;
  tmpOtauReq->currentFirmwareVersion              = zclOtauMem.initParam.firmwareVersion;

  ZCL_CommandReq(tmpZclReq);
#endif
}

/***************************************************************************//**
\brief Start to query after elapsing a jitter

\param[in] queryJitter - queryJitter value
******************************************************************************/
void otauQueryAfterJitter(uint8_t queryJitter)
{
  uint32_t randomJitter;
  zclRaiseCustomMessage(OTAU_IMAGE_NOTIFICATION_RECEIVED);

  if (!OTAU_CHECK_STATE(stateMachine, OTAU_WAIT_TO_QUERY_STATE))
  {
    SYS_E_ASSERT_ERROR(false, ZCL_OTAU_INVALID_STATE_QUERY_AFTER_JITTER);
    return;
  }

  randomJitter = (uint32_t)SYS_GetNormalizedRandomNumber(OTAU_IMAGE_NOTIFY_MAX_QUERY_JITTER_VALUE);
  if (randomJitter <= queryJitter)
  {
    otauStartQueryTimer();
  }
  else
  {
    OTAU_SET_STATE(stateMachine, OTAU_QUERY_FOR_IMAGE_STATE);
    otauStartQueryTimer();
  }
}

/***************************************************************************//**
\brief Query next image response indication

\param[in] addressing - pointer to addressing information;
\param[in] payloadLength - data payload length;
\param[in] payload - data pointer.

\return status of indication routine
******************************************************************************/
ZCL_Status_t queryNextImageRespInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_OtauQueryNextImageResp_t *payload)
{
  ZCL_Status_t status = ZCL_SUCCESS_STATUS;
  uint8_t i;
  uint16_t csManufacturerId;
  CS_ReadParameter(CS_MANUFACTURER_CODE_ID, &csManufacturerId);

  if (!OTAU_CHECK_STATE(stateMachine, OTAU_QUERY_FOR_IMAGE_STATE))
  {
    SYS_E_ASSERT_WARN(false, ZCL_OTAU_INVALID_STATE_QUERY_NEXT_IMAGE_RSP);
    return status;
  }

  isOtauBusy = false;

  // process pending img notify(if from another server or for different file)
  if (IS_IMGNTFY_PENDING(imgNtfyServer.addr))
  {
    if (ZCL_SUCCESS_STATUS == otauCheckServerAddrAndTakeAction(false, true))
    {
      return status;
    }
  }

  switch (payload->status)
  {
    case ZCL_NO_IMAGE_AVAILABLE_STATUS:
      otauStartQueryTimer();
      break;

    case ZCL_NOT_AUTHORIZED_STATUS:
      for (i = 0; i < OTAU_MAX_UNAUTHORIZED_SERVERS; i++)
      {
        if ((otauUnauthorizedServers[i] == ZERO_SERVER_EXT_ADDRESS) || \
            (otauUnauthorizedServers[i] == COMMON_SERVER_EXT_ADDRESS))
        {
          COPY_EXT_ADDR(otauUnauthorizedServers[i], serverExtAddr);
        }
      }
      serverExtAddr = ZERO_SERVER_EXT_ADDRESS;
      otauStartDiscovery();
      break;

    case ZCL_SUCCESS_STATUS:
      if ((csManufacturerId != payload->manufacturerId) || (OTAU_SPECIFIC_IMAGE_TYPE != payload->imageType))
      {
        zclRaiseCustomMessage(OTAU_SERVER_RECEIVED_MALFORMED_COMMAND);
        status = ZCL_MALFORMED_COMMAND_STATUS;
      }
      else
      {
#if APP_SUPPORT_OTAU_RECOVERY == 1
        if (PDS_IsAbleToRestore(OTAU_IMGFILES_MEMORY_MEM_ID) &&
            PDS_IsAbleToRestore(OTAU_AUXFILES_MEMORY_MEM_ID) &&
            PDS_IsAbleToRestore(OTAU_OFFSETFILES_MEMORY_MEM_ID))
        {
          PDS_Restore(OTAU_IMGFILES_MEMORY_MEM_ID);
          PDS_Restore(OTAU_AUXFILES_MEMORY_MEM_ID);
          PDS_Restore(OTAU_OFFSETFILES_MEMORY_MEM_ID);
        }
#endif
        otauStartImageLoading(payload);
      }
      break;

    default:
      status = ZCL_MALFORMED_COMMAND_STATUS;
      break;
  }

  (void)addressing;
  (void)payloadLength;

  return status;
}

/***************************************************************************//**
\brief Start image downloading process
******************************************************************************/
void otauStartImageLoading(ZCL_OtauQueryNextImageResp_t *payload)
{
  ZCL_OtauClientMem_t *clientMem = zclGetOtauClientMem();
  ZclOtauClientImageBuffer_t *tmpParam = &clientMem->otauParam;
  ZCL_OtauFirmwareVersion_t version = clientMem->newFirmwareVersion;
  uint32_t tmpSize = payload->imageSize;
  ExtAddr_t zeroAddr = ZERO_SERVER_EXT_ADDRESS;
#if APP_SUPPORT_OTAU_RECOVERY == 1
  OFD_MemoryAccessParam_t *tmpMemParam = &clientMem->memParam;
#endif
  
#if APP_SUPPORT_OTAU_PAGE_REQUEST == 1
  OTAU_SET_STATE(stateMachine, OTAU_GET_IMAGE_PAGES_STATE);
#else
  OTAU_SET_STATE(stateMachine, OTAU_GET_IMAGE_BLOCKS_STATE);
#endif // APP_SUPPORT_OTAU_PAGE_REQUEST == 1

  if ((sizeof(ZCL_OtauSubElementHeader_t) + sizeof(ZCL_OtauUpgradeImageHeader_t)) >= payload->imageSize)
  {
    SYS_E_ASSERT_ERROR(false, ZCL_OTAU_INVALID_IMAGE_RECEIVED);

    clientMem->imageAuxParam.currentFileOffset     = sizeof(ZCL_OtauSubElementHeader_t) + sizeof(ZCL_OtauUpgradeImageHeader_t);
    clientMem->imageAuxParam.currentDataSize       = AUXILIARY_STRUCTURE_IS_FULL;
    clientMem->imageAuxParam.imageInternalLength   = 0;
    clientMem->imageAuxParam.internalAddressStatus = 0;
    clientMem->imageAuxParam.imageRemainder        = 0;
    clientMem->firstStart                          = OTAU_FIRST_CLIENT_START;
#if APP_SUPPORT_OTAU_PAGE_REQUEST == 1        
    clientMem->missedBytesGetting                  = OTAU_NOT_GET_MISSED_BYTES;
#endif

#if APP_SUPPORT_OTAU_RECOVERY == 1
    COPY_EXT_ADDR(otauServerExtAddr, zeroAddr);
    otauNextOffset          = 0;
    otauInternalAddrStatus  = 0;
    otauInternalLength      = 0;
    otauImageSize           = 0;
    otauImageRemainder      = 0;
    otauImageVersion        = 0;
    otauFlashWriteOffset    = 0;
    otauRunningChecksum     = 0xFF;

    PDS_Store(OTAU_IMGFILES_MEMORY_MEM_ID);
    PDS_Store(OTAU_AUXFILES_MEMORY_MEM_ID);
    PDS_Store(OTAU_OFFSETFILES_MEMORY_MEM_ID);
#endif
    (void)zeroAddr;
    
    //file is empty, send upgrade end req with invalid image status
    OTAU_SET_STATE(stateMachine, OTAU_WAIT_TO_UPGRADE_STATE);
    otauUpgradeEndStatus = ZCL_INVALID_IMAGE_STATUS;
    otauStartGenericTimer(0, otauUpgradeEndReq);    
    return;
  }
  
#if APP_SUPPORT_OTAU_RECOVERY == 1
  if ((IS_EQ_EXT_ADDR(otauServerExtAddr, serverExtAddr)) && \
      (0 == memcmp(&otauImageVersion, &payload->currentFirmwareVersion, sizeof(ZCL_OtauFirmwareVersion_t))) && \
      (0 == memcmp(&otauImageSize, &tmpSize, sizeof(uint32_t))) && \
      (0 < otauImageSize) && (0 != otauNextOffset))
  {
    if (OTAU_FIRST_CLIENT_START == clientMem->firstStart)
    {
      clientMem->imageAuxParam.currentFileOffset     = otauNextOffset;
      clientMem->imageAuxParam.imageInternalLength   = otauInternalLength;
      clientMem->imageAuxParam.imageRemainder        = otauImageRemainder;
      clientMem->imageAuxParam.internalAddressStatus = otauInternalAddrStatus;
      tmpMemParam->offset                            = otauFlashWriteOffset;
      clientMem->firstStart                          = OTAU_CONTINUE_CLIENT_WORK;

      COPY_EXT_ADDR(clientMem->otauServer, otauServerExtAddr);
      clientMem->newFirmwareVersion.memAlloc = payload->currentFirmwareVersion.memAlloc;
      tmpParam->imageSize = payload->imageSize;

      otauConfigureNextPdsWriteOffset();
      otauCountActuallyDataSize();
      otauStartDownload();
    }
    else // OTAU_CONTINUE_CLIENT_WORK
    { // Same server rediscovered
      clientMem->imageAuxParam = recoveryLoading;
      SYS_E_ASSERT_ERROR((clientMem->imageAuxParam.currentFileOffset < otauNextPdsWriteOffset), ZCL_OTAU_INVALID_NEXT_PDS_WRITE_OFFSET);
      otauStartDownload();
    }
  }
  else
  {
    COPY_EXT_ADDR(clientMem->otauServer, serverExtAddr);
    clientMem->newFirmwareVersion.memAlloc = payload->currentFirmwareVersion.memAlloc;
    tmpParam->imageSize = payload->imageSize;
    otauStartErase();
  }
 (void)version;
#else // APP_SUPPORT_OTAU_RECOVERY == 1
  if ((OTAU_CONTINUE_CLIENT_WORK == clientMem->firstStart) && \
      (IS_EQ_EXT_ADDR(clientMem->otauServer, serverExtAddr)) && \
      (0 == memcmp(&version, &payload->currentFirmwareVersion, sizeof(ZCL_OtauFirmwareVersion_t))) && \
      (0 == memcmp(&tmpParam->imageSize, &tmpSize, sizeof(uint32_t))))
  { // Server is rediscovered. Ensure that client has found the same server,
    // same image version and the image should be same size it was downloading
    // previously. If yes, resume the download from where it left.
    clientMem->imageAuxParam = recoveryLoading;
    otauStartDownload();
  }
  else
  { // If atleast one of the above check failed then restart the download from scratch.
    COPY_EXT_ADDR(clientMem->otauServer, serverExtAddr);
    clientMem->newFirmwareVersion.memAlloc = payload->currentFirmwareVersion.memAlloc;
    tmpParam->imageSize = payload->imageSize;

    otauStartErase();
  }
#endif
}

/***************************************************************************//**
\brief Write image notify command busyness status

\param[in] busyness - true->entry is valid, false->entry is invalid;
******************************************************************************/
void otauWriteImgNtfyBusyStatus(bool busyness)
{
  imgNtfyServer.addr.busy = busyness;
  if (false == busyness)
  {
    imgNtfyServer.addr.serverShortAddress = 0xFFFF;
    imgNtfyServer.addr.serverEndpoint = 0;
    imgNtfyServer.ver.memAlloc = 0;
    imgNtfyServer.queryJitter = 0;
  }
}

/***************************************************************************//**
\brief Process Image notify indication depending on the current state of client

\param[in] newState - proposed new state;
\param[in] newUpgrdSts - proposed upgrade status;
******************************************************************************/
void otauPrepareForNewState(ZclOtauClientStateMachine_t newState, ZCL_ImageUpdateStatus_t newUpgrdSts)
{
  isOtauBusy = false;
  otauStopGenericTimer();
  otauClientAttributes.imageUpgradeStatus.value = newUpgrdSts;

#if APP_SUPPORT_OTAU_RECOVERY == 1
  otauClearPdsParams();
#endif
  OTAU_SET_STATE(stateMachine, newState);
}

/***************************************************************************//**
\brief Process Image notify by checking server address, take appropriate action

\param[in] saveImgNtfyParams - request to keep image notify as pending request
\param[in] checkImgVer - request to compare firmware version

\return status of whether the command was processed or not
******************************************************************************/
ZCL_Status_t otauCheckServerAddrAndTakeAction(bool saveImgNtfyParams, bool checkImgVer)
{
  ZCL_OtauClientMem_t *clientMem = zclGetOtauClientMem();
  ZCL_Status_t sts = ZCL_SUCCESS_STATUS;

  if (saveImgNtfyParams)
  {
    otauWriteImgNtfyBusyStatus(true);
  }

  if ((NULL == actvServer) || (actvServer->serverShortAddress != imgNtfyServer.addr.serverShortAddress))
  {
    if (checkImgVer)
    {
      // from new server. save server ep, short addr, find ieee address
      if ((clientMem->newFirmwareVersion.memAlloc != imgNtfyServer.ver.memAlloc) ||
          (OTAU_UPGRADE_END_RESP_FW_VER_WILDCARD_VAL == imgNtfyServer.ver.memAlloc))
      {
        otauPrepareForNewState(OTAU_GET_IEEE_ADDR_STATE, OTAU_NORMAL);
        otauIeeeAddrReq(&imgNtfyServer.addr);
      }
      else
      {
        sts = ZCL_INVALID_DATA_TYPE_STATUS;
      }
    }
    else
    {
      otauPrepareForNewState(OTAU_GET_IEEE_ADDR_STATE, OTAU_NORMAL);
      otauIeeeAddrReq(&imgNtfyServer.addr);
    }
  }
  else
  {
    if (checkImgVer)
    {
      // same server. different fw version or wildcard are pass
      if ((clientMem->newFirmwareVersion.memAlloc != imgNtfyServer.ver.memAlloc) ||
          (OTAU_UPGRADE_END_RESP_FW_VER_WILDCARD_VAL == imgNtfyServer.ver.memAlloc))
      {
        otauPrepareForNewState(OTAU_QUERY_FOR_IMAGE_STATE, OTAU_NORMAL);
        otauTransitionToQuery();
      }
      else
      {
        sts = ZCL_INVALID_DATA_TYPE_STATUS;
      }
    }
    else
    {
      otauPrepareForNewState(OTAU_WAIT_TO_QUERY_STATE, OTAU_NORMAL);
      otauQueryAfterJitter(imgNtfyServer.queryJitter);
    }
    
    otauWriteImgNtfyBusyStatus(false);
  }
  return sts;
}

/***************************************************************************//**
\brief Process Image notify indication depending on the current state of client

\param[in] addressing - pointer to addressing information;
\param[in] payloadLength - data payload length;
\param[in] payload - data pointer.
******************************************************************************/
void otauProcessImageNotify(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_OtauImageNotify_t *payload)
{
  imgNtfyServer.addr.serverShortAddress = addressing->addr.shortAddress;
  imgNtfyServer.addr.serverEndpoint = addressing->endpointId;
  imgNtfyServer.queryJitter = payload->queryJitter;
  
  switch(stateMachine)
  {
    //----------------------- discovery states -----------------------
    case OTAU_INIT_OFD_STATE:
      otauWriteImgNtfyBusyStatus(true);
      break;
    case OTAU_WAIT_TO_DISCOVER_STATE:
      otauWriteImgNtfyBusyStatus(true);
      otauStopGenericTimer();
      OTAU_SET_STATE(stateMachine, OTAU_GET_IEEE_ADDR_STATE);
      otauIeeeAddrReq(&imgNtfyServer.addr);
      break;
    case OTAU_GET_MATCH_DESC_BROADCAST_STATE:
    case OTAU_GET_IEEE_ADDR_STATE:
    case OTAU_GET_MATCH_DESC_UNICAST_STATE:
    case OTAU_GET_NWK_ADDR_STATE:
    case OTAU_GET_LINK_KEY_STATE:
      otauWriteImgNtfyBusyStatus(true);
      break;
    //----------------------- query states --------------------------
    case OTAU_WAIT_TO_QUERY_STATE:
    case OTAU_QUERY_FOR_IMAGE_STATE:
      if (!addressing->nonUnicast)
      {
        // consider unicast IN as forced upgrade
        imgNtfyServer.ver.memAlloc = OTAU_UPGRADE_END_RESP_FW_VER_WILDCARD_VAL;
      }
      else if (OTAU_PAYLOAD_TYPE_NEW_FILE_VERSION <= payload->payloadType)
      {
        // broadcast IN with FW version received
        imgNtfyServer.ver.memAlloc = payload->newFileVersion;
      }

      if (false == isOtauBusy)
      {
        otauCheckServerAddrAndTakeAction(true, false);
      }
      else
      {
        otauWriteImgNtfyBusyStatus(true);
      }
      break;
    //----------------------- download states -----------------------
    case OTAU_GET_IMAGE_BLOCKS_STATE:
    case OTAU_GET_IMAGE_PAGES_STATE:
    case OTAU_GET_MISSED_BLOCKS_STATE:
    //----------------------- upgrade state -------------------------
    case OTAU_WAIT_TO_UPGRADE_STATE:
      if (!addressing->nonUnicast)
      {
        // consider unicast IN as forced upgrade
        imgNtfyServer.ver.memAlloc = OTAU_UPGRADE_END_RESP_FW_VER_WILDCARD_VAL;
      }
      else if (OTAU_PAYLOAD_TYPE_NEW_FILE_VERSION <= payload->payloadType)
      {
        // broadcast IN with FW version received
        imgNtfyServer.ver.memAlloc = payload->newFileVersion;
      }
      else
      {
        // broadcast IN without FW version
        otauEnqueueDiscoveryQueue(addressing->addr.shortAddress, actvServer->serverEndpoint);
        return;
      }
      
      if (false == isOtauBusy)
      {
        otauCheckServerAddrAndTakeAction(true, true);
      }
      else
      {
        otauWriteImgNtfyBusyStatus(true);
      }
      break;
    //----------------------- default -------------------------
    default:
      break;
  }
  
  (void)payloadLength;
}

/***************************************************************************//**
\brief Image notify indication

\param[in] addressing - pointer to addressing information;
\param[in] payloadLength - data payload length;
\param[in] payload - data pointer.

\return status of indication routine
******************************************************************************/
ZCL_Status_t imageNotifyInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_OtauImageNotify_t *payload)
{
  bool processImageNotify = true;
  uint16_t csManufacturerId;
  CS_ReadParameter(CS_MANUFACTURER_CODE_ID, &csManufacturerId);

  if (!addressing->nonUnicast)
  { // Unicast IN
    zclRaiseCustomMessage(OTAU_IMAGE_NOTIFICATION_RECEIVED);

    if ((OTAU_PAYLOAD_TYPE_QUERY_JITTER != payload->payloadType) && \
        (OTAU_IMAGE_NOTIFY_MAX_QUERY_JITTER_VALUE != payload->queryJitter))
    {/* Scenarios for this error case include unicast Image Notify command
     * with payload type of non-zero value, unicast Image Notify command
     * with query jitter value that is not 100. In such scenario, the
     * client should ignore the invalid message and shall send default
     * response command with MALFORMED_COMMAND status to the server.
     *
     * - Chapter 6.10.3.5.1, ZigBee OTAU Cluster Spec 095264r19
     */
      return ZCL_MALFORMED_COMMAND_STATUS;
    }
  }
  else
  { // Broadcast IN
    if ((OTAU_IMAGE_NOTIFY_MAX_QUERY_JITTER_VALUE < payload->queryJitter) || \
        (OTAU_PAYLOAD_TYPE_RESERVED == payload->payloadType))
    {/* For invalid broadcast or multicast Image Notify command, for example,
     * out-of-range query jitter value is used, or the reserved payload type
     * value is used, or the command is badly formatted, the client shall
     * ignore such command and no processing shall be done.
     *
     * - Chapter 6.10.3.5, ZigBee OTAU Cluster Spec 095264r19
     */
      return ZCL_SUCCESS_STATUS;
    }
  }

  /* For payload type value of 0x01, if manufacturer code matches the
  * device's own value, the device shall proceed. For payload type value
  * of 0x02, if both manufacturer code and image type match the device's
  * own values, the device shall proceed. For payload type value of 0x03
  * if both manufacturer code and image type match the device's own values
  * but the new file version is not a match, the device shall proceed.
  *
  * - Chapter 6.10.3.4, ZigBee OTAU Cluster Spec 095264r19
  */
  switch (payload->payloadType)
  {
    case OTAU_PAYLOAD_TYPE_RESERVED:
      processImageNotify = false;
      break;

    case OTAU_PAYLOAD_TYPE_NEW_FILE_VERSION:
      // do version check later based on current state 
    case OTAU_PAYLOAD_TYPE_IMAGE_TYPE:
      processImageNotify &= ((OTAU_SPECIFIC_IMAGE_TYPE == payload->imageType) || (OTAU_UPGRADE_END_RESP_IMG_TYPE_WILDCARD_VAL == payload->imageType));

    case OTAU_PAYLOAD_TYPE_MANUFACTURER_CODE:
      processImageNotify &= (csManufacturerId == payload->manufacturerCode);

    case OTAU_PAYLOAD_TYPE_QUERY_JITTER:
      break;
  }

  if (processImageNotify)
  {
    otauProcessImageNotify(addressing, payloadLength, payload);
  }

  (void)payloadLength;
  return ZCL_SUCCESS_STATUS;
}

/***************************************************************************//**
\brief Start timer to elapse query interval
******************************************************************************/
void otauStartQueryTimer(void)
{
  uint32_t queryInterval;
  isOtauBusy = false;
  CS_ReadParameter(CS_ZCL_OTAU_QUERY_INTERVAL_ID, &queryInterval);
  
  otauStartGenericTimer(queryInterval, otauTransitionToQuery);
}

#endif // (ZCL_SUPPORT == 1) && (APP_USE_OTAU == 1)

//eof zclOtauClientQuery.c
