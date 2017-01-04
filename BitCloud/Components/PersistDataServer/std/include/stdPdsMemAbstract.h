/***************************************************************************//**
  \file pdsMemAbstract.h

  \brief Memory abstract header

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    6/11/08 A. Khromykh - Created
  Last change:
    $Id: stdPdsMemAbstract.h 27584 2015-01-09 14:45:42Z unithra.c $
*****************************************************************************/
#ifndef _PDSMEMABSTRACT_H
#define _PDSMEMABSTRACT_H

/* Type of non-volatile memory */
#define USE_EEPROM

/* This header file performs persistence memory API redefinition.
 * The goal is to achieve memory type independence for persist data server (PDS).
 *
 *
 * MEMORY_DESCRIPTOR - persistence memory descriptor. It is a structure type, which
 * includes the following variables:
 *   uint16_t address - internal persistence memory address to be accessed.
 *   uint8_t  *data   - pointer to the variable in RAM to be stored (or to be filled
 *                      by read value from persistence).
 *   uint16_t length  - "data" variable length (bytes). Equal to the number of bytes
 *                      to be exchanged between persistence memory and RAM.
 *
 *
 * WRITE_MEMORY, READ_MEMORY - persistence memory access functions. These functions
 * should have the following form:
 *   int write/read(MEMORY_DESCRIPTOR *memoryDescr, void (*callback)(void));
 *     Parameters:
 *       memoryDescr - pointer to memory descriptor
 *       callback    - pointer to callback function. Callback function will be called
 *                     after read or write persistence memory operation is completed.
 *     Returns:
 *        0 - successful access;
 *       -1 - the number of bytes to read (write) is too large.
 * Persistence memory access functions should perform memory arbitration also, i.e.
 * memory busy return status is illegal.
 *
 *
 * IS_MEMORY_BUSY - persistence memory access function, should have the following form:
 *   bool isMemoryBusy(void);
 *     Parameters:
 *       none.
 *     Returns:
 *       true  - memory is busy
 *       false - memory is free;
*/

#if defined(USE_EEPROM)

#include <eeprom.h>

#define WRITE_MEMORY        HAL_WriteEeprom
#define READ_MEMORY         HAL_ReadEeprom
#define IS_MEMORY_BUSY      HAL_IsEepromBusy
#define MEMORY_DESCRIPTOR   HAL_EepromParams_t

#endif

#endif /* _PDSMEMABSTRACT_H */
