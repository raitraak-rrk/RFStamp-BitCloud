/**************************************************************************//**
  \file nlmeJoin.h

  \brief NLME-JOIN interface

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2009-04-11 Max Gekk - Created.
   Last change:
    $Id: nlmeJoin.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#if !defined _NLME_JOIN_H
#define _NLME_JOIN_H

/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <mac.h>
#include <nwkCommon.h>

/******************************************************************************
                                Types section
 ******************************************************************************/
/**//**
 * \brief NLME-JOIN confirm primitive's parameters structure declaration.
 * ZigBee Specification r17, 3.2.2.13 NLME-JOIN.confirm, page 285.
 */
typedef struct _NWK_JoinConf_t
{
  /** The status of the corresponding request. Valid Range: NWK_SUCCESS_STATUS,
   * NWK_NOT_PERMITTED_STATUS, NWK_NO_NETWORKS_STATUS or any status value
   * returned from MLME-ASSOCIATE.confirm, MLME-SET.confirm, MLME-GET.confirm,
   * MLME-RX-ENABLE.confirm, NLME-NETWORK-DISCOVERY.confirm. */
  NWK_Status_t status;
  /** The 16-bit network address that was allocated to this device. This parameter
   * will be equal to 0xffff if the join attempt was unsuccessful. */
  ShortAddr_t networkAddress;
  /** The 64-bit extended PAN identifier for the network of which the device is
   * now a member. */
  ExtPanId_t extendedPANId;
  /** The value of phyCurrentChannel attribute of the PHY PIB, which is equal to
   * the current channel of the network that has been joined. */
  Channel_t activeChannel;
} NWK_JoinConf_t;

/**//**
 * \brief NLME-JOIN request primitive's parameters structure declaration.
 * Zigbee Specification r17, 3.2.2.11 NLME-JOIN.request, page 281.
 */
typedef struct _NWK_JoinReq_t
{
  /** Service fields - for internal needs. */
  struct
  {
    QueueElement_t qelem; /**< link used for queuing */
  } service;
  /** The 64-bit PAN identifier of the network to join.*/
  ExtPanId_t extendedPANId;
  /** This parameter controls the method of network joining.
   * Valid values of the method field are:
   *   \li NWK_JOIN_VIA_COMMISSIONING
   *   \li NWK_JOIN_VIA_ASSOCIATION
   *   \li NWK_JOIN_VIA_REJOIN
   *   \li NWK_JOIN_VIA_ORPHAN_SCAN */
  NWK_JoinControl_t ctrl;
  /** The 27 least significant bits (b0, b1,... b26) indicate which channels
   * are to be scanned (1=scan, 0=do not scan) for each of the 27 valid channels.
   **/
  ChannelsMask_t scanChannels;
  /** A value used to calculate the length of time to spend scanning each
   * channel. The time spent scanning each channel is
   *        (aBaseSuperframeDuration * (2^n + 1)) symbols,
   * where n is the value of the ScanDuration parameter. */
  ScanDuration_t scanDuration;
  /** This field shall contain the device capability information.
   * See ZigBee spec r18, Table 3.44, page 345. */
  MAC_CapabilityInf_t capabilityInformation;
  /** NLME-JOIN confirm callback function's pointer. */
  void (*NWK_JoinConf)(NWK_JoinConf_t *conf);
  NWK_JoinConf_t confirm;
} NWK_JoinReq_t;

/**//**
 * \brief NLME-JOIN indication primitive's parameters structure declaration.
 * Zigbee Specification r17, 3.2.2.12 NLME-JOIN.indication, page 283.
 */
typedef struct _NWK_JoinInd_t
{
  /** The network address of an entity that has been added to the network. */
  ShortAddr_t networkAddress;
  /** The 64-bit IEEE address of an entity that has been added to the network.*/
  ExtAddr_t extendedAddress;
  /** Specifies the operational capabilities of the joining device. */
  MAC_CapabilityInf_t capabilityInformation;
  /** This parameter indicating the method used to join the network. */  
  NWK_JoinControl_t ctrl;
} NWK_JoinInd_t;

/******************************************************************************
                              Prototypes section
 ******************************************************************************/
/**************************************************************************//**
  \brief NLME-JOIN request primitive's prototype.

  \param[in] req - NLME-JOIN request parameters' structure pointer.
  \return None.
 ******************************************************************************/
void NWK_JoinReq(NWK_JoinReq_t *const req);

/**************************************************************************//**
  \brief NLME-JOIN indication primitive's prototype.

  \param[in] ind - NLME-JOIN indication parameters' structure pointer.
  \return None.
 ******************************************************************************/
extern void NWK_JoinInd(NWK_JoinInd_t *ind);

#endif /* _NLME_JOIN_H */
/** eof nlmeJoin.h */

