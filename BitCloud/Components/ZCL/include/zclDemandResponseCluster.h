/************************************************************************//**
  \file zclDemandResponseCluster.h

  \brief
    The header file describes the Demand Response Cluster.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    03.02.09 A. Mandychev - Created.
******************************************************************************/

#ifndef _ZCLDEMANDRESPONSECLUSTER_H
#define _ZCLDEMANDRESPONSECLUSTER_H

/******************************************************************************
                        Includes section
******************************************************************************/
#include <zcl.h>
#include <clusters.h>

/******************************************************************************
                        Defines section
******************************************************************************/
/**
 * \brief Demand Response Server Cluster attributes amount.
*/
#define DRLC_CLUSTER_SERVER_ATTRIBUTES_AMOUNT     0

/**
 * \brief Demand Response Client Cluster attributes amount.
*/
#define DRLC_CLUSTER_CLIENT_ATTRIBUTES_AMOUNT     4

/**
 * \brief Demand Response Cluster commands amount.
*/
#define DRLC_CLUSTER_COMMANDS_AMOUNT  5

/**
 * \brief Demand Response Server Cluster commands identifiers.
*/
#define LOAD_CONTROL_EVENT_COMMAND_ID               0x00
#define CANCEL_LOAD_CONTROL_EVENT_COMMAND_ID        0x01
#define CANCEL_ALL_LOAD_CONTROL_EVENTS_COMMAND_ID   0x02

/**
 * \brief Demand Response Client Cluster commands identifiers.
*/
#define REPORT_EVENT_STATUS_COMMAND_ID              0x00
#define GET_SCHEDULED_EVENTS_COMMAND_ID             0x01

/**
 * \brief Demand Response Client Cluster attributes identifiers.
*/
#define ZCL_DRLC_CLUSTER_UTILITY_ENROLMENT_GROUP_ID    CCPU_TO_LE16(0x0000)
#define ZCL_DRLC_CLUSTER_START_RANDOMIZE_MINUTES_ID    CCPU_TO_LE16(0x0001)
#define ZCL_DRLC_CLUSTER_STOP_RANDOMIZE_MINUTES_ID     CCPU_TO_LE16(0x0002)
#define ZCL_DRLC_CLUSTER_DEVICE_CLASS_VALUE_ID         CCPU_TO_LE16(0x0003)

/**
 * \brief Demand Response Client Cluster Load Control Event command ignore values
*/
#define ZCL_DRLC_CLUSTER_LCE_IGNORE_COOLING_TEMPERATURE_OFFSET     0xFF
#define ZCL_DRLC_CLUSTER_LCE_IGNORE_HEATING_TEMPERATURE_OFFSET     0xFF
#define ZCL_DRLC_CLUSTER_LCE_IGNORE_COOLING_TEMPERATURE_SETPOINT   CCPU_TO_LE16(0x8000)
#define ZCL_DRLC_CLUSTER_LCE_IGNORE_COOLING_TEMPERATURE_SETPOINT   CCPU_TO_LE16(0x8000)
#define ZCL_DRLC_CLUSTER_LCE_IGNORE_AVERAGE_LOAD_ADJUST_PERCENTAGE 0x80
#define ZCL_DRLC_CLUSTER_LCE_IGNORE_DUTY_CYCLE                     0xFF

/**
 * \brief Demand Response Client Cluster reserved values
*/

#define ZCL_DRLC_CLUSTER_DEVICE_CLASS_MIN_RESERVED_VALUE      0xF000
#define ZCL_DRCL_CLUSTER_CRITICALITY_LEVEL_MIN_RESERVED_VALUE 0x10
#define ZCL_DRCL_CLUSTER_DURATION_IN_MINUTES_MAX_VALUE        0x05A0
#define ZCL_DRCL_CLUSTER_UTILITY_ENROLMENT_GROUP_ALL_GROUPS   0x00

/**
 * \brief Report event status identifiers
 */
#define REPORT_EVENT_STATUS_COMMAND_RECEIVED                       0x01
#define REPORT_EVENT_STATUS_EVENT_STARTED                          0x02
#define REPORT_EVENT_STATUS_EVENT_COMPLETED                        0x03
#define REPORT_EVENT_STATUS_OPT_OUT                                0x04
#define REPORT_EVENT_STATUS_EVENT_CANCELED                         0x06
#define REPORT_EVENT_STATUS_EVENT_SUPERSEDED                       0x07
#define REPORT_EVENT_STATUS_NO_PARTICIPATION                       0x0A
#define REPORT_EVENT_STATUS_INVALID_CANCEL_COMMAND_DEFAULT         0xF8
#define REPORT_EVENT_STATUS_INVALID_CANCEL_COMMAND_UNDEFINED_EVENT 0xFD
#define REPORT_EVENT_STATUS_LOAD_CONTROL_EVENT_COMMAND_REJECTED    0xFE
#define REPORT_EVENT_STATUS_REJECTED_EVENT_ALREADY_EXPIRED         0xFB

#define DEFINE_DRLC_CLUSTER_COMMANDS(clLoadControlEventInd,           \
                                     clCancelLoadControlEventInd,     \
                                     clCancelAllLoadControlEventsInd, \
                                     clReportEventStatusInd,          \
                                     clGetScheduledEventsInd) \
  DEFINE_COMMAND(loadControlEventCommand,           0x00, COMMAND_OPTIONS(SERVER_TO_CLIENT, ZCL_THERE_IS_NO_RELEVANT_RESPONSE, ZCL_COMMAND_ACK), clLoadControlEventInd), \
  DEFINE_COMMAND(cancelLoadControlEventCommand,     0x01, COMMAND_OPTIONS(SERVER_TO_CLIENT, ZCL_THERE_IS_NO_RELEVANT_RESPONSE, ZCL_COMMAND_ACK), clCancelLoadControlEventInd), \
  DEFINE_COMMAND(cancelAllLoadControlEventsCommand, 0x02, COMMAND_OPTIONS(SERVER_TO_CLIENT, ZCL_THERE_IS_NO_RELEVANT_RESPONSE, ZCL_COMMAND_ACK), clCancelAllLoadControlEventsInd), \
  DEFINE_COMMAND(reportEventStatusCommand,          0x00, COMMAND_OPTIONS(CLIENT_TO_SERVER, ZCL_THERE_IS_NO_RELEVANT_RESPONSE, ZCL_COMMAND_ACK), clReportEventStatusInd), \
  DEFINE_COMMAND(getScheduledEventsCommand,         0x01, COMMAND_OPTIONS(CLIENT_TO_SERVER, ZCL_THERE_IS_NO_RELEVANT_RESPONSE, ZCL_COMMAND_ACK), clGetScheduledEventsInd)




#define DEFINE_DRLC_CLUSTER_CLIENT_ATTRIBUTES() \
  DEFINE_ATTRIBUTE(utilityEnrolmentGroupAttr, ZCL_READWRITE_ATTRIBUTE, ZCL_DRLC_CLUSTER_UTILITY_ENROLMENT_GROUP_ID, ZCL_U8BIT_DATA_TYPE_ID),        \
  DEFINE_ATTRIBUTE(startRandomizeMinutesAttr, ZCL_READWRITE_ATTRIBUTE, ZCL_DRLC_CLUSTER_START_RANDOMIZE_MINUTES_ID, ZCL_U8BIT_DATA_TYPE_ID),        \
  DEFINE_ATTRIBUTE(endRandomizeMinutesAttr, ZCL_READWRITE_ATTRIBUTE, ZCL_DRLC_CLUSTER_STOP_RANDOMIZE_MINUTES_ID, ZCL_U8BIT_DATA_TYPE_ID),        \
  DEFINE_ATTRIBUTE(deviceClassValueAttr, ZCL_READWRITE_ATTRIBUTE, ZCL_DRLC_CLUSTER_DEVICE_CLASS_VALUE_ID, ZCL_16BIT_BITMAP_DATA_TYPE_ID)

#define DRLC_CLUSTER_ZCL_SERVER_CLUSTER_TYPE(clattributes, clcommands) \
  { \
    .id = DEMAND_RESPONSE_AND_LOAD_CONTROL_CLUSTER_ID, \
    .options = {.type = ZCL_SERVER_CLUSTER_TYPE, .security = ZCL_APPLICATION_LINK_KEY_CLUSTER_SECURITY}, \
    .attributesAmount = DRLC_CLUSTER_SERVER_ATTRIBUTES_AMOUNT, \
    .attributes = NULL, \
    .commandsAmount = DRLC_CLUSTER_COMMANDS_AMOUNT, \
    .commands = (uint8_t *) clcommands \
  }

#define DRLC_CLUSTER_ZCL_CLIENT_CLUSTER_TYPE(clattributes, clcommands) \
  { \
    .id = DEMAND_RESPONSE_AND_LOAD_CONTROL_CLUSTER_ID, \
    .options = {.type = ZCL_CLIENT_CLUSTER_TYPE, .security = ZCL_APPLICATION_LINK_KEY_CLUSTER_SECURITY}, \
    .attributesAmount = DRLC_CLUSTER_CLIENT_ATTRIBUTES_AMOUNT, \
    .attributes = (uint8_t *) clattributes, \
    .commandsAmount = DRLC_CLUSTER_COMMANDS_AMOUNT, \
    .commands = (uint8_t *) clcommands \
  }

#define DEFINE_DRLC_CLUSTER(cltype, clattributes, clcommands) \
  DRLC_CLUSTER_##cltype(clattributes, clcommands)

#define DRLC_CLUSTER_ZCL_SERVER_CLUSTER_TYPE_FLASH(clattributes, clcommands) \
  { \
    .id = DEMAND_RESPONSE_AND_LOAD_CONTROL_CLUSTER_ID, \
    .options = {.type = ZCL_SERVER_CLUSTER_TYPE, .security = ZCL_APPLICATION_LINK_KEY_CLUSTER_SECURITY}, \
    .attributesAmount = DRLC_CLUSTER_SERVER_ATTRIBUTES_AMOUNT, \
    .attributes = NULL, \
    .commandsAmount = DRLC_CLUSTER_COMMANDS_AMOUNT, \
    .commands = (FLASH_PTR uint8_t *) clcommands \
  }

#define DRLC_CLUSTER_ZCL_CLIENT_CLUSTER_TYPE_FLASH(clattributes, clcommands) \
  { \
    .id = DEMAND_RESPONSE_AND_LOAD_CONTROL_CLUSTER_ID, \
    .options = {.type = ZCL_CLIENT_CLUSTER_TYPE, .security = ZCL_APPLICATION_LINK_KEY_CLUSTER_SECURITY}, \
    .attributesAmount = DRLC_CLUSTER_CLIENT_ATTRIBUTES_AMOUNT, \
    .attributes = (uint8_t *) clattributes, \
    .commandsAmount = DRLC_CLUSTER_COMMANDS_AMOUNT, \
    .commands = (FLASH_PTR uint8_t *) clcommands \
  }

#define DEFINE_DRLC_CLUSTER_FLASH(cltype, clattributes, clcommands) \
  DRLC_CLUSTER_##cltype##_FLASH(clattributes, clcommands)
//! Signature type. AMI r15 profile spec., D.2.3.3.1.1.
#define ZCL_ECDSA_SIGNATURE_TYPE 0x01
//! Signature length. AMI r15 profile spec., D.2.3.3.1.1.
#define REPORT_EVENT_STATUS_SIGNATURE_LENGTH 42

/******************************************************************************
                    Types section
******************************************************************************/

BEGIN_PACK

/**
 * \brief Device Class type.
*/
typedef union PACK
{
  uint16_t value;
  struct 
  {
    LITTLE_ENDIAN_OCTET(8,(
      uint16_t hvacCompressor : 1,
      uint16_t stripHeaters : 1,
      uint16_t waterHeater : 1,
      uint16_t poolPump : 1,
      uint16_t smartAppliances : 1,
      uint16_t irrigationPump : 1,
      uint16_t managedCommercAndIndustrLoads : 1,
      uint16_t simpleMiscLoads : 1
    ))
    LITTLE_ENDIAN_OCTET(5,(
      uint16_t exteriorLighting : 1,
      uint16_t interiorLighting : 1,
      uint16_t electricalVehicle : 1,
      uint16_t generationSystem : 1,
      uint16_t reserved : 4
    ))
  };
} ZCL_DeviceClass_t;

/**
 * \brief Device Class type.
*/
typedef struct PACK
{
  LITTLE_ENDIAN_OCTET(3,(
    uint8_t randStartTime : 1,
    uint8_t randEndTime : 1,
    uint8_t reserved : 6
  ))
} ZCL_EventControl_t;

/**
 * \brief The Load Control Event Command Payload.
*/
typedef struct PACK
{
  uint32_t  issuerEventId;
  ZCL_DeviceClass_t  deviceClass;
  uint8_t   utilityEnrolmentGroup;
  uint32_t  startTime;
  uint16_t  durationInMinutes;
  uint8_t   criticalityLevel;
  uint8_t   coolingTemperatureOffset;
  uint8_t   heatingTemperatureOffset;
  uint16_t  coolingTemperatureSetPoint;
  uint16_t  heatingTemperatureSetPoint;
  int8_t    avrgLoadAdjstPercentage;
  uint8_t   dutyCycle;
  ZCL_EventControl_t eventControl;
} ZCL_LoadControlEvent_t;

/**
 * \brief The Cancel Control type.
*/
typedef struct PACK
{
  LITTLE_ENDIAN_OCTET(2,(
    uint8_t randomizeEnd : 1,
    uint8_t reserved : 7
  ))
} ZCL_CancelControl_t;

/**
 * \brief The Cancel Load Control Event Command Payload.
*/
typedef struct PACK
{
  uint32_t  issuerEventId;
  ZCL_DeviceClass_t  deviceClass;
  uint8_t   utilityEnrolmentGroup;
  ZCL_CancelControl_t  cancelControl;
  ZCL_UTCTime_t  effectiveTime;
} ZCL_CancelLoadControlEvent_t;

typedef struct PACK
{
  ZCL_CancelControl_t cancelControl; // ZCL_U8BIT_BITMAP_TYPE_ID
} ZCL_CancelAllLoadControlEvents_t;

typedef struct PACK
{
  //! Utility Enrolment Group Attribute descriptor
  struct PACK
  {
    ZCL_AttributeId_t id;                         //!<Attribute Id (0x0200)
    uint8_t           type;                       //!<Attribute data type (8 bit Bitmap)
    uint8_t           properties;                 //!<Attribute properties bitmask
    uint8_t           utilityEnrolmentGroup;
  } utilityEnrolmentGroupAttr;

  //! Start Randomize Minutes Attribute descriptor
  struct PACK
  {
    ZCL_AttributeId_t id;                         //!<Attribute Id (0x0200)
    uint8_t           type;                       //!<Attribute data type (8 bit Bitmap)
    uint8_t           properties;                 //!<Attribute properties bitmask
    uint8_t           startRandomizeMinutes;
  } startRandomizeMinutesAttr;

  //! End Randomize Minutes Attribute descriptor
  struct PACK
  {
    ZCL_AttributeId_t id;                         //!<Attribute Id (0x0200)
    uint8_t           type;                       //!<Attribute data type (8 bit Bitmap)
    uint8_t           properties;                 //!<Attribute properties bitmask
    uint8_t           endRandomizeMinutes;
  } endRandomizeMinutesAttr;

  //! Device Class Value Attribute descriptor
  struct PACK
  {
    ZCL_AttributeId_t   id;                         //!<Attribute Id (0x0200)
    uint8_t             type;                       //!<Attribute data type (8 bit Bitmap)
    uint8_t             properties;                 //!<Attribute properties bitmask
    ZCL_DeviceClass_t   deviceClassValue;
  } deviceClassValueAttr;

} ZCL_DemandResponseClientClusterAttributes_t;

/**
 * \brief Report event status type.
*/
typedef struct PACK
{
  uint32_t            issuerEventId;
  uint8_t             eventStatus;
  ZCL_UTCTime_t       eventStatusTime;
  uint8_t             criticalityLevelApplied;
  uint16_t            coolingTemperatureSetPointApplied;
  uint16_t            heatingTemperatureSetPointApplied;
  int8_t              avrgLoadAdjstPercentageApplied;
  uint8_t             dutyCycleApplied;
  ZCL_EventControl_t  eventControl;
  uint8_t             signatureType;
  uint8_t             signature[REPORT_EVENT_STATUS_SIGNATURE_LENGTH];
} ZCL_ReportEventStatus_t;

/**
 * \brief Get scheduled events type.
*/
typedef struct PACK
{
  ZCL_UTCTime_t startTime;
  uint8_t       numberOfEvents;
} ZCL_GetScheduledEvents_t;

END_PACK

/**
 * \brief ZCL Demand Response Cluster Commands.
*/
typedef struct
{
  //! Load Control Event Command descriptor
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
    ZCL_Status_t (*loadControlEvent)(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_LoadControlEvent_t *payload);
  } loadControlEventCommand;

  //! Cancel Load Control Event Command descriptor
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
    ZCL_Status_t (*cancelLoadControlEvent)(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_CancelLoadControlEvent_t *payload);
  } cancelLoadControlEventCommand;

  //! Confirm Key Data Request Command descriptor
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
    ZCL_Status_t (*cancelAllLoadControlEvents)(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_CancelAllLoadControlEvents_t *payload);
  } cancelAllLoadControlEventsCommand;
  //! Report Event Status Command descriptor
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
    ZCL_Status_t (*reportEventStatus)(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_ReportEventStatus_t *payload);
  } reportEventStatusCommand;

  //! Confirm Key Data Request Command descriptor
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
    ZCL_Status_t (*getScheduledEvents)(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_GetScheduledEvents_t *payload);
  } getScheduledEventsCommand;

} ZCL_DemandResponseAndLoadControlClusterCommands_t;
/**
 * \brief ZCL Demand Response Cluster Commands into flash memory.
*/

#endif /*_ZCLDEMANDRESPONSECLUSTER_H*/
