/**************************************************************************//**
  \file wlPdsDataServer.c

  \brief PDS data server implementation.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    21.05.13 A. Fomin - Created.
******************************************************************************/

#if PDS_ENABLE_WEAR_LEVELING == 1
#ifdef _ENABLE_PERSISTENT_SERVER_
/******************************************************************************
                               Includes section
******************************************************************************/
#include <wlPdsMemIds.h>
#include <wlPdsSecurity.h>
#include <wlPdsBindings.h>
#include <S_Nv_Init.h>
#include <S_Nv.h>
#include <wlPdsTaskManager.h>
#include <wlPdsTypesConverter.h>
#include <N_ErrH.h>
#include <D_Nv_Init.h>
#include <sysEvents.h>
#include <wlPdsTypes.h>

/******************************************************************************
                              Defines section
******************************************************************************/
#define EVENT_TO_MEM_ID_MAPPING(event, id)  {.eventId = event, .itemId = id}
#define COMPID "wlPdsDataServer"

/******************************************************************************
                            Types section
******************************************************************************/
typedef struct _EventToMemoryIdMapping_t
{
  uint8_t        eventId;
  S_Nv_ItemId_t itemId;
} EventToMemoryIdMapping_t;

typedef uint8_t PDS_MemMask_t[PDS_ITEM_MASK_SIZE];

/******************************************************************************
                    Prototypes section
******************************************************************************/
static void pdsObserver(SYS_EventId_t eventId, SYS_EventData_t data);
static void pdsStoreItem(S_Nv_ItemId_t id);
static bool pdsRestoreItem(S_Nv_ItemId_t id);
static bool pdsInitItemMask(S_Nv_ItemId_t memoryId, uint8_t *itemMask);

/******************************************************************************
                    Static variables section
******************************************************************************/
static SYS_EventReceiver_t pdsEventReceiver = { .func = pdsObserver};

/* Memory map, shows relationships between BitCloud events and memory which
   is updated on specific event. Such memory should be updated in non-volatile
   storage on an event occurrence. */
static EventToMemoryIdMapping_t PROGMEM_DECLARE(pdsMemoryMap[]) =
{
  EVENT_TO_MEM_ID_MAPPING(BC_EVENT_NETWORK_ENTERED,             BC_EXT_GEN_MEMORY_ITEM_ID),
  EVENT_TO_MEM_ID_MAPPING(BC_EVENT_NETWORK_ENTERED,             CS_NEIB_TABLE_ITEM_ID),
  EVENT_TO_MEM_ID_MAPPING(BC_EVENT_NETWORK_ENTERED,             CS_NWK_SECURITY_IB_ITEM_ID),
  EVENT_TO_MEM_ID_MAPPING(BC_EVENT_NETWORK_ENTERED,             CS_NWK_SECURITY_KEYS_ITEM_ID),
  EVENT_TO_MEM_ID_MAPPING(BC_EVENT_NETWORK_ENTERED,             NWK_SECURITY_COUNTERS_ITEM_ID),
  EVENT_TO_MEM_ID_MAPPING(BC_EVENT_NETWORK_ENTERED,             CS_APS_BINDING_TABLE_ITEM_ID),
  EVENT_TO_MEM_ID_MAPPING(BC_EVENT_NETWORK_ENTERED,             CS_GROUP_TABLE_ITEM_ID),
  EVENT_TO_MEM_ID_MAPPING(BC_EVENT_NETWORK_ENTERED,             NWK_RREQ_IDENTIFIER_ITEM_ID),

  EVENT_TO_MEM_ID_MAPPING(BC_EVENT_NETWORK_STARTED,             BC_EXT_GEN_MEMORY_ITEM_ID),
  EVENT_TO_MEM_ID_MAPPING(BC_EVENT_NETWORK_STARTED,             CS_NEIB_TABLE_ITEM_ID),
  EVENT_TO_MEM_ID_MAPPING(BC_EVENT_NETWORK_STARTED,             CS_NWK_SECURITY_IB_ITEM_ID),
  EVENT_TO_MEM_ID_MAPPING(BC_EVENT_NETWORK_STARTED,             CS_NWK_SECURITY_KEYS_ITEM_ID),
  EVENT_TO_MEM_ID_MAPPING(BC_EVENT_NETWORK_STARTED,             NWK_SECURITY_COUNTERS_ITEM_ID),
  EVENT_TO_MEM_ID_MAPPING(BC_EVENT_NETWORK_STARTED,             CS_APS_BINDING_TABLE_ITEM_ID),
  EVENT_TO_MEM_ID_MAPPING(BC_EVENT_NETWORK_STARTED,             CS_GROUP_TABLE_ITEM_ID),
  EVENT_TO_MEM_ID_MAPPING(BC_EVENT_NETWORK_STARTED,             NWK_RREQ_IDENTIFIER_ITEM_ID),

  EVENT_TO_MEM_ID_MAPPING(BC_EVENT_NETWORK_LEFT,                BC_EXT_GEN_MEMORY_ITEM_ID),
  EVENT_TO_MEM_ID_MAPPING(BC_EVENT_NETWORK_LEFT,                CS_NEIB_TABLE_ITEM_ID),
  EVENT_TO_MEM_ID_MAPPING(BC_EVENT_NETWORK_LEFT,                CS_NWK_SECURITY_IB_ITEM_ID),
  EVENT_TO_MEM_ID_MAPPING(BC_EVENT_NETWORK_LEFT,                NWK_SECURITY_COUNTERS_ITEM_ID),
  EVENT_TO_MEM_ID_MAPPING(BC_EVENT_NETWORK_LEFT,                CS_NWK_SECURITY_KEYS_ITEM_ID),
  EVENT_TO_MEM_ID_MAPPING(BC_EVENT_NETWORK_LEFT,                CS_APS_BINDING_TABLE_ITEM_ID),
  EVENT_TO_MEM_ID_MAPPING(BC_EVENT_NETWORK_LEFT,                CS_GROUP_TABLE_ITEM_ID),
  EVENT_TO_MEM_ID_MAPPING(BC_EVENT_NETWORK_LEFT,                NWK_RREQ_IDENTIFIER_ITEM_ID),

  EVENT_TO_MEM_ID_MAPPING(BC_EVENT_NETWORK_UPDATE,              BC_EXT_GEN_MEMORY_ITEM_ID),

  EVENT_TO_MEM_ID_MAPPING(BC_EVENT_CHILD_JOINED,                CS_NEIB_TABLE_ITEM_ID),
  EVENT_TO_MEM_ID_MAPPING(BC_EVENT_CHILD_REMOVED,               CS_NEIB_TABLE_ITEM_ID),

  EVENT_TO_MEM_ID_MAPPING(BC_EVENT_KEY_PAIR_DELETED,            CS_APS_KEY_PAIR_DESCRIPTORS_ITEM_ID),
  EVENT_TO_MEM_ID_MAPPING(BC_EVENT_APS_SECURITY_TABLES_UPDATED, CS_APS_KEY_PAIR_DESCRIPTORS_ITEM_ID),
  EVENT_TO_MEM_ID_MAPPING(BC_SECURITY_APS_COUNTERS_UPDATE,      CS_APS_KEY_PAIR_DESCRIPTORS_ITEM_ID),

  EVENT_TO_MEM_ID_MAPPING(BC_EVENT_NWK_SECURITY_TABLES_UPDATED, CS_NWK_SECURITY_IB_ITEM_ID),
  EVENT_TO_MEM_ID_MAPPING(BC_EVENT_NWK_SECURITY_TABLES_UPDATED, CS_NWK_SECURITY_KEYS_ITEM_ID),
  EVENT_TO_MEM_ID_MAPPING(BC_EVENT_NWK_SECURITY_TABLES_UPDATED, NWK_SECURITY_COUNTERS_ITEM_ID),

  EVENT_TO_MEM_ID_MAPPING(BC_EVENT_BIND_TABLE_UPDATED,          CS_APS_BINDING_TABLE_ITEM_ID),

  EVENT_TO_MEM_ID_MAPPING(BC_EVENT_GROUP_TABLE_UPDATED,         CS_GROUP_TABLE_ITEM_ID),
  EVENT_TO_MEM_ID_MAPPING(BC_EVENT_GROUPS_REMOVED,              CS_GROUP_TABLE_ITEM_ID),

  EVENT_TO_MEM_ID_MAPPING(BC_SECURITY_NWK_COUNTERS_UPDATE,      NWK_SECURITY_COUNTERS_ITEM_ID),

  EVENT_TO_MEM_ID_MAPPING(BC_EVENT_NWK_RREQ_ID_UPDATED,         NWK_RREQ_IDENTIFIER_ITEM_ID),

  EVENT_TO_MEM_ID_MAPPING(BC_EVENT_CHANNEL_CHANGED,             BC_EXT_GEN_MEMORY_ITEM_ID)
};

static uint8_t itemsToStore[PDS_ITEM_MASK_SIZE];

/******************************************************************************
                   Implementation section
******************************************************************************/
/**************************************************************************//**
\brief Restores data from non-volatile storage

PDS files not included in the current build configuration will be ignored.
Restoring process will be performed only if all files, expected for actual
configuration, are presented in NV storage

\param[in] memoryId - an identifier of PDS file or directory to be restored
                      from non-volatile memory

\return true, if all expected files have been restored, false - otherwise
******************************************************************************/
bool PDS_Restore(PDS_MemId_t memoryId)
{
  uint8_t itemsToRestore[PDS_ITEM_MASK_SIZE] = {0U};
  uint8_t i, j;

  if (!pdsInitItemMask(memoryId, itemsToRestore))
    return false;

  for (i = 0U; i < PDS_ITEM_MASK_SIZE; i++)
    for (j = 0U; j < 8U; j++)
      if (itemsToRestore[i] & (1U << j))
        if (!pdsRestoreItem(((S_Nv_ItemId_t)i << 3U) + j))
          return false;

  return true;
}

/**************************************************************************//**
\brief Deletes data from non-volatile storage

\param[in] memoryId - an identifier of PDS item or directory to be deleted
                      from non-volatile memory

\return PDS_DataServerState_t - status of PDS delete
******************************************************************************/
PDS_DataServerState_t PDS_Delete(PDS_MemId_t memoryId)
{
  uint8_t itemsToDelete[PDS_ITEM_MASK_SIZE] = {0U};
  uint8_t i, j;

  if (!pdsInitItemMask(memoryId, itemsToDelete))
    return false;

  for (i = 0U; i < PDS_ITEM_MASK_SIZE; i++)
    for (j = 0U; j < 8U; j++)
      if (itemsToDelete[i] & (1U << j))
        S_Nv_Delete(((S_Nv_ItemId_t)i << 3U) + j);

  return PDS_SUCCESS;
}
/**************************************************************************//**
\brief Stores data in non-volatile memory in background, not blocking other
       processes.

\ingroup pds

All PDS files which are absent in the current build configuration will be ignored.

\param[in] memoryId - an identifier of PDS file or directory to be stored
                      in non-volatile memory.

\return True, if storing process has begun, false - otherwise.
******************************************************************************/
bool PDS_Store(PDS_MemId_t memoryId)
{
  if (!pdsInitItemMask(memoryId, itemsToStore))
    return false;
  
  pdsPostTask(PDS_STORE_ITEM_TASK_ID);

  return true;
}

/**************************************************************************//**
\brief Deletes data from non-volatile storage except the Persistant items
       depending on the parameter passed.

\param[in] includingPersistentItems - deletes persistant items if TRUE
                                      deletes all other items except persistant 
                                      items if FALSE

\return PDS_DataServerState_t - status of PDS DeleteAll
******************************************************************************/
PDS_DataServerState_t PDS_DeleteAll(bool includingPersistentItems)
{
  S_Nv_EraseAll(includingPersistentItems);
  return PDS_SUCCESS;
}

/******************************************************************************
\brief PDS store item hadler
******************************************************************************/
void pdsStoreItemTaskHandler(void)
{
  uint8_t i, j;
  bool breakLoop = false;

  // run through all items and store the first item
  for (i = 0U; i < PDS_ITEM_MASK_SIZE; i++)
  {
    for (j = 0U; j < 8U; j++)
      if (itemsToStore[i] & (1U << j))
      {
        itemsToStore[i] &= ~(1U << j);
        breakLoop = true;
        break;
      }
    if (breakLoop)
      break;
  }

#ifdef PDS_SECURITY_CONTROL_ENABLE
  if (!pdsIsItemUnderSecurityControl(((S_Nv_ItemId_t)i << 3U) + j) && !S_Nv_IsItemAvailable(((S_Nv_ItemId_t)i << 3U) + j))
#else
  if (!S_Nv_IsItemAvailable(((S_Nv_ItemId_t)i << 3U) + j))
#endif
  {
    ItemIdToMemoryMapping_t itemDescr;

    if(pdsGetItemDescr(((S_Nv_ItemId_t)i << 3U) + j, &itemDescr))
    {
      // Not finding item so initialize it
      S_Nv_ReturnValue_t ret;
      
      if (itemDescr.filler)
        itemDescr.filler();
      ret = S_Nv_ItemInit(((S_Nv_ItemId_t)i << 3U) + j, itemDescr.itemSize, itemDescr.itemData);
      N_ERRH_ASSERT_FATAL((S_Nv_ReturnValue_DidNotExist == ret) || (S_Nv_ReturnValue_Ok == ret));
    }
  }
  else
  {
    // store found item
    pdsStoreItem((((S_Nv_ItemId_t)i) << 3U) + j);
  }

  // check whether there is any item to store
  for (i = 0U; i < PDS_ITEM_MASK_SIZE; i++)
    if (itemsToStore[i])
    {
      pdsPostTask(PDS_STORE_ITEM_TASK_ID);
      break;
    }
}

/**************************************************************************//**
\brief Provides PDS with information about memory that should be kept up-to-date
       in the non-volatile storage
       Data will be saved im non-volatile memory upon specific events

\param[in] memoryId - an identifier of PDS file or directory to be stored
                      in non-volatile memory
******************************************************************************/
void PDS_StoreByEvents(PDS_MemId_t memoryId)
{
  EventToMemoryIdMapping_t evMemoryIdMapping;

  for (uint8_t i = 0U; i < ARRAY_SIZE(pdsMemoryMap); i++)
  {
    memcpy_P(&evMemoryIdMapping, &pdsMemoryMap[i], sizeof(EventToMemoryIdMapping_t));
    if ((BC_ALL_MEMORY_MEM_ID == memoryId) || (evMemoryIdMapping.itemId == memoryId))
      SYS_SubscribeToEvent(evMemoryIdMapping.eventId, &pdsEventReceiver);
  }
}

/**************************************************************************//**
\brief Checks if the specified PDS file or directory can be restored
       from non-volatile memory
       PDS files not included in the current build configuration will be ignored

\param[in] memoryId - an identifier of PDS file or directory to be checked

\return true, if the specified memory can be restored; false - otherwise
******************************************************************************/
bool PDS_IsAbleToRestore(PDS_MemId_t memoryId)
{
  uint8_t itemsToBeAbleRestore[PDS_ITEM_MASK_SIZE] = {0U};
  uint8_t i, j;

  if (!pdsInitItemMask(memoryId, itemsToBeAbleRestore))
    return false;

  for (i = 0U; i < PDS_ITEM_MASK_SIZE; i++)
    for (j = 0U; j < 8U; j++)
      if (itemsToBeAbleRestore[i] & (1U << j))
#ifdef PDS_SECURITY_CONTROL_ENABLE
      {
        if (pdsIsItemUnderSecurityControl(((S_Nv_ItemId_t)i << 3U) + j))
        {
          if (!pdsIsSecureItemAvailable(((S_Nv_ItemId_t)i << 3U) + j))
             return false;
        }
        else
        {
          if (!S_Nv_IsItemAvailable(((S_Nv_ItemId_t)i << 3U) + j))
             return false;
        }
      }
#else
      if (!S_Nv_IsItemAvailable(((S_Nv_ItemId_t)i << 3U) + j))
          return false;
#endif

  return true;
}

/******************************************************************************
\brief PDS observer routine. Force parameters storing by an event occurrence

\param[in] eventId - occured event identifier;
\param[in] data    - associated data
******************************************************************************/
static void pdsObserver(SYS_EventId_t eventId, SYS_EventData_t data)
{
  EventToMemoryIdMapping_t evMemoryIdMapping;
  bool post = false;

  for (uint8_t i = 0U; i < ARRAY_SIZE(pdsMemoryMap); i++)
  {
    memcpy_P(&evMemoryIdMapping, &pdsMemoryMap[i], sizeof(EventToMemoryIdMapping_t));

    if (evMemoryIdMapping.eventId == eventId)
    {
      itemsToStore[evMemoryIdMapping.itemId / 8U] |= 1U << (evMemoryIdMapping.itemId % 8U);
      post = true;
    }
  }

  if (post)
    pdsPostTask(PDS_STORE_ITEM_TASK_ID);

  (void)data;
}

/******************************************************************************
\brief Stores item

\param[in] id - item id to store
******************************************************************************/
static void pdsStoreItem(S_Nv_ItemId_t id)
{
  ItemIdToMemoryMapping_t itemDescr;

  if (pdsGetItemDescr(id, &itemDescr))
  {
    N_ERRH_ASSERT_FATAL((0U != itemDescr.itemSize) && (NULL != itemDescr.itemData));
    
    if (itemDescr.filler)
      itemDescr.filler();

#ifdef PDS_SECURITY_CONTROL_ENABLE
    if (pdsIsItemUnderSecurityControl(id))
      pdsStoreSecuredItem(id, itemDescr.itemSize, itemDescr.itemData);
    else
#endif
    {
      S_Nv_ReturnValue_t ret;

      ret = S_Nv_Write(id, 0U, itemDescr.itemSize, itemDescr.itemData);
      N_ERRH_ASSERT_FATAL(ret == S_Nv_ReturnValue_Ok);
    }
  }
}

/******************************************************************************
\brief Restores item

\param[in] id - item id to store

\return true item was restored, false otherwise
******************************************************************************/
static bool pdsRestoreItem(S_Nv_ItemId_t id)
{
  ItemIdToMemoryMapping_t itemDescr;
  uint16_t oldSize;
  
  if (pdsGetItemDescr(id, &itemDescr))
  {
    N_ERRH_ASSERT_FATAL((0U != itemDescr.itemSize) && (NULL != itemDescr.itemData));

    if (itemDescr.filler)
      itemDescr.filler();

#ifdef PDS_SECURITY_CONTROL_ENABLE
    if (pdsIsItemUnderSecurityControl(id))
    {
      oldSize = pdsSecureItemLength(id);
      bool result = pdsRestoreSecuredItem(id, itemDescr.itemSize, itemDescr.itemData);

      if (result)
        if (!pdsUpdateMemory(id, itemDescr.itemData, itemDescr.itemSize, oldSize))
          return false;
      return result;
    }
    else
#endif
    {
      oldSize = S_Nv_ItemLength(id);
      S_Nv_ReturnValue_t ret = S_Nv_ItemInit(id, itemDescr.itemSize, itemDescr.itemData);

      N_ERRH_ASSERT_FATAL((S_Nv_ReturnValue_DidNotExist == ret) || (S_Nv_ReturnValue_Ok == ret));

      if (S_Nv_ReturnValue_Ok == ret)
        if (!pdsUpdateMemory(id, itemDescr.itemData, itemDescr.itemSize, oldSize))
          return false;
      return (S_Nv_ReturnValue_Ok == ret);
    }
  }
  return false;
}

/******************************************************************************
\brief To init the itemmask for the given memory ID

\param[in] memoryId - memeory id to initialize itemMask
\param[in] itemMask - itemMask to be initialized

\return true itemMask is initialized, false otherwise
******************************************************************************/
static bool pdsInitItemMask(S_Nv_ItemId_t memoryId, uint8_t *itemMask)
{
  if (PDS_ALL_EXISTENT_MEMORY == memoryId)
  {
    memset(itemMask, 0xFFU, PDS_ITEM_MASK_SIZE);
    itemMask[0] &= 0xFEU;
  }
  else if (PDS_DIRECTORY_MASK & memoryId)
  {
    PDS_DirDescr_t dirDescr;
    PDS_MemId_t memId;

    if (pdsGetDirDescr(memoryId, &dirDescr))
    {
      for (uint8_t index=0U; index < dirDescr.filesCount; index++)
      {
        memcpy_P(&memId, (void const FLASH_PTR *)(dirDescr.list + index), sizeof(uint16_t));
        itemMask[memId / 8U] |= 1U << (memId % 8U);
      }
    }
    else
      return false;
  }
  else
  {
    itemMask[memoryId / 8] |= 1 << (memoryId % 8);
  }

  return true;
}
#endif // _ENABLE_PERSISTENT_SERVER_
#endif // PDS_ENABLE_WEAR_LEVELING == 1
// eof wlpdsDataServer.c