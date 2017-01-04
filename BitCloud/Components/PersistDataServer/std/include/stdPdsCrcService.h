/***************************************************************************//**
  \file pdsCrcService.h

  \brief The header file describes the interface of crc counting

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
    $Id: stdPdsCrcService.h 27584 2015-01-09 14:45:42Z unithra.c $
*****************************************************************************/

#ifndef _PDSCRCSERVICE_H
#define _PDSCRCSERVICE_H

/******************************************************************************
                   Includes section
******************************************************************************/
#include <pdsDataServer.h>
#include <stdPdsMemAccess.h>

/******************************************************************************
                   Defines section
******************************************************************************/
#define PDS_CRC_INITIAL_VALUE   0U

#ifdef _ENABLE_PERSISTENT_SERVER_
/******************************************************************************
                   Prototypes section
******************************************************************************/
/******************************************************************************
\brief Calculates CRC of specified data.

\param[in] initValue - CRC initial value.
\param[in] data - pointer to data to be processed.
\param[in] data - data length in bytes.

\return Result of calculation.
******************************************************************************/
PDS_FileCrc_t pdsCalculateRAMDataCrc(PDS_FileCrc_t initValue, uint8_t *data, uint16_t length);

/******************************************************************************
\brief Check file data within non-volatile storage.

\param[in] memoryId - file unique identificator.
\param[in] fileDataDescr - file payload memory access descriptor.

\return result of checking.
******************************************************************************/
PDS_DataServerState_t pdsCheckFile(PDS_MemId_t memoryId, const MEMORY_DESCRIPTOR *const fileDataDescr);

/******************************************************************************
\brief Check if any valid data exists in non-volatile memory.

\param[out] memoryMask - pointer to mask of PDTs to be checked on presence
                            in non-volatile memory.

\return Operation result, and mask of PDTs been found in non-volatile memory.
******************************************************************************/
PDS_DataServerState_t pdsCheckStorage(PDS_MemMask_t memoryMask);

#endif /* _ENABLE_PERSISTENT_SERVER_ */
#endif /*_PDSCRCSERVICE_H*/
