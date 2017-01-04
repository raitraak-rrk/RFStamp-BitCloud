/**************************************************************************//**
  \file nwkStatusInd.h

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2007-06-14 V. Panov - Created
    2009-06-16 M. Gekk - Refactoring.
   Last change:
    $Id: nwkStatusInd.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#if !defined _NWK_STATUS_IND_H
#define _NWK_STATUS_IND_H

/******************************************************************************
                             Includes section
 ******************************************************************************/
#include <nwkConfig.h>
#include <nwkCommon.h>
#include <nlmeNwkStatus.h>
#include <nwkFrame.h>
#include <nwkRx.h>

/******************************************************************************
                          Inline functions section
 ******************************************************************************/
/**************************************************************************//**
  \brief Wrapper for NLME-NWK-STATUS indication primitive.

  \param[in] shortAddr - a network address of the device associated with
     the status information.
  \param[in] status - error code associated with the status indication.
  \return None.
 ******************************************************************************/
INLINE void nwkStatusIndicate(const ShortAddr_t shortAddr,
  const NWK_StatusIndErrorCodes_t status)
{
  NWK_NwkStatusInd_t ind;

  ind.shortAddress = shortAddr;
  ind.status = status;
  NWK_NwkStatusInd(&ind);
}

/******************************************************************************
                             Prototypes section
 ******************************************************************************/
#if defined(_ROUTER_) || defined(_COORDINATOR_)
/**************************************************************************//**
  \brief NWK status command has been received.

  \param[in] payload - pointer to status code field and destination address.
  \param[in] header - NWK header of the network status command.
  \param[in] parse - parsed NWK header.
  \return 'true' if continue processing of command packet otherwise 'false'.
 ******************************************************************************/
NWK_PRIVATE bool nwkStatusFrameInd(const uint8_t *const payload,
  const NwkFrameHeader_t *const header, const NwkParseHeader_t *const parse);

#else /* _ROUTER_ or _COORDINATOR_ */
#define nwkStatusFrameInd NULL
#endif /* _ROUTER_ or _COORDINATOR_ */

#if defined NWK_ROUTING_CAPACITY && defined _NWK_MANY_TO_ONE_ROUTING_
/**************************************************************************//**
  \brief Is it the input many-to-one network status.

  \param[in] inPkt - pointer to the input packet

  \return 'true' if the input packet is network status command with status code
           equal to many-to-one route error otherwise 'false'.
 ******************************************************************************/
NWK_PRIVATE
bool nwkIsManyToOneNetworkStatus(const NwkInputPacket_t *const inPkt);

#else
#define nwkIsManyToOneNetworkStatus(inPkt) false
#endif /* NWK_ROUTING_CAPACITY and _NWK_MANY_TO_ONE_ROUTING_ */
#endif /* _NWK_STATUS_IND_H */
/** eof nwkStatusInd.h */

