/**************************************************************************//**
  \file pdsMemAccess.c

  \brief Implementation of memory access functionality.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    2012-03-15 A. Razinkov - Modified
  $Id: pdsMemAccess.c 19883 2012-01-27 14:20:37Z arazinkov $
*****************************************************************************/

#if PDS_ENABLE_WEAR_LEVELING != 1
#ifdef _ENABLE_PERSISTENT_SERVER_
/******************************************************************************
                   Includes section
******************************************************************************/
#include <stdPdsMemAccess.h>
#include <stdPdsMem.h>
#include <stdPdsDbg.h>
#include <stdPdsWriteData.h>
#include <sysAssert.h>

/*****************************************************************************
                   Definitions section
******************************************************************************/
#define BAD_FILE_INDEX      0xFFFF

/****************************************************************************
                   Static functions prototypes section
******************************************************************************/
static uint16_t pdsGetFileOffsetTableIndex(PDS_MemId_t memoryId);

/******************************************************************************
                   Implementation section
******************************************************************************/
/******************************************************************************
\brief Obtains appropriate file descriptor record by specified memory identifier.

\param[in] memoryId - an unique file identifier.

\return Appropriate file descriptor record or NULL, if no one exists.
******************************************************************************/
PDS_FileDescrRec_t pdsGetFileDescrRec(PDS_MemId_t memoryId)
{
  PDS_FileDescrRec_t ffTable = pdsMemory()->ffStart;
  PDS_MemId_t fileMemoryId;

  for (uint16_t recordIndex = 0; recordIndex < pdsMemory()->ffSize; recordIndex++)
  {
    memcpy_P(&fileMemoryId, &ffTable[recordIndex].memoryId, sizeof(fileMemoryId));
    if (fileMemoryId == memoryId)
      return &ffTable[recordIndex];
  }
  return NULL;
}
/******************************************************************************
\brief Obtains appropriate directory descriptor record by specified memory identifier.

\param[in] memoryId - an unique directory identifier.

\return Appropriate directory descriptor record or NULL, if no one exists.
******************************************************************************/
PDS_DirDescrRec_t pdsGetDirectoryDescrRec(PDS_MemId_t memoryId)
{
  PDS_DirDescrRec_t fdTable = pdsMemory()->fdStart;
  PDS_MemId_t directoryMemoryId;

  for (uint16_t recordIndex = 0; recordIndex < pdsMemory()->fdSize; recordIndex++)
  {
    memcpy_P(&directoryMemoryId, &fdTable[recordIndex].memoryId, sizeof(directoryMemoryId));
    if (directoryMemoryId == memoryId)
      return &fdTable[recordIndex];
  }
  return NULL;
}

/******************************************************************************
\brief Calculates file sequence number by memory mask of allowed for storing
       files.

\param[in] memoryId - an unique file identifier.

\return File index or BAD_FILE_INDEX if error is occured.
*******************************************************************************/
static uint16_t pdsGetFileOffsetTableIndex(PDS_MemId_t memoryId)
{
  PDS_MemMask_t memoryMask;
  uint16_t index = 0;
  uint8_t i;

  if (!PDS_MEM_MASK_IS_BIT_SET(pdsMemory()->allowedForStoring, memoryId))
    return BAD_FILE_INDEX;

  memcpy(memoryMask, pdsMemory()->allowedForStoring, PDS_MEM_MASK_LENGTH);

  /* Count bits set in mask before byte which contains bit specified by memoryId */
  for (i = 0; i < memoryId / 8; i++)
  {
    memoryMask[i] = (memoryMask[i] & 0x55) + ((memoryMask[i] >> 1) & 0x55);
    memoryMask[i] = (memoryMask[i] & 0x33) + ((memoryMask[i] >> 2) & 0x33);
    index += (memoryMask[i] & 0x0f) + ((memoryMask[i] >> 4) & 0x0f);
  }

  /* Count bits set in byte which contains bit specified by memoryId */
  for (i = 0; i < memoryId % 8; i++)
    index += (memoryMask[memoryId / 8] & (1U << i)) ? 1 : 0;

  return index;
}

/******************************************************************************
\brief Calculates file offset within non-volatile memory.

\param[in] memoryId - a unique file identifier.

\return Offset in non-volatile memory from which the file stored.
******************************************************************************/
void pdsGetFileOffset(PDS_MemId_t memoryId, uint16_t *fileOffset)
{
  MEMORY_DESCRIPTOR descriptor;
  PDS_FileOffsetInfo_t fileOffsetInfo;
  uint16_t offsetTableIndex = pdsGetFileOffsetTableIndex(memoryId);

  descriptor.data = (uint8_t *)&fileOffsetInfo;
  descriptor.address = SYSTEM_BASE_STORAGE_ADDRESS + offsetTableIndex * sizeof(PDS_FileOffsetInfo_t);
  descriptor.length = sizeof(PDS_FileOffsetInfo_t);
  pdsRead(&descriptor, pdsDummyCallback);

  SYS_E_ASSERT_FATAL((memoryId == fileOffsetInfo.memoryId), PDSMEMACCESS_PDSPREPAREACCESSCONTEXT1);

  *fileOffset = fileOffsetInfo.offset;
}

/******************************************************************************
\brief Prepares a context for access to data stored in non-volatile memory.
\
\param[out] accessContext - access context pointer.
\param[in] memoryMask - bitmask of non-volatile memory contents to which access
                        is needed.
******************************************************************************/
void pdsPrepareAccessContext(PdsDataAccessContext_t *accessContext, const uint8_t *const memoryMask)
{
  accessContext->memoryId = PDS_NO_MEMORY_SPECIFIED;
  memcpy(accessContext->memoryMask, memoryMask, PDS_MEM_MASK_LENGTH);
  pdsExpandWithDirContent(accessContext->memoryMask);

  /* Ignore all memory, unexistent for current build configuration */
  for (uint8_t i = 0; i < PDS_MEM_MASK_LENGTH; i++)
  {
    accessContext->memoryMask[i] &= pdsMemory()->allowedForStoring[i];
  }
}

/******************************************************************************
\brief Locates first file by specified access context and prepares memory
       descriptor to store the file's data in non-volatile memory.
       Located file is deleted from a list in the access context structure.

\param[in] accessContext - access context pointer.
\param[out] fileDataDescr - memory descriptor to store the file's value.

\return True if location has been successful, false otherwise.
******************************************************************************/
bool pdsPrepareMemoryAccess(PdsDataAccessContext_t *accessContext,
                            MEMORY_DESCRIPTOR *fileDataDescr)
{
  PDS_MemId_t memoryId;
  PDS_FileDescrRec_t fileDescrRec = NULL;
  PDS_FileDescr_t fileDescr;

  /* Obtain currently processed file memoryId */
  for (memoryId = 0; memoryId < PDS_MEM_IDS_AMOUNT; memoryId++)
    if (PDS_MEM_MASK_IS_BIT_SET(accessContext->memoryMask, memoryId))
    {
      PDS_MEM_MASK_CLEAR_BIT(accessContext->memoryMask, memoryId);
      fileDescrRec = pdsGetFileDescrRec(memoryId);
      break;
    }

  // Fill memory descriptor if file descroptor is found
  if (fileDescrRec)
  {
    memcpy_P(&fileDescr, fileDescrRec, sizeof(PDS_FileDescr_t));

    fileDataDescr->data = fileDescr.RAMAddr;
    pdsGetFileOffset(fileDescr.memoryId, &fileDataDescr->address);
    fileDataDescr->address += sizeof(PDS_FileHeader_t);
    fileDataDescr->length = fileDescr.size;

    SYS_E_ASSERT_FATAL(fileDataDescr->data, PDSMEMACCESS_PDSPREPAREMEMORYACCESS0);

    accessContext->memoryId = fileDescr.memoryId;
    return true;
  }

  accessContext->memoryId = PDS_NO_MEMORY_SPECIFIED;
  return false;
}

/******************************************************************************
\brief If memory mask contains any directory memoryIds they'll be expanded
       into set of file memoryIds.

\param[in] accessContext - access context pointer
\param[in] memoryId - unique memory identifier
******************************************************************************/
void pdsExpandWithDirContent(PDS_MemMask_t memoryMask)
{
  PDS_DirDescrRec_t dirDescrRec = pdsMemory()->fdStart;
  PDS_DirDescr_t dir;
  PDS_MemId_t memoryId;
  bool expanded;

  if (PDS_MEM_MASK_IS_BIT_SET(memoryMask, PDS_ALL_EXISTENT_MEMORY))
  {
    PDS_MEM_MASK_SET_ALL(memoryMask);
    return;
  }

  /* Looping through all directories, including nested ones and expanding them */
  do
  {
    expanded = false;

    for (uint16_t recordIndex = 0; recordIndex < pdsMemory()->fdSize; recordIndex++)
    {
      memcpy_P(&dir, &dirDescrRec[recordIndex], sizeof(PDS_DirDescr_t));

      if (PDS_MEM_MASK_IS_BIT_SET(memoryMask, dir.memoryId))
      {
        PDS_MEM_MASK_CLEAR_BIT(memoryMask, dir.memoryId);

        /* Loop one more time in case of nested directory */
        expanded = true;
        for (uint8_t i = 0; i < dir.filesCount; i++)
        {
          memcpy_P(&memoryId, &dir.list[i], sizeof(PDS_MemId_t));
          PDS_MEM_MASK_SET_BIT(memoryMask, memoryId);
        }
      }
    }
  } while (expanded);
}

#endif /* _ENABLE_PERSISTENT_SERVER_ */
#endif /* PDS_ENABLE_WEAR_LEVELING != 1 */
// eof pdsMemAccess.c
