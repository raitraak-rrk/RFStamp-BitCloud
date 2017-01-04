/**************************************************************************//**
  \file pdsMemIds.h

  \brief PDS file and directory memory identificators definitions.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2012-04-25 D. Kolmakov - Created.
   Last change:
    $Id: pdsMemIds.h 23302 2012-10-25 06:56:21Z nfomin $
******************************************************************************/

#if PDS_ENABLE_WEAR_LEVELING != 1
#ifndef _PDS_MEM_IDS_H
#define _PDS_MEM_IDS_H

/******************************************************************************
                               Includes section
******************************************************************************/
#include <sysTypes.h>

/******************************************************************************
                               Types section
******************************************************************************/
/*! \brief IDs of files and directories used to store data in the NV memory

The IDs from this enumeration are used to refer to stack parameters (files) and
groups made of these parameters (directories), which can be stored in the
NV memory via PDS functions. See \ref pds.
*/
typedef enum _PDS_MemIdsTable_t
{
  /* BitCloud files IDs */
  CS_UID_MEM_ID                          =  0x0000, //!< Refers to ::CS_UID
  CS_RF_TX_POWER_MEM_ID                  =  0x0001, //!< Refers to ::CS_RF_TX_POWER
  CS_EXT_PANID_MEM_ID                    =  0x0002, //!< Refers to ::CS_EXT_PANID
  CS_CHANNEL_MASK_MEM_ID                 =  0x0003, //!< Refers to ::CS_CHANNEL_MASK
  CS_CHANNEL_PAGE_MEM_ID                 =  0x0004, //!< Refers to ::CS_CHANNEL_PAGE
  CS_DEVICE_TYPE_MEM_ID                  =  0x0005, //!< Refers to ::CS_DEVICE_TYPE
  CS_RX_ON_WHEN_IDLE_MEM_ID              =  0x0006, //!< Refers to ::CS_RX_ON_WHEN_IDLE
  CS_COMPLEX_DESCRIPTOR_AVAILABLE_MEM_ID =  0x0007, //!< Refers to ::CS_COMPLEX_DESCRIPTOR_AVAILABLE
  CS_USER_DESCRIPTOR_AVAILABLE_MEM_ID    =  0x0008, //!< Refers to ::CS_USER_DESCRIPTOR_AVAILABLE
  CS_ZDP_USER_DESCRIPTOR_MEM_ID          =  0x0009, //!< Refers to ::CS_ZDP_USER_DESCRIPTOR
  CS_NWK_PANID_MEM_ID                    =  0x000A, //!< Refers to ::CS_NWK_PANID
  CS_NWK_PREDEFINED_PANID_MEM_ID         =  0x000B, //!< Refers to ::CS_NWK_PREDEFINED_PANID
  CS_NWK_ADDR_MEM_ID                     =  0x000C, //!< Refers to ::CS_NWK_ADDR
  CS_NWK_UNIQUE_ADDR_MEM_ID              =  0x000D, //!< Refers to ::CS_NWK_UNIQUE_ADDR
  CS_NWK_LEAVE_REQ_ALLOWED_MEM_ID        =  0x000E, //!< Refers to ::CS_NWK_LEAVE_REQ_ALLOWED
  CS_DTR_WAKEUP_MEM_ID                   =  0x000F, //!< Refers to ::CS_DTR_WAKEUP
  CS_APS_TRUST_CENTER_ADDRESS_MEM_ID     =  0x0010, //!< Refers to ::CS_APS_TRUST_CENTER_ADDRESS
  CS_SHORT_TRUST_CENTER_ADDRESS_MEM_ID   =  0x0011, //!< Refers to ::CS_SHORT_TRUST_CENTER_ADDRESS
  CS_ZDO_SECURITY_STATUS_MEM_ID          =  0x0012, //!< Refers to ::CS_ZDO_SECURITY_STATUS
  CS_NWK_PARENT_ADDR_MEM_ID              =  0x0013, //!< Refers to ::CS_NWK_PARENT_ADDR
  CS_NWK_DEPTH_MEM_ID                    =  0x0014, //!< Refers to ::CS_NWK_DEPTH
  CS_NWK_EXT_PANID_MEM_ID                =  0x0015, //!< Refers to ::CS_NWK_EXT_PANID
  CS_NWK_LOGICAL_CHANNEL_MEM_ID          =  0x0016, //!< Refers to ::CS_NWK_LOGICAL_CHANNEL
  CS_NEIB_TABLE_MEM_ID                   =  0x0017, //!< Refers to the neighbor table. The size of the table is set by ::CS_NEIB_TABLE_SIZE.
  CS_GROUP_TABLE_MEM_ID                  =  0x0018, //!< Refers to the group table. The size of the table is set by ::CS_GROUP_TABLE_SIZE.
  CS_APS_BINDING_TABLE_MEM_ID            =  0x0019, //!< Refers to the binding table. The size of the table is set by to ::CS_APS_BINDING_TABLE_SIZE.
  CS_NWK_SECURITY_IB_MEM_ID              =  0x001A, //!< Refers to the security information base.
  CS_NWK_SECURITY_KEYS_MEM_ID            =  0x001B, //!< Refers to the network keys table. The size of the table is set by to ::CS_NWK_SECURITY_KEYS_AMOUNT.
  CS_APS_KEY_PAIR_DESCRIPTORS_MEM_ID     =  0x001C, //!< Refers to the APS key-pair set. The size of the set is set by to ::CS_APS_KEY_PAIR_DESCRIPTORS_AMOUNT.
  CS_PERMISSIONS_TABLE_MEM_ID            =  0x001D, //!< Refers to the permission table. The size of the table is set by to ::CS_PERMISSIONS_TABLE_SIZE.
  NWK_UPDATE_ID_MEM_ID                   =  0x001E,

  /* Application files IDs */
  APP_PARAM1_MEM_ID                      =  0x001F,
  APP_PARAM2_MEM_ID                      =  0x0020,
  APP_PARAM3_MEM_ID                      =  0x0021,
  APP_PARAM4_MEM_ID                      =  0x0022,
  APP_PARAM5_MEM_ID                      =  0x0023,
  APP_PARAM6_MEM_ID                      =  0x0024,
  APP_PARAM7_MEM_ID                      =  0x0025,

  NWK_RREQ_IDENTIFIER_MEM_ID             =  0x0026,
  NWK_SECURITY_COUNTERS_MEM_ID           =  0x0027,

  /* Service value */
  PDS_FILE_IDS_AMOUNT,

  /* BitCloud directories IDs */
  BC_GENERAL_PARAMS_MEM_ID, //!< Most commonly used BitCloud parameters
  BC_EXTENDED_PARAMS_MEM_ID,  //!< Additional network parameters
  BC_NWK_SECURITY_TABLES_MEM_ID, //!< Security parameters of the NWK component
  BC_ALL_MEMORY_MEM_ID, //!< All BitCloud parameters prepared for storing in the non-volatile memory
  BC_EXT_GEN_MEMORY_MEM_ID, //!< The extended set of commonly used BitCloud parameters
  BC_NETWORK_REJOIN_PARAMS_MEM_ID, //!<Parameters to perform network rejoin after reset

  /* Application directories IDs */
  APP_DIR1_MEM_ID,
  APP_DIR2_MEM_ID,
  APP_DIR3_MEM_ID,

  /* Service values */
  PDS_ALL_EXISTENT_MEMORY,
  PDS_MEM_IDS_AMOUNT,
  PDS_NO_MEMORY_SPECIFIED = 0xFFFFU,
} PDS_MemIdsTable_t;

/*! Type of PDS file or directory. Possible values are defined in ::PDS_MemIdsTable_t. */
typedef uint16_t PDS_MemId_t;

/* Memory identifier record pointer */
typedef const PDS_MemId_t FLASH_PTR *PDS_MemIdRec_t;

#endif /* _PDS_MEM_IDS_H */
#endif /* PDS_ENABLE_WEAR_LEVELING != 1 */
/** eof pdsMemIds.h */

