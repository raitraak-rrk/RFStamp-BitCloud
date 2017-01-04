/************************************************************************//**
  \file zclOtauClientUpgrade.c

  \brief
    The file implements the OTAU client upgrade end request/response handling

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
                   Global variables section
******************************************************************************/
ZCL_Status_t otauUpgradeEndStatus = ZCL_SUCCESS_STATUS;

/******************************************************************************
                   External variables section
******************************************************************************/
extern ZclOtauClientStateMachine_t stateMachine;
extern ZCL_OtauClusterClientAttributes_t otauClientAttributes;
extern ZclOtauImageNotifyParams_t imgNtfyServer;
extern uint8_t retryCount, otauMaxRetryCount;

/******************************************************************************
                   Implementation section
******************************************************************************/
/***************************************************************************//**
\brief Send indication about finalizing otau process
******************************************************************************/
void otauFinalizeProcess(void)
{
  zclRaiseCustomMessage(OTAU_DEVICE_SHALL_CHANGE_IMAGE);
}

/***************************************************************************//**
\brief Sends upgrade end request to server
******************************************************************************/
void otauUpgradeEndReq(void)
{
  ZCL_OtauClientMem_t *clientMem = zclGetOtauClientMem();
  ZCL_OtauUpgradeEndReq_t *tmpOtauReq = &clientMem->zclReqMem.uUpgradeEndReq;
  ZCL_Request_t *tmpZclReq = &clientMem->reqMem.zclCommandReq;
  ZCL_OtauImageType_t imgType = OTAU_SPECIFIC_IMAGE_TYPE;
  uint16_t csManufacturerId;
  CS_ReadParameter(CS_MANUFACTURER_CODE_ID, &csManufacturerId);

  // If polling is happening already, no need to raise this action
  if (!zclIsOtauBusy())
  {
    zclRaiseCustomMessage(OTAU_UPGRADE_REQUEST_INITIATED);
  }

  if (!isOtauBusy)
  {
    isOtauBusy = true;
  }
 
#if (USE_IMAGE_SECURITY == 1)
  imgType = clientMem->eepromImgType;
#endif

  zclOtauFillOutgoingZclRequest(UPGRADE_END_REQUEST_ID, sizeof(ZCL_OtauUpgradeEndReq_t), (uint8_t *)tmpOtauReq);

  tmpOtauReq->status          = otauUpgradeEndStatus;
  tmpOtauReq->manufacturerId  = csManufacturerId;
  tmpOtauReq->imageType       = imgType;
  tmpOtauReq->firmwareVersion = clientMem->newFirmwareVersion;

  ZCL_CommandReq(tmpZclReq);
}

/***************************************************************************//**
\brief Handles upgrade end response from server

\param[in] addressing - source parameters
\param[in] payloadLength - length of payload
\param[in] payload - upgrade end parameters pointer
******************************************************************************/
ZCL_Status_t upgradeEndRespInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_OtauUpgradeEndResp_t *payload)
{
  ZCL_OtauClientMem_t *clientMem = zclGetOtauClientMem();
  uint16_t csManufacturerId;
  bool validManId;
  bool validImgType;
  bool validFwVer;
  bool validUpgrdTime;
  uint32_t deltaTime = 0;

  CS_ReadParameter(CS_MANUFACTURER_CODE_ID, &csManufacturerId);
  (void)addressing;
  (void)payloadLength;

  if (!OTAU_CHECK_STATE(stateMachine, OTAU_WAIT_TO_UPGRADE_STATE))
  {
    SYS_E_ASSERT_WARN(false, ZCL_OTAU_INVALID_STATE_UPGRADE_END_RSP);
    retryCount = otauMaxRetryCount;
    return ZCL_SUCCESS_STATUS;
  }
  else if(isOtauStopped())
  {
    retryCount = otauMaxRetryCount;
    return ZCL_ABORT_STATUS;
  }

  isOtauBusy = false;

  // process pending img notify(if from another server or for different file)
  if (IS_IMGNTFY_PENDING(imgNtfyServer.addr))
  {
    if (ZCL_SUCCESS_STATUS == otauCheckServerAddrAndTakeAction(false, true))
    {
      return ZCL_SUCCESS_STATUS;
    }
  }

  validManId = (OTAU_UPGRADE_END_RESP_MAN_ID_WILDCARD_VAL == payload->manufacturerId)?true:(csManufacturerId == payload->manufacturerId);
  validImgType = (OTAU_UPGRADE_END_RESP_IMG_TYPE_WILDCARD_VAL == payload->imageType)?true:(OTAU_SPECIFIC_IMAGE_TYPE == payload->imageType);
  validFwVer = (OTAU_UPGRADE_END_RESP_FW_VER_WILDCARD_VAL == payload->firmwareVersion.memAlloc)?true:(clientMem->newFirmwareVersion.memAlloc == payload->firmwareVersion.memAlloc);
  validUpgrdTime = (payload->upgradeTime >= payload->currentTime)?true:false;

  //go for upgrade if server keeps sending wrong parameters for otauMaxRetryCount times
  if (!validManId || !validImgType || !validFwVer || !validUpgrdTime)
  {
    if (retryCount--)
    {
      otauStartGenericTimer(AMOUNT_MSEC_IN_SEC, otauPollServerEndUpgrade);
    }
    else
    {
      retryCount = otauMaxRetryCount;
#if APP_SUPPORT_OTAU_RECOVERY == 1
      otauClearPdsParams();
#endif // APP_SUPPORT_OTAU_RECOVERY == 1
      otauStartGenericTimer(AMOUNT_MSEC_IN_SEC, otauFinalizeProcess);
    }
    return ZCL_MALFORMED_COMMAND_STATUS;
  }

  // keep querying the server either until finite time is sent or until goes unreachable
  if (UNLIMITED_WAITING_TIME == payload->upgradeTime)
  {
    retryCount = otauMaxRetryCount;
    otauClientAttributes.imageUpgradeStatus.value = OTAU_WAITING_TO_UPGRADE;
    otauStartGenericTimer(WAITING_UPGRADE_END_RESPONSE_TIMEOUT, otauPollServerEndUpgrade);
    return ZCL_SUCCESS_STATUS;
  }

  otauClientAttributes.imageUpgradeStatus.value = OTAU_COUNT_DOWN;
#if APP_SUPPORT_OTAU_RECOVERY == 1
  otauClearPdsParams();
#endif // APP_SUPPORT_OTAU_RECOVERY == 1
  deltaTime = AMOUNT_MSEC_IN_SEC * (payload->upgradeTime - payload->currentTime);
  if (AMOUNT_MSEC_IN_SEC > deltaTime)
  {
    deltaTime = AMOUNT_MSEC_IN_SEC;
  }
  
  otauStartGenericTimer(deltaTime, otauFinalizeProcess);
  retryCount = otauMaxRetryCount;
  return ZCL_SUCCESS_STATUS;
}

/***************************************************************************//**
\brief Poll otau server for upgrade end time
******************************************************************************/
void otauPollServerEndUpgrade(void)
{
  otauUpgradeEndReq();
}
#endif // (ZCL_SUPPORT == 1) && (APP_USE_OTAU == 1)

//eof zclOtauClientUpgrade.c
