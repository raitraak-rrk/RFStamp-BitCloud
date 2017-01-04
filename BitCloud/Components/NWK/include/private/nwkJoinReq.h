/**************************************************************************//**
  \file nwkJoinReq.h

  \brief Join request header file.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2007-06-14 S. Vladykin - Created.
    2008-12-10 M. Gekk     - Optimization of NWK memory.
    2009-02-09 M. Gekk     - Refactoring.
   Last change:
    $Id: nwkJoinReq.h 27584 2015-01-09 14:45:42Z unithra.c $
******************************************************************************/
#if !defined _NWK_JOINREQ_H
#define _NWK_JOINREQ_H

/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <nwkConfig.h>
#include <sysQueue.h>
#include <mac.h>
#include <nlmeJoin.h>
#include <nldeData.h>
#include <nlmeNetworkDiscovery.h>
#include <nwkNeighbor.h>
#include <nwkFrame.h>
#include <appTimer.h>
#include <nwkAddress.h>

/******************************************************************************
                               Define(s) section
 ******************************************************************************/
#define NWK_JOIN_REQ_TX_PARAMETERS \
  {NWK_TX_DELAY_UNICAST_COMMAND, nwkPrepareJoinReqTx, nwkConfirmJoinReqTx, true}

/******************************************************************************
                                 Types section
 ******************************************************************************/
/** Internal states of the join request component. */
typedef enum _NwkJoinReqState_t
{
   JOIN_REQ_IDLE_STATE = 0xDA,
   JOIN_REQ_FIRST_STATE = 0x21,
   JOIN_REQ_GET_ELEM_FROM_QUEUE = JOIN_REQ_FIRST_STATE,
   JOIN_REQ_CONFIRM_STATE = 0x22,
   JOIN_REQ_ASSOCIATION_WAIT_STATE = 0x23,
   JOIN_REQ_SET_BEACON_PAYLOAD_STATE = 0x24,
   JOIN_REQ_SET_BEACON_PAYLOAD_LEN_STATE = 0x25,
   JOIN_REQ_SET_RX_ON_WHEN_IDLE_STATE = 0x26,
   JOIN_REQ_RX_ENABLE_STATE = 0x27,
   JOIN_REQ_PKT_PREPARE_AND_SEND_STATE = 0x28,
   JOIN_REQ_SET_PANID_STATE = 0x29,
   JOIN_REQ_SET_CHANNEL_STATE = 0x2A,
   JOIN_REQ_GET_RESPONSE_WAIT_TIME_STATE = 0x2B,
   JOIN_REQ_WAITING_MAC_LATENCY = 0x2C,
   JOIN_REQ_REJOIN_RESPONSE_WAITING_STATE = 0x2D,
   JOIN_REQ_SET_SHORT_ADDRESS_STATE = 0x2E,
   JOIN_REQ_SET_COORD_SHORT_ADDR_STATE = 0x2F,
   JOIN_REQ_SET_COORD_EXT_ADDR_STATE = 0x30,
   JOIN_REQ_SET_SHORT_ADDRESS_ONLY_STATE = 0x31,
   JOIN_REQ_NETWORK_DISCOVERY_STATE = 0x32,
   JOIN_REQ_SET_FIRST_SHORT_ADDRESS_STATE = 0x33,
   JOIN_REQ_SET_TRANSACTION_TIME_STATE = 0x34,
   JOIN_REQ_SET_EXT_ADDRESS_STATE = 0x35,
   JOIN_REQ_LAST_STATE
} NwkJoinReqState_t;

/** Format of rejoin command payload. ZigBee spec r17, 3.4.6.3, page 329. */
/** The NWK frame payload contains a command identifier field and a capability
 * information field. The command frame identifier shall contain the value
 * indicating a rejoin request command frame. */
BEGIN_PACK
typedef struct PACK
{
  NwkCommandIdField_t commandId;
  /** This one-octet field has the format of the capability information field
   * in the association request command. ZigBee spec r17, 3.4.6.3.1, page 329. */
  MAC_CapabilityInf_t capability;
} NwkRejoinReqCmd_t;
END_PACK

/** Internal parameters of the join request component. */
typedef struct _NwkJoinReq_t
{
  NwkJoinReqState_t state;
  QueueDescriptor_t queue;
  NWK_JoinReq_t *joinReq;
  NwkNeighbor_t *currentCandidateRouter;
  uint8_t attempNum;
  HAL_AppTimer_t timer;
  MAC_PollReq_t pollReq;
  union
  {
    MAC_AssociateReq_t macAssoc;
    MAC_SetReq_t macSet;
    MAC_GetReq_t macGet;
    MAC_RxEnableReq_t macRxEnable;
    NWK_NetworkDiscoveryReq_t netDisc;
  } req;
} NwkJoinReq_t;

/******************************************************************************
                             Prototypes section
 ******************************************************************************/
#if defined _ROUTER_ || defined _ENDDEVICE_
/**************************************************************************//**
  \brief Action when command received.

  \param[in] payload - a payload of network join request command.
  \param[in] header - pointer to raw NWK header.
  \param[in] parse - parsed header fields.

  \return 'true' if continue processing of command packet otherwise 'false'.
 ******************************************************************************/
NWK_PRIVATE bool nwkRejoinResponseFrameInd(const uint8_t *const payload,
  const NwkFrameHeader_t *const header, const NwkParseHeader_t *const parse);

/**************************************************************************//**
  \brief Reset the join request component
 ******************************************************************************/
NWK_PRIVATE void nwkResetJoinReq(void);

/**************************************************************************//**
  \brief Task handler of NWK join request component.
 ******************************************************************************/
NWK_PRIVATE void nwkJoinReqTaskHandler(void);

/**************************************************************************//**
  \brief Prepare header and payload of the join request command.

  \param[in] outPkt - pointer to output packet.
  \return None.
 ******************************************************************************/
NWK_PRIVATE void nwkPrepareJoinReqTx(NwkOutputPacket_t *const outPkt);

/**************************************************************************//**
  \brief Confirmation of join request command transmission.

  \param[in] outPkt - pointer to output packet.
  \param[in] status - network status of join request transmission.
  \return None.
 ******************************************************************************/
NWK_PRIVATE void nwkConfirmJoinReqTx(NwkOutputPacket_t *const outPkt,
  const NWK_Status_t status);

/******************************************************************************
  \brief nwkJoinReq idle checking.

  \return true, if nwkJoinReq performs no activity, false - otherwise.
 ******************************************************************************/
NWK_PRIVATE bool nwkJoinReqIsIdle(void);

#else /* _ROUTER_ or _ENDDEVICE_ */

#define nwkRejoinResponseFrameInd NULL
#define nwkResetJoinReq() (void)0
#define nwkJoinReqTaskHandler NULL
#define nwkPrepareJoinReqTx NULL
#define nwkConfirmJoinReqTx NULL
#define nwkJoinReqIsIdle NULL

#endif /* _ROUTER_ or _ENDDEVICE_ */

/**************************************************************************//**
  \brief Processes BC_EVENT_NETWORK_ENTERED

  \param[in] eventId - id of raised event;
  \param[in] data    - event's data
 ******************************************************************************/
NWK_PRIVATE void nwkJoinComplete(SYS_EventId_t eventId, SYS_EventData_t data);

#endif /* _NWK_JOINREQ_H */
/** eof nwkJoinReq.h */

