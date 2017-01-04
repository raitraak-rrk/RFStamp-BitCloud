/******************************************************************************
  \file csPersistentMem.c

  \brief
    Configuration Server persistent memory to store in non-volatile memory.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015 , Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    28.10.10 A. Razinkov - Created.
******************************************************************************/
#ifndef ZAPPSI_HOST
/******************************************************************************
                    Includes section
******************************************************************************/
#include <pdsDataServer.h>
#include <macenvMem.h>
#include <csSIB.h>
#include <csBuffers.h>
#ifndef _MAC2_
#include <apsConfigServer.h>
#include <nwkConfigServer.h>
#include <zdoZib.h>
#endif

#ifdef _ENABLE_PERSISTENT_SERVER_
/******************************************************************************
                    External variables section
******************************************************************************/
#if !defined(_USE_KF_MAC_)
extern PIB_t csPIB;
#endif // !defined(_USE_KF_MAC_)
extern SIB_t csSIB;
#ifndef _MAC2_
extern NIB_t csNIB;
extern AIB_t csAIB;
extern ZIB_t csZIB;
#endif

extern CS_StackBuffers_t stackBuffers;
#ifdef _LINK_SECURITY_
extern CS_ReadOnlyItems_t PROGMEM_DECLARE(csReadOnlyItems);
#endif

#if defined(_USE_KF_MAC_)
extern uint64_t tal_pib_IeeeAddress;
#endif // defined(_USE_KF_MAC_)

/******************************************************************************
                    Files definitions
******************************************************************************/
#if PDS_ENABLE_WEAR_LEVELING != 1
/*******************************************
         BitCloud General parameters
********************************************/
/* File descriptors definitions for BitCloud General parameters.
   Shall be placed in the PDS_FF code segment. */
#ifdef _USE_KF_MAC_
  PDS_DECLARE_FILE(CS_UID_MEM_ID,                          sizeof(tal_pib_IeeeAddress),                &tal_pib_IeeeAddress,                FILE_INTEGRITY_CONTROL_MARK);
#else
  PDS_DECLARE_FILE(CS_UID_MEM_ID,                          sizeof(csPIB.macAttr.extAddr),              &csPIB.macAttr.extAddr,              FILE_INTEGRITY_CONTROL_MARK);
#endif /* _USE_KF_MAC_ */
  PDS_DECLARE_FILE(CS_RF_TX_POWER_MEM_ID,                  sizeof(csSIB.csRfTxPower),                  &csSIB.csRfTxPower,                  FILE_INTEGRITY_CONTROL_MARK);
#ifndef _MAC2_
  PDS_DECLARE_FILE(CS_EXT_PANID_MEM_ID,                    sizeof(csSIB.csExtPANID),                   &csSIB.csExtPANID,                   FILE_INTEGRITY_CONTROL_MARK);
  PDS_DECLARE_FILE(CS_CHANNEL_MASK_MEM_ID,                 sizeof(csSIB.csChannelMask),                &csSIB.csChannelMask,                FILE_INTEGRITY_CONTROL_MARK);
  PDS_DECLARE_FILE(CS_CHANNEL_PAGE_MEM_ID,                 sizeof(csNIB.channelPage),                  &csNIB.channelPage,                  FILE_INTEGRITY_CONTROL_MARK);
  PDS_DECLARE_FILE(CS_DEVICE_TYPE_MEM_ID,                  sizeof(csNIB.deviceType),                   &csNIB.deviceType,                   FILE_INTEGRITY_CONTROL_MARK);
  PDS_DECLARE_FILE(CS_RX_ON_WHEN_IDLE_MEM_ID,              sizeof(csSIB.csRxOnWhenIdle),               &csSIB.csRxOnWhenIdle,               FILE_INTEGRITY_CONTROL_MARK);
  PDS_DECLARE_FILE(CS_COMPLEX_DESCRIPTOR_AVAILABLE_MEM_ID, sizeof(csSIB.csComplexDescriptorAvailable), &csSIB.csComplexDescriptorAvailable, FILE_INTEGRITY_CONTROL_MARK);
  PDS_DECLARE_FILE(CS_USER_DESCRIPTOR_AVAILABLE_MEM_ID,    sizeof(csSIB.csUserDescriptorAvailable),    &csSIB.csUserDescriptorAvailable,    FILE_INTEGRITY_CONTROL_MARK);
  PDS_DECLARE_FILE(CS_ZDP_USER_DESCRIPTOR_MEM_ID,          sizeof(csSIB.csUserDescriptor),             &csSIB.csUserDescriptor,             FILE_INTEGRITY_CONTROL_MARK);
  PDS_DECLARE_FILE(CS_NWK_PANID_MEM_ID,                    sizeof(csSIB.csNwkPanid),                   &csSIB.csNwkPanid,                   FILE_INTEGRITY_CONTROL_MARK);
  PDS_DECLARE_FILE(CS_NWK_PREDEFINED_PANID_MEM_ID,         sizeof(csSIB.csNwkPredefinedPanid),         &csSIB.csNwkPredefinedPanid,         FILE_INTEGRITY_CONTROL_MARK);
  PDS_DECLARE_FILE(CS_NWK_ADDR_MEM_ID,                     sizeof(csNIB.networkAddress),               &csNIB.networkAddress,               FILE_INTEGRITY_CONTROL_MARK);
  PDS_DECLARE_FILE(CS_NWK_UNIQUE_ADDR_MEM_ID,              sizeof(csNIB.uniqueAddr),                   &csNIB.uniqueAddr,                   FILE_INTEGRITY_CONTROL_MARK);
  PDS_DECLARE_FILE(CS_NWK_LEAVE_REQ_ALLOWED_MEM_ID,        sizeof(csNIB.leaveReqAllowed),              &csNIB.leaveReqAllowed,              FILE_INTEGRITY_CONTROL_MARK);
  PDS_DECLARE_FILE(CS_DTR_WAKEUP_MEM_ID,                   sizeof(csSIB.csDtrWakeup),                  &csSIB.csDtrWakeup,                  FILE_INTEGRITY_CONTROL_MARK);
  PDS_DECLARE_FILE(NWK_UPDATE_ID_MEM_ID,                   sizeof(csNIB.updateId),                     &csNIB.updateId,                     FILE_INTEGRITY_CONTROL_MARK);
  PDS_DECLARE_FILE(NWK_RREQ_IDENTIFIER_MEM_ID,             sizeof(csNIB.routeRequestIdTop),            &csNIB.routeRequestIdTop,            FILE_INTEGRITY_CONTROL_MARK);
#ifdef _SECURITY_
  PDS_DECLARE_FILE(CS_APS_TRUST_CENTER_ADDRESS_MEM_ID,     sizeof(csAIB.trustCenterAddress),           &csAIB.trustCenterAddress,           FILE_INTEGRITY_CONTROL_MARK);
  PDS_DECLARE_FILE(CS_SHORT_TRUST_CENTER_ADDRESS_MEM_ID,   sizeof(csAIB.tcNwkAddr),                    &csAIB.tcNwkAddr,                    FILE_INTEGRITY_CONTROL_MARK);
  PDS_DECLARE_FILE(CS_ZDO_SECURITY_STATUS_MEM_ID,          sizeof(csSIB.csZdoSecurityStatus),          &csSIB.csZdoSecurityStatus,          FILE_INTEGRITY_CONTROL_MARK);
#endif /* _SECURITY_ */
#endif /* _MAC2_ */

#ifndef _MAC2_
/*******************************************
         BitCloud Extended parameters
********************************************/
/* File descriptors definitions for BitCloud Extneded parameters.
   Shall be placed in the PDS_FF code segment. */
PDS_DECLARE_FILE(CS_NWK_PARENT_ADDR_MEM_ID,     sizeof(csNIB.parentNetworkAddress), &csNIB.parentNetworkAddress, FILE_INTEGRITY_CONTROL_MARK);
PDS_DECLARE_FILE(CS_NWK_DEPTH_MEM_ID,           sizeof(csNIB.depth),                &csNIB.depth,                FILE_INTEGRITY_CONTROL_MARK);
PDS_DECLARE_FILE(CS_NWK_EXT_PANID_MEM_ID,       sizeof(csNIB.extendedPanId),        &csNIB.extendedPanId,        FILE_INTEGRITY_CONTROL_MARK);
PDS_DECLARE_FILE(CS_NWK_LOGICAL_CHANNEL_MEM_ID, sizeof(csSIB.csNwkLogicalChannel),  &csSIB.csNwkLogicalChannel,  FILE_INTEGRITY_CONTROL_MARK);


/*******************************************
         BitCloud Neighbor table
********************************************/
/* File descriptor definition for Neighbor table.
   Shall be placed in the PDS_FF code segment. */
PDS_DECLARE_FILE(CS_NEIB_TABLE_MEM_ID, (CS_NEIB_TABLE_SIZE * sizeof(Neib_t)), &stackBuffers.csNeibTable, NO_FILE_MARKS);

/*******************************************
         BitCloud Group table
********************************************/
#ifdef _GROUP_TABLE_
/* File descriptor definition for Group table.
   Shall be placed in the PDS_FF code segment. */
  PDS_DECLARE_FILE(CS_GROUP_TABLE_MEM_ID, (CS_GROUP_TABLE_SIZE * sizeof(NWK_GroupTableEntry_t)), &stackBuffers.csGroupTable, FILE_INTEGRITY_CONTROL_MARK);
#endif /* _GROUP_TABLE_ */

/*******************************************
         BitCloud Binding table
********************************************/
#if defined(_BINDING_) && (CS_APS_BINDING_TABLE_SIZE > 0)
/* File descriptor definition for Binding table.
   Shall be placed in the PDS_FF code segment. */
  PDS_DECLARE_FILE(CS_APS_BINDING_TABLE_MEM_ID, (CS_APS_BINDING_TABLE_SIZE * sizeof(ApsBindingEntry_t)), &stackBuffers.csApsBindingTable, FILE_INTEGRITY_CONTROL_MARK);
#endif /* _BINDING_ && CS_APS_BINDING_TABLE_SIZE > 0 */

#ifdef _SECURITY_
/*******************************************
         BitCloud NWK security tables
********************************************/
/* File descriptors definitions for BitCloud NWK security parameters.
   Shall be placed in the PDS_FF code segment. */
  PDS_DECLARE_FILE(CS_NWK_SECURITY_IB_MEM_ID,   sizeof(csNIB.securityIB), &csNIB.securityIB, FILE_INTEGRITY_CONTROL_MARK | FILE_SECURITY_MARK);
  PDS_DECLARE_FILE(CS_NWK_SECURITY_KEYS_MEM_ID, CS_NWK_SECURITY_KEYS_AMOUNT * sizeof(NWK_SecurityKey_t), &stackBuffers.csNwkSecKeys, FILE_INTEGRITY_CONTROL_MARK | FILE_SECURITY_MARK);
  PDS_DECLARE_FILE(NWK_SECURITY_COUNTERS_MEM_ID, sizeof(csNIB.securityCounters.outCounterTop), &csNIB.securityCounters.outCounterTop, NO_FILE_MARKS);

/*******************************************
         BitCloud APS key pair descroptors table
********************************************/
#if defined(_LINK_SECURITY_) && CS_APS_KEY_PAIR_DESCRIPTORS_AMOUNT > 0
/* File descriptor definition for APS security tables.
   Shall be placed in the PDS_FF code segment. */
  PDS_DECLARE_FILE(CS_APS_KEY_PAIR_DESCRIPTORS_MEM_ID, (CS_APS_KEY_PAIR_DESCRIPTORS_AMOUNT * sizeof(ApsKeyPairDescriptor_t)), &stackBuffers.csApsKeyPairDescriptors, FILE_INTEGRITY_CONTROL_MARK | FILE_SECURITY_MARK);
#endif /* _LINK_SECURITY_ && CS_APS_KEY_PAIR_DESCRIPTORS_AMOUNT > 0 */
#endif /* _SECURITY_ */

/*******************************************
         BitCloud permission table
********************************************/
#if defined(_PERMISSIONS_) && CS_PERMISSIONS_TABLE_SIZE > 0
/* File descriptor definition for the permissions table.
   Shall be placed in the PDS_FF code segment. */
  PDS_DECLARE_FILE(CS_PERMISSIONS_TABLE_MEM_ID, (CS_PERMISSIONS_TABLE_SIZE * sizeof(ZdoPermissionsRecord_t)), &stackBuffers.csPermissionsTable, FILE_INTEGRITY_CONTROL_MARK);
#endif /* _PERMISSIONS_ && CS_PERMISSIONS_TABLE_SIZE > 0 */
#endif /* _MAC2_ */

/****************************************************************
         Directories definitions
*****************************************************************/
/*! \brief The group of common BitCloud parameters that can be stored
in the non-volatile memory and identified via ::BC_GENERAL_PARAMS_MEM_ID
*/
PROGMEM_DECLARE(PDS_MemId_t CsGeneralMemoryIdsTable[]) =
{
  CS_UID_MEM_ID,
  CS_RF_TX_POWER_MEM_ID,
#ifndef _MAC2_
  CS_EXT_PANID_MEM_ID,
  CS_CHANNEL_MASK_MEM_ID,
  CS_CHANNEL_PAGE_MEM_ID,
  CS_DEVICE_TYPE_MEM_ID,
  CS_RX_ON_WHEN_IDLE_MEM_ID,
  CS_COMPLEX_DESCRIPTOR_AVAILABLE_MEM_ID,
  CS_USER_DESCRIPTOR_AVAILABLE_MEM_ID,
  CS_ZDP_USER_DESCRIPTOR_MEM_ID,
  CS_NWK_PANID_MEM_ID,
  CS_NWK_PREDEFINED_PANID_MEM_ID,
  CS_NWK_ADDR_MEM_ID,
  CS_NWK_UNIQUE_ADDR_MEM_ID,
  CS_NWK_LEAVE_REQ_ALLOWED_MEM_ID,
  CS_DTR_WAKEUP_MEM_ID,
  NWK_UPDATE_ID_MEM_ID,
#ifdef _SECURITY_
  CS_APS_TRUST_CENTER_ADDRESS_MEM_ID,
  CS_SHORT_TRUST_CENTER_ADDRESS_MEM_ID,
  CS_ZDO_SECURITY_STATUS_MEM_ID,
#endif /* _SECURITY_ */
#endif /* _MAC2_ */
};

/* BitCloud General parameters directory descriptor.
   Shall be placed in the PDS_FD code segment. */
PDS_DECLARE_DIR(PDS_DirDescr_t csGeneralParamsDirDescr) =
{
  .list       = CsGeneralMemoryIdsTable,
  .filesCount = ARRAY_SIZE(CsGeneralMemoryIdsTable),
  .memoryId     = BC_GENERAL_PARAMS_MEM_ID
};

//---------------------------------------------------------------
/*! \brief The group of additional network parameters that can be stored
in the non-volatile memory and identified via ::BC_EXTENDED_PARAMS_MEM_ID
*/
PROGMEM_DECLARE(PDS_MemId_t CsExtendedMemoryIdsTable[]) =
{
  CS_NWK_PARENT_ADDR_MEM_ID,
  CS_NWK_DEPTH_MEM_ID,
  CS_NWK_EXT_PANID_MEM_ID,
  CS_NWK_LOGICAL_CHANNEL_MEM_ID,
};

/* BitCloud Extneded parameters directory descriptor.
   Shall be placed in the PDS_FD code segment. */
PDS_DECLARE_DIR(PDS_DirDescr_t csExtendedParamsDirDescr) =
{
  .list       = CsExtendedMemoryIdsTable,
  .filesCount = ARRAY_SIZE(CsExtendedMemoryIdsTable),
  .memoryId     = BC_EXTENDED_PARAMS_MEM_ID
};

#ifdef _SECURITY_
//---------------------------------------------------------------
/*! \bried The group BitCloud NWK security parameters that can be stored
in the non-volatile memory and identified via ::BC_NWK_SECURITY_TABLES_MEM_ID
*/
PROGMEM_DECLARE(PDS_MemId_t CsNwkSecurityMemoryIdsTable[]) =
{
  CS_NWK_SECURITY_IB_MEM_ID,
  CS_NWK_SECURITY_KEYS_MEM_ID,
  NWK_SECURITY_COUNTERS_MEM_ID,
};

/* BitCloud NWK security tables directory descriptor.
   Shall be placed in the PDS_FD code segment. */
PDS_DECLARE_DIR(PDS_DirDescr_t csNwkSecurityTablesDirDescr) =
{
  .list       = CsNwkSecurityMemoryIdsTable,
  .filesCount = ARRAY_SIZE(CsNwkSecurityMemoryIdsTable),
  .memoryId     = BC_NWK_SECURITY_TABLES_MEM_ID
};
#endif /* _SECURITY_ */

//---------------------------------------------------------------
/*! \brief The most extended set of BitCloud parameters that can be stored
in the non-volatile memory and identified via BC_ALL_MEMORY_MEM_ID
*/
PROGMEM_DECLARE(PDS_MemId_t BCSystemMemoryIdsTable[]) =
{
  CS_UID_MEM_ID,
  CS_RF_TX_POWER_MEM_ID,
#ifndef _MAC2_
  CS_EXT_PANID_MEM_ID,
  CS_CHANNEL_MASK_MEM_ID,
  CS_CHANNEL_PAGE_MEM_ID,
  CS_DEVICE_TYPE_MEM_ID,
  CS_RX_ON_WHEN_IDLE_MEM_ID,
  CS_COMPLEX_DESCRIPTOR_AVAILABLE_MEM_ID,
  CS_USER_DESCRIPTOR_AVAILABLE_MEM_ID,
  CS_ZDP_USER_DESCRIPTOR_MEM_ID,
  CS_NWK_PANID_MEM_ID,
  CS_NWK_PREDEFINED_PANID_MEM_ID,
  CS_NWK_ADDR_MEM_ID,
  CS_NWK_UNIQUE_ADDR_MEM_ID,
  CS_NWK_LEAVE_REQ_ALLOWED_MEM_ID,
  CS_DTR_WAKEUP_MEM_ID,
  NWK_UPDATE_ID_MEM_ID,
#if defined(_SECURITY_)
  CS_APS_TRUST_CENTER_ADDRESS_MEM_ID,
  CS_SHORT_TRUST_CENTER_ADDRESS_MEM_ID,
  CS_ZDO_SECURITY_STATUS_MEM_ID,
#endif /* _SECURITY_ */
  CS_NWK_PARENT_ADDR_MEM_ID,
  CS_NWK_DEPTH_MEM_ID,
  CS_NWK_EXT_PANID_MEM_ID,
  CS_NWK_LOGICAL_CHANNEL_MEM_ID,
  CS_NEIB_TABLE_MEM_ID,
  CS_GROUP_TABLE_MEM_ID,
  CS_APS_BINDING_TABLE_MEM_ID,
#ifdef _SECURITY_
  CS_NWK_SECURITY_IB_MEM_ID,
  CS_NWK_SECURITY_KEYS_MEM_ID,
  NWK_SECURITY_COUNTERS_MEM_ID,
#if defined(_LINK_SECURITY_) && CS_APS_KEY_PAIR_DESCRIPTORS_AMOUNT > 0
  CS_APS_KEY_PAIR_DESCRIPTORS_MEM_ID,
#endif /* _LINK_SECURITY_ && CS_APS_KEY_PAIR_DESCRIPTORS_AMOUNT > 0 */
#if defined(_PERMISSIONS_) && CS_PERMISSIONS_TABLE_SIZE > 0
  CS_PERMISSIONS_TABLE_MEM_ID,
#endif /* _PERMISSIONS_ && CS_PERMISSIONS_TABLE_SIZE > 0 */
#endif /* _SECURITY_ */
  NWK_RREQ_IDENTIFIER_MEM_ID
#endif /* _MAC2_ */
};

/* BitCloud system parameters directory descriptor.
   Shall be placed in the PDS_FD code segment. */
PDS_DECLARE_DIR(PDS_DirDescr_t BCSystemDirDescr) =
{
  .list       = BCSystemMemoryIdsTable,
  .filesCount = ARRAY_SIZE(BCSystemMemoryIdsTable),
  .memoryId     = BC_ALL_MEMORY_MEM_ID
};

//---------------------------------------------------------------
/*! \brief The extended set of commonly used BitCloud parameters that can
be stored in the non-volatile memory and identified via BC_EXT_GEN_MEMORY_MEM_ID
*/
PROGMEM_DECLARE(PDS_MemId_t BCExtGenParamsMemoryIdsTable[]) =
{
  CS_UID_MEM_ID,
  CS_RF_TX_POWER_MEM_ID,
#ifndef _MAC2_
  CS_EXT_PANID_MEM_ID,
  CS_CHANNEL_MASK_MEM_ID,
  CS_CHANNEL_PAGE_MEM_ID,
  CS_DEVICE_TYPE_MEM_ID,
  CS_RX_ON_WHEN_IDLE_MEM_ID,
  CS_COMPLEX_DESCRIPTOR_AVAILABLE_MEM_ID,
  CS_USER_DESCRIPTOR_AVAILABLE_MEM_ID,
  CS_ZDP_USER_DESCRIPTOR_MEM_ID,
  CS_NWK_PANID_MEM_ID,
  CS_NWK_PREDEFINED_PANID_MEM_ID,
  CS_NWK_ADDR_MEM_ID,
  CS_NWK_UNIQUE_ADDR_MEM_ID,
  CS_NWK_LEAVE_REQ_ALLOWED_MEM_ID,
  CS_DTR_WAKEUP_MEM_ID,
  NWK_UPDATE_ID_MEM_ID,
#if defined(_SECURITY_)
  CS_APS_TRUST_CENTER_ADDRESS_MEM_ID,
  CS_SHORT_TRUST_CENTER_ADDRESS_MEM_ID,
  CS_ZDO_SECURITY_STATUS_MEM_ID,
#endif /* _SECURITY_ */
  CS_NWK_PARENT_ADDR_MEM_ID,
  CS_NWK_DEPTH_MEM_ID,
  CS_NWK_EXT_PANID_MEM_ID,
  CS_NWK_LOGICAL_CHANNEL_MEM_ID,
#endif /* _MAC2_ */
};

/* BitCloud system parameters directory descriptor.
   Shall be placed in the PDS_FD code segment. */
PDS_DECLARE_DIR(PDS_DirDescr_t BCExtGenParamsDirDescr) =
{
  .list       = BCExtGenParamsMemoryIdsTable,
  .filesCount = ARRAY_SIZE(BCExtGenParamsMemoryIdsTable),
  .memoryId     = BC_EXT_GEN_MEMORY_MEM_ID
};

//---------------------------------------------------------------
/*! \brief A set of parameters to be stored in the non-volatile memory to
provide network rejoin after power failure.The set is identified
via BC_NETWORK_REJOIN_PARAMS_MEM_ID
*/
PROGMEM_DECLARE(PDS_MemId_t BCRejoinParamsMemoryIdsTable[]) =
{
  CS_UID_MEM_ID,
  CS_RF_TX_POWER_MEM_ID,
#ifndef _MAC2_
  CS_EXT_PANID_MEM_ID,
  CS_CHANNEL_MASK_MEM_ID,
  CS_CHANNEL_PAGE_MEM_ID,
  CS_DEVICE_TYPE_MEM_ID,
  CS_RX_ON_WHEN_IDLE_MEM_ID,
  CS_COMPLEX_DESCRIPTOR_AVAILABLE_MEM_ID,
  CS_USER_DESCRIPTOR_AVAILABLE_MEM_ID,
  CS_ZDP_USER_DESCRIPTOR_MEM_ID,
  CS_NWK_PANID_MEM_ID,
  CS_NWK_PREDEFINED_PANID_MEM_ID,
  CS_NWK_ADDR_MEM_ID,
  CS_NWK_UNIQUE_ADDR_MEM_ID,
  CS_NWK_LEAVE_REQ_ALLOWED_MEM_ID,
  CS_DTR_WAKEUP_MEM_ID,
  NWK_UPDATE_ID_MEM_ID,
#if defined(_SECURITY_)
  CS_APS_TRUST_CENTER_ADDRESS_MEM_ID,
  CS_SHORT_TRUST_CENTER_ADDRESS_MEM_ID,
  CS_ZDO_SECURITY_STATUS_MEM_ID,
#endif /* _SECURITY_ */
  CS_GROUP_TABLE_MEM_ID,
#ifdef _SECURITY_
  CS_NWK_SECURITY_IB_MEM_ID,
  CS_NWK_SECURITY_KEYS_MEM_ID,
  NWK_SECURITY_COUNTERS_MEM_ID,
#if defined(_PERMISSIONS_) && CS_PERMISSIONS_TABLE_SIZE > 0
  CS_PERMISSIONS_TABLE_MEM_ID,
#endif /* _PERMISSIONS_ && CS_PERMISSIONS_TABLE_SIZE > 0 */
#endif /* _SECURITY_ */
#endif /* _MAC2_ */
};

/* BitCloud rejoin parameters directory descriptor.
   Shall be placed in the PDS_FD code segment. */
PDS_DECLARE_DIR(PDS_DirDescr_t BCRejoinParamsDirDescr) =
{
  .list       = BCRejoinParamsMemoryIdsTable,
  .filesCount = ARRAY_SIZE(BCRejoinParamsMemoryIdsTable),
  .memoryId     = BC_NETWORK_REJOIN_PARAMS_MEM_ID
};
#else /* PDS_ENABLE_WEAR_LEVELING != 1  */

PDS_DECLARE_ITEM(BC_EXT_GEN_MEMORY_ITEM_ID, EXTENDED_SET_ITEM_SIZE, &extGenMem, fillExtendedBcSet, NO_ITEM_FLAGS);
PDS_DECLARE_ITEM(CS_NEIB_TABLE_ITEM_ID, NEIGHBOR_TABLE_ITEM_SIZE, &stackBuffers.csNeibTable, NULL, SIZE_MODIFICATION_ALLOWED);
#if defined _SECURITY_
#ifdef _LINK_SECURITY_
PDS_DECLARE_ITEM(CS_APS_KEY_PAIR_DESCRIPTORS_ITEM_ID, KEY_PAIR_DESCRIPTOR_ITEM_SIZE, &stackBuffers.csApsKeyPairDescriptors, NULL, ITEM_UNDER_SECURITY_CONTROL);
#endif
PDS_DECLARE_ITEM(CS_NWK_SECURITY_IB_ITEM_ID, SECURITY_IB_ITEM_SIZE, &csNIB.securityIB, NULL, NO_ITEM_FLAGS);
PDS_DECLARE_ITEM(CS_NWK_SECURITY_KEYS_ITEM_ID, SECURITY_KEYS_ITEM_SIZE, &stackBuffers.csNwkSecKeys, NULL, ITEM_UNDER_SECURITY_CONTROL);
PDS_DECLARE_ITEM(NWK_SECURITY_COUNTERS_ITEM_ID, SECURITY_COUNTERS_ITEM_SIZE, &csNIB.securityCounters.outCounterTop, NULL, NO_ITEM_FLAGS);
#endif
#ifdef _BINDING_
PDS_DECLARE_ITEM(CS_APS_BINDING_TABLE_ITEM_ID, BINDING_TABLE_ITEM_SIZE, &stackBuffers.csApsBindingTable, NULL, SIZE_MODIFICATION_ALLOWED);
#endif //_BINDING_
#ifdef _GROUP_TABLE_
PDS_DECLARE_ITEM(CS_GROUP_TABLE_ITEM_ID, GROUP_TABLE_ITEM_SIZE, &stackBuffers.csGroupTable, NULL, SIZE_MODIFICATION_ALLOWED);
#endif //_GROUP_TABLE_
PDS_DECLARE_ITEM(NWK_RREQ_IDENTIFIER_ITEM_ID, RREQ_IDENTIFIER_ITEM_SIZE, &csNIB.routeRequestIdTop, NULL, NO_ITEM_FLAGS);

PROGMEM_DECLARE(S_Nv_ItemId_t BCItemIdsTable[]) =
{
  BC_EXT_GEN_MEMORY_ITEM_ID,
  CS_NEIB_TABLE_ITEM_ID,
#if defined _SECURITY_
#ifdef _LINK_SECURITY_
  CS_APS_KEY_PAIR_DESCRIPTORS_ITEM_ID,
#endif // _LINK_SECURITY_
  CS_NWK_SECURITY_IB_ITEM_ID,
  CS_NWK_SECURITY_KEYS_ITEM_ID,
  NWK_SECURITY_COUNTERS_ITEM_ID,
#endif  //_SECURITY_
#ifdef _BINDING_
  CS_APS_BINDING_TABLE_ITEM_ID,
#endif //_BINDING_
#ifdef _GROUP_TABLE_
  CS_GROUP_TABLE_ITEM_ID,
#endif //_GROUP_TABLE_
  NWK_RREQ_IDENTIFIER_ITEM_ID
};

/* BitCloud rejoin parameters directory descriptor.
   Shall be placed in the PDS_FD code segment. */
PDS_DECLARE_DIR(PDS_DirDescr_t BCRejoinParamsDirDescr) =
{
  .list       = BCItemIdsTable,
  .filesCount = ARRAY_SIZE(BCItemIdsTable),
  .memoryId   = BC_ALL_MEMORY_MEM_ID
};

#endif /* PDS_ENABLE_WEAR_LEVELING != 1  */
#endif /* _ENABLE_PERSISTENT_SERVER_ */
#endif /* !ZAPPSI_HOST */
/* eof csPersistentMem.c */
