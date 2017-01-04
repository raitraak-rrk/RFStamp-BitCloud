/*********************************************************************************************//**
\file  

\brief 

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal

$Id: N_AddressManager.c 24615 2013-02-27 05:14:24Z arazinkov $

***************************************************************************************************/

/***************************************************************************************************
* LOCAL INCLUDE FILES
***************************************************************************************************/

#include "N_AddressManager_Bindings.h"
#include "N_AddressManager.h"
#include "N_AddressManager_Init.h"

/***************************************************************************************************
* EXTERNAL INCLUDE FILES
***************************************************************************************************/

#include "S_Nv.h"
#include "S_NvInternal.h"
#include "S_Nv_Stack_Ids.h"

#include "N_ErrH.h"
#include "N_Util.h"
#include "N_Log.h"
#include "N_Types.h"

#include <sysEvents.h>
#include <macAddr.h>
#include <nwkCommon.h>

/***************************************************************************************************
* LOCAL MACROS AND CONSTANTS
***************************************************************************************************/

#define COMPID "N_AddressManager"

#define MIN_DEALER_ADDRESS_COUNT        50u
#define MIN_DEALER_GROUP_COUNT          50u

#define VALID_ADDRESS_MIN               0x0001u
#define VALID_ADDRESS_MAX               0xFFF7u
#define VALID_ADDRESS_COUNT             ((VALID_ADDRESS_MAX - VALID_ADDRESS_MIN) + 1u)

#define VALID_GROUP_MIN                 0x0001u
#define VALID_GROUP_MAX                 0xFEFFu
#define VALID_GROUP_COUNT               ((VALID_GROUP_MAX - VALID_GROUP_MIN) + 1u)

/***************************************************************************************************
* LOCAL TYPES
***************************************************************************************************/

typedef struct N_AddressManager_FreeRanges_t
{
    uint16_t addressFirst;
    uint16_t addressCount;
    uint16_t groupFirst;
    uint16_t groupCount;
} N_AddressManager_FreeRanges_t;

typedef struct N_AddressManager_GroupValidRanges_t
{
    uint16_t groupAddrStart;
    uint16_t groupAddrEnd;
} N_AddressManager_GroupValidRanges_t;

/***************************************************************************************************
* FUNCTIONS PROTOTYPES
***************************************************************************************************/
static void nAddressManagerObserver(SYS_EventId_t eventId, SYS_EventData_t data);
static void nAddressManagerAllocNwkAddrForAssociatingDevice(ShortAddr_t *const address);

/***************************************************************************************************
* LOCAL VARIABLES
***************************************************************************************************/

static N_AddressManager_FreeRanges_t s_freeRanges =
{
  VALID_ADDRESS_MIN,
  VALID_ADDRESS_COUNT,
  VALID_GROUP_MIN,
  VALID_GROUP_COUNT
};

static N_AddressManager_GroupValidRanges_t s_groupValidRanges =
{
  VALID_GROUP_MIN,
  VALID_GROUP_MIN + VALID_GROUP_COUNT - 1u
};
/* BitCloud events receiver */
static SYS_EventReceiver_t bcEventReceiver = { .func = nAddressManagerObserver};

/***************************************************************************************************
* LOCAL FUNCTIONS
***************************************************************************************************/

static void WriteFreeRanges(void)
{
  // flash access
  S_Nv_ReturnValue_t ret = S_Nv_Write(FREE_RANGES_STORAGE_ID, 0u, sizeof(s_freeRanges), &s_freeRanges);
  N_ERRH_ASSERT_FATAL(ret == S_Nv_ReturnValue_Ok);
}

static bool IsValidRandomAddress(uint16_t address)
{
    // 0x0000 is not valid because it is the ZigBee Coordinator address.
    // 0x0001..0x000F are not valid because they are the first addresses
    // allocated from the pool of the first address assignment capable device.
    if ( (address <= 0x000Fu) )
    {
        return FALSE;
    }
    // 0x7FFC..0x800F are not valid because they are the first addresses
    // allocated from the pool of the second address assignment capable device.
    if ( (address >= 0x7FFCu) && (address <= 0x800Fu) )
    {
        return FALSE;
    }
    // 0xFFF8..0xFFFF are not valid because they are ZigBee Broadcast addresses
    if ( (address >= 0xFFF8u) )
    {
        return FALSE;
    }

    return TRUE;
}

static bool IsValidRandomGroup(uint16_t groupFirst, uint8_t numGroups)
{
    for ( uint8_t i = 0u; i < numGroups; i++ )
    {
        uint16_t group = groupFirst + i;

        // 0x0000 is not valid because it is not a valid ZigBee group id
        // 0x0001..0x000F are not valid because they are the first groups
        // allocated from the pool of the first address assignment capable device.
        if ( (group <= 0x000Fu) )
        {
            return FALSE;
        }
        // 0x7F80..0x7F8F are not valid because they are the first groups
        // allocated from the pool of the second address assignment capable device.
        if ( (group >= 0x7F80u) && (group <= 0x7F8Fu) )
        {
            return FALSE;
        }
        // 0xFF00..0xFFFF are not valid because they are reserved by for future
        // specification enhancements.
        if ( (group >= 0xFF00u) )
        {
            return FALSE;
        }
    }

    return TRUE;
}

static void AllocateRandomAddress(uint16_t* pAddress)
{
    do
    {
        *pAddress = N_Util_Random();
    }
    while ( !IsValidRandomAddress(*pAddress) );
}

static void AllocateRandomGroups(uint8_t numGroups, uint16_t* pGroupIdFirst)
{
    if ( numGroups > 0u )
    {
        do
        {
            *pGroupIdFirst = N_Util_Random();
        }
        while ( !IsValidRandomGroup(*pGroupIdFirst, numGroups) );
    }
}

/***************************************************************************************************
* EXPORTED FUNCTIONS
***************************************************************************************************/

N_AddressManager_Status_t N_AddressManager_AllocateAddresses_Impl(
    uint8_t numGroups,
    uint16_t* pAddr, uint16_t* pGroupIdFirst)
{
    N_AddressManager_Status_t status;

    if ( (s_freeRanges.addressCount >= 1u) &&
         (s_freeRanges.groupCount >= numGroups) )
    {
        *pAddr = s_freeRanges.addressFirst;
        s_freeRanges.addressFirst += 1u;
        s_freeRanges.addressCount -= 1u;

        *pGroupIdFirst = s_freeRanges.groupFirst;
        s_freeRanges.groupFirst += numGroups;
        s_freeRanges.groupCount -= numGroups;

        WriteFreeRanges();

        status = N_AddressManager_Status_Success;
    }
    else
    {
        // Insufficient addresses, so return random values
        AllocateRandomAddress(pAddr);
        AllocateRandomGroups(numGroups, pGroupIdFirst);

        status = N_AddressManager_Status_RandomValue;
    }

    return status;
}

N_AddressManager_Status_t N_AddressManager_AllocateAddressesForDealer_Impl(
    uint8_t numGroups,
    uint16_t* pAddr, uint16_t* pGroupIdFirst,
    uint16_t* pDealAddrFirst, uint16_t* pDealAddrLast, uint16_t* pDealGroupFirst, uint16_t* pDealGroupLast)
{
    N_AddressManager_Status_t status;

    if ( (s_freeRanges.addressCount >= ((2u * MIN_DEALER_ADDRESS_COUNT) + 1u)) &&
         (s_freeRanges.groupCount >= ((2u * MIN_DEALER_GROUP_COUNT) + (uint16_t) numGroups)) )
    {
        // Calculate the address range that is available for distribution to a dealer device
        uint16_t addressRangeHalf = (s_freeRanges.addressCount - 1u) / 2u;
        uint16_t groupRangeHalf = (s_freeRanges.groupCount - numGroups) / 2u;

        // Allocate the addresses
        *pAddr = s_freeRanges.addressFirst;
        *pDealAddrLast = (s_freeRanges.addressFirst + s_freeRanges.addressCount) - 1u;
        s_freeRanges.addressFirst += 1u;
        s_freeRanges.addressCount = addressRangeHalf;
        *pDealAddrFirst = (s_freeRanges.addressFirst + s_freeRanges.addressCount);

        // Allocate the group ids
        *pGroupIdFirst = s_freeRanges.groupFirst;
        *pDealGroupLast = (s_freeRanges.groupFirst + s_freeRanges.groupCount) - 1u;
        s_freeRanges.groupFirst += numGroups;
        s_freeRanges.groupCount = groupRangeHalf;
        s_groupValidRanges.groupAddrEnd = s_freeRanges.groupFirst + s_freeRanges.groupCount - 1u;
        *pDealGroupFirst = (s_freeRanges.groupFirst + s_freeRanges.groupCount);

        WriteFreeRanges();

        status = N_AddressManager_Status_Success;
    }
    else
    {
        // Insufficient addresses, so return random values
        AllocateRandomAddress(pAddr);
        AllocateRandomGroups(numGroups, pGroupIdFirst);

        // Return empty dealer ranges
        *pDealAddrFirst = 0u;
        *pDealAddrLast = 0u;
        *pDealGroupFirst = 0u;
        *pDealGroupLast = 0u;

        status = N_AddressManager_Status_RandomValue;
    }

    return status;
}

N_AddressManager_Status_t N_AddressManager_AssignAddressRange_Impl(
    uint16_t dealAddrFirst, uint16_t dealAddrLast, uint16_t dealGroupFirst, uint16_t dealGroupLast)
{
    N_AddressManager_Status_t status;

    // setting all zero's is a special value for empty ranges
    if ( (dealAddrFirst == 0u) && (dealAddrLast == 0u) && (dealGroupFirst == 0u) && (dealGroupLast == 0u) )
    {
        s_freeRanges.addressFirst = 0u;
        s_freeRanges.addressCount = 0u;
        s_freeRanges.groupFirst = 0u;
        s_freeRanges.groupCount = 0u;
        s_groupValidRanges.groupAddrStart = VALID_GROUP_MIN;
        s_groupValidRanges.groupAddrEnd = VALID_GROUP_MAX;

        WriteFreeRanges();

        status = N_AddressManager_Status_Success;
    }
    else if ( (dealAddrFirst > dealAddrLast) ||
              (dealGroupFirst > dealGroupLast) ||
              (dealAddrFirst < VALID_ADDRESS_MIN) ||
              (dealAddrLast > VALID_ADDRESS_MAX) ||
              (dealGroupFirst < VALID_GROUP_MIN) ||
              (dealGroupLast > VALID_GROUP_MAX) )
    {
        status = N_AddressManager_Status_IllegalParameters;
    }
    else
    {
        s_freeRanges.addressFirst = dealAddrFirst;
        s_freeRanges.addressCount = (dealAddrLast - dealAddrFirst) + 1u;
        s_freeRanges.groupFirst = dealGroupFirst;
        s_freeRanges.groupCount = (dealGroupLast - dealGroupFirst) + 1u;
        s_groupValidRanges.groupAddrStart = s_freeRanges.groupFirst;
        s_groupValidRanges.groupAddrEnd = s_freeRanges.groupFirst + s_freeRanges.groupCount - 1u;
        WriteFreeRanges();

        status = N_AddressManager_Status_Success;
    }

    return status;
}

bool N_AddressManager_HasFreeGroupIds_Impl(void)
{
    return N_UTIL_BOOL(s_freeRanges.groupCount > 0u);
}

N_AddressManager_Status_t N_AddressManager_AllocateGroupId_Impl(uint16_t* pGroupId)
{
    N_AddressManager_Status_t status;

    if ( N_AddressManager_HasFreeGroupIds() )
    {
        *pGroupId = s_freeRanges.groupFirst;
        s_freeRanges.groupFirst += 1u;
        s_freeRanges.groupCount -= 1u;

        WriteFreeRanges();

        status = N_AddressManager_Status_Success;
    }
    else
    {
        AllocateRandomGroups(1u, pGroupId);

        status = N_AddressManager_Status_RandomValue;
    }
    return status;
}

/** Initialises the component
*/
void N_AddressManager_Init(void)
{
    S_Nv_ReturnValue_t ret;
    // flash access
    ret = S_Nv_ItemInit(FREE_RANGES_STORAGE_ID, sizeof(s_freeRanges), &s_freeRanges);
    N_ERRH_ASSERT_FATAL( (ret == S_Nv_ReturnValue_DidNotExist) || (ret == S_Nv_ReturnValue_Ok) );

    if ((s_freeRanges.addressFirst == 0u) && (s_freeRanges.addressCount == 0u) &&
        (s_freeRanges.groupFirst == 0u) && (s_freeRanges.groupCount == 0u))
    {
        // All zeros is a special value for empty ranges.
    }
    else
    {
        uint32_t lastAddress = ((uint32_t) s_freeRanges.addressFirst + (uint32_t) s_freeRanges.addressCount) - 1uL;
        uint32_t lastGroup = ((uint32_t) s_freeRanges.groupFirst + (uint32_t) s_freeRanges.groupCount) - 1uL;

        if ((s_freeRanges.addressFirst < VALID_ADDRESS_MIN) ||
            (lastAddress > VALID_ADDRESS_MAX) ||
            (s_freeRanges.groupFirst < VALID_GROUP_MIN) ||
            (lastGroup > VALID_GROUP_MAX))
        {
            // invalid values, reset to empty ranges
            N_LOG_NONFATAL();

            s_freeRanges.addressFirst = 0u;
            s_freeRanges.addressCount = 0u;
            s_freeRanges.groupFirst = 0u;
            s_freeRanges.groupCount = 0u;
            s_groupValidRanges.groupAddrStart = VALID_GROUP_MIN;
            s_groupValidRanges.groupAddrEnd = VALID_GROUP_MAX;

            WriteFreeRanges();
        }
    }

    /* Subscribe to BitCloud events */
    SYS_SubscribeToEvent(BC_EVENT_ACCESS_REQUEST, &bcEventReceiver);
}

/** BitCloud events observer routine

    \param eventId Event ID
    \param data Data associated with event occured
*/
static void nAddressManagerObserver(SYS_EventId_t eventId, SYS_EventData_t data)
{
  if (BC_EVENT_ACCESS_REQUEST == eventId)
  {
    BcAccessReq_t *const accessReq = (BcAccessReq_t*)data;

    if (BC_CHILD_NWK_ADDR_ALLOC_ACTION == accessReq->action)
    {
      ShortAddr_t *const addrRequested = (ShortAddr_t *)accessReq->context;

      /* Prohibit further processing by NWK */
      accessReq->denied = true;
      nAddressManagerAllocNwkAddrForAssociatingDevice(addrRequested);
    }
  }
}

/** Allocates NWK address for device Associating

    \param address Pointer to NWK address proposed by joining device. Could be
                   modified by this routine depending on situation.
*/
static void nAddressManagerAllocNwkAddrForAssociatingDevice(ShortAddr_t *const address)
{
  /* Device is requesting a NWK address */
  if (NWK_NO_SHORT_ADDR == *address)
  {
    /* We are in ZLL network - try to allocate corresponding NWK address */
    if (s_freeRanges.addressCount)
    {
      *address = s_freeRanges.addressFirst;
      s_freeRanges.addressFirst++;
      s_freeRanges.addressCount--;

      WriteFreeRanges();
      return;
    }
  }
  /* Return proposed address, if one is valid */
  else if ((VALID_ADDRESS_MIN <= *address) && (*address <= VALID_ADDRESS_MAX))
    return;

  /* Generate random address */
  AllocateRandomAddress(address);
}
/** Get the current valid FreeGroupIdRangeBegin and idEnd possible

   \param address pointers to idBegin and idEnd
*/
void N_AddressManager_GetFreeGroupIdRange_Impl(uint16_t* idBegin, uint16_t* idEnd)
{
  if(NULL != idBegin)
    memcpy(idBegin, &s_groupValidRanges.groupAddrStart, sizeof(uint16_t));

  if(NULL!= idEnd)
    memcpy(idEnd, &s_groupValidRanges.groupAddrEnd, sizeof(uint16_t));
}
