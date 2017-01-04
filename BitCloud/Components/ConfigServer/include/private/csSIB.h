/******************************************************************************
  \file csSIB.h

  \brief
    BitCloud stack information base

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015 , Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    21.10.10 A. Razinkov - Created.
******************************************************************************/

#ifndef _CSSIB_H_
#define _CSSIB_H_

/******************************************************************************
                    Includes section
******************************************************************************/
#include <sysTypes.h>
#include <csDefaults.h>
#ifdef _MAC_BAN_NODE_
  #include <macBanNode.h>
#endif /* _MAC_BAN_NODE_ */
#ifndef _MAC2_
  #include <nwkConfigServer.h>
  #include <apsConfigServer.h>
  #include <zdoConfigServer.h>
#endif

/******************************************************************************
                    Types section
******************************************************************************/
/**//**
 * \brief Config Server parameter handler
 */
typedef struct _CS_MemoryItem_t
{
  union
  {
    const void FLASH_PTR* flashValue;
    void* ramValue;
  } value;
  uint16_t size;
} CS_MemoryItem_t;

/**//**
 * \brief Read-only parameters, stored in FLASH
 */
typedef struct _CS_ReadOnlyItems_t
{
  /* MAC parameters */
  uint16_t csMacFrameRxBufferSize;
#ifdef _FFD_
  uint16_t csMacFrameDataReqBufferSize;
#endif
#ifdef _MAC_BAN_NODE_
  MAC_BanTableSize_t csMacBanTableSize;
#endif /* _MAC_BAN_NODE_ */

#ifndef _MAC2_
  /* NWK parameters */
  NwkRouteCacheSize_t csNwkRouteCacheSize;
  NwkSizeOfNeighborTable_t csNeibTableSize;
  NwkSizeOfAddressMap_t csAddressMapTableSize;
  uint8_t csMacPanDescriptorAmount;
  uint8_t csMaxChildrenAmount;
  uint8_t csMaxChildrenRouterAmount;
  uint8_t csMaxNetworkDepth;
  NwkPacketsAmount_t csNwkBuffersAmount;
  NwkJoinIndObjAmount_t csJoinIndObjAmount;
  NwkRoutingTableSize_t csRouteTableSize;
  NwkRouteDiscoveryTableSize_t csRouteDiscoveryTableSize;
  NwkBttSize_t csNwkBttSize;
  uint8_t csNwkLinkStatusCount;
  uint32_t csNwkLinkStatusFragmentationPeriod;
#ifdef _GROUP_TABLE_
  NWK_GroupTableSize_t csGroupTableSize;
#endif /* _GROUP_TABLE_ */
#ifdef _NWK_PASSIVE_ACK_
  NwkPassiveAckAmount_t csPassiveAckAmount;
#endif /* _NWK_PASSIVE_ACK_ */

  /* ZDO parameters */
  uint8_t  csZdoMaxSyncFailAmount;
  uint8_t  csZdoParentLinkRetryTreshold;

  /* APS parameters */
  uint8_t  csApsDataReqBuffersAmount;
  uint8_t  csApsAckFrameBuffesAmount;
  uint8_t  csDuplicateRejectionTableSize;
#ifdef _DUPLICATE_REJECTION_TABLE_BIT_MASK_ENABLE_
  uint8_t  csApsDuplicateEntryCounterSize;
#endif // _DUPLICATE_REJECTION_TABLE_BIT_MASK_ENABLE_
  uint32_t csApsDuplicateEntryLifeTime;
#ifdef _BINDING_
  uint8_t  csApsBindingTableSize;
#endif /* _BINDING_ */
#ifdef _APS_FRAGMENTATION_
  uint8_t  csApsMaxBlocksAmount;
  uint16_t csApsBlockSize;
#endif /* _APS_FRAGMENTATION_ */

  /* ZCL parameters */
#if ZCL_SUPPORT == 1
  uint8_t csZclMemoryBuffersAmount;
  uint8_t csZclMemoryBuffersSize;
  #if APP_USE_OTAU == 1
    uint8_t  csOtauDiscoveredServerAmount;
    uint8_t  csOtauClientSessionAmount;
    #if APP_SUPPORT_OTAU_PAGE_REQUEST == 1
      uint16_t csOtauServerPageRequestResponseSpacing;
      uint16_t csOtauServerPageSize;
    #endif /* APP_SUPPORT_OTAU_PAGE_REQUEST == 1 */
  #endif /* APP_USE_OTAU == 1 */
#endif /* ZCL_SUPPORT == 1 */

  /* Other parameters */
  uint32_t csStackVersion;
  uint16_t csManufacturerCode;

  /* Security parameters */
#ifdef _SECURITY_
  NwkKeyAmount_t csNwkSecurityKeysAmount;
  uint32_t csApsSecurityTimeoutPeriod;
#ifdef _TRUST_CENTRE_
    uint8_t csMaxTcAuthenticProcessAmount;
#endif /* _TRUST_CENTRE_ */
#ifdef _LINK_SECURITY_
  APS_KeyPairAmount_t csApsKeyPairDescriptorsAmount;
  #ifdef _TRUST_CENTRE_
    uint8_t csMaxTcEstablishKeyAmount;
  #endif /* _TRUST_CENTRE_ */
#endif /* _LINK_SECURITY_ */
#endif /* _SECURITY_ */
#ifdef _PERMISSIONS_
  ZdoPermissionsTableSize_t csPermissionsTableSize;
#endif /* _PERMISSIONS_ */
#ifdef _APS_FRAGMENTATION_
  uint8_t csApsTxBlockSize;
  uint8_t csApsRxBlockSize;
#endif /* _APS_FRAGMENTATION_ */
  NWK_MaxPermitJoiningPeriod_t csNwkMaxPermitJoinPeriodLogic;
  uint8_t csZdoCommandsAmount;
#endif /* _MAC2_ */
} CS_ReadOnlyItems_t;

/**//**
 * \brief Common stack information base
 */
typedef struct _SIB_t
{
  /* MAC parameters */
  uint32_t    csMacTransactionTime;
  int8_t      csRfTxPower;
  int8_t      csRfch26MaxTxPower;
  uint32_t    csChannelMask;
#ifdef _FFD_
#ifdef _PENDING_EMPTY_DATA_FRAME_
  bool             csMacSendEmptyDataFrame;
#endif /*_PENDING_EMPTY_DATA_FRAME_*/
#endif /*_FFD_*/
#ifdef AT86RF212
  bool        csLbtMode;
#endif /* AT86RF212 */
#ifndef _MAC2_

  /* NWK parameters */
  PanId_t     csNwkPanid;
  bool        csNwkUseMulticast;
  uint8_t     csNwkMaxLinkStatusFailures;
  uint8_t     csNwkEndDeviceMaxFailures;
  uint8_t     csNwkLogicalChannel;
  bool        csNwkPredefinedPanid;
  uint8_t     csAddressAssignmentMethod;
#ifdef _NWK_CONCENTRATOR_
  uint32_t     csNwkConcentratorDiscoveryTime;
#endif
  NWK_JoinControl_t csJoinCtrl;
  NWK_LeaveControl_t csLeaveRejoinCtrl;
  NWK_LeaveControl_t csLeaveNonRejoinCtrl;
  /* ZDO parameters */
  uint32_t         csEndDeviceSleepPeriod;
  uint32_t         csFfdSleepPeriod;
  bool             csRxOnWhenIdle;
  bool             csComplexDescriptorAvailable;
  bool             csUserDescriptorAvailable;
  uint32_t         csIndirectPollRate;
  uint8_t          csZdoJoinAttempts;
  uint32_t         csZdoJoinInterval;
  UserDescriptor_t csUserDescriptor; /* to EEPROM */
  ExtPanId_t       csExtPANID; /* user-defined PAN ID */


  /* PDS parameters*/
  bool    csDtrWakeup;

  /* ZCL parameters */
#if ZCL_SUPPORT == 1
  #if APP_USE_OTAU == 1
    ExtAddr_t csOtauDefaultServerAddress;
    uint32_t  csOtauServerDiscoveryPeriod;
    bool      csOtauServerPageRequestUsage;
    uint32_t  csOtauQueryInterval;
    uint8_t   csOtauMaxRetryCount;
  #endif /* APP_USE_OTAU == 1 */
#endif /* ZCL_SUPPORT == 1*/

  /* Security parameters */
#ifdef _SECURITY_
  bool      csSecurityOn;
  uint8_t   csZdoSecurityStatus;
#endif /* _SECURITY_ */
#endif /* _MAC2_ */
} SIB_t;

#endif /* _CSSIB_H_ */
/* eof csSIB.h */
