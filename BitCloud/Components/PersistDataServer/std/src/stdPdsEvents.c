/**************************************************************************//**
  \file pdsEvents.c

  \brief Persistent Data Server events handling mechanism.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2012-03-14 A. Razinkov  - Created.
   Last change:
    $Id: pdsEvents.c 19835 2012-01-24 16:51:08Z arazinkov $
 ******************************************************************************/

#if PDS_ENABLE_WEAR_LEVELING != 1
#ifdef _ENABLE_PERSISTENT_SERVER_
/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <pdsDataServer.h>
#include <stdPdsDbg.h>
#include <stdPdsMem.h>
#include <sysEvents.h>
#include <stdPdsMemAccess.h>

#define EVENT_TO_MEM_ID_MAPPING(event, id)  {.eventId = event, .memoryId = id}

typedef struct _EventToMemoryIdMapping_t
{
  uint8_t eventId;
  PDS_MemId_t memoryId;
} EventToMemoryIdMapping_t;

/******************************************************************************
                    Prototypes section
******************************************************************************/
static void pdsObserver(SYS_EventId_t eventId, SYS_EventData_t data);

/******************************************************************************
                    Static variables section
******************************************************************************/
/* BitCloud events receiver */
static SYS_EventReceiver_t pdsEventReceiver = { .func = pdsObserver};

/* Memory map, shows relationships between BitCloud events and memory which
   is updated on specific event. Such memory should be updated in non-volatile
   storage on an event occurrence. */
static EventToMemoryIdMapping_t PROGMEM_DECLARE(pdsMemoryMap[]) =
{
  EVENT_TO_MEM_ID_MAPPING(BC_EVENT_NETWORK_ENTERED,             BC_ALL_MEMORY_MEM_ID),
  EVENT_TO_MEM_ID_MAPPING(BC_EVENT_NETWORK_STARTED,             BC_ALL_MEMORY_MEM_ID),
  EVENT_TO_MEM_ID_MAPPING(BC_EVENT_NETWORK_LEFT,                BC_ALL_MEMORY_MEM_ID),
  EVENT_TO_MEM_ID_MAPPING(BC_EVENT_NETWORK_LOST,                PDS_NO_MEMORY_SPECIFIED),
  EVENT_TO_MEM_ID_MAPPING(BC_EVENT_NETWORK_UPDATE,              BC_EXT_GEN_MEMORY_MEM_ID),

  EVENT_TO_MEM_ID_MAPPING(BC_EVENT_CHILD_JOINED,                CS_NEIB_TABLE_MEM_ID),
  EVENT_TO_MEM_ID_MAPPING(BC_EVENT_CHILD_REMOVED,               CS_NEIB_TABLE_MEM_ID),
  EVENT_TO_MEM_ID_MAPPING(BC_EVENT_CHILD_KEY_ABSENT,            CS_NEIB_TABLE_MEM_ID),

  EVENT_TO_MEM_ID_MAPPING(BC_EVENT_KEY_PAIR_DELETED,            CS_APS_KEY_PAIR_DESCRIPTORS_MEM_ID),
  EVENT_TO_MEM_ID_MAPPING(BC_EVENT_APS_SECURITY_TABLES_UPDATED, CS_APS_KEY_PAIR_DESCRIPTORS_MEM_ID),
  EVENT_TO_MEM_ID_MAPPING(BC_SECURITY_APS_COUNTERS_UPDATE,      CS_APS_KEY_PAIR_DESCRIPTORS_MEM_ID),
  EVENT_TO_MEM_ID_MAPPING(BC_EVENT_NWK_SECURITY_TABLES_UPDATED, BC_NWK_SECURITY_TABLES_MEM_ID),
  EVENT_TO_MEM_ID_MAPPING(BC_SECURITY_NWK_COUNTERS_UPDATE,      NWK_SECURITY_COUNTERS_MEM_ID),
  EVENT_TO_MEM_ID_MAPPING(BC_EVENT_BIND_TABLE_UPDATED,          CS_APS_BINDING_TABLE_MEM_ID),
  EVENT_TO_MEM_ID_MAPPING(BC_EVENT_GROUP_TABLE_UPDATED,         CS_GROUP_TABLE_MEM_ID),
  EVENT_TO_MEM_ID_MAPPING(BC_EVENT_GROUPS_REMOVED,              CS_GROUP_TABLE_MEM_ID),
  EVENT_TO_MEM_ID_MAPPING(BC_EVENT_PERMISSION_TABLE_UPDATED,    CS_PERMISSIONS_TABLE_MEM_ID),

  EVENT_TO_MEM_ID_MAPPING(BC_EVENT_ADDRESS_CONFLICT, PDS_NO_MEMORY_SPECIFIED),
  EVENT_TO_MEM_ID_MAPPING(BC_EVENT_TC_OFFLINE,       PDS_NO_MEMORY_SPECIFIED),
  EVENT_TO_MEM_ID_MAPPING(BC_EVENT_NEW_CONCENTRATOR, PDS_NO_MEMORY_SPECIFIED),
  EVENT_TO_MEM_ID_MAPPING(BC_EVENT_DEVICE_ANNCE,     PDS_NO_MEMORY_SPECIFIED),
  EVENT_TO_MEM_ID_MAPPING(BC_EVENT_NWK_RREQ_ID_UPDATED,         NWK_RREQ_IDENTIFIER_MEM_ID)
};

/******************************************************************************
                   Implementation section
******************************************************************************/
/***************************************************************************//**
\brief Provides PDS with information about memory that should be kept up-to-date
       in the non-volatile storage.

\ingroup pds

Data will be saved im non-volatile memory upon specific events.

\param[in] memoryId - an identifier of PDS file or directory to be stored
                      in non-volatile memory.
******************************************************************************/
void PDS_StoreByEvents(PDS_MemId_t memoryId)
{
  PDS_MemMask_t reqMemMask;
  PDS_MemMask_t eventMemMask;
  EventToMemoryIdMapping_t evMemoryIdMapping;

  PDS_MEM_MASK_CLEAR_ALL(eventMemMask);
  pdsInitMemMask(memoryId, reqMemMask);

  /* Subscribe to required events to store memory specified */
  for (uint8_t i = 0U; i < ARRAY_SIZE(pdsMemoryMap); i++)
  {
    memcpy_P(&evMemoryIdMapping, &pdsMemoryMap[i], sizeof(EventToMemoryIdMapping_t));

    /* Unsubscribe from all events at first */
    SYS_UnsubscribeFromEvent(evMemoryIdMapping.eventId, &pdsEventReceiver);

    PDS_MEM_MASK_SET_BIT(eventMemMask, evMemoryIdMapping.memoryId);
    pdsExpandWithDirContent(eventMemMask);

    for (uint8_t i = 0; i < PDS_MEM_MASK_LENGTH; i++)
    {
      if (eventMemMask[i] & reqMemMask[i])
      {
        SYS_SubscribeToEvent(evMemoryIdMapping.eventId, &pdsEventReceiver);
        break;
      }
    }
  }
}

/******************************************************************************
\brief PDS observer routine. Force parameters storing by an event occurrence.

\param[in] eventId - occured event identifier
\param[in] data - associated data
******************************************************************************/
static void pdsObserver(SYS_EventId_t eventId, SYS_EventData_t data)
{
  EventToMemoryIdMapping_t evMemoryIdMapping;

  /* Ignore all events in STOPPED state */
  if (pdsMemory()->status & PDS_STOPPED_FLAG)
    return;

  for (uint8_t i = 0U; i < ARRAY_SIZE(pdsMemoryMap); i++)
  {
    memcpy_P(&evMemoryIdMapping, &pdsMemoryMap[i], sizeof(EventToMemoryIdMapping_t));

    if (evMemoryIdMapping.eventId == eventId &&
        PDS_NO_MEMORY_SPECIFIED != evMemoryIdMapping.memoryId)
      PDS_Store(evMemoryIdMapping.memoryId);
  }

  (void)data;
}

#endif /* _ENABLE_PERSISTENT_SERVER_ */
#endif /* PDS_ENABLE_WEAR_LEVELING != 1 */
/* eof pdsEvents.c */

