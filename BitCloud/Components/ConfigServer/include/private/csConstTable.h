/******************************************************************************
  \file csConstTable.h

  \brief
    Configuration Server constant parameters information table

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015 , Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    22.12.10 A. Razinkov - Created.
******************************************************************************/

/* This table contains information about particular paramter displacement address.
 *
 * This table should be inlined to external module.
 * FLASH_PARAMETER(id, addr) and DUMMY_FLASH_PARAMETER(id) macroses should be defined
 * first to provide specific table morphing.
 *
 * DUMMY_FLASH_PARAMETER(id) macro is used to specify few stack buffers sizes during
 * the application compilation phase.
 */

FLASH_PARAMETER(CS_MAC_FRAME_RX_BUFFER_SIZE_ID, 0x00, csReadOnlyItems.csMacFrameRxBufferSize)
#ifdef _MAC_BAN_NODE_
FLASH_PARAMETER(CS_BAN_TABLE_SIZE_ID, 0x01, csReadOnlyItems.csMacBanTableSize)
#endif
#ifndef _MAC2_
FLASH_PARAMETER(CS_NEIB_TABLE_SIZE_ID, 0x02, csReadOnlyItems.csNeibTableSize)
FLASH_PARAMETER(CS_ROUTE_CACHE_SIZE_ID, 0x03, csReadOnlyItems.csNwkRouteCacheSize)
FLASH_PARAMETER(CS_MAC_PAN_DESCRIPTOR_AMOUNT_ID, 0x04, csReadOnlyItems.csMacPanDescriptorAmount)
FLASH_PARAMETER(CS_MAX_CHILDREN_AMOUNT_ID, 0x05, csReadOnlyItems.csMaxChildrenAmount)
FLASH_PARAMETER(CS_MAX_CHILDREN_ROUTER_AMOUNT_ID, 0x06, csReadOnlyItems.csMaxChildrenRouterAmount)
FLASH_PARAMETER(CS_MAX_NETWORK_DEPTH_ID, 0x07, csReadOnlyItems.csMaxNetworkDepth)
FLASH_PARAMETER(CS_NWK_BUFFERS_AMOUNT_ID, 0x08, csReadOnlyItems.csNwkBuffersAmount)
FLASH_PARAMETER(CS_JOIN_IND_OBJ_AMOUNT_ID, 0x09, csReadOnlyItems.csJoinIndObjAmount)
FLASH_PARAMETER(CS_ROUTE_TABLE_SIZE_ID, 0x0A, csReadOnlyItems.csRouteTableSize)
FLASH_PARAMETER(CS_ADDRESS_MAP_TABLE_SIZE_ID, 0x0B, csReadOnlyItems.csAddressMapTableSize)
FLASH_PARAMETER(CS_ROUTE_DISCOVERY_TABLE_SIZE_ID, 0x0C, csReadOnlyItems.csRouteDiscoveryTableSize)
FLASH_PARAMETER(CS_BTT_SIZE_ID, 0x0D, csReadOnlyItems.csNwkBttSize)
FLASH_PARAMETER(CS_MANUFACTURER_CODE_ID, 0x0E, csReadOnlyItems.csManufacturerCode)
FLASH_PARAMETER(CS_APS_DATA_REQ_BUFFERS_AMOUNT_ID, 0x0F, csReadOnlyItems.csApsDataReqBuffersAmount)
FLASH_PARAMETER(CS_APS_ACK_FRAME_BUFFERS_AMOUNT_ID, 0x10, csReadOnlyItems.csApsAckFrameBuffesAmount)
FLASH_PARAMETER(CS_DUPLICATE_REJECTION_TABLE_SIZE_ID, 0x11, csReadOnlyItems.csDuplicateRejectionTableSize)
FLASH_PARAMETER(CS_STACK_VERSION_ID, 0x12, csReadOnlyItems.csStackVersion)
FLASH_PARAMETER(CS_NWK_LINK_STATUS_COUNT_ID, 0x13, csReadOnlyItems.csNwkLinkStatusCount)
FLASH_PARAMETER(CS_NWK_LINK_STATUS_FRAGMENTATION_PERIOD_ID, 0x14, csReadOnlyItems.csNwkLinkStatusFragmentationPeriod)
#ifdef _GROUP_TABLE_
FLASH_PARAMETER(CS_GROUP_TABLE_SIZE_ID, 0x15, csReadOnlyItems.csGroupTableSize)
#endif
#ifdef _NWK_PASSIVE_ACK_
FLASH_PARAMETER(CS_NWK_PASSIVE_ACK_AMOUNT_ID, 0x16, csReadOnlyItems.csPassiveAckAmount)
#endif
#ifdef _BINDING_
FLASH_PARAMETER(CS_APS_BINDING_TABLE_SIZE_ID, 0x17, csReadOnlyItems.csApsBindingTableSize)
#endif
#ifdef _APS_FRAGMENTATION_
FLASH_PARAMETER(CS_APS_MAX_BLOCKS_AMOUNT_ID, 0x18, csReadOnlyItems.csApsMaxBlocksAmount)
FLASH_PARAMETER(CS_APS_BLOCK_SIZE_ID, 0x19, csReadOnlyItems.csApsBlockSize)
#endif
#if ZCL_SUPPORT == 1
FLASH_PARAMETER(CS_ZCL_MEMORY_BUFFERS_AMOUNT_ID, 0x1A, csReadOnlyItems.csZclMemoryBuffersAmount)
FLASH_PARAMETER(CS_ZCL_BUFFER_SIZE_ID, 0x1B, csReadOnlyItems.csZclMemoryBuffersSize)
  #if APP_USE_OTAU == 1
FLASH_PARAMETER(CS_ZCL_OTAU_DISCOVERED_SERVER_AMOUNT_ID, 0x1C, csReadOnlyItems.csOtauDiscoveredServerAmount)
FLASH_PARAMETER(CS_ZCL_OTAU_CLIENT_SESSION_AMOUNT_ID, 0x1D, csReadOnlyItems.csOtauClientSessionAmount)
    #if APP_SUPPORT_OTAU_PAGE_REQUEST == 1
FLASH_PARAMETER(CS_ZCL_OTAU_IMAGE_PAGE_REQUEST_RESPONSE_SPACING_ID, 0x1E, csReadOnlyItems.csOtauServerPageRequestResponseSpacing)
FLASH_PARAMETER(CS_ZCL_OTAU_IMAGE_PAGE_REQUEST_PAGE_SIZE_ID, 0x1F, csReadOnlyItems.csOtauServerPageSize)
    #endif /* APP_SUPPORT_OTAU_PAGE_REQUEST == 1 */
  #endif /* APP_USE_OTAU == 1 */
#endif /* ZCL_SUPPORT == 1 */

#ifdef _SECURITY_
FLASH_PARAMETER(CS_NWK_SECURITY_KEYS_AMOUNT_ID, 0x20, csReadOnlyItems.csNwkSecurityKeysAmount)
FLASH_PARAMETER(CS_APS_SECURITY_TIMEOUT_PERIOD_ID, 0x21, csReadOnlyItems.csApsSecurityTimeoutPeriod)
  #ifdef _TRUST_CENTRE_
FLASH_PARAMETER(CS_MAX_TC_AUTHENTIC_PROCESS_AMOUNT_ID, 0x22, csReadOnlyItems.csMaxTcAuthenticProcessAmount)
  #endif /* _TRUST_CENTRE_ */
#endif /* _SECURITY_ */
#ifdef _PERMISSIONS_
FLASH_PARAMETER(CS_PERMISSIONS_TABLE_SIZE_ID, 0x23, csReadOnlyItems.csPermissionsTableSize)
#endif /* _PERMISSIONS_ */
#ifdef _LINK_SECURITY_
FLASH_PARAMETER(CS_APS_KEY_PAIR_DESCRIPTORS_AMOUNT_ID, 0x24, csReadOnlyItems.csApsKeyPairDescriptorsAmount)
  #ifdef _TRUST_CENTRE_
FLASH_PARAMETER(CS_MAX_TC_ESTABLISH_KEY_AMOUNT_ID, 0x25, csReadOnlyItems.csMaxTcEstablishKeyAmount)
  #endif /* _TRUST_CENTRE_ */
#endif /* _LINK_SECURITY_ */
FLASH_PARAMETER(CS_ZDO_MAX_SYNC_FAIL_AMOUNT_ID, 0x26, csReadOnlyItems.csZdoMaxSyncFailAmount)
FLASH_PARAMETER(CS_ZDO_PARENT_LINK_RETRY_THRESHOLD_ID, 0x27, csReadOnlyItems.csZdoParentLinkRetryTreshold)
#ifdef _APS_FRAGMENTATION_
FLASH_PARAMETER(CS_APS_TX_BLOCK_SIZE_ID, 0x28, csReadOnlyItems.csApsTxBlockSize)
FLASH_PARAMETER(CS_APS_RX_BLOCK_SIZE_ID, 0x29, csReadOnlyItems.csApsRxBlockSize)
#endif
FLASH_PARAMETER(CS_NWK_MAX_PERMIT_JOIN_PERIOD_LOGIC_ID, 0x2A, csReadOnlyItems.csNwkMaxPermitJoinPeriodLogic)
FLASH_PARAMETER(CS_ZDO_COMMANDS_AMOUNT_ID, 0x2B, csReadOnlyItems.csZdoCommandsAmount)
#ifdef _DUPLICATE_REJECTION_TABLE_BIT_MASK_ENABLE_
FLASH_PARAMETER(CS_APS_DUPLICATE_ENTRY_NO_OF_COUNTER_PER_NODE_ID, 0x2C, csReadOnlyItems.csApsDuplicateEntryCounterSize)
#endif /* _DUPLICATE_REJECTION_TABLE_BIT_MASK_ENABLE_ */
FLASH_PARAMETER(CS_APS_DUPLICATE_ENTRY_LIFE_TIME_ID, 0x2D, csReadOnlyItems.csApsDuplicateEntryLifeTime)
#endif /* _MAC2_ */
#ifdef _FFD_
FLASH_PARAMETER(CS_MAC_FRAME_DATA_REQ_BUFFER_SIZE_ID, 0x2E, csReadOnlyItems.csMacFrameDataReqBufferSize)
#endif
