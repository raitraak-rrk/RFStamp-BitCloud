/***************************************************************************//**
  \file zclFanControlCluster.h

  \brief
    The header file describes the ZCL Fan control Cluster and its interface

    The file describes the types and interface of the ZCL Fan control Cluster

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    6.11.14 Viswanadham Kotla - Created.
*******************************************************************************/

#ifndef _ZCLFANCONTROLCLUSTER_H
#define _ZCLFANCONTROLCLUSTER_H

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
 * \brief Fan Control cluster attributes values
*/

#define ZCL_FAN_CONTROL_CL_FAN_MODE_SER_ATTR_MIN_VAL                (0x00)
#define ZCL_FAN_CONTROL_CL_FAN_MODE_SER_ATTR_MAX_VAL                (0x06)
#define ZCL_FAN_CONTROL_CL_FAN_MODE_SER_ATTR_DEFAULT_VAL            (0x05)

#define ZCL_FAN_CONTROL_CL_FAN_SEQUENCE_OPERATION_SER_ATTR_MIN_VAL        (0x00)
#define ZCL_FAN_CONTROL_CL_FAN_SEQUENCE_OPERATION_SER_ATTR_MAX_VAL        (0x04)
#define ZCL_FAN_CONTROL_CL_FAN_SEQUENCE_OPERATION_SER_ATTR_DEFAULT_VAL    (0x02)


/**
 * \brief Fan control Cluster server attributes amount
*/

#define ZCL_FAN_CONTROL_CLUSTER_SERVER_ATTRIBUTES_AMOUNT (2)

/**
 * \brief Fan control Cluster client attributes amount. Clinet doesn't have attributes.
*/

#define ZCL_FAN_CONTROL_CLUSTER_CLIENT_ATTRIBTUES_AMOUNT  (0)

/**
 * \brief Fan control Cluster commands amount
*/

#define ZCL_FAN_CONTROL_CLUSTER_COMMANDS_AMOUNT   (0)  

/**
 * \brief Fan mode Cluster server's attributes identifiers
*/
#define ZCL_FAN_CONTROL_CLUSTER_FAN_MODE_SERVER_ATTRIBUTE_ID                CCPU_TO_LE16(0x0000)
#define ZCL_FAN_CONTROL_CLUSTER_FAN_MODE_SEQUENCE_SERVER_ATTRIBUTE_ID       CCPU_TO_LE16(0x0001)

/**
 * \brief Fan control Cluster server define attributes macros
*/

#define ZCL_DEFINE_FAN_CONTROL_CLUSTER_SERVER_ATTRIBUTES() \
    DEFINE_ATTRIBUTE_WITH_BOUNDARY(fanMode,\
                                   ZCL_READWRITE_ATTRIBUTE,\
                                   ZCL_FAN_CONTROL_CLUSTER_FAN_MODE_SERVER_ATTRIBUTE_ID,\
                                   ZCL_8BIT_ENUM_DATA_TYPE_ID,\
                                   ZCL_FAN_CONTROL_CL_FAN_MODE_SER_ATTR_MIN_VAL,\
                                   ZCL_FAN_CONTROL_CL_FAN_MODE_SER_ATTR_MAX_VAL),\
    DEFINE_ATTRIBUTE_WITH_BOUNDARY(fanModeSequence, \
                                   ZCL_READWRITE_ATTRIBUTE, \
                                   ZCL_FAN_CONTROL_CLUSTER_FAN_MODE_SEQUENCE_SERVER_ATTRIBUTE_ID, \
                                   ZCL_8BIT_ENUM_DATA_TYPE_ID, \
                                   ZCL_FAN_CONTROL_CL_FAN_SEQUENCE_OPERATION_SER_ATTR_MIN_VAL,\
                                   ZCL_FAN_CONTROL_CL_FAN_SEQUENCE_OPERATION_SER_ATTR_MAX_VAL)

/**
 * \brief Fan Control Cluster definition macros
*/

#define FAN_CONTROL_CLUSTER_ZCL_CLIENT_CLUSTER_TYPE(clattributes, clcommands) \
{ \
   .id = FAN_CONTROL_CLUSTER_ID, \
   .options = { \
                .type = ZCL_CLIENT_CLUSTER_TYPE, \
                .security = ZCL_NETWORK_KEY_CLUSTER_SECURITY, \
              }, \
   .attributesAmount = ZCL_FAN_CONTROL_CLUSTER_CLIENT_ATTRIBTUES_AMOUNT, \
   .attributes = (uint8_t *)clattributes, \
   .commandsAmount = ZCL_FAN_CONTROL_CLUSTER_COMMANDS_AMOUNT, \
   .commands = (uint8_t *)clcommands \
}

#define FAN_CONTROL_CLUSTER_ZCL_SERVER_CLUSTER_TYPE(clattributes, clcommands) \
{ \
   .id = FAN_CONTROL_CLUSTER_ID, \
   .options = { \
                .type = ZCL_SERVER_CLUSTER_TYPE, \
                .security = ZCL_NETWORK_KEY_CLUSTER_SECURITY, \
              }, \
   .attributesAmount = ZCL_FAN_CONTROL_CLUSTER_SERVER_ATTRIBUTES_AMOUNT, \
   .attributes = (uint8_t *)clattributes, \
   .commandsAmount = ZCL_FAN_CONTROL_CLUSTER_COMMANDS_AMOUNT, \
   .commands = (uint8_t *)clcommands \
}

#define DEFINE_FAN_CONTROL_CLUSTER(cltype, clattributes, clcommands) \
    FAN_CONTROL_CLUSTER_##cltype(clattributes, clcommands)

#define FAN_CONTROL_CLUSTER_ZCL_CLIENT_CLUSTER_TYPE_FLASH(clattributes, clcommands) \
{ \
   .id = FAN_CONTROL_CLUSTER_ID, \
   .options = { \
                .type = ZCL_CLIENT_CLUSTER_TYPE, \
                .security = ZCL_NETWORK_KEY_CLUSTER_SECURITY, \
              }, \
   .attributesAmount = ZCL_FAN_CONTROL_CLUSTER_CLIENT_ATTRIBTUES_AMOUNT, \
   .attributes = (uint8_t *)clattributes, \
   .commandsAmount = ZCL_FAN_CONTROL_CLUSTER_COMMANDS_AMOUNT, \
   .commands = (FLASH_PTR uint8_t *)clcommands \
}

#define FAN_CONTROL_CLUSTER_ZCL_SERVER_CLUSTER_TYPE_FLASH(clattributes, clcommands) \
{ \
   .id = FAN_CONTROL_CLUSTER_ID, \
   .options = { \
                .type = ZCL_SERVER_CLUSTER_TYPE, \
                .security = ZCL_NETWORK_KEY_CLUSTER_SECURITY, \
              }, \
   .attributesAmount = ZCL_FAN_CONTROL_CLUSTER_SERVER_ATTRIBUTES_AMOUNT, \
   .attributes = (uint8_t *)clattributes, \
   .commandsAmount = ZCL_FAN_CONTROL_CLUSTER_COMMANDS_AMOUNT, \
   .commands = (FLASH_PTR uint8_t *)clcommands \
}

#define DEFINE_FAN_CONTROL_CLUSTER_FLASH(cltype, clattributes, clcommands) \
    FAN_CONTROL_CLUSTER_##cltype##_FLASH(clattributes, clcommands)

/******************************************************************************
                    Types section
******************************************************************************/
typedef enum _ZCL_FanControlFanMode_t
{
  ZCL_FC_FAN_MODE_OFF = (0x00),
  ZCL_FC_FAN_MODE_LOW,
  ZCL_FC_FAN_MODE_MEDIUM,
  ZCL_FC_FAN_MODE_HIGH,
  ZCL_FC_FAN_MODE_ON,
  ZCL_FC_FAN_MODE_AUTO,
  ZCL_FC_FAN_MODE_SMART, 
} ZCL_FanControlFanMode_t;

typedef enum _ZCL_FanControlFanSequenceOperation_t
{
  ZCL_FC_FAN_SEQUENCE_OPERATION_LOW_MED_HIGH = (0x00),
  ZCL_FC_FAN_SEQUENCE_OPERATION_LOW_HIGH,
  ZCL_FC_FAN_SEQUENCE_OPERATION_LOW_MED_HIGH_AUTO,
  ZCL_FC_FAN_SEQUENCE_OPERATION_LOW_HIGH_AUTO,
  ZCL_FC_FAN_SEQUENCE_OPERATION_ON_AUTO
} ZCL_FanControlFanSequenceOperation_t;

BEGIN_PACK

/**
 * \brief Fan Control Cluster server's attributes
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
  } fanMode;

  struct PACK
  {
    ZCL_AttributeId_t id;
    uint8_t type;
    uint8_t properties;
    uint8_t value;
    uint8_t minVal;
    uint8_t maxVal;
  } fanModeSequence;

} ZCL_FanControlClusterServerAttributes_t;

END_PACK


#endif /* _ZCLFANCONTROLCLUSTER_H */

