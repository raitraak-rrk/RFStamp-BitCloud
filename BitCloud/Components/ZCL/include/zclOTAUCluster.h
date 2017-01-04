/**************************************************************************//**
  \file zclOTAUCluster.h

  \brief The header file describes the OTAU cluster interface.
  ZigBee Document 095264r17.
  Revision 17 Version 0.7 March 14, 2010

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    15.01.10 A. Khromykh - Created.
  Last change:
    $Id: zclOTAUCluster.h 27584 2015-01-09 14:45:42Z unithra.c $
*******************************************************************************/
#ifndef _ZCLOTAUCLUSTER_H
#define _ZCLOTAUCLUSTER_H

/******************************************************************************
                        Includes section
******************************************************************************/
#include <zclParser.h>
#include <zcl.h>
#include <clusters.h>
#include <ofdExtMemory.h>

/******************************************************************************
                        Defines section
******************************************************************************/
//#define _OTAU_DEBUG_

/***************************************************************************//**
  \brief ZCL OTAU Cluster attributes
*******************************************************************************/
#define ZCL_OTAU_CLUSTER_SERVER_ATTRIBUTES_AMOUNT      0
#define ZCL_OTAU_CLUSTER_CLIENT_ATTRIBUTES_AMOUNT      3

/***************************************************************************//**
  \brief ZCL OTAU Cluster commands amount
*******************************************************************************/
#define ZCL_OTAU_CLUSTER_COMMANDS_AMOUNT               8

//!ZCL OTAU Cluster client side attributes id
#define ZCL_OTAU_CLUSTER_CLIENT_UPGRADE_SERVER_ID                  0x0000
#define ZCL_OTAU_CLUSTER_CLIENT_IMAGE_UPGRADE_STATUS_ID            0x0006
#define ZCL_OTAU_CLUSTER_CLIENT_MINIMUM_BLOCK_REQUEST_DELAY_ID     0x0009

#define DEFINE_OTAU_CLIENT_ATTRIBUTES \
        DEFINE_ATTRIBUTE(upgradeServerIEEEAddr, ZCL_READONLY_ATTRIBUTE, ZCL_OTAU_CLUSTER_CLIENT_UPGRADE_SERVER_ID, ZCL_IEEE_ADDRESS_DATA_TYPE_ID), \
        DEFINE_ATTRIBUTE(imageUpgradeStatus, ZCL_READONLY_ATTRIBUTE, ZCL_OTAU_CLUSTER_CLIENT_IMAGE_UPGRADE_STATUS_ID, ZCL_8BIT_ENUM_DATA_TYPE_ID), \
        DEFINE_ATTRIBUTE_WITH_BOUNDARY(minimumBlockRequestDelay, ZCL_READONLY_ATTRIBUTE, ZCL_OTAU_CLUSTER_CLIENT_MINIMUM_BLOCK_REQUEST_DELAY_ID, ZCL_U16BIT_DATA_TYPE_ID, 0x0000, 0x0258)

#define OTAU_HEADER_STRING_SIZE                                      32
#define MINIMUM_BLOCK_DELAY_REQUEST_DELAY                            0x0258

// OTAU commands
// list of commands id
#define IMAGE_NOTIFY_ID                             0x00
#define QUERY_NEXT_IMAGE_REQUEST_ID                 0x01
#define QUERY_NEXT_IMAGE_RESPONSE_ID                0x02
#define IMAGE_BLOCK_REQUEST_ID                      0x03
#define IMAGE_PAGE_REQUEST_ID                       0x04
#define IMAGE_BLOCK_RESPONSE_ID                     0x05
#define UPGRADE_END_REQUEST_ID                      0x06
#define UPGRADE_END_RESPONSE_ID                     0x07

#define OTAU_IMAGE_NOTIFY_MAX_QUERY_JITTER_VALUE    100

#define DEFINE_IMAGE_NOTIFY(imageNotifyInd) \
        DEFINE_COMMAND(imageNotify, IMAGE_NOTIFY_ID, \
        COMMAND_OPTIONS(SERVER_TO_CLIENT, ZCL_THERE_IS_NO_RELEVANT_RESPONSE, ZCL_COMMAND_ACK), \
        imageNotifyInd)

#define DEFINE_QUERY_NEXT_IMAGE_REQUEST(queryNextImageReqInd) \
        DEFINE_COMMAND(queryNextImageReq, QUERY_NEXT_IMAGE_REQUEST_ID, \
        COMMAND_OPTIONS(CLIENT_TO_SERVER, ZCL_THERE_IS_RELEVANT_RESPONSE, ZCL_COMMAND_ACK), \
        queryNextImageReqInd)

#define DEFINE_QUERY_NEXT_IMAGE_RESPONSE(queryNextImageRespInd) \
        DEFINE_COMMAND(queryNextImageResp, QUERY_NEXT_IMAGE_RESPONSE_ID, \
        COMMAND_OPTIONS(SERVER_TO_CLIENT, ZCL_THERE_IS_NO_RELEVANT_RESPONSE, ZCL_COMMAND_ACK), \
        queryNextImageRespInd)

#define DEFINE_IMAGE_BLOCK_REQUEST(imageBlockReqInd)  \
        DEFINE_COMMAND(imageBlockReq, IMAGE_BLOCK_REQUEST_ID, \
        COMMAND_OPTIONS(CLIENT_TO_SERVER, ZCL_THERE_IS_RELEVANT_RESPONSE, ZCL_COMMAND_ACK), \
        imageBlockReqInd)

#define DEFINE_IMAGE_PAGE_REQUEST(imagePageReqInd)  \
        DEFINE_COMMAND(imagePageReq, IMAGE_PAGE_REQUEST_ID, \
        COMMAND_OPTIONS(CLIENT_TO_SERVER, ZCL_THERE_IS_RELEVANT_RESPONSE, ZCL_COMMAND_ACK), \
        imagePageReqInd)

#define DEFINE_IMAGE_BLOCK_RESPONSE(imageBlockRespInd) \
        DEFINE_COMMAND(imageBlockResp, IMAGE_BLOCK_RESPONSE_ID, \
        COMMAND_OPTIONS(SERVER_TO_CLIENT, ZCL_THERE_IS_NO_RELEVANT_RESPONSE, ZCL_COMMAND_ACK), \
        imageBlockRespInd)

#define DEFINE_UPGRADE_END_REQUEST(upgradeEndReqInd) \
        DEFINE_COMMAND(upgradeEndReq, UPGRADE_END_REQUEST_ID, \
        COMMAND_OPTIONS(CLIENT_TO_SERVER, ZCL_THERE_IS_RELEVANT_RESPONSE, ZCL_COMMAND_ACK), \
        upgradeEndReqInd)

#define DEFINE_UPGRADE_END_RESPONSE(upgradeEndRespInd) \
        DEFINE_COMMAND(upgradeEndResp, UPGRADE_END_RESPONSE_ID, \
        COMMAND_OPTIONS(SERVER_TO_CLIENT, ZCL_THERE_IS_NO_RELEVANT_RESPONSE, ZCL_COMMAND_ACK), \
        upgradeEndRespInd)

#define OTAU_COMMANDS(imageNotifyInd, queryNextImageReqInd, queryNextImageRespInd, imageBlockReqInd,     \
        imagePageReqInd, imageBlockRespInd, upgradeEndReqInd, upgradeEndRespInd)                         \
        DEFINE_IMAGE_NOTIFY(imageNotifyInd),                                                             \
        DEFINE_QUERY_NEXT_IMAGE_REQUEST(queryNextImageReqInd),                                           \
        DEFINE_QUERY_NEXT_IMAGE_RESPONSE(queryNextImageRespInd),                                         \
        DEFINE_IMAGE_BLOCK_REQUEST(imageBlockReqInd),                                                    \
        DEFINE_IMAGE_PAGE_REQUEST(imagePageReqInd),                                                      \
        DEFINE_IMAGE_BLOCK_RESPONSE(imageBlockRespInd),                                                  \
        DEFINE_UPGRADE_END_REQUEST(upgradeEndReqInd),                                                    \
        DEFINE_UPGRADE_END_RESPONSE(upgradeEndRespInd)

/***************************************************************************//**
  \brief ZCL OTAU Cluster server side defining macros

  \param attributes - pointer to cluster server attributes (ZCL_OtauClusterClientAttributes_t)
         or NULL if there are no attributes
  \param clcommands - pointer to cluster server commands
         or NULL if there are no commands

  \return None
*******************************************************************************/
#define OTAU_CLUSTER_ZCL_SERVER_CLUSTER_TYPE(clattributes, clcommands)        \
  {                                                                           \
    .id = OTAU_CLUSTER_ID,                                                    \
    .options = {                                                              \
      .type = ZCL_SERVER_CLUSTER_TYPE,                                        \
      .security = ZCL_NETWORK_KEY_CLUSTER_SECURITY,                           \
      .reserved = 0},                                                         \
    .attributesAmount = ZCL_OTAU_CLUSTER_SERVER_ATTRIBUTES_AMOUNT,            \
    .attributes = (uint8_t *) clattributes,                                   \
    .commandsAmount = ZCL_OTAU_CLUSTER_COMMANDS_AMOUNT,                       \
    .commands = (uint8_t *) clcommands                                        \
  }

#define OTAU_CLUSTER_ZCL_SERVER_CLUSTER_TYPE_FLASH(clattributes, clcommands)  \
  {                                                                           \
    .id = OTAU_CLUSTER_ID,                                                    \
    .options = {                                                              \
      .type = ZCL_SERVER_CLUSTER_TYPE,                                        \
      .security = ZCL_NETWORK_KEY_CLUSTER_SECURITY,                           \
      .reserved = 0},                                                         \
    .attributesAmount = ZCL_OTAU_CLUSTER_SERVER_ATTRIBUTES_AMOUNT,            \
    .attributes = (uint8_t *) clattributes,                                   \
    .commandsAmount = ZCL_OTAU_CLUSTER_COMMANDS_AMOUNT,                       \
    .commands = (FLASH_PTR uint8_t *) clcommands                              \
  }

/***************************************************************************//**
  \brief ZCL OTAU Cluster client side defining macros

  \param attributes - pointer to cluster client attributes (ZCL_OtauClusterClientAttributes_t)
          or NULL if there are no attributes
  \param clcommands - pointer to cluster client commands
          or NULL if there are no commands

  \return None
*******************************************************************************/
#define OTAU_CLUSTER_ZCL_CLIENT_CLUSTER_TYPE(clattributes, clcommands) \
  {                                                                    \
    .id = OTAU_CLUSTER_ID,                                             \
    .options = {                                                       \
      .type = ZCL_CLIENT_CLUSTER_TYPE,                                 \
      .security = ZCL_NETWORK_KEY_CLUSTER_SECURITY,                    \
      .reserved = 0},                                                  \
    .attributesAmount = ZCL_OTAU_CLUSTER_CLIENT_ATTRIBUTES_AMOUNT,     \
    .attributes = (uint8_t *) clattributes,                            \
    .commandsAmount = ZCL_OTAU_CLUSTER_COMMANDS_AMOUNT,                \
    .commands = (uint8_t *) clcommands                                 \
  }

#define OTAU_CLUSTER_ZCL_CLIENT_CLUSTER_TYPE_FLASH(clattributes, clcommands) \
  {                                                                          \
    .id = OTAU_CLUSTER_ID,                                                   \
    .options = {                                                             \
      .type = ZCL_CLIENT_CLUSTER_TYPE,                                       \
      .security = ZCL_NETWORK_KEY_CLUSTER_SECURITY,                          \
      .reserved = 0},                                                        \
    .attributesAmount = ZCL_OTAU_CLUSTER_CLIENT_ATTRIBUTES_AMOUNT,           \
    .attributes = (uint8_t *) clattributes,                                  \
    .commandsAmount = ZCL_OTAU_CLUSTER_COMMANDS_AMOUNT,                      \
    .commands = (FLASH_PTR uint8_t *) clcommands                             \
  }

#define DEFINE_OTAU_CLUSTER(cltype, clattributes, clcommands) OTAU_CLUSTER_##cltype(clattributes, clcommands)
#define DEFINE_OTAU_CLUSTER_FLASH(cltype, clattributes, clcommands) OTAU_CLUSTER_##cltype##_FLASH(clattributes, clcommands)

#define ZCL_IMAGE_BLOCK_RESP_HEADER_SIZE \
            ( sizeof(ZCL_Status_t) + sizeof(uint16_t) + sizeof(ZCL_OtauImageType_t) \
              + sizeof(ZCL_OtauFirmwareVersion_t) + sizeof(uint32_t) + sizeof(uint8_t) )

/*******************************************************************************
                   Types section
*******************************************************************************/
typedef uint8_t OtauHeaderString_t[OTAU_HEADER_STRING_SIZE];

typedef enum PACK
{
  OTAU_SERVER_DISCOVERY_INITIATED,
  OTAU_SERVER_HAS_BEEN_FOUND,
  OTAU_SERVER_HAS_NOT_BEEN_FOUND,
  OTAU_SERVER_HAS_BEEN_LOST,
  OTAU_OFD_DRIVER_ERROR,
  OTAU_SERVER_RECEIVED_MALFORMED_COMMAND,
  OTAU_SERVER_NO_IMAGE_AVAILABLE,
  OTAU_SERVER_RECEIVED_UNSUPPORTED_COMMAND,
  OTAU_SERVER_RECEIVED_UNKNOWN_STATUS,
  OTAU_SERVER_IS_NOT_AUTHORIZED_TO_UPDATE,
  OTAU_DOWNLOAD_STARTED,
  OTAU_DOWNLOAD_ABORTED,
  OTAU_QUERY_NEXT_IMAGE_INITIATED,
  OTAU_IMAGE_BLOCK_REQ_INITIATED,
  OTAU_DOWNLOAD_FINISHED,
  OTAU_UPGRADE_REQUEST_INITIATED,
  OTAU_DEVICE_SHALL_CHANGE_IMAGE,
  OTAU_ISD_IMAGE_CACHE_IS_FOUND,
  OTAU_ISD_HARDWARE_FAULT,
  OTAU_ISD_NO_COMMUNICATION,
  OTAU_IMAGE_NOTIFICATION_RECEIVED,
  OTAU_DOUBLE_START,
  OTAU_MAX_MESSAGE_NUMBER,
} ZCL_OtauAction_t;

typedef void (* ZCL_OtauStatInd_t)(ZCL_OtauAction_t action);

typedef enum
{
  OTAU_ZB_STACK_VERSION_2006 = 0x0000,
  OTAU_ZB_STACK_VERSION_2007 = 0x0001,
  OTAU_ZB_STACK_VERSION_PRO  = 0x0002,
  OTAU_ZB_STACK_VERSION_IP   = 0x0003,
  OTAU_FAKE_VERSION          = 0xFFFF
} ZCL_OtauZigbeeStackVersion_t;

typedef enum
{
  OTAU_SPECIFIC_IMAGE_TYPE      = 0x0000,
  OTAU_MANU_SPEC_ENCRYPTED_NO_MIC_IMAGE_TYPE = 0x8000,
  OTAU_MANU_SPEC_ENCRYPTED_IMAGE_PLAIN_CRC_IMAGE_TYPE = 0xC000,
  OTAU_MANU_SPEC_ENCRYPTED_IMAGE_PLAIN_MAC_IMAGE_TYPE = 0xD000,
  OTAU_MANU_SPEC_ENCRYPTED_IMAGE_ENCRYPTED_CRC_IMAGE_TYPE = 0xE000,
  OTAU_MANU_SPEC_ENCRYPTED_IMAGE_ENCRYPTED_MAC_IMAGE_TYPE = 0xF000,
  OTAU_SECURITY_CREDENTIAL_TYPE = 0xFFC0,
  OTAU_CONFIGURATION_TYPE       = 0xFFC1,
  OTAU_LOG_TYPE                 = 0xFFC2,
  OTAU_WILD_CARD_TYPE           = 0xFFFF
} ZCL_OtauImageType_t;

typedef enum
{
  OTAU_UPGRADE_IMAGE             = 0x0000,
  OTAU_ECDSA_SIGNATURE           = 0x0001,
  OTAU_ECDSA_SIGNING_CERTIFICATE = 0x0002,
  OTAU_FAKE_NUMBER               = 0xFFFF
} ZCL_OtauTagID_t;

BEGIN_PACK
typedef enum PACK
{
  OTAU_NORMAL,
  OTAU_DOWNLOAD_IN_PROGRESS,
  OTAU_DOWNLOAD_COMPLETE,
  OTAU_WAITING_TO_UPGRADE,
  OTAU_COUNT_DOWN,
  OTAU_WAIT_FOR_MORE
} ZCL_ImageUpdateStatus_t;

typedef union PACK
{
  uint16_t memAlloc;
  struct PACK
  {
    uint16_t securityCredentialVersionPresent : 1;
    uint16_t deviceSpecificFile               : 1;
    uint16_t hardwareVersionPresent           : 1;
    uint16_t reserved                         : 13;
  };
} ZCL_OtauHeaderFieldControl_t;

typedef struct PACK
{
  ZCL_OtauTagID_t tagID;
  uint32_t        lengthField;
} ZCL_OtauSubElementHeader_t;

typedef union PACK
{
  uint32_t memAlloc;
  struct PACK
  {
    uint8_t appRelease;
    uint8_t appBuild;
    uint8_t stackRelease;
    uint8_t stackBuild;
  };
} ZCL_OtauFirmwareVersion_t;

/***************************************************************************//**
  \brief
  ZCL OTAU upgrade image header
*******************************************************************************/
typedef struct PACK
{
  uint32_t                     magicNumber;
  uint16_t                     headerVersion;
  uint16_t                     headerLength;
  ZCL_OtauHeaderFieldControl_t fieldControl;
  uint16_t                     manufacturerId;
  uint16_t                     imageType;
  ZCL_OtauFirmwareVersion_t    firmwareVersion;
  ZCL_OtauZigbeeStackVersion_t zigbeeStackVersion;
  OtauHeaderString_t           headerString;
  uint32_t                     totalImageSize;
} ZCL_OtauUpgradeImageHeader_t;

/***************************************************************************//**
  \brief
  ZCL OTAU Cluster attributes.
  Currently, all attributes are client side attributes (only stored on the client).
*******************************************************************************/
typedef struct PACK
{
  /*!
  \brief The attribute is used to store the IEEE address of the upgrade server
  resulted from the discovery of the upgrade server's identity. If the value is set
  to a non-zero value and corresponds to an IEEE address of a device that is no longer
  accessible, a device may choose to discover a new Upgrade Server depending on its own
  security policies. The attribute is mandatory because it serves as a placeholder in a case
  where the client is programmed, during manufacturing time, its upgrade server ID.
  In addition, the attribute is used to identify the current upgrade server the client is using
  in a case where there are multiple upgrade servers in the network.  The attribute is also helpful
  in a case when a client has temporarily lost connection to the network (for example, via a reset
  or a rejoin), it shall try to rediscover the upgrade server via network address discovery using
  the IEEE address stored in the attribute. By default the value is 0xffffffffffffffff, which is
  an invalid IEEE address.  The attribute is a client-side attribute and stored on the client.
  */
  struct PACK
  {
    ZCL_AttributeId_t       id;           //!<Attribute Id (0x0000)
    uint8_t                 type;         //!<Attribute type (ExtAddr_t - 64-bit) (typeId = 0xf0, #ZCL_IEEE_ADDRESS_DATA_TYPE_ID)
    uint8_t                 properties;   //!<Attribute properties bitmask
    ExtAddr_t               value;        //!<Attribute value (default 0xFFFFFFFFFFFFFFFF)
  } upgradeServerIEEEAddr;

  struct PACK
  {
    ZCL_AttributeId_t       id;           //!<Attribute Id (0x0006)
    uint8_t                 type;         //!<Attribute type (ZCL_ImageUpdateStatus_t - 8-bit enum) (typeId = 0x30)
    uint8_t                 properties;   //!<Attribute properties bitmask
    ZCL_ImageUpdateStatus_t value;        //!<Attribute value (default 0x00)
  } imageUpgradeStatus;

  struct PACK
  {
    ZCL_AttributeId_t       id;           //!<Attribute Id (0x0009)
    uint8_t                 type;         //!<Attribute type (uint16_t - 16-bit) (typeId = 0x21)
    uint8_t                 properties;   //!<Attribute properties bitmask
    uint16_t                value;        //!<Attribute value (default none)
    uint16_t                minVal;        //!<Attribute min value
    uint16_t                maxVal;        //!<Attribute max value
  } minimumBlockRequestDelay;
} ZCL_OtauClusterClientAttributes_t;

END_PACK

/***************************************************************************//**
  \brief
  ZCL OTAU Cluster commands.
*******************************************************************************/
BEGIN_PACK
/**
 * \brief Payload type field for Image Notify command.
*/
typedef enum PACK
{
  OTAU_PAYLOAD_TYPE_QUERY_JITTER      = 0x00, //!< Query jitter
  OTAU_PAYLOAD_TYPE_MANUFACTURER_CODE = 0x01, //!< Query jitter and manufacturer code
  OTAU_PAYLOAD_TYPE_IMAGE_TYPE        = 0x02, //!< Query jitter, manufacturer code and image type
  OTAU_PAYLOAD_TYPE_NEW_FILE_VERSION  = 0x03, //!< Query jitter, manufacturer code, image type, and new file version
  OTAU_PAYLOAD_TYPE_RESERVED          = 0x04, //!< Values 0x04 - 9xFF are reserved
} ZCL_ImageNotifyPayloadType_t;

/**
 * \brief Image Notify Command.
*/
typedef struct PACK
{
  ZCL_ImageNotifyPayloadType_t payloadType;
  uint8_t                      queryJitter;
  uint16_t                     manufacturerCode;
  uint16_t                     imageType;
  uint32_t                     newFileVersion;
} ZCL_OtauImageNotify_t;

/**
 * \brief Control field of query next image request.
*/
typedef struct PACK
{
  uint8_t hardwareVersionPresent  : 1;
  uint8_t reserved                : 7;
} ZCL_QueryNextImageReqControlField_t;

/**
 * \brief Query Next Image Request Command.
*/
typedef struct PACK
{
  ZCL_QueryNextImageReqControlField_t  controlField;
  uint16_t                             manufacturerId;
  ZCL_OtauImageType_t                  imageType;
  ZCL_OtauFirmwareVersion_t            currentFirmwareVersion;
} ZCL_OtauQueryNextImageReq_t;

/**
 * \brief Query Next Image Response Command.
*/
typedef struct PACK
{
  ZCL_Status_t              status;
  uint16_t                  manufacturerId;
  ZCL_OtauImageType_t       imageType;
  ZCL_OtauFirmwareVersion_t currentFirmwareVersion;
  uint32_t                  imageSize;
} ZCL_OtauQueryNextImageResp_t;

/**
 * \brief Control field of image block request.
*/
typedef struct PACK
{
  uint8_t reqNodeIeeeAddrPresent   : 1;
  uint8_t blockRequestDelayPresent : 1;
  uint8_t reserved                 : 6;
} ZCL_ImageBlockReqControlField_t;

/**
 * \brief Control field of image page request.
*/
typedef struct PACK
{
  uint8_t reqNodeIeeeAddrPresent   : 1;
  uint8_t reserved                 : 7;
} ZCL_PageBlockReqControlField_t;


/**
 * \brief Image Block Request Command.
*/
typedef struct PACK
{
  ZCL_ImageBlockReqControlField_t controlField;
  uint16_t                        manufacturerId;
  ZCL_OtauImageType_t             imageType;
  ZCL_OtauFirmwareVersion_t       firmwareVersion;
  uint32_t                        fileOffset;
  uint8_t                         maxDataSize;
  uint16_t                        blockRequestDelay;
} ZCL_OtauImageBlockReq_t;

/**
 * \brief Image Block Response Command.
*/
typedef struct PACK
{
  ZCL_Status_t                 status;
  union PACK
  {
    struct PACK
    {
      uint16_t                  manufacturerId;
      ZCL_OtauImageType_t       imageType;
      ZCL_OtauFirmwareVersion_t firmwareVersion;
      uint32_t                  fileOffset;
      uint8_t                   dataSize;
      uint8_t                   imageData[OFD_BLOCK_SIZE];
    };
    struct PACK
    {
      uint32_t                  currentTime;
      uint32_t                  requestTime;
      uint16_t                  blockRequestDelay;
    };
  };
} ZCL_OtauImageBlockResp_t;

/**
 * \brief Upgrade End Request Command.
*/
typedef struct PACK
{
  ZCL_Status_t              status;
  uint16_t                  manufacturerId;
  ZCL_OtauImageType_t       imageType;
  ZCL_OtauFirmwareVersion_t firmwareVersion;
} ZCL_OtauUpgradeEndReq_t;

/**
 * \brief Upgrade End Response Command.
*/
typedef struct PACK
{
  uint16_t                  manufacturerId;
  ZCL_OtauImageType_t       imageType;
  ZCL_OtauFirmwareVersion_t firmwareVersion;
  uint32_t                  currentTime;
  uint32_t                  upgradeTime;
} ZCL_OtauUpgradeEndResp_t;

/**
 * \brief Image Page Request Command.
*/
typedef struct PACK
{
  ZCL_PageBlockReqControlField_t controlField;
  uint16_t                       manufacturerId;
  ZCL_OtauImageType_t            imageType;
  ZCL_OtauFirmwareVersion_t      firmwareVersion;
  uint32_t                       fileOffset;
  uint8_t                        maxDataSize;
  uint16_t                       pageSize;
  uint16_t                       responseSpacing;
} ZCL_OtauImagePageReq_t;
END_PACK

/**
 * \brief ZCL OTAU Cluster Commands
 */
typedef struct
{
  //! Image Notify Command descriptor
  struct
  {
    ZCL_CommandId_t       id;
    ZclCommandOptions_t   options;
    ZCL_Status_t (* pImageNotifyind)(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_OtauImageNotify_t *payload);
  } imageNotify;

  //! Query Next Image Request Command descriptor
  struct
  {
    ZCL_CommandId_t       id;
    ZclCommandOptions_t   options;
    ZCL_Status_t (* pQueryNextImageReqInd)(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_OtauQueryNextImageReq_t *payload);
  } queryNextImageReq;

  //! Query Next Image Response Command descriptor
  struct
  {
    ZCL_CommandId_t       id;
    ZclCommandOptions_t   options;
    ZCL_Status_t (* pQueryNextImageRespInd)(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_OtauQueryNextImageResp_t *payload);
  } queryNextImageResp;

  //! Image Block Request Command descriptor
  struct
  {
    ZCL_CommandId_t       id;
    ZclCommandOptions_t   options;
    ZCL_Status_t (* pImageBlockReqInd)(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_OtauImageBlockReq_t *payload);
  } imageBlockReq;

  //! Image Page Request Command descriptor
  struct
  {
    ZCL_CommandId_t       id;
    ZclCommandOptions_t   options;
    ZCL_Status_t (* pImagePageReqInd)(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_OtauImagePageReq_t *payload);
  } imagePageReq;

  //! Image Block Response Command descriptor
  struct
  {
    ZCL_CommandId_t       id;
    ZclCommandOptions_t   options;
    ZCL_Status_t (* pImageBlockRespInd)(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_OtauImageBlockResp_t *payload);
  } imageBlockResp;

  //! Upgrade End Request Command descriptor
  struct
  {
    ZCL_CommandId_t       id;
    ZclCommandOptions_t   options;
    ZCL_Status_t (* pUpgradeEndReqInd)(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_OtauUpgradeEndReq_t *payload);
  } upgradeEndReq;

  //! Upgrade End Response Command descriptor
  struct
  {
    ZCL_CommandId_t       id;
    ZclCommandOptions_t   options;
    ZCL_Status_t (* pUpgradeEndRespInd)(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_OtauUpgradeEndResp_t *payload);
  } upgradeEndResp;
} ZCL_OtauClusterCommands_t;

/**//**
 \brief  Initialization parameters for OTAU service
 */
typedef struct
{
/** \brief Cluster side. Must be chosen from: \n
    ZCL_SERVER_CLUSTER_TYPE \n
    ZCL_CLIENT_CLUSTER_TYPE */
  uint8_t                    clusterSide;
/** \brief Endpoint for cluster. */
  Endpoint_t                 otauEndpoint;
/** \brief Profile ID, that uses otau service. */
  ProfileId_t                profileId;
/** \brief Current firmware version. Parameter is valid only for client. */
  ZCL_OtauFirmwareVersion_t  firmwareVersion;
} ZCL_OtauInitParams_t;

/******************************************************************************
                   Prototypes section
******************************************************************************/
/***************************************************************************//**
\brief Gets Otau's cluster

\ingroup zcl_otau

\return Otau's client cluster
******************************************************************************/
ZCL_Cluster_t ZCL_GetOtauClientCluster(void);

/***************************************************************************//**
\brief Gets Otau's cluster

\ingroup zcl_otau

\return Otau's server cluster
******************************************************************************/
ZCL_Cluster_t ZCL_GetOtauServerCluster(void);

/***************************************************************************//**
\brief OTAU parametres initialization. Start server discovery procedure
 on client side or start server.

\ingroup zcl_otau

\param[in] pInitParam - pointer to init structure.
\param[in] ind        - indication for OTAU actions \n
("ind" can be NULL if indication is not required.)

\return Service status
******************************************************************************/
ZCL_Status_t ZCL_StartOtauService(ZCL_OtauInitParams_t *pInitParam, ZCL_OtauStatInd_t ind);

/***************************************************************************//**
\brief Stop OTAU service. (It is not implemented for client)

\ingroup zcl_otau

\param[in] pInitParam - pointer to init structure.

\return Service status
******************************************************************************/
ZCL_Status_t ZCL_StopOtauService(ZCL_OtauInitParams_t *pInitParam);

/***************************************************************************//**
\brief Checks whether OTAU cluster is busy or not.

\return true - if OTAU cluster is busy, false - otherwise.
******************************************************************************/
bool zclIsOtauBusy(void);

/***************************************************************************//**
\brief The server may send an unsolicited Upgrade End Response command to the client.
This may be used for example if the server wants to synchronize the upgrade
on multiple clients simultaneously.

\ingroup zcl_otau

\param[in] addressing - pointer to structure with addressing information;
\param[in] resp - pointer to upgrade end response payload

\return true - data is sending, false - server is busy.
******************************************************************************/
bool ZCL_UnsolicitedUpgradeEndResp(ZCL_Addressing_t *addressing, ZCL_OtauUpgradeEndResp_t *resp);

/***************************************************************************//**
\brief This is called by the server to send image notify command to client

\ingroup zcl_otau

\param[in] addressing - pointer to structure with addressing information;
\param[in] resp - pointer to image notify command payload

\return true - data is sending, false - server is busy.
******************************************************************************/
bool ZCL_ImageNotifyReq(ZCL_Addressing_t *addressing, ZCL_OtauImageNotify_t *resp);

#if (USE_IMAGE_SECURITY == 1)
/***************************************************************************//**
\brief configure OTAU image key

\ingroup zcl_otau

\param[in] configureKeyDone - callback after key is written.
                              can be set to NULL if callback is not required

\return None
******************************************************************************/
void ZCL_ConfigureOtauImageKey(void (*configureKeyDone)());
#endif

#endif /* _ZCLOTAUCLUSTER_H */
