/**
  \file D_Nv.c

  \brief NV component implementation.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    15.06.13 N. Fomin - created
    24.06.13 M. E. Johansen - modified
*/

#if PDS_ENABLE_WEAR_LEVELING == 1
#ifdef _ENABLE_PERSISTENT_SERVER_
/******************************************************************************
                   Includes section
******************************************************************************/
#include <D_Nv_Bindings.h>
#include <D_Nv.h>
#include <D_Nv_Init.h>
#include <flash.h>
#include <N_ErrH.h>
#include <N_Types.h>
#include <sysUtils.h>

/******************************************************************************
                   Definitions section
******************************************************************************/
#define COMPID "D_Nv"

#ifdef  __IAR_SYSTEMS_ICC__
#pragma segment="D_NV_MEMORY"
#define D_NV_MEMORY_START  ((uint32_t)__sfb("D_NV_MEMORY"))
// Location of last memory element, NOT the first memory location after it
#define D_NV_MEMORY_END  ((uint32_t)__sfe("D_NV_MEMORY") - 1U)
#elif __GNUC__
#define D_NV_MEMORY_START (uint32_t)&__d_nv_mem_start
#define D_NV_MEMORY_END (((uint32_t)&__d_nv_mem_end) - 1U)
#else
  #error "Unsupported compiler"
#endif

/******************************************************************************
                   Extern section
******************************************************************************/
#ifdef __GNUC__
extern uint32_t __d_nv_mem_start;
extern uint32_t __d_nv_mem_end;
#endif

/***************************************************************************************************
* LOCAL FUNCTION DECLARATIONS
***************************************************************************************************/
static bool CompareData(uint8_t sector, uint16_t offset, uint8_t *pBuffer, D_Nv_Size_t numberOfBytes);

/******************************************************************************
                   Implementations section
******************************************************************************/
/** Initializes the component -- checks that parameters are correct.
*/
void D_Nv_Init(void)
{
  // Ensure that the memory range starts on a page boundary
  N_ERRH_ASSERT_FATAL((D_NV_MEMORY_START % D_NV_PAGE_SIZE) == 0U);
  // Ensure that sector size is an integer number of pages
  N_ERRH_ASSERT_FATAL((D_NV_SECTOR_SIZE % D_NV_PAGE_SIZE) == 0U);
  // Ensure that segment is as large as we need
  N_ERRH_ASSERT_FATAL((D_NV_MEMORY_END - D_NV_MEMORY_START + 1U) == D_NV_MEMORY_SIZE);
}

/** Read bytes from the internal NV.
    \param sector The sector to use (0..D_NV_SECTOR_COUNT)
    \param offset The offset to start reading at
    \param[out] pBuffer The start address of the buffer to store the read data to
    \param numberOfBytes The number of bytes to read
    \note offset+numberOfBytes must be <= D_NV_SECTOR_SIZE (meaning sector crossing is not permitted)
*/
void D_Nv_Read(uint8_t sector, uint16_t offset, uint8_t *pBuffer, D_Nv_Size_t numberOfBytes)
{
  uint32_t address = D_NV_MEMORY_START + (uint32_t)(sector - D_NV_FIRST_SECTOR) * D_NV_SECTOR_SIZE + offset;

  N_ERRH_ASSERT_FATAL(address <= D_NV_MEMORY_END);
  N_ERRH_ASSERT_FATAL((address + numberOfBytes) <= (D_NV_MEMORY_END + 1U));
  N_ERRH_ASSERT_FATAL((offset + numberOfBytes) <= D_NV_SECTOR_SIZE);

  memcpy_P(pBuffer, (uint8_t FLASH_PTR *)address, numberOfBytes);
}

/** Write bytes to the internal NV.
    \param sector The sector to use (0..D_NV_SECTOR_COUNT)
    \param offset The offset to start writing to
    \param[in] pBuffer The start address of the buffer that contains the data to write
    \param numberOfBytes The number of bytes to write
    \note offset+numberOfBytes must be <= D_NV_SECTOR_SIZE (meaning sector crossing is not permitted)
    \note pBuffer cannot be const because it is passed to HAL_FillFlashPageBuffer()
*/
void D_Nv_Write(uint8_t sector, uint16_t offset, uint8_t *pBuffer, D_Nv_Size_t numberOfBytes)
{
  uint32_t address = D_NV_MEMORY_START + (uint32_t)(sector - D_NV_FIRST_SECTOR) * D_NV_SECTOR_SIZE + offset;
  uint16_t pageOffset;
  D_Nv_Size_t numberOfPageBytes;

  N_ERRH_ASSERT_FATAL(numberOfBytes);
  N_ERRH_ASSERT_FATAL(address <= D_NV_MEMORY_END);
  N_ERRH_ASSERT_FATAL((address + numberOfBytes) <= (D_NV_MEMORY_END + 1U));
  N_ERRH_ASSERT_FATAL((offset + numberOfBytes) <= D_NV_SECTOR_SIZE);

  pageOffset = address % D_NV_PAGE_SIZE;
  numberOfPageBytes = MIN((D_NV_PAGE_SIZE - pageOffset), numberOfBytes);

  HAL_FillFlashPageBuffer(pageOffset, numberOfPageBytes, pBuffer);
  HAL_WriteFlashPage(address);

  numberOfBytes -= numberOfPageBytes;
  address += numberOfPageBytes;
  pBuffer += numberOfPageBytes;

  while (0U < numberOfBytes)
  {
    numberOfPageBytes = MIN(D_NV_PAGE_SIZE, numberOfBytes);

    HAL_FillFlashPageBuffer(0U, numberOfPageBytes, pBuffer);
    HAL_WriteFlashPage(address);

    numberOfBytes -= numberOfPageBytes;
    address += numberOfPageBytes;
    pBuffer += numberOfPageBytes;
  }
}

/** Erases a sector of the internal NV.
   \param sector The sector to erase (0..D_NV_SECTOR_COUNT)
*/
void D_Nv_EraseSector(uint8_t sector)
{
  uint32_t address = D_NV_MEMORY_START + (uint32_t)(sector - D_NV_FIRST_SECTOR) * D_NV_SECTOR_SIZE;

  N_ERRH_ASSERT_FATAL(address <= D_NV_MEMORY_END);

  for (uint8_t i = 0U; i < (D_NV_SECTOR_SIZE / D_NV_PAGE_SIZE); i++)
  {
    HAL_EraseFlashPage(address);
    address += D_NV_PAGE_SIZE;
  }
}

/** Compare bytes with contents of the internal NV.
    \param sector The sector to use (0..D_NV_SECTOR_COUNT)
    \param offset The offset to start comparing with
    \param[in] pBuffer If NULL then read buffer will be compared with 0xFF
     Otherwise it is the start address of the buffer that contains the data to compare with
    \param numberOfBytes The number of bytes to compare
    \note offset+numberOfBytes must be <= D_NV_SECTOR_SIZE (meaning sector crossing is not permitted)
*/
static bool CompareData(uint8_t sector, uint16_t offset, uint8_t *pBuffer, D_Nv_Size_t numberOfBytes)
{
  uint32_t address = D_NV_MEMORY_START + (uint32_t)(sector - D_NV_FIRST_SECTOR) * D_NV_SECTOR_SIZE + offset;
  uint8_t  buffer[16U];

  N_ERRH_ASSERT_FATAL(numberOfBytes);
  N_ERRH_ASSERT_FATAL(address <= D_NV_MEMORY_END);
  N_ERRH_ASSERT_FATAL((address + numberOfBytes) <= (D_NV_MEMORY_END + 1U));
  N_ERRH_ASSERT_FATAL((offset + numberOfBytes) <= D_NV_SECTOR_SIZE);

  while(numberOfBytes)
  {
    uint16_t bytesToRead = MIN(sizeof(buffer), numberOfBytes);

    memcpy_P(buffer, (uint8_t FLASH_PTR *)address, bytesToRead);
    for (uint8_t i = 0U; i < bytesToRead; i++)
      if (buffer[i] != ((pBuffer != NULL) ? *(pBuffer++) : 0xFF))
        return false;

    numberOfBytes -= bytesToRead;
    address += bytesToRead;
  }

  return true;
}

/** Checks if the requested range is empty (containing all 0xFF).
    \param sector The sector to use (0..D_NV_SECTOR_COUNT)
    \param offset The start offset to check
    \param numberOfBytes The number of bytes to check
    \returns TRUE if the range is empty, FALSE otherwise.
    \note Sector crossing is not permitted
*/
bool D_Nv_IsEmpty(uint8_t sector, uint16_t offset, D_Nv_Size_t numberOfBytes)
{
  return CompareData(sector, offset, NULL, numberOfBytes);
}

/** Compare bytes with contents of the internal NV.
    \param sector The sector to use (0..D_NV_SECTOR_COUNT)
    \param offset The offset to start comparing with
    \param[in] pBuffer The start address of the buffer that contains the data to compare with
    \param numberOfBytes The number of bytes to compare
    \note offset+numberOfBytes must be <= D_NV_SECTOR_SIZE (meaning sector crossing is not permitted)
*/
bool D_Nv_IsEqual(uint8_t sector, uint16_t offset, uint8_t *pBuffer, D_Nv_Size_t numberOfBytes)
{
  return CompareData(sector, offset, pBuffer, numberOfBytes);
}
#endif // _ENABLE_PERSISTENT_SERVER_
#endif // #if PDS_ENABLE_WEAR_LEVELING == 1
