/**************************************************************************//**
  \file zclOtauClient.h

  \brief Declaration of the OTAU client interface

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    14.10.14 Karthik.P_u - Created.
  Last change:
    $Id: zclOtauClient.h 27258 2014-10-15 05:16:59Z karthik.p_u $
*******************************************************************************/
#ifndef _ZCLOTAUCLIENT_H
#define _ZCLOTAUCLIENT_H

#if (ZCL_SUPPORT == 1) && (APP_USE_OTAU == 1)
/******************************************************************************
                   Includes section
******************************************************************************/
#include <zcl.h>
#include <clusters.h>
#include <zclOTAUCluster.h>
#include <zclDbg.h>
#include <zdo.h>
#include <ofdExtMemory.h>
#include <configServer.h>
#include <sysAssert.h>
#if APP_SUPPORT_OTAU_PAGE_REQUEST == 1
  #include <nwkAttributes.h>
#endif // APP_SUPPORT_OTAU_PAGE_REQUEST == 1
#include <eeprom.h>
#if APP_SUPPORT_OTAU_RECOVERY == 1 && defined(_ENABLE_PERSISTENT_SERVER_)
#include <pdsDataServer.h>
#endif // APP_SUPPORT_OTAU_RECOVERY == 1 && defined(_ENABLE_PERSISTENT_SERVER_)

/******************************************************************************
                   External variables section
******************************************************************************/
#ifdef __GNUC__
extern uint32_t __d_nv_mem_start;
extern uint32_t __d_nv_mem_end;
#endif

/******************************************************************************
                   Define(s) section
******************************************************************************/
#define OTAU_BLOCK_REQUEST_USAGE         1
#define OTAU_PAGE_REQUEST_USAGE          0
#define OTAU_GET_MISSED_BYTES            1
#define OTAU_NOT_GET_MISSED_BYTES        0
#define OTAU_FIRST_CLIENT_START          1
#define OTAU_CONTINUE_CLIENT_WORK        0
#define OTAU_BLOCK_REQUEST_DELAY_OFF     0
#define OTAU_BLOCK_REQUEST_DELAY_ON      1

#ifdef  __IAR_SYSTEMS_ICC__
  #pragma segment="D_NV_MEMORY"
  #define OTAU_WRITESKIP_MEMORY_START  ((uint32_t)__sfb("D_NV_MEMORY"))
  #define OTAU_WRITESKIP_MEMORY_END  ((uint32_t)__sfe("D_NV_MEMORY") - 1U)
#elif __GNUC__
  #define OTAU_WRITESKIP_MEMORY_START (uint32_t)&__d_nv_mem_start
  #define OTAU_WRITESKIP_MEMORY_END (((uint32_t)&__d_nv_mem_end) - 1U)
#else
  #error "Unsupported compiler"
#endif

#define EEPROM_IMGTYPE_OFFSET                (37U)
#define EEPROM_IMGTYPE_LENGTH                (2U)
#define OTAU_READ_EEPROM_OK                  (0)
#define OTAU_READ_EEPROM_FAILED              (-1)
#define OTAU_READ_EEPROM_BUSY                (-2)
#define OTAU_IMAGE_KEY_START_ADDRESS_OFFSET  (4)

#define OTAU_FLASH_WRITE_SKIP_START_EEPROM_OFFSET (EEPROM_IMGTYPE_OFFSET+EEPROM_IMGTYPE_LENGTH)
#define OTAU_FLASH_WRITE_SKIP_START_EEPROM_LENGTH (4U)
#define OTAU_FLASH_WRITE_SKIP_END_EEPROM_OFFSET   (OTAU_FLASH_WRITE_SKIP_START_EEPROM_OFFSET+OTAU_FLASH_WRITE_SKIP_START_EEPROM_LENGTH)
#define OTAU_FLASH_WRITE_SKIP_WORDS_LENGTH        (2U)
#define OTAU_FLASH_WRITE_SKIP_BYTES_LENGTH        (OTAU_FLASH_WRITE_SKIP_WORDS_LENGTH*OTAU_FLASH_WRITE_SKIP_START_EEPROM_LENGTH)
#define OTAU_EEPROM_ACTION_FLAG_OFFSET            (OTAU_FLASH_WRITE_SKIP_END_EEPROM_OFFSET + OTAU_FLASH_WRITE_SKIP_START_EEPROM_LENGTH)
#define OTAU_EEEPROM_ACTION_FLAG_LENGTH           (1U)
#define OTAU_WRITE_EEPROM_BUSY                    (-2)

#define OTAU_UPGRADE_END_RESP_MAN_ID_WILDCARD_VAL 0xFFFF
#define OTAU_UPGRADE_END_RESP_IMG_TYPE_WILDCARD_VAL 0xFFFF
#define OTAU_UPGRADE_END_RESP_FW_VER_WILDCARD_VAL 0xFFFFFFFF

#define AUXILIARY_STRUCTURE_IS_FULL          (sizeof(uint32_t) + sizeof(uint32_t)) //  offset + internalLength
#define IMAGE_CRC_SIZE                       1

/* 
   Account for delay at server for ongoing transaction. Consider page processing
   at server as the longest transaction even if this client is in block mode.
   timeout ~= 3000 ms ~= (((pageSize/serverBlockRspPayloadSize)+1) * 
                           (respSpacing + ISDDelay))
   Considering, pageSize = 256, respSpacing = 200, serverBlockRspPayloadSize = 38,
   200 for server delay for read.
   Note : (1) This is just a guess at client side. If server is transacting page from 
          a client with much higher page size, this client's requests will have 
          frequent retry. (2) Assuming page parameters of server's ongoing transaction
          to be same as this client's page parameters is also not correct, so go for
          default value
*/
#define OTAU_DEFAULT_RESPONSE_WAIT_TIMEOUT   3000ul

#define OTAU_UPGRADE_END_RESP_MAN_ID_WILDCARD_VAL 0xFFFF
#define OTAU_UPGRADE_END_RESP_IMG_TYPE_WILDCARD_VAL 0xFFFF
#define OTAU_UPGRADE_END_RESP_FW_VER_WILDCARD_VAL 0xFFFFFFFF

/* frequency of write(in bytes) = total-file-size/OTAU_NUM_OF_PDS_WRITES */
#define OTAU_NUM_OF_PDS_WRITES               20

#define REPOST_OFD_ACTION                    10ul
#define AMOUNT_MSEC_IN_SEC                   1000ul
#define WAIT_FOR_RUN_UPGRADE_REQ             0xFF
#define WAITING_UPGRADE_END_RESPONSE_TIMEOUT 3600000ul  // 1 hour (in msec)
#define UNLIMITED_WAITING_TIME               0xFFFFFFFFul
#define UPGRADE_END_REQ_QUICK_TIMEOUT        100
#define CONTEXT_BREAK_QUICK_TIMEOUT          10

#define ZERO_SERVER_EXT_ADDRESS              0x0000000000000000ull
#define COMMON_SERVER_EXT_ADDRESS            0xFFFFFFFFFFFFFFFFull
#define OTAU_MAX_UNAUTHORIZED_SERVERS        3

#define OTAU_SET_STATE(machine, state)       ((machine) = (state))
#define OTAU_CHECK_STATE(machine, state)     ((machine) == (state))

#define IS_IMGNTFY_PENDING(server)           (server.busy)

#define OTAU_IMGFILES_MEMORY_MEM_ID          OTAU_DIR1_MEM_ID
#define OTAU_AUXFILES_MEMORY_MEM_ID          OTAU_DIR2_MEM_ID
#define OTAU_OFFSETFILES_MEMORY_MEM_ID       OTAU_DIR3_MEM_ID
#define OTAU_SERVER_EXT_ADDR_MEM_ID          OTAU_PARAM1_MEM_ID
#define OTAU_IMAGE_VERSION_MEM_ID            OTAU_PARAM2_MEM_ID
#define OTAU_IMAGE_SIZE_MEM_ID               OTAU_PARAM3_MEM_ID
#define OTAU_NEXT_OFFSET_MEM_ID              OTAU_PARAM4_MEM_ID
#define OTAU_RUNNING_CHECKSUM_MEM_ID         OTAU_PARAM5_MEM_ID
#define OTAU_INTERNAL_LENGTH_MEM_ID          OTAU_PARAM6_MEM_ID
#define OTAU_IMAGE_REMAINDER_MEM_ID          OTAU_PARAM7_MEM_ID
#define OTAU_ADDRESS_STATUS_MEM_ID           OTAU_PARAM8_MEM_ID
#define OTAU_IMAGE_WRITE_OFFSET_ID           OTAU_PARAM9_MEM_ID

/******************************************************************************
                   Types section
******************************************************************************/

/******************************************************************************
                   Constants section
******************************************************************************/
typedef enum
{
  OTAU_STOPPED_STATE,
  OTAU_INIT_OFD_STATE,
  OTAU_WAIT_TO_DISCOVER_STATE,
  OTAU_GET_MATCH_DESC_BROADCAST_STATE,
  OTAU_GET_MATCH_DESC_UNICAST_STATE,
  OTAU_GET_IEEE_ADDR_STATE,
  OTAU_GET_NWK_ADDR_STATE,
  OTAU_GET_LINK_KEY_STATE,
  OTAU_WAIT_TO_QUERY_STATE,
  OTAU_QUERY_FOR_IMAGE_STATE,
  OTAU_GET_IMAGE_BLOCKS_STATE,
  OTAU_GET_IMAGE_PAGES_STATE,
  OTAU_GET_MISSED_BLOCKS_STATE,
  OTAU_WAIT_TO_UPGRADE_STATE
} ZclOtauClientStateMachine_t;

typedef struct {
  ZclOtauDiscoveryResult_t addr;
  ZCL_OtauFirmwareVersion_t ver;
  uint8_t queryJitter;
} ZclOtauImageNotifyParams_t;

/******************************************************************************
                          Prototypes section
******************************************************************************/
bool isExtAddrValid(ExtAddr_t addr);

// initialization related functions
void otauOpenOfdCallback(OFD_Status_t status);

// discovery related functions
void otauEnqueueDiscoveryQueue(ShortAddr_t shortAddr, uint8_t endpoint);

void otauStartDiscovery(void);
void otauStartDiscoveryTimer(void);

void otauIeeeAddrReq(ZclOtauDiscoveryResult_t *srv);
void otauIeeeAddrResp(ZDO_ZdpResp_t *ieeeAddrResp);

void otauNwkAddrReq(void);
void otauNwkAddrResp(ZDO_ZdpResp_t *nwkAddrResp);

void otauMatchDescReq(void (* zdoZdpResp)(ZDO_ZdpResp_t *zdpResp));
void otauBroadcastMatchDescResp(ZDO_ZdpResp_t *zdpResp);
void otauUnicastMatchDescResp(ZDO_ZdpResp_t *zdpResp);

#if (defined _LINK_SECURITY_) && (!defined _LIGHT_LINK_PROFILE_)
void otauRequestKeyReq(ExtAddr_t *tcExtAddr);
void otauRequestKeyResp(APS_RequestKeyConf_t *requestKeyConf);
#endif // (defined _LINK_SECURITY_) && (!defined _LIGHT_LINK_PROFILE_)

// query related functions
void otauStartQuery(void);
void otauStartQueryTimer(void);

void otauQueryNextImageReq(void);
void otauQueryAfterJitter(uint8_t queryJitter);
void otauStartImageLoading(ZCL_OtauQueryNextImageResp_t *payload);

ZCL_Status_t queryNextImageRespInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_OtauQueryNextImageResp_t *payload);
ZCL_Status_t imageNotifyInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_OtauImageNotify_t *payload);
ZCL_Status_t otauCheckServerAddrAndTakeAction(bool saveImgNtfyParams, bool checkImgVer);
void otauWriteImgNtfyBusyStatus(bool busyness);
void otauTransitionToQuery(void);

// downloading related functions
void otauStartDownload(void);
void otauGetMissedBlocks(void);
void otauImageBlockReq(void);
void otauImagePageReq(void);
ZCL_Status_t imageBlockRespInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_OtauImageBlockResp_t *payload);
ZCL_Status_t otauProcessSuccessfullImageBlockResponse(ZCL_OtauImageBlockResp_t *payload);
void otauImagePageReqIntervalElapsed(void);
bool otauCheckBlockRespFields(ZCL_OtauImageBlockResp_t *payload);
ZCL_Status_t otauFillAuxiliaryStructure(ZCL_OtauImageBlockResp_t *payload);
void otauBlockResponseImageDataStoring(ZCL_OtauImageBlockResp_t *payload);
void otauCountActuallyDataSize(void);
bool otauCheckPageIntegrity(void);
void otauScheduleImageBlockReq(void);
void otauFinalizeProcess(void);
void otauPollServerEndUpgrade(void);

// upgrading related functions
void otauUpgradeEndReq(void);
ZCL_Status_t upgradeEndRespInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_OtauUpgradeEndResp_t *payload);

// helper functions
void otauClearPdsParams(void);
void otauConfigureNextPdsWriteOffset(void);
uint8_t otauCalcCrc(uint8_t crc, uint8_t *pcBlock, uint8_t length);
uint8_t otauCalculateRunningChecksum(uint8_t *data);
void otauContinueWritingImageToFlash(void);
void otauStartSwitch(void);
void otauStartFlush(void);
void otauStartWrite(void);
void otauStartErase(void);
void otauGetCrc(void);

void otauSwitchCallback(OFD_Status_t status);
void otauFlushCallback(OFD_Status_t status, OFD_ImageInfo_t *imageInfo);
void otauWriteCallback(OFD_Status_t status);
void otauEraseCallback(OFD_Status_t status);
void otauGetCrcCallback(OFD_Status_t status, OFD_ImageInfo_t *imageInfo);

void zclOtauFillOutgoingZclRequest(uint8_t id, uint8_t length, uint8_t *payload);
void otauSomeRequestConfirm(ZCL_Notify_t *resp);
void otauSomeDefaultResponse(ZCL_Addressing_t *addressing, uint8_t payloadLength, uint8_t *payload);

// security related functions
#if (USE_IMAGE_SECURITY == 1)
void otauStartEepromDelayedTransaction(void);
void otauSendReqWithNewImgType(void);
void otauReadImgTypeFromEeprom(void);
void readEepromCallback(void);
void writeEepromCallback(void);
void writeKeyToEeprom(void);
#endif /* #if (USE_IMAGE_SECURITY == 1) */

void writeSkipSectionToEepromCallback(void);
void writeEepromActionFlagCallback(void);
void otauWriteEepromAction(void);
void otauStartEepromPollTimer(void (*delayedTransaction)());
void otauWriteSkipSectionToEeprom(void);
#endif // (ZCL_SUPPORT == 1) && (APP_USE_OTAU == 1)

#endif // _ZCLOTAUCLIENT_H
//eof zclOtauClient.h
