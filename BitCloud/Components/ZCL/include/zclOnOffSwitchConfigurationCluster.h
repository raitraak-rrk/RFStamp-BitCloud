/***************************************************************************//**
  \file zclOnOffSwitchConfigurationCluster.h

  \brief
    The header file describes the ZCL On/Off Switch Configuration Cluster and its interface

    The file describes the types and interface of the ZCL On/Off Switch Configuration Cluster

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    12.02.09 D. Kasyanov - Created.
*******************************************************************************/

#ifndef _ZCLONOFFSWITCHCONFIGURATIONCLUSTER_H
#define _ZCLONOFFSWITCHCONFIGURATIONCLUSTER_H

/*!
Attributes and commands for determining basic information about a device,
setting user device information such as location, enabling a device and resetting it
to factory defaults.
*/

/*******************************************************************************
                   Includes section
*******************************************************************************/

#include <zcl.h>
#include <clusters.h>

/*******************************************************************************
                   Define(s) section
*******************************************************************************/
/**
 * \brief On/Off Switch Configuration attributes values
*/

#define SWITCH_TYPE_ATTRIBUTE_VALUE_TOGGLE 0x00
#define SWITCH_TYPE_ATTRIBUTE_VALUE_MOMENTARY 0x01

#define SWITCH_ACTIONS_ATTRIBUTE_VALUE_ON_OFF 0x00
#define SWITCH_ACTIONS_ATTRIBUTE_VALUE_OFF_ON 0x01
#define SWITCH_ACTIONS_ATTRIBITE_VALUE_TOGGLE_TOGGLE 0x02

/**
 * \brief On/Off Switch Configuration Cluster server attributes amount
*/

#define ZCL_ONOFF_SWITCH_CONFIGURATION_CLUSTER_SERVER_ATTRIBUTES_AMOUNT 2

/**
 * \brief On/Off Switch Configuration Cluster client attributes amount. Clinet doesn't have attributes.
*/

#define ZCL_ONOFF_SWITCH_CONFIGURATION_CLUSTER_CLIENT_ATTRIBTUES_AMOUNT 0

/**
 * \brief On/Off Switch Configuration Cluster commands amount
*/

#define ZCL_ONOFF_SWITCH_CONFIGURATION_CLUSTER_COMMANDS_AMOUNT 0

/**
 * \brief On/Off Switch Configuration Cluster server's attributes identifiers
*/

#define ZCL_ONOFF_SWITCH_CONFIGURATION_CLUSTER_SWITCH_TYPE_SERVER_ATTRIBUTE_ID    CCPU_TO_LE16(0x0000)
#define ZCL_ONOFF_SWITCH_CONFIGURATION_CLUSTER_SWITCH_ACTIONS_SERVER_ATTRIBUTE_ID CCPU_TO_LE16(0x0010)

/**
 * \brief On/Off Switch Configuration Cluster server define attributes macros
*/

#define ZCL_DEFINE_ONOFF_SWITCH_CONFIGURATION_CLUSTER_SERVER_ATTRIBUTES() \
    DEFINE_ATTRIBUTE_WITH_BOUNDARY(switchType, ZCL_READONLY_ATTRIBUTE, ZCL_ONOFF_SWITCH_CONFIGURATION_CLUSTER_SWITCH_TYPE_SERVER_ATTRIBUTE_ID, ZCL_8BIT_ENUM_DATA_TYPE_ID, 0x00, 0x01), \
    DEFINE_ATTRIBUTE_WITH_BOUNDARY(switchActions, ZCL_READWRITE_ATTRIBUTE, ZCL_ONOFF_SWITCH_CONFIGURATION_CLUSTER_SWITCH_ACTIONS_SERVER_ATTRIBUTE_ID, ZCL_8BIT_ENUM_DATA_TYPE_ID, 0x00, 0x02)


/**
 * \brief On/Off Switch Configuration Cluster definition macros
*/

#define ONOFF_SWITCH_CONFIGURATION_CLUSTER_ZCL_CLIENT_CLUSTER_TYPE(clattributes, clcommands) \
{ \
   .id = ONOFF_SWITCH_CONFIGURATION_CLUSTER_ID, \
   .options = { \
                .type = ZCL_CLIENT_CLUSTER_TYPE, \
                .security = ZCL_APPLICATION_LINK_KEY_CLUSTER_SECURITY, \
              }, \
   .attributesAmount = ZCL_ONOFF_SWITCH_CONFIGURATION_CLUSTER_CLIENT_ATTRIBTUES_AMOUNT, \
   .attributes = (uint8_t *)clattributes, \
   .commandsAmount = ZCL_ONOFF_SWITCH_CONFIGURATION_CLUSTER_COMMANDS_AMOUNT, \
   .commands = (uint8_t *)clcommands \
}

#define ONOFF_SWITCH_CONFIGURATION_CLUSTER_ZCL_SERVER_CLUSTER_TYPE(clattributes, clcommands) \
{ \
   .id = ONOFF_SWITCH_CONFIGURATION_CLUSTER_ID, \
   .options = { \
                .type = ZCL_SERVER_CLUSTER_TYPE, \
                .security = ZCL_APPLICATION_LINK_KEY_CLUSTER_SECURITY, \
              }, \
   .attributesAmount = ZCL_ONOFF_SWITCH_CONFIGURATION_CLUSTER_SERVER_ATTRIBUTES_AMOUNT, \
   .attributes = (uint8_t *)clattributes, \
   .commandsAmount = ZCL_ONOFF_SWITCH_CONFIGURATION_CLUSTER_COMMANDS_AMOUNT, \
   .commands = (uint8_t *)clcommands \
}

#define DEFINE_ONOFF_SWITCH_CONFIGURATION_CLUSTER(cltype, clattributes, clcommands) \
    ONOFF_SWITCH_CONFIGURATION_CLUSTER_##cltype(clattributes, clcommands)

#define ONOFF_SWITCH_CONFIGURATION_CLUSTER_ZCL_CLIENT_CLUSTER_TYPE_FLASH(clattributes, clcommands) \
{ \
   .id = ONOFF_SWITCH_CONFIGURATION_CLUSTER_ID, \
   .options = { \
                .type = ZCL_CLIENT_CLUSTER_TYPE, \
                .security = ZCL_APPLICATION_LINK_KEY_CLUSTER_SECURITY, \
              }, \
   .attributesAmount = ZCL_ONOFF_SWITCH_CONFIGURATION_CLUSTER_CLIENT_ATTRIBTUES_AMOUNT, \
   .attributes = (uint8_t *)clattributes, \
   .commandsAmount = ZCL_ONOFF_SWITCH_CONFIGURATION_CLUSTER_COMMANDS_AMOUNT, \
   .commands = (FLASH_PTR uint8_t *)clcommands \
}

#define ONOFF_SWITCH_CONFIGURATION_CLUSTER_ZCL_SERVER_CLUSTER_TYPE_FLASH(clattributes, clcommands) \
{ \
   .id = ONOFF_SWITCH_CONFIGURATION_CLUSTER_ID, \
   .options = { \
                .type = ZCL_SERVER_CLUSTER_TYPE, \
                .security = ZCL_APPLICATION_LINK_KEY_CLUSTER_SECURITY, \
              }, \
   .attributesAmount = ZCL_ONOFF_SWITCH_CONFIGURATION_CLUSTER_SERVER_ATTRIBUTES_AMOUNT, \
   .attributes = (uint8_t *)clattributes, \
   .commandsAmount = ZCL_ONOFF_SWITCH_CONFIGURATION_CLUSTER_COMMANDS_AMOUNT, \
   .commands = (FLASH_PTR uint8_t *)clcommands \
}

#define DEFINE_ONOFF_SWITCH_CONFIGURATION_CLUSTER_FLASH(cltype, clattributes, clcommands) \
    ONOFF_SWITCH_CONFIGURATION_CLUSTER_##cltype##_FLASH(clattributes, clcommands)
/******************************************************************************
                    Types section
******************************************************************************/

BEGIN_PACK

/**
 * \brief On/Off Switch Configuration Cluster server's attributes
 */

typedef struct PACK
{
  struct PACK
  {
    ZCL_AttributeId_t id;
    uint8_t type;
    uint8_t properties;
    uint8_t value;
    uint8_t minVal;
    uint8_t maxVal;
  } switchType;

  struct PACK
  {
    ZCL_AttributeId_t id;
    uint8_t type;
    uint8_t properties;
    uint8_t value;
    uint8_t minVal;
    uint8_t maxVal;
  } switchActions;
} ZCL_OnOffSwitchConfigurationClusterServerAttributes_t;

END_PACK

#endif /* _ZCLONOFFSWITCHCONFIGURATIONCLUSTER_H */

