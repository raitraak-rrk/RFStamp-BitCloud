/**************************************************************************//**
  \file nwk.h

  \brief Interface of NWK layer.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2009-04-13 Max Gekk - Created.
   Last change:
    $Id: nwk.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#if !defined _NWK_H
#define _NWK_H

/******************************************************************************
                                 Includes section
 ******************************************************************************/
#include <nwkCommon.h>

#include <nldeData.h>
#include <nlmeDirectJoin.h>
#include <nlmeEdScan.h>
#include <nlmeJoin.h>
#include <nlmeLeave.h>
#include <nlmeNetworkDiscovery.h>
#include <nlmeNetworkFormation.h>
#include <nlmeNwkStatus.h>
#include <nlmePermitJoining.h>
#include <nlmeReset.h>
#include <nlmeRouteDiscovery.h>
#include <nlmeStartRouter.h>
#include <nlmeSync.h>
#include <nlmeSyncLoss.h>

/** Extra network headers */
#include <nwkAttributes.h>
#include <nwkSecurity.h>
#include <nwkAddressResolv.h>
#include <nwkNeighbor.h>
#include <nwkRouteInfo.h>
#include <nwkGroup.h>
#include <nwkRouteTable.h>

/******************************************************************************
                              Prototypes section
 ******************************************************************************/
/**************************************************************************//**
  \brief NWK_Init shall be called only by APS and only once at startup.

  \return None.
 ******************************************************************************/
void NWK_Init(void);

/**************************************************************************//**
  \brief The stack don't sleep if this function return 'true'.

  \return 'true' if the network layer if active otherwise 'false'.
 ******************************************************************************/
bool NWK_IsActiveTransaction(void);

/**************************************************************************//**
  \brief NWK layer activities launching
 ******************************************************************************/
void NWK_LaunchActivities(void);

/**************************************************************************//**
  \brief NWK layer activities stopping
 ******************************************************************************/
void NWK_StopActivities(void);

/**************************************************************************//**
  \brief Determine if node has all the parameters required to operate in network.

  \return True, if node has all the parameters required to operate in network;
          false - otherwise.
 ******************************************************************************/
bool NWK_DeviceIsAbleToOperateInNetwork(void);

/******************************************************************************
  \brief Force LinkStatus command transmission.
 ******************************************************************************/
void NWK_ForceLinkStatusSending(void);

#endif  /* _NWK_H */
/** eof nwk.h */

