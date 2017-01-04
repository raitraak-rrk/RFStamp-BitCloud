/**************************************************************************//**
  \file nwkDataReq.h

  \brief Interface of the data request component.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2009-08-29 M. Gekk - Created.
   Last change:
    $Id: nwkDataReq.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#if !defined _NWK_DATA_REQ_H
#define _NWK_DATA_REQ_H

/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <nwkConfig.h>
#include <sysQueue.h>
#include <nwkCommon.h>
#include <nldeData.h>

/******************************************************************************
                               Define(s) section
 ******************************************************************************/
/** Transmission parameters of data packets. */
#define NWK_UNICAST_DATA_TX_PARAMETERS \
  {NWK_TX_DELAY_UNICAST_DATA, NULL, nwkConfirmDataTx, false}
#define NWK_BROADCAST_DATA_TX_PARAMETERS \
  {NWK_TX_DELAY_INITIAL_BROADCAST, NULL, nwkConfirmDataTx, false}
#define NWK_MULTICAST_MEMBER_TX_PARAMETERS \
  {NWK_TX_DELAY_INITIAL_MULTICAST, NULL, nwkConfirmDataTx, false}

/******************************************************************************
                                 Types section
 ******************************************************************************/
/** Internal states of data confirmation. */
typedef enum _NwkDataConfState_t
{
  NWK_DATA_CONF_IDLE_STATE = 0x41,
  NWK_DATA_CONF_FIRST_STATE = 0x03,
  NWK_DATA_CONF_PROCESS_STATE = NWK_DATA_CONF_FIRST_STATE,
  NWK_DATA_CONF_LAST_STATE
} NwkDataConfState_t;

/** Internal variables of this component. */
typedef struct _NwkDataConf_t
{
  QueueDescriptor_t queue;
  NwkDataConfState_t state;
} NwkDataConf_t;

/******************************************************************************
                               Prototypes section
 ******************************************************************************/
/**************************************************************************//**
  \brief Reset the data request component.
 ******************************************************************************/
NWK_PRIVATE void nwkResetDataConf(void);

/**************************************************************************//**
  \brief Main task handler of the data request component.
 ******************************************************************************/
NWK_PRIVATE void nwkDataConfTaskHandler(void);

/**************************************************************************//**
  \brief Confirmation of data transmission.

  \param[in] outPkt - pointer to output packet.
  \param[in] status - network status of data transmission.
  \return None.
 ******************************************************************************/
NWK_PRIVATE void nwkConfirmDataTx(NwkOutputPacket_t *const outPkt,
  const NWK_Status_t status);

/******************************************************************************
  \brief nwkDataReq idle checking.

  \return true, if nwkDataReq performs no activity, false - otherwise.
 ******************************************************************************/
NWK_PRIVATE bool nwkDataReqIsIdle(void);

#endif /* _NWK_DATA_REQ_H */
/** eof nwkDataReq.h */

