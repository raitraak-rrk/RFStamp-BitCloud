/************************************************************************//**
  \file zclPriceCluster.h

  \brief
    The header file describes the Price Cluster.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    05.02.09 A. Mandychev - Changed.
******************************************************************************/

#ifndef _ZCLPRICECLUSTER_H
#define _ZCLPRICECLUSTER_H

/******************************************************************************
                        Includes section
******************************************************************************/
#include <zcl.h>
#include <clusters.h>

/******************************************************************************
                        Defines section
******************************************************************************/
/**
 * \brief Publish Price command fields default values.
*/
#define PUBLISH_PRICE_DURATION_UNTIL_CHANGED                 0xFFFF
#define PUBLISH_PRICE_START_TIME_NOW                         0x0000
#define PUBLISH_PRICE_PRICE_RATIO_IS_NOT_USED                0xFF
#define PUBLISH_PRICE_GENERATION_PRICE_RATIO_IS_NOT_USED     0xFF
#define PUBLISH_PRICE_GENERATION_PRICE_IS_NOT_USED           0xFFFFFFFF
#define PUBLISH_PRICE_NUMBER_OF_BLOCK_THRESHOLDS_IS_NOT_USED 0xFF
#define PUBLISH_PRICE_CONTROL_IS_NOT_USED                    0x00
#define PUBLISH_PRICE_RATE_LABEL_MAX_LEN                     12

/**
 * \brief Price Server Cluster attributes amount.
*/
#define PRICE_CLUSTER_SERVER_ATTRIBUTES_AMOUNT     0

/**
 * \brief Price Client Cluster attributes amount.
*/
#define PRICE_CLUSTER_CLIENT_ATTRIBUTES_AMOUNT     0

/**
 * \brief Price Client Cluster commands amount.
*/
#define PRICE_CLUSTER_COMMANDS_AMOUNT  4

/**
 * \brief Price Cluster commands identifiers.
*/
#define GET_CURRENT_PRICE_COMMAND_ID      0x00
#define GET_SCHEDULED_PRICES_COMMAND_ID   0x01
#define PRICE_ACKNOWLEDGEMENT_COMMAND_ID  0x02
#define PUBLISH_PRICE_COMMAND_ID          0x00

#define PRICE_CLUSTER_COMMANDS(getCurrentPriceInd, publishPriceInd, getScheduledPricesInd, priceAcknowledgementInd) \
    DEFINE_COMMAND(getCurrentPriceCommand, GET_CURRENT_PRICE_COMMAND_ID, COMMAND_OPTIONS(CLIENT_TO_SERVER, ZCL_THERE_IS_RELEVANT_RESPONSE, ZCL_COMMAND_ACK), getCurrentPriceInd), \
    DEFINE_COMMAND(publishPriceCommand, PUBLISH_PRICE_COMMAND_ID, COMMAND_OPTIONS(SERVER_TO_CLIENT, ZCL_RESPONSE_CONTROLLED_BY_PAYLOAD, ZCL_COMMAND_ACK), publishPriceInd), \
    DEFINE_COMMAND(getScheduledPricesCommand, GET_SCHEDULED_PRICES_COMMAND_ID, COMMAND_OPTIONS(CLIENT_TO_SERVER, ZCL_THERE_IS_NO_RELEVANT_RESPONSE, ZCL_COMMAND_ACK), getScheduledPricesInd), \
    DEFINE_COMMAND(priceAcknowledgementCommand, PRICE_ACKNOWLEDGEMENT_COMMAND_ID, COMMAND_OPTIONS(CLIENT_TO_SERVER, ZCL_THERE_IS_NO_RELEVANT_RESPONSE, ZCL_COMMAND_ACK), priceAcknowledgementInd)

#define PRICE_CLUSTER_ZCL_CLIENT_CLUSTER_TYPE(clattributes, clcommands) \
  { \
    .id = PRICE_CLUSTER_ID, \
    .options = {.type = ZCL_CLIENT_CLUSTER_TYPE, .security = ZCL_APPLICATION_LINK_KEY_CLUSTER_SECURITY}, \
    .attributesAmount = PRICE_CLUSTER_CLIENT_ATTRIBUTES_AMOUNT, \
    .attributes = (uint8_t *)clattributes, \
    .commandsAmount = PRICE_CLUSTER_COMMANDS_AMOUNT, \
    .commands = (uint8_t *)clcommands \
  }

#define PRICE_CLUSTER_ZCL_SERVER_CLUSTER_TYPE(clattributes, clcommands) \
  { \
    .id = PRICE_CLUSTER_ID, \
    .options = {.type = ZCL_SERVER_CLUSTER_TYPE, .security = ZCL_APPLICATION_LINK_KEY_CLUSTER_SECURITY}, \
    .attributesAmount = PRICE_CLUSTER_SERVER_ATTRIBUTES_AMOUNT, \
    .attributes = (uint8_t *)clattributes, \
    .commandsAmount = PRICE_CLUSTER_COMMANDS_AMOUNT, \
    .commands = (uint8_t *)clcommands \
  }
#define DEFINE_PRICE_CLUSTER(cltype, clattributes, clcommands) PRICE_CLUSTER_##cltype(clattributes, clcommands)

#define PRICE_CLUSTER_ZCL_CLIENT_CLUSTER_TYPE_FLASH(clattributes, clcommands) \
  { \
    .id = PRICE_CLUSTER_ID, \
    .options = {.type = ZCL_CLIENT_CLUSTER_TYPE, .security = ZCL_APPLICATION_LINK_KEY_CLUSTER_SECURITY}, \
    .attributesAmount = PRICE_CLUSTER_CLIENT_ATTRIBUTES_AMOUNT, \
    .attributes = (uint8_t *)clattributes, \
    .commandsAmount = PRICE_CLUSTER_COMMANDS_AMOUNT, \
    .commands = (FLASH_PTR uint8_t *)clcommands \
  }

#define PRICE_CLUSTER_ZCL_SERVER_CLUSTER_TYPE_FLASH(clattributes, clcommands) \
  { \
    .id = PRICE_CLUSTER_ID, \
    .options = {.type = ZCL_SERVER_CLUSTER_TYPE, .security = ZCL_APPLICATION_LINK_KEY_CLUSTER_SECURITY}, \
    .attributesAmount = PRICE_CLUSTER_SERVER_ATTRIBUTES_AMOUNT, \
    .attributes = (uint8_t *)clattributes, \
    .commandsAmount = PRICE_CLUSTER_COMMANDS_AMOUNT, \
    .commands = (FLASH_PTR uint8_t *)clcommands \
  }
#define DEFINE_PRICE_CLUSTER_FLASH(cltype, clattributes, clcommands) PRICE_CLUSTER_##cltype##_FLASH(clattributes, clcommands)

/******************************************************************************
                    Types section
******************************************************************************/
BEGIN_PACK
/**
 * \brief Publish Price Command Payload format.
*/
typedef struct PACK
{
  uint32_t        providerId;
  uint8_t         rateLabelLength;
  uint8_t         rateLabel[PUBLISH_PRICE_RATE_LABEL_MAX_LEN];
  uint32_t        issuerEventId;
  ZCL_UTCTime_t   currentTime;
  uint8_t         unitOfMeasure;
  uint16_t        currency;
  struct PACK
  {
    LITTLE_ENDIAN_OCTET(2,(
      uint8_t priceTrailDigit : 4,
      uint8_t priceTier : 4
    ))
  } priceTrailDigitPriceTier;
  struct PACK
  {
    LITTLE_ENDIAN_OCTET(2,(
      uint8_t numberOfPriceTiers : 4,
      uint8_t registerTier : 4
    ))
  } numberOfPriceTiersRegisterTier;
  ZCL_UTCTime_t   startTime;
  uint16_t        durationInMinutes;
  uint32_t        price;
  uint8_t         priceRatio;
  uint32_t        generationPrice;
  uint8_t         generationPriceRatio;
  uint32_t        alternateCostDelivered;
  uint8_t         alternateCostUnit;
  uint8_t         alternateCostTrailingDigit;
  uint8_t         numberOfBlockThresholds;
  uint8_t         priceControl;
//  uint32_t        standingCharge;
} ZCL_PublishPrice_t;

/**
 * \brief Get Current Price Command Payload format.
*/
typedef struct PACK
{
  LITTLE_ENDIAN_OCTET(2,(
    uint8_t requestorRxOnWhenIdle :1, //!<The requestor Rx On When Idle sub-field
    uint8_t reserved              :7  //!<Reserved field
  ))
} ZCL_GetCurrentPrice_t;

/**
 * \brief Get Scheduled Prices Command Payload format.
*/
typedef struct PACK
{
  ZCL_UTCTime_t   startTime;
  uint8_t         numberOfEvents;
} ZCL_GetScheduledPrices_t;

/**
 * \brief Price Acknowledgement Command Payload format.
*/
typedef struct PACK
{
  uint32_t      providerId;
  uint32_t      issuerEventId;
  ZCL_UTCTime_t priceAckTime;
  uint8_t       control;
} ZCL_PriceAcknowledgement_t;
END_PACK


/**
 * \brief ZCL Price Cluster Commands
 */
typedef struct
{
  //! Get Current Price Command descriptor
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
    ZCL_Status_t (*getCurrentPrice)(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_GetCurrentPrice_t *payload);
  } getCurrentPriceCommand;

  //! Get Scheduled Prices Command descriptor
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
    ZCL_Status_t (*getScheduledPrices)(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_GetScheduledPrices_t *payload);
  } getScheduledPricesCommand;

  //! Publish Price Command descriptor
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
    ZCL_Status_t (*publishPrice)(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_PublishPrice_t *payload);
  } publishPriceCommand;

  //! Price Acknowledgement Command descriptor
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
    ZCL_Status_t (*priceAcknowledgement)(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_PriceAcknowledgement_t *payload);
  } priceAcknowledgementCommand;

} ZCL_PriceClusterCommands_t;
/**
 * \brief ZCL Price Cluster Commands
 */

#endif // _ZCLPRICECLUSTER_H
