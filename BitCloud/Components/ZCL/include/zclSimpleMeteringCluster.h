/************************************************************************//**
  \file zclSimpleMeteringCluster.h

  \brief
    The header file describes the ZCL Simple Metering Cluster

    The header file describes the ZCL Simple Metering Cluster

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    27.11.08 A. Potashov - Created.
******************************************************************************/

#ifndef _SIMPLEMETERINGCLUSTER_H
#define _SIMPLEMETERINGCLUSTER_H

#include <zcl.h>
#include <clusters.h>

#ifdef __UK_MARKET_EXT__
  #include <zclSimpleMeteringClusterMacroSet.h>
#endif

/*************************************************************************//**
  \brief Simple Metering Cluster attributes ids
*****************************************************************************/
#define CURRENT_SUMMATION_DELIVERED_ATTRIBUTE_ID     CCPU_TO_LE16(0x0000)
#define METER_STATUS_ATTRIBUTE_ID                    CCPU_TO_LE16(0x0200)
#define UNIT_OF_MEASURE_ATTRIBUTE_ID                 CCPU_TO_LE16(0x0300)
#define SUMMATION_FORMATING_ATTRIBUTE_ID             CCPU_TO_LE16(0x0303)
#define METERING_DEVICE_TYPE_ATTRIBUTE_ID            CCPU_TO_LE16(0x0306)

#ifdef __UK_MARKET_EXT__
  #define METER_EXTENDED_STATUS_ATTRIBUTE_ID         CCPU_TO_LE16(0x0204)
  #define SITE_ID_ATTRIBUTE_ID                       CCPU_TO_LE16(0x0308)
  #define OPERATING_TARIFF_LABEL_ATTRIBUTE_ID        CCPU_TO_LE16(0x030F)
  #define DEMAND_LIMIT_ARM_DURATION_ATTRIBUTE_ID     CCPU_TO_LE16(0x0604)
  #define GENERIC_ALARM_MASK_ATTRIBUTE_ID            CCPU_TO_LE16(0x0800)
  #define GAS_SPECIFIC_ALARM_MASK_ATTRIBUTE_ID       CCPU_TO_LE16(0x0805)
  #define MANUFACTURE_ALARM_MASK_ATTRIBUTE_ID        CCPU_TO_LE16(0x0806)
  #define BILL_TO_DATE_ATTRIBUTE_ID                  CCPU_TO_LE16(0x0A00)
  #define BILL_TO_DATE_TIME_STAMP_ATTRIBUTE_ID       CCPU_TO_LE16(0x0A01)
  #define PROJECTED_BILL_ATTRIBUTE_ID                CCPU_TO_LE16(0x0A02)
  #define PROJECTED_BILL_TIME_STAMP_ATTRIBUTE_ID     CCPU_TO_LE16(0x0A03)

  #define NOTIFICATION_CONTROL_FLAGS_ATTRIBUTE_ID    CCPU_TO_LE16(0x0000)
  #define NOTOFICATION_FLAGS_ATTRIBUTE_ID            CCPU_TO_LE16(0x0001)
  #define PRICE_NOTIFICATION_FLAGS_ATTRIBUTE_ID      CCPU_TO_LE16(0x0002)
  #define CALENDAR_NOTIFICATION_FLAGS_ATTRIBUTE_ID   CCPU_TO_LE16(0x0003)
  #define PREPAY_NOTIFICATION_FLAGS_ATTRIBUTE_ID     CCPU_TO_LE16(0x0004)
  #define DEVICE_MANAGEMENT_FLAGS_ATTRIBUTE_ID       CCPU_TO_LE16(0x0005)

#endif //__UK_MARKET_EXT__
/*************************************************************************//**
  \brief Simple Metering Cluster UnitofMeasure attribute values ids
*****************************************************************************/
#define KILOWATTS_UNIT_OF_MEASURE_ID   0x00
#define CUBIC_METER_UNIT_OF_MEASURE_ID 0x01

/*************************************************************************//**
  \brief Simple Metering Cluster MeteringDeviceType attribute values ids
*****************************************************************************/
#define ELECTRIC_METERING_METERING_DEVICE_TYPE_ID                       0x00
#define GAS_METERING_METERING_DEVICE_TYPE_ID                            0x01

#ifdef __UK_MARKET_EXT__
  #define WATER_METERING_METERING_DEVICE_TYPE_ID                        0x02
  #define THERMAL_METERING_METERING_DEVICE_TYPE_ID                      0x03
  #define PRESSURE_METERING_DEVICE_TYPE_ID                              0x04
  #define HEAT_METERING_DEVICE_TYPE_ID                                  0x05
  #define COOLING_METERING_DEVICE_TYPE_ID                               0x06
  #define ELECTRIC_VEHICLE_CHARGING_METERING_DEVICE_TYPE_ID             0x07
  #define PV_GENERATION_METERING_DEVICE_TYPE_ID                         0x08
  #define WIND_TURBINE_GENERATION_METERING_DEVICE_TYPE_ID               0x09
  #define WATER_TURBINE_GENERATION_METERING_DEVICE_TYPE_ID              0x0A
  #define MICRO_GENERATION_METERING_DEVICE_TYPE_ID                      0x0B
  #define SOLAR_HOT_WATER_GENERATION_METERING_DEVICE_TYPE_ID            0x0C

  #define MIRRORING_GAS_METERING_DEVICE_TYPE_ID                         0x80
  #define MIRRORING_WATER_METERING_DEVICE_TYPE_ID                       0x81
  #define MIRRORING_THERMAL_METERING_METERING_DEVICE_TYPE_ID            0x82
  #define MIRRORING_PRESSURE_METERING_DEVICE_TYPE_ID                    0x83
  #define MIRRORING_HEAT_METERING_DEVICE_TYPE_ID                        0x84
  #define MIRRORING_COOLING_METERING_DEVICE_TYPE_ID                     0x85
  #define MIRRORING_ELECTRIC_VEHICLE_CHARGING_METERING_DEVICE_TYPE_ID   0x86
  #define MIRRORING_PV_GENERATION_METERING_DEVICE_TYPE_ID               0x87
  #define MIRRORING_WIND_TURBINE_GENERATION_METERING_DEVICE_TYPE_ID     0x88
  #define MIRRORING_WATER_TURBINE_GENERATION_METERING_DEVICE_TYPE_ID    0x89
  #define MIRRORING_MICRO_GENERATION_METERING_DEVICE_TYPE_ID            0x8A
  #define MIRRORING_SOLAR_HOT_WATER_GENERATION_METERING_DEVICE_TYPE_ID  0x8B
#endif

/*************************************************************************//**
  \brief Simple Metering Cluster attributes amount
*****************************************************************************/
#ifdef __UK_MARKET_EXT__
  #define SIMPLE_METERING_CLUSTER_CLIENT_ATTRIBUTES_AMOUNT     6
  #define SIMPLE_METERING_CLUSTER_SERVER_ATTRIBUTES_AMOUNT     272  //notice: uint8_t attributesAmount;
#else
  #define SIMPLE_METERING_CLUSTER_CLIENT_ATTRIBUTES_AMOUNT     0
  #define SIMPLE_METERING_CLUSTER_SERVER_ATTRIBUTES_AMOUNT     5
#endif
/*************************************************************************//**
  \brief Simple Metering Cluster commands amount
*****************************************************************************/
#ifdef __UK_MARKET_EXT__
  #define SIMPLE_METERING_CLUSTER_CLIENT_COMMANDS_AMOUNT       6
  #define SIMPLE_METERING_CLUSTER_SERVER_COMMANDS_AMOUNT       4
#else
  #define SIMPLE_METERING_CLUSTER_CLIENT_COMMANDS_AMOUNT       3
  #define SIMPLE_METERING_CLUSTER_SERVER_COMMANDS_AMOUNT       3
#endif
/*************************************************************************//**
  \brief Simple Metering Server Cluster commands
*****************************************************************************/
#define GET_PROFILE_RESPONSE_COMMAND_ID     0x00
#define REQUEST_MIRROR_COMMAND_ID           0x01
#define REMOVE_MIRROR_COMMAND_ID            0x02

#ifdef __UK_MARKET_EXT__
  #define GET_SNAPSHAT_RESPONSE_COMMAND_ID  0x04
#endif
/*************************************************************************//**
  \brief Simple Metering Client Cluster commands
*****************************************************************************/
#define GET_PROFILE_COMMAND_ID                        0x00
#define REQUEST_MIRROR_RESPONSE_COMMAND_ID            0x01
#define MIRROR_REMOVED_COMMAND_ID                     0x02

#ifdef __UK_MARKET_EXT__
  #define GET_SNAPSHOT_COMMAND_ID                     0x04
  #define TAKE_SNAPSHOT_COMMAND_ID                    0x05
  #define MIRROR_REPORT_ATTRIBUTE_RESPONSE_COMMAND_ID 0x06
#endif

#ifdef __UK_MARKET_EXT__

  #define DEFINE_SIMPLE_METERING_CLIENT_CLUSTER_COMMANDS(clGetProfileResponseInd, clRequestMirrorInd, clRemoveMirrorInd, clGetSnapshotInd, clTakeSnapshotInd, clMirrorReportAttributeResponseInd) \
    DEFINE_COMMAND(getProfileResponseCommand,             0x00, COMMAND_OPTIONS(SERVER_TO_CLIENT, ZCL_THERE_IS_NO_RELEVANT_RESPONSE,  ZCL_COMMAND_ACK), clGetProfileResponseInd), \
    DEFINE_COMMAND(requestMirrorCommand,                  0x01, COMMAND_OPTIONS(SERVER_TO_CLIENT, ZCL_THERE_IS_RELEVANT_RESPONSE,     ZCL_COMMAND_ACK), clRequestMirrorInd), \
    DEFINE_COMMAND(removeMirrorCommand,                   0x02, COMMAND_OPTIONS(SERVER_TO_CLIENT, ZCL_THERE_IS_RELEVANT_RESPONSE,     ZCL_COMMAND_ACK), clRemoveMirrorInd), \
    DEFINE_COMMAND(getSnapshotCommand,                    0x04, COMMAND_OPTIONS(SERVER_TO_CLIENT, ZCL_THERE_IS_RELEVANT_RESPONSE,     ZCL_COMMAND_ACK), clGetSnapshotInd), \
    DEFINE_COMMAND(takeSnapshotCommand,                   0x05, COMMAND_OPTIONS(SERVER_TO_CLIENT, ZCL_THERE_IS_NO_RELEVANT_RESPONSE,  ZCL_COMMAND_ACK), clTakeSnapshotInd), \
    DEFINE_COMMAND(mirrorReportAttributeResponseCommand,  0x06, COMMAND_OPTIONS(SERVER_TO_CLIENT, ZCL_THERE_IS_NO_RELEVANT_RESPONSE,  ZCL_COMMAND_ACK), clMirrorReportAttributeResponseInd)

  #define DEFINE_SIMPLE_METERING_SERVER_CLUSTER_COMMANDS(clGetProfileInd, clRequestMirrorResponseInd, clMirrorRemovedInd, clGetSnapshotResponseInd) \
    DEFINE_COMMAND(getProfileCommand,                     0x00, COMMAND_OPTIONS(CLIENT_TO_SERVER, ZCL_THERE_IS_RELEVANT_RESPONSE,     ZCL_COMMAND_ACK), clGetProfileInd), \
    DEFINE_COMMAND(requestMirrorResponseCommand,          0x01, COMMAND_OPTIONS(CLIENT_TO_SERVER, ZCL_THERE_IS_NO_RELEVANT_RESPONSE,  ZCL_COMMAND_ACK), clRequestMirrorResponseInd), \
    DEFINE_COMMAND(mirrorRemovedCommand,                  0x02, COMMAND_OPTIONS(CLIENT_TO_SERVER, ZCL_THERE_IS_NO_RELEVANT_RESPONSE,  ZCL_COMMAND_ACK), clMirrorRemovedInd), \
    DEFINE_COMMAND(getSnapshotResponseCommand,            0x04, COMMAND_OPTIONS(CLIENT_TO_SERVER, ZCL_THERE_IS_NO_RELEVANT_RESPONSE,  ZCL_COMMAND_ACK), clGetSnapshotResponseInd)

#else

  #define DEFINE_SIMPLE_METERING_CLIENT_CLUSTER_COMMANDS(clGetProfileResponseInd, clRequestMirrorInd, clRemoveMirrorInd) \
    DEFINE_COMMAND(getProfileResponseCommand,    0x00, COMMAND_OPTIONS(SERVER_TO_CLIENT, ZCL_THERE_IS_NO_RELEVANT_RESPONSE, ZCL_COMMAND_ACK), clGetProfileResponseInd), \
    DEFINE_COMMAND(requestMirrorCommand,         0x01, COMMAND_OPTIONS(SERVER_TO_CLIENT, ZCL_THERE_IS_RELEVANT_RESPONSE,    ZCL_COMMAND_ACK), clRequestMirrorInd), \
    DEFINE_COMMAND(removeMirrorCommand,          0x02, COMMAND_OPTIONS(SERVER_TO_CLIENT, ZCL_THERE_IS_RELEVANT_RESPONSE,    ZCL_COMMAND_ACK), clRemoveMirrorInd)

  #define DEFINE_SIMPLE_METERING_SERVER_CLUSTER_COMMANDS(clGetProfileInd, clRequestMirrorResponseInd, clMirrorRemovedInd) \
    DEFINE_COMMAND(getProfileCommand,            0x00, COMMAND_OPTIONS(CLIENT_TO_SERVER, ZCL_THERE_IS_RELEVANT_RESPONSE,    ZCL_COMMAND_ACK), clGetProfileInd), \
    DEFINE_COMMAND(requestMirrorResponseCommand, 0x01, COMMAND_OPTIONS(CLIENT_TO_SERVER, ZCL_THERE_IS_NO_RELEVANT_RESPONSE, ZCL_COMMAND_ACK), clRequestMirrorResponseInd), \
    DEFINE_COMMAND(mirrorRemovedCommand,         0x02, COMMAND_OPTIONS(CLIENT_TO_SERVER, ZCL_THERE_IS_NO_RELEVANT_RESPONSE, ZCL_COMMAND_ACK), clMirrorRemovedInd)

#endif // __UK_MARKET_EXT__

#ifdef __UK_MARKET_EXT__

  #define DEFINE_SIMPLE_METERING_SERVER_ATTRIBUTES(min, max) \
    DEFINE_REPORTABLE_ATTRIBUTE(currentSummationDelivered, ZCL_READONLY_ATTRIBUTE, CURRENT_SUMMATION_DELIVERED_ATTRIBUTE_ID, ZCL_U48BIT_DATA_TYPE_ID, min, max), \
    DEFINE_ATTRIBUTE(meterStatus,             ZCL_READONLY_ATTRIBUTE,   METER_STATUS_ATTRIBUTE_ID,              ZCL_8BIT_BITMAP_DATA_TYPE_ID),  \
    DEFINE_ATTRIBUTE(extendedStatus,          ZCL_READONLY_ATTRIBUTE,   METER_EXTENDED_STATUS_ATTRIBUTE_ID,     ZCL_48BIT_BITMAP_DATA_TYPE_ID), \
    DEFINE_ATTRIBUTE(unitofMeasure,           ZCL_READONLY_ATTRIBUTE,   UNIT_OF_MEASURE_ATTRIBUTE_ID,           ZCL_8BIT_ENUM_DATA_TYPE_ID),    \
    DEFINE_ATTRIBUTE(summationFormatting,     ZCL_READONLY_ATTRIBUTE,   SUMMATION_FORMATING_ATTRIBUTE_ID,       ZCL_8BIT_BITMAP_DATA_TYPE_ID),  \
    DEFINE_ATTRIBUTE(meteringDeviceType,      ZCL_READONLY_ATTRIBUTE,   METERING_DEVICE_TYPE_ATTRIBUTE_ID,      ZCL_8BIT_ENUM_DATA_TYPE_ID),    \
    DEFINE_ATTRIBUTE(siteID,                  ZCL_READWRITE_ATTRIBUTE,  SITE_ID_ATTRIBUTE_ID,                   ZCL_OCTET_STRING_DATA_TYPE_ID), \
    DEFINE_ATTRIBUTE(operatingTariffLabel,    ZCL_READWRITE_ATTRIBUTE,  OPERATING_TARIFF_LABEL_ATTRIBUTE_ID,    ZCL_OCTET_STRING_DATA_TYPE_ID), \
    DEFINE_ATTRIBUTE(demandLimitArmDuration,  ZCL_READONLY_ATTRIBUTE,   DEMAND_LIMIT_ARM_DURATION_ATTRIBUTE_ID, ZCL_U16BIT_DATA_TYPE_ID),       \
    DEFINE_ATTRIBUTE(genericAlarmMask,        ZCL_READWRITE_ATTRIBUTE,  GENERIC_ALARM_MASK_ATTRIBUTE_ID,        ZCL_64BIT_BITMAP_DATA_TYPE_ID), \
    DEFINE_ATTRIBUTE(gasSpecificAlarmMask,    ZCL_READWRITE_ATTRIBUTE,  GAS_SPECIFIC_ALARM_MASK_ATTRIBUTE_ID,   ZCL_16BIT_BITMAP_DATA_TYPE_ID), \
    DEFINE_ATTRIBUTE(manufactureAlarmMask,    ZCL_READWRITE_ATTRIBUTE,  MANUFACTURE_ALARM_MASK_ATTRIBUTE_ID,    ZCL_16BIT_BITMAP_DATA_TYPE_ID), \
    \
    DEFINE_BLOCK_RECEIVED_INFORMATION_ATTRIBUTS_SET(), \
    \
    DEFINE_ATTRIBUTE(billToDate,              ZCL_READONLY_ATTRIBUTE,   BILL_TO_DATE_ATTRIBUTE_ID,              ZCL_U32BIT_DATA_TYPE_ID),       \
    DEFINE_ATTRIBUTE(billToDateTimeStamp,     ZCL_READONLY_ATTRIBUTE,   BILL_TO_DATE_TIME_STAMP_ATTRIBUTE_ID,   ZCL_UTC_TIME_DATA_TYPE_ID),     \
    DEFINE_ATTRIBUTE(projectedBill,           ZCL_READONLY_ATTRIBUTE,   PROJECTED_BILL_ATTRIBUTE_ID,            ZCL_U32BIT_DATA_TYPE_ID),       \
    DEFINE_ATTRIBUTE(projectedBillTimeStamp,  ZCL_READONLY_ATTRIBUTE,   PROJECTED_BILL_TIME_STAMP_ATTRIBUTE_ID, ZCL_UTC_TIME_DATA_TYPE_ID)

  #define DEFINE_SIMPLE_METERING_CLIENT_ATTRIBUTES() \
    DEFINE_ATTRIBUTE(notificationControlFlags, ZCL_READWRITE_ATTRIBUTE, ,NOTIFICATION_CONTROL_FLAGS_ATTRIBUTE_ID,  ZCL_8BIT_BITMAP_DATA_TYPE_ID),  \
    DEFINE_ATTRIBUTE(notificationFlags,        ZCL_READONLY_ATTRIBUTE,  ,NOTOFICATION_FLAGS_ATTRIBUTE_ID,          ZCL_8BIT_BITMAP_DATA_TYPE_ID),  \
    DEFINE_ATTRIBUTE(priceNotificationFlags,   ZCL_READONLY_ATTRIBUTE,  ,PRICE_NOTIFICATION_FLAGS_ATTRIBUTE_ID,    ZCL_16BIT_BITMAP_DATA_TYPE_ID), \
    DEFINE_ATTRIBUTE(calendarNotificationFlags,ZCL_READONLY_ATTRIBUTE,  ,CALENDAR_NOTIFICATION_FLAGS_ATTRIBUTE_ID, ZCL_8BIT_BITMAP_DATA_TYPE_ID),  \
    DEFINE_ATTRIBUTE(prePayNotificationFlags,  ZCL_READONLY_ATTRIBUTE,  ,PREPAY_NOTIFICATION_FLAGS_ATTRIBUTE_ID,   ZCL_16BIT_BITMAP_DATA_TYPE_ID), \
    DEFINE_ATTRIBUTE(deviceManagementFlags,    ZCL_READONLY_ATTRIBUTE,  ,DEVICE_MANAGEMENT_FLAGS_ATTRIBUTE_ID,     ZCL_8BIT_BITMAP_DATA_TYPE_ID)

#else

  #define DEFINE_SIMPLE_METERING_SERVER_ATTRIBUTES(min, max) \
    DEFINE_REPORTABLE_ATTRIBUTE(currentSummationDelivered, ZCL_READONLY_ATTRIBUTE, CURRENT_SUMMATION_DELIVERED_ATTRIBUTE_ID, ZCL_U48BIT_DATA_TYPE_ID, min, max), \
    DEFINE_ATTRIBUTE(meterStatus,             ZCL_READONLY_ATTRIBUTE,   METER_STATUS_ATTRIBUTE_ID,              ZCL_8BIT_BITMAP_DATA_TYPE_ID),  \
    DEFINE_ATTRIBUTE(unitofMeasure,           ZCL_READONLY_ATTRIBUTE,   UNIT_OF_MEASURE_ATTRIBUTE_ID,           ZCL_8BIT_ENUM_DATA_TYPE_ID),    \
    DEFINE_ATTRIBUTE(summationFormatting,     ZCL_READONLY_ATTRIBUTE,   SUMMATION_FORMATING_ATTRIBUTE_ID,       ZCL_8BIT_BITMAP_DATA_TYPE_ID),  \
    DEFINE_ATTRIBUTE(meteringDeviceType,      ZCL_READONLY_ATTRIBUTE,   METERING_DEVICE_TYPE_ATTRIBUTE_ID,      ZCL_8BIT_ENUM_DATA_TYPE_ID)

#endif // __UK_MARKET_EXT__

#define SIMPLE_METERING_CLUSTER_ZCL_CLIENT_CLUSTER_TYPE(clattributes, clcommands) \
  { \
    .id = SIMPLE_METERING_CLUSTER_ID, \
    .options = {.type = ZCL_CLIENT_CLUSTER_TYPE, .security = ZCL_APPLICATION_LINK_KEY_CLUSTER_SECURITY}, \
    .attributesAmount = SIMPLE_METERING_CLUSTER_CLIENT_ATTRIBUTES_AMOUNT, \
    .attributes = (uint8_t *)clattributes, \
    .commandsAmount = SIMPLE_METERING_CLUSTER_CLIENT_COMMANDS_AMOUNT, \
    .commands = (uint8_t *)clcommands \
  }

#define SIMPLE_METERING_CLUSTER_ZCL_SERVER_CLUSTER_TYPE(clattributes, clcommands) \
  { \
    .id = SIMPLE_METERING_CLUSTER_ID, \
    .options = {.type = ZCL_SERVER_CLUSTER_TYPE, .security = ZCL_APPLICATION_LINK_KEY_CLUSTER_SECURITY}, \
    .attributesAmount = SIMPLE_METERING_CLUSTER_SERVER_ATTRIBUTES_AMOUNT, \
    .attributes = (uint8_t *)clattributes, \
    .commandsAmount = SIMPLE_METERING_CLUSTER_SERVER_COMMANDS_AMOUNT, \
    .commands = (uint8_t *)clcommands \
  }

#define DEFINE_SIMPLE_METERING_CLUSTER(cltype, clattributes, clcommands) SIMPLE_METERING_CLUSTER_##cltype(clattributes, clcommands)

#define SIMPLE_METERING_CLUSTER_ZCL_CLIENT_CLUSTER_TYPE_FLASH(clattributes, clcommands) \
  { \
    .id = SIMPLE_METERING_CLUSTER_ID, \
    .options = {.type = ZCL_CLIENT_CLUSTER_TYPE, .security = ZCL_APPLICATION_LINK_KEY_CLUSTER_SECURITY}, \
    .attributesAmount = SIMPLE_METERING_CLUSTER_CLIENT_ATTRIBUTES_AMOUNT, \
    .attributes = (uint8_t *)clattributes, \
    .commandsAmount = SIMPLE_METERING_CLUSTER_CLIENT_COMMANDS_AMOUNT, \
    .commands = (FLASH_PTR uint8_t *)clcommands \
  }

#define SIMPLE_METERING_CLUSTER_ZCL_SERVER_CLUSTER_TYPE_FLASH(clattributes, clcommands) \
  { \
    .id = SIMPLE_METERING_CLUSTER_ID, \
    .options = {.type = ZCL_SERVER_CLUSTER_TYPE, .security = ZCL_APPLICATION_LINK_KEY_CLUSTER_SECURITY}, \
    .attributesAmount = SIMPLE_METERING_CLUSTER_SERVER_ATTRIBUTES_AMOUNT, \
    .attributes = (uint8_t *)clattributes, \
    .commandsAmount = SIMPLE_METERING_CLUSTER_SERVER_COMMANDS_AMOUNT, \
    .commands = (FLASH_PTR uint8_t *)clcommands \
  }
#define DEFINE_SIMPLE_METERING_CLUSTER_FLASH(cltype, clattributes, clcommands) SIMPLE_METERING_CLUSTER_##cltype##_FLASH(clattributes, clcommands)

BEGIN_PACK

/*************************************************************************//**
  \brief ZCL Simple Metering Cluster Descriptor

  For internal use
*****************************************************************************/
typedef struct PACK
{
  //!Reading information attribute set (Id = 0x00)
  //!Current Summation Delivered Attribute descriptor
  struct PACK
  {
    ZCL_AttributeId_t id;                         //!<Attribute Id (0x0000)
    uint8_t           type;                       //!<Attribute data type (Unsignet 48 bit Integer)
    uint8_t           properties;                 //!<Attribute properties bitmask
    uint8_t           value[6];                   //!<Attribute value
    ZCL_ReportTime_t  reportCounter;              //!<For internal use only
    ZCL_ReportTime_t  minReportInterval;          //!<Minimum reporting interval field value
    ZCL_ReportTime_t  maxReportInterval;          //!<Maximum reporting interval field value
    uint8_t           reportableChange[6];        //!<Reporting change field value
    ZCL_ReportTime_t  timeoutPeriod;              //!<Timeout period field value
    uint8_t           lastReportedValue[6];       //!<Last reported value
  } currentSummationDelivered;

  //!Meter status attribute set (Id = 0x02)
  //!Meter Status Attribute descriptor
  struct PACK
  {
    ZCL_AttributeId_t id;                         //!<Attribute Id (0x0200)
    uint8_t           type;                       //!<Attribute data type (8 bit Bitmap)
    uint8_t           properties;                 //!<Attribute properties bitmask
    //!Attribute value
    struct PACK
    {
      LITTLE_ENDIAN_OCTET(8,(
        uint8_t         checkMeter            :1, //!<Check Meter
        uint8_t         lowBattery            :1, //!<Low Battery
        uint8_t         tamperDetect          :1, //!<Tamper Detect
        uint8_t         powerFailure          :1, //!<Power Failure
        uint8_t         powerQuality          :1, //!<Power Quality
        uint8_t         leakDetect            :1, //!<Leak Detect
        uint8_t         serviceDisconnectOpen :1, //!<Service Disconnect Open
        uint8_t         reserved              :1  //!<Reserved
      ))
    } value;
  } meterStatus;

#ifdef __UK_MARKET_EXT__
  //!Meter Extended Status Attribute descriptor
  struct PACK
  {
    ZCL_AttributeId_t id;                         //!<Attribute Id (0x0204)
    uint8_t           type;                       //!<Attribute data type (48 bit Bitmap)
    uint8_t           properties;                 //!<Attribute properties bitmask
    struct PACK                                   //!Attribute value (default ---)
    {
      struct PACK
      {
        LITTLE_ENDIAN_OCTET(8,(
          uint8_t        meterCoverRemoved           :1, //!<Meter Cover Removed
          uint8_t        strongMagneticFieldDetected :1, //!<Strong Magnetic Field detected
          uint8_t        batteryFailure              :1, //!<Battery Failure
          uint8_t        programMemoryError          :1, //!<Program Memory Error
          uint8_t        ramError                    :1, //!<RAM Error
          uint8_t        nvMemoryError               :1, //!<NV Memory Error
          uint8_t        measurementSystemError      :1, //!<Measurement System Error
          uint8_t        watchdogError               :1  //!<Watchdog Error
        ))
        LITTLE_ENDIAN_OCTET(7,(
          uint8_t        supplyDisconnectFailure     :1, //!<Supply disconnect Failure
          uint8_t        supplyConnectFailure        :1, //!<Supply connect Failure
          uint8_t        measurementSWChanged        :1, //!<Measurement SW changed/tampered
          uint8_t        dayLightSavingEnabled       :1, //!<Day light saving enabled
          uint8_t        clockInvalid                :1, //!<Clock Invalid
          uint8_t        communicationsTamper        :1, //!<Communications Tamper (Security failure detected on ZigBee communications link)
          uint8_t        reservedTo16                :2  //!<Reserved
        ))
        uint8_t          reservedTo24;                   //!<Reserved
      } generalFlags;
      union
      {
        struct PACK
        {
          LITTLE_ENDIAN_OCTET(5,(
            uint8_t        batteryCoverRemoved       :1, //!<Battery Cover Removed
            uint8_t        tiltTamper                :1, //!<Tilt Tamper
            uint8_t        excessFlow                :1, //!<Excess Flow
            uint8_t        reverseFlow               :1, //!<Reverse Flow
            uint8_t        reservedTo32              :4  //!<Reserved
          ))
        uint8_t            reservedTo40;                 //!<Reserved
        } gasMeterFlags;

        struct PACK
        {
          LITTLE_ENDIAN_OCTET(7,(
            uint8_t        terminalCoverRemoved      :1, //!<Terminal Cover Removed
            uint8_t        incorrectPolarity         :1, //!<Incorrect Polarity
            uint8_t        currentWithNovoltage      :1, //!<Current With no voltage
            uint8_t        limitThresholdExceeded    :1, //!<Limit threshold exceeded
            uint8_t        underVoltage              :1, //!<Under Voltage
            uint8_t        overVoltage               :1, //!<Over Voltage
            uint8_t        reservedTo32              :1  //!<Reserved
          ))
          uint8_t          reservedTo40;                 //!<Reserved
        } electricityMeterFlags;
      };
      struct PACK
      {
        LITTLE_ENDIAN_OCTET(8,(
          uint8_t            FlagA                   :1, //!<Manufacturer specific extended status flag A
          uint8_t            FlagB                   :1, //!<Manufacturer specific extended status flag B
          uint8_t            FlagC                   :1, //!<Manufacturer specific extended status flag C
          uint8_t            FlagD                   :1, //!<Manufacturer specific extended status flag D
          uint8_t            FlagE                   :1, //!<Manufacturer specific extended status flag E
          uint8_t            FlagF                   :1, //!<Manufacturer specific extended status flag F
          uint8_t            FlagG                   :1, //!<Manufacturer specific extended status flag G
          uint8_t            FlagH                   :1  //!<Manufacturer specific extended status flag H
        ))
      } manufacturerFlags;
    } value;
  } extendedStatus;
#endif /* __UK_MARKET_EXT__ */

  //!Formating attribute set (Id = 0x03)
  //!UnitofMeasure Attribute descriptor
  struct PACK
  {
    ZCL_AttributeId_t id;                         //!<Attribute Id (0x0300)
    uint8_t           type;                       //!<Attribute type (8-bit Enumeration)
    uint8_t           properties;                 //!<Attribute properties bitmask
    uint8_t           value;                      //!<Attribute value
  } unitofMeasure;


  //!Summation Formatting Attribute descriptor
  struct PACK
  {
    ZCL_AttributeId_t id;                         //!<Attribute Id (0x0303)
    uint8_t           type;                       //!<Attribute type (8 bit Bitmap)
    uint8_t           properties;                 //!<Attribute properties bitmask
    /**
    \brief Attribute value
    Summation Formatting provides a method to properly decipher the number of digits
    and the decimal location of the values found in the Summation Information Set
    of the attributes.
    */
    struct PACK
    {
      LITTLE_ENDIAN_OCTET(3,(
        uint8_t         right   :3,               //!<Number of Digits to the right of the Decimal Point
        uint8_t         left    :4,               //!<Number of Digits to the left of the Decimal Point
        uint8_t         zeros   :1                //!<If set, suppress leading zeros
      ))
    } value;
  } summationFormatting;

  //!Metering Device Type Attribute descriptor
  struct PACK
  {
    ZCL_AttributeId_t id;                         //!<Attribute Id (0x0306)
    uint8_t type;                                 //!<Attribute type (8 bit Bitmap)
    uint8_t properties;                           //!<Attribute properties bitmask
    /**
      \brief Attribute value
      MeteringDeviceType provides a lable for identifying of metering device present.
      The attribute are enumerated values presenting Energy, GAs, Water, Thermal, and
      mirrored metering device.
    */
    uint8_t value;
  } meteringDeviceType;

#ifdef __UK_MARKET_EXT__
  //!Metering Device Site ID Attribute descriptor
  struct PACK
  {
    ZCL_AttributeId_t id;                         //!<Attribute Id (0x0308)
    uint8_t type;                                 //!<Attribute type (32 character Octet String)
    uint8_t properties;                           //!<Attribute properties bitmask
    uint8_t value[32];                            //!<Attribute value (default ---)
  } siteID;

  //!Metering Device Operating Tariff Label Attribute descriptor
  struct PACK
  {
    ZCL_AttributeId_t id;                         //!<Attribute Id (0x030F)
    uint8_t type;                                 //!<Attribute type (12 character Octet String)
    uint8_t properties;                           //!<Attribute properties bitmask
    uint8_t value[12];                            //!<Attribute value (default ---)
  } operatingTariffLabel;

  //!Supply Limit Attribute Set (Id = 0x06)
  //!Metering Device Demand Limit Arm Duration(in minutes) Attribute descriptor
  struct PACK
  {
    ZCL_AttributeId_t id;                         //!<Attribute Id (0x0604)
    uint8_t type;                                 //!<Attribute type (uint8_t 16 bit Integer)
    uint8_t properties;                           //!<Attribute properties bitmask
    uint16_t value;                               //!<Attribute value (default 0x0001)
  } demandLimitArmDuration;

  //!Alarms Attribute Set (Id = 0x08)
  //!Metering Device Generic Alarm Mask Attribute descriptor
  struct PACK
  {
    ZCL_AttributeId_t id;                         //!<Attribute Id (0x0800)
    uint8_t type;                                 //!<Attribute type (64 bit Bitmap)
    uint8_t properties;                           //!<Attribute properties bitmask
    struct PACK                                   //!<Attribute value (default 0xFFFFFFFF)
    {
      LITTLE_ENDIAN_OCTET(8,(
        uint8_t        checkMeter                    :1, //!<Check Meter
        uint8_t        lowBattery                    :1, //!<Low Battery
        uint8_t        tamperDetect                  :1, //!<Tamper Detect
        uint8_t        sourseFailure                 :1, //!<Electricity: Power Failure; Gas: Not Defined; Water: Pipe Empty; Heat/Cooling: Temperature Sensor;
        uint8_t        sourseQuality                 :1, //!<Electricity: Power Quality; Gas: Low Pressure; Water: Low Pressure; Heat/Cooling: Burst Detect
        uint8_t        leakDetect                    :1, //!<Leak Detect
        uint8_t        serviceDisconnect             :1, //!<Service Disconnect
        uint8_t        checkSourse                   :1  //!<Electricity: Reserved; Gas: Reverse Flow; Water: Reverse Flow; Heat/Cooling: Flow Sensor
      ))
      LITTLE_ENDIAN_OCTET(8,(
        uint8_t        meterCoverRemoved             :1, //!<Meter Cover Removed
        uint8_t        strongMagneticFieldDetected   :1, //!<Strong Magnetic Field detected
        uint8_t        batteryFailure                :1, //!<Battery Failure
        uint8_t        programMemoryError            :1, //!<Program Memory Error
        uint8_t        ramError                      :1, //!<RAM Error
        uint8_t        nvMemoryError                 :1, //!<NV Memory Error
        uint8_t        measurementSystemError        :1, //!<Measurement System Error
        uint8_t        watchdogError                 :1  //!<Watchdog Error
      ))
      LITTLE_ENDIAN_OCTET(7,(
        uint8_t        supplydisconnectFailure       :1, //!<Supply disconnect Failure
        uint8_t        supplyconnectFailure          :1, //!<Supply connect Failure
        uint8_t        measurementSWChanged          :1, //!<Measurement SW changed/tampered
        uint8_t        dayLightSavingEnabled         :1, //!<Day light saving enabled
        uint8_t        clockInvalid                  :1, //!<Clock Invalid
        uint8_t        communicationsTamper          :1, //!<Communications Tamper (Security failure detected on ZigBee communications link)
        uint8_t        reservedTo24                  :2  //!<Reserved
      ))
      uint8_t          reservedTo64[5]                   //!<Reserved
    } value;
  } genericAlarmMask;

  //!Metering Device Gas Specific Alarm Mask Attribute descriptor
  struct PACK
  {
    ZCL_AttributeId_t id;                         //!<Attribute Id (0x0805)
    uint8_t type;                                 //!<Attribute type (16 bit Bitmap)
    uint8_t properties;                           //!<Attribute properties bitmask
    struct PACK                                   //!<Attribute value (default 0xFFFF)
    {
      LITTLE_ENDIAN_OCTET(4,(
        uint8_t        batteryCoverRemoved    :1, //!<Battery Cover Removed
        uint8_t        tiltTamper             :1, //!<Tilt Tamper
        uint8_t        excessFlow             :1, //!<Excess Flow
        uint8_t        reservedTo8            :5  //!<Reserved
      ))
      uint8_t        ReservedTo16;                //!<Reserved
    } value;
  } gasSpecificAlarmMask;

  //!Metering Device Manufacturer specific Mask Attribute descriptor
  struct PACK
  {
    ZCL_AttributeId_t   id;                       //!<Attribute Id (0x0806)
    uint8_t             type;                     //!<Attribute type (16 bit Bitmap)
    uint8_t             properties;               //!<Attribute properties bitmask
    struct PACK                                   //!<Attribute value (default 0xFFFF)
    {
      LITTLE_ENDIAN_OCTET(8,(
        uint8_t        FlagA                  :1, //!<Manufacturer specific extended status flag A
        uint8_t        FlagB                  :1, //!<Manufacturer specific extended status flag B
        uint8_t        FlagC                  :1, //!<Manufacturer specific extended status flag C
        uint8_t        FlagD                  :1, //!<Manufacturer specific extended status flag D
        uint8_t        FlagE                  :1, //!<Manufacturer specific extended status flag E
        uint8_t        FlagF                  :1, //!<Manufacturer specific extended status flag F
        uint8_t        FlagG                  :1, //!<Manufacturer specific extended status flag G
        uint8_t        FlagH                  :1  //!<Manufacturer specific extended status flag H
      ))
      uint8_t        reservedTo16;                //!<Reserved
    } value;
  } manufactureAlarmMask;

  //!Block Information (Received)(Id = 0x09)

  //!Metering Device Current Tier N Block N Summation Received Attribute descriptors

  STRUCT_BLOCK_RECEIVED_INFORMATION_ATTRIBUTS_SET()//!<Attributs type (uint8_t 48-bit integer)

  //!Billing Attribute Set (Id = 0x0A)
  //!Metering Device Bill To Date Attribute descriptor
  struct PACK
  {
    ZCL_AttributeId_t   id;                       //!<Attribute Id (0x0A00)
    uint8_t             type;                     //!<Attribute type(uint8_t 32 bit Integer)
    uint8_t             properties;               //!<Attribute properties bitmask
    uint32_t            value;                    //!<Attribute value (default 0x00)
  } billToDate;

  //!Metering Device Bill To Date Time Stamp Attribute descriptor
  struct PACK
  {
    ZCL_AttributeId_t   id;                       //!<Attribute Id (0x0A01)
    uint8_t             type;                     //!<Attribute type (UTCTime - 32-bit) (typeId = 0xe2, #ZCL_UTC_TIME_DATA_TYPE_ID)
    uint8_t             properties;               //!<Attribute properties bitmask
    ZCL_UTCTime_t       value;                    //!<Attribute value (default 0)
  } billToDateTimeStamp;

  //!Metering Device Projected Bill Attribute descriptor
  struct PACK
  {
    ZCL_AttributeId_t   id;                       //!<Attribute Id (0x0A02)
    uint8_t             type;                     //!<Attribute type(uint8_t 32 bit Integer)
    uint8_t             properties;               //!<Attribute properties bitmask
    uint32_t            value;                    //!<Attribute value (default 0x00)
  } projectedBill;

  //!Metering Device Projected Bill Time Stamp Attribute descriptor
  struct PACK
  {
    ZCL_AttributeId_t   id;                       //!<Attribute Id (0x0A03)
    uint8_t             type;                     //!<Attribute type (UTCTime - 32-bit) (typeId = 0xe2, #ZCL_UTC_TIME_DATA_TYPE_ID)
    uint8_t             properties;               //!<Attribute properties bitmask
    ZCL_UTCTime_t       value;                    //!<Attribute value (default 0)
  } projectedBillTimeStamp;

#endif // __UK_MARKET_EXT__
} SimpleMeteringServerClusterAttributes_t;

#ifdef __UK_MARKET_EXT__
typedef struct PACK
{
  //!Notification Attribute Set (Id = 0x00)
  //!Notification Control Flags Attribute descriptor
  struct PACK
  {
    ZCL_AttributeId_t   id;                       //!<Attribute Id (0x0000)
    uint8_t             type;                     //!<Attribute type (8 bit Bitmap)
    uint8_t             properties;               //!<Attribute properties bitmask
    struct PACK                                   //!<Attribute value (default 0x00)
    {
      LITTLE_ENDIAN_OCTET(3,(
        uint8_t        mirrorReportAttributeResponse :1, //!<Battery Cover Removed
        uint8_t        changeReportingProfile        :1, //!<Battery Cover Removed
        uint8_t        reserved                      :6  //!<Reserved
      ))
    } value;
  } notificationControlFlags;

  //!Notification Flags Attribute descriptor
  struct PACK
  {
    ZCL_AttributeId_t   id;                       //!<Attribute Id (0x0001)
    uint8_t             type;                     //!<Attribute type (8 bit Bitmap)
    uint8_t             properties;               //!<Attribute properties bitmask
    struct PACK                                   //!<Attribute value (default 0x00)
    {
      LITTLE_ENDIAN_OCTET(7,(
        uint8_t        newOTAFirmware         :1, //!<New OTA Firmware
        uint8_t        CBKEUpdateRequest      :1, //!<CBKE Update Request
        uint8_t        timeSync               :1, //!<Time Sync
        uint8_t        newPassword            :1, //!<New Password
        uint8_t        reserved               :2, //!<Reserved
        uint8_t        stayAwakeRequestHAN    :1, //!<Stay Awake Request HAN
        uint8_t        stayAwakeRequestHES    :1  //!<Stay Awake Request HES
      ))
    } value;
  } notificationFlags;

  //!Price Notification Flags Attribute descriptor
  struct PACK
  {
    ZCL_AttributeId_t   id;                       //!<Attribute Id (0x0002)
    uint8_t             type;                     //!<Attribute type (16 bit Bitmap)
    uint8_t             properties;               //!<Attribute properties bitmask
    struct PACK                                   //!<Attribute value (default 0x0000)
    {
      LITTLE_ENDIAN_OCTET(8,(
        uint8_t        PublishPrice              :1, //!<Publish Price
        uint8_t        PublishBlockPeriod        :1, //!<Publish Block Period
        uint8_t        PublishTariffInformation  :1, //!<Publish Tariff Information
        uint8_t        PublishConversionFactor   :1, //!<Publish Conversion Factor
        uint8_t        PublishCalorificValue     :1, //!<Publish Calorific Value
        uint8_t        PublishCO2Value           :1, //!<Publish CO2 Value
        uint8_t        PublishBillingPeriod      :1, //!<Publish Billing Period
        uint8_t        PublishConsolidatedBill   :1  //!<Publish Consolidated Bill
      ))
      uint8_t        reservedTo16;                   //!<Reserved
    } value;
  } priceNotificationFlags;

  //!Calendar Notification Flags Attribute descriptor
  struct PACK
  {
    ZCL_AttributeId_t   id;                       //!<Attribute Id (0x0003)
    uint8_t             type;                     //!<Attribute type (8 bit Bitmap)
    uint8_t             properties;               //!<Attribute properties bitmask
    struct PACK                                   //!<Attribute value (default 0x00)
    {
      LITTLE_ENDIAN_OCTET(6,(
        uint8_t        publishCalendar        :1, //!<PublishCalendar
        uint8_t        publishSpecialDays     :1, //!<PublishSpecialDays
        uint8_t        publishSeasons         :1, //!<PublishSeasons
        uint8_t        publishWeek            :1, //!<PublishWeek
        uint8_t        publishDay             :1, //!<PublishDay
        uint8_t        reserved               :3  //!<Reserved
      ))
    } value;
  } calendarNotificationFlags;

  //!PrePay Notification Flags Attribute descriptor
  struct PACK
  {
    ZCL_AttributeId_t   id;                       //!<Attribute Id (0x0004)
    uint8_t             type;                     //!<Attribute type (16 bit Bitmap)
    uint8_t             properties;               //!<Attribute properties bitmask
    struct PACK                                   //!<Attribute value (default 0x0000)
    {
      LITTLE_ENDIAN_OCTET(8,(
        uint8_t        selectAvailableEmergencyCredit  :1, //!<Select Available Emergency Credit
        uint8_t        changeDebt                      :1, //!<Change Debt
        uint8_t        emergencyCreditSetup            :1, //!<Emergency Credit Setup
        uint8_t        consumerTopUp                   :1, //!<Consumer Top Up
        uint8_t        creditAdjustment                :1, //!<Credit Adjustment
        uint8_t        changePaymentMode               :1, //!<Change Payment Mode
        uint8_t        getPrepaySnapshot               :1, //!<Get Prepay Snapshot
        uint8_t        reservedTo8                     :1  //!<Reserved
      ))
      uint8_t        reservedTo16;                         //!<Reserved
    } value;
  } prePayNotificationFlags;

  //!Device Management Flags Attribute descriptor
  struct PACK
  {
    ZCL_AttributeId_t   id;                       //!<Attribute Id (0x0005)
    uint8_t             type;                     //!<Attribute type (8 bit Bitmap)
    uint8_t             properties;               //!<Attribute properties bitmask
    struct PACK                                   //!<Attribute value (default 0x00)
    {
      LITTLE_ENDIAN_OCTET(4,(
        uint8_t        PublishChangeofTenancy   :1, //!<PublishChangeofTenancy
        uint8_t        PublishChangeofSupplier  :1, //!<PublishChangeofSupplier
        uint8_t        ChangeSupply             :1, //!<ChangeSupply
        uint8_t        Reserved                 :5  //!<Reserved for future expansion
      ))
    } value;
  } DeviceManagementFlags;
} SimpleMeteringClientClusterAttributes_t;
#endif // __UK_MARKET_EXT__
/***************************************************************************//**
  \brief
    ZCL Simple Metering Cluster Get Profile Response Command payload format
*******************************************************************************/
typedef struct PACK _GetProfileResponseData_t
{
  ZCL_UTCTime_t endTime;
  uint8_t status;
  uint8_t profileIntervalPeriod;
  uint8_t numberOfPeriodsDelivered;
  uint8_t intervals[];
} GetProfileResponseData_t;

/***************************************************************************//**
  \brief
    ZCL Simple Metering Cluster Get Profile Command payload format
*******************************************************************************/
typedef struct PACK _GetProfileData_t
{
  uint8_t intervalChannel;
  ZCL_UTCTime_t endTime;
  uint8_t NumberOfPeriods;
} GetProfileData_t;

/***************************************************************************//**
  \brief
    ZCL Simple Metering Cluster Request Mirror Response Command payload format
*******************************************************************************/
typedef struct PACK _RequestMirrorResponseData_t
{
  uint16_t endPointId;
} RequestMirrorResponseData_t;

/***************************************************************************//**
  \brief
    ZCL Simple Metering Cluster Mirror Removed Command payload format
*******************************************************************************/
typedef struct PACK _MirrorRemovedData_t
{
  uint16_t endPointId;
} MirrorRemovedData_t;

#ifdef __UK_MARKET_EXT__
/***************************************************************************//**
  \brief
    ZCL Simple Metering Cluster Get Snapshot Command payload format
*******************************************************************************/
typedef struct PACK _GetSnapshotData_t
{
  ZCL_UTCTime_t startTime;
  uint8_t       numberOfSnapshots;
  uint16_t      snapshotCause;
} GetSnapshotData_t;

/***************************************************************************//**
  \brief
    ZCL Simple Metering Cluster Get Snapshot Response Command payload format
*******************************************************************************/
typedef struct PACK _GetSnapshotResponseData_t
{
  uint32_t      issuerEventId;
  ZCL_UTCTime_t snapshotTime;
  uint8_t       commandIndex;
  uint16_t      snapshotCause;
  uint8_t       snapshotPayloadType;
  union PACK
  {
    struct PACK
    {
      uint8_t  currentSummationDelivered[6];
    } currentSummation;

    struct PACK
    {
      uint8_t  numberOfTiersInUse;
      uint8_t  tierSummation[6];
    } touInformation;

    struct PACK
    {
      uint32_t  numberOfTiersAndBlock;
      uint8_t  tierBlockSummationThresholdsInUse;
    } blockInformation;

  } snapshotPayload;
} GetSnapshotResponseData_t;

/***************************************************************************//**
  \brief
    ZCL Simple Metering Cluster Mirror  Report  Attribute Response Command payload format
*******************************************************************************/
typedef struct PACK _MirrorReportAttributeResponseData_t
{
  uint8_t       notificationFlags;
  uint16_t      priceNotification;
  uint8_t       calendarNotificationFlags;
  uint16_t      PrePayNotificationFlags;
  uint8_t       deviceManagementNotificationFlags;
} MirrorReportAttributeResponseData_t;
#endif // __UK_MARKET_EXT__

END_PACK

/***************************************************************************//**
  \brief
    ZCL Simple Metering Server Cluster Commands
*******************************************************************************/
typedef struct
{
  //! Get Profile Response Command descriptor
  struct
  {
    ZCL_CommandId_t       id;       //!<Command Id (0x00)
    ZclCommandOptions_t   options;  //!<Command options
    /**
      \brief Command indication handler
      \param payloadLength - length of payload in octets
      \param payload - payload
      \return none
    */
    ZCL_Status_t (*indHandler)(ZCL_Addressing_t *addressing, uint8_t payloadLength, void *payload);
  } getProfileResponseCommand;

  //! Request Mirror Command descriptor
  struct
  {
    ZCL_CommandId_t       id;       //!<Command Id (0x01)
    ZclCommandOptions_t   options;  //!<Command options
    /**
      \brief Command indication handler
      \param payloadLength - length of payload in octets
      \param payload - payload
      \return none
    */
    ZCL_Status_t (*indHandler)(ZCL_Addressing_t *addressing, uint8_t payloadLength, void *payload);
  } requestMirrorCommand;

  //! Remove Mirror Command descriptor
  struct
  {
    ZCL_CommandId_t       id;       //!<Command Id (0x02)
    ZclCommandOptions_t   options;  //!<Command options
    /**
      \brief Command indication handler
      \param payloadLength - length of payload in octets
      \param payload - payload
      \return none
    */
    ZCL_Status_t (*indHandler)(ZCL_Addressing_t *addressing, uint8_t payloadLength, void *payload);
  } removeMirrorCommand;

#ifdef __UK_MARKET_EXT__
  //! Get Snapshot Response Command descriptor
  struct
  {
    ZCL_CommandId_t       id;       //!<Command Id (0x04)
    ZclCommandOptions_t   options;  //!<Command options
    /**
      \brief Command indication handler
      \param payloadLength - length of payload in octets
      \param payload - payload
      \return none
    */
    ZCL_Status_t (*indHandler)(ZCL_Addressing_t *addressing, uint8_t payloadLength, GetSnapshotResponseData_t *payload);
  } getSnapshotResponseCommand;
#endif // __UK_MARKET_EXT__

} SimpleMeteringServerClusterCommands_t;

/***************************************************************************//**
  \brief
    ZCL Simple Metering Client Cluster Commands
*******************************************************************************/
typedef struct
{
  //! Get Profile Command descriptor
  struct
  {
    ZCL_CommandId_t       id;       //!<Command Id (0x00)
    ZclCommandOptions_t   options;  //!<Command options
    /**
      \brief Command indication handler
      \param payloadLength - length of payload in octets
      \param payload - payload
      \return none
    */
    ZCL_Status_t (*indHandler)(ZCL_Addressing_t *addressing, uint8_t payloadLength, void *payload);
  } getProfileCommand;

  //! Request Mirror Response Command descriptor
  struct
  {
    ZCL_CommandId_t       id;       //!<Command Id (0x01)
    ZclCommandOptions_t   options;  //!<Command options
    /**
      \brief Command indication handler
      \param payloadLength - length of payload in octets
      \param payload - payload
      \return none
    */
    ZCL_Status_t (*indHandler)(ZCL_Addressing_t *addressing, uint8_t payloadLength, RequestMirrorResponseData_t *payload);
  } requestMirrorResponseCommand;

  //! Mirror Removed Command descriptor
  struct
  {
    ZCL_CommandId_t       id;       //!<Command Id (0x02)
    ZclCommandOptions_t   options;  //!<Command options
    /**
      \brief Command indication handler
      \param payloadLength - length of payload in octets
      \param payload - payload
      \return none
    */
    ZCL_Status_t (*indHandler)(ZCL_Addressing_t *addressing, uint8_t payloadLength, MirrorRemovedData_t *payload);
  } mirrorRemovedCommand;

#ifdef __UK_MARKET_EXT__
  //! Get Snapshot Command descriptor
  struct
  {
    ZCL_CommandId_t       id;       //!<Command Id (0x04)
    ZclCommandOptions_t   options;  //!<Command options
    /**
      \brief Command indication handler
      \param payloadLength - length of payload in octets
      \param payload - payload
      \return none
    */
    ZCL_Status_t (*indHandler)(ZCL_Addressing_t *addressing, uint8_t payloadLength, GetSnapshotData_t *payload);
  } getSnapshotCommand;

  //! Take Snapshot Command descriptor
  struct
  {
    ZCL_CommandId_t       id;       //!<Command Id (0x05)
    ZclCommandOptions_t   options;  //!<Command options
    /**
      \brief Command indication handler
      \param payloadLength - length of payload in octets
      \param payload - payload
      \return none
    */
    ZCL_Status_t (*indHandler)(ZCL_Addressing_t *addressing, uint8_t payloadLength, void *payload);
  } takeSnapshotCommand;

  //! Mirror Report Attribute Response Command descriptor
  struct
  {
    ZCL_CommandId_t       id;       //!<Command Id (0x06)
    ZclCommandOptions_t   options;  //!<Command options
    /**
      \brief Command indication handler
      \param payloadLength - length of payload in octets
      \param payload - payload
      \return none
    */
    ZCL_Status_t (*indHandler)(ZCL_Addressing_t *addressing, uint8_t payloadLength, MirrorReportAttributeResponseData_t *payload);
  } mirrorReportAttributeResponseCommand;

#endif // __UK_MARKET_EXT__

} SimpleMeteringClientClusterCommands_t;

#endif // _SIMPLEMETERINGCLUSTER_H
