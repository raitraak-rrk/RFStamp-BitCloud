#ifndef N_ADDRESSMANAGER_BINDINGS_H
#define N_ADDRESSMANAGER_BINDINGS_H

#if defined(__GNUC__)
#include "N_Util_Bindings.h"
#include "N_Util.h"
#endif
// This

#define N_AddressManager_LogState N_AddressManager_LogState_Impl
#define N_AddressManager_AllocateAddresses N_AddressManager_AllocateAddresses_Impl
#define N_AddressManager_AllocateAddressesForDealer N_AddressManager_AllocateAddressesForDealer_Impl
#define N_AddressManager_AssignAddressRange N_AddressManager_AssignAddressRange_Impl
#define N_AddressManager_HasFreeGroupIds N_AddressManager_HasFreeGroupIds_Impl
#define N_AddressManager_AllocateGroupId N_AddressManager_AllocateGroupId_Impl
#define N_AddressManager_GetFreeGroupIdRange N_AddressManager_GetFreeGroupIdRange_Impl

#ifdef TESTHARNESS
#  define N_Flash_ItemInit N_Flash_Stub_ItemInit
#  define N_Flash_Read N_Flash_Stub_Read
#  define N_Flash_Write N_Flash_Stub_Write

#  define N_Util_Random N_Util_Stub_Random

#else
#  define N_Flash_ItemInit N_Flash_ItemInit_Impl
#  define N_Flash_Read N_Flash_Read_Impl
#  define N_Flash_Write N_Flash_Write_Impl

#  define S_Nv_ItemInit S_Nv_ItemInit_Impl
#  define S_Nv_Write S_Nv_Write_Impl

#  define N_Util_Random N_Util_Random_Impl

#endif

#endif /*N_ADDRESSMANAGER_BINDINGS_H*/
