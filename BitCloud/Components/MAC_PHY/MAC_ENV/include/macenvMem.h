/**************************************************************************//**
  \file macenvMem.h

  \brief MAC and PHY PIB memory.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      23/04/08 A. Mandychev - Created.
******************************************************************************/

#ifndef _MACENVMEM_H
#define _MACENVMEM_H

/******************************************************************************
                    Includes section
******************************************************************************/
#include <sysTypes.h>
#include <phy.h>
#include <macAddr.h>

/******************************************************************************
                    Define(s) section
******************************************************************************/

/******************************************************************************
                    Types section
******************************************************************************/
//! PHY PIB attributes.
typedef struct
{
  uint8_t         channel;
  uint32_t        channelsSupported; // Just one row for current page.
  int8_t          transmitPower;
  PHY_CcaMode_t   ccaMode;
  uint8_t         currentPage;
  uint16_t        maxFrameDuration;
  uint8_t         shrDuration;
  uint8_t         symbolsPerOctet;
} PhyPibAttr_t;

//! MAC PIB attributes.
typedef struct
{
  uint32_t        ackWaitDuration;
  bool            associatedPanCoord;
#ifdef _FFD_
  bool            associationPermit;
#endif // _FFD_
  bool            autoRequest;
  //bool          battLifeExt;
  //uint8_t       battLifeExtPeriods;
#ifdef _FFD_
  uint8_t         *beaconPayload;
  uint8_t         beaconPayloadLength;
  //uint8_t       beaconOrder;
  //uint32_t      beaconTxTime;
  uint8_t         bsn;
#endif // _FFD_
  ExtAddr_t       coordExtAddr;
  ShortAddr_t     coordShortAddr;
  uint8_t         dsn;
  //bool          gtsPermit;
  uint8_t         maxBe;
  uint8_t         maxCsmaBackoffs;
  //uint8_t       maxFrameTotalWaitTime;
  uint8_t         maxFrameRetries;
  uint8_t         minBe;
  //uint8_t       minLifsPeriod;
  //uint8_t       minSifsPeriod;
  PanId_t         panId;
#ifdef _FFD_
  //bool          promiscuousMode;
#endif // _FFD_
  uint8_t         responseWaitTime;
  bool            rxOnWhenIdle;
  bool            securityEnabled;
  ShortAddr_t     shortAddr;
#ifdef _FFD_
  //uint8_t       superframeOrder;
  //uint8_t       syncSymbolOffset;
#endif // _FFD_
  //bool          timestampSupported;
#ifdef _FFD_
  //uint16_t        transactionPersistenceTime;
#endif // _FFD_
  /************************************************************/
  // Additional attributes. NOT described in the standard.
  ExtAddr_t       extAddr;  // Device own extended address.
  bool            panCoordinator;
  // For PanServer needs.
#ifdef _FFD_
  bool            beaconTxEnable;
#endif //_FFD_
/** Duration in milliseconds of maximum length frame transferring. */
  uint8_t         maxFrameTransmissionTime;
  uint8_t         ccaEdThres;
} MacPibAttr_t;

//! MAC and PHY PIB attributes.
typedef struct
{
  MacPibAttr_t macAttr;
  PhyPibAttr_t phyAttr;
} PIB_t;

#endif /*_MACENVMEM_H*/

// eof macphyPibMem.h
