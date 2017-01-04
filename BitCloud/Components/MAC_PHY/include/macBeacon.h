/**************************************************************************//**
  \file macBeacon.h

  \brief Beacon specific types declarations and prototypes.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      18/06/07 ALuzhetsky - Created.
******************************************************************************/

#ifndef _MACBEACON_H
#define _MACBEACON_H

/******************************************************************************
                        Includes section.
******************************************************************************/
#include <sysEndian.h>
#include <macSuperframe.h>

/******************************************************************************
                        Definitions section.
******************************************************************************/
#define MAC_MAX_PENDING_ADDRESS_COUNT 7
#define MAC_FINAL_CAP_SLOT 15

/******************************************************************************
                        Types section.
******************************************************************************/
/**
  \brief Pending address specification fields.
  IEEE 802.15.4-2006 7.2.2.1.6 Pending Address Specification field.
*/
BEGIN_PACK
typedef struct PACK
{
  LITTLE_ENDIAN_OCTET(4, (
    uint8_t shortAddrNum : 3,
    uint8_t reserved1    : 1,
    uint8_t extAddrNum   : 3,
    uint8_t reserved2    : 1
  ))
}  MAC_PendingAddrSpec_t;
END_PACK

/**
  \brief PAN descriptor.
   IEEE 802.15.4-2006 Table 55.
*/
typedef struct
{
  MAC_AddrMode_t       coordAddrMode;
  PanId_t              coordPANId;
  MAC_Addr_t           coordAddr;
  uint8_t              channel;
  uint8_t              page;
  MAC_SuperframeSpec_t superframeSpec;
  bool                 gtsPermit;
  uint8_t              quality;
  int8_t               rssi; // Additional parameter (NOT described in the standard)
                             // RSSI value while beacon frame receiving procedure.
  uint32_t             timeStamp;
}  MAC_PanDescriptor_t;

/**
  \brief Parameters of BEACON indication.
  IEEE 802.15.4-2006 7.1.5.1 MLME-BEACON-NOTIFY.indication.
*/
typedef struct
{
  //! The beacon sequence number.
  uint8_t               bsn;
  //! The PANDescriptor for the received beacon.
  MAC_PanDescriptor_t   panDescr;
  //! The beacon pending address specification.
  MAC_PendingAddrSpec_t pendAddrSpec;
  //! List of short addresses.
  ShortAddr_t           (*shortPendAddrList)[];
  //! List of extended addresses.
  ExtAddr_t             (*extPendAddrList)[];
  //! The number of octets contained in the beacon payload of the beacon frame
  //! received by the MAC sublayer.
  uint8_t               msduLength;
  //! The set of octets comprising the beacon payload to be transferred from the
  //! MAC sublayer entity to the next higher layer.
  uint8_t               *msdu;
}  MAC_BeaconNotifyInd_t;


/******************************************************************************
                        Prototypes section
******************************************************************************/
/**************************************************************************//**
  \brief MLME-BEACON_NOTIFY indication primitive's prototype.
  \param indParams - MLME-BEACON_NOTIFY indication parameters' structure pointer.
  \return none.
******************************************************************************/
extern void MAC_BeaconNotifyInd(MAC_BeaconNotifyInd_t *indParams);

#endif //_MACBEACON_H

// eof macBeacon.h
