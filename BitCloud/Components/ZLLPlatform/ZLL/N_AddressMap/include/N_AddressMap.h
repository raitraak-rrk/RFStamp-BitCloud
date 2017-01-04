/*********************************************************************************************//**
\file  

\brief 

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal

$Id: N_AddressMap.h 24615 2013-02-27 05:14:24Z arazinkov $

***************************************************************************************************/

#ifndef N_ADDRESSMAP_H
#define N_ADDRESSMAP_H

/***************************************************************************************************
* INCLUDE FILES
***************************************************************************************************/

#include "N_Address.h"
#include "N_Types.h"

/***************************************************************************************************
* C++ DECLARATION WRAPPER
***************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************************************
* EXPORTED TYPES
***************************************************************************************************/


/***************************************************************************************************
* EXPORTED MACROS AND CONSTANTS
***************************************************************************************************/


/***************************************************************************************************
* EXPORTED FUNCTIONS
***************************************************************************************************/
/** Look up the index and extended address of a device by network address in the address map
    \param networkAddress [in] The network address of that device
    \param pExtendedAddress [out] The extended address of that device
    \returns TRUE if the entry exists, otherwise FALSE

    \note The out parameters of this function should not be used if the return value is FALSE
*/
bool N_AddressMap_LookupByNetworkAddress(uint16_t networkAddress, N_Address_Extended_t* pExtendedAddress);

/** Look up the index and network address of a device by the extended address in the address map
    \param pExtendedAddress [in] The extended address of that device
    \param pNetworkAddress [out] The network address of that device
    \returns TRUE if the entry exists, otherwise FALSE

    \note The out parameters of this function should not be used if the return value is FALSE
*/
bool N_AddressMap_LookupByExtendedAddress(N_Address_Extended_t* pExtendedAddress, uint16_t* pNetworkAddress);

/** Update the address map with these values or create a new entry if it does not exist
    \param networkAddress [in] The network address of that device
    \param pExtendedAddress [in] The extended address of that device
    \returns TRUE if the entry exists, otherwise FALSE

    \note This function will automatically add a reference
    \note The out parameters of this function should not be used if the return value is FALSE
*/
bool N_AddressMap_UpdateNetworkAddress(uint16_t networkAddress, N_Address_Extended_t* pExtendedAddress);

/***************************************************************************************************
* END OF C++ DECLARATION WRAPPER
***************************************************************************************************/

#ifdef __cplusplus
}
#endif

#endif // N_ADDRESSMAP_H
