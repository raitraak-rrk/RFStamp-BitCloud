/***************************************************************************//**
  \file zclKeyEstablishmentCluster.h

  \brief
    The header file describes the ZCL Key Establishment Cluster and its interface

    The file describes the types and interface of the ZCL Key Establishment Cluster

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    10.03.09 A. Potashov - Created.
*******************************************************************************/

#ifndef _ZCLKEYESTABLISHMENTCLUSTER_H
#define _ZCLKEYESTABLISHMENTCLUSTER_H

/*!
Attributes and commands necessary for managing secure communication between
ZigBee devices.
*/

#if(defined _LINK_SECURITY_) && (!defined _LIGHT_LINK_PROFILE_)
#if CERTICOM_SUPPORT == 1

/*******************************************************************************
                   Includes section
*******************************************************************************/

#include <zcl.h>
#include <clusters.h>
#include <zclSecurityManager.h>

/*******************************************************************************
                   Define(s) section
*******************************************************************************/

#define ZCL_KE_INVALID_ENDPOINT             APS_ZDO_ENDPOINT
#define ZCL_KE_INVALID_SHORT_ADDRESS        0xffff
#define ZCL_KE_INVALID_EXT_ADDRESS          0xffffffffffffffffLL


//KE Cluster timeout settings
#ifndef ZCL_KE_EPHEMERAL_DATA_GENERATE_TIME
#define ZCL_KE_EPHEMERAL_DATA_GENERATE_TIME           0x0E
#endif

#ifndef ZCL_KE_CONFIRM_KEY_GENERATE_TIME
#define ZCL_KE_CONFIRM_KEY_GENERATE_TIME              0x0E
#endif

//!This timeout (seconds) will be sent in Terminate KE Command
#ifndef ZCL_KE_TERMINATE_KE_TIMEOUT
#define ZCL_KE_TERMINATE_KE_TIMEOUT                   50
#endif

//!This timeout (seconds) will be applied for commands receive wating with unspecified timeouts
#ifndef ZCL_KE_TIMEOUT
#define ZCL_KE_TIMEOUT                                50
#endif

#define ZCL_KE_INITIATE_RANDOM_SEQ_SIZE               16 // 16 bytes

/***************************************************************************//**
  \brief ZCL Key Establishment Cluster server side attributes amount
*******************************************************************************/
#define ZCL_KE_CLUSTER_SERVER_ATTRIBUTES_AMOUNT        1

/***************************************************************************//**
  \brief ZCL Key Establishment Cluster client side attributes amount
*******************************************************************************/
#define ZCL_KE_CLUSTER_CLIENT_ATTRIBUTES_AMOUNT        1

/***************************************************************************//**
  \brief ZCL Key Establishment Cluster commands amount
*******************************************************************************/
#define ZCL_KE_CLUSTER_COMMANDS_AMOUNT                 8

//!Key Establishment Suite Attribute
#define ZCL_KE_CLUSTER_SERVER_KEY_ESTABLISHMENT_SUITE_ATTRIBUTE_ID    CCPU_TO_LE16(0x0000)

//!Key Establishment Suite Attribute
#define ZCL_KE_CLUSTER_CLIENT_KEY_ESTABLISHMENT_SUITE_ATTRIBUTE_ID    CCPU_TO_LE16(0x0000)


#define ZCL_KE_INITIATE_KEY_ESTABLISHMENT_REQUEST_COMMAND_ID          0x00
#define ZCL_KE_EPHEMERAL_DATA_REQUEST_COMMAND_ID                      0x01
#define ZCL_KE_CONFIRM_KEY_DATA_REQUEST_COMMAND_ID                    0x02
#define ZCL_KE_TERMINATE_KEY_ESTABLISHMENT_COMMAND_ID                 0x03

#define ZCL_KE_INITIATE_KEY_ESTABLISHMENT_RESPONSE_COMMAND_ID         0x00
#define ZCL_KE_EPHEMERAL_DATA_RESPONSE_COMMAND_ID                     0x01
#define ZCL_KE_CONFIRM_KEY_DATA_RESPONSE_COMMAND_ID                   0x02


#define ZCL_KE_CBKE_ECMQV_KEY_ESTABLISHMENT_SUITE_ID                  0x0001


/*******************************************************************************
                   Types section
*******************************************************************************/

/***************************************************************************//**
  \brief ZCL Key Establishment Cluster server side attributes defining macros

  This macros should be used for ZCL Key Establishment Cluster server side attributes defining.

  \return None

  \internal
  //The typical usage is:
  //Key Establishment Cluster server side related attributes
  ZCL_EKClusterServerAttributes_t ekClusterServerAttributes = ZCL_DEFINE_EK_CLUSTER_SERVER_ATTRIBUTES();
*******************************************************************************/
#define ZCL_DEFINE_EK_CLUSTER_SERVER_ATTRIBUTES()   \
  {DEFINE_ATTRIBUTE(keyEstablishmentSuite, ZCL_READONLY_ATTRIBUTE, ZCL_KE_CLUSTER_SERVER_KEY_ESTABLISHMENT_SUITE_ATTRIBUTE_ID, ZCL_16BIT_ENUM_DATA_TYPE_ID)}


/***************************************************************************//**
  \brief ZCL Key Establishment Cluster client side attributes defining macros

  This macros should be used for ZCL Key Establishment Cluster client side attributes defining.

  \return None

  \internal
  //The typical usage is:
  //Key Establishment Cluster client side related attributes
  ZCL_EKClusterClientAttributes_t ekClusterClientAttributes = ZCL_DEFINE_EK_CLUSTER_CLIENT_ATTRIBUTES();
*******************************************************************************/
#define ZCL_DEFINE_EK_CLUSTER_CLIENT_ATTRIBUTES()   \
  {DEFINE_ATTRIBUTE(keyEstablishmentSuite, ZCL_READONLY_ATTRIBUTE, ZCL_KE_CLUSTER_CLIENT_KEY_ESTABLISHMENT_SUITE_ATTRIBUTE_ID,  ZCL_16BIT_ENUM_DATA_TYPE_ID)}

#define DEFINE_INITIATE_KEY_ESTABLISHMENT_REQUEST(initiateKeyEstablishmentReqInd) \
        DEFINE_COMMAND(initiateKeyEstablishmentReq, ZCL_KE_INITIATE_KEY_ESTABLISHMENT_REQUEST_COMMAND_ID, \
        COMMAND_OPTIONS(CLIENT_TO_SERVER, ZCL_THERE_IS_RELEVANT_RESPONSE, ZCL_COMMAND_ACK), \
        initiateKeyEstablishmentReqInd)

#define DEFINE_INITIATE_KEY_ESTABLISHMENT_RESPONSE(initiateKeyEstablishmentRespInd) \
        DEFINE_COMMAND(initiateKeyEstablishmentResp, ZCL_KE_INITIATE_KEY_ESTABLISHMENT_RESPONSE_COMMAND_ID, \
        COMMAND_OPTIONS(SERVER_TO_CLIENT, ZCL_THERE_IS_NO_RELEVANT_RESPONSE, ZCL_COMMAND_ACK), \
        initiateKeyEstablishmentRespInd)

#define DEFINE_EPHEMERAL_DATA_REQUEST(ephemeralDataReqInd) \
        DEFINE_COMMAND(ephemeralDataReq, ZCL_KE_EPHEMERAL_DATA_REQUEST_COMMAND_ID, \
        COMMAND_OPTIONS(CLIENT_TO_SERVER, ZCL_THERE_IS_RELEVANT_RESPONSE, ZCL_COMMAND_ACK), \
        ephemeralDataReqInd)

#define DEFINE_EPHEMERAL_DATA_RESPONSE(ephemeralDataRespInd) \
        DEFINE_COMMAND(ephemeralDataResp, ZCL_KE_EPHEMERAL_DATA_RESPONSE_COMMAND_ID, \
        COMMAND_OPTIONS(SERVER_TO_CLIENT, ZCL_THERE_IS_NO_RELEVANT_RESPONSE, ZCL_COMMAND_ACK), \
        ephemeralDataRespInd)

#define DEFINE_CONFIRM_KEY_DATA_REQUEST(confirmKeyDataReqInd) \
        DEFINE_COMMAND(confirmKeyDataReq, ZCL_KE_CONFIRM_KEY_DATA_REQUEST_COMMAND_ID, \
        COMMAND_OPTIONS(CLIENT_TO_SERVER, ZCL_THERE_IS_RELEVANT_RESPONSE, ZCL_COMMAND_ACK), \
        confirmKeyDataReqInd)

#define DEFINE_CONFIRM_KEY_DATA_RESPONSE(confirmKeyDataRespInd) \
        DEFINE_COMMAND(confirmKeyDataResp, ZCL_KE_CONFIRM_KEY_DATA_RESPONSE_COMMAND_ID, \
        COMMAND_OPTIONS(SERVER_TO_CLIENT, ZCL_THERE_IS_NO_RELEVANT_RESPONSE, ZCL_COMMAND_ACK), \
        confirmKeyDataRespInd)

#define DEFINE_TERMINATE_KEY_ESTABLISHMENT_SERVER_PART(terminateKeyEstablishmentReqIndServerPart) \
        DEFINE_COMMAND(terminateKeyEstablishmentReqServerPart, ZCL_KE_TERMINATE_KEY_ESTABLISHMENT_COMMAND_ID, \
        COMMAND_OPTIONS(CLIENT_TO_SERVER, ZCL_THERE_IS_NO_RELEVANT_RESPONSE, ZCL_COMMAND_ACK), \
        terminateKeyEstablishmentReqIndServerPart)

#define DEFINE_TERMINATE_KEY_ESTABLISHMENT_CLIENT_PART(terminateKeyEstablishmentReqIndClientPart) \
        DEFINE_COMMAND(terminateKeyEstablishmentReqClientPart, ZCL_KE_TERMINATE_KEY_ESTABLISHMENT_COMMAND_ID, \
        COMMAND_OPTIONS(SERVER_TO_CLIENT, ZCL_THERE_IS_NO_RELEVANT_RESPONSE, ZCL_COMMAND_ACK), \
        terminateKeyEstablishmentReqIndClientPart)

#define KE_CLUSTER_COMMANDS(initiateKeyEstablishmentReqInd, initiateKeyEstablishmentRespInd,                        \
                            ephemeralDataReqInd, ephemeralDataRespInd, confirmKeyDataReqInd, confirmKeyDataRespInd, \
                            terminateKeyEstablishmentReqIndServerPart, terminateKeyEstablishmentReqIndClientPart)   \
        DEFINE_INITIATE_KEY_ESTABLISHMENT_REQUEST(initiateKeyEstablishmentReqInd),                                  \
        DEFINE_INITIATE_KEY_ESTABLISHMENT_RESPONSE(initiateKeyEstablishmentRespInd),                                \
        DEFINE_EPHEMERAL_DATA_REQUEST(ephemeralDataReqInd),                                                         \
        DEFINE_EPHEMERAL_DATA_RESPONSE(ephemeralDataRespInd),                                                       \
        DEFINE_CONFIRM_KEY_DATA_REQUEST(confirmKeyDataReqInd),                                                      \
        DEFINE_CONFIRM_KEY_DATA_RESPONSE(confirmKeyDataRespInd),                                                    \
        DEFINE_TERMINATE_KEY_ESTABLISHMENT_SERVER_PART(terminateKeyEstablishmentReqIndServerPart),                  \
        DEFINE_TERMINATE_KEY_ESTABLISHMENT_CLIENT_PART(terminateKeyEstablishmentReqIndClientPart)                   \

/***************************************************************************//**
  \brief ZCL Key Establishment Cluster server side defining macros

  This macros should be used with #ZCL_DEFINE_EK_CLUSTER_SERVER_ATTRIBUTES and
  #ZCL_DEFINE_EK_CLUSTER_SERVER_COMMANDS for ZCL Key Establishment Cluster server
  side defining.

  \param attributes - pointer to cluster server attributes (ZCL_KEClusterServerAttributes_t)
  \param commands - pointer to cluster server commands (ZCL_KEServerCommands_t)

  \return None

  \internal
  //The typical code is:
  //Key Establishment Cluster server side related attributes and commands
  ZCL_KEClusterServerAttributes_t keClusterServerAttributes = ZCL_DEFINE_EK_CLUSTER_SERVER_ATTRIBUTES();
  ZCL_KEClusterServerCommands_t keClusterServerCommands = ZCL_DEFINE_EK_CLUSTER_SERVER_COMMANDS(handler1, handler2, handler3, handlerv);
  ZCL_Cluster_t ekClusterServer = ZCL_DEFINE_KE_CLUSTER_SERVER(&keClusterServerAttributes, &keClusterServerCommands);
*******************************************************************************/
#define ZCL_DEFINE_KE_CLUSTER_SERVER(clattributes, clcommands)                \
  {                                                                           \
    .id = ZCL_KEY_ESTABLISHMENT_CLUSTER_ID,                                   \
    .options =                                                                \
    {                                                                         \
      .type = ZCL_SERVER_CLUSTER_TYPE,                                        \
      .security = ZCL_NETWORK_KEY_CLUSTER_SECURITY,                           \
      .reserved = 0,                                                          \
    },                                                                        \
    .attributesAmount = ZCL_KE_CLUSTER_SERVER_ATTRIBUTES_AMOUNT,              \
    .attributes = (uint8_t *) clattributes,                                   \
    .commandsAmount = ZCL_KE_CLUSTER_COMMANDS_AMOUNT,                         \
    .commands = (uint8_t *) clcommands                                        \
  }

#define ZCL_DEFINE_KE_CLUSTER_SERVER_FLASH(clattributes, clcommands)          \
  {                                                                           \
    .id = ZCL_KEY_ESTABLISHMENT_CLUSTER_ID,                                   \
    .options =                                                                \
    {                                                                         \
      .type = ZCL_SERVER_CLUSTER_TYPE,                                        \
      .security = ZCL_NETWORK_KEY_CLUSTER_SECURITY,                           \
      .reserved = 0,                                                          \
    },                                                                        \
    .attributesAmount = ZCL_KE_CLUSTER_SERVER_ATTRIBUTES_AMOUNT,              \
    .attributes = (uint8_t *) clattributes,                                   \
    .commandsAmount = ZCL_KE_CLUSTER_COMMANDS_AMOUNT,                         \
    .commands = (FLASH_PTR uint8_t *) clcommands                              \
  }

/***************************************************************************//**
  \brief ZCL Key Establishment Cluster client side defining macros

  This macros should be used with #ZCL_DEFINE_EK_CLUSTER_CLIENT_ATTRIBUTES and
  #ZCL_DEFINE_EK_CLUSTER_CLIENT_COMMANDS for ZCL Key Establishment Cluster client
  side defining.

  \param attributes - pointer to cluster client attributes (ZCL_KEClusterClientAttributes_t)
  \param commands - pointer to cluster client commands (ZCL_KEClientCommands_t)

  \return None

  \internal
  //The typical code is:
  //Key Establishment Cluster client side related attributes and commands
  ZCL_KEClusterClientAttributes_t keClusterClientAttributes = ZCL_DEFINE_EK_CLUSTER_CLIENT_ATTRIBUTES();
  ZCL_KEClusterClientCommands_t keClusterClientCommands = ZCL_DEFINE_EK_CLUSTER_CLIENT_COMMANDS(handler1, handler2, handler3, handlerv);
  ZCL_Cluster_t ekClusterClient = ZCL_DEFINE_KE_CLUSTER_Client(&keClusterClientAttributes, &keClusterClientCommands);
*******************************************************************************/
#define ZCL_DEFINE_KE_CLUSTER_CLIENT(clattributes, clcommands)                \
  {                                                                           \
    .id = ZCL_KEY_ESTABLISHMENT_CLUSTER_ID,                                   \
    .options =                                                                \
    {                                                                         \
      .type = ZCL_CLIENT_CLUSTER_TYPE,                                        \
      .security = ZCL_NETWORK_KEY_CLUSTER_SECURITY,                           \
      .reserved = 0,                                                          \
    },                                                                        \
    .attributesAmount = ZCL_KE_CLUSTER_SERVER_ATTRIBUTES_AMOUNT,              \
    .attributes = (uint8_t *) clattributes,                                   \
    .commandsAmount = ZCL_KE_CLUSTER_COMMANDS_AMOUNT,                         \
    .commands = (uint8_t *) clcommands                                        \
  }

#define ZCL_DEFINE_KE_CLUSTER_CLIENT_FLASH(clattributes, clcommands)          \
  {                                                                           \
    .id = ZCL_KEY_ESTABLISHMENT_CLUSTER_ID,                                   \
    .options =                                                                \
    {                                                                         \
      .type = ZCL_CLIENT_CLUSTER_TYPE,                                        \
      .security = ZCL_NETWORK_KEY_CLUSTER_SECURITY,                           \
      .reserved = 0,                                                          \
    },                                                                        \
    .attributesAmount = ZCL_KE_CLUSTER_SERVER_ATTRIBUTES_AMOUNT,              \
    .attributes = (uint8_t *) clattributes,                                   \
    .commandsAmount = ZCL_KE_CLUSTER_COMMANDS_AMOUNT,                         \
    .commands = (FLASH_PTR uint8_t *) clcommands                              \
  }

BEGIN_PACK
/***************************************************************************//**
  \brief
    ZCL Key Establishment Cluster server side attributes
*******************************************************************************/
typedef struct PACK
{
  //!Information attribute set (Id == 0x00)

  /*!KeyEstablishmentSuite attribute (Read only, M)
  The KeyEstablishmentSuite attribute is 16-bits in length and specifies all the
  cryptographic schemes for key establishment on the device. A device shall set the
  corresponding bit to 1 for every cryptographic scheme that is supports. All other
  cryptographic schemes and reserved bits shall be set to 0.

  */
  struct PACK
  {
    ZCL_AttributeId_t   id;           //!<Attribute Id (0x0000, #ZCL_KE_CLUSTER_SERVER_KEY_ESTABLISHMENT_SUITE_ATTRIBUTE_ID)
    uint8_t             type;         //!<Attribute type (16-bit enumeration, #ZZCL_16BIT_ENUM_DATA_TYPE_ID)
    uint8_t             properties;   //!<Attribute properties bitmask
    uint16_t            value;        //!<Attribute value (default 0x0000)
  } keyEstablishmentSuite;
} ZCL_KEClusterServerAttributes_t;
END_PACK

BEGIN_PACK
/***************************************************************************//**
  \brief
    ZCL Key Establishment Cluster client side attributes
*******************************************************************************/
typedef struct PACK
{
  //!Information attribute set (Id == 0x00)

  /*!KeyEstablishmentSuite attribute (Read only, M)
  The KeyEstablishmentSuite attribute is 16-bits in length and specifies all the
  cryptographic schemes for key establishment on the device. A device shall set the
  corresponding bit to 1 for every cryptographic scheme that is supports. All other
  cryptographic schemes and reserved bits shall be set to 0.

  */
  struct PACK
  {
    ZCL_AttributeId_t   id;           //!<Attribute Id (0x0000, #ZCL_KE_CLUSTER_CLIENT_KEY_ESTABLISHMENT_SUITE_ATTRIBUTE_ID)
    uint8_t             type;         //!<Attribute type (16-bit enumeration, #ZZCL_16BIT_ENUM_DATA_TYPE_ID)
    uint8_t             properties;   //!<Attribute properties bitmask
    uint16_t            value;        //!<Attribute value (default 0x0000)
  } keyEstablishmentSuite;
} ZCL_KEClusterClientAttributes_t;
END_PACK


BEGIN_PACK

/***************************************************************************//**
  \brief
    ZCL Initiate Key Establishment command
*******************************************************************************/
typedef struct PACK
{
  /*!This will be type of KeyEstablishment that the initiator is requesting for
  the Key Establishment Cluster. For CBKE-ECMQV this will be 0x0001.
  */
  uint16_t              keyEstablishmentSuite;
  /*!This value indicates approximately how long the initiator device will take
  in seconds to generate the Ephemeral Data Request command. The valid range is
  0x00 to 0xFE.
  */
  uint8_t               ephemeralDataGenerateTime;
  /*!This value indicates approximately how long the initiator device will take
  in seconds to generate the Confirm Key Request command. The valid range is
  0x00 to 0xFE.
  */
  uint8_t               confirmKeyGenerateTime;
  /*!For keyEstablishmentSuite = 0x0001 (CBKE), the identify field shall be the
  block of octets containing the implicit certificate CERTU.
  */
  uint8_t               identify[48];
} ZCL_InitiateKeyEstablishmentCommand_t;


/***************************************************************************//**
  \brief
    ZCL Confirm Key Request and ZCL Confirm Key Response commands
*******************************************************************************/
typedef struct PACK
{
  /*!Secure Message Authentication Code field which shall be the octet-string
  representation of MACU or MACV.
  */
  uint8_t               mac[16];
} ZCL_ConfirmKeyCommand_t;


typedef enum
{
  //0x00 - Reserved

  /*!The Issuer field whithin the key establishment partner's certificate is
  unknown to the sending device, and it has terminated the key establishment.
  */
  ZCL_TKE_UNKNOWN_ISSUER_STATUS     = 0x01,
  /*!The device could not confirm that is shares the same key with the
  corresponding device and has terminated the key establishment.
  */
  ZCL_TKE_BAD_KEY_CONFIRM_STATUS    = 0x02,
  /*!The device received a bad messagefrom the corresponding device (e.g. message
  whith bad data, an out of sequence number, or a message with a bad format)
  and has terminated the key establishment.
  */
  ZCL_TKE_BAD_MESSAGE_STATUS        = 0x03,
  /*!The device does not currently have the internal resources necessary to
  perform key establishment and has terminated the exchange.
  */
  ZCL_TKE_NO_RESOURCES_STATUS       = 0x04,
  /*!The device does not support the specified key establishment suite in the
  partner's Initiate Key Establishment message.
  */
  ZCL_TKE_UNSUPPORTED_SUITE_STATUS  = 0x05,

  /*For internal use only*/
  ZCL_TKE_NO_STATUS                 = 0xff
  //0x06 - 0xFF - Reserved
} ZCL_TKEStatus_t;


/***************************************************************************//**
  \brief
    ZCL Terminate Key Establishment command
*******************************************************************************/
typedef struct PACK
{
  /*!
  Status code indicates the key establishment termination reason.
  #ZCL_TEKStatus_t values should be used.
  */
  uint8_t               statusCode;
  /*!This value indicates the minimum amount of time in seconds the initiator
  device should wait befor trying to initiate key establishment again. The valid
  range is 0x00 to 0xFE.
  */
  uint8_t               waitTime;
  /*!This value will be set the value of the KeyEstablishmentSuite attribute.
  It indicates the list of key exchange methods that the device supports.
  */
  uint16_t              keyEstablishmentSuite;
} ZCL_TerminateKeyEstablishmentCommand_t;


/***************************************************************************//**
  \brief
    ZCL Ephemeral Data Request and ZCL Ephemeral Data Request commands

    The Ephemeral Data Request command allows a device to communicate its
    ephemeral data to another device and request that the device send back
    its own ephemeral data.

    The Ephemeral Data Response command allows a device to communicate its
    ephemeral data to another device and request that the device send back
    its own ephemeral data.
*******************************************************************************/
typedef struct PACK
{
  /*!
  Ephemeral Data (QEU, QEV)
  */
  uint8_t               data[22];
} ZCL_EphemeralDataCommand_t;

END_PACK

/***************************************************************************//**
  \brief
    ZCL Key Establishment Cluster Commands
*******************************************************************************/
typedef struct
{
  //!Initiate Key Establishment Request Command descriptor
  struct
  {
    ZCL_CommandId_t       id;
    ZclCommandOptions_t   options;
    ZCL_Status_t (*indHandler)(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_InitiateKeyEstablishmentCommand_t *payload);
  } initiateKeyEstablishmentReq;

  //!Initiate Key Establishment Response Command descriptor
  struct
  {
    ZCL_CommandId_t       id;
    ZclCommandOptions_t   options;
    ZCL_Status_t (*indHandler)(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_InitiateKeyEstablishmentCommand_t *payload);
  } initiateKeyEstablishmentResp;

  //!Ephemeral Data Request Command descriptor
  struct
  {
    ZCL_CommandId_t       id;
    ZclCommandOptions_t   options;
    ZCL_Status_t (*indHandler)(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_EphemeralDataCommand_t *payload);
  } ephemeralDataReq;

  //!Ephemeral Data Response Command descriptor
  struct
  {
    ZCL_CommandId_t       id;
    ZclCommandOptions_t   options;  //!<Command options
    ZCL_Status_t (*indHandler)(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_EphemeralDataCommand_t *payload);
  } ephemeralDataResp;

  //!Confirm Key Data Request Command descriptor
  struct
  {
    ZCL_CommandId_t       id;
    ZclCommandOptions_t   options;
    ZCL_Status_t (*indHandler)(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_ConfirmKeyCommand_t *payload);
  } confirmKeyDataReq;

  //!Confirm Key Data Response Command descriptor
  struct
  {
    ZCL_CommandId_t       id;
    ZclCommandOptions_t   options;
    ZCL_Status_t (*indHandler)(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_ConfirmKeyCommand_t *payload);
  } confirmKeyDataResp;

  //!Terminate Key Establishment Command descriptor (server part)
  struct
  {
    ZCL_CommandId_t       id;
    ZclCommandOptions_t   options;
    ZCL_Status_t (*indHandler)(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_TerminateKeyEstablishmentCommand_t *payload);
  } terminateKeyEstablishmentReqServerPart;

  //!Terminate Key Establishment Command descriptor (client part)
  struct
  {
    ZCL_CommandId_t       id;
    ZclCommandOptions_t   options;
    ZCL_Status_t (*indHandler)(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_TerminateKeyEstablishmentCommand_t *payload);
  } terminateKeyEstablishmentReqClientPart;
} ZCL_KEClusterCommands_t;

/***************************************************************************//**
  \brief Finds KE Certificate descriptor for the device with particular address

  \param subject - device's address which certificate is looking for
  \return pointer to KE Certificate descriptor if it was found, NULL otherwise
*******************************************************************************/
ZclCertificate_t *keGetCertificate(ExtAddr_t subject);

/*************************************************************************************//**
\brief Return server or client cluster descriptor for external use.

Use this function if you want register KE cluster on endpoint.

\param[in] server - If true returns server descriptor otherwise client descriptor.
******************************************************************************************/
ZCL_Cluster_t* keGetClusterDescriptor(uint8_t server);

#endif // CERTICOM_SUPPORT == 1
#endif // (defined _LINK_SECURITY_) && (!defined _LIGHT_LINK_PROFILE_)
#endif //#ifndef _ZCLKEYESTABLISHMENTCLUSTER_H

//eof zclKeyEstablishmentCluster.h
