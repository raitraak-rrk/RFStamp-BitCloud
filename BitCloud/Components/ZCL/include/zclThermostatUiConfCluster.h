/***************************************************************************//**
  \file zclThermostatUiConfCluster.h

  \brief
    The header file describes the ZCL Thermostat Ui conf Cluster and its interface

    The file describes the types and interface of the ZCL Thermostat UI conf Cluster

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    09.10.14 Unithra - Created.
*******************************************************************************/

#ifndef _ZCLTHERMOSTATUICONFCLUSTER_H
#define _ZCLTHERMOSTATUICONFCLUSTER_H
/*******************************************************************************
                   Includes section
*******************************************************************************/
#include <sysTypes.h>
#include <clusters.h>

/*******************************************************************************
                   Define(s) section
*******************************************************************************/
/***************************************************************************//**
\brief Thermostat Ui conf Cluster server attributes amount
*******************************************************************************/

#define ZCL_THERMOSTAT_UI_CONF_CLUSTER_SERVER_ATTRIBUTES_AMOUNT 3

/***************************************************************************//**
\brief Thermostat UI conf Cluster client attributes amount. Clinet doesn't have attributes
*******************************************************************************/

#define ZCL_THERMOSTAT_UI_CONF_CLUSTER_CLIENT_ATTRIBTUES_AMOUNT 0

/***************************************************************************//**
\brief Thermostat UI conf Cluster commands amount
*******************************************************************************/

#define ZCL_THERMOSTAT_UI_CONF_CLUSTER_COMMANDS_AMOUNT 0

/***************************************************************************//**
\brief Thermostat UI conf Cluster server's attributes identifiers
*******************************************************************************/
// Thermostat ui conf information
#define ZCL_THERMOSTAT_UI_CONF_CLUSTER_TEMPERATURE_DISPLAY_MODE_SERVER_ATTRIBUTE_ID         CCPU_TO_LE16(0x0000)
#define ZCL_THERMOSTAT_UI_CONF_CLUSTER_KEYPAD_LOCKOUT_SERVER_ATTRIBUTE_ID                   CCPU_TO_LE16(0x0001)
#define ZCL_THERMOSTAT_UI_CONF_CLUSTER_SCHEDULE_PROGRAMMING_VISIBILITY_SERVER_ATTRIBUTE_ID  CCPU_TO_LE16(0x0002)

/***************************************************************************//**
\brief Thermostat UI conf Cluster server define attributes macros
*******************************************************************************/

#define ZCL_DEFINE_THERMOSTAT_UI_CONF_CLUSTER_SERVER_ATTRIBUTES() \
    DEFINE_ATTRIBUTE_WITH_BOUNDARY(temperatureDisplayMode, ZCL_READWRITE_ATTRIBUTE, ZCL_THERMOSTAT_UI_CONF_CLUSTER_TEMPERATURE_DISPLAY_MODE_SERVER_ATTRIBUTE_ID, ZCL_8BIT_ENUM_DATA_TYPE_ID, 0x00, 0x01), \
    DEFINE_ATTRIBUTE_WITH_BOUNDARY(keypadLockOut, ZCL_READWRITE_ATTRIBUTE, ZCL_THERMOSTAT_UI_CONF_CLUSTER_KEYPAD_LOCKOUT_SERVER_ATTRIBUTE_ID, ZCL_8BIT_ENUM_DATA_TYPE_ID, 0x00, 0x05), \
    DEFINE_ATTRIBUTE_WITH_BOUNDARY(scheduleProgVisibility, ZCL_READWRITE_ATTRIBUTE, ZCL_THERMOSTAT_UI_CONF_CLUSTER_SCHEDULE_PROGRAMMING_VISIBILITY_SERVER_ATTRIBUTE_ID, ZCL_8BIT_ENUM_DATA_TYPE_ID, 0x00, 0x01)
/***************************************************************************//**
\brief Thermostat Cluster definition macros
*******************************************************************************/

#define THERMOSTAT_UI_CONF_CLUSTER_ZCL_CLIENT_CLUSTER_TYPE(clattributes, clcommands) \
{ \
   .id = THERMOSTAT_UI_CONF_CLUSTER_ID, \
   .options = { \
                .type = ZCL_CLIENT_CLUSTER_TYPE, \
                .security = ZCL_NETWORK_KEY_CLUSTER_SECURITY, \
              }, \
   .attributesAmount = ZCL_THERMOSTAT_UI_CONF_CLUSTER_CLIENT_ATTRIBTUES_AMOUNT, \
   .attributes = (uint8_t *)clattributes, \
   .commandsAmount = ZCL_THERMOSTAT_UI_CONF_CLUSTER_COMMANDS_AMOUNT, \
   .commands = (uint8_t *)clcommands \
}

#define THERMOSTAT_UI_CONF_CLUSTER_ZCL_SERVER_CLUSTER_TYPE(clattributes, clcommands) \
{ \
   .id = THERMOSTAT_UI_CONF_CLUSTER_ID, \
   .options = { \
                .type = ZCL_SERVER_CLUSTER_TYPE, \
                .security = ZCL_NETWORK_KEY_CLUSTER_SECURITY, \
              }, \
   .attributesAmount = ZCL_THERMOSTAT_UI_CONF_CLUSTER_SERVER_ATTRIBUTES_AMOUNT, \
   .attributes = (uint8_t *)clattributes, \
   .commandsAmount = ZCL_THERMOSTAT_UI_CONF_CLUSTER_COMMANDS_AMOUNT, \
   .commands = (uint8_t *)clcommands \
}
#define DEFINE_THERMOSTAT_UI_CONF_CLUSTER(cltype, clattributes, clcommands) \
    THERMOSTAT_UI_CONF_CLUSTER_##cltype(clattributes, clcommands)

#define THERMOSTAT_UI_CONF_CLUSTER_ZCL_CLIENT_CLUSTER_TYPE_FLASH(clattributes, clcommands) \
{ \
   .id = THERMOSTAT_UI_CONF_CLUSTER_ID, \
   .options = { \
                .type = ZCL_CLIENT_CLUSTER_TYPE, \
                .security = ZCL_NETWORK_KEY_CLUSTER_SECURITY, \
              }, \
   .attributesAmount = ZCL_THERMOSTAT_UI_CONF_CLUSTER_CLIENT_ATTRIBTUES_AMOUNT, \
   .attributes = (uint8_t *)clattributes, \
   .commandsAmount = ZCL_THERMOSTAT_UI_CONF_CLUSTER_COMMANDS_AMOUNT, \
   .commands = (FLASH_PTR uint8_t *)clcommands \
}

#define THERMOSTAT_UI_CONF_CLUSTER_ZCL_SERVER_CLUSTER_TYPE_FLASH(clattributes, clcommands) \
{ \
   .id = THERMOSTAT_UI_CONF_CLUSTER_ID, \
   .options = { \
                .type = ZCL_SERVER_CLUSTER_TYPE, \
                .security = ZCL_NETWORK_KEY_CLUSTER_SECURITY, \
              }, \
   .attributesAmount = ZCL_THERMOSTAT_UI_CONF_CLUSTER_SERVER_ATTRIBUTES_AMOUNT, \
   .attributes = (uint8_t *)clattributes, \
   .commandsAmount = ZCL_THERMOSTAT_UI_CONF_CLUSTER_COMMANDS_AMOUNT, \
   .commands = (FLASH_PTR uint8_t *)clcommands \
}

#define DEFINE_THERMOSTAT_UI_CONF_CLUSTER_FLASH(cltype, clattributes, clcommands) \
    THERMOSTAT_UI_CONF_CLUSTER_##cltype##_FLASH(clattributes, clcommands)

/******************************************************************************
                    Types section
******************************************************************************/
typedef enum
{
  ZCL_TEMPERATURE_IN_CELSIUS = 0x00,
  ZCL_TEMPERATURE_IN_FAHRENHEIT = 0x01
} ZCL_ThUiConfTemperatureDisplayMode_t;

typedef enum
{
  ZCL_NO_LOCKOUT = 0x00,
  ZCL_LEVEL_1_LOCKOUT = 0x01,
  ZCL_LEVEL_2_LOCKOUT = 0x02,
  ZCL_LEVEL_3_LOCKOUT = 0x03,
  ZCL_LEVEL_4_LOCKOUT = 0x04,
  ZCL_LEVEL_5_LOCKOUT = 0x05
} ZCL_ThUiConfKeypadLockOut_t;

typedef enum
{
  ZCL_ENABLE_LOCAL_SCHEDULE_PROGRAMMING = 0x00,
  ZCL_DISABLE_LOCAL_SCHEDULE_PROGRAMMING = 0x01
} ZCL_ThUiConfScheduleProgVisibility_t;

BEGIN_PACK
/***************************************************************************//**
\brief Thermostat Cluster server's attributes
*******************************************************************************/

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
  } temperatureDisplayMode;
  
  struct PACK
  {
    ZCL_AttributeId_t id;
    uint8_t type;
    uint8_t properties;
    uint8_t value;
    uint8_t minVal;
    uint8_t maxVal;
  } keypadLockOut;
  
   struct PACK
  {
    ZCL_AttributeId_t id;
    uint8_t type;
    uint8_t properties;
    uint8_t value;
    uint8_t minVal;
    uint8_t maxVal;
  } scheduleProgVisibility;

} ZCL_ThermostatUiConfClusterServerAttributes_t;

END_PACK
#endif /* _ZCLTHERMOSTATUICONFCLUSTER_H */
