/**************************************************************************//**
  \file nwkAddressResolv.h

  \brief Interface of address resolution.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2010-07-04 Max Gekk - Created.
   Last change:
    $Id: nwkAddressResolv.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#if !defined _NWK_ADDRESS_RESOLV_H
#define _NWK_ADDRESS_RESOLV_H

/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <appFramework.h>

/******************************************************************************
                              Prototypes section
 ******************************************************************************/
/**************************************************************************//**
  \brief Link short network and IEEE extended address.

  \param[in] shortAddr - a short address.
  \param[in] extAddr - an extended address.
  \return 'true' if short and extended addresses are linked successfully
    otherwise 'false'.
 ******************************************************************************/
bool NWK_LinkShortAndExtAddress(const ShortAddr_t shortAddr,
  const ExtAddr_t extAddr);

/**************************************************************************//**
  \brief Remove the entry with given address from the address map.

  \param[in] extAddr - pointer to a extended address.
  \return None.
 ******************************************************************************/
void NWK_RemoveFromAddressMap(const ExtAddr_t *const extAddr);

/**************************************************************************//**
  \brief Searching a extended address by the short address.

  \param[in] shortAddr - a short address.
  \return Pointer to the extended address in the address map.
          \c NULL if the extended address is not found.
 ******************************************************************************/
const ExtAddr_t* NWK_GetExtByShortAddress(const ShortAddr_t shortAddr);

/**************************************************************************//**
  \brief Searching a short address by the extended address.

  \param[in] extAddr - pointer to the extended address.
  \return Pointer to the short address in the address map.
          \c NULL if the short address is not found.
 ******************************************************************************/
const ShortAddr_t* NWK_GetShortByExtAddress(const ExtAddr_t *const extAddr);

#endif /* _NWK_ADDRESS_RESOLV_H */
/** eof nwkAddressResolv.h */

