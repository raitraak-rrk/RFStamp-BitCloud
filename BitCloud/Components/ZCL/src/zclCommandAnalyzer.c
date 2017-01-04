/************************************************************************//**
  \file zclCommandAnalyzer.c

  \brief
    ZCL command analyzer implementation

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    21.03.13 N. Fomin - Created.
******************************************************************************/

/******************************************************************************
                           Includes section
******************************************************************************/
#include <zclCommandAnalyzer.h>
#include <clusters.h>
#include <zclMessagingCluster.h>
#include <zclPriceCluster.h>

/******************************************************************************
                            Definitions section
******************************************************************************/
#define ZCL_COMMAND_TABLE_SIZE 2u

/******************************************************************************
                            Types section
******************************************************************************/
typedef bool (*CheckerRoutine_t)(uint8_t *);

typedef struct
{
  ClusterId_t      cluster;
  ZCL_CommandId_t  command;
  CheckerRoutine_t checker;
} ZclCommandRecord_t;

/******************************************************************************
                           Prototypes section
******************************************************************************/
static bool displayMessagePayloadChecker(uint8_t *payload);
static bool publishPricePayloadChecker(uint8_t *payload);

/******************************************************************************
                           Constants section
******************************************************************************/
PROGMEM_DECLARE(ZclCommandRecord_t zclCommandTable[ZCL_COMMAND_TABLE_SIZE]) =
{
  {MESSAGE_CLUSTER_ID, DISPLAY_MESSAGE_COMMAND_ID, displayMessagePayloadChecker},
  {PRICE_CLUSTER_ID,   PUBLISH_PRICE_COMMAND_ID,   publishPricePayloadChecker}
};

/******************************************************************************
                           Implementation section
******************************************************************************/
/**************************************************************************//**
\brief Checks if response for command is required based on its payload

\param[in] cluster - cluster id of command
\param[in] cmd     - command id
\param[in] payload - command payload

\returns true if resposne is required, false otherwise
******************************************************************************/
bool zclIsResponseRequiredByPayload(ClusterId_t cluster, ZCL_CommandId_t cmd, uint8_t dir, uint8_t *payload)
{
  for (uint8_t i = 0; i < ZCL_COMMAND_TABLE_SIZE; i++)
  {
    if (cluster == zclCommandTable[i].cluster)
      if (cmd == zclCommandTable[i].command)
        if (ZCL_FRAME_CONTROL_DIRECTION_SERVER_TO_CLIENT == dir)
          return zclCommandTable[i].checker(payload);
  }

  return false;
}

/***************************************************************************//**
\brief Checks whether response for command is required based on its address mode

\param[in] req - pointer to request structure

\return true - if response is required, false - otherwise.
******************************************************************************/
bool zclIsResponseRequiredByAddrMode(ZCL_Request_t *req)
{
  if ((APS_SHORT_ADDRESS == req->dstAddressing.addrMode &&
       IS_BROADCAST_ADDR(req->dstAddressing.addr.shortAddress))
#ifdef _APS_MULTICAST_
       || APS_GROUP_ADDRESS == req->dstAddressing.addrMode
#endif
     )
    return false;

  return true;
}

/**************************************************************************//**
\brief Checks if response for Display Message is required

\param[in] payload - comamnd payload

\returns true if resposne is required, false otherwise
******************************************************************************/
static bool displayMessagePayloadChecker(uint8_t *payload)
{
  DisplayMessage_t *cmd = (DisplayMessage_t *)payload;

  return cmd->messageControl.messageConfirmation;
}

/**************************************************************************//**
\brief Checks if response for Publish Price is required

\param[in] payload - comamnd payload

\returns true if resposne is required, false otherwise
******************************************************************************/
static bool publishPricePayloadChecker(uint8_t *payload)
{
  ZCL_PublishPrice_t *cmd = (ZCL_PublishPrice_t *)payload;

  return cmd->priceControl;
}

//eof zclCommandAnalyzer.c

