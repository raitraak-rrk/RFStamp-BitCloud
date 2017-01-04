/************************************************************************//**
  \file wlPdsTypesConverter.h

  \brief
    PDS types convertions definition

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    21.05.13 N. Fomin - Created.
******************************************************************************/

#ifndef _WLPDS_TYPESCONVERTER
#define _WLPDS_TYPESCONVERTER

/******************************************************************************
                   Includes section
******************************************************************************/
#include <sysTypes.h>
#include <S_Nv.h>
#include <appFramework.h>
#include <wlPdsMemIds.h>

/******************************************************************************
                   Definitions section
******************************************************************************/
#ifndef FILE_TO_ITEM_MAPPING
  #define FILE_TO_ITEM_MAPPING
#endif

#define NO_ITEM_FLAGS               0x00U
#define SIZE_MODIFICATION_ALLOWED   0x01U
#define ITEM_UNDER_SECURITY_CONTROL 0x02U

#define EXTENDED_SET_ITEM_SIZE        (sizeof(ExtGetMem_t))
#define NEIGHBOR_TABLE_ITEM_SIZE      (CS_NEIB_TABLE_SIZE * sizeof(Neib_t))
#define KEY_PAIR_DESCRIPTOR_ITEM_SIZE (CS_APS_KEY_PAIR_DESCRIPTORS_AMOUNT * sizeof(ApsKeyPairDescriptor_t))
#define SECURITY_IB_ITEM_SIZE         (sizeof(NWK_SecurityIB_t))
/* SECURITY_KEYS_ITEM_SIZE includes pointers to frame counters which are not required here.
 Left now as is for compatibility. */
#define SECURITY_KEYS_ITEM_SIZE       (CS_NWK_SECURITY_KEYS_AMOUNT * sizeof(NWK_SecurityKey_t))
#define SECURITY_COUNTERS_ITEM_SIZE   (sizeof(NwkOutFrameCounterTop_t))
#define BINDING_TABLE_ITEM_SIZE       (CS_APS_BINDING_TABLE_SIZE * sizeof(ApsBindingEntry_t))
#define GROUP_TABLE_ITEM_SIZE         (CS_GROUP_TABLE_SIZE * sizeof(NWK_GroupTableEntry_t))
#define RREQ_IDENTIFIER_ITEM_SIZE     (sizeof(uint8_t))

#define ITEM_ID_TO_MEM_MAPPING(item, size, pointer, func, flag) \
  {.itemId = item, .itemSize = size, .itemData = pointer, .filler = func, .flags = flag}

/* PDS item declaration */
#define PDS_DECLARE_ITEM(item, size, pointer, func, flag) \
  PDS_FF_OBJECT(ItemIdToMemoryMapping_t pds_ff_##item) = \
    ITEM_ID_TO_MEM_MAPPING(item, size, pointer, func, flag);


#ifdef FILE_TO_ITEM_MAPPING
#define PDS_DECLARE_FILE(id, dataSize, ramAddr, fileMarks) \
        PDS_DECLARE_ITEM(id, dataSize, ramAddr, NULL, 0x00)
#else
#define PDS_DECLARE_FILE(id, dataSize, ramAddr, fileMarks)
#endif

#ifdef __IAR_SYSTEMS_ICC__
  /* Declaration to place object to PDS_FF (files) segment in MCU Flash */
  #define PDS_FF_OBJECT(object) \
    _Pragma("location=\"PDS_FF\"") __root PROGMEM_DECLARE (object)
  /* Declaration to place object to PDS_FD (directories) segment in MCU Flash */
  #define PDS_DECLARE_DIR(object) \
    _Pragma("location=\"PDS_FD\"") __root PROGMEM_DECLARE (object)
#elif __GNUC__
  #define PDS_FF_OBJECT(object) \
    __attribute__((__section__(".pds_ff"))) const object
  #define PDS_DECLARE_DIR(object) \
    __attribute__((__section__(".pds_fd"))) const object
#else
  #error "Unsupported compiler"
#endif

#ifdef  __IAR_SYSTEMS_ICC__
#pragma segment="PDS_FF"
  #define PDS_FF_START       (ItemIdToMemoryMapping_t *) __sfb("PDS_FF")
  #define PDS_FF_END         (ItemIdToMemoryMapping_t *) __sfe("PDS_FF")
  #define PDS_FF_SIZE         __sfs("PDS_FF")
#pragma segment="PDS_FD"
  #define PDS_FD_START       (PDS_DirDescr_t *) __sfb("PDS_FD")
  #define PDS_FD_END         (PDS_DirDescr_t *) __sfe("PDS_FD")
  #define PDS_FD_SIZE        __sfs("PDS_FD")
#elif __GNUC__
  #define PDS_FF_START       (ItemIdToMemoryMapping_t *) &__pds_ff_start
  #define PDS_FF_END         (ItemIdToMemoryMapping_t *) &__pds_ff_end
  #define PDS_FF_SIZE        (&__pds_ff_end - &__pds_ff_start)
  #define PDS_FD_START       (PDS_DirDescrRec_t) &__pds_fd_start
  #define PDS_FD_END         (PDS_DirDescrRec_t) &__pds_fd_end
  #define PDS_FD_SIZE        (&__pds_fd_end - &__pds_fd_start)
#else
  #error "Unsupported compiler"
#endif

/******************************************************************************
                   Types section
******************************************************************************/
/* The extended set of commonly used BitCloud parameters that can
be stored in the non-volatile memory and identified via BC_EXT_GEN_MEMORY_MEM_ID */
typedef struct
{
  uint64_t         csUid;
  uint8_t          txPower;
  uint64_t         extPanId;
  uint32_t         channelMask;
  uint8_t          channelPage;
  DeviceType_t     deviceType;
  bool             rxOnWhenIdle;
  bool             complexDescrAvailable;
  bool             userDescrAvailable;
  UserDescriptor_t userDescr;
  uint16_t         panId;
  bool             predefinedPanId;
  uint16_t         shortAddress;
  bool             uniqueNwkAddress;
  bool             leaveReqAllowed;
  bool             dtrWakeUp;
  uint8_t          updateId;
  uint64_t         extTcAddress;
  uint16_t         nwkTcAddress;
  uint8_t          securityStatus;
  uint16_t         parentNwkAddress;
  uint8_t          nwkDepth;
  uint64_t         nwkExtPanId;
  uint8_t          logicalChannel;
} ExtGetMem_t;

typedef struct
{
  S_Nv_ItemId_t itemId;
  uint16_t       itemSize;
  void           *itemData;
  void           (*filler)(void);
  uint8_t        flags;
} ItemIdToMemoryMapping_t;

typedef struct
{
  PDS_MemIdRec_t list;
  uint16_t    filesCount;
  PDS_MemId_t memoryId;
} PDS_DirDescr_t;

/* Directory descriptor record pointer */
typedef const PDS_DirDescr_t FLASH_PTR *PDS_DirDescrRec_t;

/******************************************************************************
                   Extern section
******************************************************************************/
extern ExtGetMem_t extGenMem;

#ifdef __GNUC__
extern uint32_t __pds_ff_start;
extern uint32_t __pds_ff_end;
extern uint32_t __pds_fd_start;
extern uint32_t __pds_fd_end;
#endif /* __GNUC__ */

/******************************************************************************
                    Prototypes section
******************************************************************************/
/******************************************************************************
\brief Checks whether item is under security control

\param[in] id - item id

\returns true if item is under security control, false - otherwise
******************************************************************************/
bool pdsIsItemUnderSecurityControl(S_Nv_ItemId_t id);

/******************************************************************************
\brief Updates BC parameters after restoring taking into account possible size
  changes

\param[in] id      - item id;
\param[in] data    - pointer to data;
\param[in] size    - current item size;
\param[in] oldSize - last size of item

returns true if memory was updated successfully, false - otherwise
******************************************************************************/
bool pdsUpdateMemory(S_Nv_ItemId_t id, void *data, uint16_t size, uint16_t oldSize);

/******************************************************************************
\brief To get the directory descriptor for the given dir ID

\param[in] itemId           - item id;
\param[in] dirDescrToGet    - pointer to dir descriptor to be loaded;

returns true if descriptor is found out for the given dir ID, false - otherwise
******************************************************************************/
bool pdsGetDirDescr(S_Nv_ItemId_t itemId, PDS_DirDescr_t *dirDescrToGet );

/******************************************************************************
\brief To get the item descriptor for the given item ID

\param[in] itemId            - item id;
\param[in] itemDescrToGet    - pointer to item descriptor to be loaded;

returns true if descriptor is found out for the given item ID, false - otherwise
******************************************************************************/
bool pdsGetItemDescr(S_Nv_ItemId_t itemId, ItemIdToMemoryMapping_t *itemDescrToGet );

/******************************************************************************
\brief To fill the extended BC set
******************************************************************************/
void fillExtendedBcSet(void);

#endif // _WLPDS_TYPESCONVERTER
/* eof wlPdsTypesConverter.h */
