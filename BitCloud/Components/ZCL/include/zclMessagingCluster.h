/************************************************************************//**
  \file zclMessageingCluster.h

  \brief
    The header file describes the ZCL Message Cluster

    The header file describes the ZCL Message Cluster

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    27.11.08 A. Potashov - Created.
******************************************************************************/

#ifndef _MESSAGINGCLUSTER_H
#define _MESSAGINGCLUSTER_H

#include <zcl.h>
#include <clusters.h>
/*******************************************************************************
                             Defines section
 ******************************************************************************/
/***************************************************************************//**
\brief Macros defines duration value of Display Message command,
 * which means 'until change'.
 * According to ZB AMI Spec r15, D.5.2.3.1.1.1, page 203.
*******************************************************************************/
#define DISPLAY_MESSAGE_DURATION_UNTIL_CHANGED 0xFFFF

/*************************************************************************//**
  \brief Message Cluster attributes amount
*****************************************************************************/
#define MESSAGE_CLUSTER_CLIENT_ATTRIBUTES_AMOUNT      0
#define MESSAGE_CLUSTER_SERVER_ATTRIBUTES_AMOUNT      0

/*************************************************************************//**
  \brief Message Cluster commands amount
*****************************************************************************/
#define MESSAGE_CLUSTER_COMMANDS_AMOUNT               4

#define DISPLAY_MESSAGE_COMMAND_ID  0x00
#define CANCEL_MESSAGE_COMMAND_ID   0x01
#define GET_LAST_MESSAGE_COMMAND_ID 0x00
#define CONFIRM_MESSAGE_COMMAND_ID  0x01

#define DEFINE_MESSAGING_CLUSTER_COMMANDS(clDisplayMessageInd, clCancelMessageInd, clGetLastMessageInd, clMessageConfirmationInd) \
  DEFINE_COMMAND(displayMessageCommand, 0x00, COMMAND_OPTIONS(SERVER_TO_CLIENT, ZCL_RESPONSE_CONTROLLED_BY_PAYLOAD, ZCL_COMMAND_ACK), clDisplayMessageInd), \
  DEFINE_COMMAND(cancelMessageCommand,  0x01, COMMAND_OPTIONS(SERVER_TO_CLIENT, ZCL_THERE_IS_NO_RELEVANT_RESPONSE, ZCL_COMMAND_ACK), clCancelMessageInd), \
  DEFINE_COMMAND(getLastMessageCommand,      0x00, COMMAND_OPTIONS(CLIENT_TO_SERVER, ZCL_THERE_IS_NO_RELEVANT_RESPONSE, ZCL_COMMAND_ACK), clGetLastMessageInd), \
  DEFINE_COMMAND(messageConfirmationCommand, 0x01, COMMAND_OPTIONS(CLIENT_TO_SERVER, ZCL_THERE_IS_NO_RELEVANT_RESPONSE, ZCL_COMMAND_ACK), clMessageConfirmationInd)

#define MESSAGING_CLUSTER_ZCL_CLIENT_CLUSTER_TYPE(clattributes, clcommands) \
  { \
    .id = MESSAGE_CLUSTER_ID, \
    .options = {.type = ZCL_CLIENT_CLUSTER_TYPE, .security = ZCL_APPLICATION_LINK_KEY_CLUSTER_SECURITY}, \
    .attributesAmount = MESSAGE_CLUSTER_CLIENT_ATTRIBUTES_AMOUNT, \
    .attributes = (uint8_t *)clattributes, \
    .commandsAmount = MESSAGE_CLUSTER_COMMANDS_AMOUNT, \
    .commands = (uint8_t *)clcommands \
  }

#define MESSAGING_CLUSTER_ZCL_SERVER_CLUSTER_TYPE(clattributes, clcommands) \
  { \
    .id = MESSAGE_CLUSTER_ID, \
    .options = {.type = ZCL_SERVER_CLUSTER_TYPE, .security = ZCL_APPLICATION_LINK_KEY_CLUSTER_SECURITY}, \
    .attributesAmount = MESSAGE_CLUSTER_SERVER_ATTRIBUTES_AMOUNT, \
    .attributes = (uint8_t *)clattributes, \
    .commandsAmount = MESSAGE_CLUSTER_COMMANDS_AMOUNT, \
    .commands = (uint8_t *)clcommands \
  }
#define DEFINE_MESSAGING_CLUSTER(cltype, clattributes, clcommands) MESSAGING_CLUSTER_##cltype(clattributes, clcommands)

#define MESSAGING_CLUSTER_ZCL_CLIENT_CLUSTER_TYPE_FLASH(clattributes, clcommands) \
  { \
    .id = MESSAGE_CLUSTER_ID, \
    .options = {.type = ZCL_CLIENT_CLUSTER_TYPE, .security = ZCL_APPLICATION_LINK_KEY_CLUSTER_SECURITY}, \
    .attributesAmount = MESSAGE_CLUSTER_CLIENT_ATTRIBUTES_AMOUNT, \
    .attributes = (uint8_t *)clattributes, \
    .commandsAmount = MESSAGE_CLUSTER_COMMANDS_AMOUNT, \
    .commands = (FLASH_PTR uint8_t *)clcommands \
  }

#define MESSAGING_CLUSTER_ZCL_SERVER_CLUSTER_TYPE_FLASH(clattributes, clcommands) \
  { \
    .id = MESSAGE_CLUSTER_ID, \
    .options = {.type = ZCL_SERVER_CLUSTER_TYPE, .security = ZCL_APPLICATION_LINK_KEY_CLUSTER_SECURITY}, \
    .attributesAmount = MESSAGE_CLUSTER_SERVER_ATTRIBUTES_AMOUNT, \
    .attributes = (uint8_t *)clattributes, \
    .commandsAmount = MESSAGE_CLUSTER_COMMANDS_AMOUNT, \
    .commands = (FLASH_PTR uint8_t *)clcommands \
  }
#define DEFINE_MESSAGING_CLUSTER_FLASH(cltype, clattributes, clcommands) MESSAGING_CLUSTER_##cltype##_FLASH(clattributes, clcommands)
BEGIN_PACK

typedef union PACK
{
  struct PACK
  {
    LITTLE_ENDIAN_OCTET(4,(
      uint8_t transmissionType : 2,
      uint8_t importanceLevel : 2,
      uint8_t reserved : 3,
      uint8_t messageConfirmation : 1
    ))
  };
  uint8_t u8;
} MessageControl_t;

/***************************************************************************//**
  \brief
    ZCL Messaging Cluster Display Message Command payload format
*******************************************************************************/
typedef struct PACK
{
  uint32_t            messageId;        //!<Message Id (Unsignet 32 bit Integer)
  MessageControl_t    messageControl;   //!<Message Control (8-bit Bitmap)
  uint32_t            startTime;        //!<Start Time (UTCTime)
  uint16_t            duration;         //!<Duration In Minutes (Unsigned 16-bit Integer)
  uint8_t             message[1];       //!<Message (Character string)
} DisplayMessage_t;

/***************************************************************************//**
  \brief
    ZCL Messaging Cluster Cancel Message Command payload format
*******************************************************************************/
typedef struct PACK
{
  uint32_t            messageId;        //!<Message Id (Unsignet 32 bit Integer)
  MessageControl_t    messageControl;   //!<Message Control (8-bit Bitmap)
} CancelMessage_t;

typedef struct PACK
{
  uint32_t      messageId;
  ZCL_UTCTime_t confirmationTime;
} MessageConfirmation_t;
END_PACK


/***************************************************************************//**
  \brief
    ZCL Messaging Cluster Commands
*******************************************************************************/
typedef struct
{
  //!Display Message Command descriptor
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
    ZCL_Status_t (*displayMessage)(ZCL_Addressing_t *addressing, uint8_t payloadLength, DisplayMessage_t *payload);
  } displayMessageCommand;

  //!Cancel Message Command descriptor
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
    ZCL_Status_t (*cancelMessage)(ZCL_Addressing_t *addressing, uint8_t payloadLength, CancelMessage_t *payload);
  } cancelMessageCommand;
  //!Get Last Message Command descriptor
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
    ZCL_Status_t (*getLastMessage)(ZCL_Addressing_t *addressing, uint8_t payloadLength, void *payload);
  } getLastMessageCommand;
  //!Message Confirmation Command descriptor
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
    ZCL_Status_t (*messageConfirmation)(ZCL_Addressing_t *addressing, uint8_t payloadLength, MessageConfirmation_t *payload);
  } messageConfirmationCommand;
} MessagingClusterCommands_t;

#endif // _MESSAGINGCLUSTER_H
