/*****************************************************************************
  \file  zclPowerConfigurationCluster.h

  \brief
    The file describes the types and interface of the Power Configuration cluster

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    18.01.2010 I.Vagulin - Created
******************************************************************************/


#ifndef _ZCLPOWERCONFIGURATIONCLUSTER_H_
#define _ZCLPOWERCONFIGURATIONCLUSTER_H_

/*******************************************************************************
                   Includes section
*******************************************************************************/
#include <sysTypes.h>
#include <clusters.h>

/*******************************************************************************
                   Define(s) section
*******************************************************************************/

// Default values
#define ZCL_DS_DEFAULT_BATTERY_VOLTAGE                   0
#define ZCL_DS_DEFAULT_BATTERY_PERCENTAGE                0
#define ZCL_DS_DEFAULT_BATTERY_SIZE                      0xFF
#define ZCL_DS_DEFAULT_BATTERY_QUANDITY                  0
#define ZCL_DS_DEFAULT_BATTERY_ALARM_MASK                0
#define ZCL_DS_DEFAULT_BATTERY_ALARM_STATE               0
#define ZCL_DS_DEFAULT_BATTERY_MIN_THREASHOLD            0
#define ZCL_DS_DEFAULT_BATTERY_THREASHOLD_1          0
#define ZCL_DS_DEFAULT_BATTERY_THREASHOLD_2          0
#define ZCL_DS_DEFAULT_BATTERY_THREASHOLD_3          0

/**
* \brief server attributes amount
*/
#define ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_ATTRIBUTES_AMOUNT 44

/**
* \brief server commands amount
*/
#define ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_COMMANDS_AMOUNT 0

/**
* \brief client attributes amount
*/
#define ZCL_POWER_CONFIGURATION_CLUSTER_CLIENT_ATTRIBUTES_AMOUNT 0

/**
* \brief client commands amount
*/
#define ZCL_POWER_CONFIGURATION_CLUSTER_CLIENT_COMMANDS_AMOUNT 0


/**
* \brief Mains supported attribute id
*/
#define ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_MAINS_VOLTAGE_ATTRIBUTE_ID                   CCPU_TO_LE16(0x0000)
#define ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_MAINS_FREQUENCY_ATTRIBUTE_ID                 CCPU_TO_LE16(0x0001)
#define ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_MAINS_ALARM_MASK_ATTRIBUTE_ID                CCPU_TO_LE16(0x0010)
#define ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_MAINS_VOLTAGE_MIN_THRESHOLD_ATTRIBUTE_ID     CCPU_TO_LE16(0x0011)
#define ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_MAINS_VOLTAGE_MAX_THRESHOLD_ATTRIBUTE_ID     CCPU_TO_LE16(0x0012)
#define ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_MAINS_VOLTAGE_DWELL_TRIP_POINT_ATTRIBUTE_ID  CCPU_TO_LE16(0x0013)

/***************************************************************************//**
\brief Battery Source 1 Information 
*******************************************************************************/
#define ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_1_VOLTAGE_ID                  CCPU_TO_LE16(0x0020)
#define ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_1_PERCENTAGE_REMAINING_ID     CCPU_TO_LE16(0x0021)

/***************************************************************************//**
\brief Battery Source 1 Settings 
*******************************************************************************/
#define ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_1_MANUFACTURER_ID             CCPU_TO_LE16(0x0030)
#define ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_1_SIZE_ID                     CCPU_TO_LE16(0x0031)
#define ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_1_AHR_RATING_ID               CCPU_TO_LE16(0x0032)
#define ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_1_QUANDITY_ID                 CCPU_TO_LE16(0x0033)
#define ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_1_RATED_VOLTAGE_ID            CCPU_TO_LE16(0x0034)
#define ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_1_ALARM_MASK_ID               CCPU_TO_LE16(0x0035)
#define ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_1_VOLTAGE_MIN_THRESHOLD_ID    CCPU_TO_LE16(0x0036)
#define ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_1_VOLTAGE_THRESHOLD_1_ID      CCPU_TO_LE16(0x0037)
#define ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_1_VOLTAGE_THRESHOLD_2_ID      CCPU_TO_LE16(0x0038)
#define ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_1_VOLTAGE_THRESHOLD_3_ID      CCPU_TO_LE16(0x0039)
#define ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_1_PERCENTAGE_MIN_THRESHOLD_ID CCPU_TO_LE16(0x003A)
#define ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_1_PERCENTAGE_THRESHOLD_1_ID   CCPU_TO_LE16(0x003B)
#define ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_1_PERCENTAGE_THRESHOLD_2_ID   CCPU_TO_LE16(0x003C)
#define ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_1_PERCENTAGE_THRESHOLD_3_ID   CCPU_TO_LE16(0x003D)
#define ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_1_ALARM_STATE_ID              CCPU_TO_LE16(0x003E)

/***************************************************************************//**
\brief Battery Source 2 Information 
*******************************************************************************/
#define ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_2_VOLTAGE_ID                  CCPU_TO_LE16(0x0040)
#define ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_2_PERCENTAGE_REMAINING_ID     CCPU_TO_LE16(0x0041)

/***************************************************************************//**
\brief Battery Source 2 Settings 
*******************************************************************************/
#define ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_2_MANUFACTURER_ID             CCPU_TO_LE16(0x0050)
#define ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_2_SIZE_ID                     CCPU_TO_LE16(0x0051)
#define ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_2_AHR_RATING_ID               CCPU_TO_LE16(0x0052)
#define ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_2_QUANDITY_ID                 CCPU_TO_LE16(0x0053)
#define ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_2_RATED_VOLTAGE_ID            CCPU_TO_LE16(0x0054)
#define ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_2_ALARM_MASK_ID               CCPU_TO_LE16(0x0055)
#define ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_2_VOLTAGE_MIN_THRESHOLD_ID    CCPU_TO_LE16(0x0056)
#define ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_2_VOLTAGE_THRESHOLD_1_ID      CCPU_TO_LE16(0x0057)
#define ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_2_VOLTAGE_THRESHOLD_2_ID      CCPU_TO_LE16(0x0058)
#define ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_2_VOLTAGE_THRESHOLD_3_ID      CCPU_TO_LE16(0x0059)
#define ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_2_PERCENTAGE_MIN_THRESHOLD_ID CCPU_TO_LE16(0x005A)
#define ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_2_PERCENTAGE_THRESHOLD_1_ID   CCPU_TO_LE16(0x005B)
#define ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_2_PERCENTAGE_THRESHOLD_2_ID   CCPU_TO_LE16(0x005C)
#define ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_2_PERCENTAGE_THRESHOLD_3_ID   CCPU_TO_LE16(0x005D)
#define ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_2_ALARM_STATE_ID              CCPU_TO_LE16(0x005E)

/***************************************************************************//**
\brief Battery Source 3 Information 
*******************************************************************************/
#define ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_3_VOLTAGE_ID                  CCPU_TO_LE16(0x0060)
#define ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_3_PERCENTAGE_REMAINING_ID     CCPU_TO_LE16(0x0061)

/***************************************************************************//**
\brief Battery Source 3 Settings 
*******************************************************************************/
#define ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_3_MANUFACTURER_ID             CCPU_TO_LE16(0x0070)
#define ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_3_SIZE_ID                     CCPU_TO_LE16(0x0071)
#define ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_3_AHR_RATING_ID               CCPU_TO_LE16(0x0072)
#define ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_3_QUANDITY_ID                 CCPU_TO_LE16(0x0073)
#define ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_3_RATED_VOLTAGE_ID            CCPU_TO_LE16(0x0074)
#define ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_3_ALARM_MASK_ID               CCPU_TO_LE16(0x0075)
#define ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_3_VOLTAGE_MIN_THRESHOLD_ID    CCPU_TO_LE16(0x0076)
#define ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_3_VOLTAGE_THRESHOLD_1_ID      CCPU_TO_LE16(0x0077)
#define ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_3_VOLTAGE_THRESHOLD_2_ID      CCPU_TO_LE16(0x0078)
#define ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_3_VOLTAGE_THRESHOLD_3_ID      CCPU_TO_LE16(0x0079)
#define ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_3_PERCENTAGE_MIN_THRESHOLD_ID CCPU_TO_LE16(0x007A)
#define ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_3_PERCENTAGE_THRESHOLD_1_ID   CCPU_TO_LE16(0x007B)
#define ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_3_PERCENTAGE_THRESHOLD_2_ID   CCPU_TO_LE16(0x007C)
#define ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_3_PERCENTAGE_THRESHOLD_3_ID   CCPU_TO_LE16(0x007D)
#define ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_3_ALARM_STATE_ID              CCPU_TO_LE16(0x007E)

/**
 * \brief server define attributes macros
*/
#define ZCL_DEFINE_POWER_CONFIGURATION_CLUSTER_SERVER_ATTRIBUTES(reportMin, reportMax) \
  DEFINE_ATTRIBUTE(mainsVoltage, ZCL_READONLY_ATTRIBUTE,  ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_MAINS_VOLTAGE_ATTRIBUTE_ID,  ZCL_U16BIT_DATA_TYPE_ID), \
  DEFINE_ATTRIBUTE(mainsFrequency, ZCL_READONLY_ATTRIBUTE, ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_MAINS_FREQUENCY_ATTRIBUTE_ID, ZCL_U8BIT_DATA_TYPE_ID), \
  DEFINE_ATTRIBUTE(batterySourceOneSize, ZCL_READWRITE_ATTRIBUTE, ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_1_SIZE_ID, ZCL_8BIT_ENUM_DATA_TYPE_ID), \
  DEFINE_ATTRIBUTE(batterySourceOneQuantity, ZCL_READWRITE_ATTRIBUTE, ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_1_QUANDITY_ID, ZCL_U8BIT_DATA_TYPE_ID), \
  DEFINE_ATTRIBUTE_WITH_BOUNDARY(batterySourceOneAlarmMask, ZCL_READWRITE_ATTRIBUTE, ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_1_ALARM_MASK_ID, ZCL_8BIT_BITMAP_DATA_TYPE_ID, 0x00, 0x0F), \
  DEFINE_REPORTABLE_ATTRIBUTE_WITH_BOUNDARY(batterySourceOneAlarmState, ZCL_READONLY_ATTRIBUTE , ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_1_ALARM_STATE_ID, ZCL_32BIT_BITMAP_DATA_TYPE_ID, reportMin, reportMax, 0x00, 0x00FFFFFF), \
  DEFINE_ATTRIBUTE(batterySourceOneVoltage, ZCL_READONLY_ATTRIBUTE , ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_1_VOLTAGE_ID, ZCL_U8BIT_DATA_TYPE_ID), \
  DEFINE_ATTRIBUTE(batterySourceOneVoltageMinThreshold, ZCL_READWRITE_ATTRIBUTE , ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_1_VOLTAGE_MIN_THRESHOLD_ID, ZCL_U8BIT_DATA_TYPE_ID), \
  DEFINE_ATTRIBUTE(batterySourceOneVoltageThreshold_1, ZCL_READWRITE_ATTRIBUTE , ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_1_VOLTAGE_THRESHOLD_1_ID, ZCL_U8BIT_DATA_TYPE_ID), \
  DEFINE_ATTRIBUTE(batterySourceOneVoltageThreshold_2, ZCL_READWRITE_ATTRIBUTE , ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_1_VOLTAGE_THRESHOLD_2_ID, ZCL_U8BIT_DATA_TYPE_ID), \
  DEFINE_ATTRIBUTE(batterySourceOneVoltageThreshold_3, ZCL_READWRITE_ATTRIBUTE , ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_1_VOLTAGE_THRESHOLD_3_ID, ZCL_U8BIT_DATA_TYPE_ID), \
  DEFINE_REPORTABLE_ATTRIBUTE(batterySourceOnePercentageRemaining, ZCL_READONLY_ATTRIBUTE , ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_1_PERCENTAGE_REMAINING_ID, ZCL_U8BIT_DATA_TYPE_ID, reportMin, reportMax), \
  DEFINE_ATTRIBUTE(batterySourceOnePercentageMinThreshold, ZCL_READWRITE_ATTRIBUTE , ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_1_PERCENTAGE_MIN_THRESHOLD_ID, ZCL_U8BIT_DATA_TYPE_ID), \
  DEFINE_ATTRIBUTE(batterySourceOnePercentageThreshold_1, ZCL_READWRITE_ATTRIBUTE , ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_1_PERCENTAGE_THRESHOLD_1_ID, ZCL_U8BIT_DATA_TYPE_ID), \
  DEFINE_ATTRIBUTE(batterySourceOnePercentageThreshold_2, ZCL_READWRITE_ATTRIBUTE , ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_1_PERCENTAGE_THRESHOLD_2_ID, ZCL_U8BIT_DATA_TYPE_ID), \
  DEFINE_ATTRIBUTE(batterySourceOnePercentageThreshold_3, ZCL_READWRITE_ATTRIBUTE , ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_1_PERCENTAGE_THRESHOLD_3_ID, ZCL_U8BIT_DATA_TYPE_ID), \
  DEFINE_ATTRIBUTE(batterySourceTwoSize, ZCL_READWRITE_ATTRIBUTE, ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_2_SIZE_ID, ZCL_8BIT_ENUM_DATA_TYPE_ID), \
  DEFINE_ATTRIBUTE(batterySourceTwoQuantity, ZCL_READWRITE_ATTRIBUTE, ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_2_QUANDITY_ID, ZCL_U8BIT_DATA_TYPE_ID), \
  DEFINE_ATTRIBUTE_WITH_BOUNDARY(batterySourceTwoAlarmMask, ZCL_READWRITE_ATTRIBUTE, ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_2_ALARM_MASK_ID, ZCL_8BIT_BITMAP_DATA_TYPE_ID, 0x00, 0x0F), \
  DEFINE_REPORTABLE_ATTRIBUTE_WITH_BOUNDARY(batterySourceTwoAlarmState, ZCL_READONLY_ATTRIBUTE , ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_2_ALARM_STATE_ID, ZCL_32BIT_BITMAP_DATA_TYPE_ID, reportMin, reportMax, 0x00, 0x00FFFFFF), \
  DEFINE_ATTRIBUTE(batterySourceTwoVoltage, ZCL_READONLY_ATTRIBUTE , ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_2_VOLTAGE_ID, ZCL_U8BIT_DATA_TYPE_ID), \
  DEFINE_ATTRIBUTE(batterySourceTwoVoltageMinThreshold, ZCL_READWRITE_ATTRIBUTE , ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_2_VOLTAGE_MIN_THRESHOLD_ID, ZCL_U8BIT_DATA_TYPE_ID), \
  DEFINE_ATTRIBUTE(batterySourceTwoVoltageThreshold_1, ZCL_READWRITE_ATTRIBUTE , ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_2_VOLTAGE_THRESHOLD_1_ID, ZCL_U8BIT_DATA_TYPE_ID), \
  DEFINE_ATTRIBUTE(batterySourceTwoVoltageThreshold_2, ZCL_READWRITE_ATTRIBUTE , ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_2_VOLTAGE_THRESHOLD_2_ID, ZCL_U8BIT_DATA_TYPE_ID), \
  DEFINE_ATTRIBUTE(batterySourceTwoVoltageThreshold_3, ZCL_READWRITE_ATTRIBUTE , ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_2_VOLTAGE_THRESHOLD_3_ID, ZCL_U8BIT_DATA_TYPE_ID), \
  DEFINE_REPORTABLE_ATTRIBUTE(batterySourceTwoPercentageRemaining, ZCL_READONLY_ATTRIBUTE , ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_2_PERCENTAGE_REMAINING_ID, ZCL_U8BIT_DATA_TYPE_ID, reportMin, reportMax), \
  DEFINE_ATTRIBUTE(batterySourceTwoPercentageMinThreshold, ZCL_READWRITE_ATTRIBUTE , ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_2_PERCENTAGE_MIN_THRESHOLD_ID, ZCL_U8BIT_DATA_TYPE_ID), \
  DEFINE_ATTRIBUTE(batterySourceTwoPercentageThreshold_1, ZCL_READWRITE_ATTRIBUTE , ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_2_PERCENTAGE_THRESHOLD_1_ID, ZCL_U8BIT_DATA_TYPE_ID), \
  DEFINE_ATTRIBUTE(batterySourceTwoPercentageThreshold_2, ZCL_READWRITE_ATTRIBUTE , ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_2_PERCENTAGE_THRESHOLD_2_ID, ZCL_U8BIT_DATA_TYPE_ID), \
  DEFINE_ATTRIBUTE(batterySourceTwoPercentageThreshold_3, ZCL_READWRITE_ATTRIBUTE , ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_2_PERCENTAGE_THRESHOLD_3_ID, ZCL_U8BIT_DATA_TYPE_ID), \
  DEFINE_ATTRIBUTE(batterySourceThreeSize, ZCL_READWRITE_ATTRIBUTE, ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_3_SIZE_ID, ZCL_8BIT_ENUM_DATA_TYPE_ID), \
  DEFINE_ATTRIBUTE(batterySourceThreeQuantity, ZCL_READWRITE_ATTRIBUTE, ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_3_QUANDITY_ID, ZCL_U8BIT_DATA_TYPE_ID), \
  DEFINE_ATTRIBUTE_WITH_BOUNDARY(batterySourceThreeAlarmMask, ZCL_READWRITE_ATTRIBUTE, ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_3_ALARM_MASK_ID, ZCL_8BIT_BITMAP_DATA_TYPE_ID, 0x00, 0x0F), \
  DEFINE_REPORTABLE_ATTRIBUTE_WITH_BOUNDARY(batterySourceThreeAlarmState, ZCL_READONLY_ATTRIBUTE , ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_3_ALARM_STATE_ID, ZCL_32BIT_BITMAP_DATA_TYPE_ID, reportMin, reportMax, 0x00, 0x00FFFFFF), \
  DEFINE_ATTRIBUTE(batterySourceThreeVoltage, ZCL_READONLY_ATTRIBUTE , ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_3_VOLTAGE_ID, ZCL_U8BIT_DATA_TYPE_ID), \
  DEFINE_ATTRIBUTE(batterySourceThreeVoltageMinThreshold, ZCL_READWRITE_ATTRIBUTE , ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_3_VOLTAGE_MIN_THRESHOLD_ID, ZCL_U8BIT_DATA_TYPE_ID), \
  DEFINE_ATTRIBUTE(batterySourceThreeVoltageThreshold_1, ZCL_READWRITE_ATTRIBUTE , ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_3_VOLTAGE_THRESHOLD_1_ID, ZCL_U8BIT_DATA_TYPE_ID), \
  DEFINE_ATTRIBUTE(batterySourceThreeVoltageThreshold_2, ZCL_READWRITE_ATTRIBUTE , ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_3_VOLTAGE_THRESHOLD_2_ID, ZCL_U8BIT_DATA_TYPE_ID), \
  DEFINE_ATTRIBUTE(batterySourceThreeVoltageThreshold_3, ZCL_READWRITE_ATTRIBUTE , ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_3_VOLTAGE_THRESHOLD_3_ID, ZCL_U8BIT_DATA_TYPE_ID), \
  DEFINE_REPORTABLE_ATTRIBUTE(batterySourceThreePercentageRemaining, ZCL_READONLY_ATTRIBUTE , ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_3_PERCENTAGE_REMAINING_ID, ZCL_U8BIT_DATA_TYPE_ID, reportMin, reportMax), \
  DEFINE_ATTRIBUTE(batterySourceThreePercentageMinThreshold, ZCL_READWRITE_ATTRIBUTE , ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_3_PERCENTAGE_MIN_THRESHOLD_ID, ZCL_U8BIT_DATA_TYPE_ID), \
  DEFINE_ATTRIBUTE(batterySourceThreePercentageThreshold_1, ZCL_READWRITE_ATTRIBUTE , ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_3_PERCENTAGE_THRESHOLD_1_ID, ZCL_U8BIT_DATA_TYPE_ID), \
  DEFINE_ATTRIBUTE(batterySourceThreePercentageThreshold_2, ZCL_READWRITE_ATTRIBUTE , ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_3_PERCENTAGE_THRESHOLD_2_ID, ZCL_U8BIT_DATA_TYPE_ID), \
  DEFINE_ATTRIBUTE(batterySourceThreePercentageThreshold_3, ZCL_READWRITE_ATTRIBUTE , ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_BATTERY_SOURCE_3_PERCENTAGE_THRESHOLD_3_ID, ZCL_U8BIT_DATA_TYPE_ID),

#define POWER_CONFIGURATION_CLUSTER_ZCL_SERVER_CLUSTER_TYPE(clattributes, clcommands)  \
  { \
    .id = POWER_CONFIGURATION_CLUSTER_ID, \
    .options = {.type = ZCL_SERVER_CLUSTER_TYPE, .security = ZCL_NETWORK_KEY_CLUSTER_SECURITY}, \
    .attributesAmount = ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_ATTRIBUTES_AMOUNT, \
    .attributes = (uint8_t *) (clattributes), \
    .commandsAmount = ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_COMMANDS_AMOUNT, \
    .commands = (uint8_t *)clcommands \
  }

#define POWER_CONFIGURATION_CLUSTER_ZCL_CLIENT_CLUSTER_TYPE(clattributes, clcommands) \
  { \
    .id = POWER_CONFIGURATION_CLUSTER_ID, \
    .options = {.type = ZCL_CLIENT_CLUSTER_TYPE, .security = ZCL_NETWORK_KEY_CLUSTER_SECURITY}, \
    .attributesAmount = ZCL_POWER_CONFIGURATION_CLUSTER_CLIENT_ATTRIBUTES_AMOUNT, \
    .attributes = NULL, \
    .commandsAmount = ZCL_POWER_CONFIGURATION_CLUSTER_CLIENT_COMMANDS_AMOUNT, \
    .commands = (uint8_t *)clcommands \
  }

#define DEFINE_POWER_CONFIGURATION_CLUSTER(cltype, clattributes, clcommands) \
    POWER_CONFIGURATION_CLUSTER_##cltype(clattributes, clcommands)
  
#define ZCL_DEFINE_POWER_CONFIGURATION_CLUSTER_SERVER_FLASH(clattributes) ZCL_DEFINE_POWER_CONFIGURATION_CLUSTER_SERVER(clattributes)
#define ZCL_DEFINE_POWER_CONFIGURATION_CLUSTER_CLIENT_FLASH() ZCL_DEFINE_POWER_CONFIGURATION_CLUSTER_CLIENT()

#define BATTERY_ONE_MIN_THRESHOLD_MASK   (1ul<<0)
#define BATTERY_ONE_THRESHOLD_1_MASK     (1ul<<1)
#define BATTERY_ONE_THRESHOLD_2_MASK     (1ul<<2)
#define BATTERY_ONE_THRESHOLD_3_MASK     (1ul<<3)
#define BATTERY_TWO_MIN_THRESHOLD_MASK   (1ul<<10)
#define BATTERY_TWO_THRESHOLD_1_MASK     (1ul<<11)
#define BATTERY_TWO_THRESHOLD_2_MASK     (1ul<<12)
#define BATTERY_TWO_THRESHOLD_3_MASK     (1ul<<13)
#define BATTERY_THREE_MIN_THRESHOLD_MASK (1ul<<20)
#define BATTERY_THREE_THRESHOLD_1_MASK   (1ul<<21)
#define BATTERY_THREE_THRESHOLD_2_MASK   (1ul<<22)
#define BATTERY_THREE_THRESHOLD_3_MASK   (1ul<<23)

/******************************************************************************
                    Types section
******************************************************************************/
typedef enum _ZCL_PwrBatteryAlarmCode_t
{
  ZCL_BATTERY_MIN_THRESHOLD_SOURCE_1_ALARM  = 0x10,
  ZCL_BATTERY_THRESHOLD_1_SOURCE_1_ALARM    = 0x11,
  ZCL_BATTERY_THRESHOLD_2_SOURCE_1_ALARM    = 0x12,
  ZCL_BATTERY_THRESHOLD_3_SOURCE_1_ALARM    = 0x13,
  ZCL_BATTERY_MIN_THRESHOLD_SOURCE_2_ALARM  = 0x20,
  ZCL_BATTERY_THRESHOLD_1_SOURCE_2_ALARM    = 0x21,
  ZCL_BATTERY_THRESHOLD_2_SOURCE_2_ALARM    = 0x22,
  ZCL_BATTERY_THRESHOLD_3_SOURCE_2_ALARM    = 0x23,
  ZCL_BATTERY_MIN_THRESHOLD_SOURCE_3_ALARM  = 0x30,
  ZCL_BATTERY_THRESHOLD_1_SOURCE_3_ALARM    = 0x31,
  ZCL_BATTERY_THRESHOLD_2_SOURCE_3_ALARM    = 0x32,
  ZCL_BATTERY_THRESHOLD_3_SOURCE_3_ALARM    = 0x33,
  ZCL_BATTERY_THRESHOLD_NO_ALARM            = 0xFF,
}ZCL_PwrBatteryAlarmCode_t;

typedef enum _ZCL_PwrBatterySize_t
{
  ZCL_BATTERY_NO_BATTERY = 0x00,
  ZCL_BATTERY_BUILT_IN   = 0x01,
  ZCL_BATTERY_OTHER      = 0x02,
  ZCL_BATTERY_AA         = 0x03,
  ZCL_BATTERY_AAA        = 0x04,
  ZCL_BATTERY_C          = 0x05,
  ZCL_BATTERY_D          = 0x06,
  ZCL_BATTERY_RES1       = 0x07,
  ZCL_BATTERY_RES2       = 0xfe,  
  ZCL_BATTERY_UNKNOWN    = 0xFF,
}ZCL_PwrBatterySize_t;

typedef enum _ZCL_PwrMainsAlarmMask_t
{
  ZCL_MAINS_VOLTAGE_TOO_LOW  = (1<<0),
  ZCL_MAINS_VOLTAGE_TOO_HIGH = (1<<1),
}ZCL_PwrMainsAlarmMask_t;

/***************************************************************************//**
\brief Power Configuration Cluster server's attributes
*******************************************************************************/
BEGIN_PACK
typedef struct PACK {
  struct PACK {
    ZCL_AttributeId_t   id;
    uint8_t             type;
    uint8_t             properties;
    uint16_t            value;
  } mainsVoltage ;

  struct PACK {
    ZCL_AttributeId_t   id;
    uint8_t             type;
    uint8_t             properties;
    uint8_t             value;
  } mainsFrequency ;
  
  struct PACK {
    ZCL_AttributeId_t    id;
    uint8_t              type;
    uint8_t              properties;
    ZCL_PwrBatterySize_t value;
  } batterySourceOneSize ;
  
  struct PACK {
    ZCL_AttributeId_t   id;
    uint8_t             type;
    uint8_t             properties;
    uint8_t             value;
  } batterySourceOneQuantity ;
  
  struct PACK {
    ZCL_AttributeId_t         id;
    uint8_t                   type;
    uint8_t                   properties;
    struct PACK
    {
      LITTLE_ENDIAN_OCTET(5, (
        uint8_t batteryTooLow :1,
        uint8_t batteryAlarm1 :1,
        uint8_t batteryAlarm2 :1,
        uint8_t batteryAlarm3 :1,
        uint8_t reserved :4
      ))
    }value;
    uint8_t minVal;
    uint8_t maxVal;
  } batterySourceOneAlarmMask ;
  
  struct PACK {
    ZCL_AttributeId_t id;
    uint8_t           type;
    uint8_t           properties;
    uint32_t          value;
    ZCL_ReportTime_t reportCounter;     //!<For internal use only
    ZCL_ReportTime_t minReportInterval; //!<Minimum reporting interval field value
    ZCL_ReportTime_t maxReportInterval; //!<Maximum reporting interval field value
    uint32_t reportableChange;           //!<Reporting change field value
    ZCL_ReportTime_t timeoutPeriod;     //!<Timeout period field value
    uint32_t lastReportedValue;          //!<Last reported value
    uint32_t          minVal;
    uint32_t          maxVal;
  } batterySourceOneAlarmState ;
  
  struct PACK {
    ZCL_AttributeId_t id;
    uint8_t           type;
    uint8_t           properties;
    uint8_t           value;
  } batterySourceOneVoltage ;
  
  struct PACK {
    ZCL_AttributeId_t id;
    uint8_t           type;
    uint8_t           properties;
    uint8_t           value;
  } batterySourceOneVoltageMinThreshold ;
  
  struct PACK {
    ZCL_AttributeId_t id;
    uint8_t           type;
    uint8_t           properties;
    uint8_t           value;
  } batterySourceOneVoltageThreshold_1 ;
  
  struct PACK {
    ZCL_AttributeId_t id;
    uint8_t           type;
    uint8_t           properties;
    uint8_t           value;
  } batterySourceOneVoltageThreshold_2 ;
  
  struct PACK {
    ZCL_AttributeId_t id;
    uint8_t           type;
    uint8_t           properties;
    uint8_t           value;
  } batterySourceOneVoltageThreshold_3 ;
  
  struct PACK {
    ZCL_AttributeId_t id;
    uint8_t           type;
    uint8_t           properties;
    uint8_t           value;
    ZCL_ReportTime_t  reportCounter;     //!<For internal use only
    ZCL_ReportTime_t  minReportInterval; //!<Minimum reporting interval field value
    ZCL_ReportTime_t  maxReportInterval; //!<Maximum reporting interval field value
    uint8_t           reportableChange;  //!<Reporting change field value
    ZCL_ReportTime_t  timeoutPeriod;     //!<Timeout period field value
    uint8_t           lastReportedValue; //!<Last reported value
  } batterySourceOnePercentageRemaining ;

  struct PACK {
    ZCL_AttributeId_t id;
    uint8_t           type;
    uint8_t           properties;
    uint8_t           value;
  } batterySourceOnePercentageMinThreshold;
  
  struct PACK {
    ZCL_AttributeId_t   id;
    uint8_t             type;
    uint8_t             properties;
    uint8_t             value;
  } batterySourceOnePercentageThreshold_1 ;
  
  struct PACK {
    ZCL_AttributeId_t   id;
    uint8_t             type;
    uint8_t             properties;
    uint8_t             value;
  } batterySourceOnePercentageThreshold_2 ;
  
  struct PACK {
    ZCL_AttributeId_t   id;
    uint8_t             type;
    uint8_t             properties;
    uint8_t             value;
  } batterySourceOnePercentageThreshold_3 ;
  
  struct PACK {
    ZCL_AttributeId_t    id;
    uint8_t              type;
    uint8_t              properties;
    ZCL_PwrBatterySize_t value;
  } batterySourceTwoSize ;
  
  struct PACK {
    ZCL_AttributeId_t   id;
    uint8_t             type;
    uint8_t             properties;
    uint8_t             value;
  } batterySourceTwoQuantity ;
  
  struct PACK {
    ZCL_AttributeId_t         id;
    uint8_t                   type;
    uint8_t                   properties;
    struct PACK
    {
      LITTLE_ENDIAN_OCTET(5, (
        uint8_t batteryTooLow :1,
        uint8_t batteryAlarm1 :1,
        uint8_t batteryAlarm2 :1,
        uint8_t batteryAlarm3 :1,
        uint8_t reserved :4
      ))
    }value;
    uint8_t                   minVal;
    uint8_t                   maxVal;
  } batterySourceTwoAlarmMask ;
  
  struct PACK {
    ZCL_AttributeId_t   id;
    uint8_t             type;
    uint8_t             properties;
    uint32_t            value;
    ZCL_ReportTime_t reportCounter;     //!<For internal use only
    ZCL_ReportTime_t minReportInterval; //!<Minimum reporting interval field value
    ZCL_ReportTime_t maxReportInterval; //!<Maximum reporting interval field value
    uint32_t reportableChange;           //!<Reporting change field value
    ZCL_ReportTime_t timeoutPeriod;     //!<Timeout period field value
    uint32_t lastReportedValue;          //!<Last reported value
    uint32_t            minVal;
    uint32_t            maxVal;
  } batterySourceTwoAlarmState ;
  
  struct PACK {
    ZCL_AttributeId_t id;
    uint8_t           type;
    uint8_t           properties;
    uint8_t           value;
  } batterySourceTwoVoltage ;
  
  struct PACK {
    ZCL_AttributeId_t id;
    uint8_t           type;
    uint8_t           properties;
    uint8_t           value;
  } batterySourceTwoVoltageMinThreshold ;
  
  struct PACK {
    ZCL_AttributeId_t id;
    uint8_t           type;
    uint8_t           properties;
    uint8_t           value;
  } batterySourceTwoVoltageThreshold_1 ;
  
  struct PACK {
    ZCL_AttributeId_t id;
    uint8_t           type;
    uint8_t           properties;
    uint8_t           value;
  } batterySourceTwoVoltageThreshold_2 ;
  
  struct PACK {
    ZCL_AttributeId_t id;
    uint8_t           type;
    uint8_t           properties;
    uint8_t           value;
  } batterySourceTwoVoltageThreshold_3 ;
  
  struct PACK {
    ZCL_AttributeId_t id;
    uint8_t           type;
    uint8_t           properties;
    uint8_t           value;
    ZCL_ReportTime_t  reportCounter;     //!<For internal use only
    ZCL_ReportTime_t  minReportInterval; //!<Minimum reporting interval field value
    ZCL_ReportTime_t  maxReportInterval; //!<Maximum reporting interval field value
    uint8_t           reportableChange;  //!<Reporting change field value
    ZCL_ReportTime_t  timeoutPeriod;     //!<Timeout period field value
    uint8_t           lastReportedValue; //!<Last reported value
  } batterySourceTwoPercentageRemaining ;

  struct PACK {
    ZCL_AttributeId_t id;
    uint8_t           type;
    uint8_t           properties;
    uint8_t           value;
  } batterySourceTwoPercentageMinThreshold;
  
  struct PACK {
    ZCL_AttributeId_t   id;
    uint8_t             type;
    uint8_t             properties;
    uint8_t             value;
  } batterySourceTwoPercentageThreshold_1 ;
  
  struct PACK {
    ZCL_AttributeId_t   id;
    uint8_t             type;
    uint8_t             properties;
    uint8_t             value;
  } batterySourceTwoPercentageThreshold_2 ;
  
  struct PACK {
    ZCL_AttributeId_t   id;
    uint8_t             type;
    uint8_t             properties;
    uint8_t             value;
  } batterySourceTwoPercentageThreshold_3 ;

  struct PACK {
    ZCL_AttributeId_t    id;
    uint8_t              type;
    uint8_t              properties;
    ZCL_PwrBatterySize_t value;
  } batterySourceThreeSize ;
  
  struct PACK {
    ZCL_AttributeId_t   id;
    uint8_t             type;
    uint8_t             properties;
    uint8_t             value;
  } batterySourceThreeQuantity ;
  
  struct PACK {
    ZCL_AttributeId_t         id;
    uint8_t                   type;
    uint8_t                   properties;
    struct PACK
    {
      LITTLE_ENDIAN_OCTET(5, (
        uint8_t batteryTooLow :1,
        uint8_t batteryAlarm1 :1,
        uint8_t batteryAlarm2 :1,
        uint8_t batteryAlarm3 :1,
        uint8_t reserved :4
      ))
    }value;
    uint8_t                   minVal;
    uint8_t                   maxVal;
  } batterySourceThreeAlarmMask ;
  
  struct PACK {
    ZCL_AttributeId_t   id;
    uint8_t             type;
    uint8_t             properties;
    uint32_t            value;
    ZCL_ReportTime_t reportCounter;     //!<For internal use only
    ZCL_ReportTime_t minReportInterval; //!<Minimum reporting interval field value
    ZCL_ReportTime_t maxReportInterval; //!<Maximum reporting interval field value
    uint32_t reportableChange;           //!<Reporting change field value
    ZCL_ReportTime_t timeoutPeriod;     //!<Timeout period field value
    uint32_t lastReportedValue;          //!<Last reported value
    uint32_t            minVal;
    uint32_t            maxVal;
  } batterySourceThreeAlarmState ;
  
  struct PACK {
    ZCL_AttributeId_t id;
    uint8_t           type;
    uint8_t           properties;
    uint8_t           value;
  } batterySourceThreeVoltage ;
  
  struct PACK {
    ZCL_AttributeId_t id;
    uint8_t           type;
    uint8_t           properties;
    uint8_t           value;
  } batterySourceThreeVoltageMinThreshold ;
  
  struct PACK {
    ZCL_AttributeId_t id;
    uint8_t           type;
    uint8_t           properties;
    uint8_t           value;
  } batterySourceThreeVoltageThreshold_1 ;
  
  struct PACK {
    ZCL_AttributeId_t id;
    uint8_t           type;
    uint8_t           properties;
    uint8_t           value;
  } batterySourceThreeVoltageThreshold_2 ;
  
  struct PACK {
    ZCL_AttributeId_t id;
    uint8_t           type;
    uint8_t           properties;
    uint8_t           value;
  } batterySourceThreeVoltageThreshold_3 ;
  
  struct PACK {
    ZCL_AttributeId_t id;
    uint8_t           type;
    uint8_t           properties;
    uint8_t           value;
    ZCL_ReportTime_t  reportCounter;     //!<For internal use only
    ZCL_ReportTime_t  minReportInterval; //!<Minimum reporting interval field value
    ZCL_ReportTime_t  maxReportInterval; //!<Maximum reporting interval field value
    uint8_t           reportableChange;  //!<Reporting change field value
    ZCL_ReportTime_t  timeoutPeriod;     //!<Timeout period field value
    uint8_t           lastReportedValue; //!<Last reported value
  } batterySourceThreePercentageRemaining ;

  struct PACK {
    ZCL_AttributeId_t id;
    uint8_t           type;
    uint8_t           properties;
    uint8_t           value;
  } batterySourceThreePercentageMinThreshold;
  
  struct PACK {
    ZCL_AttributeId_t   id;
    uint8_t             type;
    uint8_t             properties;
    uint8_t             value;
  } batterySourceThreePercentageThreshold_1 ;
  
  struct PACK {
    ZCL_AttributeId_t   id;
    uint8_t             type;
    uint8_t             properties;
    uint8_t             value;
  } batterySourceThreePercentageThreshold_2 ;
  
  struct PACK {
    ZCL_AttributeId_t   id;
    uint8_t             type;
    uint8_t             properties;
    uint8_t             value;
  } batterySourceThreePercentageThreshold_3 ;
} ZCL_PowerConfigurationClusterServerAttributes_t ;
END_PACK

#endif /* _ZCLPOWERCONFIGURATIONCLUSTER_H_ */
