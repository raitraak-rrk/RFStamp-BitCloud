/**************************************************************************//**
  \file nwkManyToOne.h

  \brief Internal interface of the many-to-one component.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2010-05-13 M. Gekk - Created.
   Last change:
    $Id: nwkManyToOne.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#if !defined _NWK_MANY_TO_ONE_H
#define _NWK_MANY_TO_ONE_H

/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <nwkConfig.h>
#include <nwkCommon.h>
#include <nwkRoutingTable.h>
#include <sysQueue.h>

/******************************************************************************
                               Prototypes section
 ******************************************************************************/
#if defined NWK_ROUTING_CAPACITY && defined _NWK_MANY_TO_ONE_ROUTING_
/**************************************************************************//**
  \brief Main task handler of the many-to-one component.
 ******************************************************************************/
NWK_PRIVATE void nwkManyToOneTaskHandler(void);

/**************************************************************************//**
  \brief Reset the many-to-one component.
 ******************************************************************************/
NWK_PRIVATE void nwkResetManyToOne(void);

/**************************************************************************//**
  \brief Many-to-one route request from concentrator has been received.

  \param[in] entry - pointer to concentrator entry in the routing table.
  \return None.
 ******************************************************************************/
NWK_PRIVATE void nwkNewConcentratorFound(NwkRoutingTableEntry_t *const entry);

#else

#define nwkManyToOneTaskHandler NULL
#define nwkResetManyToOne() (void)0
#define nwkNewConcentratorFound(entry) (void)0

#endif /* NWK_ROUTING_CAPACITY and _NWK_MANY_TO_ONE_ROUTING_ */
#endif /* _NWK_MANY_TO_ONE_H */
/** eof nwkManyToOne.h */

