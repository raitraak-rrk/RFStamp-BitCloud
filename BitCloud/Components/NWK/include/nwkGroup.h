/**************************************************************************//**
  \file nwkGroup.h

  \brief Interface of the group table.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2010-07-20 Max Gekk - Created.
   Last change:
    $Id: nwkGroup.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#if !defined _NWK_GROUP_H
#define _NWK_GROUP_H

/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <appFramework.h>
#include <nlmeReset.h>

/******************************************************************************
                              Definitions section
 ******************************************************************************/
/** Value that can not be used as a group data parameter. */
#define NWK_INVALID_GROUP_DATA 0x00U
/** Universal group data. It means any group data value. */
#define NWK_ANY_GROUP_DATA 0xFFU

/******************************************************************************
                                Types section
 ******************************************************************************/
/** Type of data linked to group. */
typedef uint8_t NWK_GroupData_t;
/** Type of group address. */
typedef GroupAddr_t NWK_GroupAddr_t;

/** Type of group table entry. */
typedef struct _NWK_GroupTableEntry_t
{
  /** The data used by user, for example APS end point. */
  NWK_GroupData_t data;
  /** Address of group. */
  NWK_GroupAddr_t addr;
} NWK_GroupTableEntry_t;

/** Type of size of the group table.*/
typedef uint8_t NWK_GroupTableSize_t;

/** Type of internal variables of group component. */
typedef struct _NWK_GroupTable_t
{
  /** Pointer to first entry of the group table. */
  NWK_GroupTableEntry_t *begin;
  /** Pointer to memory after last entry of the group table. */
  NWK_GroupTableEntry_t *end;
} NWK_GroupTable_t;

/******************************************************************************
                               Prototypes section
 ******************************************************************************/
#if defined _GROUP_TABLE_
/**************************************************************************//**
  \brief Add new entry to the group table.

  \param[in] addr - address of group. Valid range 0x0000 - 0xffff.
  \param[in] data - value linked to group. Valid range 0x01 - 0xfe.

  \return 'true' if entry is added successfully otherwise 'false'.
 ******************************************************************************/
bool NWK_AddGroup(const NWK_GroupAddr_t addr, const NWK_GroupData_t data);

/**************************************************************************//**
  \brief Remove group entries from the group table.

  \param[in] addr - address of group. Valid range 0x0000 - 0xffff.
  \param[in] data - value linked to group. Valid range 0x01 - 0xff.
                    If data is equal to NWK_ANY_GROUP_DATA then remove
                    all entries with given group address.

  \return 'true' if any entry is removed successfully otherwise 'false'.
 ******************************************************************************/
bool NWK_RemoveGroup(const NWK_GroupAddr_t addr, const NWK_GroupData_t data);

/**************************************************************************//**
  \brief Remove all group entries with given group data

  \param[in] data - value linked to group. Valid range 0x01 - 0xff.
                    If data is equal to NWK_ANY_GROUP_DATA then remove
                    all entries from table.

  \return 'true' if any entry is removed successfully otherwise 'false'.
 ******************************************************************************/
bool NWK_RemoveAllGroups(const NWK_GroupData_t data);

/**************************************************************************//**
  \brief Check group address and data in the group table.

  \param[in] addr - address of group. Valid range 0x0000 - 0xffff.
  \param[in] data - value linked to group. Valid range 0x01 - 0xff.
                    If data is equal to NWK_ANY_GROUP_DATA that means
                    any enties with given group address.

  \return 'true' if pair addr and data is found in the group table
           otherwise 'false'.
 ******************************************************************************/
bool NWK_IsGroupMember(const NWK_GroupAddr_t addr, const NWK_GroupData_t data);

/**************************************************************************//**
  \brief Get remaining groups capacity.

  \return number of free entries in the group table.
 ******************************************************************************/
NWK_GroupTableSize_t NWK_GroupCapacity(void);

/**************************************************************************//**
  \brief Get first or next non-free entry from the groups table.

  \param[in] entry - address of the current entry or NULL to get the first entry.

  \return pointer to the next non-free entry or NULL if no more entries exist.
 ******************************************************************************/
NWK_GroupTableEntry_t* NWK_NextGroup(NWK_GroupTableEntry_t *entry);

/**************************************************************************//**
  \brief Allocate and initialize the group table.
 ******************************************************************************/
void NWK_ResetGroups(void);

#else
#define NWK_ResetGroups() (void)0
#endif /* _GROUP_TABLE_ */
#endif /* _NWK_GROUP_H */
/** eof nwkGroup.h */

