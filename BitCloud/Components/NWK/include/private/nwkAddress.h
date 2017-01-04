/**************************************************************************//**
  \file nwkAddress.h

  \brief Formation header file.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2009-05-12 M. Gekk - Created.
   Last change:
    $Id: nwkAddress.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#if !defined _NWK_ADDRESS_H
#define _NWK_ADDRESS_H

/******************************************************************************
                                Includes section
 ******************************************************************************/
#include <nwkConfig.h>
#include <sysTypes.h>
#include <mac.h>
#include <nwkCommon.h>
#include <nwkSystem.h>

/******************************************************************************
                               Prototypes section
 ******************************************************************************/
#if defined _COORDINATOR_ || defined _ROUTER_
/**************************************************************************//**
  \brief Allocate short address.

  \param[in] router - parameters is used if tree addressing is active.
  \param[in] addrRequested - if rejoin is used, it is address device selects for
     itself otherwise NWK_NO_SHORT_ADDR.
  \param[in] extAddr - IEEE extended address of node.
  \return Allocated address. addrRequested - is possible,
          or address allocated in little endian.
 ******************************************************************************/
NWK_PRIVATE ShortAddr_t nwkAllocateShortAddr(const bool router,
  const ShortAddr_t addrRequested, const ExtAddr_t extAddr);
#endif /* _COORDINATOR_ or _ROUTER_ */

#if defined _NWK_DISTRIBUTED_ADDRESSING_
/**************************************************************************//**
  \brief Computing the size of the address sub-block.

  \param[in] depth - current depth of node in network tree topology.
  \return cskip block in big endian format.
 ******************************************************************************/
NWK_PRIVATE NwkAddrRange_t nwkCSkip(const NwkDepth_t depth);
#endif /* _NWK_DISTRIBUTED_ADDRESSING_ */

/**************************************************************************//**
  \brief Allocate non-conflict short address

  \param[in] shortAddr - initial value of short address.
  \param[in] extAddr - pointer to IEEE extended address of node for which is
                       allocated a short address.
  \return Valid short address in little endian format.
 ******************************************************************************/
NWK_PRIVATE ShortAddr_t nwkAllocNonConflAddr(ShortAddr_t shortAddr,
  const ExtAddr_t *const extAddr);

/**************************************************************************//**
  \brief Is received broadcast packet for this device or not?

  \param[in] dstAddr - destination broadcast address.

  \return 'true' if broadcast packet can be handled by this device
          otherwise return 'false'.
 ******************************************************************************/
NWK_PRIVATE bool nwkBroadcastForMe(const ShortAddr_t dstAddr);

#endif /* _NWK_ADDRESS_H */
/** eof nwkAddress.h */

