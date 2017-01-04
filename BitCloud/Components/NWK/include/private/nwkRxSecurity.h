/**************************************************************************//**
  \file nwkRxSecurity.h

  \brief Interface of security processing of incoming packets.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2011-02-03 M. Gekk - Created.
   Last change:
    $Id: nwkRxSecurity.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#if !defined _NWK_RX_SECURITY_H
#define _NWK_RX_SECURITY_H

#if defined _SECURITY_
/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <nwkConfig.h>
#include <nwkSecurity.h>

/******************************************************************************
                              Prototypes section
 ******************************************************************************/

/**************************************************************************//**
  \brief Get network key by key sequence number.

  \return Pointer to value of a network key.
 ******************************************************************************/
NWK_PRIVATE const uint8_t* NWK_GetKey(const NWK_KeySeqNum_t seqNum);

#endif /* _SECURITY_ */
#endif /* _NWK_RX_SECURITY_H */
/** eof nwkRxSecurity.h */

