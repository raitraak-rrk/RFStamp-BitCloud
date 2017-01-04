/**************************************************************************//**
  \file macSuperframe.h

  \brief Superframe types' declarations.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      18/06/07 ALuzhetsky - Created.
******************************************************************************/

#ifndef _MACSUPERFRAME_H
#define _MACSUPERFRAME_H

/******************************************************************************
                        Include section.
******************************************************************************/
#include <sysTypes.h>
#include <sysEndian.h>
#include <macAddr.h>

/******************************************************************************
                        Types section.
******************************************************************************/
//! MAC GTS characteristics. IEEE 802.15.4-2006 Figure 65.
BEGIN_PACK
typedef struct PACK
{
  LITTLE_ENDIAN_OCTET(4, (
    uint8_t    length : 4,
    uint8_t direction : 1,
    uint8_t      type : 1,
    uint8_t  reserved : 2
  ))
}  MAC_GtsCharacteristics_t;
END_PACK

//! Superframe specification. IEEE 802.15.4-2006 Figure 47.
BEGIN_PACK
typedef struct PACK
{
  LITTLE_ENDIAN_OCTET(2, (
    uint16_t beaconOrder       : 4,
    uint16_t superframeOrder   : 4
  ))
  LITTLE_ENDIAN_OCTET(5, (
    uint16_t finalCapSlot      : 4,
    uint16_t battLifeExt       : 1,
    uint16_t reserved          : 1,
    uint16_t panCoordinator    : 1,
    uint16_t associationPermit : 1
  ))
}  MAC_SuperframeSpec_t;
END_PACK

//! GTS specification field. IEEE 802.15.4-2006 Figure 48.
BEGIN_PACK
typedef struct PACK
{
  LITTLE_ENDIAN_OCTET(3, (
    uint8_t count    : 3,
    uint8_t reserved : 4,
    uint8_t permit   : 1
  ))
}  MAC_GtsSpec_t;
END_PACK

//! GTS descriptor. IEEE 802.15.4-2006  Figure 50.
BEGIN_PACK
typedef struct PACK
{
  ShortAddr_t shortAddr;
  struct
  {
    LITTLE_ENDIAN_OCTET(2, (
      uint8_t startSlot : 4,
      uint8_t length    : 4
    ))
  } gts;
}  MAC_GtsDescr_t;
END_PACK

/******************************************************************************
                        Defines section.
******************************************************************************/
//! Max of GTS.
#define MAC_GTS_MAX 7

//! MAC GTS direction type. IEEE 802.15.4-2006 7.3.9.2 GTS Characteristics field.
typedef enum
{
  MAC_GTS_DIRECTION_TRANSMIT = 0,
  MAC_GTS_DIRECTION_RECEIVE  = 1
} MAC_GtsDirection_t;

#endif // _MACSUPERFRAME_H

// eof macSuperframe.h
