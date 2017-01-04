/*****************************************************************************
  \file nwkDirectJoin.h

  \brief Interface of directly join another device to network.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2010-03-17 I.Vagulin - Created.
   Last change:
    $Id: nwkDirectJoin.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#if !defined _NWK_DIRECT_JOIN_H
#define _NWKDIRECTJOIN_H

/******************************************************************************
                                Includes section
 ******************************************************************************/
#include <nwkConfig.h>
#include <sysQueue.h>

/******************************************************************************
                                 Types section
 ******************************************************************************/
/** Direct join memory */
typedef struct _NwkDirectJoin_t
{
  /** Request queue */
  QueueDescriptor_t queue;
} NwkDirectJoin_t;

/******************************************************************************
                               Prototypes section
 ******************************************************************************/
#if (defined(_ROUTER_) || defined(_COORDINATOR_)) && defined(_NWK_DIRECT_JOIN_)
/**************************************************************************//**
  \brief Reseting of the direct join module.
 ******************************************************************************/
NWK_PRIVATE void nwkResetDirectJoin(void);

/**************************************************************************//**
  \brief task handler of diret join module
 ******************************************************************************/
NWK_PRIVATE void nwkDirectJoinTaskHandler(void);
#else

#define nwkResetDirectJoin() (void)0
#define nwkDirectJoinTaskHandler NULL

#endif /* _NWK_DIRECT_JOIN_ and (_ROUTER_ or _COORDINATOR_) */
#endif /* _NWK_DIRECT_JOIN_H */
/** eof nwkDirectJoin.h */

