/*********************************************************************************************//**
\file  

\brief 

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal

$Id: N_AddressManager.h 24615 2013-02-27 05:14:24Z arazinkov $

***************************************************************************************************/

#ifndef N_ADDRESSMANAGER_H
#define N_ADDRESSMANAGER_H

/***************************************************************************************************
* INCLUDE FILES
***************************************************************************************************/

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

typedef enum N_AddressManager_Status_t
{
    N_AddressManager_Status_Success,                    //< Returned values from the pool
    N_AddressManager_Status_RandomValue,                //< Returned random values
    N_AddressManager_Status_IllegalParameters,          //< The input parameters were incorrect
} N_AddressManager_Status_t;

/***************************************************************************************************
* EXPORTED FUNCTIONS
***************************************************************************************************/

/** Sets an address range and a group identifier range (obtained from another device)
    \param dealAddrFirst [in] The first address that can be dealt
    \param dealAddrLast [in] The last address that can be dealt
    \param dealGroupFirst [in] The first group identifier that can be dealt
    \param dealGroupLast [in] The last group identifier that can be dealt
    \returns Indicates success

    This function must only be called with address ranges obtained from another device's
    Address Manager (using AllocateAddressesForDealer).
    A virgin device automatically starts with a full address range.

    \note Call with all zeroes to make the address and group id pool of the
          device empty. The device is only capable of handing out random
          values after this
*/
N_AddressManager_Status_t N_AddressManager_AssignAddressRange(
    uint16_t dealAddrFirst, uint16_t dealAddrLast, uint16_t dealGroupFirst, uint16_t dealGroupLast);

/** Returns TRUE if there are group identifiers available
    \returns TRUE if group identifiers available in the pool, FALSE otherwise
*/
bool N_AddressManager_HasFreeGroupIds(void);

/** Allocates a group identifier for the caller
    \param *pGroupId [out] The group identifier allocated for the caller
    \returns Indication of success.
*/
N_AddressManager_Status_t N_AddressManager_AllocateGroupId(uint16_t* pGroupId);

/** Get the current valid FreeGroupIdRangeBegin and idEnd possible

   \param address pointers to idBegin and idEnd
*/
void N_AddressManager_GetFreeGroupIdRange_Impl(uint16_t* idBegin, uint16_t* idEnd);

/***************************************************************************************************
* PLATFORM INTERNAL INTERFACE
***************************************************************************************************/
/** \privatesection
*/

/** Allocates a short address and group identifiers for a non-assignment-capable device
    \param numGroups [in] The number of group identifiers to allocate for the device
    \param *pAddr [out] The short address allocated for the device itself
    \param *pGroupIdFirst [out] The start of the allocated group identifier range for the device
    \returns Indication of success.
*/
N_AddressManager_Status_t N_AddressManager_AllocateAddresses(
    uint8_t numGroups, uint16_t* pAddr, uint16_t* pGroupIdFirst);

/** Allocates a short address and group identifiers for an assignment-capable device, plus
    ranges of addresses and group identifiers for the Address Manager of that device
    \param numGroups [in] The number of group identifiers to allocate for the device, must be 1
    \param *pAddr [out] The short address allocated for the device itself
    \param *pGroupIdFirst [out] The start of the allocated group identifier range for the device
    \param *pDealAddrFirst [out] The first address for the Address Manager of the other device
    \param *pDealAddrLast [out] The last address for the Address Manager of the other device
    \param *pDealGroupFirst [out] The first group identifier for the Address Manager of the other device
    \param *pDealGroupLast [out] The last group identifier for the Address Manager of the other device
    \returns Indication of success.
    \note short addresses and group identifiers range must be kept the same for now
*/
N_AddressManager_Status_t N_AddressManager_AllocateAddressesForDealer(
    uint8_t numGroups, uint16_t* pAddr, uint16_t* pGroupIdFirst,
    uint16_t* pDealAddrFirst, uint16_t* pDealAddrLast, uint16_t* pDealGroupFirst, uint16_t* pDealGroupLast);


/***************************************************************************************************
* END OF C++ DECLARATION WRAPPER
***************************************************************************************************/

#ifdef __cplusplus
}
#endif

#endif // N_ADDRESSMANAGER_H
