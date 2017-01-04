/**************************************************************************//**
  \file nwkTxDelay.h

  \brief Interface of transmission delays.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2009-09-20 Max Gekk - Created.
   Last change:
    $Id: nwkTxDelay.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#if !defined _NWK_TX_DELAY_H
#define _NWK_TX_DELAY_H

/******************************************************************************
                                Includes section
 ******************************************************************************/
#include <nwkConfig.h>
#include <appTimer.h>
#include <sysQueue.h>
#include <nldeData.h>

/******************************************************************************
                                 Types section
 ******************************************************************************/
/** Internal variables of delay component. */
typedef struct _NwkTxDelay_t
{
  bool isTimerStarted;
  HAL_AppTimer_t timer;
  QueueDescriptor_t queue;
} NwkTxDelay_t;

/******************************************************************************
                              Prototypes section
 ******************************************************************************/
/**************************************************************************//**
  \brief Transmission delay request.

  \param[in] delayReq - pointer to delay request's parameters.
  \param[in] macStatus - pointer to MAC status of MLDE-DATA.confirm or NULL if
    it is first transmission.
  \return further behavior.
 ******************************************************************************/
NWK_PRIVATE NwkTxDelayStatus_t nwkTxDelayReq(NwkTxDelayReq_t *const delayReq,
  MAC_Status_t *const macStatus);

/**************************************************************************//**
  \brief The transmission delay has expired.

  \param[in] delayReq - pointer to delay request's parameters.
  \param[in] status - status of further behavior.
  \return None.
 ******************************************************************************/
NWK_PRIVATE void nwkTxDelayConf(NwkTxDelayReq_t *const delayReq,
  const NwkTxDelayStatus_t status);

/**************************************************************************//**
  \brief Flush all delays.
 ******************************************************************************/
NWK_PRIVATE void nwkFlushTxDelays(void);

/**************************************************************************//**
  \brief Reset delay component.
 ******************************************************************************/
NWK_PRIVATE void nwkResetTxDelays(void);

/******************************************************************************
  \brief nwkTxDelay idle checking.

  \return true, if nwkTxDelay performs no activity, false - otherwise.
 ******************************************************************************/
NWK_PRIVATE bool nwkTxDelayIsIdle(void);

/**************************************************************************//**
  \brief Delivery time of broadcast data transmission on a network.

  \return Current value of broadcast delivery time in milliseconds.
 ******************************************************************************/
NWK_PRIVATE uint32_t nwkGetBroadcastDeliveryTime(void);

#if defined _NWK_PASSIVE_ACK_ && (defined _ROUTER_ || defined _COORDINATOR_)
/**************************************************************************//**
  \brief All expected passive acks are received, confirm the passive ack req.

  \param[in] delayReq - pointer to delay request's parameters.
  \return None.
 ******************************************************************************/
NWK_PRIVATE void nwkPassiveAckConf(NwkTxDelayReq_t *const delayReq);
#endif /* _NWK_PASSIVE_ACK_ */

#endif /* _NWK_TX_DELAY_H */
/** eof nwkTxDelay.h */

