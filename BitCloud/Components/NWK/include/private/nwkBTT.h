/**************************************************************************//**
  \file nwkBTT.h

  \brief Broadcast transaction table.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2007-09-01 V. Panov - Created.
    2008-11-17 M. Gekk - Correct clearing of record in BTT. In case of overflow
                       of the table function nwkCheckBTT returns an error.
                       BTT moved in ConfigServer.
    2009-04-29 M. Gekk - Refactoring.
    2009-11-24 Igor Vagulin - reimplemented using sysDuplicateTable.
   Last change:
    $Id: nwkBTT.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#if !defined _NWK_BTT_H
#define _NWK_BTT_H

/******************************************************************************
                                Includes section
 ******************************************************************************/
#include <nwkConfig.h>
#include <mac.h>
#include <sysDuplicateTable.h>
#include <sysTimer.h>

/******************************************************************************
                                 Types section
 ******************************************************************************/
typedef enum _NwkBTTTableAnswer_t
{
  /* Entry already exists with given source address and seq num */
  NWK_BTT_TABLE_ENTRY_ALREADY_EXISTS,
  /* Entry is added with given source address and seq num */
  NWK_BTT_TABLE_ENTRY_ADDED,
 /* New entry cannot be added. The table is already full */
  NWK_BTT_TABLE_FULL,
  /* Entry is not added for end device with RxOnWhenIdle false */
  NWK_BTT_NOT_SUPPORTED
} NwkBTTTableAnswer_t;

typedef struct _NWK_BTTTableEntry_t
{
  /* The 16-bit network address of the broadcast initiator.*/
  uint16_t address;
  /* The NWK layer sequence number of the initiator’s broadcast. */
  uint8_t seqNumber;
  /* Flag indicates whether the entry is active or not*/
  bool active;
  /* Current value of time-to-leave. A countdown timer indicating 
    the number of milli-seconds until this entry expires; the initial value
    is nwkNetworkBroadcastDeliveryTime. */
  uint32_t ttl;
} NWK_BTTTableEntry_t;

typedef struct _NWK_BTTTable_t
{
  /* Current size of the table. */
  uint8_t size;

  /* Holds the current Maximum Table index reached. */
  uint8_t currentMaxTableIndex;

  /* The timer to track ttl*/
  SYS_Timer_t  timer;

  /* A pointer to the duplicate rejection table from configServer. */
  NWK_BTTTableEntry_t *entries;
} NWK_BTTTable_t ;

/** Broadcast transaction table */
typedef struct _NwkBTT_t
{
  /** Pointer to BTT Table descriptor */
  NWK_BTTTable_t btt;
} NwkBTT_t;

/** Type of BTT size. */
typedef uint8_t NwkBttSize_t;

/******************************************************************************
                             Prototypes section
 ******************************************************************************/
/**************************************************************************//**
  \brief Reseting of the broadcast transaction table.
 ******************************************************************************/
NWK_PRIVATE void nwkResetBTT(void);

/**************************************************************************//**
  \brief Add new BTR to BTT with given address and given sequence number

  \param[in] sequenceNumber - a sequence number of broadcast packet.
  \return Status of a broadcast transaction record with shortAddr
         and sequenceNumber.
 ******************************************************************************/
NWK_PRIVATE NwkBTTTableAnswer_t nwkAddBTTEntry(const ShortAddr_t shortAddr,
  const uint8_t sequenceNumber);

/**************************************************************************//**
  \brief Check for record existence in table, returns status

  \param[in] address, seqNumber - record to search for or to add if not found

  \return true  - Record exists
          false - Record doesnt exist
 ******************************************************************************/
NWK_PRIVATE bool nwkBTTEntryExists(uint16_t address, uint8_t seqNumber);

/**************************************************************************//**
  \brief Check entry for short address and sequence number.

  \param[in] shortAddr - source address of broadcast packet.
  \param[in] sequenceNumber - a sequence number of broadcast packet.
 ******************************************************************************/
NWK_PRIVATE NwkBTTTableAnswer_t nwkCheckBTT(const ShortAddr_t shortAddr,
  const uint8_t sequenceNumber);

/**************************************************************************//**
  \brief Clear the existing entry in the broadcast transaction table.

  \param[in] shortAddr - a short address of broadcast packet,
  \param[in] sequenceNumber - a sequence number of broadcast packet.

  \return None.
 ******************************************************************************/
NWK_PRIVATE void nwkClearBTR(const ShortAddr_t shortAddr,
  const uint8_t sequenceNumber);

#ifdef UNIT_TESTING
/**************************************************************************//**
  \brief BTT Timer Expiry Callback.

  \param[in] None
 ******************************************************************************/
void nwkBTTTimerFired(void);
#endif

/**************************************************************************//**
  \brief Add new BTR to BTT with own address and given sequence number

  \param[in] sequenceNumber - a sequence number of broadcast packet.
  \return Status of a broadcast transaction record with shortAddr
       and sequenceNumber.
 ******************************************************************************/
#if !defined NWK_ALWAYS_REJECT_OWN_BROADCAST
NWK_PRIVATE NwkBTTTableAnswer_t nwkAddOwnBTR(const uint8_t sequenceNumber);
#else
#define nwkAddOwnBTR(sequenceNumber) (void)0
#endif /* NWK_ALWAYS_REJECT_OWN_BROADCAST */
#endif /* _NWK_BTT_H */
/** eof nwkBTT.h */

