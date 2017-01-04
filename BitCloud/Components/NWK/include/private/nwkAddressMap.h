/**************************************************************************//**
  \file nwkAddressMap.h

  \brief The access interface to the address map..

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2010-07-04  Max Gekk - Created.
   Last change:
    $Id: nwkAddressMap.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#if !defined _NWK_ADDRESS_MAP_H
#define _NWK_ADDRESS_MAP_H

/******************************************************************************
                                Includes section
 ******************************************************************************/
#include <appFramework.h>
#include <nwkSystem.h>
#include <nwkCommon.h>

/******************************************************************************
                                 Types section
 ******************************************************************************/
/** Type of size of the address map. */
typedef uint16_t NwkSizeOfAddressMap_t;

/** Type of entry of the address map. */
typedef struct _NwkAddressMapEntry_t
{
  /** The entry is busy (true) or not (false). */
  bool busy               : 1;
  /** Conflict of short addresses is detected (true) or not (false). */
  bool conflict           : 1;
  NwkBitField_t reserved  : 6;
  /** A network address (16-bit). */
  ShortAddr_t shortAddr;
  /** IEEE address (64-bit). */
  ExtAddr_t extAddr;
} NwkAddressMapEntry_t;

/** Type of the address map. */
typedef struct _NwkAddressMap_t
{
  /** Array of address map entries. */
  NwkAddressMapEntry_t *table;
  /** Pointer to memory area after last entry of the address map. */
  NwkAddressMapEntry_t *end;
  /** Current position for a new entry. */
  NwkAddressMapEntry_t *current;
} NwkAddressMap_t;

/******************************************************************************
                               Prototypes section
 ******************************************************************************/
/**************************************************************************//**
  \brief Add a short and extended addresses in the address map.

  \param[in] shortAddr - a short address.
  \param[in] extAddr - an extended address.
  \param[in] isConflict - 'true' if shortAddr conflict with other short address
                          in the network otherwise 'false'.
  \return None.
 ******************************************************************************/
NWK_PRIVATE void nwkAddToAddressMap(const ShortAddr_t shortAddr,
  const ExtAddr_t extAddr, const bool isConflict);

/**************************************************************************//**
  \brief Find entry in the address map by IEEE address.

  \param[in] extAddr - pointer to IEEE extended 64-bit address.
  \return Pointer to an entry in the address map.
 ******************************************************************************/
NWK_PRIVATE NwkAddressMapEntry_t*
  nwkFindAddressMapEntryByExtAddr(const ExtAddr_t *const extAddr);

/**************************************************************************//**
  \brief Find entry in the address map by short address.

  \param[in] shortAddr - a network short address.
  \return Pointer to an entry in the address map.
 ******************************************************************************/
NWK_PRIVATE NwkAddressMapEntry_t*
  nwkFindAddressMapEntryByShortAddr(const ShortAddr_t shortAddr);

#if defined _RESOLVE_ADDR_CONFLICT_
/**************************************************************************//**
  \brief Find entry in the address map with conflicted short address.

  \return Pointer to an entry in the address map.
 ******************************************************************************/
NWK_PRIVATE NwkAddressMapEntry_t* nwkFindAddrMapConflictEntry(void);
#endif /* _RESOLVE_ADDR_CONFLICT_ */

/**************************************************************************//**
  \brief Get maximum size of NWK address map.
 ******************************************************************************/
NWK_PRIVATE NwkSizeOfAddressMap_t nwkAddrMapSize(void);

/**************************************************************************//**
  \brief Reset the address map.
 ******************************************************************************/
NWK_PRIVATE void nwkResetAddressMap(void);

#endif /* _NWK_ADDRESS_MAP_H */
/** eof nwkAddressMap.h */

