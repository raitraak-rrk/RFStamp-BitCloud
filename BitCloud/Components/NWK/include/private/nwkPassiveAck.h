/**************************************************************************//**
  \file nwkPassiveAck.h

  \brief Private interface of the passive acknowledgement mechanism.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2011-07-16 Max Gekk - Created.
   Last change:
    $Id: nwkPassiveAck.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
/**//**
 *
 *  Passive acknowledgement means that every ZigBee router and ZigBee
 * coordinator keeps track of which of its neighboring devices have successfully
 * relayed the broadcast transmission. ZigBee spec r18, 3.6.5, page 411.
 *
 **/
#if !defined _NWK_PASSIVE_ACK_H
#define _NWK_PASSIVE_ACK_H
/******************************************************************************
                                Includes section
 ******************************************************************************/
#include <sysTypes.h>
#include <mac.h>
#include <nwkFrame.h>
#include <nwkTxDelay.h>
#include <nwkRx.h>

/******************************************************************************
                               Definitions section
 ******************************************************************************/
#define IS_PASSIVE_ACK_FOUND(offset) \
  ((offset).index < nwkMemPassiveAck()->amount)

/******************************************************************************
                                 Types section
 ******************************************************************************/
/** In this structute the node stores information about passive acks
 * (rebroadcasts) that are received from other routers (and coordinator too)
 * in the neighborhood. */
typedef struct _NwkPassiveAckEntry_t
{
  /*  This field is used as unque identifier of the entry. NULL means that
   * the entry is free. txDelay only stores pointer to NwkTxDelayReq_t
   * structure and nwkPassiveAck doesn't read or write fields of the structure. */
  NwkTxDelayReq_t *txDelay;
  /* Short address of node that originates a transmission of the broadcast packets. */
  ShortAddr_t srcAddr;
  /* Sequence number of NWK packets for which we wait passive acks. */
  NwkSequenceNumber_t seqNum;
} NwkPassiveAckEntry_t;

/* Type of value for storing the amount of passive ack entries. */
typedef uint8_t NwkPassiveAckAmount_t;
/* Type of index of a passive ack record. */
typedef NwkPassiveAckAmount_t NwkPassiveAckIndex_t;

typedef struct _NwkPassiveAck_t
{
  /* Total amount of passive acknowledgement in the table. */
  NwkPassiveAckAmount_t amount;
  /* The pointer to the first item of table that stores information
   * about each passive acks. */
  NwkPassiveAckEntry_t *table;
} NwkPassiveAck_t;

/* Offset of the passive ack entry in the passive ack table. */
typedef struct _NwkPassiveAckOffset_t
{
  /* Index of the passive ack entry in the passive ack table. */
  NwkPassiveAckIndex_t index;
  /* Value of this field is equal 1U << index. */
  NwkPassiveAckMap_t mask;
} NwkPassiveAckOffset_t;

/******************************************************************************
                              Prototypes section
 ******************************************************************************/
#if defined _NWK_PASSIVE_ACK_ && (defined _ROUTER_ || defined _COORDINATOR_)
/******************************************************************************
  \brief Creates the new entry in the passive ack table.

  \param[in] txDelay - that pointer is used as unique identifier.
  \param[in] prevHopAddr - short address of node from which the packet is received.
  \param[in] srcAddr - short address of node that initiates broadcast.
  \param[in] seqNum - sequence number in NWK header of broadcast packet.

  \return None.
 ******************************************************************************/
NWK_PRIVATE void nwkNewPassiveAck(NwkTxDelayReq_t *const txDelay,
  const ShortAddr_t prevHopAddr, const ShortAddr_t srcAddr,
  const NwkSequenceNumber_t seqNum);

/******************************************************************************
  \brief Starts tracing external broadcasts.

    Creates the new entry only for transit broadcast packets.

  \param[in] inPkt - the pointer to an incoming packet.
  \param[in] prevHopAddr - short address of node from which the packet is received.
  \param[in] srcAddr - short address of node that initiates broadcast.
  \param[in] seqNum - sequence number in NWK header of broadcast packet.
  \param[in] radius - value of radius from NWK header of original broadcast.

  \return None.
 ******************************************************************************/
NWK_PRIVATE void nwkTracePassiveAck(NwkInputPacket_t *const inPkt,
  const ShortAddr_t prevHopAddr, const ShortAddr_t srcAddr,
  const NwkSequenceNumber_t seqNum, const NwkRadius_t radius);

/******************************************************************************
  \brief Finds an passive ack entry by the identifier.

  \param[in] txDelay - primary key of an passive ack entry.

  \return Offset of an passive ack entry if it is found
          otherwise returns invalid offset \see IS_PASSIVE_ACK_FOUND.
 ******************************************************************************/
NWK_PRIVATE
NwkPassiveAckOffset_t nwkFindPassiveAck(const NwkTxDelayReq_t *const txDelay);

/******************************************************************************
  \brief Frees an passive ack entry.

  \param[in] txDelay - the valid pointer to NwkTxDelayReq_t structure,
                       used to find and free an entry in the passive ack table
  \return None.
 ******************************************************************************/
NWK_PRIVATE void nwkFreePassiveAck(const NwkTxDelayReq_t *const txDelay);

/******************************************************************************
  \brief Checks that all expected rebroadcast are received.

  \param[in] offset - offset of entry in the passive ack table.
  \return 'true' if all expected neighbors confirms that they was received
          original broadcast packet otherwise 'false'.
 ******************************************************************************/
NWK_PRIVATE bool nwkIsPassiveAckDone(const NwkPassiveAckOffset_t offset);

/******************************************************************************
  \brief A rebroadcast is received, stores information about it.

  \param[in] prevHopAddr - short address of node from which the packet is received.
  \param[in] srcAddr - short address of node that initiates broadcast.
  \param[in] seqNum - sequence number in NWK header of broadcast packet.

  \return None.
 ******************************************************************************/
NWK_PRIVATE void nwkPassiveAckReceived(const ShortAddr_t prevHopAddr,
  const ShortAddr_t srcAddr, const NwkSequenceNumber_t seqNum);

/******************************************************************************
  \brief Resets the internal state and structures of the passive ack component.
 ******************************************************************************/
NWK_PRIVATE void nwkResetPassiveAck(void);

#else
#define nwkNewPassiveAck(id, prevHopAddr, srcAddr, seqNum) (void)0
#define nwkTracePassiveAck(inPkt, prevHopAddr, srcAddr, seqNum, radius) (void)0
#define nwkFreePassiveAck(id) (void)0
#define nwkPassiveAckReceived(prevHopAddr, srcAddr, seqNum) (void)0
#define nwkResetPassiveAck() (void)0
#endif /* _NWK_PASSIVE_ACK_ */
#endif /* _NWK_PASSIVE_ACK_H */
/** eof nwkPassiveAck.h */

