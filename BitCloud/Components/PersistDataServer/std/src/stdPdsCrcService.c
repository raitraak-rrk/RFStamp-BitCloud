/***************************************************************************//**
  \file pdsCrcService.c

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
    $Id: stdPdsCrcService.c 27584 2015-01-09 14:45:42Z unithra.c $
*****************************************************************************/

#if PDS_ENABLE_WEAR_LEVELING != 1
#ifdef _ENABLE_PERSISTENT_SERVER_
/******************************************************************************
                   Includes section
******************************************************************************/
#include <stdPdsMemAbstract.h>
#include <stdPdsCrcService.h>
#include <stdPdsMemAccess.h>
#include <sysTaskManager.h>
#include <stdPdsWriteData.h>
#include <stdPdsMem.h>

/******************************************************************************
                   Implementation section
******************************************************************************/
/******************************************************************************
\brief Calculates CRC of specified data.

\param[in] initValue - CRC initial value.
\param[in] data - pointer to data to be processed.
\param[in] data - data length in bytes.

\return Result of calculation.
******************************************************************************/
PDS_FileCrc_t pdsCalculateRAMDataCrc(PDS_FileCrc_t initValue, uint8_t *data, uint16_t length)
{
  PDS_FileCrc_t crc = initValue;

  for (uint16_t i = 0; i < length; i++)
    crc += data[i];

  return crc;
}

/******************************************************************************
\brief Check if any valid data exists in non-volatile memory.

\param[out] memoryMask - pointer to mask of PDTs to be checked on presence
                            in non-volatile memory.

\return Operation result, and mask of PDTs been found in non-volatile memory.
******************************************************************************/
PDS_DataServerState_t pdsCheckStorage(PDS_MemMask_t memoryMask)
{
  MEMORY_DESCRIPTOR fileDataDescr;
  PdsDataAccessContext_t accessContext = {{0u}, 0u};

  pdsPrepareAccessContext(&accessContext, memoryMask);
  PDS_MEM_MASK_CLEAR_ALL(memoryMask);

  while (pdsPrepareMemoryAccess(&accessContext, &fileDataDescr))
  {
    if (PDS_SUCCESS == pdsCheckFile(accessContext.memoryId, &fileDataDescr))
    {
      PDS_MEM_MASK_SET_BIT(memoryMask, accessContext.memoryId);
    }
  }

  return PDS_SUCCESS;
}

/******************************************************************************
\brief Check file data within non-volatile storage.

\param[in] memoryId - file unique identificator.
\param[in] fileDataDescr - file payload memory access descriptor.

\return result of checking.
******************************************************************************/
PDS_DataServerState_t pdsCheckFile(PDS_MemId_t memoryId, const MEMORY_DESCRIPTOR *const fileDataDescr)
{
  uint8_t data;
  PDS_FileHeader_t header;
  MEMORY_DESCRIPTOR accessDescriptor;
  PDS_FileCrc_t fileCrc = PDS_CRC_INITIAL_VALUE;

  accessDescriptor.data = (uint8_t *)&header;
  accessDescriptor.length = sizeof(PDS_FileHeader_t);
  accessDescriptor.address = fileDataDescr->address - sizeof(PDS_FileHeader_t);
  pdsRead(&accessDescriptor, pdsDummyCallback);

  if (memoryId != header.memoryId ||
      fileDataDescr->length != header.size)
    return PDS_CRC_ERROR;

  accessDescriptor.data = &data;
  accessDescriptor.length = 1;

  /* Check for any changes in byte-by-byte order */
  for (uint16_t i = 0; i < fileDataDescr->length; i++)
  {
    accessDescriptor.address = fileDataDescr->address + i;
    pdsRead(&accessDescriptor, pdsDummyCallback);
    fileCrc = pdsCalculateRAMDataCrc(fileCrc, accessDescriptor.data, 1);
  }

  if (fileCrc != header.crc)
    return PDS_CRC_ERROR;

  return PDS_SUCCESS;
}

#ifdef __DBG_PDS__
uint8_t  eepromCopy[0x400u];

void pdsDbgReadAllEeprom(void)
{
  PDS_DataServerState_t status;
  MEMORY_DESCRIPTOR descriptor;

  descriptor.address = 0;
  descriptor.data = eepromCopy;
  descriptor.length = 0x400u;
  status = pdsRead(&descriptor, pdsDummyCallback);

  if (status != PDS_SUCCESS)
    return;
}
#endif
#endif /* _ENABLE_PERSISTENT_SERVER_ */
#endif /* PDS_ENABLE_WEAR_LEVELING != 1 */
// eof pdsDataServer.c
