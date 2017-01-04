/**************************************************************************//**
 \file nwkConfigServer.h

 \brief The data types necessary for building of the ConfigServer.

 \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2009-04-13 Max Gekk - Created.
   Last change:
    $Id: nwkConfigServer.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#if !defined _NWK_CONFIG_SERVER_H
#define _NWK_CONFIG_SERVER_H

/******************************************************************************
                               Definitions section
 ******************************************************************************/
#ifndef NWK_PRIVATE
  #define NWK_PRIVATE
#endif /* #ifndef NWK_PRIVATE */

/******************************************************************************
                                Includes section
 ******************************************************************************/
#include <nwkTx.h>
#include <nwkPacket.h>
#include <nwkJoinInd.h>
#include <nwkRoutingTable.h>
#include <nwkRouteDiscoveryTable.h>
#include <nwkRouteDiscovery.h>
#include <nwkAddressMap.h>
#include <nwkBTT.h>
#include <nwkRouteCache.h>
#include <nwkGroup.h>
#include <nwkIB.h>
#include <nwkPassiveAck.h>
#include <nwkSecurity.h>

#endif /* _NWK_CONFIG_SERVER_H */
/** eof nwkConfigServer.h */

