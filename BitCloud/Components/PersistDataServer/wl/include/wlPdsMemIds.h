/************************************************************************//**
  \file wlPdsMemIds.h

  \brief
    PDS file and directory memory identificators definitions

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    21.05.13 N. Fomin - Created.
******************************************************************************/

#ifndef _WLPDSMEMIDS_H_
#define _WLPDSMEMIDS_H_

/******************************************************************************
                               Includes section
******************************************************************************/
#include <sysTypes.h>

/* Compatibility with std PDS mem IDs  */

#define BC_EXT_GEN_MEMORY_MEM_ID              BC_EXT_GEN_MEMORY_ITEM_ID
#define CS_NEIB_TABLE_MEM_ID                  CS_NEIB_TABLE_ITEM_ID
#define CS_APS_KEY_PAIR_DESCRIPTORS_MEM_ID    CS_APS_KEY_PAIR_DESCRIPTORS_ITEM_ID
#define CS_NWK_SECURITY_IB_MEM_ID             CS_NWK_SECURITY_IB_ITEM_ID
#define CS_NWK_SECURITY_KEYS_MEM_ID           CS_NWK_SECURITY_KEYS_ITEM_ID
#define NWK_SECURITY_COUNTERS_MEM_ID          NWK_SECURITY_COUNTERS_ITEM_ID
#define CS_APS_BINDING_TABLE_MEM_ID           CS_APS_BINDING_TABLE_ITEM_ID
#define CS_GROUP_TABLE_MEM_ID                 CS_GROUP_TABLE_ITEM_ID
#define NWK_RREQ_IDENTIFIER_MEM_ID            NWK_RREQ_IDENTIFIER_ITEM_ID

#define BC_EXT_GEN_MEMORY_ITEM_ID           0x0001U
#define CS_NEIB_TABLE_ITEM_ID               0x0002U
#define CS_APS_KEY_PAIR_DESCRIPTORS_ITEM_ID 0x0003U
#define CS_NWK_SECURITY_IB_ITEM_ID          0x0004U
#define CS_NWK_SECURITY_KEYS_ITEM_ID        0x0005U
#define NWK_SECURITY_COUNTERS_ITEM_ID       0x0006U
#define CS_APS_BINDING_TABLE_ITEM_ID        0x0007U
#define CS_GROUP_TABLE_ITEM_ID              0x0008U
#define NWK_RREQ_IDENTIFIER_ITEM_ID         0x0009U
#define BITCLOUD_MAX_ITEMS_AMOUNT           NWK_RREQ_IDENTIFIER_ITEM_ID

/* Application files IDs */
#define  APP_PARAM1_MEM_ID                  0x000AU
#define  APP_PARAM2_MEM_ID                  0x000BU
#define  APP_PARAM3_MEM_ID                  0x000CU
#define  APP_PARAM4_MEM_ID                  0x000DU
#define  APP_PARAM5_MEM_ID                  0x000EU
#define  APP_PARAM6_MEM_ID                  0x000FU
#define  APP_PARAM7_MEM_ID                  0x0010U
#define  APP_PARAM8_MEM_ID                  0x0011U
#define  APP_PARAM9_MEM_ID                  0x0012U
#define  APP_PARAM10_MEM_ID                 0x0013U
#define  APP_PARAM11_MEM_ID                 0x0014U
#define  APP_PARAM12_MEM_ID                 0x0015U
#define  APP_PARAM13_MEM_ID                 0x0016U
#define  APP_PARAM14_MEM_ID                 0x0017U
#define  APP_PARAM15_MEM_ID                 0x0018U
#define  APP_PARAM16_MEM_ID                 0x0019U
#define  APP_PARAM1A_MEM_ID                 0x001AU
#define  APP_PARAM1B_MEM_ID                 0x001BU
#define  APP_PARAM1C_MEM_ID                 0x001CU
#define  APP_PARAM1D_MEM_ID                 0x001DU
#define  APP_PARAM1E_MEM_ID                 0x001EU
#define  APP_PARAM1F_MEM_ID                 0x001FU
#define  APP_PARAM20_MEM_ID                 0x0020U
#define  APP_PARAM_MAX_MEM_ID_AMOUNT        APP_PARAM20_MEM_ID

#if APP_USE_OTAU == 1
/* OTAU file IDs */
#define  OTAU_PARAM1_MEM_ID                 0x0021U
#define  OTAU_PARAM2_MEM_ID                 0x0022U
#define  OTAU_PARAM3_MEM_ID                 0x0023U
#define  OTAU_PARAM4_MEM_ID                 0x0024U
#define  OTAU_PARAM5_MEM_ID                 0x0025U
#define  OTAU_PARAM6_MEM_ID                 0x0026U
#define  OTAU_PARAM7_MEM_ID                 0x0027U
#define  OTAU_PARAM8_MEM_ID                 0x0028U
#define  OTAU_PARAM9_MEM_ID                 0x0029U
#endif

/* Directory mask */
#define  PDS_DIRECTORY_MASK                 0x4000U

#define BC_ALL_MEMORY_MEM_ID                0x4001U
#define BITCLOUD_MAX_DIRECTORIES_AMOUNT     (1)

#define PDS_ALL_EXISTENT_MEMORY             0x4002U

/* Application directory IDs */
#define  APP_DIR1_MEM_ID                    0x4003U
#define  APP_DIR2_MEM_ID                    0x4004U
#define  APP_DIR3_MEM_ID                    0x4005U
#define  APP_DIR4_MEM_ID                    0x4006U

/* Application files and directories amount */
#ifndef APPLICATION_MAX_FILES_AMOUNT
  #define APPLICATION_MAX_FILES_AMOUNT      APP_PARAM_MAX_MEM_ID_AMOUNT
#endif /* APPLICATION_MAX_FILES_AMOUNT */

#ifndef APPLICATION_MAX_DIRECTORIES_AMOUNT
  #define APPLICATION_MAX_DIRECTORIES_AMOUNT 0x0004U
#endif /* APPLICATION_MAX_DIRECTORIES_AMOUNT */

#if APP_USE_OTAU == 1
/* OTAU directory IDs */
#define  OTAU_DIR1_MEM_ID                    0x4007U
#define  OTAU_DIR2_MEM_ID                    0x4008U
#define  OTAU_DIR3_MEM_ID                    0x4009U

/* OTAU files and directories amount */
#ifndef OTAU_MAX_FILES_AMOUNT
  #define OTAU_MAX_FILES_AMOUNT             0x0009U
#endif // OTAU_MAX_FILES_AMOUNT

#ifndef OTAU_MAX_DIRECTORIES_AMOUNT
  #define OTAU_MAX_DIRECTORIES_AMOUNT       0x0003U
#endif // OTAU_MAX_DIRECTORIES_AMOUNT
#endif // APP_USE_OTAU == 1

#if APP_USE_OTAU == 1
#define PDS_ITEM_AMOUNT             (BITCLOUD_MAX_ITEMS_AMOUNT +    \
                                     APPLICATION_MAX_FILES_AMOUNT + \
                                     OTAU_MAX_FILES_AMOUNT)

#define PDS_DIRECTORIES_AMOUNT      (BITCLOUD_MAX_DIRECTORIES_AMOUNT +    \
                                     APPLICATION_MAX_DIRECTORIES_AMOUNT + \
                                     OTAU_MAX_DIRECTORIES_AMOUNT)
#else
#define PDS_ITEM_AMOUNT             (BITCLOUD_MAX_ITEMS_AMOUNT + \
                                     APPLICATION_MAX_FILES_AMOUNT)

#define PDS_DIRECTORIES_AMOUNT      (BITCLOUD_MAX_DIRECTORIES_AMOUNT + \
                                     APPLICATION_MAX_DIRECTORIES_AMOUNT)
#endif // APP_USE_OTAU == 1

/* Total files and directories amount supported by PDS */
#define PDS_ITEM_IDS_AMOUNT                 (PDS_ITEM_AMOUNT +  PDS_DIRECTORIES_AMOUNT)

#define PDS_ITEM_MASK_SIZE                  (PDS_ITEM_AMOUNT / 8U + (PDS_ITEM_AMOUNT % 8U ? 1U : 0U))

/******************************************************************************
                               Types section
******************************************************************************/

/*! Type of PDS file or directory unique identifier. */
typedef uint16_t PDS_MemId_t;

/* Memory identifier record pointer */
typedef const PDS_MemId_t FLASH_PTR *PDS_MemIdRec_t;

#endif // _WLPDSMEMIDS_H_
/* eof wlPdsMemIds.h */