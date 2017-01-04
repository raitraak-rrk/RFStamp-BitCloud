/**************************************************************************//**
  \file wlPdsTypesConverter.c

  \brief PDS types converter implementation.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    22.05.13 A. Fomin - Created.
******************************************************************************/

#if PDS_ENABLE_WEAR_LEVELING == 1
#ifdef _ENABLE_PERSISTENT_SERVER_
/******************************************************************************
                    Includes section
******************************************************************************/
#include <wlPdsBindings.h>
#include <wlPdsMemIds.h>
#include <wlPdsTypesConverter.h>
#include <csBuffers.h>
#include <csSIB.h>
#include <zdoConfigServer.h>
#include <zdoZib.h>
#include <N_ErrH.h>

/******************************************************************************
                    Definitions section
******************************************************************************/
#define COMPID "wlPdsTypesConverter"

/******************************************************************************
                    Types section
******************************************************************************/


/******************************************************************************
                    Externals section
******************************************************************************/
extern CS_StackBuffers_t stackBuffers;

extern NIB_t csNIB;
extern SIB_t csSIB;
extern ZIB_t csZIB;
extern AIB_t csAIB;

/******************************************************************************
                    Prototypes section
******************************************************************************/
static void verifyItemSizes(S_Nv_ItemId_t id, uint16_t size, uint16_t oldSize);

static bool updateExtendedBcSet(void *data);
#ifdef _SECURITY_
static void updateSecurityCounters(void *data);
#ifdef _NWK_IN_FRAME_COUNTERS_
static void updateSecurityKeys(void *data);
#endif  // _NWK_IN_FRAME_COUNTERS_
#endif  // _SECURITY_
static void updateTable(void *data, uint16_t size, uint16_t oldSize);
#ifdef _BINDING_
static void updateBindTable(void *data, uint16_t size, uint16_t oldSize);
#endif //_BINDING_
#ifdef _GROUP_TABLE_
static void updateGroupTable(void *data, uint16_t size, uint16_t oldSize);
#endif //_GROUP_TABLE_
static void updateNeighborTable(void *data, uint16_t size, uint16_t oldSize);

/******************************************************************************
                    Static variables section
******************************************************************************/
ExtGetMem_t extGenMem;

/******************************************************************************
                    Implementation section
******************************************************************************/
/******************************************************************************
\brief Checks whether item is under security control

\param[in] id - item id

\returns true if item is under security control, false - otherwise
******************************************************************************/
bool pdsIsItemUnderSecurityControl(S_Nv_ItemId_t id)
{
#ifdef PDS_SECURITY_CONTROL_ENABLE
  ItemIdToMemoryMapping_t mapItem;

  if (pdsGetItemDescr(id, &mapItem))
  {
    if (mapItem.flags & ITEM_UNDER_SECURITY_CONTROL)
      return true;
  }
#else
  // To avoid compiler warning
  (void)id;
#endif
  return false;
}

/******************************************************************************
\brief Updates BC parameters after restoring taking into account possible size
  changes

\param[in] id      - item id;
\param[in] data    - pointer to data;
\param[in] size    - current item size;
\param[in] oldSize - last size of item

returns true if memory was updated successfully, false - otherwise
******************************************************************************/
bool pdsUpdateMemory(S_Nv_ItemId_t id, void *data, uint16_t size, uint16_t oldSize)
{
  verifyItemSizes(id, size, oldSize);

  switch (id)
  {
    case BC_EXT_GEN_MEMORY_ITEM_ID:
      return updateExtendedBcSet(data);
#ifdef _SECURITY_
    case NWK_SECURITY_COUNTERS_ITEM_ID:
      updateSecurityCounters(data);
      break;
#endif
    case CS_NEIB_TABLE_ITEM_ID:
      updateNeighborTable(data, size, oldSize);
      break;
#ifdef _BINDING_
    case CS_APS_BINDING_TABLE_ITEM_ID:
      updateBindTable(data, size, oldSize);
      break;
#endif
#ifdef _GROUP_TABLE_
    case CS_GROUP_TABLE_ITEM_ID:
      updateGroupTable(data, size, oldSize);
      break;
#endif //_GROUP_TABLE_
#ifdef _SECURITY_
    case CS_NWK_SECURITY_KEYS_ITEM_ID:
#ifdef _NWK_IN_FRAME_COUNTERS_
      updateSecurityKeys(data);
#endif
      break;
    case CS_APS_KEY_PAIR_DESCRIPTORS_ITEM_ID:
    case CS_NWK_SECURITY_IB_ITEM_ID:
#endif
    case NWK_RREQ_IDENTIFIER_ITEM_ID:
      break;
    default:
#ifndef ATMEL_APPLICATION_SUPPORT
      N_ERRH_FATAL();
#endif
      break;
  }

  return true;
}
/******************************************************************************
\brief Fills extended BC set
******************************************************************************/
void fillExtendedBcSet(void)
{
  extGenMem.csUid = csPIB.macAttr.extAddr;
  extGenMem.txPower = csSIB.csRfTxPower;
  extGenMem.extPanId = csSIB.csExtPANID;
  extGenMem.channelMask = csSIB.csChannelMask;
  extGenMem.channelPage = csNIB.channelPage;
  extGenMem.deviceType = csNIB.deviceType;
  extGenMem.rxOnWhenIdle = csSIB.csRxOnWhenIdle;
  extGenMem.complexDescrAvailable = csSIB.csComplexDescriptorAvailable;
  extGenMem.userDescrAvailable = csSIB.csUserDescriptorAvailable;
  extGenMem.userDescr = csSIB.csUserDescriptor;
  extGenMem.panId = csSIB.csNwkPanid;
  extGenMem.predefinedPanId = csSIB.csNwkPredefinedPanid;
  extGenMem.shortAddress = csNIB.networkAddress;
  extGenMem.uniqueNwkAddress = csNIB.uniqueAddr;
  extGenMem.leaveReqAllowed = csNIB.leaveReqAllowed;
  extGenMem.dtrWakeUp = csSIB.csDtrWakeup;
  extGenMem.updateId = csNIB.updateId;
#if defined _SECURITY_
  extGenMem.extTcAddress = csAIB.trustCenterAddress;
  extGenMem.nwkTcAddress = csAIB.tcNwkAddr;
  extGenMem.securityStatus = csSIB.csZdoSecurityStatus;
#endif 
  extGenMem.parentNwkAddress = csNIB.parentNetworkAddress;
  extGenMem.nwkDepth = csNIB.depth;
  extGenMem.nwkExtPanId = csNIB.extendedPanId;
  extGenMem.logicalChannel = csSIB.csNwkLogicalChannel;
}

/******************************************************************************
\brief Verifies whether size change is allowed for given item

\param[in] id      - item id;
\param[in] size    - current item size;
\param[in] oldSize - last size of item
******************************************************************************/
static void verifyItemSizes(S_Nv_ItemId_t id, uint16_t size, uint16_t oldSize)
{
  ItemIdToMemoryMapping_t mapItem;

  if (size == oldSize)
    return;

  if (pdsGetItemDescr(id, &mapItem))
  {
    N_ERRH_ASSERT_FATAL(mapItem.flags & SIZE_MODIFICATION_ALLOWED);
  }
}

/******************************************************************************
\brief Updates extneded BC set of stack parameters

\param[in] data - pointer to data with BC set

\returns returns true extended BC set was updated successfully,
  false - otherwise
******************************************************************************/
static bool updateExtendedBcSet(void *data)
{
  ExtGetMem_t *extGenMem = (ExtGetMem_t *)data;
  uint8_t *ptr = (uint8_t *)data;
  bool inconsistentFlag = true;

  // check network parameters for inconsistence
  for (uint8_t i = 0U; i < sizeof(ExtGetMem_t); i++)
    if (0xFFU != ptr[i])
    {
      inconsistentFlag = false;
      break;
    }

  if (inconsistentFlag)
    return false;

  // update appropriate BC structures
  csPIB.macAttr.extAddr = extGenMem->csUid;
  csSIB.csRfTxPower = extGenMem->txPower;
  csSIB.csExtPANID = extGenMem->extPanId;
  csSIB.csChannelMask = extGenMem->channelMask;
  csNIB.channelPage = extGenMem->channelPage;
  csNIB.deviceType = extGenMem->deviceType;
  csSIB.csRxOnWhenIdle = extGenMem->rxOnWhenIdle;
  csSIB.csComplexDescriptorAvailable = extGenMem->complexDescrAvailable;
  csSIB.csUserDescriptorAvailable = extGenMem->userDescrAvailable;
  csSIB.csUserDescriptor = extGenMem->userDescr;
  csSIB.csNwkPanid = extGenMem->panId;
  csSIB.csNwkPredefinedPanid = extGenMem->predefinedPanId;
  csNIB.networkAddress = extGenMem->shortAddress;
  csNIB.uniqueAddr = extGenMem->uniqueNwkAddress;
  csNIB.leaveReqAllowed = extGenMem->leaveReqAllowed;
  csSIB.csDtrWakeup = extGenMem->dtrWakeUp;
  csNIB.updateId = extGenMem->updateId;
#ifdef _SECURITY_  
  csAIB.trustCenterAddress = extGenMem->extTcAddress;
  csAIB.tcNwkAddr = extGenMem->nwkTcAddress;
  csSIB.csZdoSecurityStatus = extGenMem->securityStatus;
#endif  
  csNIB.parentNetworkAddress = extGenMem->parentNwkAddress;
  csNIB.depth = extGenMem->nwkDepth;
  csNIB.extendedPanId = extGenMem->nwkExtPanId;
  csSIB.csNwkLogicalChannel = extGenMem->logicalChannel;

  return true;
}
#ifdef _SECURITY_ 
/******************************************************************************
\brief Updates outgoing security counters

\param[in] data - pointer to data top part of security counter
******************************************************************************/
static void updateSecurityCounters(void *data)
{
  NwkOutFrameCounterTop_t *counterTop = (NwkOutFrameCounterTop_t *)data;

  // we restored default after-init value
  if (0xFFFF == *counterTop)
    *counterTop = 0;
}

#ifdef _NWK_IN_FRAME_COUNTERS_
/******************************************************************************
\brief Updates security keys

\param[in] data - pointer to key
******************************************************************************/
static void updateSecurityKeys(void *data)
{
  NWK_SecurityKey_t *keys = (NWK_SecurityKey_t *)data;
  NWK_InFrameCounterEntry_t *inFrameCounterSet;
  NwkKeyAmount_t keyAmount;
  uint8_t keyIndex;
  uint8_t neighborsAmount;

  CS_ReadParameter(CS_NWK_SECURITY_KEYS_AMOUNT_ID, &keyAmount);
  CS_ReadParameter(CS_NEIB_TABLE_SIZE_ID, &neighborsAmount);
  CS_GetMemory(CS_NWK_INCOMING_FRAME_COUNTER_SET_ID, NWK_ADDR_OF_TABLE(inFrameCounterSet));

  for (keyIndex = 0U; keyIndex < keyAmount; ++keyIndex)
  {
    if (keys[keyIndex].isSet)
    {
      keys[keyIndex].inFrameCounterSet.table = inFrameCounterSet + neighborsAmount * keys[keyIndex].seqNum;
      keys[keyIndex].inFrameCounterSet.end   = inFrameCounterSet + neighborsAmount * (keys[keyIndex].seqNum + 1);
    }
  }
}
#endif
#endif
/******************************************************************************
\brief Updates table

\param[in] data    - pointer to data with table;
\param[in] size    - current item size;
\param[in] oldSize - last size of item
******************************************************************************/
static void updateTable(void *data, uint16_t size, uint16_t oldSize)
{
  uint8_t *ptr = (uint8_t *)data;

  if (size > oldSize)
    memset(ptr + oldSize, 0U, size - oldSize);
}
#ifdef _BINDING_
/******************************************************************************
\brief Updates binding table

\param[in] data    - pointer to data with binding table;
\param[in] size    - current item size;
\param[in] oldSize - last size of item
******************************************************************************/
static void updateBindTable(void *data, uint16_t size, uint16_t oldSize)
{
  if (size == oldSize)
    return;

  updateTable(data, size, oldSize);

  if (size > oldSize)
  {
    uint8_t *ptr = (uint8_t *)data + oldSize; // points to added records;
    ApsBindingEntry_t *entry = (ApsBindingEntry_t *)ptr;
    S_Nv_ReturnValue_t ret;

    // go through all new records and mark them as free
    while (oldSize < size)
    {
      entry->confirm.status = APS_NO_BOUND_DEVICE_STATUS;
      entry++;
      oldSize += sizeof(ApsBindingEntry_t);
    }
    ret = S_Nv_Write(CS_APS_BINDING_TABLE_ITEM_ID, 0U, size, data);
    N_ERRH_ASSERT_FATAL(ret == S_Nv_ReturnValue_Ok);
  }
}
#endif // _BINDING_
#ifdef _GROUP_TABLE_
/******************************************************************************
\brief Updates group table

\param[in] data    - pointer to data with group table;
\param[in] size    - current item size;
\param[in] oldSize - last size of item
******************************************************************************/
static void updateGroupTable(void *data, uint16_t size, uint16_t oldSize)
{
  S_Nv_ReturnValue_t ret;

  if (size == oldSize)
    return;

  updateTable(data, size, oldSize);

  ret = S_Nv_Write(CS_GROUP_TABLE_ITEM_ID, 0U, size, data);
  N_ERRH_ASSERT_FATAL(ret == S_Nv_ReturnValue_Ok);
}
#endif //_GROUP_TABLE_
/******************************************************************************
\brief Updates neighbor table

\param[in] data    - pointer to data with neighbor table;
\param[in] size    - current item size;
\param[in] oldSize - last size of item
******************************************************************************/
static void updateNeighborTable(void *data, uint16_t size, uint16_t oldSize)
{
  if (size == oldSize)
    return;

  updateTable(data, size, oldSize);

  if (size > oldSize)
  {
    uint8_t *ptr = (uint8_t *)data + oldSize; // points to added records;
    Neib_t *entry = (Neib_t *)ptr;
    S_Nv_ReturnValue_t ret;

    // go through all new records and mark them as free
    while (oldSize < size)
    {
      entry->relationship = RELATIONSHIP_EMPTY;
      entry++;
      oldSize += sizeof(Neib_t);
    }
    ret = S_Nv_Write(CS_NEIB_TABLE_ITEM_ID, 0U, size, data);
    N_ERRH_ASSERT_FATAL(ret == S_Nv_ReturnValue_Ok);
  }
}
/******************************************************************************
\brief To get the item descriptor for the given item ID

\param[in] itemId            - item id;
\param[in] itemDescrToGet    - pointer to item descriptor to be loaded;

returns true if descriptor is found out for the given item ID, false - otherwise
******************************************************************************/
bool pdsGetItemDescr(S_Nv_ItemId_t itemId, ItemIdToMemoryMapping_t *itemDescrToGet )
{
  ItemIdToMemoryMapping_t *itemDescrPtr = (ItemIdToMemoryMapping_t *)PDS_FF_START;

  /* Calculate raw data size */
  for (uint8_t fileIdx = 0; fileIdx < PDS_ITEM_AMOUNT; fileIdx++)
  {
    ItemIdToMemoryMapping_t itemDescr;

    memcpy_P(&itemDescr, (void const FLASH_PTR *)itemDescrPtr, sizeof(ItemIdToMemoryMapping_t));
    if(itemDescr.itemId == itemId)
    {
      memcpy(itemDescrToGet, &itemDescr, sizeof(ItemIdToMemoryMapping_t));
      return true;
    }
    itemDescrPtr++;
  }
  return false;
}
/******************************************************************************
\brief To get the directory descriptor for the given dir ID

\param[in] itemId           - item id;
\param[in] dirDescrToGet    - pointer to dir descriptor to be loaded;

returns true if descriptor is found out for the given dir ID, false - otherwise
******************************************************************************/
bool pdsGetDirDescr(S_Nv_ItemId_t itemId, PDS_DirDescr_t *dirDescrToGet )
{
  PDS_DirDescr_t *dirDescrPtr = (PDS_DirDescr_t *)PDS_FD_START;

  /* Calculate raw data size */
  for (uint8_t fileIdx = 0; fileIdx < PDS_DIRECTORIES_AMOUNT; fileIdx++)
  {
    PDS_DirDescr_t dirDescr;

    memcpy_P(&dirDescr, (void const FLASH_PTR *)dirDescrPtr, sizeof(PDS_DirDescr_t));
    if(dirDescr.memoryId == itemId)
    {
      memcpy(dirDescrToGet, &dirDescr, sizeof(PDS_DirDescr_t));
      return true;
    }
    dirDescrPtr++;
  }
  return false;
}
#endif // _ENABLE_PERSISTENT_SERVER_
#endif // PDS_ENABLE_WEAR_LEVELING == 1
// eof wlPdsTypesConverter.c