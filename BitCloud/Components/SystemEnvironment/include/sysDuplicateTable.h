/**************************************************************************//**
  \file sysDuplicateTable.h

  \brief Common interface for BTT, APS Rejection Table and other tables.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2009-11-20 I.Vagulin - Created.
    2013-02-11 Max Gekk - Refactoring. The bit mask is extened up to 32-bits.
   Last change:
    $Id: sysDuplicateTable.h 24479 2013-02-11 12:08:31Z mgekk $
 ******************************************************************************/
#if !defined _SYS_DUPLICATE_TABLE_H
#define _SYS_DUPLICATE_TABLE_H

/******************************************************************************
                                Includes section
 ******************************************************************************/
#include <sysTypes.h>

/******************************************************************************
                                 Types section
 ******************************************************************************/
/** Enumeration of results of duplicate checking. */
typedef enum _SysDuplicateTableAnswer_t
{
  /** A packet with given sequence number has already been received. */
  SYS_DUPLICATE_TABLE_ANSWER_FOUND,
  /** The received packet is not duplicate and it is added to the table. */
  SYS_DUPLICATE_TABLE_ANSWER_ADDED,
  /** Check can return Full only if removeOldest set to false */
  SYS_DUPLICATE_TABLE_ANSWER_FULL
} SysDuplicateTableAnswer_t;

/** Type of mask to track duplicates. */
typedef uint32_t SYS_DuplicateMask_t;

/** Type of a duplicate rejection entry. */
typedef struct _SYS_DuplicateTableEntry_t
{
  uint16_t address; /*!< Short address of node from which duplicates are tracked. */
  uint8_t seqNumber; /*!< Most recent sequence number which is received from the node. */
  uint8_t ttl; /*!< Current value of time-to-leave. */
#ifdef _DUPLICATE_REJECTION_TABLE_BIT_MASK_ENABLE_
  /* The mask indicates received packets from particular node. */
  SYS_DuplicateMask_t mask;
#endif // _DUPLICATE_REJECTION_TABLE_BIT_MASK_ENABLE_
} SYS_DuplicateTableEntry_t;

/* Type of structure which contains internal states of a rejection table. */
typedef struct _SYS_DuplicateTable_t
{
  /** The flag indicates that the oldest entry is removed if the table is full. */
  bool removeOldest:1;
  /* Current size of the table. */
  uint8_t size:7;
  /* A pointer to the duplicate rejection table from configServer. */
  SYS_DuplicateTableEntry_t *entries;
  /* Variable to track timeout and to age out an entry. */
  uint16_t agingPeriod;
  uint32_t lastStamp;
  uint8_t maxTTL;
#ifdef _DUPLICATE_REJECTION_TABLE_BIT_MASK_ENABLE_
  uint8_t maskSize; /*!<Counter value for duplicate entry. */
#endif // _DUPLICATE_REJECTION_TABLE_BIT_MASK_ENABLE_
} SYS_DuplicateTable_t ;

/******************************************************************************
                             Prototypes section
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
void SYS_DuplicateTableReset(SYS_DuplicateTable_t *table,
    SYS_DuplicateTableEntry_t *entries, uint8_t tableSize,
    uint16_t agingPeriod, uint8_t maxTTL, bool removeOldest,uint8_t maskSize);

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
    uint16_t address, uint8_t seqNumber);

/**************************************************************************//**
  \brief Clear info about given transaction from the rejection table.

  \param[in] table - pointer to allocated table
  \param[in] address, seqNumber - record to search for or to add if not found

  \return None.
 ******************************************************************************/
void SYS_DuplicateTableClear(SYS_DuplicateTable_t *table,
  uint16_t address, uint8_t seqNumber);

/**************************************************************************//**
  \brief Check for record existence in table, returns status

  \param[in] table - pointer to allocated table
  \param[in] address, seqNumber - record to search for or to add if not found

  \return true  - Record exists
          false - Record doesnt exist
 ******************************************************************************/
bool SYS_DuplicateTableEntryExists(SYS_DuplicateTable_t *table,
    uint16_t address, uint8_t seqNumber);

#endif /* _SYS_DUPLICATE_TABLE_H */
/** eof sysDuplicateTable.h */
