/**************************************************************************//**
  \file nwkInCounterSet.h

  \brief The access interface to the incoming frame counters set.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2013-04-22  Sergey Dmitriev - Created.
   Last change:
    $Id:
 ******************************************************************************/
 #if !defined _NWK_IN_COUNTER_SET_H && defined _SECURITY_ && defined _NWK_IN_FRAME_COUNTERS_
 #define _NWK_IN_COUNTER_SET_H

/******************************************************************************
                                Includes section
 ******************************************************************************/
#include <nwkCommon.h>
#include <nwkSystem.h>
#include <nwkSecurity.h>

/******************************************************************************
                        Definitons section.
******************************************************************************/
#define NWK_INVALID_IN_FRAME_COUNTER_VALUE CCPU_TO_LE32(UINT32_MAX)
#define FREE_ENTRY_MARK 0x00ULL

/******************************************************************************
                                 Types section
 ******************************************************************************/
typedef enum
{
  IN_FRAME_COUNTER_SEARCH_STATUS_SUCCESS,
  IN_FRAME_COUNTER_SEARCH_STATUS_KEY_SEQ_INVALID,
  IN_FRAME_COUNTER_SEARCH_STATUS_NO_RECORD
} InFrameCounterSetSearchStatus_t;

/******************************************************************************
                               Prototypes section
 ******************************************************************************/

/**************************************************************************//**
  \brief Find entry in the set of incoming frame counters with given parameters.

  \param[in]  seqNum - key sequence number.
  \param[in]  srcExtAddr - pointer to IEEE extended 64-bit address.
  \param[out] entry - pointer to inFrameCounterSet entry.

  \return Status of search for entry
 ******************************************************************************/
NWK_PRIVATE InFrameCounterSetSearchStatus_t findInFrameCounter(const NWK_KeySeqNum_t seqNum,
  const ExtAddr_t *const srcExtAddr, NWK_InFrameCounterEntry_t **entry);

/**************************************************************************//**
  \brief Validate given counter value.

  \param[in] seqNum - key sequence number.
  \param[in] srcExtAddr - pointer to IEEE extended 64-bit address.
  \param[in] counter - pointer to value of counter to compare with

  \return 'true' if given value of counter is valid otherwise 'false'
 ******************************************************************************/
NWK_PRIVATE bool nwkVerifyInFrameCounter(const NWK_KeySeqNum_t seqNum,
  const ExtAddr_t *const srcExtAddr, const NwkInFrameCounter_t *counter);

/**************************************************************************//**
  \brief Remove the entry with given address from the incoming frame counter set.

  \param[in] seqNum - key sequence number.
  \param[in] srcExtAddr - pointer to IEEE extended 64-bit address.

  \return 'true' if incoming frame counter is added otherwise 'false'.
 ******************************************************************************/
NWK_PRIVATE bool nwkRemoveInFrameCounterByExtAddr(const NwkKeyIndex_t keyIndex,
  const ExtAddr_t *const srcExtAddr);

/**************************************************************************//**
  \brief Add new counter in the incoming frame counter set

  \param[in] seqNum - key sequence number.
  \param[in] srcExtAddr - pointer to IEEE extended 64-bit address.
  \param[in] counter - a new value of the incoming frame counter.

  \return 'true' if incoming frame counter is added otherwise 'false'.
 ******************************************************************************/
NWK_PRIVATE bool nwkAddInFrameCounter(const NWK_KeySeqNum_t seqNum,
  const ExtAddr_t *const srcExtAddr, const NwkInFrameCounter_t *counter);

/**************************************************************************//**
  \brief Update the incoming frame counter by key sequence number.

  \param[in] seqNum - key sequence number.
  \param[in] srcExtAddr - pointer to IEEE extended 64-bit address.
  \param[in] counter - a new value of the incoming frame counter.

  \return 'true' if incoming frame counter is updated otherwise 'false'.
 ******************************************************************************/
NWK_PRIVATE bool nwkUpdateInFrameCounter(const NWK_KeySeqNum_t seqNum,
  const ExtAddr_t *const srcExtAddr, const NwkInFrameCounter_t *counter);

/**************************************************************************//**
  \brief Initialize incoming frame counters table for given key sequence number.

  \param[in] keySeqNum - key sequence number.

  \return true if incoming counters set is initialized properly,
    false - otherwise.
 ******************************************************************************/
NWK_PRIVATE bool nwkInitInCounterSet(const NWK_KeySeqNum_t keySeqNum);

/**************************************************************************//**
  \brief Reset all incoming frame counters.
 ******************************************************************************/
NWK_PRIVATE void nwkResetInCounterSet(void);

 #endif /* !defined _NWK_IN_COUNTER_SET_H && defined _SECURITY_ && defined _NWK_IN_FRAME_COUNTERS_ */