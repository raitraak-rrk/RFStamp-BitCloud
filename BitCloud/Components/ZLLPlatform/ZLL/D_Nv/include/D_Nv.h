/**
  \file D_Nv.h

  \brief NV component header.

  The segment D_NV_MEMORY must be defined and reserved in the linker script
  (.xcl file), for example like this:

    -D_D_NV_MEMORY_START=400
    -D_D_NV_MEMORY_SIZE=4000

    -Z(CODE)D_NV_MEMORY+_D_NV_MEMORY_SIZE=[_D_NV_MEMORY_START:+_D_NV_MEMORY_SIZE]

  Note that nothing should actually be placed in the segment.
  These linker script commands will ensure that the entire range
  is reserved and not programmed (left as 0xff).

  The segment should start on a flash page boundary, and the
  size should correspond to the define D_NV_MEMORY_SIZE.

  Further, since the device can only erase a whole page and not
  arbitrary locations in flash, the sector size _must_ be an
  integer multiple of the flash page size.
  Otherwise, erasing the first or last page of a sector will
  also erase content in flash locations neighbouring the sector.

  The init function D_Nv_Init() asserts that the segment is
  located on a flash page boundary, and that the sector and
  segment sizes are correct.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    24.06.13 M. E. Johansen - created
*/

#include <N_Types.h>

#ifndef D_NV_H
#define D_NV_H


typedef uint16_t D_Nv_Size_t;

// Flash page size of the device
#if !defined(D_NV_PAGE_SIZE)
#  define D_NV_PAGE_SIZE  256
#endif

#if !defined(D_NV_FIRST_SECTOR)
#  define D_NV_FIRST_SECTOR  0
#endif

#if !defined(D_NV_SECTOR_COUNT)
#  define D_NV_SECTOR_COUNT  2
#endif

// Sector size must be an integer multiple of flash pages
#if !defined(D_NV_SECTOR_SIZE)
#    define D_NV_SECTOR_SIZE  (8192U)
#endif

// Size of D_NV_MEMORY segment should equal this
#define D_NV_MEMORY_SIZE  (D_NV_SECTOR_COUNT * D_NV_SECTOR_SIZE)

#ifdef __cplusplus
extern "C" {
#endif

void D_Nv_Read(uint8_t sector, uint16_t offset, uint8_t *pBuffer, D_Nv_Size_t numberOfBytes);
void D_Nv_Write(uint8_t sector, uint16_t offset, uint8_t *pBuffer, D_Nv_Size_t numberOfBytes);
void D_Nv_EraseSector(uint8_t sector);
bool D_Nv_IsEmpty(uint8_t sector, uint16_t offset, D_Nv_Size_t numberOfBytes);
bool D_Nv_IsEqual(uint8_t sector, uint16_t offset, uint8_t *pBuffer, D_Nv_Size_t numberOfBytes);

#ifdef __cplusplus
}
#endif

#endif // D_NV_H
