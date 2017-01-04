/***************************************************************************//**
  \file zclBACnetProtocolTunnelCluster.h

  \brief
    The header file describes the ZCL BACnet Protocol Tunnel Cluster and its interface

    The file describes the types and interface of the ZCL BACnet Protocol Tunnel Cluster

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    20.10.09 D. Kasyanov - Created.
*******************************************************************************/

#ifndef _ZCLBACNETPROTOCOLTUNNELCLUSTER_H
#define _ZCLBACNETPROTOCOLTUNNELCLUSTER_H

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
/***************************************************************************//**
  \brief ZCL BACnet Protocol Tunnel Cluster server side attributes amount
*******************************************************************************/
#define ZCL_BACNET_PRTOCOL_TUNNEL_CLUSTER_SERVER_ATTRIBUTES_AMOUNT        0
/***************************************************************************//**
  \brief ZCL BACnet Protocol Tunnel client side attributes amount
*******************************************************************************/
#define ZCL_BACNET_PRTOCOL_TUNNEL_CLUSTER_CLIENT_ATTRIBUTES_AMOUNT        0
//There are no any attributes at the client side
/***************************************************************************//**
  \brief ZCL BACnet Protocol Tunnel Cluster derver side commands amount
*******************************************************************************/
#define ZCL_BACNET_PROTOCOL_TUNNEL_COMMANDS_AMOUNT          1
/**
 * \brief Generic tunnel client side commands identifiers.
*/
#define ZCL_BACNET_PROTOCOL_TRANSFER_NPDU_COMMAND_ID 0x00

#define ZCL_DEFINE_BACNET_PROTOCOL_CLUSTER_TUNNEL_CLUSTER_COMMANDS(transferNPDUCommandInd) \
    DEFINE_COMMAND(transferNPDUCommand, ZCL_BACNET_PROTOCOL_TRANSFER_NPDU_COMMAND_ID, COMMAND_OPTIONS(CLIENT_TO_SERVER, ZCL_THERE_IS_NO_RELEVANT_RESPONSE, ZCL_COMMAND_ACK), transferNPDUCommandInd)

#define BACNET_PROTOCOL_TUNNEL_ZCL_CLIENT_CLUSTER_TYPE(clattributes, clcommands) \
{   \
  .id = BACNET_PROTOCOL_TUNNEL_CLUSTER_ID, \
  .options = {.type = ZCL_CLIENT_CLUSTER_TYPE, .security = ZCL_APPLICATION_LINK_KEY_CLUSTER_SECURITY}, \
  .attributesAmount = ZCL_BACNET_PRTOCOL_TUNNEL_CLUSTER_CLIENT_ATTRIBUTES_AMOUNT, \
  .attributes = (uint8_t *)clattributes, \
  .commandsAmount = ZCL_BACNET_PROTOCOL_TUNNEL_COMMANDS_AMOUNT, \
  .commands = (uint8_t *)clcommands \
}

#define BACNET_PROTOCOL_TUNNEL_ZCL_SERVER_CLUSTER_TYPE(clattributes, clcommands) \
{ \
  .id = BACNET_PROTOCOL_TUNNEL_CLUSTER_ID, \
  .options = {.type = ZCL_SERVER_CLUSTER_TYPE, .security = ZCL_APPLICATION_LINK_KEY_CLUSTER_SECURITY}, \
  .attributesAmount = ZCL_BACNET_PRTOCOL_TUNNEL_CLUSTER_SERVER_ATTRIBUTES_AMOUNT, \
  .attributes = (uint8_t *)clattributes, \
  .commandsAmount = ZCL_BACNET_PROTOCOL_TUNNEL_COMMANDS_AMOUNT, \
  .commands = (uint8_t *)clcommands \
}

#define DEFINE_BACNET_PROTOCOL_TUNNEL_CLUSTER(cltype, clattributes, clcommands) BACNET_PROTOCOL_TUNNEL_CLUSTER##cltype(clattributes, clcommands)

#define BACNET_PROTOCOL_TUNNEL_ZCL_CLIENT_CLUSTER_TYPE_FLASH(clattributes, clcommands) \
{   \
  .id = BACNET_PROTOCOL_TUNNEL_CLUSTER_ID, \
  .options = {.type = ZCL_CLIENT_CLUSTER_TYPE, .security = ZCL_APPLICATION_LINK_KEY_CLUSTER_SECURITY}, \
  .attributesAmount = ZCL_BACNET_PRTOCOL_TUNNEL_CLUSTER_CLIENT_ATTRIBUTES_AMOUNT, \
  .attributes = (uint8_t *)clattributes, \
  .commandsAmount = ZCL_BACNET_PROTOCOL_TUNNEL_COMMANDS_AMOUNT, \
  .commands = (FLASH_PTR uint8_t *)clcommands \
}

#define BACNET_PROTOCOL_TUNNEL_ZCL_SERVER_CLUSTER_TYPE_FLASH(clattributes, clcommands) \
{ \
  .id = BACNET_PROTOCOL_TUNNEL_CLUSTER_ID, \
  .options = {.type = ZCL_SERVER_CLUSTER_TYPE, .security = ZCL_APPLICATION_LINK_KEY_CLUSTER_SECURITY}, \
  .attributesAmount = ZCL_BACNET_PRTOCOL_TUNNEL_CLUSTER_SERVER_ATTRIBUTES_AMOUNT, \
  .attributes = (uint8_t *)clattributes, \
  .commandsAmount = ZCL_BACNET_PROTOCOL_TUNNEL_COMMANDS_AMOUNT, \
  .commands = (FLASH_PTR uint8_t *)clcommands \
}

#define DEFINE_BACNET_PROTOCOL_TUNNEL_CLUSTER_FLASH(cltype, clattributes, clcommands) BACNET_PROTOCOL_TUNNEL_CLUSTER##cltype##_FLASH(clattributes, clcommands)
/*******************************************************************************
                    Types section
*******************************************************************************/
BEGIN_PACK
/**
 * \brief Transfer NPDU Command Payload format.
*/
typedef struct PACK
{
    uint8_t npdu[1]; //!<npdu octet string
} ZCL_TransferNPDU_t;
END_PACK
/**
 * \brief BACnet protocol tunnel cluster commands.
*/
typedef struct
{
    struct
    {
        ZCL_CommandId_t id;
        ZclCommandOptions_t options;
        ZCL_Status_t (*transferNPDUCommand)(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_TransferNPDU_t *payload);
    } transferNPDUCommand;
} ZCL_BACnetProtocolTunnelCommands_t;

#endif /* _ZCLBACNETPROTOCOLTUNNELCLUSTER_H */

