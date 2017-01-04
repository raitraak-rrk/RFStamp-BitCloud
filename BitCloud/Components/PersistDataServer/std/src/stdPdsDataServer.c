/***************************************************************************//**
  \file pdsDataServer.c

  \brief Persistence Data Server implementation

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    22/01/08 A. Khromykh - Created
    01/11/10 A. Razinkov - Modified
  Last change:
    $Id: stdPdsDataServer.c 27584 2015-01-09 14:45:42Z unithra.c $
*****************************************************************************/

#if PDS_ENABLE_WEAR_LEVELING != 1
/******************************************************************************
                   Includes section
******************************************************************************/
#include <pdsDataServer.h>
#include <stdPdsCrcService.h>
#include <stdPdsMemAccess.h>
#include <stdPdsWriteData.h>
#include <sysTaskManager.h>
#include <stdPdsMem.h>
#include <stdPdsDbg.h>
#include <configServer.h>

#ifdef _ENABLE_PERSISTENT_SERVER_

/*****************************************************************************
                               Types section
******************************************************************************/
/* Enumeration of memory by it's ability to be stored/restored */
typedef enum _PDS_MemoryAllowedType_t
{
  ALLOWED_FOR_STORING,
  ALLOWED_FOR_RESTORING
} PDS_MemoryAllowedType_t;

/****************************************************************************
                              Static functions prototypes section
******************************************************************************/
static bool pdsGetMemoryAllowed(PDS_MemoryAllowedType_t type, PDS_MemMask_t memoryMask);

/******************************************************************************
                   Implementations section
******************************************************************************/
/***************************************************************************//**
\brief Restores data from non-volatile storage.

\ingroup pds

PDS files not included in the current build configuration will be ignored.
Restoring process will be performed only if all files, expected for actual
configuration, are presented in NV storage.

\param[in] memoryId - an identifier of PDS file or directory to be restored
                      from non-volatile memory.

\return true, if all expected files have been restored, false - otherwise.
*******************************************************************************/
bool PDS_Restore(PDS_MemId_t memoryId)
{
  PDS_MemMask_t memoryToRestore, existentMemory;

  pdsInitMemMask(memoryId, memoryToRestore);

  /* Ignore all memory, unexistent for current build configuration */
  pdsGetMemoryAllowed(ALLOWED_FOR_STORING, memoryToRestore);

  if (pdsMemMaskIsAnyBitSet(memoryToRestore))
  {
    /* Check a non-volatile storage for required content */
    memcpy(existentMemory, memoryToRestore, PDS_MEM_MASK_LENGTH);
    pdsGetMemoryAllowed(ALLOWED_FOR_RESTORING, existentMemory);

    /* If required data exists - try to update from non-volatile memory */
    if (0 == memcmp(memoryToRestore, existentMemory, PDS_MEM_MASK_LENGTH))
      return PDS_SUCCESS == pdsUpdate(memoryToRestore);
  }

  /* Required region(s) wasn't found in non-volatile memory or error occured */
  return false;
}

/***************************************************************************//**
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
  PDS_MemMask_t memoryMask;

  pdsInitMemMask(memoryId, memoryMask);

  /* Ignore all memory, unexistent for current build configuration */
  pdsGetMemoryAllowed(ALLOWED_FOR_STORING, memoryMask);

  if (pdsMemMaskIsAnyBitSet(memoryMask))
  {
    pdsAddDataForCommitment(memoryMask);

    if (!(pdsMemory()->status & PDS_WRITING_INPROGRESS_FLAG))
      pdsStartCommitment();

    return true;
  }

  return false;
}

/***************************************************************************//**
\brief Stores data in non-volatile memory in a synchronous way -
       the application execution will be blocked until the process is completed.

\ingroup pds

All PDS files which are absent in the current build configuration will be ignored.

\param[in] memoryId - an identifier of PDS file or directory to be stored
                      in non-volatile memory.

\return True, if storing process has been performed successfully, false - otherwise.
******************************************************************************/
bool PDS_BlockingStore(PDS_MemId_t memoryId)
{
  if (PDS_Store(memoryId))
  {
    while (pdsMemory()->status & PDS_WRITING_INPROGRESS_FLAG)
      SYS_ForceRunTask();

    return true;
  }

  return false;
}

/***************************************************************************//**
\brief Checks if the specified PDS file or directory can be restored
       from non-volatile memory

\ingroup pds

PDS files not included in the current build configuration will be ignored.

\param[in] memoryId - an identifier of PDS file or directory to be checked.

\return true, if the specified memory can be restored; false - otherwise.
******************************************************************************/
bool PDS_IsAbleToRestore(PDS_MemId_t memoryId)
{
  PDS_MemMask_t memoryToRestore, existentMemory;

  pdsInitMemMask(memoryId, memoryToRestore);
  /* Ignore all memory, unexistent for current build configuration */
  pdsGetMemoryAllowed(ALLOWED_FOR_STORING, memoryToRestore);

  if (pdsMemMaskIsAnyBitSet(memoryToRestore))
  {
    /* Check a non-volatile storage for required content */
    memcpy(existentMemory, memoryToRestore, PDS_MEM_MASK_LENGTH);
    pdsGetMemoryAllowed(ALLOWED_FOR_RESTORING, existentMemory);

    /* True - only if all required data exists in NV storage */
    if (0 == memcmp(memoryToRestore, existentMemory, PDS_MEM_MASK_LENGTH))
      return true;
  }

  return false;
}

/***************************************************************************//**
\brief Checks if the specified PDS file or directory can be stored
       in non-volatile memory

\ingroup pds

\param[in] memoryId - an identifier of PDS file or directory to be checked.

\return true, if the specified memory can be stored; false - otherwise.
*******************************************************************************/
bool PDS_IsAbleToStore(PDS_MemId_t memoryId)
{
  PDS_MemMask_t memoryMask;

  pdsInitMemMask(memoryId, memoryMask);
  /* Ignore all memory, unexistent for current build configuration */
  pdsGetMemoryAllowed(ALLOWED_FOR_STORING, memoryMask);

  if (pdsMemMaskIsAnyBitSet(memoryMask))
    return true;

  return false;
}

/***************************************************************************//**
\brief Returns mask of PDS files, which allowed to be stored in
       (or restored from) non-volatile storage.
       Particular set depends on build configuration and actual storage content.

\param[in] type - request type (ALLOWED_FOR_STORING / ALLOWED_FOR_RESTORING).
\param[out] memoryMask - mask of PDS files.

\return true if operation has been successful, false otherwise.
*******************************************************************************/
static bool pdsGetMemoryAllowed(PDS_MemoryAllowedType_t type, PDS_MemMask_t memoryMask)
{
  switch (type)
  {
    case ALLOWED_FOR_STORING:
      for (uint8_t i = 0; i < PDS_MEM_MASK_LENGTH; i++)
        memoryMask[i] &= pdsMemory()->allowedForStoring[i];
      return pdsMemMaskIsAnyBitSet(memoryMask);
      break;

    case ALLOWED_FOR_RESTORING:
      /* Check if required data exists in non-volatile memory */
      return (PDS_SUCCESS == pdsCheckStorage(memoryMask));
      break;

    default:
      break;
  }

  return false;
}

/***************************************************************************//**
\brief Checks if files specified have been restored from non-volatile memory
       during the latest restoring procedure

\ingroup pds

\param[in] memoryId - an identifier of PDS file or directory to be checked.

\return true, if the specified PDT(s) have been restored; false - otherwise.
******************************************************************************/
bool PDS_IsMemoryRestored(PDS_MemId_t memoryId)
{
  PDS_MemMask_t memoryMask;

  pdsInitMemMask(memoryId, memoryMask);
  /* Ignore all memory, unexistent for current build configuration */
  pdsGetMemoryAllowed(ALLOWED_FOR_STORING, memoryMask);

  if (pdsMemMaskIsAnyBitSet(memoryMask))
  {
    for (uint8_t i = 0; i < PDS_MEM_MASK_LENGTH; i++)
      memoryMask[i] &= pdsMemory()->restoredMemory[i];

    if (pdsMemMaskIsAnyBitSet(memoryMask))
      return true;
  }

  return false;
}

/***************************************************************************//**
\brief Marks the specified restored PDS files to be cleared during a reset phase
      (during ZDO_StartNetwork() execution)

\ingroup pds

\param[in] memoryId - an identifier of PDS file or directory to be cleared.
******************************************************************************/
void PDS_ClearRestoredMemory(PDS_MemId_t memoryId)
{
  PDS_MemMask_t memoryMask;

  pdsInitMemMask(memoryId, memoryMask);

  for (uint8_t i = 0; i < PDS_MEM_MASK_LENGTH; i++)
    pdsMemory()->restoredMemory[i] &= ~memoryMask[i];
}

/*****************************************************************************//**
\brief Removes specified file records from NV Storage

\ingroup pds

\param[in] memoryId - an identifier of PDS file or directory to be removed
                      from NV memory.

\return - PDS state as an operation result.
******************************************************************************/
PDS_DataServerState_t PDS_Delete(PDS_MemId_t memoryId)
{
  PDS_MemMask_t memoryMask;

  PDS_MEM_MASK_CLEAR_ALL(memoryMask);
  PDS_MEM_MASK_SET_BIT(memoryMask, memoryId);

  return (PDS_SUCCESS == pdsCorruptFiles(memoryMask));
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
  // TBD
  // added to remove compiler warning
  (void)includingPersistentItems;
  return PDS_SUCCESS;
}

/******************************************************************************
\brief This function is deprecated - use PDS_BlockingStore() instead.
******************************************************************************/
void PDS_FlushData(PDS_MemId_t memoryId)
{
  PDS_BlockingStore(memoryId);
}

/******************************************************************************
\brief This function is deprecated - use PDS_Delete() instead.
******************************************************************************/
PDS_DataServerState_t PDS_ResetStorage(PDS_MemId_t memoryId)
{
  return PDS_Delete(memoryId);
}

/******************************************************************************
  \brief General PDS task handler - just a stub for this version of PDS
 ******************************************************************************/
void PDS_TaskHandler(void) {}

/******************************************************************************
\brief Must be called from the ::APL_TaskHandler() function only.\n
Reads data from the user area of EEPROM.

\param[in] offset   - data offset
\param[in] data     - pointer to user data area
\param[in] length   - data length
\param[in] callback - pointer to a callback function; if callback is NULL,
                      then data will be read syncronously

\return - PDS state as a result of data reading operation
*******************************************************************************/
PDS_DataServerState_t PDS_ReadUserData(uint16_t offset, uint8_t *data,
                                       uint16_t length, void (*callback)(void))
{
  MEMORY_DESCRIPTOR descriptor;
  PDS_DataServerState_t status;

  descriptor.address = USER_BASE_STORAGE_ADDRESS + offset;
  descriptor.length = length;
  descriptor.data = data;

  if (NULL == callback)
    callback = pdsDummyCallback;

  status = pdsRead(&descriptor, callback);
  if (status != PDS_SUCCESS)
    return status;

  return PDS_SUCCESS;
}

/******************************************************************************
\brief Must be called only from ::APL_TaskHandler() function.\n
Writes data to user area of EEPROM.

\param[in] offset   - data offset
\param[in] data     - pointer to user data area
\param[in] length   - data length
\param[in] callback - pointer to a callback function; if callback is NULL,
                      then data will be written syncronously

\return - PDS state as a result of data writing operation
*******************************************************************************/
PDS_DataServerState_t PDS_WriteUserData(uint16_t offset, uint8_t *data,
                                        uint16_t length, void (*callback)(void))
{
  MEMORY_DESCRIPTOR descriptor;
  PDS_DataServerState_t status;

  descriptor.address = USER_BASE_STORAGE_ADDRESS + offset;
  descriptor.length = length;
  descriptor.data = data;

  if (NULL == callback)
    callback = pdsDummyCallback;

  status = pdsWrite(&descriptor, callback);
  if (status != PDS_SUCCESS)
    return status;

  return PDS_SUCCESS;
}

#endif /* _ENABLE_PERSISTENT_SERVER_ */
#endif /* PDS_ENABLE_WEAR_LEVELING != 1 */
// eof pdsDataServer.c
