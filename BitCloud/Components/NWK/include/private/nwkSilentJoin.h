/*************************************************************************//**
  \file nwkSilentJoin.h

  \brief Interface of silent join functionality.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2011-03-17 A. Taradov - Created.
   Last change:
    $Id: nwkOrphan.h 15388 2011-02-20 23:55:47Z mgekk $
 ******************************************************************************/
#if !defined _NWK_SILENT_JOIN_H
#define _NWK_SILENT_JOIN_H

/******************************************************************************
                                Includes section
 ******************************************************************************/
#include <nwkConfig.h>
#include <sysQueue.h>
#include <nlmeJoin.h>
#include <macSetGet.h>
#include <macRxEnable.h>

/******************************************************************************
                                 Types section
 ******************************************************************************/
typedef enum
{
  NWK_SILENT_JOIN_STATE_MIN = 0x15,
  NWK_SILENT_JOIN_STATE_IDLE,
  NWK_SILENT_JOIN_STATE_REQUEST_QUEUED,
  NWK_SILENT_JOIN_STATE_SET_PARAMS,
  NWK_SILENT_JOIN_STATE_MAX
} NwkSilentJoinState_t;

/** Silent join memory */
typedef struct _NwkSilentJoin_t
{
  NwkSilentJoinState_t state;

#if defined _ROUTER_ || defined _ENDDEVICE_
  /** Request queue */
  QueueDescriptor_t queue;
#endif

#if defined _ROUTER_ || defined _ENDDEVICE_
  /** Memory for mac requests */
  union
  {
    MAC_SetReq_t set;
    MAC_RxEnableReq_t rxEnable;
  } req;
#endif
} NwkSilentJoin_t;

/******************************************************************************
                             Prototypes section
 ******************************************************************************/
#if (defined(_ROUTER_) || defined(_ENDDEVICE_)) && defined(_COMMISSIONING_)
/**************************************************************************//**
  \brief Silent join module reset.
 ******************************************************************************/
NWK_PRIVATE void nwkResetSilentJoin(void);

/**************************************************************************//**
  \brief Main task handler of the silent join module.
 ******************************************************************************/
NWK_PRIVATE void nwkSilentJoinTaskHandler(void);

/**************************************************************************//**
  \brief Process silent join requests.

  \param[in] req - NLME-JOIN request parameters' structure pointer.
  \return None.
 ******************************************************************************/
NWK_PRIVATE void nwkSilentJoinReq(NWK_JoinReq_t *req);

#else

#define nwkResetSilentJoin() (void)0
#define nwkSilentJoinTaskHandler NULL

#endif /* (_ROUTER_ or _ENDDEVICE_) and _COMMISSIONING_ */
#endif /* _NWK_SILENT_JOIN_H */

/** eof nwkSilentJoin.h */

