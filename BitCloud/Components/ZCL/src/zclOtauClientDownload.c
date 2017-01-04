/************************************************************************//**
  \file zclOtauClientDownload.c

  \brief
    The file implements the OTAU client image downloading part

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

/******************************************************************************
                   External variables section
******************************************************************************/
extern ZclOtauClientStateMachine_t stateMachine;
extern ZCL_OtauClusterClientAttributes_t otauClientAttributes;
extern ExtAddr_t serverExtAddr;
extern ExtAddr_t otauServerExtAddr;
extern uint32_t otauImageSize, otauImageVersion, otauNextOffset;
extern uint32_t otauInternalLength, otauImageRemainder;
extern uint8_t otauRunningChecksum;
extern OtauImageAuxVar_t recoveryLoading;
extern uint8_t otauInternalAddrStatus;
extern ZclOtauImageNotifyParams_t imgNtfyServer;
extern uint8_t retryCount, otauMaxRetryCount;
extern ZCL_Status_t otauUpgradeEndStatus;

/******************************************************************************
                   Implementation section
******************************************************************************/

/***************************************************************************//**
\brief Check fields of block response command

\param[in] payload - pointer to command.

\return true - fields are correct,  \n
        false - otherwise.
******************************************************************************/
bool otauCheckBlockRespFields(ZCL_OtauImageBlockResp_t *payload)
{
  ZCL_OtauClientMem_t *clientMem = zclGetOtauClientMem();
  ZclOtauClientImageBuffer_t *tmpParam = &clientMem->otauParam;
  OtauImageAuxVar_t *tmpAuxParam = &clientMem->imageAuxParam;
  uint16_t csManufacturerId;
  CS_ReadParameter(CS_MANUFACTURER_CODE_ID, &csManufacturerId);

  /* for secured image, server needs to propagate the imgType of the OTA file */
  if ((csManufacturerId == payload->manufacturerId) && \
      (OTAU_SPECIFIC_IMAGE_TYPE == payload->imageType) && \
      (clientMem->newFirmwareVersion.memAlloc == payload->firmwareVersion.memAlloc) && \
      (payload->dataSize <= tmpAuxParam->currentDataSize) && \
      (payload->fileOffset < tmpParam->imageSize))
  {
    return true;
  }

  return false;
}

/***************************************************************************//**
\brief Check integrity of the recieved page

\return true - the whole page was received,  \n
        false - page receiving is in progress.
******************************************************************************/
bool otauCheckPageIntegrity(void)
{
  ZCL_OtauClientMem_t *clientMem = zclGetOtauClientMem();
  OtauImageAuxVar_t *tmpAuxParam = &clientMem->imageAuxParam;

  for (uint16_t itr = 0; itr < tmpAuxParam->lastPageSize; itr++)
    if (!(clientMem->missedBytesMask[itr >> 3] & (1 << ((uint8_t)itr & 0x07))))
      return false;

  return true;
}

/***************************************************************************//**
\brief Stores received from image block response data to page buffer or
       to flash directly (depends on page request usage).

\param[in] payload - payload form received image block response
******************************************************************************/
void otauBlockResponseImageDataStoring(ZCL_OtauImageBlockResp_t *payload)
{
  ZCL_OtauClientMem_t *clientMem = zclGetOtauClientMem();
  ZclOtauClientImageBuffer_t *tmpParam = &clientMem->otauParam;
  OFD_MemoryAccessParam_t *tmpMemParam = &clientMem->memParam;
  OtauImageAuxVar_t *tmpAuxParam = &clientMem->imageAuxParam;
#if APP_SUPPORT_OTAU_PAGE_REQUEST == 1
  bool pageRequestUsed;
  HAL_AppTimer_t *tmpPageReqTimer = &clientMem->pageRequestTimer;
  uint16_t responseSpacing;
#endif

#if APP_SUPPORT_OTAU_PAGE_REQUEST == 1
  CS_ReadParameter(CS_ZCL_OTAU_IMAGE_PAGE_REQUEST_ENABLE_ID, &pageRequestUsed);

  if (pageRequestUsed && (OTAU_PAGE_REQUEST_USAGE == clientMem->blockRequest))
  {
    memcpy(tmpParam->imagePageData + (payload->fileOffset - tmpAuxParam->imagePageOffset), payload->imageData, payload->dataSize);
    if (otauCheckPageIntegrity())
    {
      HAL_StopAppTimer(tmpPageReqTimer);
      tmpAuxParam->currentFileOffset = tmpAuxParam->imagePageOffset + tmpAuxParam->lastPageSize;
      tmpAuxParam->imageInternalLength -= tmpAuxParam->lastPageSize;
      tmpMemParam->data = tmpParam->imagePageData;
      tmpMemParam->length = tmpAuxParam->lastPageSize;

      clientMem->missedBytesGetting = OTAU_NOT_GET_MISSED_BYTES;

      OTAU_SET_STATE(stateMachine, OTAU_GET_IMAGE_PAGES_STATE);

#if APP_SUPPORT_OTAU_RECOVERY == 1
      otauNextOffset = tmpAuxParam->currentFileOffset;
      otauInternalLength = tmpAuxParam->imageInternalLength;
#endif
      otauStartWrite();
    }
    else
    {
      if (OTAU_GET_MISSED_BYTES == clientMem->missedBytesGetting)
      {
        OTAU_SET_STATE(stateMachine, OTAU_GET_MISSED_BLOCKS_STATE);
        otauGetMissedBlocks();
      }
      else
      {
        CS_ReadParameter(CS_ZCL_OTAU_IMAGE_PAGE_REQUEST_RESPONSE_SPACING_ID, &responseSpacing);
        tmpPageReqTimer->interval  = ((NWK_GetUnicastDeliveryTime() + responseSpacing) * 2);
        tmpPageReqTimer->mode      = TIMER_ONE_SHOT_MODE;
        tmpPageReqTimer->callback  = otauImagePageReqIntervalElapsed;

        // Start the tmpPageReqTimer for the next response
        HAL_StartAppTimer(tmpPageReqTimer);
      }
    }
  }
  else
#endif // APP_SUPPORT_OTAU_PAGE_REQUEST == 1
  {
    tmpAuxParam->currentFileOffset += payload->dataSize;
    memcpy(&tmpParam->imageBlockData, payload->imageData, payload->dataSize);
    tmpAuxParam->imageInternalLength -= payload->dataSize;
    tmpMemParam->data = tmpParam->imageBlockData;
    tmpMemParam->length = payload->dataSize;

#if APP_SUPPORT_OTAU_RECOVERY == 1
    otauNextOffset = tmpAuxParam->currentFileOffset;
    otauInternalLength = tmpAuxParam->imageInternalLength;
#endif
    otauStartWrite();
  }
}

/***************************************************************************//**
\brief Count the data size to receive further
******************************************************************************/
void otauCountActuallyDataSize(void)
{
  ZCL_OtauClientMem_t *clientMem = zclGetOtauClientMem();
  OtauImageAuxVar_t *tmpAuxParam = &clientMem->imageAuxParam;
#if APP_SUPPORT_OTAU_PAGE_REQUEST == 1
  ZCL_OtauImagePageReq_t *tmpOtauReq;
  uint16_t pageSize;
#endif
  if (tmpAuxParam->imageInternalLength > OFD_BLOCK_SIZE)
    tmpAuxParam->currentDataSize = OFD_BLOCK_SIZE;
  else
    tmpAuxParam->currentDataSize = tmpAuxParam->imageInternalLength;

#if APP_SUPPORT_OTAU_PAGE_REQUEST == 1
  clientMem->blockRequest = OTAU_PAGE_REQUEST_USAGE;
  if (tmpAuxParam->imageInternalLength <= OFD_BLOCK_SIZE)
  {
    clientMem->blockRequest = OTAU_BLOCK_REQUEST_USAGE;
    return;
  }

  tmpOtauReq = &clientMem->zclReqMem.uImagePageReq;
  CS_ReadParameter(CS_ZCL_OTAU_IMAGE_PAGE_REQUEST_PAGE_SIZE_ID, &pageSize);
  if (tmpAuxParam->imageInternalLength < pageSize)
    pageSize = tmpAuxParam->imageInternalLength;

  tmpOtauReq->pageSize = pageSize;
  tmpAuxParam->lastPageSize = pageSize;
#endif
}

/***************************************************************************//**
\brief Fills auxiliary structure from received block response payload

\param[in] payload - payload form received image block response

\return ZCL_SUCCESS_STATUS
******************************************************************************/
ZCL_Status_t otauFillAuxiliaryStructure(ZCL_OtauImageBlockResp_t *payload)
{
  ZCL_OtauClientMem_t         *clientMem = zclGetOtauClientMem();
  ZclOtauClientImageBuffer_t   *tmpParam = &clientMem->otauParam;
  OtauImageAuxVar_t         *tmpAuxParam = &clientMem->imageAuxParam;
  OFD_MemoryAccessParam_t   *tmpMemParam = &clientMem->memParam;

  memcpy(&tmpParam->imageBlockData[tmpAuxParam->internalAddressStatus], payload->imageData, payload->dataSize);
  tmpAuxParam->internalAddressStatus += payload->dataSize;

#if APP_SUPPORT_OTAU_RECOVERY == 1
  otauInternalAddrStatus = tmpAuxParam->internalAddressStatus;
#endif

  if (tmpAuxParam->internalAddressStatus < AUXILIARY_STRUCTURE_IS_FULL)
  {
    tmpAuxParam->currentDataSize = AUXILIARY_STRUCTURE_IS_FULL - tmpAuxParam->internalAddressStatus;
    if ((tmpAuxParam->currentFileOffset+payload->dataSize) >= tmpAuxParam->imageRemainder)
    {
      SYS_E_ASSERT_ERROR(false, ZCL_OTAU_INVALID_IMAGE_RECEIVED);
      otauUpgradeEndStatus = ZCL_INVALID_IMAGE_STATUS;
      otauStartGenericTimer(UPGRADE_END_REQ_QUICK_TIMEOUT, otauUpgradeEndReq);
    }
    else
    {
      tmpAuxParam->currentFileOffset += payload->dataSize;
      otauScheduleImageBlockReq();
    }
    return ZCL_SUCCESS_STATUS;
  }
  else
  {
    memcpy(&tmpMemParam->offset, tmpParam->imageBlockData, sizeof(uint32_t));
    memcpy(&tmpAuxParam->imageInternalLength, (tmpParam->imageBlockData + sizeof(uint32_t)), sizeof(uint32_t));

    if ((tmpMemParam->offset >= tmpAuxParam->imageRemainder) ||
        (!tmpAuxParam->imageInternalLength))
    {
      SYS_E_ASSERT_ERROR(false, ZCL_OTAU_INVALID_IMAGE_RECEIVED);
      otauUpgradeEndStatus = ZCL_INVALID_IMAGE_STATUS;
      otauStartGenericTimer(UPGRADE_END_REQ_QUICK_TIMEOUT, otauUpgradeEndReq);
      return ZCL_SUCCESS_STATUS;
    }
    else
    {
      otauCountActuallyDataSize();
      tmpAuxParam->imageRemainder -= (tmpAuxParam->imageInternalLength + AUXILIARY_STRUCTURE_IS_FULL);
#if APP_SUPPORT_OTAU_RECOVERY == 1
      otauImageRemainder = tmpAuxParam->imageRemainder;
      PDS_Store(OTAU_AUXFILES_MEMORY_MEM_ID);
#endif
    }
  }

  tmpAuxParam->currentFileOffset += payload->dataSize;
  otauStartDownload();
  return ZCL_SUCCESS_STATUS;
}

/***************************************************************************//**
\brief Process the successful image block received as per the current state

\param[in] payload - data pointer.

\return status of the processing
******************************************************************************/
ZCL_Status_t otauProcessSuccessfullImageBlockResponse(ZCL_OtauImageBlockResp_t *payload)
{
  ZCL_Status_t status = ZCL_SUCCESS_STATUS;
  ZCL_OtauClientMem_t *clientMem = zclGetOtauClientMem();
  OtauImageAuxVar_t *tmpAuxParam = &clientMem->imageAuxParam;
#if APP_SUPPORT_OTAU_PAGE_REQUEST == 1
  HAL_AppTimer_t *tmpPageReqTimer = &clientMem->pageRequestTimer;
  bool pageRequestUsed;
  uint8_t dataSize;
  uint16_t maskOffset;
#endif
  if (!otauCheckBlockRespFields(payload))
  {
    status = ZCL_MALFORMED_COMMAND_STATUS;
    zclRaiseCustomMessage(OTAU_SERVER_RECEIVED_MALFORMED_COMMAND);

    if ((OTAU_CHECK_STATE(stateMachine, OTAU_GET_IMAGE_BLOCKS_STATE)) || \
        (OTAU_CHECK_STATE(stateMachine, OTAU_GET_MISSED_BLOCKS_STATE)))
    {
      otauScheduleImageBlockReq();
    }

    return status;
  }

  /*
   *        OTAU_GET_IMAGE_BLOCKS_STATE
  */
  if (OTAU_CHECK_STATE(stateMachine, OTAU_GET_IMAGE_BLOCKS_STATE))
  {
    if (payload->fileOffset != tmpAuxParam->currentFileOffset)
    {
      return status;
    }
    if (tmpAuxParam->internalAddressStatus < AUXILIARY_STRUCTURE_IS_FULL)
    {
      return otauFillAuxiliaryStructure(payload);
    }

    otauBlockResponseImageDataStoring(payload);
  }
#if APP_SUPPORT_OTAU_PAGE_REQUEST == 1
  /*
   *        OTAU_GET_MISSED_BLOCKS_STATE
  */
  else if (OTAU_CHECK_STATE(stateMachine, OTAU_GET_MISSED_BLOCKS_STATE))
  {
    if (payload->fileOffset != tmpAuxParam->currentFileOffset)
    {
      return status;
    }
    // set up marker those bytes have been received
    maskOffset = (uint16_t)(payload->fileOffset - tmpAuxParam->imagePageOffset);
    dataSize = payload->dataSize;

    for (uint16_t itr = maskOffset; itr < (maskOffset + dataSize); itr++)
    {
      clientMem->missedBytesMask[itr >> 3] |= (0x01 << ((uint8_t)itr & 0x07));
    }

    otauBlockResponseImageDataStoring(payload);
  }
  /*
   *        OTAU_GET_IMAGE_PAGES_STATE
  */
  else if (OTAU_CHECK_STATE(stateMachine, OTAU_GET_IMAGE_PAGES_STATE))
  {
    CS_ReadParameter(CS_ZCL_OTAU_IMAGE_PAGE_REQUEST_ENABLE_ID, &pageRequestUsed);

    if (OTAU_BLOCK_REQUEST_USAGE == clientMem->blockRequest)
    {
      if (payload->fileOffset != tmpAuxParam->currentFileOffset)
      {
        return status;
      }

      if (tmpAuxParam->internalAddressStatus < AUXILIARY_STRUCTURE_IS_FULL)
      {
        return otauFillAuxiliaryStructure(payload);
      }

      otauBlockResponseImageDataStoring(payload);
    }
    else if (pageRequestUsed && (OTAU_PAGE_REQUEST_USAGE == clientMem->blockRequest))
    {
      if ((payload->fileOffset < tmpAuxParam->imagePageOffset) || \
          (payload->fileOffset > (tmpAuxParam->lastPageSize + tmpAuxParam->imagePageOffset)))
      { // response with wrong file offset has been received
        return status;
      }

      // Stop the tmpPageReqTimer that is running for current response
      HAL_StopAppTimer(tmpPageReqTimer);

      // set up marker those bytes have been received
      maskOffset = (uint16_t)(payload->fileOffset - tmpAuxParam->imagePageOffset);
      dataSize = payload->dataSize;

      for (uint16_t itr = maskOffset; itr < (maskOffset + dataSize); itr++)
      {
        clientMem->missedBytesMask[itr >> 3] |= (0x01 << ((uint8_t)itr & 0x07));
      }

      otauBlockResponseImageDataStoring(payload);
    }
  }
#endif // APP_SUPPORT_OTAU_PAGE_REQUEST == 1
  return status;
}

/***************************************************************************//**
\brief Process the image block received with wait status as per the current state

\param[in] payload - data pointer.

\return status of the processing
******************************************************************************/
ZCL_Status_t otauProcessWaitDataImageBlockResponse(ZCL_OtauImageBlockResp_t *payload)
{
  ZCL_Status_t status = ZCL_SUCCESS_STATUS;
  ZCL_OtauClientMem_t *clientMem = zclGetOtauClientMem();
  uint32_t delay = 0ul;

  if (payload->requestTime < payload->currentTime)
  { // time values are wrong
    status = ZCL_MALFORMED_COMMAND_STATUS;
    zclRaiseCustomMessage(OTAU_SERVER_RECEIVED_MALFORMED_COMMAND);
    otauClientAttributes.imageUpgradeStatus.value = OTAU_NORMAL;
    return status;
  }

  delay = payload->requestTime - payload->currentTime;
  if (0ul < delay)
  { // there is positive delay time
    delay *= AMOUNT_MSEC_IN_SEC;
    otauClientAttributes.minimumBlockRequestDelay.value = payload->blockRequestDelay;
  }
  else
  {
    delay = payload->blockRequestDelay;
  }

  if (OTAU_CHECK_STATE(stateMachine, OTAU_GET_IMAGE_BLOCKS_STATE) || \
      OTAU_CHECK_STATE(stateMachine, OTAU_GET_MISSED_BLOCKS_STATE))
  {
    retryCount = otauMaxRetryCount;
    otauStartGenericTimer(delay, otauImageBlockReq);
  }
#if APP_SUPPORT_OTAU_PAGE_REQUEST == 1
  else if (OTAU_CHECK_STATE(stateMachine, OTAU_GET_IMAGE_PAGES_STATE))
  {
    OTAU_SET_STATE(stateMachine, OTAU_GET_MISSED_BLOCKS_STATE);
    retryCount = otauMaxRetryCount;
    otauStartGenericTimer(delay, otauImageBlockReq);
  }
#endif // APP_SUPPORT_OTAU_PAGE_REQUEST == 1

  clientMem->blockRequestDelayOn = OTAU_BLOCK_REQUEST_DELAY_ON;
  return status;
}

/***************************************************************************//**
\brief Image block response indication

\param[in] addressing - pointer to addressing information;
\param[in] payloadLength - data payload length;
\param[in] payload - data pointer.

\return status of indication routine
******************************************************************************/
ZCL_Status_t imageBlockRespInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_OtauImageBlockResp_t *payload)
{
  ZCL_Status_t status = ZCL_SUCCESS_STATUS;
  ZCL_OtauClientMem_t *clientMem = zclGetOtauClientMem();

  if ((!OTAU_CHECK_STATE(stateMachine, OTAU_GET_IMAGE_BLOCKS_STATE)) && \
      (!OTAU_CHECK_STATE(stateMachine, OTAU_GET_IMAGE_PAGES_STATE)) && \
      (!OTAU_CHECK_STATE(stateMachine, OTAU_GET_MISSED_BLOCKS_STATE)))
  {
    SYS_E_ASSERT_WARN(false, ZCL_OTAU_INVALID_STATE_IMAGE_BLOCK_RSP);
    return status;
  }
  
  if (OTAU_CHECK_STATE(stateMachine, OTAU_GET_IMAGE_BLOCKS_STATE) || \
      OTAU_CHECK_STATE(stateMachine, OTAU_GET_MISSED_BLOCKS_STATE))    
  {/** If block requests are currently being used to fetch image blocks then
    * drop block responses that does not have the sequence that is currently
    * requested (clientMem->otauReqSeqNum).
    * E.g.: If a fresh block request is issued to fetch missing blocks
    * in GET_MISSED_BLOCKS state, then all the delayed block responses for the
    * previous page req has to be dropped. Because, they contain older seqnum.
    * This sequence number is assigned for every fresh issue of block request
    * at otauImageBlockReq() function.
    */
    if (clientMem->otauReqSeqNum != addressing->sequenceNumber)
    {
      return status;
    }
  }

  // process pending img notify(if from another server or for different file)
  if (IS_IMGNTFY_PENDING(imgNtfyServer.addr))
  {
    if (ZCL_SUCCESS_STATUS == otauCheckServerAddrAndTakeAction(false, true))
    {
      return status;
    }
  }

  switch(payload->status)
  {
    case ZCL_SUCCESS_STATUS:
      status = otauProcessSuccessfullImageBlockResponse(payload);
      break;

    case ZCL_ABORT_STATUS:
      isOtauBusy = false;
      otauStopGenericTimer();
      otauClientAttributes.imageUpgradeStatus.value = OTAU_NORMAL;
#if APP_SUPPORT_OTAU_RECOVERY == 1
      otauClearPdsParams();
#endif
      OTAU_SET_STATE(stateMachine, OTAU_WAIT_TO_UPGRADE_STATE);
      SYS_E_ASSERT_ERROR(false, ZCL_OTAU_INVALID_IMAGE_RECEIVED);
      otauUpgradeEndStatus = ZCL_INVALID_IMAGE_STATUS;
      otauStartGenericTimer(UPGRADE_END_REQ_QUICK_TIMEOUT, otauUpgradeEndReq);
      break;

    case ZCL_WAIT_FOR_DATA_STATUS:
      status = otauProcessWaitDataImageBlockResponse(payload);
      break;

    default:
      status = ZCL_MALFORMED_COMMAND_STATUS;
      zclRaiseCustomMessage(OTAU_SERVER_RECEIVED_MALFORMED_COMMAND);
      if (OTAU_CHECK_STATE(stateMachine, OTAU_GET_IMAGE_BLOCKS_STATE) || \
          OTAU_CHECK_STATE(stateMachine, OTAU_GET_MISSED_BLOCKS_STATE))
      {
        otauScheduleImageBlockReq();
      }
#if APP_SUPPORT_OTAU_PAGE_REQUEST == 1
      else if (OTAU_CHECK_STATE(stateMachine, OTAU_GET_IMAGE_PAGES_STATE))
      {
        retryCount = otauMaxRetryCount;
        otauImagePageReq();
      }
#endif // APP_SUPPORT_OTAU_PAGE_REQUEST == 1
      break;
  }

  (void)addressing;
  (void)payloadLength;
  return status;
}

/***************************************************************************//**
\brief Send image block request
******************************************************************************/
void otauImageBlockReq(void)
{
  ZCL_OtauClientMem_t *clientMem = zclGetOtauClientMem();
  ZCL_OtauImageBlockReq_t *tmpOtauReq = &clientMem->zclReqMem.uImageBlockReq;
  ZCL_Request_t *tmpZclReq = &clientMem->reqMem.zclCommandReq;
  uint16_t csManufacturerId;
  CS_ReadParameter(CS_MANUFACTURER_CODE_ID, &csManufacturerId);

  // check the necessity of the following line
  uint8_t size = clientMem->blockRequestDelayOn ? sizeof(ZCL_OtauImageBlockReq_t) : sizeof(ZCL_OtauImageBlockReq_t) - sizeof(uint16_t);

  ZCL_OtauImageType_t imgType = OTAU_SPECIFIC_IMAGE_TYPE;

  if ((!OTAU_CHECK_STATE(stateMachine, OTAU_GET_IMAGE_BLOCKS_STATE)) && \
      (!OTAU_CHECK_STATE(stateMachine, OTAU_GET_IMAGE_PAGES_STATE)) && \
      (!OTAU_CHECK_STATE(stateMachine, OTAU_GET_MISSED_BLOCKS_STATE)))
  {
    SYS_E_ASSERT_ERROR(false, ZCL_OTAU_INVALID_STATE_IMAGE_BLOCK_REQ);
    return;
  }

  if (!isOtauBusy)
  {
    isOtauBusy = true;
  }

  zclOtauFillOutgoingZclRequest(IMAGE_BLOCK_REQUEST_ID, size, (uint8_t *)tmpOtauReq);

#if (USE_IMAGE_SECURITY == 1)
  imgType = clientMem->eepromImgType;
#endif

  tmpOtauReq->controlField.blockRequestDelayPresent = 1;
  tmpOtauReq->controlField.reqNodeIeeeAddrPresent   = 0;
  tmpOtauReq->controlField.reserved                 = 0;
  tmpOtauReq->manufacturerId                        = csManufacturerId;
  tmpOtauReq->imageType                             = imgType;
  tmpOtauReq->firmwareVersion                       = clientMem->newFirmwareVersion;
  tmpOtauReq->fileOffset                            = clientMem->imageAuxParam.currentFileOffset;
  tmpOtauReq->maxDataSize                           = clientMem->imageAuxParam.currentDataSize;
  tmpOtauReq->blockRequestDelay                     = otauClientAttributes.minimumBlockRequestDelay.value;
  clientMem->otauReqSeqNum                          = tmpZclReq->dstAddressing.sequenceNumber;
  recoveryLoading = clientMem->imageAuxParam;
  ZCL_CommandReq(tmpZclReq);
}

/***************************************************************************//**
\brief Send image block request
******************************************************************************/
void otauScheduleImageBlockReq(void)
{
  retryCount = otauMaxRetryCount;
  if (0ul < otauClientAttributes.minimumBlockRequestDelay.value)
    otauStartGenericTimer(otauClientAttributes.minimumBlockRequestDelay.value, otauImageBlockReq);
  else
    otauImageBlockReq();
}

#if APP_SUPPORT_OTAU_PAGE_REQUEST == 1
/***************************************************************************//**
\brief Send image page request
******************************************************************************/
void otauImagePageReq(void)
{
  ZCL_OtauClientMem_t *clientMem = zclGetOtauClientMem();
  ZCL_OtauImagePageReq_t *tmpOtauReq = &clientMem->zclReqMem.uImagePageReq;
  ZCL_Request_t *tmpZclReq = &clientMem->reqMem.zclCommandReq;
  OFD_MemoryAccessParam_t *tmpMemParam = &clientMem->memParam;
  ZCL_OtauImageType_t imgType = OTAU_SPECIFIC_IMAGE_TYPE;
  uint16_t responseSpacing;
  uint16_t csManufacturerId;
  CS_ReadParameter(CS_MANUFACTURER_CODE_ID, &csManufacturerId);

  if (!OTAU_CHECK_STATE(stateMachine, OTAU_GET_IMAGE_PAGES_STATE))
  {
    SYS_E_ASSERT_ERROR(false, ZCL_OTAU_INVALID_STATE_IMAGE_PAGE_REQ);
    return;
  }

  zclOtauFillOutgoingZclRequest(IMAGE_PAGE_REQUEST_ID, sizeof(ZCL_OtauImagePageReq_t), (uint8_t *)tmpOtauReq);

#if (USE_IMAGE_SECURITY == 1)
  imgType = clientMem->eepromImgType;
#endif

  tmpOtauReq->controlField.reqNodeIeeeAddrPresent = 0;
  tmpOtauReq->controlField.reserved               = 0;
  tmpOtauReq->manufacturerId                      = csManufacturerId;
  tmpOtauReq->imageType                           = imgType;
  tmpOtauReq->firmwareVersion                     = clientMem->newFirmwareVersion;
  tmpOtauReq->fileOffset                          = clientMem->imageAuxParam.currentFileOffset;
  tmpOtauReq->maxDataSize                         = clientMem->imageAuxParam.currentDataSize;

  /* To address alignment issue */
  CS_ReadParameter(CS_ZCL_OTAU_IMAGE_PAGE_REQUEST_RESPONSE_SPACING_ID, &responseSpacing);
  tmpOtauReq->responseSpacing = responseSpacing;

  // clear mask for the lost bytes
  memset(clientMem->missedBytesMask, 0x00, tmpOtauReq->pageSize / 8 + 1);

  clientMem->imageAuxParam.imagePageOffset = clientMem->imageAuxParam.currentFileOffset;
  tmpMemParam->length = 0;
  clientMem->missedBytesGetting = OTAU_NOT_GET_MISSED_BYTES;
  recoveryLoading = clientMem->imageAuxParam;
  ZCL_CommandReq(tmpZclReq);
}

/***************************************************************************//**
\brief PageReqInterval duration has elapsed
******************************************************************************/
void otauImagePageReqIntervalElapsed(void)
{
  ZCL_OtauClientMem_t *clientMem = zclGetOtauClientMem();

  if (OTAU_CHECK_STATE(stateMachine, OTAU_GET_IMAGE_PAGES_STATE))
  {
    clientMem->missedBytesGetting = OTAU_GET_MISSED_BYTES;
    OTAU_SET_STATE(stateMachine, OTAU_GET_MISSED_BLOCKS_STATE);
    otauGetMissedBlocks();
  }
  else
  {
    SYS_E_ASSERT_WARN(false, ZCL_OTAU_INVALID_STATE_PAGE_REQ_TIMER_FIRED);
    clientMem->missedBytesGetting = OTAU_GET_MISSED_BYTES;
  }
}

/***************************************************************************//**
\brief Retrieve the missed image blocks in a image page
******************************************************************************/
void otauGetMissedBlocks(void)
{
  ZCL_OtauClientMem_t *clientMem = zclGetOtauClientMem();
  OtauImageAuxVar_t *tmpAuxParam = &clientMem->imageAuxParam;

  bool beginningFound = false, isBlockReceived;
  uint16_t begin = 0, end;

  if (!OTAU_CHECK_STATE(stateMachine, OTAU_GET_MISSED_BLOCKS_STATE))
  {
    SYS_E_ASSERT_ERROR(false, ZCL_OTAU_INVALID_STATE_GET_MISSED_BLOCKS);
    return;
  }

  end = tmpAuxParam->lastPageSize;

  // looking for continuous set of zeros i.e., missed blocks
  for (uint16_t itr = 0; itr < tmpAuxParam->lastPageSize; itr++)
  {
    isBlockReceived = (clientMem->missedBytesMask[itr >> 3] & (1 << ((uint8_t)itr & 0x07)));

    if (!isBlockReceived)
    { // is corresponding bit set to zero?
      if (!beginningFound)
      {
        begin = itr;
        beginningFound = true;
      }
    }

    if (beginningFound)
    { // is corresponding bit set to one after zero bit has been found?
      if (isBlockReceived)
      {
        end = itr;
        break;
      }
    }
  }

  if ((end - begin))
  {
    tmpAuxParam->currentFileOffset = tmpAuxParam->imagePageOffset + begin;

    if ((end - begin) > (uint16_t)OFD_BLOCK_SIZE)
    {
      tmpAuxParam->currentDataSize = OFD_BLOCK_SIZE;
    }
    else
    {
      tmpAuxParam->currentDataSize = (end - begin);
    }

    otauScheduleImageBlockReq();
  }
  else
  {
    OTAU_SET_STATE(stateMachine, OTAU_GET_IMAGE_PAGES_STATE);
    otauStartWrite();
    return;
  }
}
#endif // APP_SUPPORT_OTAU_PAGE_REQUEST == 1

/***************************************************************************//**
\brief Start or continue the image download
******************************************************************************/
void otauStartDownload(void)
{
  ZCL_OtauClientMem_t *clientMem = zclGetOtauClientMem();
  OtauImageAuxVar_t *tmpAuxParam = &clientMem->imageAuxParam;
#if APP_SUPPORT_OTAU_PAGE_REQUEST == 1
  bool pageRequestUsed;
#endif

  if ((!OTAU_CHECK_STATE(stateMachine, OTAU_GET_IMAGE_PAGES_STATE)) && \
      (!OTAU_CHECK_STATE(stateMachine, OTAU_GET_IMAGE_BLOCKS_STATE)))
  {
    SYS_E_ASSERT_ERROR(false, ZCL_OTAU_INVALID_STATE_START_DOWNLOAD);
    return;
  }

#if APP_SUPPORT_OTAU_RECOVERY == 1
  if (tmpAuxParam->currentFileOffset == clientMem->otauParam.imageSize)
  { // if full file is already present, go for upgrade
    OTAU_SET_STATE(stateMachine, OTAU_WAIT_TO_UPGRADE_STATE);
    otauStartGenericTimer(REPOST_OFD_ACTION, otauStartFlush);
    return;
  }
#endif // APP_SUPPORT_OTAU_RECOVERY == 1

  otauClientAttributes.imageUpgradeStatus.value = OTAU_DOWNLOAD_IN_PROGRESS;

  if (OTAU_CHECK_STATE(stateMachine, OTAU_GET_IMAGE_BLOCKS_STATE))
  {
    if (tmpAuxParam->internalAddressStatus < AUXILIARY_STRUCTURE_IS_FULL)
    {
      tmpAuxParam->currentDataSize = AUXILIARY_STRUCTURE_IS_FULL - tmpAuxParam->internalAddressStatus;
    }
    else
    {
      if (tmpAuxParam->imageInternalLength > OFD_BLOCK_SIZE)
      {
        tmpAuxParam->currentDataSize = OFD_BLOCK_SIZE;
      }
      else
      {
        tmpAuxParam->currentDataSize = tmpAuxParam->imageInternalLength;
      }
    }
    otauScheduleImageBlockReq();
  }
#if APP_SUPPORT_OTAU_PAGE_REQUEST == 1
  else if (OTAU_CHECK_STATE(stateMachine, OTAU_GET_IMAGE_PAGES_STATE))
  {
    if (OTAU_GET_MISSED_BYTES == clientMem->missedBytesGetting)
    {
      OTAU_SET_STATE(stateMachine, OTAU_GET_MISSED_BLOCKS_STATE);
      otauGetMissedBlocks();
      return;
    }

    CS_ReadParameter(CS_ZCL_OTAU_IMAGE_PAGE_REQUEST_ENABLE_ID, &pageRequestUsed);
    clientMem->blockRequest = OTAU_PAGE_REQUEST_USAGE;

    if (tmpAuxParam->internalAddressStatus < AUXILIARY_STRUCTURE_IS_FULL)
    {
      tmpAuxParam->currentDataSize = AUXILIARY_STRUCTURE_IS_FULL - tmpAuxParam->internalAddressStatus;
      clientMem->blockRequest = OTAU_BLOCK_REQUEST_USAGE;
    }
    else
    {
      if (tmpAuxParam->imageInternalLength <= OFD_BLOCK_SIZE)
      {
        tmpAuxParam->currentDataSize = tmpAuxParam->imageInternalLength;
        clientMem->blockRequest = OTAU_BLOCK_REQUEST_USAGE;
      }
    }

    if (pageRequestUsed && (OTAU_PAGE_REQUEST_USAGE == clientMem->blockRequest))
    {
      retryCount = otauMaxRetryCount;
      otauImagePageReq();
    }
    else
    {
      otauScheduleImageBlockReq();
    }
  }
#endif // APP_SUPPORT_OTAU_PAGE_REQUEST == 1

}

#endif // (ZCL_SUPPORT == 1) && (APP_USE_OTAU == 1)

//eof zclOtauClientDownload.c
