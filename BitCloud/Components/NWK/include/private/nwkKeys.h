/**************************************************************************//**
  \file nwkKeys.h

  \brief NWK keys internal structure

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2012-12-19 A. Razinkov - Extracted from nwkIB.h.
   Last change:
    $Id: nwkKeys.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#if !defined _NWK_KEYS_H
#define _NWK_KEYS_H

#if defined _SECURITY_
/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <nwkSecurity.h>

/******************************************************************************
                               Types section
 ******************************************************************************/
/** NWK keys service structure */
typedef struct _NWK_Keys_t
{
  /** Total amount of NWK security keys. */
  NwkKeyAmount_t keyAmount;
  /** Set of network security material descriptors capable of maintaining
   * an active and alternate network key. */
  NWK_SecurityKey_t *key;
} NWK_Keys_t;

/******************************************************************************
                               Prototypes section
 ******************************************************************************/

/**************************************************************************//**
  \brief Get key index by key sequence number.

  \param[in] seqNum - key sequence number.
  \return Index of key entry or NWK_INVALID_KEY_INDEX if entry isn't found.
 ******************************************************************************/
NWK_PRIVATE NwkKeyIndex_t nwkFindKeyIndex(const NWK_KeySeqNum_t seqNum);

#endif /* _SECURITY_ */
#endif /* _NWK_KEYS_H */
/** eof nwkKeys.h */

