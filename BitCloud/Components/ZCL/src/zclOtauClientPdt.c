/************************************************************************//**
  \file zclOtauClientPdt.c

  \brief
    The file implements the OTAU client PDT definition

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    15.10.14 Karthik.P_u - Created
  Last change:
    $Id: zclOtauClient.c 27259 2014-10-15 08:10:39Z karthik.p_u $
******************************************************************************/

#if APP_SUPPORT_OTAU_RECOVERY == 1 && defined(_ENABLE_PERSISTENT_SERVER_)

/*******************************************************************************
                            Includes section
*******************************************************************************/
#include <zclOtauManager.h>
#include <zclOtauClient.h>
#include <pdsDataServer.h>
#include <sysUtils.h>

/*******************************************************************************
                      External Global variables section
*******************************************************************************/
ExtAddr_t otauServerExtAddr     = ZERO_SERVER_EXT_ADDRESS;
uint32_t otauImageSize          = 0ul;
uint32_t otauImageVersion       = 0ul;
uint32_t otauNextOffset         = 0ul;
uint32_t otauInternalLength     = 0ul;

uint8_t otauInternalAddrStatus  = 0u;
uint8_t otauRunningChecksum     = 0xFFu;
uint32_t otauImageRemainder     = 0ul;
uint32_t otauFlashWriteOffset   = 0ul;

/*******************************************************************************
                OTAU client persistent data table definition
*******************************************************************************/
// ZCL OTA Upgrade Client data file descriptors. Shall be placed in the PDS_FF code segment.
PDS_DECLARE_FILE(OTAU_SERVER_EXT_ADDR_MEM_ID,  sizeof(ExtAddr_t), &otauServerExtAddr,      NO_FILE_MARKS);
PDS_DECLARE_FILE(OTAU_IMAGE_VERSION_MEM_ID,    sizeof(uint32_t),  &otauImageVersion,       NO_FILE_MARKS);
PDS_DECLARE_FILE(OTAU_IMAGE_SIZE_MEM_ID,       sizeof(uint32_t),  &otauImageSize,          NO_FILE_MARKS);
PDS_DECLARE_FILE(OTAU_NEXT_OFFSET_MEM_ID,      sizeof(uint32_t),  &otauNextOffset,         NO_FILE_MARKS);
PDS_DECLARE_FILE(OTAU_RUNNING_CHECKSUM_MEM_ID, sizeof(uint8_t),   &otauRunningChecksum,    NO_FILE_MARKS);
PDS_DECLARE_FILE(OTAU_INTERNAL_LENGTH_MEM_ID,  sizeof(uint32_t),  &otauInternalLength,     NO_FILE_MARKS);
PDS_DECLARE_FILE(OTAU_IMAGE_REMAINDER_MEM_ID,  sizeof(uint32_t),  &otauImageRemainder,     NO_FILE_MARKS);
PDS_DECLARE_FILE(OTAU_ADDRESS_STATUS_MEM_ID,   sizeof(uint8_t),   &otauInternalAddrStatus, NO_FILE_MARKS);
PDS_DECLARE_FILE(OTAU_IMAGE_WRITE_OFFSET_ID,   sizeof(uint32_t),  &otauFlashWriteOffset,   NO_FILE_MARKS);

// ZCL OTA Upgrade Client data file identifiers list.
// Shall be placed in PDS_FF segment
PROGMEM_DECLARE(PDS_MemId_t otauImgFilesMemoryIdsTable[]) =
{
  OTAU_SERVER_EXT_ADDR_MEM_ID,
  OTAU_IMAGE_VERSION_MEM_ID,
  OTAU_IMAGE_SIZE_MEM_ID,
};

PROGMEM_DECLARE(PDS_MemId_t otauAuxFilesMemoryIdsTable[]) =
{
  OTAU_IMAGE_REMAINDER_MEM_ID,
  OTAU_ADDRESS_STATUS_MEM_ID,
};

PROGMEM_DECLARE(PDS_MemId_t otauOffsetFilesMemoryIdsTable[]) =
{
  OTAU_NEXT_OFFSET_MEM_ID,
  OTAU_RUNNING_CHECKSUM_MEM_ID,
  OTAU_INTERNAL_LENGTH_MEM_ID,
  OTAU_IMAGE_WRITE_OFFSET_ID,
};

// ZCL OTA Upgrade Client data directory descriptor.
//Shall be placed in the PDS_FD code segment.
PDS_DECLARE_DIR(PDS_DirDescr_t otauImgFilesMemoryDirDescr) =
{
  .list       = otauImgFilesMemoryIdsTable,
  .filesCount = ARRAY_SIZE(otauImgFilesMemoryIdsTable),
  .memoryId   = OTAU_IMGFILES_MEMORY_MEM_ID
};

PDS_DECLARE_DIR(PDS_DirDescr_t otauAuxFilesMemoryDirDescr) =
{
  .list       = otauAuxFilesMemoryIdsTable,
  .filesCount = ARRAY_SIZE(otauAuxFilesMemoryIdsTable),
  .memoryId   = OTAU_AUXFILES_MEMORY_MEM_ID
};

PDS_DECLARE_DIR(PDS_DirDescr_t otauOffsetFilesMemoryDirDescr) =
{
  .list       = otauOffsetFilesMemoryIdsTable,
  .filesCount = ARRAY_SIZE(otauOffsetFilesMemoryIdsTable),
  .memoryId   = OTAU_OFFSETFILES_MEMORY_MEM_ID
};

#endif // APP_SUPPORT_OTAU_RECOVERY == 1 && defined(_ENABLE_PERSISTENT_SERVER_)

//eof zclOtauClientPdt.c
