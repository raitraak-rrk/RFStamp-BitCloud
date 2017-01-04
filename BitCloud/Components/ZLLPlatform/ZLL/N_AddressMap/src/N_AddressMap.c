/***************************************************************************//**
  \file N_AddressMap.c

  \brief Implementation of API for the address map.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2013-01-23 Max Gekk - Created.
   Last change:
    $Id: N_AddressMap.c 24319 2013-01-28 10:18:19Z arazinkov $
 ******************************************************************************/
/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <nwk.h>

#include <N_ErrH.h>

#include <N_AddressMap.h>
#include <N_AddressMap_Bindings.h>

/******************************************************************************
                              Definitions section
 ******************************************************************************/
#define COMPID "N_AddressMap"

/******************************************************************************
                            Implementations section
 ******************************************************************************/
/** Look up the index and extended address of a device by network address in the address map
    \param networkAddress [in] The network address of that device
    \param pExtendedAddress [out] The extended address of that device
    \returns TRUE if the entry exists, otherwise FALSE

    \note The out parameters of this function should not be used if the return value is FALSE
*/
bool N_AddressMap_LookupByNetworkAddress_Impl(uint16_t networkAddress,
  N_Address_Extended_t* pExtendedAddress)
{
  const ExtAddr_t *const pFoundExtAddr = NWK_GetExtByShortAddress(networkAddress);
  const bool isFound = (NULL != pFoundExtAddr);

  if (isFound)
  {
    N_ERRH_ASSERT_FATAL( NULL != pExtendedAddress );
    memcpy(pExtendedAddress, pFoundExtAddr, sizeof(ExtAddr_t));
  }

  return isFound;
}

/** Look up the index and network address of a device by the extended address in the address map
    \param pExtendedAddress [in] The extended address of that device
    \param pNetworkAddress [out] The network address of that device
    \returns TRUE if the entry exists, otherwise FALSE

    \note The out parameters of this function should not be used if the return value is FALSE
*/
bool N_AddressMap_LookupByExtendedAddress_Impl(N_Address_Extended_t* pExtendedAddress,
  uint16_t* pNetworkAddress)
{
  const ShortAddr_t *const pFoundShortAddr = NWK_GetShortByExtAddress((void *)pExtendedAddress);
  const bool isFound = (NULL != pFoundShortAddr);

  N_ERRH_ASSERT_FATAL( NULL != pExtendedAddress );
  if (isFound)
  {
    N_ERRH_ASSERT_FATAL( NULL != pNetworkAddress );
    memcpy(pNetworkAddress, pFoundShortAddr, sizeof(ShortAddr_t));
  }

  return isFound;
}

/** Update the address map with these values or create a new entry if it does not exist
    \param networkAddress [in] The network address of that device
    \param pExtendedAddress [in] The extended address of that device
    \returns TRUE if the entry exists, otherwise FALSE

    \note This function will automatically add a reference
    \note The out parameters of this function should not be used if the return value is FALSE
*/
bool N_AddressMap_UpdateNetworkAddress_Impl(uint16_t networkAddress, N_Address_Extended_t* pExtendedAddress)
{
  ExtAddr_t extAddr;

  N_ERRH_ASSERT_FATAL( NULL != pExtendedAddress );
  memcpy(&extAddr, pExtendedAddress, sizeof(extAddr));

  return NWK_LinkShortAndExtAddress(networkAddress, extAddr);
}

/** eof N_AddressMap.c */
