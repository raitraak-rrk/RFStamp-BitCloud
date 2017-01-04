/***************************************************************************//**
  \file zclDiagnosticsCluster.h

  \brief
    The header file describes the ZCL Diagnostics Cluster and its interface

    The file describes the types and interface of the ZCL Diagnostics Cluster

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    11.11.14 Parthasarathy G - Created.
*******************************************************************************/

#ifndef _ZCLDIAGNOSTICSCLUSTER_H
#define _ZCLDIAGNOSTICSCLUSTER_H

/*******************************************************************************
                   Includes section
*******************************************************************************/
#include <zcl.h>
#include <clusters.h>

/*******************************************************************************
                   Define(s) section
*******************************************************************************/
/**
 * \brief Diagnostics Server Cluster attributes amount.
*/
#define ZCL_DIAGNOSTICS_CLUSTER_SERVER_ATTRIBUTES_AMOUNT 3
/* HA_PROFILE_EXTENSIONS */

/**
 * \brief Diagnostics Client Cluster attributes amount.
*/
#define ZCL_DIAGNOSTICS_CLUSTER_CLIENT_ATTRIBTUES_AMOUNT 0

/**
 * \brief Diagnostics Client Cluster commands amount.
*/
#define ZCL_DIAGNOSTICS_CLUSTER_COMMANDS_AMOUNT 0

/**
 * \brief Diagnostics server cluster attributes identifiers.
*/
#define ZCL_DIAGNOSTICS_CLUSTER_AVG_MAC_RETRY_APS_MSG_SENT_ATTRIBUTE_ID CCPU_TO_LE16(0x011b)
#define ZCL_DIAGNOSTICS_CLUSTER_LAST_MSG_LQI_ATTRIBUTE_ID               CCPU_TO_LE16(0x011c)
#define ZCL_DIAGNOSTICS_CLUSTER_LAST_MSG_RSSI_ATTRIBUTE_ID              CCPU_TO_LE16(0x011d)

/*
 *\brief ZCL Diagnostics Cluster server side attributes defining macros
 */
#define ZCL_DEFINE_DIAGNOSTICS_CLUSTER_SERVER_ATTRIBUTES() \
  DEFINE_ATTRIBUTE_WITH_BOUNDARY(averageMACRetryPerAPSMessageSent, ZCL_READONLY_ATTRIBUTE,\
          ZCL_DIAGNOSTICS_CLUSTER_AVG_MAC_RETRY_APS_MSG_SENT_ATTRIBUTE_ID, ZCL_S16BIT_DATA_TYPE_ID, 0x0000, 0xffff),\
  DEFINE_ATTRIBUTE_WITH_BOUNDARY(lastMessageLQI, ZCL_READONLY_ATTRIBUTE,\
          ZCL_DIAGNOSTICS_CLUSTER_LAST_MSG_LQI_ATTRIBUTE_ID, ZCL_U8BIT_DATA_TYPE_ID, 0x00, 0xff),\
  DEFINE_ATTRIBUTE_WITH_BOUNDARY(lastMessageRSSI, ZCL_READONLY_ATTRIBUTE,\
          ZCL_DIAGNOSTICS_CLUSTER_LAST_MSG_RSSI_ATTRIBUTE_ID, ZCL_S8BIT_DATA_TYPE_ID, 0x00, 0xff),\
  
/***************************************************************************//**
\brief Diagnostics Cluster definition macros
*******************************************************************************/

#define DIAGNOSTICS_CLUSTER_ZCL_CLIENT_CLUSTER_TYPE(clattributes, clcommands) \
{ \
   .id = DIAGNOSTICS_CLUSTER_ID, \
   .options = { \
                .type = ZCL_CLIENT_CLUSTER_TYPE, \
                .security = ZCL_NETWORK_KEY_CLUSTER_SECURITY, \
              }, \
   .attributesAmount = ZCL_DIAGNOSTICS_CLUSTER_CLIENT_ATTRIBTUES_AMOUNT, \
   .attributes = (uint8_t *)clattributes, \
   .commandsAmount = ZCL_DIAGNOSTICS_CLUSTER_COMMANDS_AMOUNT, \
   .commands = (uint8_t *)clcommands \
}

#define DIAGNOSTICS_CLUSTER_ZCL_SERVER_CLUSTER_TYPE(clattributes, clcommands) \
{ \
   .id = DIAGNOSTICS_CLUSTER_ID, \
   .options = { \
                .type = ZCL_SERVER_CLUSTER_TYPE, \
                .security = ZCL_NETWORK_KEY_CLUSTER_SECURITY, \
              }, \
   .attributesAmount = ZCL_DIAGNOSTICS_CLUSTER_SERVER_ATTRIBUTES_AMOUNT, \
   .attributes = (uint8_t *)clattributes, \
   .commandsAmount = ZCL_DIAGNOSTICS_CLUSTER_COMMANDS_AMOUNT, \
   .commands = (uint8_t *)clcommands \
}

#define DEFINE_DIAGNOSTICS_CLUSTER(cltype, clattributes, clcommands) \
    DIAGNOSTICS_CLUSTER_##cltype(clattributes, clcommands)

#define DIAGNOSTICS_CLUSTER_ZCL_CLIENT_CLUSTER_TYPE_FLASH(clattributes, clcommands) \
{ \
   .id = DIAGNOSTICS_CLUSTER_ID, \
   .options = { \
                .type = ZCL_CLIENT_CLUSTER_TYPE, \
                .security = ZCL_NETWORK_KEY_CLUSTER_SECURITY, \
              }, \
   .attributesAmount = ZCL_DIAGNOSTICS_CLUSTER_CLIENT_ATTRIBTUES_AMOUNT, \
   .attributes = (uint8_t *)clattributes, \
   .commandsAmount = ZCL_DIAGNOSTICS_CLUSTER_COMMANDS_AMOUNT, \
   .commands = (FLASH_PTR uint8_t *)clcommands \
}

#define DIAGNOSTICS_CLUSTER_ZCL_SERVER_CLUSTER_TYPE_FLASH(clattributes, clcommands) \
{ \
   .id = DIAGNOSTICS_CLUSTER_ID, \
   .options = { \
                .type = ZCL_SERVER_CLUSTER_TYPE, \
                .security = ZCL_NETWORK_KEY_CLUSTER_SECURITY, \
              }, \
   .attributesAmount = ZCL_DIAGNOSTICS_CLUSTER_SERVER_ATTRIBUTES_AMOUNT, \
   .attributes = (uint8_t *)clattributes, \
   .commandsAmount = ZCL_DIAGNOSTICS_CLUSTER_COMMANDS_AMOUNT, \
   .commands = (FLASH_PTR uint8_t *)clcommands \
}

#define DEFINE_DIAGNOSTICS_CLUSTER_FLASH(cltype, clattributes, clcommands) \
    DIAGNOSTICS_CLUSTER_##cltype##_FLASH(clattributes, clcommands)
/******************************************************************************
                    Types section
******************************************************************************/

BEGIN_PACK

/**
 * \brief Diagnostics cluster attributes.
*/
typedef struct PACK
{
  struct PACK
  {
    ZCL_AttributeId_t id;
    uint8_t type;
    uint8_t properties;
    uint16_t value;
    uint16_t minVal;
    uint16_t maxVal;
  } averageMACRetryPerAPSMessageSent;            //! <Attr ID 0x011b>
  struct PACK
  {
    ZCL_AttributeId_t id;
    uint8_t type;
    uint8_t properties;
    uint8_t value;
    uint8_t minVal;
    uint8_t maxVal;
  } lastMessageLQI;            //! <Attr ID 0x011c>
  struct PACK
  {
    ZCL_AttributeId_t id;
    uint8_t type;
    uint8_t properties;
    int8_t value;
    int8_t minVal;
    int8_t maxVal;
  } lastMessageRSSI;            //! <Attr ID 0x011c>
  
} ZCL_DiagnosticsClusterServerAttributes_t;

END_PACK

#endif /* _ZCLDIAGNOSTICSCLUSTER_H */

