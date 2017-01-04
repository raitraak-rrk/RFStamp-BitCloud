/***************************************************************************//**
  \file pdsMemAccess.h

  \brief Interface to provide memory access for Persistence Data Server.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal

  History:
    2008-01-22 A. Khromykh - Created
    2012-03-15 A. Razinkov - Refactored and renamed
  Last change:
    $Id$
*****************************************************************************/

#ifndef _PDSMEMACCESS_H
#define _PDSMEMACCESS_H

/******************************************************************************
                   Includes section
******************************************************************************/
#include <stdPdsMemAbstract.h>
#include <pdsDataServer.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
/* Available non-volatile memory offset */
#define SYSTEM_BASE_STORAGE_ADDRESS    0x0004
/* CRC area size in bytes */
#define PDS_CRC_AREA_SIZE              10U
/* User area offset */
#ifdef _ENABLE_PERSISTENT_SERVER_
#define USER_BASE_STORAGE_ADDRESS      (SYSTEM_BASE_STORAGE_ADDRESS + \
                                        pdsMemory()->ffSize * sizeof(PDS_FileOffsetInfo_t) + \
                                        pdsMemory()->totalMemorySize)
#else
#define USER_BASE_STORAGE_ADDRESS      SYSTEM_BASE_STORAGE_ADDRESS
#endif /* _ENABLE_PERSISTENT_SERVER_ */

/* Non-volatile memory transaction status */
#define STORAGE_BUSY                  -2
#define STORAGE_ERROR                 -1

/* Deprecated definitions */
#define EEPROM_BUSY                STORAGE_BUSY
#define EEPROM_ERROR               STORAGE_ERROR
#define SYSTEM_BASE_EEPROM_ADDRESS SYSTEM_BASE_STORAGE_ADDRESS
#define USER_BASE_EEPROM_ADDRESS   USER_BASE_STORAGE_ADDRESS

/* Length in bytes of all existing files bitmask */
#define PDS_MEM_MASK_LENGTH   (PDS_MEM_IDS_AMOUNT/8U + ((PDS_MEM_IDS_AMOUNT%8U) ? 1U : 0U))

#define PDS_MEM_MASK_CLEAR_ALL(mask)        memset(mask, 0, PDS_MEM_MASK_LENGTH)
#define PDS_MEM_MASK_SET_ALL(mask)          memset(mask, 0xFF, PDS_MEM_MASK_LENGTH)
#define PDS_MEM_MASK_SET_BIT(mask, bitN)    {mask[bitN/8] |= 1U << bitN%8U;}
#define PDS_MEM_MASK_CLEAR_BIT(mask, bitN)  {mask[bitN/8] &= ~(1U << bitN%8U);}
#define PDS_MEM_MASK_IS_BIT_SET(mask, bitN) (mask[bitN/8] & (1U << bitN%8U))

/******************************************************************************
                   Types section
******************************************************************************/
/* Non-volatile memory mask; bit index corresponds to file or directory unique identifier */
typedef uint8_t PDS_MemMask_t[PDS_MEM_MASK_LENGTH];

/* Context to access to stored value in non-volatile memory */
typedef struct _PdsDataAccessContext_t
{
  /* Relevant file unique id mask. */
  PDS_MemMask_t memoryMask;

  /* Currently processed memory identifier */
  PDS_MemId_t memoryId;
} PdsDataAccessContext_t;

/******************************************************************************
                   Prototypes section
******************************************************************************/
/******************************************************************************
\brief Obtains appropriate file descriptor record by specified memory identifier.

\param[in] memoryId - a unique file identifier.

\return Appropriate file descriptor record or NULL, if no one exists.
******************************************************************************/
PDS_FileDescrRec_t pdsGetFileDescrRec(PDS_MemId_t memoryId);

/******************************************************************************
\brief Obtains appropriate directory descriptor record by specified memory identifier.

\param[in] memoryId - a unique directory identifier.

\return Appropriate directory descriptor record or NULL, if no one exists.
******************************************************************************/
PDS_DirDescrRec_t pdsGetDirectoryDescrRec(PDS_MemId_t memoryId);

/******************************************************************************
\brief Calculates file offset within non-volatile memory.

\param[in] memoryId - a unique file identifier.
\param[out] fileOffset - an offset in non-volatile memory from which the
                         file stored.
******************************************************************************/
void pdsGetFileOffset(PDS_MemId_t memoryId, uint16_t *fileOffset);

/******************************************************************************
\brief Prepares a context for access to data stored in non-volatile memory.
\
\param[out] accessContext - access context pointer.
\param[in] memoryMask - bitmask of non-volatile memory contents to which access
                        is needed.
******************************************************************************/
void pdsPrepareAccessContext(PdsDataAccessContext_t *accessContext, const uint8_t *const memoryMask);

/******************************************************************************
\brief Locates first file by specified access context and prepares memory
       descriptor to store the file's data in non-volatile memory.
       Located file is deleted from a list in the access context structure.

\param[in] accessContext - access context pointer.
\param[out] fileDataDescr - memory descriptor to store the file's value.

\return True if location has been successful, false otherwise.
******************************************************************************/
bool pdsPrepareMemoryAccess(PdsDataAccessContext_t *accessContext,
                            MEMORY_DESCRIPTOR *fileDataDescr);

/******************************************************************************
\brief If memory mask contains any directory memoryIds they'll be expanded
       into set of file memoryIds.

\param[in] accessContext - access context pointer
\param[in] memoryId - unique memory identifier
******************************************************************************/
void pdsExpandWithDirContent(PDS_MemMask_t memoryMask);

/*****************************************************************************
                              Inline static functions section
******************************************************************************/
/******************************************************************************
\brief Checks if any bit in mask is set.

\param[in] memoryMask - bitmask of non-volatile memory contents.

\return True if any bit is set, false otherwise.
******************************************************************************/
static inline bool pdsMemMaskIsAnyBitSet(PDS_MemMask_t memoryMask)
{
  for (uint8_t i = 0; i < PDS_MEM_MASK_LENGTH; i++)
    if (memoryMask[i]) return true;

  return false;
}

/******************************************************************************
\brief Initializes non-volatile memory mask with specified static iNode.

\param[in] memoryId - static inode of file or directory.
\param[in] memoryMask - bitmask of non-volatile memory contents.

\return None.
******************************************************************************/
static inline void pdsInitMemMask(PDS_MemId_t memoryId, PDS_MemMask_t memoryMask)
{
  PDS_MEM_MASK_CLEAR_ALL(memoryMask);
  PDS_MEM_MASK_SET_BIT(memoryMask, memoryId);
  pdsExpandWithDirContent(memoryMask);
}

#endif /*_PDSMEMACCESS_H*/
