/***************************************************************************//**
  \file zclOccupancySensingCluster.h

  \brief
    The header file describes the ZCL Occupancy Sensing Cluster and its interface

    The file describes the types and interface of the ZCL Occupancy Sensing Configuration Cluster

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    12.02.09 D. Kasyanov - Created.
*******************************************************************************/

#ifndef _ZCLOCCUPANCYSENSINGCLUSTER_H
#define _ZCLOCCUPANCYSENSINGCLUSTER_H

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
 * \brief Occupancy Sensing cluster attributes values
*/

#define OCCUPANCY_ATTRIBUTE_VALUE_OCCUPIED 1
#define OCCUPANCY_ATTRIBUTE_VALUE_UNOCCUPIED 0

#define OCCUPANYC_SENSOR_TYPE_ATTRIBUTE_VALUE_PIR 0x00
#define OCCUPANYC_SENSOR_TYPE_ATTRIBUTE_VALUE_ULTRASONIC 0x01
#define OCCUPANYC_SENSOR_TYPE_ATTRIBITE_VALUE_PIR_AND_ULTRASONIC 0x02

/* Occupancy cluster attributes min ,max and default values */

#define ZCL_OCCUPANCY_SENSING_CL_OCCUPANCY_SER_ATTR_MIN_VAL                                       (0x00)
#define ZCL_OCCUPANCY_SENSING_CL_OCCUPANCY_SER_ATTR_MAX_VAL                                       (0x01)

#define ZCL_OCCUPANCY_SENSING_CL_OCCUPANCY_SENSOR_TYPE_SER_ATTR_MIN_VAL                           (0x00)
#define ZCL_OCCUPANCY_SENSING_CL_OCCUPANCY_SENSOR_TYPE_SER_ATTR_MAX_VAL                           (0xfe)

#define ZCL_OCCUPANCY_SENSING_CL_PIR_OCCUPIED_TO_UNOCCUPIED_DELAY_SER_ATTR_DEFAULT_VAL            (0x0000)
#define ZCL_OCCUPANCY_SENSING_CL_PIR_UNOCCUPIED_TO_OCCUPIED_DELAY_SER_ATTR_DEFAULT_VAL            (0x0000)
#define ZCL_OCCUPANCY_SENSING_CL_PIR_UNOCCUPIED_TO_OCCUPIED_THRESHOLD_SER_ATTR_DEFAULT_VAL        (0x001)

#define ZCL_OCCUPANCY_SENSING_CL_ULTRASONIC_OCCUPIED_TO_UNOCCUPIED_DELAY_SER_ATTR_DEFAULT_VAL     (0x0000)
#define ZCL_OCCUPANCY_SENSING_CL_ULTRASONIC_UNOCCUPIED_TO_OCCUPIED_DELAY_SER_ATTR_DEFAULT_VAL     (0x0000)
#define ZCL_OCCUPANCY_SENSING_CL_ULTRASONIC_UNOCCUPIED_TO_OCCUPIED_THRESHOLD_SER_ATTR_DEFAULT_VAL (0x01)

#define ZCL_OCCUPANCY_SENSING_CL_PIR_OCCUPIED_TO_UNOCCUPIED_DELAY_SER_ATTR_MIN_VAL                (0x0000)
#define ZCL_OCCUPANCY_SENSING_CL_PIR_OCCUPIED_TO_UNOCCUPIED_DELAY_SER_ATTR_MAX_VAL                (0xfffe)
#define ZCL_OCCUPANCY_SENSING_CL_PIR_UNOCCUPIED_TO_OCCUPIED_DELAY_SER_ATTR_MIN_VAL                (0x0000)
#define ZCL_OCCUPANCY_SENSING_CL_PIR_UNOCCUPIED_TO_OCCUPIED_DELAY_SER_ATTR_MAX_VAL                (0xfffe)
#define ZCL_OCCUPANCY_SENSING_CL_PIR_UNOCCUPIED_TO_OCCUPIED_THRESHOLD_SER_ATTR_MIN_VAL            (0x01)
#define ZCL_OCCUPANCY_SENSING_CL_PIR_UNOCCUPIED_TO_OCCUPIED_THRESHOLD_SER_ATTR_MAX_VAL            (0xfe)

#define ZCL_OCCUPANCY_SENSING_CL_ULTRASONIC_OCCUPIED_TO_UNOCCUPIED_DELAY_SER_ATTR_MIN_VAL         (0x0000)
#define ZCL_OCCUPANCY_SENSING_CL_ULTRASONIC_OCCUPIED_TO_UNOCCUPIED_DELAY_SER_ATTR_MAX_VAL         (0xfffe)
#define ZCL_OCCUPANCY_SENSING_CL_ULTRASONIC_UNOCCUPIED_TO_OCCUPIED_DELAY_SER_ATTR_MIN_VAL         (0x0000)
#define ZCL_OCCUPANCY_SENSING_CL_ULTRASONIC_UNOCCUPIED_TO_OCCUPIED_DELAY_SER_ATTR_MAX_VAL         (0xfffe)
#define ZCL_OCCUPANCY_SENSING_CL_ULTRASONIC_UNOCCUPIED_TO_OCCUPIED_THRESHOLD_SER_ATTR_MIN_VAL     (0x01)
#define ZCL_OCCUPANCY_SENSING_CL_ULTRASONIC_UNOCCUPIED_TO_OCCUPIED_THRESHOLD_SER_ATTR_MAX_VAL     (0xfe)

/**
 * \brief Occupancy Sensing Cluster server attributes amount
*/

#define ZCL_OCCUPANCY_SENSING_CLUSTER_SERVER_ATTRIBUTES_AMOUNT (8)//mandatory - 2 + optional - 6

/**
 * \brief Occupancy Sensing Cluster client attributes amount. Clinet doesn't have attributes.
*/

#define ZCL_OCCUPANCY_SENSING_CLUSTER_CLIENT_ATTRIBTUES_AMOUNT 0

/**
 * \brief On/Off Switch Configuration Cluster commands amount
*/

#define ZCL_OCCUPANCY_SENSING_CLUSTER_COMMANDS_AMOUNT 0

/**
 * \brief Occupancy Sensing Cluster server's attributes identifiers
*/
#define ZCL_OCCUPANCY_SENSING_CLUSTER_OCCUPANCY_SERVER_ATTRIBUTE_ID                                     CCPU_TO_LE16(0x0000)
#define ZCL_OCCUPANCY_SENSING_CLUSTER_OCCUPANCY_SENSOR_TYPE_SERVER_ATTRIBUTE_ID                         CCPU_TO_LE16(0x0001)
#define ZCL_OCCUPANCY_SENSING_CLUSTER_PIR_OCCUPIED_TO_UNOCCUPIED_DELAY_SERVER_ATTRIBUTE_ID              CCPU_TO_LE16(0x0010)
#define ZCL_OCCUPANCY_SENSING_CLUSTER_PIR_UNOCCUPIED_TO_OCCUPIED_DELAY_SERVER_ATTRIBUTE_ID              CCPU_TO_LE16(0x0011)
#define ZCL_OCCUPANCY_SENSING_CLUSTER_PIR_UNOCCUPIED_TO_OCCUPIED_THRESHOLD_SERVER_ATTRIBUTE_ID          CCPU_TO_LE16(0x0012)
#define ZCL_OCCUPANCY_SENSING_CLUSTER_ULTRASONIC_OCCUPIED_TO_UNOCCUPIED_DELAY_SERVER_ATTRIBUTE_ID       CCPU_TO_LE16(0x0020)
#define ZCL_OCCUPANCY_SENSING_CLUSTER_ULTRASONIC_UNOCCUPIED_TO_OCCUPIED_DELAY_SERVER_ATTRIBUTE_ID       CCPU_TO_LE16(0x0021)
#define ZCL_OCCUPANCY_SENSING_CLUSTER_ULTRASONIC_UNOCCUPIED_TO_OCCUPIED_THRESHOLD_SERVER_ATTRIBUTE_ID   CCPU_TO_LE16(0x0022)

/**
 * \brief Occupancy Sensing Cluster server define attributes macros
*/

#define ZCL_DEFINE_OCCUPANCY_SENSING_CLUSTER_SERVER_ATTRIBUTES(min, max) \
    DEFINE_REPORTABLE_ATTRIBUTE_WITH_BOUNDARY(occupancy, \
                                   ZCL_READONLY_ATTRIBUTE, \
                                   ZCL_OCCUPANCY_SENSING_CLUSTER_OCCUPANCY_SERVER_ATTRIBUTE_ID, \
                                   ZCL_8BIT_BITMAP_DATA_TYPE_ID, min, max, \
                                   ZCL_OCCUPANCY_SENSING_CL_OCCUPANCY_SER_ATTR_MIN_VAL, \
                                   ZCL_OCCUPANCY_SENSING_CL_OCCUPANCY_SER_ATTR_MAX_VAL),\
    DEFINE_ATTRIBUTE_WITH_BOUNDARY(occupancySensorType, \
                                   ZCL_READONLY_ATTRIBUTE, \
                                   ZCL_OCCUPANCY_SENSING_CLUSTER_OCCUPANCY_SENSOR_TYPE_SERVER_ATTRIBUTE_ID, \
                                   ZCL_8BIT_ENUM_DATA_TYPE_ID, \
                                   ZCL_OCCUPANCY_SENSING_CL_OCCUPANCY_SENSOR_TYPE_SER_ATTR_MIN_VAL, \
                                   ZCL_OCCUPANCY_SENSING_CL_OCCUPANCY_SENSOR_TYPE_SER_ATTR_MAX_VAL),\
    DEFINE_ATTRIBUTE_WITH_BOUNDARY(PIROccupiedToUnoccupiedDelay,\
                                   ZCL_READWRITE_ATTRIBUTE,\
                                   ZCL_OCCUPANCY_SENSING_CLUSTER_PIR_OCCUPIED_TO_UNOCCUPIED_DELAY_SERVER_ATTRIBUTE_ID,\
                                   ZCL_U16BIT_DATA_TYPE_ID,\
                                   ZCL_OCCUPANCY_SENSING_CL_PIR_OCCUPIED_TO_UNOCCUPIED_DELAY_SER_ATTR_MIN_VAL,\
                                   ZCL_OCCUPANCY_SENSING_CL_PIR_OCCUPIED_TO_UNOCCUPIED_DELAY_SER_ATTR_MAX_VAL),\
    DEFINE_ATTRIBUTE_WITH_BOUNDARY(PIRUnoccupiedToOccupiedDelay, \
                                   ZCL_READWRITE_ATTRIBUTE, \
                                   ZCL_OCCUPANCY_SENSING_CLUSTER_PIR_UNOCCUPIED_TO_OCCUPIED_DELAY_SERVER_ATTRIBUTE_ID, \
                                   ZCL_U16BIT_DATA_TYPE_ID, \
                                   ZCL_OCCUPANCY_SENSING_CL_PIR_UNOCCUPIED_TO_OCCUPIED_DELAY_SER_ATTR_MIN_VAL,\
                                   ZCL_OCCUPANCY_SENSING_CL_PIR_UNOCCUPIED_TO_OCCUPIED_DELAY_SER_ATTR_MAX_VAL),\
    DEFINE_ATTRIBUTE_WITH_BOUNDARY(PIRUnoccupiedToOccupiedThreshold, \
                                   ZCL_READWRITE_ATTRIBUTE, \
                                   ZCL_OCCUPANCY_SENSING_CLUSTER_PIR_UNOCCUPIED_TO_OCCUPIED_THRESHOLD_SERVER_ATTRIBUTE_ID, \
                                   ZCL_U8BIT_DATA_TYPE_ID, \
                                   ZCL_OCCUPANCY_SENSING_CL_PIR_UNOCCUPIED_TO_OCCUPIED_THRESHOLD_SER_ATTR_MIN_VAL,\
                                   ZCL_OCCUPANCY_SENSING_CL_PIR_UNOCCUPIED_TO_OCCUPIED_THRESHOLD_SER_ATTR_MAX_VAL),\
    DEFINE_ATTRIBUTE_WITH_BOUNDARY(UltrasonicOccupiedToUnoccupiedDelay,\
                                   ZCL_READWRITE_ATTRIBUTE,\
                                   ZCL_OCCUPANCY_SENSING_CLUSTER_ULTRASONIC_OCCUPIED_TO_UNOCCUPIED_DELAY_SERVER_ATTRIBUTE_ID,\
                                   ZCL_U16BIT_DATA_TYPE_ID,\
                                   ZCL_OCCUPANCY_SENSING_CL_ULTRASONIC_OCCUPIED_TO_UNOCCUPIED_DELAY_SER_ATTR_MIN_VAL,\
                                   ZCL_OCCUPANCY_SENSING_CL_ULTRASONIC_OCCUPIED_TO_UNOCCUPIED_DELAY_SER_ATTR_MAX_VAL),\
    DEFINE_ATTRIBUTE_WITH_BOUNDARY(UltrasonicUnoccupiedToOccupiedDelay, \
                                   ZCL_READWRITE_ATTRIBUTE, \
                                   ZCL_OCCUPANCY_SENSING_CLUSTER_ULTRASONIC_UNOCCUPIED_TO_OCCUPIED_DELAY_SERVER_ATTRIBUTE_ID, \
                                   ZCL_U16BIT_DATA_TYPE_ID, \
                                   ZCL_OCCUPANCY_SENSING_CL_ULTRASONIC_UNOCCUPIED_TO_OCCUPIED_DELAY_SER_ATTR_MIN_VAL,\
                                   ZCL_OCCUPANCY_SENSING_CL_ULTRASONIC_UNOCCUPIED_TO_OCCUPIED_DELAY_SER_ATTR_MAX_VAL),\
    DEFINE_ATTRIBUTE_WITH_BOUNDARY(UltrasonicUnoccupiedToOccupiedThreshold, \
                                   ZCL_READWRITE_ATTRIBUTE, \
                                   ZCL_OCCUPANCY_SENSING_CLUSTER_ULTRASONIC_UNOCCUPIED_TO_OCCUPIED_THRESHOLD_SERVER_ATTRIBUTE_ID, \
                                   ZCL_U8BIT_DATA_TYPE_ID, \
                                   ZCL_OCCUPANCY_SENSING_CL_ULTRASONIC_UNOCCUPIED_TO_OCCUPIED_THRESHOLD_SER_ATTR_MIN_VAL,\
                                   ZCL_OCCUPANCY_SENSING_CL_ULTRASONIC_UNOCCUPIED_TO_OCCUPIED_THRESHOLD_SER_ATTR_MAX_VAL)
                                   

/**
 * \brief Occupancy Sensing Cluster definition macros
*/

#define OCCUPANCY_SENSING_CLUSTER_ZCL_CLIENT_CLUSTER_TYPE(clattributes, clcommands) \
{ \
   .id = OCCUPANCY_SENSING_CLUSTER_ID, \
   .options = { \
                .type = ZCL_CLIENT_CLUSTER_TYPE, \
                .security = ZCL_NETWORK_KEY_CLUSTER_SECURITY, \
              }, \
   .attributesAmount = ZCL_OCCUPANCY_SENSING_CLUSTER_CLIENT_ATTRIBTUES_AMOUNT, \
   .attributes = (uint8_t *)clattributes, \
   .commandsAmount = ZCL_OCCUPANCY_SENSING_CLUSTER_COMMANDS_AMOUNT, \
   .commands = (uint8_t *)clcommands \
}

#define OCCUPANCY_SENSING_CLUSTER_ZCL_SERVER_CLUSTER_TYPE(clattributes, clcommands) \
{ \
   .id = OCCUPANCY_SENSING_CLUSTER_ID, \
   .options = { \
                .type = ZCL_SERVER_CLUSTER_TYPE, \
                .security = ZCL_NETWORK_KEY_CLUSTER_SECURITY, \
              }, \
   .attributesAmount = ZCL_OCCUPANCY_SENSING_CLUSTER_SERVER_ATTRIBUTES_AMOUNT, \
   .attributes = (uint8_t *)clattributes, \
   .commandsAmount = ZCL_OCCUPANCY_SENSING_CLUSTER_COMMANDS_AMOUNT, \
   .commands = (uint8_t *)clcommands \
}

#define DEFINE_OCCUPANCY_SENSING_CLUSTER(cltype, clattributes, clcommands) \
    OCCUPANCY_SENSING_CLUSTER_##cltype(clattributes, clcommands)

#define OCCUPANCY_SENSING_CLUSTER_ZCL_CLIENT_CLUSTER_TYPE_FLASH(clattributes, clcommands) \
{ \
   .id = OCCUPANCY_SENSING_CLUSTER_ID, \
   .options = { \
                .type = ZCL_CLIENT_CLUSTER_TYPE, \
                .security = ZCL_NETWORK_KEY_CLUSTER_SECURITY, \
              }, \
   .attributesAmount = ZCL_OCCUPANCY_SENSING_CLUSTER_CLIENT_ATTRIBTUES_AMOUNT, \
   .attributes = (uint8_t *)clattributes, \
   .commandsAmount = ZCL_OCCUPANCY_SENSING_CLUSTER_COMMANDS_AMOUNT, \
   .commands = (FLASH_PTR uint8_t *)clcommands \
}

#define OCCUPANCY_SENSING_CLUSTER_ZCL_SERVER_CLUSTER_TYPE_FLASH(clattributes, clcommands) \
{ \
   .id = OCCUPANCY_SENSING_CLUSTER_ID, \
   .options = { \
                .type = ZCL_SERVER_CLUSTER_TYPE, \
                .security = ZCL_NETWORK_KEY_CLUSTER_SECURITY, \
              }, \
   .attributesAmount = ZCL_OCCUPANCY_SENSING_CLUSTER_SERVER_ATTRIBUTES_AMOUNT, \
   .attributes = (uint8_t *)clattributes, \
   .commandsAmount = ZCL_OCCUPANCY_SENSING_CLUSTER_COMMANDS_AMOUNT, \
   .commands = (FLASH_PTR uint8_t *)clcommands \
}

#define DEFINE_OCCUPANCY_SENSING_CLUSTER_FLASH(cltype, clattributes, clcommands) \
    OCCUPANCY_SENSING_CLUSTER_##cltype##_FLASH(clattributes, clcommands)
/******************************************************************************
                    Types section
******************************************************************************/

BEGIN_PACK

/**
 * \brief Occupancy Sensing Cluster server's attributes
*/

typedef struct PACK
{
  struct PACK
  {
    ZCL_AttributeId_t id;
    uint8_t type;
    uint8_t properties;
    uint8_t value;
    ZCL_ReportTime_t  reportCounter;     //!<For internal use only
    ZCL_ReportTime_t  minReportInterval; //!<Minimum reporting interval field value
    ZCL_ReportTime_t  maxReportInterval; //!<Maximum reporting interval field value
    uint8_t           reportableChange;  //!<Reporting change field value
    ZCL_ReportTime_t  timeoutPeriod;     //!<Timeout period field value
    uint8_t           lastReportedValue; //!<Last reported value
    uint8_t minVal;
    uint8_t maxVal;
  } occupancy;

  struct PACK
  {
    ZCL_AttributeId_t id;
    uint8_t type;
    uint8_t properties;
    uint8_t value;
    uint8_t minVal;
    uint8_t maxVal;
  } occupancySensorType;

  struct PACK
  {
    ZCL_AttributeId_t id;
    uint8_t type;
    uint8_t properties;
    uint16_t value;
    uint16_t minVal;
    uint16_t maxVal;
  } PIROccupiedToUnoccupiedDelay;

  struct PACK
  {
    ZCL_AttributeId_t id;
    uint8_t type;
    uint8_t properties;
    uint16_t value;
    uint16_t minVal;
    uint16_t maxVal;
  } PIRUnoccupiedToOccupiedDelay;

  struct PACK
  {
    ZCL_AttributeId_t id;
    uint8_t type;
    uint8_t properties;
    uint8_t value;
    uint8_t minVal;
    uint8_t maxVal;
  } PIRUnoccupiedToOccupiedThreshold;

  struct PACK
  {
    ZCL_AttributeId_t id;
    uint8_t type;
    uint8_t properties;
    uint16_t value;
    uint16_t minVal;
    uint16_t maxVal;
  } UltrasonicOccupiedToUnoccupiedDelay;

  struct PACK
  {
    ZCL_AttributeId_t id;
    uint8_t type;
    uint8_t properties;
    uint16_t value;
    uint16_t minVal;
    uint16_t maxVal;
  } UltrasonicUnoccupiedToOccupiedDelay;

  struct PACK
  {
    ZCL_AttributeId_t id;
    uint8_t type;
    uint8_t properties;
    uint8_t value;
    uint8_t minVal;
    uint8_t maxVal;
  } UltrasonicUnoccupiedToOccupiedThreshold;

} ZCL_OccupancySensingClusterServerAttributes_t;

END_PACK


#endif /* _ZCLOCCUPANCYSENSINGCLUSTER_H */

