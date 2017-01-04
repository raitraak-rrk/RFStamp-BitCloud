/*********************************************************************************************//**
\file  

\brief S_Nv External Flash Driver implementation

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal

$Id: S_Nv-External.c 24615 2013-02-27 05:14:24Z mahendranp $

***************************************************************************************************/
#if PDS_ENABLE_WEAR_LEVELING == 1
#ifdef _ENABLE_PERSISTENT_SERVER_
#ifdef PDS_USE_EXTERNAL_FLASH
/***************************************************************************************************
* LOCAL INCLUDE FILES
***************************************************************************************************/

#include "S_Nv_Bindings.h"

#include "S_Nv_Init.h"
#include "S_Nv_Stack_Ids.h"
#include "S_Nv.h"
#include "pdsDataServer.h"

/***************************************************************************************************
* EXTERNAL INCLUDE FILES
***************************************************************************************************/

#include "N_ErrH.h"
#include "N_Log.h"
#include "N_Util.h"
#include "N_Types.h"

#include "S_XNv_Bindings.h"
#include "S_XNv.h"

/***************************************************************************************************
* LOCAL MACROS AND CONSTANTS
***************************************************************************************************/

#define COMPID "S_Nv-External"

/***************************************************************************************************
* EXPORTED FUNCTIONS
***************************************************************************************************/

S_Nv_ReturnValue_t S_Nv_ItemInit_Impl(S_Nv_ItemId_t id, uint16_t itemLength, void* pDefaultData)
{
    return (S_Nv_ReturnValue_t)S_XNv_ItemInit((S_XNv_ItemId_t)id, itemLength, pDefaultData);
}

S_Nv_ReturnValue_t S_Nv_Read_Impl(S_Nv_ItemId_t id, uint16_t offset, uint16_t dataLength, void* pData)
{
    return (S_Nv_ReturnValue_t)S_XNv_Read((S_XNv_ItemId_t)id, offset, dataLength, pData);
}

S_Nv_ReturnValue_t S_Nv_Write_Impl(S_Nv_ItemId_t id, uint16_t offset, uint16_t dataLength, void* pData)
{
    return (S_Nv_ReturnValue_t)S_XNv_Write((S_XNv_ItemId_t)id, offset, dataLength, pData);
}

S_Nv_ReturnValue_t S_Nv_Delete_Impl(S_Nv_ItemId_t id)
{
    return (S_Nv_ReturnValue_t)S_XNv_Delete((S_XNv_ItemId_t)id);
}

uint16_t S_Nv_ItemLength_Impl(S_Nv_ItemId_t id)
{
    return S_XNv_ItemLength((S_XNv_ItemId_t)id);
}

S_Nv_ReturnValue_t S_Nv_EraseAll_Impl(bool includingPersistentItems)
{
    PDS_EraseSecureItems();
    return (S_Nv_ReturnValue_t)S_XNv_EraseAll(includingPersistentItems);
}

void S_Nv_SetPowerSupplyCheckingFunction_Impl(S_Nv_PowerSupplyCheckingFunction_t pf)
{
    S_XNv_SetPowerSupplyCheckingFunction(pf);
}

bool S_Nv_IsItemAvailable_Impl(S_Nv_ItemId_t id)
{
  return S_XNv_IsItemAvailable((S_XNv_ItemId_t)id);
}

void S_Nv_Init(void)
{
}
#endif //#ifdef PDS_USE_EXTERNAL_FLASH
#endif // #ifdef_ENABLE_PERSISTENT_SERVER_
#endif // #if PDS_ENABLE_WEAR_LEVELING == 1