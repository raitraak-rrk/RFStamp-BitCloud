/**************************************************************************//**
  \file S_Nv-ATmega.c

  \brief NV component for ATmega Platforms.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    03.10.12 A. Razinkov - created
******************************************************************************/

#if PDS_ENABLE_WEAR_LEVELING == 1
#ifdef _ENABLE_PERSISTENT_SERVER_
#ifndef PDS_USE_EXTERNAL_FLASH
#if defined(ATMEGA256RFR2) || defined(ATMEGA2564RFR2)
/***************************************************************************************************
* LOCAL INCLUDE FILES
***************************************************************************************************/

#include "S_Nv_Bindings.h"
#include "S_Nv_Init.h"
#include "S_Nv.h"
#include "pdsDataServer.h"

/***************************************************************************************************
* EXTERNAL INCLUDE FILES
***************************************************************************************************/

#include "D_Nv.h"
#include "N_ErrH.h"
#include "N_Log.h"
#include "N_Util.h"
#include "N_Task.h"
#include "N_Timer.h"
#include "N_Types.h"
#include "wlPdsMemIds.h"
#include <sysTimer.h>

//#include "S_Nv_Platform_Ids.h" // layering violation!

/***************************************************************************************************
* LOCAL MACROS AND CONSTANTS
***************************************************************************************************/

#if defined __IAR_SYSTEMS_ICC__
//#define S_XNV_LOGGING
#endif

#define COMPID "S_Nv"

#if !defined(MAX_ITEM_COUNT)
/** The maxumum number of items. */
#define MAX_ITEM_COUNT 128u
#endif

/** The maxumum length of an item. */
#define MAX_ITEM_LENGTH 1024u

/** Timer event used to erase a sector. */
#define EVENT_ERASE_SECTOR   0u
#define EVENT_COMPACT_SECTOR 1u
#define EVENT_COMPACT_ITEM   2u

/** Delay before erasing a sector. */
#define ERASE_SECTOR_DELAY_MS 7000u

/** Delay before performing a preemptive compact operation. */
#define COMPACT_SECTOR_DELAY_MS 10000u

/** Delay before performing a compact item operation. */
#define COMPACT_ITEM_DELAY_MS 3000u

/** Perform a compact item operation if the number of partial writes is larger than this. */
#define COMPACT_ITEM_THRESHOLD 100u

/** Perform a compact sector operation with a delay if the sector has less free space than this. */
#define PREEMPTIVE_COMPACT_SECTOR_THRESHOLD  (MAX_ITEM_LENGTH + BLOCK_HEADER_SIZE)

#define SECTOR_SIZE   D_NV_SECTOR_SIZE
#define FIRST_SECTOR  D_NV_FIRST_SECTOR
#define SECTOR_COUNT  D_NV_SECTOR_COUNT

/** The size of the sector header. */
#define SECTOR_HEADER_SIZE ((uint16_t) sizeof(SectorHeader_t))
/** The size of the block header. */
#define BLOCK_HEADER_SIZE ((uint16_t) sizeof(BlockHeader_t))

/** The sequence number to use for the initial sector. */
#define INITIAL_SECTOR_SEQUENCE_NUMBER 0xFFFFFFFEuL

/***************************************************************************************************
* LOCAL TYPES
***************************************************************************************************/

// make sure that the flash structures are packed
#if defined(__ICC8051__)
// the IAR compiler for 8051 does not supports "#pragma pack", but does not need it either...
#else
// ...assume that all other compilers do support it
#pragma pack(1)
#endif

/** 16 byte sector header used in flash located at the start of the active sector. */
typedef struct SectorHeader_t
{
    /** Is this sector active. Written with 0x0000 at the end of the compact operation. */
    uint16_t isActive;
    /** Signature to detect valid sectors. Must have the value "S_Nv2". */
    uint8_t signature[6];
    /** Counter, decreased each time a new sector becomes the active sector. */
    uint32_t sequenceNumber;
    /** Parity bits for the sequenceNumber field = sequenceNumber ^ 0xFFFFFFFFuL. */
    uint32_t sequenceParity;
} SectorHeader_t;

typedef struct BlockHeader_t
{
    /** Is this block active. Written with 0x0000 at the end of the write operation. */
    uint16_t isActive;
#if defined(ENABLE_S_NV_DATACRC_CHECK)
    /** CRC of the data. */
    uint8_t dataCrc;
    /** CRC of the header, not including the isActive and headerCrc fields. */
    uint8_t headerCrc;
#endif
    /** Item identifier for this block. 0x0000 for a meta block. */
    uint16_t id;
    /** Offset of this block within the item. */
    uint16_t blockOffset;
    /** Length of this block. */
    uint16_t blockLength;
    /** Length of the complete item. */
    uint16_t itemLength;
    /** Pointer to the previous block for this item. */
    uint16_t previousBlock;
    /** Number of partial writes to the item since the last complete write of the item. */
    uint16_t writeCount;
#if !defined(ENABLE_S_NV_DATACRC_CHECK)
    /** CRC of the header, not including the isActive and headerCrc fields. */
    uint16_t headerCrc;
#endif
} BlockHeader_t;

/** Structure used to store where to find an item. */
typedef struct Item_t
{
    /** Item identifier. */
    uint16_t id;
    /** Pointer to the last written block for this item. */
    uint16_t lastBlock;
} Item_t;

// back to the default packing
#if defined(__ICC8051__)
// the IAR compiler for 8051 does not supports "#pragma pack", but does not need it either...
#else
// ...assume that all other compilers do support it (only tested with visual studio)
#pragma pack()
#endif

/***************************************************************************************************
* LOCAL VARIABLES
***************************************************************************************************/

static const uint16_t s_persistentItems[] = { PERSISTENT_NV_ITEMS_PLATFORM, PERSISTENT_NV_ITEMS_APPLICATION };

/** The active sector. */
static uint8_t s_sector;
/** Location of the first unprogrammed byte in the active sector. */
static uint16_t s_sectorHead;

/** The number of read, and thus cached, items */
static uint8_t s_itemCount = 0u;
static Item_t s_itemCache[MAX_ITEM_COUNT];

/** The sector to erase in the EVENT_ERASE_SECTOR handler. */
static uint8_t s_sectorToErase = 0xFFu;

/** The item to perform a compact operation on and the length of this item. */
static uint16_t s_compactItemId = 0x0000u;
static uint16_t s_compactItemLength = 0x0000u;

/** Callback function called before changing flash contents. */
static S_Nv_PowerSupplyCheckingFunction_t s_powerSupplyCheckingFunction = NULL;

static HAL_AppTimer_t eraseSectorTimer;
static SYS_Timer_t compactSectorTimer;
static SYS_Timer_t compactItemTimer;

/** Check if the early init function is called already. */
static bool s_earlyInitDone = FALSE;

/***************************************************************************************************
* LOCAL FUNCTION DECLARATIONS
***************************************************************************************************/
static void eraseSectorTimerFired(void);
static void compactSectorTimerFired(void);
static void compactItemTimerFired(void);
static bool PowerSupplyTooLow(void);
static bool CompactSector(void);
static S_Nv_ReturnValue_t CompactItem(void);

/***************************************************************************************************
* LOCAL FUNCTIONS
***************************************************************************************************/
/** Erase sector timer callback.
*/
static void eraseSectorTimerFired(void)
{
    if (!PowerSupplyTooLow())
        D_Nv_EraseSector(s_sectorToErase);
}

/** Compact sector timer callback.
*/
static void compactSectorTimerFired(void)
{
    if (!PowerSupplyTooLow())
        if (!CompactSector())
            N_ERRH_FATAL();
}

/** Compact item timer callback.
*/
static void compactItemTimerFired(void)
{
    (void)CompactItem();
}

/** Check the power supply.
    \returns TRUE if the power supply is too low, FALSE when the the power
             supply is OK or when there is no callback installed.
*/
static bool PowerSupplyTooLow()
{
    bool result = FALSE;
    if ( s_powerSupplyCheckingFunction != NULL )
    {
        if ( !s_powerSupplyCheckingFunction() )
        {
            result = TRUE;
        }
    }
    return result;
}

static bool IsPersistent(uint16_t id)
{
    for ( uint8_t i = 0u; i < N_UTIL_ARRAY_SIZE(s_persistentItems); i++ )
    {
        if ( id == s_persistentItems[i] )
        {
            return TRUE;
        }
    }
    return FALSE;
}

/** Check if the RAM buffer contains all 0xFF values (flash erased)
    \param pData The buffer to check
    \param length The length of the buffer
    \returns TRUE if the buffer contains all 0xFF, FALSE otherwise
*/
static bool IsEmpty(uint8_t* pData, uint16_t length)
{
    for ( uint16_t i = 0u; i < length; i++ )
    {
        if ( pData[i] != 0xFFu )
        {
            return FALSE;
        }
    }

    return TRUE;
}

static void UpdateSectorHead(uint16_t increment)
{
    // make sure that the sector head is aligned to 16 bytes
    s_sectorHead = (s_sectorHead + increment + 0x000Fu) & 0xFFF0u;
}

/** Return a pointer to the cache for the item.
    \param id The id to find
    \returns A pointer to the cache for the ID, or NULL if it was not found
*/
static Item_t *FindItemCache(uint16_t id)
{
    for ( uint8_t cacheIndex = 0u; cacheIndex < s_itemCount; cacheIndex++ )
    {
        Item_t *cache = &s_itemCache[cacheIndex];
        if ( cache->id == id )
        {
            N_ERRH_ASSERT_FATAL(cache->lastBlock != 0x0000);
            return cache;
        }
    }
    return NULL;
}

/** Return a pointer to a cache for a new item.
    \param id The id to create cache for
    \returns A pointer to the cache for the ID
*/
static Item_t *CreateItemCache(uint16_t id)
{
    N_ERRH_ASSERT_FATAL(FindItemCache(id) == NULL);
    N_ERRH_ASSERT_FATAL(s_itemCount < MAX_ITEM_COUNT);

    Item_t *cache = &s_itemCache[s_itemCount++];
    cache->id = id;

    return cache;
}

/** Removes the specified item from the cache array.
    \param id The id to delete the cache for
*/
static void DeleteItemCache(uint16_t id)
{
    N_ERRH_ASSERT_FATAL(FindItemCache(id) != NULL);

    // Overwrite specified item cache with the last one
    Item_t *cache = FindItemCache(id);
    *cache = s_itemCache[--s_itemCount];
}

/** Return a pointer to the last written block for the item.
    \param id The id to find
    \returns A pointer to the last written block, or 0x0000u if the item was not found
*/
static uint16_t FindItem(uint16_t id)
{
    Item_t *cache = FindItemCache(id);

    if (cache)
    {
        return cache->lastBlock;
    }

    // item not found
    return 0x0000u;
}

#if defined(ENABLE_S_NV_DATACRC_CHECK)
static uint8_t ComputeCrc(uint8_t* pData, uint16_t length, uint8_t crc)
{

    for ( /* empty */ ; length != 0u; length-- )
    {
        uint8_t data;
        if ( pData == NULL )
          data = 0xFF;
        else
          data = *pData;
        crc = crc ^ data;

        if ( pData != NULL )
          pData++;
    }

    return crc;
}

static uint8_t ComputeHeaderCrc(BlockHeader_t* pBlockHeader)
{
    return ComputeCrc(((uint8_t*) pBlockHeader) + 4u, 12u, 0xFF); // skip isActive, dataCrc and headerCrc
}
#else
static uint16_t ComputeCrc(uint8_t* pData, uint16_t length)
{
    uint16_t crc = 0xFFFFu;

    for ( /* empty */ ; length != 0u; length-- )
    {
        uint8_t x;
        x = (uint8_t) ((crc >> 8) ^ *pData);
        x = x ^ (x >> 4);
        crc = (crc << 8) ^ ((uint16_t) x << 12) ^ ((uint16_t) x << 5) ^ (uint16_t) x;

        pData++;
    }

    return crc;
}

static uint16_t ComputeHeaderCrc(BlockHeader_t* pBlockHeader)
{
    return ComputeCrc(((uint8_t*) pBlockHeader) + 2u, 12u); // skip isActive and headerCrc
}
#endif

static bool WriteAndCheck(uint16_t offset, uint8_t* pData, uint16_t length)
{
    D_Nv_Write(s_sector, offset, pData, length);
    return D_Nv_IsEqual(s_sector, offset, pData, length);
}

static bool WriteBlockHeader(BlockHeader_t* pBlockHeader)
{
    pBlockHeader->isActive = 0xFFFFu;
#if defined(ENABLE_S_NV_DATACRC_CHECK)
    pBlockHeader->dataCrc = 0xFFu;
#endif
    pBlockHeader->headerCrc = ComputeHeaderCrc(pBlockHeader);

    if ( !WriteAndCheck(s_sectorHead, (uint8_t*) pBlockHeader, BLOCK_HEADER_SIZE) )
    {
        // failed to write header to flash.
        // check if any bits were programmed
        if ( !D_Nv_IsEmpty(s_sector, s_sectorHead, BLOCK_HEADER_SIZE) )
        {
            // the next write should skip this failed header as some bits have been programmed
            UpdateSectorHead(BLOCK_HEADER_SIZE);
            return FALSE;
        }
        else
        {
            // no bits were programmed (everything still is 0xFF).
            // the init function sees this as the start of the empty space
            // in the sector, so the next write should should attempt to
            // write to the same address
            return FALSE;
        }
    }
    else
    {
        UpdateSectorHead(BLOCK_HEADER_SIZE);
        return TRUE;
    }
}

#if defined(ENABLE_S_NV_DATACRC_CHECK)
static bool ActivateBlock(uint16_t blockPointer, uint8_t dataCrc)
{
    uint8_t crcIsActive[3] = {0,0,0};
    crcIsActive[2] = dataCrc;  // 2Bytes activataion data (0x0000) + 1Byte data CRC
    if ( !WriteAndCheck(blockPointer, (uint8_t*) &crcIsActive, sizeof(crcIsActive)) )
    {
        // failed to activate the block
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}
#else
static bool ActivateBlock(uint16_t blockPointer)
{
    uint16_t isActive = 0x0000u;
    if ( !WriteAndCheck(blockPointer, (uint8_t*) &isActive, sizeof(uint16_t)) )
    {
        // failed to activate the block
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}
#endif

static bool WriteDataBlock(BlockHeader_t* pBlockHeader, uint8_t* pData)
{
    uint16_t blockPointer = s_sectorHead;

    if ( !WriteBlockHeader(pBlockHeader) )
    {
        return FALSE;
    }

    if ( pData != NULL )
    {
        if ( !WriteAndCheck(s_sectorHead, pData, pBlockHeader->blockLength) )
        {
            // failed to write data to flash.
            // skip the invalid data for the next write (even if everything still is 0xFF)
            UpdateSectorHead(pBlockHeader->blockLength);
            return FALSE;
        }
    }
    UpdateSectorHead(pBlockHeader->blockLength);

#if defined(ENABLE_S_NV_DATACRC_CHECK)
    uint8_t dataCrc;
    dataCrc = (uint8_t) ComputeCrc(pData,pBlockHeader->blockLength,0xFF);

    if ( !ActivateBlock(blockPointer,dataCrc) )
#else
    if ( !ActivateBlock(blockPointer) )
#endif
    {
        // failed to activate the block
        return FALSE;
    }

    return TRUE;
}

/** Gather data from an item for a read or compact operation.
    \param sourceSector
    \param lastBlockPointer Pointer to the last block written for the item
    \param offset The start of the range of bytes to copy from the item
    \param length The size of the range of bytes to copy from the item
    \param pData Pointer to destination buffer in RAM for a read operation. NULL for a compact operation.

    This function is used by both the read and the compact operation.
     - For a read operation, the pBuffer parameter points to a buffer in RAM.
     - For a compact operation, pBuffer parameter is NULL and the data will
       be copied to the flash memory \ref s_sectorHead in sector \ref s_sector.

*/
#if defined(ENABLE_S_NV_DATACRC_CHECK)
static bool GatherData(uint8_t sourceSector, uint16_t lastBlockPointer, uint16_t offset, uint16_t length, void* pData, uint8_t *dataCrc)
#else
static bool GatherData(uint8_t sourceSector, uint16_t lastBlockPointer, uint16_t offset, uint16_t length, void* pData)
#endif
{
    // pointer to end of destination in RAM
    uint8_t* pDestination = NULL;
    // pointer to end of destination in flash
    uint16_t destinationPointer = 0u;

    // prepare the correct pointer
    if ( pData != NULL )
    {
        pDestination = (uint8_t*) pData + length;
    }
    else
    {
        destinationPointer = s_sectorHead + length;
    }

    BlockHeader_t blockHeader;
    uint16_t blockStart;
    uint16_t blockEnd;
    uint16_t count;

    // start with the last written block
    uint16_t currentBlockPointer = lastBlockPointer;

    // [readStart, readEnd> is the range of data that can be read during the current
    // pass over the blocks (the range includes readStart, but not readEnd).
    // it is initialized with the requested range
    uint16_t readStart = offset;
    uint16_t readEnd = offset + length;

    // continue until we have all the data that was requested
    while ( readStart != readEnd )
    {
        // find the block that contains the **last byte** that we want to read.
        for ( ;; )
        {
            if ( currentBlockPointer == 0x0000u )
            {
                // reached first block without finding the data.
                // either the flash is corrupt, or this is a read beyond the item length
                return FALSE;
            }

            // get the header of the current block
            D_Nv_Read(sourceSector, currentBlockPointer, (uint8_t*) &blockHeader, BLOCK_HEADER_SIZE);

            // [blockStart, blockEnd> is the range of bytes in this block
            blockStart = blockHeader.blockOffset;
            blockEnd = blockHeader.blockOffset + blockHeader.blockLength;

            if ( (readEnd <= blockStart) || (readStart >= blockEnd) )
            {
                // this block does not contain any bytes that we want to read.
                // continue with previous written block...
            }
            else if ( readEnd > blockEnd )
            {
                // this block contains some bytes that we want to read, but not the last byte.
                // adjust the read range to prevent reading an older version of these bytes
                readStart = blockEnd;
                // continue with previous written block...
            }
            else
            {
                // this block contains the last byte that we want to read.
                // read all data that we can from this block...
                break;
            }

            // ...continue with previous written block
            currentBlockPointer = blockHeader.previousBlock;
        }

        // ...read all data that we can from this block

        // pointer to the last byte that we want to read
        uint16_t sourcePointer = (currentBlockPointer + BLOCK_HEADER_SIZE) + (readEnd - blockStart);

        // check how many of the bytes that we want are in this block
        if ( readStart < blockStart )
        {
            // the block does not contain all data we want to read this pass
            count = readEnd - blockStart;

            // continue with the current pass after reading this block
            currentBlockPointer = blockHeader.previousBlock;
        }
        else
        {
            // the block contains all data we want to read this pass
            count = readEnd - readStart;

            // start with a new pass after reading this block (unless we have all requested data)
            currentBlockPointer = lastBlockPointer;
            readStart = offset;
        }
        readEnd -= count;

        // read the data that we want from the source sector
        if ( pDestination != NULL )
        {
            // this is for a read operation, so copy to buffer in RAM
            sourcePointer -= count;
            pDestination -= count;
            D_Nv_Read(s_sector, sourcePointer, pDestination, count);
        }
        else
        {
#if defined(ENABLE_S_NV_DATACRC_CHECK)
            uint8_t dataCrcTemp = 0xFF;
            if(dataCrc == NULL)
              return FALSE;
#endif
            // this is for a compact operation, so copy to the destination sector in flash
            while ( count > 0u )
            {
                uint8_t buffer[16];
                uint16_t c;
                if ( count > sizeof(buffer) )
                {
                    c = sizeof(buffer);
                    count -= (uint16_t) sizeof(buffer);
                }
                else
                {
                    c = count;
                    count = 0u;
                }

                destinationPointer -= c;
                sourcePointer -= c;
                D_Nv_Read(sourceSector, sourcePointer, buffer, c);

#if defined(ENABLE_S_NV_DATACRC_CHECK)
                dataCrcTemp = ComputeCrc(buffer,c, dataCrcTemp);
                *dataCrc = dataCrcTemp;
#endif
                if ( !WriteAndCheck(destinationPointer, buffer, c) )
                {
                    return FALSE;
                }
            }
        }
    }

    return TRUE;
}

static bool InitSector(uint32_t sequenceNumber)
{
    // make sure the sector is completely erased
    if ( !D_Nv_IsEmpty(s_sector, 0u, SECTOR_SIZE) )
    {
        D_Nv_EraseSector(s_sector);
        // check if the erase succeeded
        if ( !D_Nv_IsEmpty(s_sector, 0u, SECTOR_SIZE) )
        {
            return FALSE;
        }
    }

    // write sector header
    SectorHeader_t sectorHeader;
    sectorHeader.isActive = 0xFFFFu;
    sectorHeader.signature[0] = (uint8_t) 'A';
    sectorHeader.signature[1] = (uint8_t) 'T';
    sectorHeader.signature[2] = (uint8_t) 'S';
    sectorHeader.signature[3] = (uint8_t) 'N';
    sectorHeader.signature[4] = (uint8_t) 'v';
    sectorHeader.signature[5] = (uint8_t) '1';
    sectorHeader.sequenceNumber = sequenceNumber;
    sectorHeader.sequenceParity = sequenceNumber ^ 0xFFFFFFFFuL;
    if ( !WriteAndCheck(0u, (uint8_t*) &sectorHeader, SECTOR_HEADER_SIZE) )
    {
        return FALSE;
    }

    s_sectorHead = SECTOR_HEADER_SIZE;

    return TRUE;
}

static bool ActivateSector(void)
{
    // activate sector header
    uint16_t isActive = 0x0000u;
    if ( !WriteAndCheck(0u, (uint8_t*) &isActive, sizeof(uint16_t)) )
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

static void LoadSector(void)
{
    // start after the sector header
    s_sectorHead = SECTOR_HEADER_SIZE;

    // Done when sectorhead reaches end of sector
    while ( s_sectorHead < SECTOR_SIZE )
    {
        BlockHeader_t blockHeader;
        D_Nv_Read(s_sector, s_sectorHead, (uint8_t*) &blockHeader, BLOCK_HEADER_SIZE);

        if ( IsEmpty((uint8_t*) &blockHeader, BLOCK_HEADER_SIZE) )
        {
            // no header. done
            break;
        }
        else if ( blockHeader.headerCrc != ComputeHeaderCrc(&blockHeader) )
        {
            // invalid header. skip header
            UpdateSectorHead(BLOCK_HEADER_SIZE);
        }
        else if ( blockHeader.isActive != 0x0000u )
        {
            // inactive header. skip header and data
            UpdateSectorHead(BLOCK_HEADER_SIZE + blockHeader.blockLength);
        }
        else
        {
#if defined(ENABLE_S_NV_DATACRC_CHECK)
          uint8_t readData[16];
          uint16_t dataLength,lengthRem,offset;
          uint8_t crc=0xFF;
          lengthRem = blockHeader.blockLength;
          offset = s_sectorHead+BLOCK_HEADER_SIZE;
          for(int8_t i= (lengthRem/sizeof(readData)); i>=0; i--)
          {
            if(i)
            {
              dataLength = sizeof(readData);
              lengthRem = lengthRem - sizeof(readData);
            }
            else
            {
              dataLength = lengthRem;
            }
           if(dataLength == 0)
             break;            
            D_Nv_Read(s_sector, offset, (uint8_t*) &readData, dataLength);
            offset = offset+dataLength;
            crc = ComputeCrc(((uint8_t*) &readData), dataLength, crc);
          }

          if( crc != blockHeader.dataCrc)
          {
            // inactive header. skip header and data
            UpdateSectorHead(BLOCK_HEADER_SIZE + blockHeader.blockLength);
          }
          else
          {
#endif
            // Try to find cache for current ID -- if none found, it's a new item!
            uint16_t id = blockHeader.id;
            Item_t *cache = FindItemCache(id);

            if (cache == NULL)
            {
                cache = CreateItemCache(id);
            }

            cache->lastBlock = s_sectorHead;

            // If item length is zero, the item had been deleted -- remove the cache
            if ( blockHeader.itemLength == 0u )
            {
                DeleteItemCache(id);
            }

            UpdateSectorHead(BLOCK_HEADER_SIZE + blockHeader.blockLength);
#if defined(ENABLE_S_NV_DATACRC_CHECK)
          }
#endif
        }
    }
}

/* Important: if CompactSector fails, the only fix is to reinitialize!
 * This is because the itemCache, sector head and sector selector will
 * be messed up.
 */
static bool CompactSector(void)
{
#if defined(ENABLE_NV_COMPACT_LOGGING)
    N_LOG_ALWAYS(("CompactSector(s=%hu)", s_sector));
#endif
    // stop timer for preemptive compact sector as this will not be needed any more
    SYS_StopTimer(&compactSectorTimer);

    // stop the compact item as a compact sector will compact all items
    s_compactItemId = 0u;
    s_compactItemLength = 0u;

    uint8_t sourceSector = s_sector;

    // get the sector header for the source sector
    SectorHeader_t sectorHeader;
    D_Nv_Read(sourceSector, 0u, (uint8_t*) &sectorHeader, SECTOR_HEADER_SIZE);

    uint32_t nextSequenceNumber = sectorHeader.sequenceNumber - 1uL;

    // find and prepare a destination sector
    do
    {
        s_sector++;
        if ( s_sector >= (FIRST_SECTOR + SECTOR_COUNT) )
        {
            s_sector = FIRST_SECTOR;
        }

        if ( s_sector == sourceSector )
        {
            // all sector failed to initialize
            N_ERRH_FATAL();
        }
    }
    while ( !InitSector(nextSequenceNumber) );

    // Traverse the item cache, writing all items to the new sector
    for (uint8_t cacheIndex = 0; cacheIndex < s_itemCount; cacheIndex++) {
        Item_t *cache = &s_itemCache[cacheIndex];

        // Start by reading out header of last block
        BlockHeader_t blockHeader;
        D_Nv_Read(sourceSector, cache->lastBlock, (uint8_t*) &blockHeader, BLOCK_HEADER_SIZE);

        // Construct header for a single block with contiguous data
        blockHeader.blockOffset = 0x0000u;
        blockHeader.blockLength = blockHeader.itemLength;
        blockHeader.previousBlock = 0x0000u;
        blockHeader.writeCount = 0u;

        uint16_t blockPointer = s_sectorHead;
#if defined(ENABLE_S_NV_DATACRC_CHECK)
        uint8_t dataCrc;
#endif

        // Now write the data block header, move its data and activate
        if ( !WriteBlockHeader(&blockHeader) )
        {
            return FALSE;
        }

#if defined(ENABLE_S_NV_DATACRC_CHECK)

        if ( !GatherData(sourceSector, cache->lastBlock, 0u, blockHeader.itemLength, NULL, &dataCrc) )
#else
        if ( !GatherData(sourceSector, cache->lastBlock, 0u, blockHeader.itemLength, NULL) )
#endif
        {
            return FALSE;
        }

        UpdateSectorHead(blockHeader.blockLength);

#if defined(ENABLE_S_NV_DATACRC_CHECK)
        if ( !ActivateBlock(blockPointer,dataCrc) )
#else
        if ( !ActivateBlock(blockPointer) )
#endif
        {
            return FALSE;
        }

        cache->lastBlock = blockPointer;
    }

    // All items moved, so now we just need to activate the sector
    if ( !ActivateSector() )
    {
        return FALSE;
    }

    // schedule an erase of the source sector,Restart the timer if it is already running.
    s_sectorToErase = sourceSector;
    HAL_StopAppTimer(&eraseSectorTimer);
    HAL_StartAppTimer(&eraseSectorTimer);

    return TRUE;
}

static void CompactSectorIfNeeded(uint16_t immediateThreshold)
{
    uint16_t freeSpace = SECTOR_SIZE - s_sectorHead;

    if ( freeSpace < immediateThreshold )
    {
        if ( !CompactSector() )
        {
            N_ERRH_FATAL();
        }
        return;
    }
    if ( freeSpace < PREEMPTIVE_COMPACT_SECTOR_THRESHOLD )
    {
        if (SYS_TIMER_STOPPED == compactSectorTimer.state)
            SYS_InitTimer(&compactSectorTimer, TIMER_ONE_SHOT_MODE, COMPACT_SECTOR_DELAY_MS, compactSectorTimerFired);
    }
}

static S_Nv_ReturnValue_t CompactItem(void)
{
#if defined(ENABLE_NV_COMPACT_LOGGING)
    N_LOG_ALWAYS(("CompactItem(id=%Xh)", s_compactItemId));
#endif

    if ( PowerSupplyTooLow() )
    {
        return S_Nv_ReturnValue_PowerSupplyTooLow;
    }

    if ( (s_compactItemLength == 0u) && ( s_compactItemId == 0u ) )
    {
        // compact sector was performed since the compact item was
        // scheduled, so there is no need for another compact item unless
        // a resize of an item is needed.
        return S_Nv_ReturnValue_Ok;
    }

    if (s_compactItemLength != 0)
        CompactSectorIfNeeded(s_compactItemLength + BLOCK_HEADER_SIZE);

    Item_t *cache = FindItemCache(s_compactItemId);
    if ( cache == NULL )
    {
        // trying to compact a non-existing item (item may have been deleted)
        return S_Nv_ReturnValue_Ok;
    }

    uint16_t blockPointer = cache->lastBlock;

    BlockHeader_t blockHeader;
    // read last written item block header
    D_Nv_Read(s_sector, blockPointer, (uint8_t*) &blockHeader, BLOCK_HEADER_SIZE);

    if (s_compactItemLength == 0)
    {
        CompactSectorIfNeeded(blockHeader.itemLength + BLOCK_HEADER_SIZE);

        cache = FindItemCache(s_compactItemId);
        N_ERRH_ASSERT_FATAL(cache != NULL);
        blockPointer = cache->lastBlock;
        // read last written item block header
        D_Nv_Read(s_sector, blockPointer, (uint8_t*) &blockHeader, BLOCK_HEADER_SIZE);
    }

    uint16_t lastBlock = s_sectorHead;

    // write the block header to the destination sector. all data will be merged into one block
    uint16_t bytesToGather = blockHeader.itemLength;
    if ( s_compactItemLength != 0u )
    {
        N_LOG_ALWAYS(("Resizing NV item (id=%hu) from %hu to %hu", s_compactItemId, blockHeader.itemLength, s_compactItemLength));

        // Change the length of the item
        blockHeader.itemLength = s_compactItemLength;

        if ( bytesToGather > blockHeader.itemLength )
        {
            // Item will be truncated as the new length is smaller than the original length.
            bytesToGather = blockHeader.itemLength;
        }
    }

    blockHeader.blockOffset = 0u;
    blockHeader.blockLength = blockHeader.itemLength;
    blockHeader.previousBlock = 0x0000u;
    blockHeader.writeCount = 0u;
    if ( !WriteBlockHeader(&blockHeader) )
    {
        N_LOG_NONFATAL();
        return S_Nv_ReturnValue_Failure;
    }

#if defined(ENABLE_S_NV_DATACRC_CHECK)
    uint8_t dataCrc;
    // gather all data of the item and copy it to a new block
    if ( !GatherData(s_sector, blockPointer, 0u, bytesToGather, NULL, &dataCrc) )
#else
    if ( !GatherData(s_sector, blockPointer, 0u, bytesToGather, NULL) )
#endif
    {
        N_LOG_NONFATAL();
        return S_Nv_ReturnValue_Failure;
    }
    UpdateSectorHead(blockHeader.blockLength);

#if defined(ENABLE_S_NV_DATACRC_CHECK)
    // activate the block
    if ( !ActivateBlock(lastBlock, dataCrc) )
#else
    if ( !ActivateBlock(lastBlock) )
#endif
    {
        N_LOG_NONFATAL();
        return S_Nv_ReturnValue_Failure;
    }

    s_compactItemId = 0u;
    s_compactItemLength = 0u;

    cache->lastBlock = lastBlock;

    return S_Nv_ReturnValue_Ok;
}

/***************************************************************************************************
* EXPORTED FUNCTIONS
***************************************************************************************************/

void S_Nv_EarlyInit(void)
{
    s_itemCount = 0u;

    SectorHeader_t sectorHeader;

    uint8_t lastSector = 0xFFu;
    uint32_t lastSectorSequence = 0xFFFFFFFFuL;

    for ( uint8_t sector = FIRST_SECTOR; sector < (FIRST_SECTOR + SECTOR_COUNT); sector++ )
    {
        D_Nv_Read(sector, 0u, (uint8_t*) &sectorHeader, SECTOR_HEADER_SIZE);
        if ( (sectorHeader.isActive == 0x0000u) &&
             (sectorHeader.signature[0] == (uint8_t) 'A') &&
             (sectorHeader.signature[1] == (uint8_t) 'T') &&
             (sectorHeader.signature[2] == (uint8_t) 'S') &&
             (sectorHeader.signature[3] == (uint8_t) 'N') &&
             (sectorHeader.signature[4] == (uint8_t) 'v') &&
             (sectorHeader.signature[5] == (uint8_t) '1') &&
             ((sectorHeader.sequenceNumber ^ sectorHeader.sequenceParity) == 0xFFFFFFFFuL) )
        {
            // active sector
            if ( sectorHeader.sequenceNumber < lastSectorSequence )
            {
                lastSector = sector;
                lastSectorSequence = sectorHeader.sequenceNumber;
            }
        }
    }

    if ( lastSector == 0xFFu )
    {
        // no active sector. initialize one
        s_sector = FIRST_SECTOR;

        for ( ;; )
        {
            if ( InitSector(INITIAL_SECTOR_SEQUENCE_NUMBER) )
            {
                if ( ActivateSector() )
                {
                    break;
                }
            }

            s_sector++;
            if ( s_sector >= (FIRST_SECTOR + SECTOR_COUNT) )
            {
                // all sectors failed to initialize
                N_ERRH_FATAL();
            }
        }

        s_itemCount = 0u;
    }
    else
    {
        // load active sector
        s_sector = lastSector;

        LoadSector();
    }
    s_earlyInitDone = TRUE;
}

void S_Nv_Init(void)
{
    if (!s_earlyInitDone)
    {
        S_Nv_EarlyInit();
    }

    eraseSectorTimer.mode     = TIMER_ONE_SHOT_MODE;
    eraseSectorTimer.callback = eraseSectorTimerFired;
    eraseSectorTimer.interval = ERASE_SECTOR_DELAY_MS;
}

/** Interface function, see \ref S_Nv_ItemInit. */
#if defined(S_XNV_LOGGING)
static S_Nv_ReturnValue_t S_Nv_ItemInit_Original(S_Nv_ItemId_t id, uint16_t itemLength, void* pDefaultData)
#else
S_Nv_ReturnValue_t S_Nv_ItemInit_Impl(S_Nv_ItemId_t id, uint16_t itemLength, void* pDefaultData)
#endif
{
    N_ERRH_ASSERT_FATAL((id != 0u) && (id < 0xF000u) && (itemLength <= MAX_ITEM_LENGTH));

    if ( FindItem(id) != 0x0000u )
    {
#if (!defined(DISABLE_NV_RESIZE))
        // Item is already initialized. Check if the item length has been changed.
        uint16_t oldItemLength = S_Nv_ItemLength(id);
        if ( oldItemLength != itemLength )
        {
            // Resize this item to the new length
            s_compactItemId = id;
            s_compactItemLength = itemLength;

            S_Nv_ReturnValue_t resizeResult = CompactItem();
            if ( resizeResult != S_Nv_ReturnValue_Ok )
            {
                return resizeResult;
            }
        }
#endif
        if (pDefaultData != NULL)
        {
            return S_Nv_Read(id, 0u, itemLength, pDefaultData);
        }
        else
        {
            return S_Nv_ReturnValue_Ok;
        }
    }

    if ( PowerSupplyTooLow() )
    {
        return S_Nv_ReturnValue_PowerSupplyTooLow;
    }

    CompactSectorIfNeeded(BLOCK_HEADER_SIZE + itemLength);

    BlockHeader_t blockHeader;

    // need to add a new item. check if possible
    N_ERRH_ASSERT_FATAL(s_itemCount < MAX_ITEM_COUNT);

    uint16_t newItemId = id;
    uint16_t newItemPointer = s_sectorHead;

    blockHeader.id = newItemId;
    blockHeader.blockOffset = 0x0000u;
    blockHeader.blockLength = itemLength;
    blockHeader.itemLength = itemLength;
    blockHeader.previousBlock = 0x0000u;
    blockHeader.writeCount = 0u;
    if ( !WriteDataBlock(&blockHeader, (uint8_t*) pDefaultData) )
    {
        return S_Nv_ReturnValue_Failure;
    }

    // After successful write, create the cache
    Item_t *newItemCache = CreateItemCache(newItemId);
    newItemCache->lastBlock = newItemPointer;

    return S_Nv_ReturnValue_DidNotExist;
}

/** Interface function, see \ref S_Nv_Write. */
#if defined(S_XNV_LOGGING)
static S_Nv_ReturnValue_t S_Nv_Write_Original(S_Nv_ItemId_t id, uint16_t offset, uint16_t dataLength, void* pData)
#else
S_Nv_ReturnValue_t S_Nv_Write_Impl(S_Nv_ItemId_t id, uint16_t offset, uint16_t dataLength, void* pData)
#endif
{
    N_ERRH_ASSERT_FATAL((id != 0u) && (pData != NULL));

    if ( PowerSupplyTooLow() )
    {
        return S_Nv_ReturnValue_PowerSupplyTooLow;
    }

    CompactSectorIfNeeded(BLOCK_HEADER_SIZE + dataLength);

    Item_t *cache = FindItemCache(id);

    // Caches will not exist for deleted or uninitialized items
    if ( cache == NULL )
    {
        return S_Nv_ReturnValue_DoesNotExist;
    }

    uint16_t blockPointer = cache->lastBlock;

    BlockHeader_t blockHeader;

    // read last written item block header
    D_Nv_Read(s_sector, blockPointer, (uint8_t*) &blockHeader, BLOCK_HEADER_SIZE);

    // check that we do not write beyond the length of the item
    if ( ((uint32_t) offset + (uint32_t) dataLength) > (uint32_t) blockHeader.itemLength )
    {
        return S_Nv_ReturnValue_BeyondEnd;
    }

    // write item block
    blockHeader.blockOffset = offset;
    blockHeader.blockLength = dataLength;
    blockHeader.previousBlock = blockPointer;
    if ( blockHeader.blockLength == blockHeader.itemLength )
    {
        // complete write. reset counter
        blockHeader.writeCount = 0u;
    }
    else
    {
        // partial write. increment counter
        blockHeader.writeCount++;
    }

    // Need current sector head to update cache after write
    uint16_t newBlockPointer = s_sectorHead;

    if ( !WriteDataBlock(&blockHeader, (uint8_t*) pData) )
    {
        return S_Nv_ReturnValue_Failure;
    }

    // Write succeeded, so update the cache
    cache->lastBlock = newBlockPointer;

    if ( blockHeader.writeCount > COMPACT_ITEM_THRESHOLD )
    {
        // schedule a compact item operation for this item.
        // this reads the complete item and writes a new block containing
        // the complete item. this limits the worst case read and compact
        // sector time.
        s_compactItemId = blockHeader.id;
        s_compactItemLength = 0u;           // no need to resize this item here

        // ignore the error if a timer cannot be started: the operation
        // is not required for a correct operation of the component
        if (SYS_TIMER_STOPPED == compactItemTimer.state)
            SYS_InitTimer(&compactItemTimer, TIMER_ONE_SHOT_MODE, COMPACT_ITEM_DELAY_MS, compactItemTimerFired);
    }

    return S_Nv_ReturnValue_Ok;
}

/** Interface function, see \ref S_Nv_Read. */
#if defined(S_XNV_LOGGING)
static S_Nv_ReturnValue_t S_Nv_Read_Original(S_Nv_ItemId_t id, uint16_t offset, uint16_t dataLength, void* pData)
#else
S_Nv_ReturnValue_t S_Nv_Read_Impl(S_Nv_ItemId_t id, uint16_t offset, uint16_t dataLength, void* pData)
#endif
{
    N_ERRH_ASSERT_FATAL((id != 0u) && (pData != NULL));

    // get the pointer to the last written block for the item
    uint16_t lastBlockPointer = FindItem(id);
    if ( lastBlockPointer == 0x0000u )
    {
        // item does not exist
        return S_Nv_ReturnValue_DoesNotExist;
    }

    // prevent overflow when calculating the range of bytes to read
    if ( ((uint32_t) offset + (uint32_t) dataLength) >= 0x10000uL )
    {
        return S_Nv_ReturnValue_BeyondEnd;
    }

    // gather the data into the destination buffer
#if defined(ENABLE_S_NV_DATACRC_CHECK)
    if ( !GatherData(s_sector, lastBlockPointer, offset, dataLength, pData, NULL) )
#else
    if ( !GatherData(s_sector, lastBlockPointer, offset, dataLength, pData) )
#endif
    {
        return S_Nv_ReturnValue_BeyondEnd;
    }

    return S_Nv_ReturnValue_Ok;
}

/** Interface function, see \ref S_Nv_ItemLength. */
uint16_t S_Nv_ItemLength_Impl(S_Nv_ItemId_t id)
{
    N_ERRH_ASSERT_FATAL(id != 0u);

    uint16_t blockPointer = FindItem(id);
    if ( blockPointer == 0x0000u )
    {
        // item does not exist
        return 0u;
    }

    // read last written item block header
    BlockHeader_t blockHeader;
    D_Nv_Read(s_sector, blockPointer, (uint8_t*) &blockHeader, BLOCK_HEADER_SIZE);
    return blockHeader.itemLength;
}

/** Interface function, see \ref S_Nv_Delete. */
S_Nv_ReturnValue_t S_Nv_Delete_Impl(S_Nv_ItemId_t id)
{
    N_ERRH_ASSERT_FATAL(id != 0u);

    if ( FindItem(id) == 0x0000u )
    {
        return S_Nv_ReturnValue_DoesNotExist;
    }

    if ( PowerSupplyTooLow() )
    {
        return S_Nv_ReturnValue_PowerSupplyTooLow;
    }

    CompactSectorIfNeeded(BLOCK_HEADER_SIZE);

    // Delete item by writing a new header for it, stating 0 length
    BlockHeader_t blockHeader;

    blockHeader.id = id;
    blockHeader.blockOffset = 0x0000u;
    blockHeader.blockLength = 0;
    blockHeader.itemLength = 0u;
    blockHeader.previousBlock = 0x0000u;
    blockHeader.writeCount = 0u;

    uint16_t blockPointer = s_sectorHead;

    if ( !WriteBlockHeader(&blockHeader) )
    {
        return S_Nv_ReturnValue_Failure;
    }

#if defined(ENABLE_S_NV_DATACRC_CHECK)
    if ( !ActivateBlock(blockPointer,0) )
#else
    if ( !ActivateBlock(blockPointer) )
#endif
    {
        // failed to activate the block
        return S_Nv_ReturnValue_Failure;
    }

    DeleteItemCache(id);

    return S_Nv_ReturnValue_Ok;
}

/** Interface function, see \ref S_Nv_EraseAll.
 *
 * Important: EraseAll leaves the file system in a non-working state if
 * persistent items are also deleted. This is because all sectors are
 * deleted without initializing a new one or even clearing the cache,
 * so reinitialization of the driver is needed.
 */
S_Nv_ReturnValue_t S_Nv_EraseAll_Impl(bool includingPersistentItems)
{
    PDS_EraseSecureItems();

    if ( PowerSupplyTooLow() )
    {
        return S_Nv_ReturnValue_PowerSupplyTooLow;
    }

    if ( includingPersistentItems )
    {
        for ( uint8_t sector = FIRST_SECTOR; sector < (FIRST_SECTOR + SECTOR_COUNT); sector++ )
        {
            D_Nv_EraseSector(sector);
        }
    }
    else
    {
        uint8_t deletedItems = 0;
        uint8_t totalItems = s_itemCount;

        // Traverse the item cache, removing all the non-persistent
        for ( uint8_t cacheIndex = 0; cacheIndex < totalItems; cacheIndex++ )
        {
            uint16_t id = s_itemCache[cacheIndex].id;

            if ( !IsPersistent(id) )
            {
                DeleteItemCache(id);
                deletedItems++;
            }
        }

        // Were any items deleted? If so, do sector compaction!
        if ( deletedItems > 0 )
        {
            if ( !CompactSector() )
            {
                N_ERRH_FATAL();
            }
        }
    }

    // Only way we end up here is if all went well.
    return S_Nv_ReturnValue_Ok;
}

/** Interface function, see \ref S_Nv_SetPowerSupplyCheckingFunction. */
void S_Nv_SetPowerSupplyCheckingFunction_Impl(S_Nv_PowerSupplyCheckingFunction_t pf)
{
    s_powerSupplyCheckingFunction = pf;
}
/** Interface function, see \ref S_Nv_IsItemAvailable.
 *
 * Important: This will check whether the item is found in the storage area
 */
bool S_Nv_IsItemAvailable_Impl(S_Nv_ItemId_t id)
{
  return ( FindItem(id) != 0x0000u );
}
#if defined(S_XNV_LOGGING)

S_Nv_ReturnValue_t S_Nv_ItemInit_Impl(S_Nv_ItemId_t id, uint16_t itemLength, void* pDefaultData)
{
    //hal_flash_readCount = 0u;
    //hal_flash_readSize = 0u;
    //hal_flash_writeCount = 0u;
    //hal_flash_writeSize = 0u;
    uint32_t stopwatch = N_Util_TimerStart();

    S_Nv_ReturnValue_t ret = S_Nv_ItemInit_Original(id, itemLength, pDefaultData);

    N_LOG_ALWAYS(("item_init(id=%Xh, l=%u, t=%lu, r=%u:%u, w=%u:%lu, h=%u): %hu",
        id, itemLength,
        N_Util_TimerElapsed(stopwatch),
//        hal_flash_readCount, hal_flash_readSize, hal_flash_writeCount, 4uL * hal_flash_writeSize,
        0u, 0u, 0u, 0uL,
        s_sectorHead,
        ret));

    return ret;
}

S_Nv_ReturnValue_t S_Nv_Write_Impl(S_Nv_ItemId_t id, uint16_t offset, uint16_t dataLength, void* pData)
{
    //hal_flash_readCount = 0u;
    //hal_flash_readSize = 0u;
    //hal_flash_writeCount = 0u;
    //hal_flash_writeSize = 0u;
    uint32_t stopwatch = N_Util_TimerStart();

    S_Nv_ReturnValue_t ret = S_Nv_Write_Original(id, offset, dataLength, pData);

    N_LOG_ALWAYS(("write(id=%Xh, o=%u, l=%u, t=%lu, r=%u:%u, w=%u:%lu, h=%u): %hu",
        id, offset, dataLength,
        N_Util_TimerElapsed(stopwatch),
//        hal_flash_readCount, hal_flash_readSize, hal_flash_writeCount, 4uL * hal_flash_writeSize,
        0u, 0u, 0u, 0uL,
        s_sectorHead,
        ret));

    return ret;
}

S_Nv_ReturnValue_t S_Nv_Read_Impl(S_Nv_ItemId_t id, uint16_t offset, uint16_t dataLength, void* pData)
{
    //hal_flash_readCount = 0u;
    //hal_flash_readSize = 0u;
    //hal_flash_writeCount = 0u;
    //hal_flash_writeSize = 0u;
    uint32_t stopwatch = N_Util_TimerStart();

    S_Nv_ReturnValue_t ret = S_Nv_Read_Original(id, offset, dataLength, pData);

    N_LOG_ALWAYS(("read(id=%Xh, o=%u, l=%u, t=%lu, r=%u:%u): %hu",
        id, offset, dataLength,
        N_Util_TimerElapsed(stopwatch),
//        hal_flash_readCount, hal_flash_readSize,
        0u, 0u,
        ret));

    return ret;
}

#endif
#endif //#if defined(ATMEGA256RFR2) || defined(ATMEGA2564RFR2)
#endif //#ifndef PDS_USE_EXTERNAL_FLASH
#endif // _ENABLE_PERSISTENT_SERVER_
#endif // #if PDS_ENABLE_WEAR_LEVELING == 1
