/**************************************************************************//**
  \file wlPdsSecurity.c

  \brief PDS security implementation.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    04.06.13 N. Fomin - Created.
******************************************************************************/

#if PDS_ENABLE_WEAR_LEVELING == 1
#ifdef _ENABLE_PERSISTENT_SERVER_
#ifdef PDS_SECURITY_CONTROL_ENABLE
/******************************************************************************
                    Includes section
******************************************************************************/
#include <wlPdsMemIds.h>
#include <csDefaults.h>
#include <nwkSecurity.h>
#include <wlPdsSecurity.h>
#include <wlPdsTypesConverter.h>
#include <eeprom.h>
#include <N_ErrH.h>
#include <sysUtils.h>
#include <csBuffers.h>

/******************************************************************************
                    Definitions section
******************************************************************************/
#define COMPID                           "wlPdsSecurity"
#define EEPROM_STORAGE_BASE_ADDRESS      8U
#define MAGIC_NUMBER                     0x5AU
#define MAGIC_NUMBER_INVALID             0xFFU
#define AREA_ONE_BIT_FIELD               0U
#define AREA_TWO_BIT_FIELD               1U
#define INVALID_DATA_IN_BOTH_AREA        0U
#define ONLY_AREA_ONE_VALID              1U
#define ONLY_AREA_TWO_VALID              2U
#define VALID_DATA_IN_BOTH_AREA          3U

#define NWK_KEYS_STORAGE_AREA_ONE_START  EEPROM_STORAGE_BASE_ADDRESS
#define NWK_KEYS_STORAGE_AREA_TWO_START  (NWK_KEYS_STORAGE_AREA_ONE_START + \
  sizeof(PdsSecureItemHeader_t) + SECURITY_KEYS_ITEM_SIZE)
#define LINK_KEYS_STORAGE_AREA_ONE_START (NWK_KEYS_STORAGE_AREA_TWO_START + \
  sizeof(PdsSecureItemHeader_t) + SECURITY_KEYS_ITEM_SIZE)
#define LINK_KEYS_STORAGE_AREA_TWO_START (LINK_KEYS_STORAGE_AREA_ONE_START + \
  sizeof(PdsSecureItemHeader_t) + KEY_PAIR_DESCRIPTOR_ITEM_SIZE)

#define ITEM_ID_TO_ADDRESS_MAPPING(item, address1, address2) \
  {.itemId = item, .areaOneStartAddress = address1, .areaTwoStartAddress = address2}

/******************************************************************************
                    Types section
******************************************************************************/
BEGIN_PACK
typedef struct PACK
{
  uint8_t        magicNumber;
  S_Nv_ItemId_t id;
  uint16_t       size;
  uint16_t       crc;
  uint16_t       version;
} PdsSecureItemHeader_t;
END_PACK

typedef enum
{
  PDS_SECURITY_IDLE,
  PDS_SECURITY_WRITING_DATA,
  PDS_SECURITY_WRITING_HEADER,
  PDS_SECURITY_WRITING_MAGIC_NUMBER,
} PdsSecurityState_t;

typedef struct
{
  S_Nv_ItemId_t id;
  uint16_t       size;
  void           *data;
} PdsDataDescriptor_t;

typedef struct
{
  void     *data;
  uint16_t startAddress;
} PdsSecurityMem_t;

typedef struct
{
  S_Nv_ItemId_t itemId;
  uint16_t       areaOneStartAddress;
  uint16_t       areaTwoStartAddress;
} ItemIdToEepromOffsetMapping_t;

/******************************************************************************
                    Prototypes section
******************************************************************************/
static uint16_t pdsCalculateCrc(uint16_t size, uint8_t *data);
static void initializeEepromDescriptor(HAL_EepromParams_t *params, uint16_t address, uint8_t *data, uint16_t length);
static void writeDone(void);
static bool isDataModified(uint16_t eepromOffset, uint8_t *ramData, uint16_t size);
static bool readData(PdsSecureItemHeader_t *header, uint16_t address, void *data, uint16_t size);
static int8_t getEepromItemPosition(S_Nv_ItemId_t id);
static bool isValidArea(uint16_t eepromOffset, uint16_t size, PdsSecureItemHeader_t *header);

/******************************************************************************
                    Static variables section
******************************************************************************/
static PdsSecureItemHeader_t itemHeader;
static PdsSecurityState_t state;
static PdsSecurityMem_t pdsSecMem;
static ItemIdToEepromOffsetMapping_t PROGMEM_DECLARE(pdsSecMemoryMap[]) =
{
  ITEM_ID_TO_ADDRESS_MAPPING(CS_NWK_SECURITY_KEYS_ITEM_ID,
    NWK_KEYS_STORAGE_AREA_ONE_START,
    NWK_KEYS_STORAGE_AREA_TWO_START),
  ITEM_ID_TO_ADDRESS_MAPPING(CS_APS_KEY_PAIR_DESCRIPTORS_ITEM_ID,
    LINK_KEYS_STORAGE_AREA_ONE_START,
    LINK_KEYS_STORAGE_AREA_TWO_START)
};
static PdsDataDescriptor_t postoponedStoring[ARRAY_SIZE(pdsSecMemoryMap)];

/******************************************************************************
                    Implementation section
******************************************************************************/
/******************************************************************************
\brief To get the length of an item.

\param[in] id   - item id
\returns The length of the item
******************************************************************************/
uint16_t pdsSecureItemLength(S_Nv_ItemId_t id)
{
  HAL_EepromParams_t eepromParams;
  PdsSecureItemHeader_t header1, header2;
  int8_t pos;
  ItemIdToEepromOffsetMapping_t mapItem;

  N_ERRH_ASSERT_FATAL(PDS_SECURITY_IDLE == state);
  pos = getEepromItemPosition(id);
  N_ERRH_ASSERT_FATAL(-1 != pos);

  memcpy_P(&mapItem, &pdsSecMemoryMap[pos], sizeof(ItemIdToEepromOffsetMapping_t));
  // find the latest version of item stored
  initializeEepromDescriptor(&eepromParams, mapItem.areaOneStartAddress, (uint8_t *)&header1, sizeof(PdsSecureItemHeader_t));
  N_ERRH_ASSERT_FATAL(0U == HAL_ReadEeprom(&eepromParams, NULL));
  initializeEepromDescriptor(&eepromParams, mapItem.areaTwoStartAddress, (uint8_t *)&header2, sizeof(PdsSecureItemHeader_t));
  N_ERRH_ASSERT_FATAL(0U == HAL_ReadEeprom(&eepromParams, NULL));
  
  if ((!((UINT16_MAX == header1.version) && (0L == header2.version))) &&
       (((0L == header1.version) && (UINT16_MAX == header2.version)) || (header1.version > header2.version)))
  {
    if (MAGIC_NUMBER == header1.magicNumber)
      return (header1.size);
    else
      return (header2.size);
  }
  else
  {
    if (MAGIC_NUMBER == header2.magicNumber)
      return (header2.size);
    else
      return (header1.size);
  }
}

/******************************************************************************
\brief This will check whether the item is found in the storage area

\param[in] id   - item id
\returns true if item is available, false - otherwise
******************************************************************************/
bool pdsIsSecureItemAvailable(S_Nv_ItemId_t id)
{
  HAL_EepromParams_t eepromParams;
  PdsSecureItemHeader_t header1, header2;
  int8_t pos;
  ItemIdToEepromOffsetMapping_t mapItem;

  N_ERRH_ASSERT_FATAL(PDS_SECURITY_IDLE == state);
  pos = getEepromItemPosition(id);
  N_ERRH_ASSERT_FATAL(-1 != pos);

  memcpy_P(&mapItem, &pdsSecMemoryMap[pos], sizeof(ItemIdToEepromOffsetMapping_t));
  // find the latest version of item stored
  initializeEepromDescriptor(&eepromParams, mapItem.areaOneStartAddress, (uint8_t *)&header1, sizeof(PdsSecureItemHeader_t));
  N_ERRH_ASSERT_FATAL(0U == HAL_ReadEeprom(&eepromParams, NULL));
  initializeEepromDescriptor(&eepromParams, mapItem.areaTwoStartAddress, (uint8_t *)&header2, sizeof(PdsSecureItemHeader_t));
  N_ERRH_ASSERT_FATAL(0U == HAL_ReadEeprom(&eepromParams, NULL));
  
  if ((!((UINT16_MAX == header1.version) && (0L == header2.version))) &&
       (((0L == header1.version) && (UINT16_MAX == header2.version)) || (header1.version > header2.version)))
  {
    if (MAGIC_NUMBER == header1.magicNumber)
      return ( header1.id == id );
    else
      return ( header2.id == id );
  }
  else
  {
    if (MAGIC_NUMBER == header2.magicNumber)
      return ( header2.id == id );
    else
      return ( header1.id == id );
  }
}

/******************************************************************************
\brief This will create an empty item in storage area

\param[in] id   - item id
\param[in] itemLength   - item size
******************************************************************************/
void pdsSecureItemInit(S_Nv_ItemId_t id, uint16_t itemLength)
{
  HAL_EepromParams_t eepromParams;
  ItemIdToEepromOffsetMapping_t mapItem;
  int8_t pos;

  N_ERRH_ASSERT_FATAL(PDS_SECURITY_IDLE == state);
  pos = getEepromItemPosition(id);
  N_ERRH_ASSERT_FATAL(-1 != pos);

  memcpy_P(&mapItem, &pdsSecMemoryMap[pos], sizeof(ItemIdToEepromOffsetMapping_t));

  itemHeader.magicNumber = MAGIC_NUMBER_INVALID;
  itemHeader.id = id;
  itemHeader.size = itemLength;
  itemHeader.version = 1;
  if (isDataModified(mapItem.areaOneStartAddress, (uint8_t *)&itemHeader, sizeof(PdsSecureItemHeader_t)))
  {
    initializeEepromDescriptor(&eepromParams, mapItem.areaOneStartAddress, (uint8_t *)&itemHeader, sizeof(PdsSecureItemHeader_t));
    N_ERRH_ASSERT_FATAL(0U == HAL_WriteEeprom(&eepromParams, NULL));
  }
  
  itemHeader.id = 0;
  itemHeader.size = 0;
  itemHeader.version = 0;
  if (isDataModified(mapItem.areaTwoStartAddress, (uint8_t *)&itemHeader, sizeof(PdsSecureItemHeader_t)))
  {
    initializeEepromDescriptor(&eepromParams, mapItem.areaTwoStartAddress, (uint8_t *)&itemHeader, sizeof(PdsSecureItemHeader_t));
    N_ERRH_ASSERT_FATAL(0U == HAL_WriteEeprom(&eepromParams, NULL));
  }
}

/******************************************************************************
\brief Erases item under security control
******************************************************************************/
void PDS_EraseSecureItems(void)
{
  HAL_EepromParams_t eepromParams;
  ItemIdToEepromOffsetMapping_t mapItem;

  HAL_StopWriteToEeprom();

  itemHeader.id = 0;
  itemHeader.size = 0;
  itemHeader.version = 0;

  for (uint16_t i = 0U; i < ARRAY_SIZE(pdsSecMemoryMap); i++)
  {
    memcpy_P(&mapItem, &pdsSecMemoryMap[i], sizeof(ItemIdToEepromOffsetMapping_t));

    initializeEepromDescriptor(&eepromParams, mapItem.areaOneStartAddress, (uint8_t *)&itemHeader, sizeof(PdsSecureItemHeader_t));
    itemHeader.magicNumber = MAGIC_NUMBER_INVALID;
    N_ERRH_ASSERT_FATAL(0U == HAL_WriteEeprom(&eepromParams, NULL));

    initializeEepromDescriptor(&eepromParams, mapItem.areaTwoStartAddress, (uint8_t *)&itemHeader, sizeof(PdsSecureItemHeader_t));
    itemHeader.magicNumber = MAGIC_NUMBER_INVALID;
    N_ERRH_ASSERT_FATAL(0U == HAL_WriteEeprom(&eepromParams, NULL));
  }
}

/******************************************************************************
\brief Stores item under security control

\param[in] id   - item id;
\param[in] size - item size;
\param[in] data - item data
******************************************************************************/
void pdsStoreSecuredItem(S_Nv_ItemId_t id, uint16_t size, void *data)
{
  PdsSecureItemHeader_t header1, header2;
  HAL_EepromParams_t eepromParams;
  int8_t pos = getEepromItemPosition(id);
  ItemIdToEepromOffsetMapping_t mapItem;
  uint8_t areaValidBitmap = 0;

  N_ERRH_ASSERT_FATAL(-1 != pos);

  if (PDS_SECURITY_IDLE != state)
  {
    for (uint8_t count=0; count < ARRAY_SIZE(pdsSecMemoryMap); count++)
    {
      if(postoponedStoring[count].id == id || postoponedStoring[count].size == 0U)
      {
        postoponedStoring[count].id   = id;
        postoponedStoring[count].size = size;
        postoponedStoring[count].data = data;
        break;
      }
    }
    return;
  }

  memcpy_P(&mapItem, &pdsSecMemoryMap[pos], sizeof(ItemIdToEepromOffsetMapping_t));

  /* Read Area One Header */
  initializeEepromDescriptor(&eepromParams, mapItem.areaOneStartAddress, (uint8_t *)&header1, sizeof(PdsSecureItemHeader_t));
  N_ERRH_ASSERT_FATAL(0U == HAL_ReadEeprom(&eepromParams, NULL));

  /* Check Area One has the valid data or not */
  if (isValidArea(mapItem.areaOneStartAddress + sizeof(PdsSecureItemHeader_t), size, &header1))
    areaValidBitmap |= (1U << AREA_ONE_BIT_FIELD);

  /* Read Area Two Header */
  initializeEepromDescriptor(&eepromParams, mapItem.areaTwoStartAddress, (uint8_t *)&header2, sizeof(PdsSecureItemHeader_t));
  N_ERRH_ASSERT_FATAL(0U == HAL_ReadEeprom(&eepromParams, NULL));

  /* Check Area One has the valid data or not */
  if (isValidArea(mapItem.areaTwoStartAddress + sizeof(PdsSecureItemHeader_t), size, &header2))
    areaValidBitmap |= (1U << AREA_TWO_BIT_FIELD);

  switch (areaValidBitmap)
  {
    case INVALID_DATA_IN_BOTH_AREA:
    {
      /* Invalid data in both the area - write the data with version as 1 in Area One*/
      initializeEepromDescriptor(&eepromParams, mapItem.areaOneStartAddress, (uint8_t *)&itemHeader, sizeof(PdsSecureItemHeader_t));
      itemHeader.version = 1U;
      pdsSecMem.startAddress = mapItem.areaOneStartAddress;
    }
    break;
    case ONLY_AREA_ONE_VALID:
    {
      if (!isDataModified(mapItem.areaOneStartAddress + sizeof(PdsSecureItemHeader_t), data, size))
        return;
      itemHeader.version = header1.version + 1U;
      initializeEepromDescriptor(&eepromParams, mapItem.areaTwoStartAddress, (uint8_t *)&itemHeader, sizeof(PdsSecureItemHeader_t));
      pdsSecMem.startAddress = mapItem.areaTwoStartAddress;
    }
    break;
    case ONLY_AREA_TWO_VALID:
    {
      if (!isDataModified(mapItem.areaTwoStartAddress + sizeof(PdsSecureItemHeader_t), data, size))
        return;
      itemHeader.version = header2.version + 1U;
      initializeEepromDescriptor(&eepromParams, mapItem.areaOneStartAddress, (uint8_t *)&itemHeader, sizeof(PdsSecureItemHeader_t));
      pdsSecMem.startAddress = mapItem.areaOneStartAddress;
    }
     break;
    case VALID_DATA_IN_BOTH_AREA:
    {
      if ((!((UINT16_MAX == header1.version) && (0L == header2.version))) &&
          ((header1.version > header2.version) || ((0L == header1.version) && (UINT16_MAX == header2.version))))
      {
        if (!isDataModified(mapItem.areaOneStartAddress + sizeof(PdsSecureItemHeader_t), data, size))
          return;
        itemHeader.version = header1.version + 1U;
        initializeEepromDescriptor(&eepromParams, mapItem.areaTwoStartAddress, (uint8_t *)&itemHeader, sizeof(PdsSecureItemHeader_t));
        pdsSecMem.startAddress = mapItem.areaTwoStartAddress;
      }
      else
      {
        if (!isDataModified(mapItem.areaTwoStartAddress + sizeof(PdsSecureItemHeader_t), data, size))
          return;
        itemHeader.version = header2.version + 1U;
        initializeEepromDescriptor(&eepromParams, mapItem.areaOneStartAddress, (uint8_t *)&itemHeader, sizeof(PdsSecureItemHeader_t));
        pdsSecMem.startAddress = mapItem.areaOneStartAddress;
      }
    }
    break;
    default:
      N_ERRH_FATAL();
    break;
  }

  pdsSecMem.startAddress += sizeof(PdsSecureItemHeader_t);
  state = PDS_SECURITY_WRITING_HEADER;

  pdsSecMem.data         = data;
  /* Write Invalid Magic Number during start of the write
   and Write the valid magic number when item is stored successfully */
  itemHeader.magicNumber = MAGIC_NUMBER_INVALID;
  itemHeader.id          = id;
  itemHeader.size        = size;
  itemHeader.crc         = pdsCalculateCrc(size, data);
  N_ERRH_ASSERT_FATAL(0U == HAL_WriteEeprom(&eepromParams, writeDone));
}

/******************************************************************************
\brief Restores item under security control

\param[in] id   - item id;
\param[in] size - item size;
\param[in] data - item data
\returns true if item is restored successfully, false - otherwise
******************************************************************************/
bool pdsRestoreSecuredItem(S_Nv_ItemId_t id, uint16_t size, void *data)
{
  HAL_EepromParams_t eepromParams;
  PdsSecureItemHeader_t header1, header2;
  int8_t pos;
  ItemIdToEepromOffsetMapping_t mapItem;

  N_ERRH_ASSERT_FATAL(PDS_SECURITY_IDLE == state);
  pos = getEepromItemPosition(id);
  N_ERRH_ASSERT_FATAL(-1 != pos);

  memcpy_P(&mapItem, &pdsSecMemoryMap[pos], sizeof(ItemIdToEepromOffsetMapping_t));
  // find the latest version of item stored
  initializeEepromDescriptor(&eepromParams, mapItem.areaOneStartAddress, (uint8_t *)&header1, sizeof(PdsSecureItemHeader_t));
  N_ERRH_ASSERT_FATAL(0U == HAL_ReadEeprom(&eepromParams, NULL));
  initializeEepromDescriptor(&eepromParams, mapItem.areaTwoStartAddress, (uint8_t *)&header2, sizeof(PdsSecureItemHeader_t));
  N_ERRH_ASSERT_FATAL(0U == HAL_ReadEeprom(&eepromParams, NULL));

  if ((!((UINT16_MAX == header1.version) && (0L == header2.version))) &&
       (((0L == header1.version) && (UINT16_MAX == header2.version)) || (header1.version > header2.version)))
  {
    if (!readData(&header1, mapItem.areaOneStartAddress + sizeof(PdsSecureItemHeader_t), data, size))
      return readData(&header2, mapItem.areaTwoStartAddress + sizeof(PdsSecureItemHeader_t), data, size);
  }
  else
  {
    if (!readData(&header2, mapItem.areaTwoStartAddress + sizeof(PdsSecureItemHeader_t), data, size))
      return readData(&header1, mapItem.areaOneStartAddress + sizeof(PdsSecureItemHeader_t), data, size);
  }
  return true;
}

/******************************************************************************
\brief Checks whether the Data in the Area is valid or not by checking the magic
       number and crc in header & CRC of the data in eeprom is valid or not
\param[in] eepromOffset - ofsset of data in eeprom;
\param[in] ramData      - pointer to data in RAM;
\param[in] size         - data size;
\returns true if the area is valid, false otherwise
******************************************************************************/
static bool isValidArea(uint16_t eepromOffset, uint16_t size, PdsSecureItemHeader_t *header)
{
  uint8_t readBuffer[16U];
  HAL_EepromParams_t eepromParams;
  uint8_t i = 0U, j;
  uint16_t eepromCrc = 0U;

  if (MAGIC_NUMBER != header->magicNumber)
    return false;

  for (; i < size / 16U; i++)
  {
    initializeEepromDescriptor(&eepromParams, eepromOffset + i * 16U, readBuffer, 16U);
    if (0U != HAL_ReadEeprom(&eepromParams, NULL))
      return false;
    for (j = 0U; j < 16U; j++)
      eepromCrc = SYS_Crc16Ccitt(eepromCrc, readBuffer[j]);
  }
  if (size % 16U)
  {
    initializeEepromDescriptor(&eepromParams, eepromOffset + i * 16U, readBuffer, size % 16U);
    if (0U != HAL_ReadEeprom(&eepromParams, NULL))
      return false;
    for (j = 0U; j < (size % 16U); j++)
      eepromCrc = SYS_Crc16Ccitt(eepromCrc, readBuffer[j]);
  }

  if (header->crc != eepromCrc)
    return false;

  return true;
}
/******************************************************************************
\brief Calculates crc of item data

\param[in] size - item size;
\param[in] data - item data
\returns crc
******************************************************************************/
static uint16_t pdsCalculateCrc(uint16_t size, uint8_t *data)
{
  uint16_t crc = 0U;

  for (uint16_t i = 0U; i < size; i++)
    crc = SYS_Crc16Ccitt(crc, data[i]);

  return crc;
}

/******************************************************************************
\brief Initializes EEPROM descriptor

\param[in] params  - pointer to strucure with eeprom descriptor;
\param[in] address - address in EEPROM to write to;
\param[in] data    - item data;
\param[in] length  - item length
******************************************************************************/
static void initializeEepromDescriptor(HAL_EepromParams_t *params, uint16_t address, uint8_t *data, uint16_t length)
{
  params->address = address;
  params->data    = data;
  params->length  = length;
}

/******************************************************************************
\brief Writing to EEPROM is done
******************************************************************************/
static void writeDone(void)
{
  HAL_EepromParams_t eepromParams;
  switch(state)
  {
    case PDS_SECURITY_WRITING_HEADER:
    {
      if (pdsSecMem.data)
      {
        initializeEepromDescriptor(&eepromParams, pdsSecMem.startAddress, (uint8_t *)pdsSecMem.data, itemHeader.size);
        state = PDS_SECURITY_WRITING_DATA;
        HAL_WriteEeprom(&eepromParams, writeDone);
        break;
      }
    }
    case PDS_SECURITY_WRITING_DATA:
    {
      itemHeader.magicNumber = MAGIC_NUMBER;
      initializeEepromDescriptor(&eepromParams, pdsSecMem.startAddress - sizeof(PdsSecureItemHeader_t), (uint8_t *)&itemHeader, sizeof(PdsSecureItemHeader_t));
      state = PDS_SECURITY_WRITING_MAGIC_NUMBER;
      HAL_WriteEeprom(&eepromParams, writeDone);
      break;
    }
    case PDS_SECURITY_WRITING_MAGIC_NUMBER:
    {
      state = PDS_SECURITY_IDLE;
      if (0U != postoponedStoring[0].size)
      {
        pdsStoreSecuredItem(postoponedStoring[0].id, postoponedStoring[0].size, postoponedStoring[0].data);
        postoponedStoring[0].id = 0U;
        postoponedStoring[0].size = 0U;
        for (uint8_t index=0,next_index = 0; index < ARRAY_SIZE(pdsSecMemoryMap); index++)
        {
          next_index++;
          if(next_index >= ARRAY_SIZE(pdsSecMemoryMap))
            next_index = 0;
          postoponedStoring[index].id = postoponedStoring[next_index].id;
          postoponedStoring[index].size = postoponedStoring[next_index].size;
          postoponedStoring[index].data = postoponedStoring[next_index].data;
        }
       }
      break;
    }
    default:
      N_ERRH_FATAL();
  }
}

/******************************************************************************
\brief Checks whether data in EEPROM differs from data in RAM

\param[in] eepromOffset - ofsset of data in eeprom;
\param[in] ramData      - pointer to data in RAM;
\param[in] size         - data size;
\returns true if data differs, false otherwise
******************************************************************************/
static bool isDataModified(uint16_t eepromOffset, uint8_t *ramData, uint16_t size)
{
  uint8_t readBuffer[16U];
  HAL_EepromParams_t eepromParams;
  uint8_t i = 0U;

  // compare data in eeprom with data in ram
  for (; i < size / 16U; i++)
  {
    initializeEepromDescriptor(&eepromParams, eepromOffset + i * 16U, readBuffer, 16U);
    if (0U != HAL_ReadEeprom(&eepromParams, NULL))
      return true;
    if (memcmp((uint8_t *)ramData + i * 16U, readBuffer, 16U))
      return true;
  }
  if (size % 16U)
  {
    initializeEepromDescriptor(&eepromParams, eepromOffset + i * 16U, readBuffer, size % 16U);
    if (0U != HAL_ReadEeprom(&eepromParams, NULL))
      return true;
    if (memcmp((uint8_t *)ramData + i * 16U, readBuffer, size % 16U))
      return true;
  }

  return false;
}

/******************************************************************************
\brief Reads data

\param[in] header  - pointer to item header structure;
\param[in] address - start address of read operation in eeprom;
\param[in] data    - item data;
\param[in] size    - size of data
\returns true if data is read correctly, false otherwise
******************************************************************************/
static bool readData(PdsSecureItemHeader_t *header, uint16_t address, void *data, uint16_t size)
{
  HAL_EepromParams_t eepromParams;
  uint16_t crc;

  if (MAGIC_NUMBER != header->magicNumber)
    return false;

  initializeEepromDescriptor(&eepromParams, address, data, size);
  N_ERRH_ASSERT_FATAL(0U == HAL_ReadEeprom(&eepromParams, NULL));

  crc = pdsCalculateCrc(size, data);
  if (crc != header->crc)
    return false;

  return true;
}

/******************************************************************************
\brief Looks for item id in idToOffset table

\param[in] id - item id
\returns position of item in table if found, -1 otherwise
******************************************************************************/
static int8_t getEepromItemPosition(S_Nv_ItemId_t id)
{
  ItemIdToEepromOffsetMapping_t mapItem;

  for (uint8_t  i = 0U; i < ARRAY_SIZE(pdsSecMemoryMap); i++)
  {
    memcpy_P(&mapItem, &pdsSecMemoryMap[i], sizeof(ItemIdToEepromOffsetMapping_t));
    if (mapItem.itemId == id)
      return i;
  }

  return -1;
}
#else
void PDS_EraseSecureItems(void)
{
// a Stub function if security items are stored in flash
}
#endif // #ifdef PDS_SECURITY_CONTROL_ENABLE
#endif // _ENABLE_PERSISTENT_SERVER_
#endif // PDS_ENABLE_WEAR_LEVELING == 1
// eof wlPdsSecurity.c
