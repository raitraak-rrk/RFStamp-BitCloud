/*************************************************************************//**
  \file nwkOrphan.h

  \brief Interface of orphan join functionality.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2010-03-17 I.Vagulin - Created.
   Last change:
    $Id: nwkOrphan.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#if !defined _NWK_ORPHAN_H
#define _NWK_ORPHAN_H

/******************************************************************************
                                Includes section
 ******************************************************************************/
#include <nwkConfig.h>
#include <sysQueue.h>
#include <nlmeJoin.h>
#include <macOrphan.h>
#include <macScan.h>
#include <macSetGet.h>
#include <macRxEnable.h>

/******************************************************************************
                                 Types section
 ******************************************************************************/
typedef enum
{
  NWK_ORPHAN_STATE_MIN = 0x65,
  NWK_ORPHAN_STATE_IDLE,

  /* child */
  NWK_ORPHAN_STATE_REQUEST_QUEUED,
  NWK_ORPHAN_STATE_SET_EXT_ADDR,
  NWK_ORPHAN_STATE_WAIT_SCAN_CONFIRM,
  NWK_ORPHAN_STATE_SET_PARAMS,
  NWK_ORPHAN_STATE_GET_PARAMS,

  /* parent */
  NWK_ORPHAN_STATE_WAIT_RESPONSE_CONFIRM,

  NWK_ORPHAN_STATE_MAX
} NwkOrphanState_t ;

/** Orphan memory */
typedef struct _NwkOrphan_t
{
  NwkOrphanState_t state;

#if defined _ROUTER_ || defined _ENDDEVICE_
  /** Request queue */
  QueueDescriptor_t queue;
  /** joinReq currently in processing */
  NWK_JoinReq_t *current;
  /** Memory for mac requests */
#endif
  union
  {
#if defined _ROUTER_ || defined _COORDINATOR_
    MAC_OrphanResp_t resp;
#endif
#if defined _ROUTER_ || defined _ENDDEVICE_
    MAC_ScanReq_t scan;
    MAC_GetReq_t get;
    MAC_SetReq_t set;
    MAC_RxEnableReq_t rxEnable;
#endif
  } req ;
} NwkOrphan_t;

/******************************************************************************
                             Prototypes section
 ******************************************************************************/
#if (defined(_ROUTER_) || defined(_ENDDEVICE_)) && defined(_NWK_ORPHAN_JOIN_)
/**************************************************************************//**
  \brief Reseting of the orphan module.
 ******************************************************************************/
NWK_PRIVATE void nwkResetOrphan(void);

/**************************************************************************//**
  \brief Main task handler of orphan module.
 ******************************************************************************/
NWK_PRIVATE void nwkOrphanTaskHandler(void);

/**************************************************************************//**
  \brief Process orphan join requests.

  \param[in] req - NLME-JOIN request parameters' structure pointer.
  \return None.
 ******************************************************************************/
NWK_PRIVATE void nwkOrphanReq(NWK_JoinReq_t *req);

#else

#define nwkResetOrphan() (void)0
#define nwkOrphanTaskHandler NULL

#endif /* (_ROUTER_ or _ENDDEVICE_) and _NWK_ORPHAN_JOIN_ */
#endif /* _NWK_ORPHAN_H */
/** eof nwkOrphan.h */

