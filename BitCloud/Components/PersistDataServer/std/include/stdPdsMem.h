/**************************************************************************//**
  \file pdsMem.h

  \brief Persistent Data Server memory header file.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2012-01-16 A. Razinkov - Created.
   Last change:
    $Id: stdPdsMem.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#ifndef _PDS_MEM_H
#define _PDS_MEM_H

/******************************************************************************
                              Includes section
 ******************************************************************************/
#include <pdsDataServer.h>
#include <stdPdsMemAccess.h>
#include <appTimer.h>

/******************************************************************************
                               Types section
 ******************************************************************************/
/* Internal variables of PDS layer. */
typedef struct _PdsMem_t
{
  /* PDS file descriptors table */
  PDS_FileDescrRec_t ffStart;
  /* PDS directory descriptors table */
  PDS_DirDescrRec_t fdStart;
  /* Number of file descriptors */
  uint16_t  ffSize;
  /* Number of directory descriptors */
  uint16_t  fdSize;
  /* Total size of memory occupied by persistent parameters */
  uint32_t totalMemorySize;
  /* Persistent data memory identifiers. Specifies files or directories that was restored
     from non-volatile memory */
  PDS_MemMask_t restoredMemory;
  /* List of file identifiers to commit to non-volatile memory */
  PDS_MemMask_t currentCommitmentList;
  /* Configuration-dependent list of memory identifiers, allowed for storing in 
     non-volatile memory */
  PDS_MemMask_t allowedForStoring;
  /* Timer for periodic commit to non-volatile memory */
  HAL_AppTimer_t periodicCommitTimer;
  /* Persistent data memory identifier. Specifies file or directory that is
     to be stored to non-volatile memory by periodic timer expiration. */
  PDS_MemId_t memIdToStoreByTimer;
  /* PDS working status */
  uint8_t status;
} PdsMem_t;

/******************************************************************************
                               External variables section
 ******************************************************************************/
extern PdsMem_t pdsMem;

/******************************************************************************
                               Implementation section
 ******************************************************************************/
INLINE PdsMem_t *pdsMemory(void)
{
  return &pdsMem;
}

#endif /* _PDS_MEM_H */
/** eof pdsMem.h */

