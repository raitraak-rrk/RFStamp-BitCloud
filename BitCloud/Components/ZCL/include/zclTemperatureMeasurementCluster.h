/*****************************************************************************
  \file  zclTemperatureMeasurement.h

  \brief
    The file describes the types and interface of the Temperature Measurement cluster

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    18.01.2010 I.Vagulin - Created
******************************************************************************/


#ifndef _ZCLTEMPERATUREMEASUREMENT_H_
#define _ZCLTEMPERATUREMEASUREMENT_H_

/*******************************************************************************
                   Includes section
*******************************************************************************/
#include <sysTypes.h>
#include <clusters.h>

/*******************************************************************************
                   Define(s) section
*******************************************************************************/
/**
* \brief server attribute values
*/
#define TEMPERATURE_MEASUREMENT_MEASURED_VALUE_UNKNOWN               0x8000

/**
* \brief server attributes amount
*/
#define ZCL_TEMPERATURE_MEASUREMENT_CLUSTER_SERVER_ATTRIBUTES_AMOUNT 4

/**
* \brief server commands amount
*/
#define ZCL_TEMPERATURE_MEASUREMENT_CLUSTER_SERVER_COMMANDS_AMOUNT 0

/**
* \brief client attributes amount
*/
#define ZCL_TEMPERATURE_MEASUREMENT_CLUSTER_CLIENT_ATTRIBUTES_AMOUNT 0

/**
* \brief client commands amount
*/
#define ZCL_TEMPERATURE_MEASUREMENT_CLUSTER_CLIENT_COMMANDS_AMOUNT 0

/**
* \brief Supported attribue id
*/
#define ZCL_TEMPERATURE_MEASUREMENT_CLUSTER_SERVER_MEASURED_VALUE_ATTRIBUTE_ID     CCPU_TO_LE16(0x0000)
#define ZCL_TEMPERATURE_MEASUREMENT_CLUSTER_SERVER_MIN_MEASURED_VALUE_ATTRIBUTE_ID CCPU_TO_LE16(0x0001)
#define ZCL_TEMPERATURE_MEASUREMENT_CLUSTER_SERVER_MAX_MEASURED_VALUE_ATTRIBUTE_ID CCPU_TO_LE16(0x0002)
#define ZCL_TEMPERATURE_MEASUREMENT_CLUSTER_SERVER_TOLERANCE_ATTRIBUTE_ID          CCPU_TO_LE16(0x0003) //optinal attr

/**
 * \brief server define attributes macros
*/
#define ZCL_DEFINE_TEMPERATURE_MEASUREMENT_CLUSTER_SERVER_ATTRIBUTES(reportMin, reportMax) \
  DEFINE_REPORTABLE_ATTRIBUTE_WITH_BOUNDARY(measuredValue, ZCL_READONLY_ATTRIBUTE | ZCL_CHECK_OTHER_ATTR, ZCL_TEMPERATURE_MEASUREMENT_CLUSTER_SERVER_MEASURED_VALUE_ATTRIBUTE_ID, ZCL_S16BIT_DATA_TYPE_ID, reportMin, reportMax, \
                                            ZCL_TEMPERATURE_MEASUREMENT_CLUSTER_SERVER_MIN_MEASURED_VALUE_ATTRIBUTE_ID, ZCL_TEMPERATURE_MEASUREMENT_CLUSTER_SERVER_MAX_MEASURED_VALUE_ATTRIBUTE_ID), \
  DEFINE_ATTRIBUTE_WITH_BOUNDARY(minMeasuredValue, ZCL_READONLY_ATTRIBUTE, ZCL_TEMPERATURE_MEASUREMENT_CLUSTER_SERVER_MIN_MEASURED_VALUE_ATTRIBUTE_ID,  ZCL_S16BIT_DATA_TYPE_ID, 0x954D, 0x7FFE), \
  DEFINE_ATTRIBUTE_WITH_BOUNDARY(maxMeasuredValue, ZCL_READONLY_ATTRIBUTE, ZCL_TEMPERATURE_MEASUREMENT_CLUSTER_SERVER_MAX_MEASURED_VALUE_ATTRIBUTE_ID, ZCL_S16BIT_DATA_TYPE_ID, 0x954E, 0x7FFF), \
  DEFINE_REPORTABLE_ATTRIBUTE_WITH_BOUNDARY(tolerance, ZCL_READONLY_ATTRIBUTE, ZCL_TEMPERATURE_MEASUREMENT_CLUSTER_SERVER_TOLERANCE_ATTRIBUTE_ID, ZCL_U16BIT_DATA_TYPE_ID, reportMin, reportMax, 0x0000, 0x0800)

#define TEMPERATURE_MEASUREMENT_CLUSTER_ZCL_SERVER_CLUSTER_TYPE(clattributes) \
  { \
    .id = TEMPERATURE_MEASUREMENT_CLUSTER_ID, \
    .options = {.type = ZCL_SERVER_CLUSTER_TYPE, .security = ZCL_NETWORK_KEY_CLUSTER_SECURITY}, \
    .attributesAmount = ZCL_TEMPERATURE_MEASUREMENT_CLUSTER_SERVER_ATTRIBUTES_AMOUNT, \
    .attributes = (uint8_t *) (clattributes), \
    .commandsAmount = ZCL_TEMPERATURE_MEASUREMENT_CLUSTER_SERVER_COMMANDS_AMOUNT, \
    .commands = NULL \
  }

#define TEMPERATURE_MEASUREMENT_CLUSTER_ZCL_CLIENT_CLUSTER_TYPE(clattributes) \
  { \
    .id = TEMPERATURE_MEASUREMENT_CLUSTER_ID, \
    .options = {.type = ZCL_CLIENT_CLUSTER_TYPE, .security = ZCL_NETWORK_KEY_CLUSTER_SECURITY}, \
    .attributesAmount = ZCL_TEMPERATURE_MEASUREMENT_CLUSTER_CLIENT_ATTRIBUTES_AMOUNT, \
    .attributes = (uint8_t *) (clattributes), \
    .commandsAmount = ZCL_TEMPERATURE_MEASUREMENT_CLUSTER_CLIENT_COMMANDS_AMOUNT, \
    .commands = NULL \
  }


#define DEFINE_TEMPERATURE_MEASUREMENT_CLUSTER(cltype, clattributes) \
    TEMPERATURE_MEASUREMENT_CLUSTER_##cltype(clattributes)


#define ZCL_DEFINE_TEMPERATURE_MEASUREMENT_CLUSTER_SERVER_FLASH(clattributes) ZCL_DEFINE_TEMPERATURE_MEASUREMENT_CLUSTER_SERVER(clattributes)
#define ZCL_DEFINE_TEMPERATURE_MEASUREMENT_CLUSTER_CLIENT_FLASH() ZCL_DEFINE_TEMPERATURE_MEASUREMENT_CLUSTER_CLIENT()
/******************************************************************************
                    Types section
******************************************************************************/
BEGIN_PACK
typedef struct PACK {
  struct PACK {
    ZCL_AttributeId_t id;
    uint8_t type;
    uint8_t properties;
    int16_t value;
    ZCL_ReportTime_t reportCounter;     //!<For internal use only
    ZCL_ReportTime_t minReportInterval; //!<Minimum reporting interval field value
    ZCL_ReportTime_t maxReportInterval; //!<Maximum reporting interval field value
    int16_t reportableChange;           //!<Reporting change field value
    ZCL_ReportTime_t timeoutPeriod;     //!<Timeout period field value
    int16_t lastReportedValue;          //!<Last reported value
    ZCL_AttributeId_t minVal;
    ZCL_AttributeId_t maxVal;
  } measuredValue ;

  struct PACK {
    ZCL_AttributeId_t id;
    uint8_t type;
    uint8_t properties;
    int16_t value;
    int16_t minVal;
    int16_t maxVal;
  } minMeasuredValue ;

  struct PACK {
    ZCL_AttributeId_t   id;
    uint8_t type;
    uint8_t properties;
    int16_t value;
    int16_t minVal;
    int16_t maxVal;
  } maxMeasuredValue ;

  struct PACK {
    ZCL_AttributeId_t id;
    uint8_t type;
    uint8_t properties;
    uint16_t value;
    ZCL_ReportTime_t reportCounter;     //!<For internal use only
    ZCL_ReportTime_t minReportInterval; //!<Minimum reporting interval field value
    ZCL_ReportTime_t maxReportInterval; //!<Maximum reporting interval field value
    int16_t reportableChange;           //!<Reporting change field value
    ZCL_ReportTime_t timeoutPeriod;     //!<Timeout period field value
    int16_t lastReportedValue;          //!<Last reported value
    uint16_t minVal;
    uint16_t maxVal;
  } tolerance ;

} ZCL_TemperatureMeasurementClusterAttributes_t ;
END_PACK


#endif /* _ZCLTEMPERATUREMEASUREMENT_H_ */
