/**************************************************************************//**
  \file nlmeEdScan.h

  \brief NLME-ED-SCAN interface

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2009-04-11 Max Gekk - Created.
   Last change:
    $Id: nlmeEdScan.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#if !defined _NLME_ED_SCAN_H
#define _NLME_ED_SCAN_H

/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <appFramework.h>
#include <nwkCommon.h>

/******************************************************************************
                               Define(s) section
 ******************************************************************************/
/** Maximum number of channels which can be scanned for one request. */
#define MAX_EDSCAN_CHANNEL_AMOUNT 16U

/******************************************************************************
                                 Types section
 ******************************************************************************/
/**//**
 * \brief NLME-ED-SCAN confirm primitive's parameters structure declaration.
 * ZigBee Specification r17, 3.2.2.10 NLME-ED-SCAN.confirm
 */
typedef struct _NWK_EDScanConf_t
{
  /** The status of the request. */
  NWK_Status_t status;
  /** Indicates which channels given in the request were not scanned. */
  ChannelsMask_t unScannedChannels;
  /** Size of energyDetectList */
  uint8_t energyDetectListSize;
  /** The list of energy measurements */
  PHY_EnergyLevel_t energyDetectList[MAX_EDSCAN_CHANNEL_AMOUNT];
} NWK_EDScanConf_t;

/**//**
 * \brief NLME-ED-SCAN request primitive's parameters structure declaration.
 * Zigbee Specification r17, 3.2.2.9 NLME-ED-SCAN.request.
 */
typedef struct _NWK_EDScanReq_t
{
  /** Service fields - for internal needs. */
  struct
  {
    QueueElement_t qelem; /**< link used for queuing */
    MAC_ScanReq_t macScanReq;
  } service;
  /** Bitmask of channels are to be scanned */
  ChannelsMask_t scanChannels;
  /** A value used to calculate the length of time to spend
   * scanning each channel. */
  ScanDuration_t scanDuration;
  /** NLME-ED-SCAN confirm callback function's pointer. */
  void (*NWK_EDScanConf)(NWK_EDScanConf_t *conf);
  NWK_EDScanConf_t confirm;
} NWK_EDScanReq_t;

/******************************************************************************
                               Prototypes section
 ******************************************************************************/
/**************************************************************************//**
  \brief NLME-ED-SCAN request primitive's prototype.

  \param[in] req - NLME-ED-SCAN request parameters' structure pointer.
  \return None.
 ******************************************************************************/
void NWK_EDScanReq(NWK_EDScanReq_t *const req);

#endif /* _NLME_ED_SCAN_H */
/** eof nlmeEdScan.h */

