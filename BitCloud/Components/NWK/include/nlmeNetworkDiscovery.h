/**************************************************************************//**
  \file nlmeNetworkDiscovery.h

  \brief NLME-NETWORK-DISCOVERY interface

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2009-04-11 Max Gekk - Created
   Last change:
    $Id: nlmeNetworkDiscovery.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#if !defined _NLME_NETWORK_DISCOVERY_H
#define _NLME_NETWORK_DISCOVERY_H

/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <sysTypes.h>
#include <mac.h>
#include <nwkCommon.h>
#include <appFramework.h>

/******************************************************************************
                                Types section
 ******************************************************************************/
/**//**
 * \brief Network Descriptor Information Fields.
 * */
BEGIN_PACK
typedef struct PACK _NWK_NetworkDescriptor_t
{
  /** The 64-bit PAN identifier of the network. */
  ExtPanId_t extendedPANId;
  /** The current logical channel occupied by the network. */
  Channel_t logicalChannel;
  /** A ZigBee stack profile identifier indicating the stack profile in use in
   * the discovered network. */
  StackProfile_t stackProfile;
  /** The version of the ZigBee protocol in use in the discovered network. */
  uint8_t zigBeeVersion;
  /** This specifies how often the MAC sub-layer beacon is to be transmitted by
   * a given device on the network. */
  BeaconOrder_t beaconOrder;
  /** For beacon-oriented networks, that is, beacon order < 15, this specifies
   * the length of the active period of the superframe. */
  BeaconOrder_t superframeOrder;
  /** A value of '1' indicates that at least one ZigBee router on the network
   * currently permits joining, i.e. its NWK has been issued an NLME-PERMIT-JOINING
   * primitive and, the time limit if given, has not yet expired. */
  bool permitJoining;
  /** This value is set to '1' if the device s capable of accepting join
   * requests from router-capable devices and set to '0' otherwise. */
  bool routerCapacity;
  /** This value is set to '1' if the device is capable of accepting join
   * requests from end devices and set to '0' otherwise. */
  bool endDeviceCapacity;
} NWK_NetworkDescriptor_t;
END_PACK

/**//**
 * \brief NLME-NETWORK-DISCOVERY confirm primitive's parameters structure.
 * ZigBee Specification r17, 3.2.2.2, NLME-NETWORK-DISCOVERY.confirm
 */
typedef struct _NWK_NetworkDiscoveryConf_t
{
  /** The status of the request. Any status values returned with MLME-SCAN.confirm
   * primitive or NWK_INVALID_REQUEST_STATUS, NWK_SUCCESS_STATUS. */
  NWK_Status_t status;
  /** The number of networks discovered by the search. */
  uint8_t networkCount;
  /** Size of networkDescriptors array. It must be initialized by user.
   * Zero value is admissible. */
  uint8_t maxNetworkDescr;
  /** The array of descriptors, one for each of the networks discovered.
   * It must be initialized by user. NULL pointer is admissible. */
  NWK_NetworkDescriptor_t *networkDescriptors;
} NWK_NetworkDiscoveryConf_t;

/**//**
 * \brief NLME-NETWORK-DISCOVERY request primitive's parameters structure.
 * Zigbee Specification r17, 3.2.2.1 NLME-NETWORK-DISCOVERY.request.
 */
typedef struct _NWK_NetworkDiscoveryReq_t
{
  /** Service fields - for internal needs. */
  struct
  {
    QueueElement_t qelem; /**< link used for queuing */
    MAC_ScanReq_t macScan;
  } service;
  /** A value used to calculate the length of time to spend scanning each channel. */
  ScanDuration_t scanDuration;
  /** The mask of channels that must be scanned. */
  ChannelsMask_t scanChannels;
  /** NLME-NETWORK-DISCOVERY confirm callback function's pointer. */
  void (*NWK_NetworkDiscoveryConf)(NWK_NetworkDiscoveryConf_t *conf);
  NWK_NetworkDiscoveryConf_t confirm;
  /** Field that allows to keep entries in neighbor table during network discovery request called from application */
  bool clearNeighborTable;
} NWK_NetworkDiscoveryReq_t;

/** The beacon payload shall contain the information shown in ZigBee spec r17,
 * Table 3.56. This enables the NWK layer to provide additional information
 * to new devices that are performing network discovery and allows these new
 * devices to more efficiently select a network and a particular neighbor
 * to join. */
BEGIN_PACK
typedef struct PACK _NwkBeaconPayload_t
{
  /** This field identifies the network layer protocols in use and,
   * for purposes of this specification, shall always be set to 0,
   * indicating the ZigBee protocols. */
  uint8_t protocolId;
  struct PACK
  {
    LITTLE_ENDIAN_OCTET(2, (
      /** A ZigBee stack profile identifier. */
      NwkBitField_t stackProfile       :4,
      /** The version of the ZigBee protocol. */
      NwkBitField_t nwkProtocolVersion :4
    ))
    LITTLE_ENDIAN_OCTET(4, (
      NwkBitField_t reserved           :2,
      /** This value is set to 1 if this device is capable of accepting join
       * requests from router capable devices and is set to 0 otherwise. */
      NwkBitField_t routerCapacity     :1,
      /** The network depth of this device. A value of 0x00 indicates that
       * this device is the ZigBee coordinator for the network. */
      NwkBitField_t deviceDepth        :4,
      /** This value is set to 1 if the device is capable of accepting join
       * requests from end devices seeking to join the network and is set
       * to 0 otherwise. */
      NwkBitField_t endDeviceCapacity  :1
    ))
  } field;
  /** The globally unique ID for the PAN of which the beaconing
   * device is a member. */
  ExtPanId_t nwkExtendedPanid;
  /** This value indicates the difference in time, measured in symbols,
   * between the beacon transmission time of the device and the beacon
   * transmission time of its parent. */
  uint8_t txOffset[3];
  /** This field reflects the value of nwkUpdateId from the NIB. */
  NwkUpdateId_t updateId;
} NwkBeaconPayload_t;
END_PACK

/******************************************************************************
                              Prototypes section
 ******************************************************************************/
/**************************************************************************//**
  \brief NLME-NETWORK-DISCOVERY request primitive's prototype.

  \param[in] req - NLME-NETWORK-DISCOVERY request parameters' structure pointer.
  \return None.
 ******************************************************************************/
void NWK_NetworkDiscoveryReq(NWK_NetworkDiscoveryReq_t *const req);

#endif /* _NLME_NETWORK_DISCOVERY_H */
/** eof nlmeNetworkDiscovery.h */

