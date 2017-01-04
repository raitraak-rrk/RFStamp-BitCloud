/************************************************************************//**
  \file zclOtauClient.c

  \brief
    The file implements the OTAU client initialization and common part

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    19.01.10 A. Khromykh - Created.
    14.10.14 Karthik.P_u - Modified.
  Last change:
    $Id: zclOtauClient.c 27753 2015-04-07 11:30:50Z viswanadham.kotla $
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
#if APP_SUPPORT_OTAU_RECOVERY == 1
extern ExtAddr_t otauServerExtAddr;
extern uint32_t otauImageSize, otauImageVersion;
extern uint32_t otauNextOffset;
extern uint8_t otauRunningChecksum;
extern uint8_t otauInternalAddrStatus;
extern uint32_t otauInternalLength;
extern uint32_t otauFlashWriteOffset;
extern uint32_t otauImageRemainder;
#endif // APP_SUPPORT_OTAU_RECOVERY == 1

extern ZCL_Status_t otauUpgradeEndStatus;
extern ZclOtauDiscoveryResult_t *actvServer;
extern ZclOtauImageNotifyParams_t imgNtfyServer;

/******************************************************************************
                          Prototypes section
******************************************************************************/

/******************************************************************************
                        Static variables section
******************************************************************************/
static uint32_t writeSkipSecStartEnd[OTAU_FLASH_WRITE_SKIP_WORDS_LENGTH];
static HAL_AppTimer_t zclEepromBusyTimer =
{
  .interval = 10,
  .mode     = TIMER_ONE_SHOT_MODE
};

/*******************************************************************************
                        Global variables section
*******************************************************************************/
// OTAU Cluster Attributes
ZCL_OtauClusterClientAttributes_t otauClientAttributes =
{
  DEFINE_OTAU_CLIENT_ATTRIBUTES
};

// OTAU Cluster Commands
ZCL_OtauClusterCommands_t otauClientCommands =
{
  OTAU_COMMANDS(imageNotifyInd, NULL, queryNextImageRespInd, NULL, NULL, \
               imageBlockRespInd, NULL, upgradeEndRespInd)
};

DECLARE_QUEUE(zclOtauServerDiscoveryQueue);
ExtAddr_t otauUnauthorizedServers[OTAU_MAX_UNAUTHORIZED_SERVERS];
uint8_t retryCount, otauMaxRetryCount;

#if (USE_IMAGE_SECURITY == 1)
void (*configureKeyDoneCallback)(void);
const ZCL_OtauImageKey_t otauImageKey = {1, IMAGE_KEY, IMAGE_IV};
#endif /* #if (USE_IMAGE_SECURITY == 1) */

/******************************************************************************
                   External variables section
******************************************************************************/
ZclOtauClientStateMachine_t stateMachine = OTAU_STOPPED_STATE;
ExtAddr_t serverExtAddr = COMMON_SERVER_EXT_ADDRESS;
uint8_t eepromInNewImage;
OtauImageAuxVar_t recoveryLoading;
uint32_t otauNextPdsWriteOffset;
uint16_t otauPdsWriteFreqInBytes;

/*******************************************************************************
                        Static variables section
*******************************************************************************/
static uint8_t ofdWriteRetry;

/******************************************************************************
                   Implementation section
******************************************************************************/

/***************************************************************************//**
\brief Checks whether the given IEEE or extended address is valid

\param[in] *addr - pointer to the IEEE or extended address

\return 'true' if address is valid otherwise 'false'
******************************************************************************/
bool isExtAddrValid(ExtAddr_t addr)
{
  ExtAddr_t zeroExtAddr = ZERO_SERVER_EXT_ADDRESS;
  ExtAddr_t commonExtAddr = COMMON_SERVER_EXT_ADDRESS;

  if ((IS_EQ_EXT_ADDR(addr, zeroExtAddr)) || (IS_EQ_EXT_ADDR(addr, commonExtAddr)))
  {
    return false;
  }

  return true;
}

/***************************************************************************//**
\brief Start otau client service
******************************************************************************/
void zclStartOtauClient(void)
{
  ZclOtauMem_t *otauMem = zclGetOtauMem();
  ZCL_OtauClientMem_t *clientMem = zclGetOtauClientMem();
  ExtAddr_t zeroAddr = ZERO_SERVER_EXT_ADDRESS;

  if (!OTAU_CHECK_STATE(stateMachine, OTAU_STOPPED_STATE))
  {
    zclRaiseCustomMessage(OTAU_DOUBLE_START);
    return;
  }
  
  CS_ReadParameter(CS_ZCL_OTAU_MAX_RETRY_COUNT_ID, &otauMaxRetryCount);
  retryCount = ofdWriteRetry = otauMaxRetryCount;

  for (uint8_t i = 0; i < OTAU_MAX_UNAUTHORIZED_SERVERS; i++)
  {
    COPY_EXT_ADDR(otauUnauthorizedServers[i], zeroAddr);
  }

  isOtauBusy = false;
  otauMem->isOtauStopTriggered = false;
  clientMem->firstStart = OTAU_FIRST_CLIENT_START;

  otauClientAttributes.minimumBlockRequestDelay.value = 0ul;
  otauClientAttributes.imageUpgradeStatus.value = OTAU_NORMAL;

  OTAU_SET_STATE(stateMachine, OTAU_INIT_OFD_STATE);
  OFD_Open(otauOpenOfdCallback);
}

/***************************************************************************//**
\brief Stop otau client service
******************************************************************************/
void zclStopOtauClient(void)
{
  ZclOtauMem_t *otauMem = zclGetOtauMem();
  OTAU_SET_STATE(stateMachine, OTAU_STOPPED_STATE);

  if (otauMem->isOtauStopTriggered)
  {
    return;
  }

  otauClientAttributes.minimumBlockRequestDelay.value = 0;
  otauMem->isOtauStopTriggered = true;
  isOtauBusy = false;
  otauStopGenericTimer();
  OFD_Close();
}

/**************************************************************************//**
\brief Counts crc current memory area. CRC-8. Polynom 0x31    x^8 + x^5 + x^4 + 1.

\param[in]
  crc - first crc state
\param[in]
  pcBlock - pointer to the memory for crc counting
\param[in]
  length - memory size

\return
  current area crc
******************************************************************************/
uint8_t otauCalcCrc(uint8_t crc, uint8_t *pcBlock, uint8_t length)
{
  uint8_t i;

  while (length--)
  {
    crc ^= *pcBlock++;

    for (i = 0; i < 8; i++)
      crc = crc & 0x80 ? (crc << 1) ^ 0x31 : crc << 1;
  }

  return crc;
}

/***************************************************************************//**
\brief Callback from flash driver about openning

\param[in] status - status of action
******************************************************************************/
void otauOpenOfdCallback(OFD_Status_t status)
{
  if (!OTAU_CHECK_STATE(stateMachine, OTAU_INIT_OFD_STATE))
  {
    SYS_E_ASSERT_ERROR(false, ZCL_OTAU_INVALID_STATE_OFD_OPEN_CALLBACK);
    return;
  }

  if (OFD_STATUS_SUCCESS != status)
  {
    OTAU_SET_STATE(stateMachine, OTAU_STOPPED_STATE);
    if (IS_IMGNTFY_PENDING(imgNtfyServer.addr))
    {
      otauWriteImgNtfyBusyStatus(false);
    }

    zclStopOtauClient();
    return;
  }

  CS_ReadParameter(CS_ZCL_OTAU_DEFAULT_UPGRADE_SERVER_IEEE_ADDRESS_ID, &serverExtAddr);

#if APP_SUPPORT_OTAU_RECOVERY == 1
  if (PDS_IsAbleToRestore(OTAU_IMGFILES_MEMORY_MEM_ID) &&
      PDS_IsAbleToRestore(OTAU_AUXFILES_MEMORY_MEM_ID) &&
      PDS_IsAbleToRestore(OTAU_OFFSETFILES_MEMORY_MEM_ID))
  {
    PDS_Restore(OTAU_IMGFILES_MEMORY_MEM_ID);
    PDS_Restore(OTAU_AUXFILES_MEMORY_MEM_ID);
    PDS_Restore(OTAU_OFFSETFILES_MEMORY_MEM_ID);

    if (isExtAddrValid(otauServerExtAddr))
    {
      COPY_EXT_ADDR(serverExtAddr, otauServerExtAddr);
    }
  }
  else
  {
    otauClearPdsParams();
  }
#endif // APP_SUPPORT_OTAU_RECOVERY == 1

  zclClearOtauClientMemory();

  // process pending img notify(if from another server or for different file)
  if (IS_IMGNTFY_PENDING(imgNtfyServer.addr))
  {
    OTAU_SET_STATE(stateMachine, OTAU_GET_IEEE_ADDR_STATE);
    otauIeeeAddrReq(&imgNtfyServer.addr);
  }
  else
  {
    OTAU_SET_STATE(stateMachine, OTAU_WAIT_TO_DISCOVER_STATE);
    otauStartDiscovery();
  }
}

/***************************************************************************//**
\brief Start write image part
******************************************************************************/
void otauStartWrite(void)
{
  ZCL_OtauClientMem_t *clientMem = zclGetOtauClientMem();

  if ((!OTAU_CHECK_STATE(stateMachine, OTAU_GET_IMAGE_BLOCKS_STATE)) && \
      (!OTAU_CHECK_STATE(stateMachine, OTAU_GET_IMAGE_PAGES_STATE)) && \
      (!OTAU_CHECK_STATE(stateMachine, OTAU_GET_MISSED_BLOCKS_STATE)) && \
      (!OTAU_CHECK_STATE(stateMachine, OTAU_WAIT_TO_UPGRADE_STATE)))
  {
    SYS_E_ASSERT_ERROR(false, ZCL_OTAU_INVALID_STATE_OFD_WRITE);
    return;
  }

  OFD_Write(OFD_POSITION_1, &clientMem->memParam, otauWriteCallback);
}

/***************************************************************************//**
\brief Start flush last image part
******************************************************************************/
void otauStartFlush(void)
{
  ZCL_OtauClientMem_t *clientMem = zclGetOtauClientMem();

  OFD_FlushAndCheckCrc(OFD_POSITION_1, clientMem->otauParam.imageBlockData, otauFlushCallback);
}

/***************************************************************************//**
\brief Callback from flash driver about end of image switching

\param[in] status - status of action;
******************************************************************************/
void otauSwitchCallback(OFD_Status_t status)
{
  switch (status)
  {
    case OFD_STATUS_SUCCESS:
      zclRaiseCustomMessage(OTAU_DOWNLOAD_FINISHED);
      // change ugrade status
      otauClientAttributes.imageUpgradeStatus.value = OTAU_DOWNLOAD_COMPLETE;
      // send upgrade end request with SUCCESS
#if APP_SUPPORT_OTAU_RECOVERY == 1
      PDS_Store(OTAU_IMGFILES_MEMORY_MEM_ID);
      PDS_Store(OTAU_AUXFILES_MEMORY_MEM_ID);
      PDS_Store(OTAU_OFFSETFILES_MEMORY_MEM_ID);
#endif
      retryCount = otauMaxRetryCount;
      otauUpgradeEndStatus = ZCL_SUCCESS_STATUS;
      otauStartGenericTimer(UPGRADE_END_REQ_QUICK_TIMEOUT, otauUpgradeEndReq);
      break;
    case OFD_STATUS_SERIAL_BUSY:
    case OFD_SERIAL_INTERFACE_BUSY:
      otauStartGenericTimer(REPOST_OFD_ACTION, otauStartSwitch);
      break;
    default:
      // reset upgrade status
      SYS_E_ASSERT_ERROR(false, ZCL_OTAU_DOWNLOAD_ABORTED);
      otauClientAttributes.imageUpgradeStatus.value = OTAU_NORMAL;
      retryCount = otauMaxRetryCount;
      otauUpgradeEndStatus = ZCL_ABORT_STATUS;
      otauStartGenericTimer(UPGRADE_END_REQ_QUICK_TIMEOUT, otauUpgradeEndReq);
      zclRaiseCustomMessage(OTAU_OFD_DRIVER_ERROR);
      break;
  }
}

/***************************************************************************//**
\brief Start switch between images
******************************************************************************/
void otauStartSwitch(void)
{
  ZCL_OtauClientMem_t *clientMem = zclGetOtauClientMem();

  otauWriteSkipSectionToEeprom();

  OFD_SwitchToNewImage(OFD_POSITION_2, OFD_POSITION_1, clientMem->otauParam.imageBlockData, otauSwitchCallback);
}

/***************************************************************************//**
\brief Callback from flash driver about end of data flushing

\param[in] status - status of action;
\param[in] imageInfo - pointer to image info
******************************************************************************/
void otauFlushCallback(OFD_Status_t status, OFD_ImageInfo_t *imageInfo)
{
  (void)imageInfo;

  switch (status)
  {
    case OFD_STATUS_SUCCESS:
      otauStartGenericTimer(REPOST_OFD_ACTION, otauStartSwitch);
      break;
    case OFD_STATUS_SERIAL_BUSY:
    case OFD_SERIAL_INTERFACE_BUSY:
      // try to flush again
      otauStartGenericTimer(REPOST_OFD_ACTION, otauStartFlush);
      break;
    default:
      // reset upgrade status
      SYS_E_ASSERT_ERROR(false, ZCL_OTAU_DOWNLOAD_ABORTED);
      otauClientAttributes.imageUpgradeStatus.value = OTAU_NORMAL;
      retryCount = otauMaxRetryCount;
      // send upgrade end request with INVALID IMAGE status. When default respose will be received
      // server descovery process will start
      otauUpgradeEndStatus = ZCL_ABORT_STATUS;
      otauStartGenericTimer(UPGRADE_END_REQ_QUICK_TIMEOUT, otauUpgradeEndReq);
      break;
  }
}

/***************************************************************************//**
\brief Writes the received image to external flash
******************************************************************************/
void otauProcessSuccessfullWritingToFlash(void)
{
  ZCL_OtauClientMem_t *clientMem = zclGetOtauClientMem();
  OtauImageAuxVar_t *tmpAuxParam = &clientMem->imageAuxParam;
  OFD_MemoryAccessParam_t *tmpMemParam = &clientMem->memParam;

#if APP_SUPPORT_OTAU_PAGE_REQUEST == 1
  bool pageRequestUsed;
  CS_ReadParameter(CS_ZCL_OTAU_IMAGE_PAGE_REQUEST_ENABLE_ID, &pageRequestUsed);
#endif

  if (OTAU_CHECK_STATE(stateMachine, OTAU_GET_IMAGE_BLOCKS_STATE) || \
      OTAU_CHECK_STATE(stateMachine, OTAU_GET_IMAGE_PAGES_STATE) || \
      OTAU_CHECK_STATE(stateMachine, OTAU_GET_MISSED_BLOCKS_STATE))
  {
#if APP_SUPPORT_OTAU_RECOVERY == 1
    otauRunningChecksum = otauCalcCrc(otauRunningChecksum, tmpMemParam->data, tmpMemParam->length);
#endif

    tmpMemParam->offset += tmpMemParam->length;

#if APP_SUPPORT_OTAU_RECOVERY == 1
    otauFlashWriteOffset = tmpMemParam->offset;
    
    if (otauNextOffset >= otauNextPdsWriteOffset)
    {
      PDS_Store(OTAU_OFFSETFILES_MEMORY_MEM_ID);
      otauNextPdsWriteOffset = otauNextOffset + otauPdsWriteFreqInBytes;
      if (otauNextPdsWriteOffset > (otauImageSize-1))
      {
        otauNextPdsWriteOffset = (otauImageSize-1);
      }
    }
#endif

    if (0 == tmpAuxParam->imageInternalLength)
    {
      if (IMAGE_CRC_SIZE == tmpAuxParam->imageRemainder)
      { // dowload complete
        OTAU_SET_STATE(stateMachine, OTAU_WAIT_TO_UPGRADE_STATE);
        otauNextPdsWriteOffset = clientMem->otauParam.imageSize;
        otauStartGenericTimer(REPOST_OFD_ACTION, otauStartFlush);
        return;
      }
      else
      { // download continue with next subimage
        tmpAuxParam->internalAddressStatus = 0;
        tmpAuxParam->currentDataSize = AUXILIARY_STRUCTURE_IS_FULL;

#if APP_SUPPORT_OTAU_PAGE_REQUEST == 1
        if (pageRequestUsed)
          clientMem->blockRequest = OTAU_BLOCK_REQUEST_USAGE;
#endif

      }
    }
    else
    {
      otauCountActuallyDataSize();
    }

    otauStartDownload();
    return;
  }
}

/***************************************************************************//**
\brief Callback from flash driver about end of data writing

\param[in] status - status of action;
******************************************************************************/
void otauWriteCallback(OFD_Status_t status)
{
  if ((!OTAU_CHECK_STATE(stateMachine, OTAU_GET_IMAGE_BLOCKS_STATE)) && \
      (!OTAU_CHECK_STATE(stateMachine, OTAU_GET_IMAGE_PAGES_STATE)) && \
      (!OTAU_CHECK_STATE(stateMachine, OTAU_GET_MISSED_BLOCKS_STATE)) && \
      (!OTAU_CHECK_STATE(stateMachine, OTAU_WAIT_TO_UPGRADE_STATE)))
  {
    SYS_E_ASSERT_ERROR(false, ZCL_OTAU_INVALID_STATE_OFD_WRITE_CALLBACK);
    return;
  }

  switch(status)
  {
    case OFD_STATUS_SERIAL_BUSY:
    case OFD_SERIAL_INTERFACE_BUSY:
      otauStartGenericTimer(REPOST_OFD_ACTION, otauStartWrite);
      break;

    case OFD_STATUS_SUCCESS:
      ofdWriteRetry = otauMaxRetryCount;
      // calculate Running CRC and write it to PDS here
      otauProcessSuccessfullWritingToFlash();
      break;

    default:
      if (ofdWriteRetry--)
      {
        otauStartGenericTimer(REPOST_OFD_ACTION, otauStartWrite);
      }
      else
      {
        SYS_E_ASSERT_ERROR(false, ZCL_OTAU_DOWNLOAD_ABORTED);
        ofdWriteRetry = otauMaxRetryCount;
        retryCount = otauMaxRetryCount;
        zclRaiseCustomMessage(OTAU_OFD_DRIVER_ERROR);
        otauClientAttributes.imageUpgradeStatus.value       = OTAU_NORMAL;
        otauClientAttributes.minimumBlockRequestDelay.value = 0;
        otauUpgradeEndStatus = ZCL_ABORT_STATUS;
        otauStartGenericTimer(UPGRADE_END_REQ_QUICK_TIMEOUT, otauUpgradeEndReq);
        OTAU_SET_STATE(stateMachine, OTAU_STOPPED_STATE);
      }
      break;
  }
}
/***************************************************************************//**
\brief Start erase image area
******************************************************************************/
void otauStartErase(void)
{
  if ((!OTAU_CHECK_STATE(stateMachine, OTAU_GET_IMAGE_BLOCKS_STATE)) && \
      (!OTAU_CHECK_STATE(stateMachine, OTAU_GET_IMAGE_PAGES_STATE)) && \
      (!OTAU_CHECK_STATE(stateMachine, OTAU_WAIT_TO_UPGRADE_STATE)))
  {
    SYS_E_ASSERT_ERROR(false, ZCL_OTAU_INVALID_STATE_OFD_ERASE);
    return;
  }

  OFD_EraseImage(OFD_POSITION_1, otauEraseCallback);
}

#if APP_SUPPORT_OTAU_RECOVERY == 1
/***************************************************************************//**
\brief Clears all PDS parameters related to OTAU
******************************************************************************/
void otauClearPdsParams(void)
{
  ExtAddr_t zeroAddr = ZERO_SERVER_EXT_ADDRESS;

  otauRunningChecksum     = 0xFF;
  otauNextOffset          = 0;
  otauImageVersion        = 0;
  otauImageSize           = 0;
  otauInternalAddrStatus  = 0;
  otauInternalLength      = 0;
  otauFlashWriteOffset    = 0;
  otauImageRemainder      = 0;
  COPY_EXT_ADDR(otauServerExtAddr, zeroAddr);

  PDS_Store(OTAU_IMGFILES_MEMORY_MEM_ID);
  PDS_Store(OTAU_AUXFILES_MEMORY_MEM_ID);
  PDS_Store(OTAU_OFFSETFILES_MEMORY_MEM_ID);
}

/***************************************************************************//**
\brief Configures the next write offset for PDS/NVM
******************************************************************************/
void otauConfigureNextPdsWriteOffset(void)
{
  ZCL_OtauClientMem_t *clientMem = zclGetOtauClientMem();
  
  /* next PDS update happens at filesize/frequency */
  otauPdsWriteFreqInBytes = otauImageSize/OTAU_NUM_OF_PDS_WRITES;
  if (!otauPdsWriteFreqInBytes)
  {
    /* forces write every packet */
    otauPdsWriteFreqInBytes = 1;
  }
  else if (otauPdsWriteFreqInBytes == otauImageSize)
  {
    otauPdsWriteFreqInBytes = otauImageSize/2;
  }
  
  otauNextPdsWriteOffset = clientMem->imageAuxParam.currentFileOffset + otauPdsWriteFreqInBytes;
  if (otauNextPdsWriteOffset > (otauImageSize-1))
  {
    otauNextPdsWriteOffset = (otauImageSize-1);
  }
}
#endif // APP_SUPPORT_OTAU_RECOVERY == 1

/***************************************************************************//**
\brief Callback from flash driver about end of flash erasing

\param[in] status - status of action;
******************************************************************************/
void otauEraseCallback(OFD_Status_t status)
{
  ZCL_OtauClientMem_t *clientMem = zclGetOtauClientMem();

  if ((!OTAU_CHECK_STATE(stateMachine, OTAU_GET_IMAGE_BLOCKS_STATE)) && \
      (!OTAU_CHECK_STATE(stateMachine, OTAU_GET_IMAGE_PAGES_STATE)) && \
      (!OTAU_CHECK_STATE(stateMachine, OTAU_WAIT_TO_UPGRADE_STATE)))
  {
    SYS_E_ASSERT_ERROR(false, ZCL_OTAU_INVALID_STATE_OFD_ERASE_CALLBACK);
    return;
  }

  switch(status)
  {
    case OFD_STATUS_SUCCESS:
      if (OTAU_CHECK_STATE(stateMachine, OTAU_GET_IMAGE_BLOCKS_STATE) || \
          OTAU_CHECK_STATE(stateMachine, OTAU_GET_IMAGE_PAGES_STATE))
      {
        clientMem->imageAuxParam.currentFileOffset     = sizeof(ZCL_OtauSubElementHeader_t) + sizeof(ZCL_OtauUpgradeImageHeader_t);
        clientMem->imageAuxParam.currentDataSize       = AUXILIARY_STRUCTURE_IS_FULL;
        clientMem->imageAuxParam.imageInternalLength   = 0ul;
        clientMem->imageAuxParam.internalAddressStatus = 0;
        clientMem->imageAuxParam.imageRemainder        = clientMem->otauParam.imageSize - clientMem->imageAuxParam.currentFileOffset;
        clientMem->firstStart                          = OTAU_CONTINUE_CLIENT_WORK;
#if APP_SUPPORT_OTAU_PAGE_REQUEST == 1        
        clientMem->missedBytesGetting                  = OTAU_NOT_GET_MISSED_BYTES;
#endif

#if APP_SUPPORT_OTAU_RECOVERY == 1
        otauRunningChecksum     = 0xFF;
        otauNextOffset          = 0;
        otauInternalAddrStatus  = 0;
        otauInternalLength      = 0;
        otauFlashWriteOffset    = 0;
        otauImageSize = clientMem->otauParam.imageSize;
        otauImageVersion = clientMem->newFirmwareVersion.memAlloc;
        COPY_EXT_ADDR(otauServerExtAddr, serverExtAddr);

        PDS_Store(OTAU_IMGFILES_MEMORY_MEM_ID);
        otauConfigureNextPdsWriteOffset();
#endif

        otauStartDownload();
      }
      break;

    case OFD_STATUS_SERIAL_BUSY:
    case OFD_SERIAL_INTERFACE_BUSY:
      otauStartGenericTimer(REPOST_OFD_ACTION, otauStartErase);
      break;

    default:
      SYS_E_ASSERT_ERROR(false, ZCL_OTAU_DOWNLOAD_ABORTED);
      zclRaiseCustomMessage(OTAU_OFD_DRIVER_ERROR);
      otauClientAttributes.imageUpgradeStatus.value = OTAU_NORMAL;
      OTAU_SET_STATE(stateMachine, OTAU_STOPPED_STATE);
      zclStopOtauClient();
      break;
  }
}

#if APP_SUPPORT_OTAU_RECOVERY == 1
/***************************************************************************//**
\brief Get the CRC of the image stored in external memory at given position

\param[in] position - image position in external memory
******************************************************************************/
void otauGetCrc(void)
{
  ZCL_OtauClientMem_t *clientMem = zclGetOtauClientMem();

  if ((!OTAU_CHECK_STATE(stateMachine, OTAU_GET_IMAGE_BLOCKS_STATE)) && \
      (!OTAU_CHECK_STATE(stateMachine, OTAU_GET_IMAGE_PAGES_STATE)))
  {
    SYS_E_ASSERT_ERROR(false, ZCL_OTAU_INVALID_STATE_OFD_GET_CRC);
    return;
  }

  OFD_CalCrc(OFD_POSITION_1, clientMem->otauParam.imageBlockData, otauNextOffset,
             otauRunningChecksum, otauGetCrcCallback);
}

/***************************************************************************//**
\brief Callback from flash driver for the end of CRC computation

\param[in] status - status of action
\param[in] checksum - pointer to the computed checksum
******************************************************************************/
void otauGetCrcCallback(OFD_Status_t status, OFD_ImageInfo_t *imageInfo)
{
  ZCL_OtauClientMem_t *clientMem = zclGetOtauClientMem();
  OtauImageAuxVar_t *tmpAuxParam = &clientMem->imageAuxParam;
  ZclOtauClientImageBuffer_t *tmpParam = &clientMem->otauParam;

  if ((!OTAU_CHECK_STATE(stateMachine, OTAU_GET_IMAGE_BLOCKS_STATE)) && \
      (!OTAU_CHECK_STATE(stateMachine, OTAU_GET_IMAGE_PAGES_STATE)))
  {
    SYS_E_ASSERT_ERROR(false, ZCL_OTAU_INVALID_STATE_OFD_CRC_CALLBACK);
    return;
  }

  switch(status)
  {
    case OFD_STATUS_SUCCESS:
      if (otauRunningChecksum == imageInfo->crc)
      {
        tmpAuxParam->currentFileOffset		 = otauNextOffset;
        tmpAuxParam->imageInternalLength	 = otauInternalLength;
        tmpAuxParam->internalAddressStatus	 = otauInternalAddrStatus;
        tmpAuxParam->imageRemainder 		 = otauImageSize - otauInternalLength;
#if APP_SUPPORT_OTAU_PAGE_REQUEST == 1
        CS_ReadParameter(CS_ZCL_OTAU_IMAGE_PAGE_REQUEST_PAGE_SIZE_ID, &tmpAuxParam->lastPageSize);
        tmpAuxParam->imagePageOffset		 = otauNextOffset;
#endif
        COPY_EXT_ADDR(clientMem->otauServer, serverExtAddr);
        tmpParam->imageSize = otauImageSize;
        clientMem->newFirmwareVersion.memAlloc = otauImageVersion;

        otauStartDownload();
      }
      else
      {
        otauStartErase();
      }
      break;

    case OFD_SERIAL_INTERFACE_BUSY:
    case OFD_STATUS_SERIAL_BUSY:
      otauStartGenericTimer(REPOST_OFD_ACTION, otauGetCrc);
      break;

    default:
      otauStartErase();
      break;
  }
}
#endif // APP_SUPPORT_OTAU_RECOVERY == 1

#if (USE_IMAGE_SECURITY == 1)
/**************************************************************************//**
\brief Starts eeprom delayed transaction
******************************************************************************/
void otauStartEepromDelayedTransaction(void)
{
  ZCL_OtauClientMem_t *clientMem = zclGetOtauClientMem();

  if (!HAL_IsEepromBusy())
  {
    otauReadImgTypeFromEeprom();
  }
  else
  {
    otauStartEepromPollTimer(otauStartEepromDelayedTransaction);
  }
}

/***************************************************************************//**
\brief Sends the new image req with the updated imgtype value
******************************************************************************/
void otauSendReqWithNewImgType(void)
{
  ZCL_OtauClientMem_t *clientMem = zclGetOtauClientMem();
  ZCL_OtauQueryNextImageReq_t *tmpOtauReq = &clientMem->zclReqMem.uQueryNextImageReq;
  ZCL_Request_t *tmpZclReq = &clientMem->reqMem.zclCommandReq;
  uint16_t csManufacturerId;
  CS_ReadParameter(CS_MANUFACTURER_CODE_ID, &csManufacturerId);
  
  //Enabling fast polling for Query Next Image
  if (!isOtauBusy)
  {
    zclRaiseCustomMessage(OTAU_QUERY_NEXT_IMAGE_INITIATED);
    isOtauBusy = true;
  }

  zclOtauFillOutgoingZclRequest(QUERY_NEXT_IMAGE_REQUEST_ID, sizeof(ZCL_OtauQueryNextImageReq_t), (uint8_t *)tmpOtauReq);

  tmpOtauReq->controlField.hardwareVersionPresent = 0;
  tmpOtauReq->controlField.reserved               = 0;
  tmpOtauReq->manufacturerId                      = csManufacturerId;
  tmpOtauReq->imageType                           = clientMem->eepromImgType;
  tmpOtauReq->currentFirmwareVersion              = zclOtauMem.initParam.firmwareVersion;

  ZCL_CommandReq(tmpZclReq);
}

/***************************************************************************//**
\brief Read done callback from EEPROM
******************************************************************************/
void readEepromCallback(void)
{
  ZCL_OtauClientMem_t *clientMem = zclGetOtauClientMem();

  switch(clientMem->eepromImgTypeVal & OTAU_MANU_SPEC_ENCRYPTED_IMAGE_ENCRYPTED_MAC_IMAGE_TYPE)
  {
  case OTAU_MANU_SPEC_ENCRYPTED_NO_MIC_IMAGE_TYPE:
  case OTAU_MANU_SPEC_ENCRYPTED_IMAGE_PLAIN_CRC_IMAGE_TYPE:
  case OTAU_MANU_SPEC_ENCRYPTED_IMAGE_PLAIN_MAC_IMAGE_TYPE:
  case OTAU_MANU_SPEC_ENCRYPTED_IMAGE_ENCRYPTED_CRC_IMAGE_TYPE:
  case OTAU_MANU_SPEC_ENCRYPTED_IMAGE_ENCRYPTED_MAC_IMAGE_TYPE:
    clientMem->eepromImgType = (ZCL_OtauImageType_t)clientMem->eepromImgTypeVal;
    break;
  default:
    clientMem->eepromImgType = OTAU_SPECIFIC_IMAGE_TYPE;
    break;
  }

  if (false == clientMem->isImgTypeValid)
  {
    clientMem->isImgTypeValid = true;
  }

  otauSendReqWithNewImgType();
}

/***************************************************************************//**
\brief Reads imagetype from EEPROM offset EEPROM_IMGTYPE_OFFSET
******************************************************************************/
void otauReadImgTypeFromEeprom(void)
{
  HAL_EepromParams_t params;
  ZCL_OtauClientMem_t *clientMem = zclGetOtauClientMem();

  if (!clientMem->isImgTypeValid)
  {
    int status;

    params.address = EEPROM_IMGTYPE_OFFSET;
    params.data = (uint8_t *)&clientMem->eepromImgTypeVal;
    params.length = sizeof(uint16_t);

    status = HAL_ReadEeprom(&params, readEepromCallback);
    if (OTAU_READ_EEPROM_BUSY == status)
    {
      // busy. poll after 10ms
      otauStartEepromPollTimer(otauStartEepromDelayedTransaction);
    }

    // success or busy are handled in callback
    return;
  }

  // no-eeprom-read case
  otauSendReqWithNewImgType();
  return;
}

/***************************************************************************//**
\brief configure OTAU image key

\param[in] configureKeyDone - callback after key is written.
                              can be set to NULL if callback is not required

\return None
******************************************************************************/
void ZCL_ConfigureOtauImageKey(void (*configureKeyDone)())
{
  configureKeyDoneCallback = configureKeyDone;
  writeKeyToEeprom();
}

/***************************************************************************//**
\brief writing the key to EEPROM
******************************************************************************/
void writeKeyToEeprom(void)
{
  HAL_EepromParams_t params;
  int status;

  params.address = OTAU_IMAGE_KEY_START_ADDRESS_OFFSET;
  params.data = (uint8_t *)&otauImageKey;
  params.length = sizeof(otauImageKey);

  // for succes, wait for cbm
  status = HAL_WriteEeprom(&params, writeEepromCallback);
  if (OTAU_WRITE_EEPROM_BUSY == status)
  {
    // busy. poll after 10ms
    otauStartEepromPollTimer(writeKeyToEeprom);
  }
}

/***************************************************************************//**
\brief write done callback from EEPROM
******************************************************************************/
void writeEepromCallback(void)
{
  if (configureKeyDoneCallback)
  {
    configureKeyDoneCallback();
  }
}
#endif /* USE_IMAGE_SECURITY */

/***************************************************************************//**
\brief write done callback from EEPROM
******************************************************************************/
void writeSkipSectionToEepromCallback(void)
{
  otauWriteEepromAction();
}

/***************************************************************************//**
\brief write done callback from EEPROM
******************************************************************************/
void writeEepromActionFlagCallback(void)
{

}

/***************************************************************************//**
\brief write eeprom action flag to eeprom
******************************************************************************/
void otauWriteEepromAction(void)
{
  HAL_EepromParams_t params;
  int status;

  params.address = OTAU_EEPROM_ACTION_FLAG_OFFSET;
  params.data    = &eepromInNewImage;
  params.length  = OTAU_EEEPROM_ACTION_FLAG_LENGTH;

  // for succes, wait for cb
  status = HAL_WriteEeprom(&params, writeEepromActionFlagCallback);
  if (OTAU_WRITE_EEPROM_BUSY == status)
  {
    // busy. poll after 10ms
    otauStartEepromPollTimer(otauWriteEepromAction);
  }
}

/**************************************************************************//**
\brief Starts timer to start delayed EEPROM read
******************************************************************************/
void otauStartEepromPollTimer(void (*delayedTransaction)())
{
  zclEepromBusyTimer.callback = delayedTransaction;

  HAL_StartAppTimer(&zclEepromBusyTimer);
}

/***************************************************************************//**
\brief writing the key to EEPROM
******************************************************************************/
void otauWriteSkipSectionToEeprom(void)
{
  HAL_EepromParams_t params;
  int status;

  writeSkipSecStartEnd[0] = OTAU_WRITESKIP_MEMORY_START;
  writeSkipSecStartEnd[1] = OTAU_WRITESKIP_MEMORY_END;

  params.address = OTAU_FLASH_WRITE_SKIP_START_EEPROM_OFFSET;
  params.data    = (uint8_t *)&writeSkipSecStartEnd;
  params.length  = OTAU_FLASH_WRITE_SKIP_BYTES_LENGTH;

  // for succes, wait for cb
  status = HAL_WriteEeprom(&params, writeSkipSectionToEepromCallback);
  if (OTAU_WRITE_EEPROM_BUSY == status)
  {
    // busy. poll after 10ms
    otauStartEepromPollTimer(otauWriteSkipSectionToEeprom);
  }
}

/***************************************************************************//**
\brief All requests confirm.

\param[in] resp - pointer to response parametres.
******************************************************************************/
void otauSomeRequestConfirm(ZCL_Notify_t *resp)
{
  ZCL_OtauClientMem_t *clientMem = zclGetOtauClientMem();
  ZCL_Request_t *tmpZclReq = GET_PARENT_BY_FIELD(ZCL_Request_t, notify, resp);
#if APP_SUPPORT_OTAU_PAGE_REQUEST == 1
  bool pageRequestUsed;
  HAL_AppTimer_t *tmpPageReqTimer = &clientMem->pageRequestTimer;
#else
  (void)clientMem;
#endif // APP_SUPPORT_OTAU_PAGE_REQUEST == 1

  switch (resp->status)
  {
    case ZCL_SUCCESS_STATUS: // received on aps ack / low-layer confirmation
#if APP_SUPPORT_OTAU_PAGE_REQUEST == 1
      if (IMAGE_PAGE_REQUEST_ID == tmpZclReq->id)
      {
        CS_ReadParameter(CS_ZCL_OTAU_IMAGE_PAGE_REQUEST_ENABLE_ID, &pageRequestUsed);
        if (pageRequestUsed && (IMAGE_PAGE_REQUEST_ID == tmpZclReq->id))
        {
          uint16_t responseSpacing;
          otauStopGenericTimer();

          HAL_StopAppTimer(tmpPageReqTimer);
          CS_ReadParameter(CS_ZCL_OTAU_IMAGE_PAGE_REQUEST_RESPONSE_SPACING_ID, &responseSpacing);
          tmpPageReqTimer->interval  = ((NWK_GetUnicastDeliveryTime() + responseSpacing) * 2);
          tmpPageReqTimer->mode      = TIMER_ONE_SHOT_MODE;
          tmpPageReqTimer->callback  = otauImagePageReqIntervalElapsed;
          HAL_StartAppTimer(tmpPageReqTimer);
        }
      }
#endif // APP_SUPPORT_OTAU_PAGE_REQUEST == 1
      break;

    default: // received on zcl wait timer expiry
      if (retryCount--)
      {
        switch (tmpZclReq->id)
        {
#if APP_SUPPORT_OTAU_PAGE_REQUEST == 1
          case IMAGE_PAGE_REQUEST_ID:
            if (IS_IMGNTFY_PENDING(imgNtfyServer.addr))
            {
              if (ZCL_SUCCESS_STATUS != otauCheckServerAddrAndTakeAction(false, true))
              {
                otauStopGenericTimer();
                otauImagePageReq();
              }
            }
            else
            {
              otauStopGenericTimer();
              otauImagePageReq();
            }
            break;
#endif // APP_SUPPORT_OTAU_PAGE_REQUEST == 1

          case IMAGE_BLOCK_REQUEST_ID:
            if (IS_IMGNTFY_PENDING(imgNtfyServer.addr))
            {
              if (ZCL_SUCCESS_STATUS != otauCheckServerAddrAndTakeAction(false, true))
              {
                otauImageBlockReq();
              }
            }
            else
            {
              otauImageBlockReq();
            }
            break;

          case QUERY_NEXT_IMAGE_REQUEST_ID:
            if (IS_IMGNTFY_PENDING(imgNtfyServer.addr))
            {
              otauCheckServerAddrAndTakeAction(false, false);
            }
            else
            {
              otauQueryNextImageReq();
            }
            break;

          case UPGRADE_END_REQUEST_ID:
            if (IS_IMGNTFY_PENDING(imgNtfyServer.addr))
            {
              if (ZCL_SUCCESS_STATUS != otauCheckServerAddrAndTakeAction(false, true))
              {
                if (ZCL_SUCCESS_STATUS == otauUpgradeEndStatus)
                {
                  otauUpgradeEndStatus = ZCL_SUCCESS_STATUS;
                  otauStartGenericTimer(UPGRADE_END_REQ_QUICK_TIMEOUT, otauUpgradeEndReq);
                }
              }
            }
            else
            {
              if (ZCL_SUCCESS_STATUS == otauUpgradeEndStatus)
              {
                otauUpgradeEndStatus = ZCL_SUCCESS_STATUS;
                otauStartGenericTimer(UPGRADE_END_REQ_QUICK_TIMEOUT, otauUpgradeEndReq);
              }
            }
            break;
        }
      }
      else
      {
        retryCount = otauMaxRetryCount;
        if (IS_IMGNTFY_PENDING(imgNtfyServer.addr))
        {
          switch (tmpZclReq->id)
          {
            case QUERY_NEXT_IMAGE_REQUEST_ID:
              otauCheckServerAddrAndTakeAction(false, false);
              break;
            case IMAGE_PAGE_REQUEST_ID:
            case IMAGE_BLOCK_REQUEST_ID:
            case UPGRADE_END_REQUEST_ID:
              if (ZCL_SUCCESS_STATUS != otauCheckServerAddrAndTakeAction(false, true))
              {
                if (UPGRADE_END_REQUEST_ID == tmpZclReq->id)
                {
                  // handle non-success at otauUpgradeEndStatus assignment place
                  if (ZCL_SUCCESS_STATUS == otauUpgradeEndStatus)
                  {
                    //do switch : OTAU r23, sec 6.13
#if APP_SUPPORT_OTAU_RECOVERY == 1
                    otauClearPdsParams();
#endif // APP_SUPPORT_OTAU_RECOVERY == 1
                    otauStartGenericTimer(AMOUNT_MSEC_IN_SEC, otauFinalizeProcess);
                  }
                }
                else
                {
                  OTAU_SET_STATE(stateMachine, OTAU_WAIT_TO_DISCOVER_STATE);
                  otauStartDiscoveryTimer();
                }
              }
            default:
              break;
          }
        }
        else
        {
          if (UPGRADE_END_REQUEST_ID == tmpZclReq->id)
          {
            // handle non-success at otauUpgradeEndStatus assignment place
            if (ZCL_SUCCESS_STATUS == otauUpgradeEndStatus)
            {
              //do switch : OTAU r23, sec 6.13
#if APP_SUPPORT_OTAU_RECOVERY == 1
              otauClearPdsParams();
#endif // APP_SUPPORT_OTAU_RECOVERY == 1
              otauStartGenericTimer(AMOUNT_MSEC_IN_SEC, otauFinalizeProcess);
            }
          }
          else
          {
            OTAU_SET_STATE(stateMachine, OTAU_WAIT_TO_DISCOVER_STATE);
            otauStartDiscoveryTimer();
          }
        }
      }
      break;
  }
}

/***************************************************************************//**
\brief otau default response handler.

\param[in] resp - pointer to response parametres.
******************************************************************************/
void otauSomeDefaultResponse(ZCL_Addressing_t *addressing, uint8_t payloadLength, uint8_t *payload)
{
  ZCL_OtauClientMem_t *clientMem = zclGetOtauClientMem();
  ZCL_DefaultResp_t *defaultResp = (ZCL_DefaultResp_t*)payload;
  ZCL_Request_t *tmpZclReq = &clientMem->reqMem.zclCommandReq;

  switch(tmpZclReq->id)
  {
    case UPGRADE_END_REQUEST_ID:
    {
      switch (defaultResp->statusCode)
      {
        case ZCL_SUCCESS_STATUS:
        default:
        {
          retryCount = otauMaxRetryCount;

          if (ZCL_SUCCESS_STATUS != otauUpgradeEndStatus)
          {
            if (ZCL_ABORT_STATUS == otauUpgradeEndStatus)
            {
              zclRaiseCustomMessage(OTAU_DOWNLOAD_ABORTED);
              otauClientAttributes.minimumBlockRequestDelay.value = 0;
            }
            else
            {
              otauClientAttributes.imageUpgradeStatus.value = OTAU_NORMAL;
              OTAU_SET_STATE(stateMachine, OTAU_WAIT_TO_DISCOVER_STATE);
              otauStartDiscoveryTimer();
            }
            isOtauBusy = false;
            otauUpgradeEndStatus = ZCL_SUCCESS_STATUS;
          }
        }
        break;

        case ZCL_ABORT_STATUS:
        case ZCL_INVALID_IMAGE_STATUS:
        case ZCL_MALFORMED_COMMAND_STATUS:
        {
          if (ZCL_SUCCESS_STATUS == otauUpgradeEndStatus)
          {
            if (retryCount--)
            {
              otauStartGenericTimer(UPGRADE_END_REQ_QUICK_TIMEOUT, otauUpgradeEndReq);
            }
            else
            {
              retryCount = otauMaxRetryCount;
  #if APP_SUPPORT_OTAU_RECOVERY == 1
              otauClearPdsParams();
  #endif // APP_SUPPORT_OTAU_RECOVERY == 1
              clientMem->imageAuxParam.currentFileOffset     = sizeof(ZCL_OtauSubElementHeader_t) + sizeof(ZCL_OtauUpgradeImageHeader_t);
              clientMem->imageAuxParam.currentDataSize       = AUXILIARY_STRUCTURE_IS_FULL;
              clientMem->imageAuxParam.imageInternalLength   = 0;
              clientMem->imageAuxParam.internalAddressStatus = 0;
              clientMem->imageAuxParam.imageRemainder        = 0;
              clientMem->firstStart                          = OTAU_FIRST_CLIENT_START;
              clientMem->otauParam.imageSize                 = 0;
              clientMem->newFirmwareVersion.memAlloc         = 0;

              isOtauBusy = false;
              OTAU_SET_STATE(stateMachine, OTAU_WAIT_TO_DISCOVER_STATE);
              otauStartDiscoveryTimer();
            }
          }
          else
          {
            if (ZCL_ABORT_STATUS == otauUpgradeEndStatus)
            {
              zclRaiseCustomMessage(OTAU_DOWNLOAD_ABORTED);
              otauClientAttributes.minimumBlockRequestDelay.value = 0;
            }
            else
            {
              otauClientAttributes.minimumBlockRequestDelay.value = 0;
              otauClientAttributes.imageUpgradeStatus.value = OTAU_NORMAL;
              OTAU_SET_STATE(stateMachine, OTAU_WAIT_TO_DISCOVER_STATE);
              otauStartDiscoveryTimer();
            }
            isOtauBusy = false;
            otauUpgradeEndStatus = ZCL_SUCCESS_STATUS;
          }
        }
        break;
      }
    }
    break;

#if APP_SUPPORT_OTAU_PAGE_REQUEST == 1
    case IMAGE_PAGE_REQUEST_ID:
      if (ZCL_SUCCESS_STATUS != defaultResp->statusCode)
      {
        HAL_StopAppTimer(&clientMem->pageRequestTimer);
      }
      //fall through
#endif
    default:
      // Goto discovery for "default-response-with-non-success"
      if (ZCL_SUCCESS_STATUS != defaultResp->statusCode)
      {
        otauClientAttributes.minimumBlockRequestDelay.value = 0;
        otauClientAttributes.imageUpgradeStatus.value = OTAU_NORMAL;
        OTAU_SET_STATE(stateMachine, OTAU_WAIT_TO_DISCOVER_STATE);
        otauStartDiscoveryTimer();
      }
      retryCount = otauMaxRetryCount;
      break;
  }

  (void)addressing;
  (void)payloadLength;
}

/***************************************************************************//**
\brief Fills ZCL_Request_t structure fields for outgoing request.

\param[in] id - zcl command id;
\param[in] length - the length of zcl command payload;
\param[in] payload - pointer to zcl command payload
******************************************************************************/
void zclOtauFillOutgoingZclRequest(uint8_t id, uint8_t length, uint8_t *payload)
{
  ZCL_Request_t *tmpZclReq = &(zclGetOtauClientMem()->reqMem.zclCommandReq);
  uint32_t rspWaitTime = NWK_GetUnicastDeliveryTime();

  SYS_E_ASSERT_ERROR((otauMaxRetryCount >= retryCount), ZCL_OTAU_INVALID_OTAURETRYCOUNT);

  /* increase the wait time with retries and ceil with predefined timeout */  
  rspWaitTime += (OTAU_DEFAULT_RESPONSE_WAIT_TIMEOUT * ((otauMaxRetryCount+1) - retryCount));

  tmpZclReq->dstAddressing.addrMode             = APS_SHORT_ADDRESS;

  tmpZclReq->dstAddressing.addr.shortAddress    = actvServer->serverShortAddress;
  tmpZclReq->dstAddressing.endpointId           = actvServer->serverEndpoint;

  tmpZclReq->dstAddressing.profileId            = zclOtauMem.initParam.profileId;
  tmpZclReq->dstAddressing.clusterId            = OTAU_CLUSTER_ID;
  tmpZclReq->dstAddressing.clusterSide          = ZCL_SERVER_CLUSTER_TYPE;
  tmpZclReq->dstAddressing.manufacturerSpecCode = 0;
  tmpZclReq->dstAddressing.sequenceNumber       = ZCL_GetNextSeqNumber();
  tmpZclReq->endpointId                         = zclOtauMem.initParam.otauEndpoint;
  tmpZclReq->id                                 = id;
  tmpZclReq->defaultResponse                    = ZCL_FRAME_CONTROL_ENABLE_DEFAULT_RESPONSE;
  tmpZclReq->requestLength                      = length;
  tmpZclReq->requestPayload                     = payload;
  tmpZclReq->responseWaitTimeout                = rspWaitTime;
  tmpZclReq->ZCL_Notify                         = otauSomeRequestConfirm;
}

/***************************************************************************//**
\brief Gets Otau's cluster

\return Otau's client cluster
******************************************************************************/
ZCL_Cluster_t ZCL_GetOtauClientCluster(void)
{
  ZCL_Cluster_t cluster = DEFINE_OTAU_CLUSTER(ZCL_CLIENT_CLUSTER_TYPE, &otauClientAttributes, &otauClientCommands);
  cluster.ZCL_DefaultRespInd = otauSomeDefaultResponse;

  return cluster;
}
#endif // (ZCL_SUPPORT == 1) && (APP_USE_OTAU == 1)

// eof zclOtauClient.c
