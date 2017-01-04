/**************************************************************************//**
  \file nwkRouteRequest.h

  \brief Interface of route request commands.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2007-09-17 V. Panov - Created.
    2008-12-10 M. Gekk  - Optimization of NWK memory.
    2009-11-20 M. Gekk  - Refactoring.
   Last change:
    $Id: nwkRouteRequest.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#if !defined _NWK_ROUTE_REQUEST_H
#define _NWK_ROUTE_REQUEST_H

/******************************************************************************
                              Includes section
 ******************************************************************************/
#include <nwkConfig.h>
#include <nwkSystem.h>
#include <appFramework.h>
#include <nldeData.h>
#include <nwkFrame.h>
#include <nwkRouteDiscoveryTable.h>

/******************************************************************************
                              Define(s) section
 ******************************************************************************/
/** Transmission parameters of route request command. */
#define NWK_INITIAL_ROUTE_REQ_TX_PARAMETERS \
  {NWK_TX_DELAY_INITIAL_ROUTE_REQUEST, NULL, nwkConfirmRouteRequestTx, true}
#define NWK_ROUTE_REQ_TX_PARAMETERS \
  {NWK_TX_DELAY_ROUTE_REQUEST, NULL, NULL, true}
#define NWK_INITIAL_MANYTOONE_ROUTE_REQ_TX_PARAMETERS \
  {NWK_TX_DELAY_INITIAL_MANYTOONE_ROUTE_REQUEST, NULL, nwkConfirmRouteRequestTx, true}
#define NWK_MANYTOONE_ROUTE_REQ_TX_PARAMETERS \
  {NWK_TX_DELAY_MANYTOONE_ROUTE_REQUEST, NULL, NULL, true}

/******************************************************************************
                                Types section
 ******************************************************************************/
/** Many-to-One field values. See ZigBee spec r18, 3.4.1.3.1.1, page 320. */
typedef enum _NwkRReqManyToOneFlag_t
{
  /** The route request is not a many-to-one route request. */
  NWK_RREQ_IS_NOT_MANY_TO_ONE = 0,
  /** The route request is a many-to-one route request and the sender supports
   * a route record table.*/
  NWK_RREQ_IS_MANY_TO_ONE = 1,
  /** The route request is a many-to-one route request and the sender does not
   * support a route record table. */
  NWK_RREQ_IS_MANY_TO_ONE_NO_RREC_TABLE = 2,
  NWK_RREQ_RESERVED_MANY_TO_ONE_FLAG = 3
} NwkRReqManyToOneFlag_t;

BEGIN_PACK
/** Route request command options field. ZigBee spec r18, 3.4.1.3.1, page 320.
 **/
typedef struct PACK _NwkRouteRequestOptions_t
{
  LITTLE_ENDIAN_OCTET(5, (
    NwkBitField_t reserved1 : 3,
    /** The many-to-one field shall have one of the non-reserved values shown
     * in ZigBee spec r18, Table 3.41 and type NwkRReqManyToOneFlag_t. */
    NwkBitField_t manyToOne : 2,
    /** The destination IEEE address field is a single-bit field. It shall have
     * a value of 1 if, and only if, the command frame contains the destination
     * IEEE address. ZigBee spec r18, 3.4.1.3.1.2, page 321. */
    NwkBitField_t dstExt    : 1,
    /** The multicast sub-field is a single-bit field. It shall have a value
     * of 1 if, and only if, the command frame is a request for a route
     * to a multicast group... ZigBee spec r18, 3.4.1.3.1.3, page 321. */
    NwkBitField_t multicast : 1,
    NwkBitField_t reserved2 : 1
  ))
} NwkRouteRequestOptions_t;
END_PACK

BEGIN_PACK
/** Route request command frame format. See ZigBee spec r18,
 * Figure 3.11, page 319. */
typedef struct PACK _NwkRouteRequestPayload_t
{
  /** The identifier of the route request command. */
  uint8_t commandId;
  /** Route request command options field. See NwkRouteRequestOptions_t. */
  NwkRouteRequestOptions_t options;
  /** The route request identifier is an 8-bit sequence number for route
   * requests and is incremented by 1 every time the NWK layer on a particular
   * device issues a route request. ZigBee spec r18, 3.4.1.3.2, page 321. */
  NwkRouteRequestId_t identifier;
  /** The destination address shall be 2 octets in length and represents
   * the intended destination of the route request command frame.
   * ZigBee spec r18, 3.4.1.3.3. */
  ShortAddr_t dstAddr;
  /** The path cost field is eight bits in length and is used to accumulate
   * routing cost information as a route request command frame moves through
   * the network. ZigBee spec r18, 3.4.1.3.4, page 321; sub-clause 3.6.3.5.2. */
  NwkPathCost_t pathCost;
  /** The destination IEEE address shall be 8 octets in length and represents
   * the IEEE address of the destination of the route request command frame.
   ** ZigBee spec r18, 3.4.1.3.5, page 321. */
  ExtAddr_t dstExtAddr;
} NwkRouteRequestPayload_t;
END_PACK

/******************************************************************************
                               Prototypes section
 ******************************************************************************/
#if defined NWK_ROUTING_CAPACITY \
  && (defined _NWK_MESH_ROUTING_ || defined _NWK_MANY_TO_ONE_ROUTING_)
/**************************************************************************//**
  \brief Action when route request command received.

  \param[in] payload - pointer to payload of route request command.
  \param[in] header - pointer to raw network header of route request command.
  \param[in] parse - parsed header fields.

  \return 'true' if continue processing of command packet otherwise 'false'.
 ******************************************************************************/
NWK_PRIVATE bool nwkRouteRequestFrameInd(const uint8_t *const payload,
  const NwkFrameHeader_t *const header, const NwkParseHeader_t *const parse);

/**************************************************************************//**
  \brief Prepare header and payload of the route request command.

  \param[in] outPkt - pointer to output packet.
  \param[in] entry - pointer to a route discovery entry.
  \return None.
 ******************************************************************************/
NWK_PRIVATE void nwkPrepareRouteRequestTx(NwkOutputPacket_t *const outPkt,
  NwkRouteDiscoveryEntry_t *const entry);

#else /* NWK_ROUTING_CAPACITY and (_NWK_MESH_ROUTING_ or _NWK_MANY_TO_ONE_ROUTING_) */

#define nwkRouteRequestFrameInd NULL

#endif /* NWK_ROUTING_CAPACITY and (_NWK_MESH_ROUTING_ or _NWK_MANY_TO_ONE_ROUTING_) */
#endif /* _NWK_ROUTE_REQUEST_H */
/** eof nwkRouteRequest.h */

