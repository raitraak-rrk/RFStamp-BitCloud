/**************************************************************************//**
  \file nwkLinkStatus.h

  \brief The data structures concerning processing of LinkStatus command

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2007-08-01 V. Panov - Created.
    2009-04-10 M. Gekk  - Refactoring.
   Last change:
    $Id: nwkLinkStatus.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
/**//**
 *  The link status command frame allows neighboring routers to communicate
 * their incoming link costs to each other as described in sub-clause 3.6.3.4.
 * Link status frames are transmitted as one-hop broadcasts without retries.
 * ZigBee spec r17, 3.4.8, page 330.
 * */
#if !defined _NWK_LINK_STATUS_H
#define _NWK_LINK_STATUS_H

/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <nwkConfig.h>
#include <nwkSystem.h>
#include <appTimer.h>
#include <appFramework.h>
#include <nwkCommon.h>
#include <nldeData.h>
#include <nwkFrame.h>

/******************************************************************************
                               Define(s) section
 ******************************************************************************/
#define NWK_LINK_STATUS_TX_PARAMETERS \
  {NWK_TX_DELAY_LINK_STATUS, nwkPrepareLinkStatusTx,\
   nwkConfirmLinkStatusTx, true}

/******************************************************************************
                                 Types section
 ******************************************************************************/
/** Internal states of the link status component. */
typedef enum _NwkLinkStatusState_t
{
  NWK_LINK_STATUS_IDLE_STATE = 0x00,
  NWK_LINK_STATUS_FIRST_STATE = 0x01,
  NWK_LINK_STATUS_WAIT_STATE = NWK_LINK_STATUS_FIRST_STATE,
  NWK_LINK_STATUS_SEND_STATE = 0x02,
  NWK_LINK_STATUS_LAST_STATE
} NwkLinkStatusState_t;

/** Internal parameters of the link status component. */
typedef struct _NwkLinkStatus_t
{
 /** The timer of counting of the link status period. */
  HAL_AppTimer_t linkStatusTimer;
 /** The timer of counting between the link status Frames. */
  HAL_AppTimer_t linkFragmentationTimer;
 /** Stores the last min shortAddress in a fragmented fragmented
     linkFrame transmission. */
  ShortAddr_t lastMinAddr;
 /** Stores if the linkStatusFrame is fragmented or not. */
  bool linkFrameFragmented;
 /** Stores the remaining number of linkStatus to be sent out in case
     of fragmented transmission. */
  uint8_t linkStatusCounter;
  NwkLinkStatusState_t state; /**< Finite-state machine */
} NwkLinkStatus_t;

BEGIN_PACK
/** Command Options Field*/
typedef struct PACK _NwkLinkStatusOptions_t
{
  LITTLE_ENDIAN_OCTET(4, (
    /** This field indicates the number of link status entries */
    NwkBitField_t entryCount : 5,
    /** The first frame sub-field is set to 1 if this is the first frame of
     * the sender's link status.*/
    NwkBitField_t firstFrame : 1,
    /** The last frame sub-field is set to 1 if this is the last frame of
     * the sender's link status. */
    NwkBitField_t lastFrame  : 1,
    NwkBitField_t reserved   : 1
  ))
} NwkLinkStatusOptions_t;

/** Link Status Entry. ZigBee spec r17, 3.4.8.3.2 */
typedef struct PACK _NwkLinkStatusEntry_t
{
  LITTLE_ENDIAN_OCTET(4, (
    /** The incoming cost field contains the device's estimate of the link cost
     * for the neighbor, which is a value between 1 and 7.*/
    NwkBitField_t incomingCost : 3,
    NwkBitField_t reserved1    : 1,
    /** The outgoing cost field contains the value of the outgoing cost field
     * from the neighbor table.*/
    NwkBitField_t outgoingCost : 3,
    NwkBitField_t reserved2    : 1
  ))
} NwkLinkStatusEntry_t;

/** Link status item. ZigBee spec r17, figure 3.23. */
typedef struct PACK _NwkLinkStatusItem_t
{
  /** Each link status entry contains the network address of a router neighbor.
   **/
  ShortAddr_t addr;
  /* Link Status Entry. */
  NwkLinkStatusEntry_t linkStatus;
} NwkLinkStatusItem_t;

/** The NWK command payload of the link status command.
 * ZigBee spec r17, figure 3.21. */
typedef struct PACK _NwkLinkPayload_t
{
  NwkLinkStatusOptions_t options;
  NwkLinkStatusItem_t table[1];
} NwkLinkStatusPayload_t;
END_PACK

/******************************************************************************
                               Prototypes section
 ******************************************************************************/
#if defined(_ROUTER_) || defined(_COORDINATOR_)

/******************************************************************************
  \brief Start HAL timer for periodically send the link status.

    A ZigBee router or coordinator shall periodically send a link status
  command every CS_NWK_LINK_STATUS_FRAGMENTATION_PERIOD seconds.
  This function start the app timer.
 ******************************************************************************/
NWK_PRIVATE void nwkStartLinkFrameFragmentationTimer(void);

/******************************************************************************
  \brief Stop HAL timer for periodically send the link status.

    A ZigBee router or coordinator shall periodically send a link status
  command every CS_NWK_LINK_STATUS_FRAGMENTATION_PERIOD seconds.
  This function stops the app timer.
 ******************************************************************************/
NWK_PRIVATE void nwkStopLinkFrameFragmentationTimer(void);

/******************************************************************************
  \brief Start HAL timer for periodically send the link status.

    A ZigBee router or coordinator shall periodically send a link status
  command every nwkLinkStatusPeriod seconds. This function start the app timer.
 ******************************************************************************/
NWK_PRIVATE void nwkStartLinkStatusTimer(void);

/******************************************************************************
  \brief Stop HAL timer for periodically send the link status.

    A ZigBee router or coordinator shall periodically send a link status
  command every nwkLinkStatusPeriod seconds. This function stops the app timer.
 ******************************************************************************/
NWK_PRIVATE void nwkStopLinkStatusTimer(void);

/**************************************************************************//**
  \brief Reset the link status component
 ******************************************************************************/
NWK_PRIVATE void nwkResetLinkStatus(void);

/**************************************************************************//**
  \brief Action when link status command received

  \param[in] payload - command payload.
  \param[in] header  - raw NWK header.
  \param[in] parse   - parsed header fields.
  \return 'true' if continue processing of command packet otherwise 'false'.
 ******************************************************************************/
NWK_PRIVATE bool nwkLinkStatusInd(const uint8_t *const payload,
  const NwkFrameHeader_t *const header, const NwkParseHeader_t *const parse);

/**************************************************************************//**
  \brief Prepare header and payload of the link status command.

  \param[in] outPkt - pointer to output packet.
  \return None.
 ******************************************************************************/
NWK_PRIVATE void nwkPrepareLinkStatusTx(NwkOutputPacket_t *const outPkt);

/**************************************************************************//**
  \brief Confirmation of link status command transmission.

  \param[in] outPkt - pointer to output packet.
  \param[in] status - network status of link status transmission.
  \return None.
 ******************************************************************************/
NWK_PRIVATE void nwkConfirmLinkStatusTx(NwkOutputPacket_t *const outPkt,
  const NWK_Status_t status);

/******************************************************************************
  \brief nwkLinkStatus idle checking.

  \return true, if nwkLinkStatus performs no activity, false - otherwise.
 ******************************************************************************/
NWK_PRIVATE bool nwkLinkStatusIsIdle(void);

/******************************************************************************
  \brief Reduce the current value of link status period. Send next link status
  command as soon as possible.
 ******************************************************************************/
NWK_PRIVATE void nwkReduceCurrentLinkStatusPeriod(void);

#else /* _ROUTER_ or _COORDINATOR_ */

#define nwkResetLinkStatus() (void)0
#define nwkStartLinkStatusTimer() (void)0
#define nwkStopLinkStatusTimer() (void)0
#define nwkStartLinkFrameFragmentationTimer() (void)0
#define nwkStopLinkFrameFragmentationTimer()  (void)0
#define nwkLinkStatusInd NULL
#define nwkPrepareLinkStatusTx NULL
#define nwkConfirmLinkStatusTx NULL
#define nwkLinkStatusIsIdle NULL

#endif /* _ROUTER_ or _COORDINATOR_ */
#endif /* _NWK_LINK_STATUS_H */
/** eof nwkLinkStatus.h */

