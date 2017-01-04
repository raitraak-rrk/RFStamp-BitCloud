/**************************************************************************//**
  \file sysDuplicateTable.c

  \brief Implementation of common part of a duplicate rejection table.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2009-11-209 I.Vagulin - Created
    2010-07-15 V.Preobrazhenskiy - Refactored.
    2013-02-11 Max Gekk - duplicate bit mask is extended up to 32 bits.
   Last change:
    $Id: sysDuplicateTable.c 24479 2013-02-11 12:08:31Z mgekk $
 ******************************************************************************/
/******************************************************************************
                             Includes section
 ******************************************************************************/
#include <sysDuplicateTable.h>
#include <appTimer.h>
#include <sysDbg.h>
#include <sysUtils.h>
#include <sysAssert.h>

/******************************************************************************
                      Local functions prototypes section
 ******************************************************************************/
static void sysDuplicateTableUpdate(SYS_DuplicateTable_t *table);

/******************************************************************************
                          Implementations section
 ******************************************************************************/
/**************************************************************************//**
  \brief Prepare duplicate table to real work.

  \param[in] table - pointer to allocated table
  \param[out] entries - pointer to array of entries
  \param[in] tableSize - count of entries in table
  \param[in] agingPeriod - aging period ms
  \param[in] maxTTL - initial value of TTL.
  \param[in] removeOldest - change behaviour on full table

  \return None.
 ******************************************************************************/
void SYS_DuplicateTableReset(SYS_DuplicateTable_t *table, SYS_DuplicateTableEntry_t *entries,
     uint8_t size, uint16_t agingPeriod, uint8_t maxTTL, bool removeOldest,uint8_t maskSize)
{
  SYS_DuplicateTableEntry_t *it;

  table->entries = entries;
  /* Only 7 bits are allocated for size. */
  if (size >= INT8_MAX)
  {
    SYS_E_ASSERT_ERROR(false, SYS_ASSERT_ID_DRT_SIZE_TOO_BIG);
    return;
  }
  /* if size >= INT8_MAX, exit from here */

  table->size = MIN(size, INT8_MAX);
  table->maxTTL = maxTTL;
  table->removeOldest = removeOldest;
  table->agingPeriod = agingPeriod;
#ifdef _DUPLICATE_REJECTION_TABLE_BIT_MASK_ENABLE_
  table->maskSize = MIN(maskSize, sizeof(SYS_DuplicateMask_t) * 8U);
#endif // _DUPLICATE_REJECTION_TABLE_BIT_MASK_ENABLE_
  (void)maskSize;

  for(it = table->entries; it < table->entries + table->size; it++)
    it->ttl = 0;
}

/**************************************************************************//**
  \brief Check for record existence in table, returns status

  \param[in] table - pointer to allocated table
  \param[in] address, seqNumber - record to search for or to add if not found

  \return true  - Record exists
          false - Record doesnt exist
 ******************************************************************************/
bool SYS_DuplicateTableEntryExists(SYS_DuplicateTable_t *table,
    uint16_t address, uint8_t seqNumber)
{
#ifdef _DUPLICATE_REJECTION_TABLE_BIT_MASK_ENABLE_
  bool entryExists = false;
  SYS_DuplicateTableEntry_t *iter;

  sysDuplicateTableUpdate(table);
  for (iter = table->entries; iter < table->entries + table->size; ++iter)
  {
    if ((iter->ttl) && (iter->address == address))
    {
      /* Excess of stored Counter over received one */
      const uint8_t excess = (int16_t)iter->seqNumber - seqNumber;
      /* If excess less than packet mask length, we assume received packet is older
       * than last remembered and we can check if this packet was already been received */
      if (excess < table->maskSize)
      {
        if (iter->mask & (1UL << excess))
          entryExists = true;
      }
      break;
    }
  }
  return entryExists;
#else // _DUPLICATE_REJECTION_TABLE_BIT_MASK_ENABLE_
  SYS_DuplicateTableEntry_t *iter;

  for (iter = table->entries; iter < table->entries + table->size; ++iter)
  {
    if (iter->ttl && (iter->address == address) && (iter->seqNumber == seqNumber))
      return true;
  }

  return false;
#endif // _DUPLICATE_REJECTION_TABLE_BIT_MASK_ENABLE_
}
/**************************************************************************//**
  \brief Search for record in table, add if not found.

  \param[in] table - pointer to allocated table
  \param[in] address, seqNumber - record to search for or to add if not found

  \return SYS_DUPLICATE_TABLE_ANSWER_FOUND - if duplicate packet is received
          SYS_DUPLICATE_TABLE_ANSWER_ADDED - new packet is received and
                                             information about the packet is added
                                             to the table.
          SYS_DUPLICATE_TABLE_ANSWER_FULL - there is no enough space in the table.
 ******************************************************************************/
SysDuplicateTableAnswer_t SYS_DuplicateTableCheck(SYS_DuplicateTable_t *table,
    uint16_t address, uint8_t seqNumber)
{

  SYS_DuplicateTableEntry_t *iter, *updatePosition = NULL;

#ifdef _DUPLICATE_REJECTION_TABLE_BIT_MASK_ENABLE_

  sysDuplicateTableUpdate(table);

  for (iter = table->entries; iter < table->entries + table->size; ++iter)
  {
    /* We have only one record in duplicate table per shortAdress */
    if (iter->ttl && iter->address == address)
    {
      /* Excess of stored apsCounter over received one */
      const uint8_t excess = (int16_t)iter->seqNumber - seqNumber;
      /* If excess less than packet mask length, we assume received packet is older
       * than last remembered and we can check if this packet was already been received */
      if (excess < table->maskSize)
      {
        if (iter->mask & (1UL << excess))
          return SYS_DUPLICATE_TABLE_ANSWER_FOUND;
        else
        {
          iter->mask |= 1UL << excess;
          return SYS_DUPLICATE_TABLE_ANSWER_ADDED;
        }
      }
      /* If excess more than mask length we shift our bit map forward to new packet */
      else
      {
        const uint8_t shiftLen = (int16_t)-excess;
        iter->seqNumber = seqNumber;
        iter->mask = (shiftLen < table->maskSize) ? (iter->mask << shiftLen) : 0UL ;
        iter->mask |= 1UL;
        iter->ttl = table->maxTTL;
        return SYS_DUPLICATE_TABLE_ANSWER_ADDED;
      }
    }
    /* Search for oldest record in table. */
    if (!updatePosition || iter->ttl < updatePosition->ttl)
      updatePosition = iter;
  }

  if (!table->removeOldest && (!updatePosition || updatePosition->ttl))
      return SYS_DUPLICATE_TABLE_ANSWER_FULL;

  /* If apsDuplicateRejectionTableSize is zero updatePosition can be NULL */
  if (updatePosition)
  {
    /* Add or update record. */
    updatePosition->address = address;
    updatePosition->seqNumber = seqNumber;
    updatePosition->ttl = table->maxTTL;
    updatePosition->mask = 1UL;
  }

  return SYS_DUPLICATE_TABLE_ANSWER_ADDED;
#else // _DUPLICATE_REJECTION_TABLE_BIT_MASK_ENABLE_

  sysDuplicateTableUpdate(table);

  for (iter = table->entries; iter < table->entries + table->size; ++iter)
  {
    if (iter->ttl && (iter->address == address) && (iter->seqNumber == seqNumber))
      return SYS_DUPLICATE_TABLE_ANSWER_FOUND;
    /* Search for oldest record in table. */
    if (!updatePosition || iter->ttl < updatePosition->ttl)
      updatePosition = iter;
  }

  if (!table->removeOldest && (!updatePosition || updatePosition->ttl))
    return SYS_DUPLICATE_TABLE_ANSWER_FULL;

  if (updatePosition)
  {
    updatePosition->address   = address;
    updatePosition->seqNumber = seqNumber;
    updatePosition->ttl       = table->maxTTL;
  }

  return SYS_DUPLICATE_TABLE_ANSWER_ADDED;

#endif // _DUPLICATE_REJECTION_TABLE_BIT_MASK_ENABLE_
}

/**************************************************************************//**
  \brief Clear info about given transaction from the rejection table.

  \param[in] table - pointer to allocated table
  \param[in] address, seqNumber - record to search for or to add if not found

  \return None.
 ******************************************************************************/
void SYS_DuplicateTableClear(SYS_DuplicateTable_t *table, uint16_t address,
  uint8_t seqNumber)
{
#ifdef _DUPLICATE_REJECTION_TABLE_BIT_MASK_ENABLE_
  SYS_DuplicateTableEntry_t *iter;

  for (iter = table->entries; iter < table->entries + table->size; ++iter)
  {
    if (iter->ttl && iter->address == address)
    {
      const uint8_t excess = (int16_t)iter->seqNumber - seqNumber;

      if (excess < table->maskSize)
      {
        iter->mask &= ~(1UL << excess);
        if (!iter->mask)
          iter->ttl = 0U;
      }
    }
  }
#else // _DUPLICATE_REJECTION_TABLE_BIT_MASK_ENABLE_
  SYS_DuplicateTableEntry_t *iter;

  for (iter = table->entries; iter < table->entries + table->size; ++iter)
  {
    if (iter->ttl && (iter->address == address) && (iter->seqNumber == seqNumber))
      iter->ttl = 0U;
  }
#endif // _DUPLICATE_REJECTION_TABLE_BIT_MASK_ENABLE_
}

/**************************************************************************//**
  \brief Update entries ttl.

  \param[in] table - able - table to work on.
  \return None.
 ******************************************************************************/
static void sysDuplicateTableUpdate(SYS_DuplicateTable_t *table)
{
  SYS_DuplicateTableEntry_t *it;
  uint32_t time = HAL_GetSystemTime();
  uint8_t diff = (uint8_t)MIN(((time - table->lastStamp)/ table->agingPeriod), UINT8_MAX);

  if(diff == 0)
    return;

  for(it = table->entries; it < table->entries + table->size; it++)
    it->ttl -= MIN(diff, it->ttl);

  table->lastStamp = time;
}

/** eof sysDuplicateTable.c */