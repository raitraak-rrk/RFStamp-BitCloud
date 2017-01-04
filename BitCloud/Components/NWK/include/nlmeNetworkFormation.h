/**************************************************************************//**
  \file nlmeNetworkFormation.h

  \brief NLME-NETWORK-FORMATION interface

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2009-04-11 Max Gekk - Created.
   Last change:
    $Id: nlmeNetworkFormation.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#if !defined _NLME_NETWORK_FORMATION_H
#define _NLME_NETWORK_FORMATION_H

/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <mac.h>
#include <nwkCommon.h>
#include <nlmeEdScan.h>

/******************************************************************************
                                Types section
 ******************************************************************************/
/**//**
 * \brief NLME-NETWORK-FORMATION confirm primitive's parameters structure.
 * ZigBee Specification r17, 3.2.2.3, NLME-NETWORK-FORMATION.confirm
 */
typedef struct _NWK_NetworkFormationConf_t
{
  /** The status of the request. */
  NWK_Status_t status;
} NWK_NetworkFormationConf_t;

/**//**
 * \brief NLME-NETWORK-FORMATION request primitive's parameters structure.
 * Zigbee Specification r17, 3.2.2.3 NLME-NETWORK-FORMATION.request.
 */
typedef struct _NWK_NetworkFormationReq_t
{
  /** Service fields - for internal needs. */
  struct
  { /** Link to a next network formation request in queue. */
    QueueElement_t qelem;
    union
    {
      struct
      {
        MAC_ScanReq_t req;
        PHY_EnergyLevel_t energyList[MAX_EDSCAN_CHANNEL_AMOUNT];
      } scan;
      MAC_SetReq_t setReq;
      MAC_StartReq_t startReq;
      MAC_RxEnableReq_t rxEnableReq;
    } mac;
  } service;
  /** This parameter controls the method of network forming. 
   * Valid values of the method field are: 
   *   \li NWK_JOIN_VIA_FORMING
   *   \li NWK_JOIN_VIA_COMMISSIONING */
  NWK_JoinControl_t ctrl;
  /** Channels are to be scanned in preparation for starting a network. */
  ChannelsMask_t scanChannels;
  /** The time spent scanning each channel is
   *  (aBaseSuperframeDuration * (2n + 1)) symbols,
   * where n is the value of the ScanDuration parameter. */
  ScanDuration_t scanDuration;
  /** This field shall contain the device capability information.
   * See ZigBee spec r18, Table 3.44, page 345. */
  MAC_CapabilityInf_t capabilityInformation;
  /** NLME-NETWORK-FORMATION confirm callback function's pointer. */
  void (*NWK_NetworkFormationConf)(NWK_NetworkFormationConf_t *conf);
  NWK_NetworkFormationConf_t confirm;
} NWK_NetworkFormationReq_t;

/******************************************************************************
                               Prototypes section
 ******************************************************************************/
/**************************************************************************//**
  \brief NLME-NETWORK-FORMATION request primitive's prototype.

  \param[in] req - NLME-NETWORK-FORMATION request parameters' structure pointer.
  \return None.
 ******************************************************************************/
void NWK_NetworkFormationReq(NWK_NetworkFormationReq_t *const req);

#endif /* _NLME_NETWORK_FORMATION_H */
/** eof nlmeNetworkFormation.h */

