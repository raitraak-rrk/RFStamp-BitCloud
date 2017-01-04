/**************************************************************************//**
  \file nwkPacket.h

  \brief Interface of the network packet manager.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2009-07-06 M. Gekk - Created.
   Last change:
    $Id: nwkPacket.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#if !defined _NWK_PACKET_H
#define _NWK_PACKET_H

/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <nwkConfig.h>
#include <sysQueue.h>
#include <mac.h>
#include <nwkCommon.h>
#include <nwkRx.h>
#include <nwkTx.h>

/******************************************************************************
                                Types section
 ******************************************************************************/
/** Type of network packet. */
typedef enum _NwkPacketType_t
{
  NWK_UNKNOWN_PACKET = 0x0,
  /** Command packet from NWK component. */
  NWK_OUTPUT_COMMAND_PACKET = 0x1,
  /** CHILD Command packet from NWK component. */
  NWK_OUTPUT_COMMAND_CHILD_PACKET = 0x2,
  /** Command packet from MAC-layer. */
  NWK_INPUT_COMMAND_PACKET = 0x3,
  /** Data packet from MAC-layer. */
  NWK_INPUT_DATA_PACKET = 0x4,
  /** Transit packet from MAC-layer. */
  NWK_TRANSIT_PACKET = 0x5,
  /** Loopback transmission from APS-layer. */
  NWK_LOOPBACK_PACKET = 0x6,
  /** Data packet from an external layer. */
  NWK_EXTERN_PACKET = 0x7,
  NWK_BUFFER_TYPE_LAST
} NwkPacketType_t;

/** Network packet type. */
typedef struct _NwkPacket_t
{
  /** Network packet type. */
  NwkPacketType_t type;
  TOP_GUARD
  uint8_t macHeader[MAC_MSDU_OFFSET];
  uint8_t macPayload[MAC_MAX_MSDU_SIZE];
#if (MAC_AFFIX_LENGTH - MAC_MSDU_OFFSET)
  uint8_t macFooter[MAC_AFFIX_LENGTH - MAC_MSDU_OFFSET];
#endif
  BOTTOM_GUARD
  union
  {
    /** Service information for incoming packet from MAC-layer. */
    NwkInputPacket_t in;
    /** Service information for outgoing packet from NWK-layer. */
    NwkOutputPacket_t out;
#if defined _NWK_ALLOCATOR_
    /** Service information for outgoing packet from an upper-layer. */
    NwkExternPacket_t ext;
#endif /* _NWK_ALLOCATOR_ */
  } pkt;
} NwkPacket_t;

/** Type of total amount of network packets. */
typedef uint8_t NwkPacketsAmount_t;

/** Internal variables of the network packet manager. */
typedef struct _NwkPacketManager_t
{
  /** Array of network packets. */
  NwkPacket_t *pkt;
  /** Total amount of network packets. */
  NwkPacketsAmount_t amount;
  /** Queue of external packet allocation requests. */
  QueueDescriptor_t queue;
} NwkPacketManager_t;

/******************************************************************************
                               Prototypes section
 ******************************************************************************/
/**************************************************************************//**
  \brief Allocate a free memory space for an input packet with type.

  \param[in] type - type of packet.
  \param[in] length - needed packet length.
  \return Pointer to input packet or NULL.
 ******************************************************************************/
NWK_PRIVATE NwkInputPacket_t* nwkAllocInputPacket(const NwkPacketType_t type,
  const NwkLength_t length);

/**************************************************************************//**
  \brief Free an input packet.

  \param[in] inPkt - pointer to an input packet.
  \return None.
 ******************************************************************************/
NWK_PRIVATE void nwkFreeInputPacket(NwkInputPacket_t *const inPkt);

/**************************************************************************//**
  \brief Allocate a free memory space for an output.

  \param[in] type - type of output packet.
  \return Pointer to output packet or NULL.
 ******************************************************************************/
NWK_PRIVATE NwkOutputPacket_t* nwkAllocOutputPacket(const NwkPacketType_t type);

/**************************************************************************//**
  \brief Free an output packet.

  \param[in] outPkt - pointer to an output packet.
  \return None.
 ******************************************************************************/
NWK_PRIVATE void nwkFreeOutputPacket(NwkOutputPacket_t *const outPkt);

/**************************************************************************//**
  \brief Reset the network packet manager.
 ******************************************************************************/
NWK_PRIVATE void nwkResetPacketManager(void);

#if defined NWK_ROUTING_CAPACITY
/**************************************************************************//**
  \brief To transform an input packet to the output packet.

  \param[in] inPkt - pointer to an input packet.
  \return None.
 ******************************************************************************/
NWK_PRIVATE
NwkOutputPacket_t* nwkTransformInToOutPacket(NwkInputPacket_t *const inPkt);
#endif /* NWK_ROUTING_CAPACITY */

#if defined _NWK_ALLOCATOR_
/**************************************************************************//**
  \brief Process an incoming request to allocate memory for NWK_DataReq.
 ******************************************************************************/
NWK_PRIVATE void nwkAllocDataReqTaskHandler(void);

#else
#define nwkAllocDataReqTaskHandler NULL
#endif /* _NWK_ALLOCATOR_ */
#endif /* _NWK_PACKET_H */
/** eof nwkPacket.h */

