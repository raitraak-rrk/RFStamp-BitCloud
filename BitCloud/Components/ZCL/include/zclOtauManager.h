/**************************************************************************//**
  \file zclOtauManager.h

  \brief Declaration of the OTAU manager interface

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    16.05.11 A. Khromykh - Created.
    14.10.14 Karthik.P_u - Modified.
  Last change:
    $Id: zclOtauManager.h 27805 2015-04-29 08:38:33Z karthik.p_u $
*******************************************************************************/
#ifndef _ZCLOTAUMANAGER_H
#define _ZCLOTAUMANAGER_H

/******************************************************************************
                        Includes section
******************************************************************************/
#include <zclOTAUCluster.h>
#include <zclDbg.h>
#include <zdo.h>
/******************************************************************************
                        Defines section
******************************************************************************/
#define KEY_LENGTH            (16U)
#define IV_LENGTH             (16U)

/******************************************************************************
                           Types section
******************************************************************************/
typedef union
{
  ZDO_ZdpReq_t          zdpOtauReq;
#if (defined _LINK_SECURITY_) && (!defined _LIGHT_LINK_PROFILE_)
  APS_RequestKeyReq_t   apsKeyReq;
#endif // (defined _LINK_SECURITY_) && (!defined _LIGHT_LINK_PROFILE_)
  ZCL_Request_t         zclCommandReq;
} OtauReqMemory_t;

typedef union
{
  ZCL_OtauQueryNextImageReq_t   uQueryNextImageReq;
  ZCL_OtauImageBlockReq_t       uImageBlockReq;
  ZCL_OtauImagePageReq_t        uImagePageReq;
  ZCL_OtauUpgradeEndReq_t       uUpgradeEndReq;
} OtauZclReqMemory_t;

/* This structure always should consist actual values to
 * continue broken uploading. */
typedef struct
{
/* Size of internal subimage within main image
 * (for example size of eeprom area) */
  uint32_t  imageInternalLength;
/* Amount bytes were received from internal information structute
 * about the internal subimage.  Should be 8 bytes. 4 byte mcu start
 * address of the subimage. 4 byte size of the subimage. */
  uint8_t   internalAddressStatus;
/* Requested offset from the current file. */
  uint32_t  currentFileOffset;
/* Size of requested data. */
  uint8_t   currentDataSize;
/* Reminder of the not received data. */
  uint32_t  imageRemainder;
/* Address of the page begin. Address form main image. */
  uint32_t  imagePageOffset;
/* Page size of the last Image Page request. */
  uint16_t lastPageSize;
} OtauImageAuxVar_t;

typedef struct
{
  uint32_t        imageSize;
  uint8_t         imageBlockData[OFD_BLOCK_SIZE];
  uint8_t        *imagePageData;
} ZclOtauClientImageBuffer_t;

typedef struct
{ /* memory for storage of server discovery result */
  struct
  {
    void *next;
  } service;

  bool        busy;
  Endpoint_t  serverEndpoint;
  ShortAddr_t serverShortAddress;
  ExtAddr_t   serverExtAddress;
} ZclOtauDiscoveryResult_t;

typedef struct
{
  struct
  {
    void *next;
  } service;

  bool                           busy;
  uint8_t                        id;
  ZCL_Addressing_t               addressing;
  ZCL_Request_t                  zclCommandReq;

  union
  {
    ZCL_OtauQueryNextImageReq_t  queryNextImageReq;
    ZCL_OtauQueryNextImageResp_t queryNextImageResp;
    ZCL_OtauImageBlockReq_t      imageBlockReq;
    ZCL_OtauImageBlockResp_t     imageBlockResp;
    ZCL_OtauUpgradeEndReq_t      upgradeEndReq;
    ZCL_OtauUpgradeEndResp_t     upgradeEndResp;
  };
  ZCL_OtauImagePageReq_t         imagePageReq;
  uint16_t                       pageReminderSize;
} ZclOtauServerTransac_t;

typedef struct
{
  // reqMem contains the memory structure for ZDO, APS and ZCL commands
  OtauReqMemory_t              reqMem;

  // zclReqMem contains the memory structure for request commands supported by the client side OTAU cluster
  OtauZclReqMemory_t           zclReqMem;

  /// pageRequestTimer is used for timing the first block resp for the page request
  // also i plan to use this for timing the image page req completion turnaround
  HAL_AppTimer_t               pageRequestTimer;

  // otauParam holds the image blocks/page and size received over the air
  ZclOtauClientImageBuffer_t   otauParam;

  // imageAuxParam holds the offset tracking, image breakage and current data size
  OtauImageAuxVar_t            imageAuxParam;

  // memParam holds the memory structure to facilitate operation of OTAU flash drive
  OFD_MemoryAccessParam_t      memParam;

  // newFirmwareVersion holds the version information of currently downloading image
  ZCL_OtauFirmwareVersion_t    newFirmwareVersion;

  // numbers of servers that responded during the discovery procedure
  uint8_t                      discoveredServerAmount;

  // OTAU server extended address
  ExtAddr_t                    otauServer;

  // information related to the discovered servers
  ZclOtauDiscoveryResult_t    *discoveredServerMem;

  // bit mask pointer to uint8_t to track the missed bytes in a page download
  uint8_t                     *missedBytesMask;

  // miscellaneous help information
  struct
  {
    uint8_t                    blockRequest : 1;
    uint8_t                    missedBytesGetting : 1;
    uint8_t                    firstStart : 1;
    uint8_t                    blockRequestDelayOn : 1;
  };
#if (USE_IMAGE_SECURITY == 1)
  // imgtype field populated/read in the header/eeprom
  ZCL_OtauImageType_t          eepromImgType;
  uint16_t                     eepromImgTypeVal;
  bool                         isImgTypeValid;
#endif
/* Sequence number used for OTAU request */
  uint8_t otauReqSeqNum;
} ZCL_OtauClientMem_t;

typedef struct
{
  ZCL_Request_t            unsolicitedReq;
  ZCL_OtauUpgradeEndResp_t unsolicitedUpgradeEndResp;
  ZCL_OtauImageNotify_t    imageNotify;
  uint8_t                  transacAmount;
  ZclOtauServerTransac_t  *serverTransac;
} ZCL_OtauServerMem_t;

typedef struct
{
  ZCL_OtauStatInd_t     otauInd;
  ZCL_OtauInitParams_t  initParam;
  HAL_AppTimer_t        genericTimer;
  bool                  isOtauStopTriggered;
  union
  {
    ZCL_OtauClientMem_t clientMem;
    ZCL_OtauServerMem_t serverMem;
  };
} ZclOtauMem_t;

typedef struct
{
  uint8_t keyUpdated;
  uint8_t key[KEY_LENGTH];
  uint8_t iv[IV_LENGTH];
} ZCL_OtauImageKey_t;

/******************************************************************************
                           External variables section
******************************************************************************/
/** zclOtauMem is defined in zclOtauManager.c */
extern ZclOtauMem_t zclOtauMem;
extern bool isOtauBusy;

/******************************************************************************
                           Inline functions section
******************************************************************************/
INLINE ZclOtauMem_t* zclGetOtauMem(void)
{
  return &zclOtauMem;
}

INLINE ZCL_OtauClientMem_t* zclGetOtauClientMem(void)
{
  return &zclOtauMem.clientMem;
}

INLINE ZCL_OtauServerMem_t* zclGetOtauServerMem(void)
{
  return &zclOtauMem.serverMem;
}

INLINE bool isOtauStopped(void)
{
  return zclOtauMem.isOtauStopTriggered;
}
/******************************************************************************
                   Prototypes section
******************************************************************************/
/******`*********************************************************************//**
\brief Lift otau cluster action for customer.

\param[in] action - the OTAU action for a customer.
******************************************************************************/
void zclRaiseCustomMessage(const ZCL_OtauAction_t action);

/***************************************************************************//**
\brief Start otau client services
******************************************************************************/
void zclStartOtauClient(void);

/***************************************************************************//**
\brief Start otau server service
******************************************************************************/
void zclStartOtauServer(void);

/***************************************************************************//**
\brief Stop otau server service
******************************************************************************/
void zclStopOtauServer(void);

/***************************************************************************//**
\brief Stop otau client service
******************************************************************************/
void zclStopOtauClient(void);

/***************************************************************************//**
\brief Clear otau server memory.
******************************************************************************/
void zclClearOtauServerMemory(void);

/***************************************************************************//**
\brief Clear otau client memory.
******************************************************************************/
void zclClearOtauClientMemory(void);

/***************************************************************************//**
\brief Find empty cell in memory pool.

\return pointer to empty cell.
******************************************************************************/
ZclOtauServerTransac_t * zclFindEmptyCell(void);

/***************************************************************************//**
\brief Restart poll timer

\param[in] period - timer fired time
\param[in] cb     - pointer to fired method
******************************************************************************/
void otauStartGenericTimer(uint32_t period, void (* cb)(void));

/***************************************************************************//**
\brief Stop poll timer
******************************************************************************/
void otauStopGenericTimer(void);

#endif /* _ZCLOTAUMANAGER_H */

//eof zclOtauManager.h
