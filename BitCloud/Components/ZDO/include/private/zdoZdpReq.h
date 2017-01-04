/**************************************************************************//**
  \file zdoZdpReq.h

  \brief The header file describes the constants, types and internal interface
  of ZDP manager. This component manages ZDP requests and responses.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2012-12-13 Max Gekk - Refactoring. Support parallel processing of incoming
                          and outgoing ZDP commands.
   Last change:
    $Id: zdoZdpReq.h 23841 2012-12-13 10:10:19Z mgekk $
 ******************************************************************************/
#if !defined _ZDO_ZDP_REQ_H
#define _ZDO_ZDP_REQ_H

/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <sysQueue.h>
#include <zdo.h>
#include <sysTimer.h>

/******************************************************************************
                                Types section
 ******************************************************************************/
/** Internal states of ZDP requests. */
typedef enum _ZdoZdpReqState_t
{
  ZDO_IDLE_ZDP_REQUEST_STATE,
  ZDO_SEND_ZDP_REQUEST_STATE,
  ZDO_WAIT_RESP_CONF_ZDP_REQUEST_STATE,
  ZDO_WAIT_CONF_ZDP_REQUEST_STATE,
  ZDO_WAIT_RESP_ZDP_REQUEST_STATE,
  ZDO_CONFIRM_MDR_ZDP_REQUEST_STATE,
  ZDO_CONFIRM_MDR1_ZDP_REQUEST_STATE,
  ZDO_CONFIRM_ZDP_REQUEST_STATE,
  ZDO_WAIT_CONFIRM_UPDATE_ZDP_REQUEST_STATE
} ZdoZdpReqState_t;

/** Internal variables of the component which processes incoming ZDP requests. */
typedef struct _ZdoZdpReq_t
{
  /** The queue of incoming ZDP requests. */
  QueueDescriptor_t queue;
  /** The time to track ttl of ZDP requests. */
  SYS_Timer_t       timer;
  /** Current ZDP Transaction Sequence Number. */
  uint8_t           seqNum;
} ZdoZdpReq_t;

/******************************************************************************
                              Prototypes section
 ******************************************************************************/
/**************************************************************************//**
  \brief Main task handler of this module.
 ******************************************************************************/
ZDO_PRIVATE void zdoZdpReqHandler(void);

/**************************************************************************//**
  \brief TTL initialization of the ZDP request.

  \param[in] zdpReq - the pointer to the ZDP request.
  \param[in] ttl - new value of the time-to-leave field of the ZDP request.

  \return None.
 ******************************************************************************/
ZDO_PRIVATE void zdoInitZdpRequestTTL(ZDO_ZdpReq_t *const zdpReq, const uint32_t ttl);

/******************************************************************************
  \brief Process an incoming ZDP command response.

    Match the incoming ZDP response with a pending ZDP request.

  \param[in] apsDataInd - pointer to APSDE-DATA indication structure of
                          received ZDP command.
  \return None.
 ******************************************************************************/
ZDO_PRIVATE void zdoProcessZdpResponse(APS_DataInd_t *apsDataInd);

/**************************************************************************//**
  \brief Resets the ZDP component.
 ******************************************************************************/
ZDO_PRIVATE void zdoZdpResetReq(void);

/******************************************************************************
  \brief Is the incoming ZDP request unicast?

  \param[in] apsDataInd - the pointer to APS_DataInd_t structure which is
                          related to the received ZDP request.

  \return true if the ZDP request is unicast packet otherwise return false.
 ******************************************************************************/
ZDO_PRIVATE bool zdoIsZdpReqUnicast(const APS_DataInd_t *const apsDataInd);

/**************************************************************************//**
  \brief Returns next ZDO frame sequence number
  \note Sequence number value will also updated internally

  \return next ZDO frame sequence number
 ******************************************************************************/
ZDO_PRIVATE uint8_t ZDO_GetNextSequenceNumber(void);

#endif /* _ZDO_ZDP_REQ_H */
/** eof zdoZdpReq.h */
