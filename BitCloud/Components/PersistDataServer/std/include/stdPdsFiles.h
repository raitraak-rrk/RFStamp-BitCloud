/***************************************************************************//**
  \file stdPdsFiles.h

  \brief PDS files description

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    16.08.12 A. Razinkov - Created.
  Last change:
    $Id: stdPdsFiles.h 22180 2012-08-01 15:10:33Z arazinkov $
*****************************************************************************/

#ifndef _STDPDSFILES_H
#define _STDPDSFILES_H

/******************************************************************************
                    Description section
******************************************************************************/
/*
   - PERSISTENT DATA SERVER (PDS)
   User can specifiy parameters he(she) wants to backup in a non-volatile memory
   and restore in case of power failure. This service is provided by Persistent
   Data Server (PDS) module.
   The BitCloud (TM) stack also uses the same service for its internal structures.

   - FILE and FILE DESCRIPTOR
   Each item which user wants to backup in a non-volatile memory and restore in
   case of power failure is treated as a FILE - actual item value with associated
   service information, FILE DESCRIPTOR. Each file could be accessed by it's ID -
   a uniq 16-bit value associated with a file. File descriptor keeps information
   about item's size and it's displacement in RAM and inside the NV storage.

   All file descriptors should be placed in a special segment inside the
   MCU Flash memory - [PDS_FF].
   The PDS_FILE_DESCR() macro is used to initialize descriptor and
   PDS_DECLARE_FILE() macro is used to place descriptor to required segment.

   - DIRECTORY and DIRECTORY DESCRIPTOR
   PDS is able to operate with separate files or with file lists - DIRECTORIES.
   Directory nesting is allowed. Each directory should be provided with
   DIRECTORY DESCRIPTOR which keeps information about associated items.
   Directories could be accessed by 16-bit ID, different from already associated
   with files.

   All directory descriptors should be placed in a special segment inside the
   MCU Flash memory - [PDS_FD].
   The PDS_DECLARE_DIR() macro is used to place a directory to required segment.
*/

/******************************************************************************
                   Includes section
******************************************************************************/
#include <sysUtils.h>
#include <pdsMemIds.h>

/******************************************************************************
                   Defines section
******************************************************************************/
/* Macros to initialize file descriptor */
#define PDS_FILE_DESCR(id, dataSize, ramAddress, fileMarks) \
{ \
  .memoryId = id, \
  .size = dataSize, \
  .RAMAddr = ramAddress, \
}

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

/* PDS file declaration */
#define PDS_DECLARE_FILE(id, dataSize, ramAddr, fileMarks) \
  PDS_FF_OBJECT(PDS_FileDescr_t pds_ff_##id) = \
    PDS_FILE_DESCR(id, dataSize, ramAddr, fileMarks);

/******************************************************************************
                   Types section
******************************************************************************/
/*! \brief PDS state result after a command call */
typedef enum
{
  PDS_SUCCESS,            //!< Command completed successfully
  PDS_STORAGE_ERROR,      //!< NVM error occurred
  PDS_CRC_ERROR,          //!< Wrong CRC
  PDS_OUT_OF_DATE_ERROR,  //!< Data in RAM does not equal data in the non-volatile memory
  PDS_COMMIT_IN_PROGRESS, //!< Commit to external memory started
} PDS_DataServerState_t;

/* Type of PDS file data size value. */
typedef uint16_t PDS_FileSize_t;

/* Type of PDS file data CRC. */
typedef uint8_t PDS_FileCrc_t;

/* PDS file descriptor structure. */
typedef struct _PDS_FileDescr_t
{
  /* Pointer to file data located in RAM */
  void *RAMAddr;
  /* File data size */
  PDS_FileSize_t size;
  /* File unique identifier */
  PDS_MemId_t memoryId;
} PDS_FileDescr_t;

/* File descriptor record pointer */
typedef const PDS_FileDescr_t FLASH_PTR *PDS_FileDescrRec_t;

/* PDS directory descriptor structure. */
typedef struct _PDS_DirDescr_t
{
  /* Pointer to list of file identifiers */
  PDS_MemIdRec_t list;
  /* Nuber of files in derectory */
  uint16_t    filesCount;
  /* Directory unique identifier */
  PDS_MemId_t memoryId;
} PDS_DirDescr_t;

/* Directory descriptor record pointer */
typedef const PDS_DirDescr_t FLASH_PTR *PDS_DirDescrRec_t;

/* PDS file header structure. */
typedef struct _PDS_FileHeader_t
{
  /* File data CRC */
  PDS_FileCrc_t crc;
  /* File data size */
  PDS_FileSize_t size;
  /* File unique identifier */
  PDS_MemId_t memoryId;
} PDS_FileHeader_t;

/* PDS file offset information. */
typedef struct _PDS_FileOffsetInfo_t
{
  /* File offset in non-volatile memory */
  uint32_t offset;
  /* File unique identifier */
  PDS_MemId_t memoryId;
} PDS_FileOffsetInfo_t;

#endif  /* _STDPDSFILES_H */
/* eof stdPdsFiles.h */

