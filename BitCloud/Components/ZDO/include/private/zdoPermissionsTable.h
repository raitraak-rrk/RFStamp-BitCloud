/**************************************************************************//**
  \file zdoPermissionsTable.h

  \brief Internal interface of the permissions configuration table.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2012-06-20 Max Gekk - Created.
   Last change:
    $Id: zdoPermissionsTable.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#if !defined _ZDO_PERMISSIONS_TABLE_H
#define _ZDO_PERMISSIONS_TABLE_H

/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <zdoPermissions.h>

#if defined _PERMISSIONS_
/******************************************************************************
                                Types section
 ******************************************************************************/
/** Type of a record of the permissions configuration table. */
typedef struct _ZdoPermissionsRecord_t
{
  /** If the record is used (some information was saved) then the field is 'true'
   * otherwise 'false' and it means the record can be reused. */
  bool busy;
  /** Current permissions which are associated the device. */
  ZDO_Permissions_t permissions;
  /** IEEE address of the device whose permissions are stored in the record. */
  ExtAddr_t deviceAddress;
} ZdoPermissionsRecord_t;

/** Type of the permissions table size. */
typedef uint8_t ZdoPermissionsTableSize_t;
/** Type of index to iterate the permissions table. */
typedef ZdoPermissionsTableSize_t ZdoPermissionsIndex_t;

typedef struct _ZdoPermissions_t
{
  /** Permissions that will be applied by default to a device which doesn't have
   * its record in the table. */
  ZDO_Permissions_t defaultPermissions;  
  /** The pointer to the permissions table. The field must be initialized in
   * the zdoResetPermissions function. */
  ZdoPermissionsRecord_t *table;  
  /** Current size of the permissions table. It must be initialized with the 
   * table field. If the table field is NULL then this field must be 0. */
  ZdoPermissionsTableSize_t tableSize;
} ZdoPermissions_t;

/******************************************************************************
                               Prototypes section
 ******************************************************************************/
/**************************************************************************//**
  \brief Reloads and reinits the permissions configuration table.

  \return None.
 ******************************************************************************/
ZDO_PRIVATE void zdoResetPermissions(void);

#endif /* _PERMISSIONS_ */
#endif /* _ZDO_PERMISSIONS_TABLE_H */
/** zdoPermissionsTable.h */
