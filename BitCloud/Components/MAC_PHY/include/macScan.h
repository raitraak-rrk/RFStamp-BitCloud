/**************************************************************************//**
  \file macScan.h

  \brief Types', constants' and functions' declarations for IEEE 802.15.4-2006
    scanning primitives.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      18/06/07 ALuzhetsky - Created.
******************************************************************************/

#ifndef _MACSCAN_H
#define _MACSCAN_H

/******************************************************************************
                        Includes section
******************************************************************************/
#include <phy.h>
#include <macAddr.h>
#include <macBeacon.h>
#include <macCommon.h>

/******************************************************************************
                        Defines section
******************************************************************************/
//! Set of MAC scanning types. IEEE 802.15.4-2006 Table 67.
typedef enum
{
  MAC_ED_SCAN      = 0x00,
  MAC_ACTIVE_SCAN  = 0x01,
  MAC_PASSIVE_SCAN = 0x02,
  MAC_ORPHAN_SCAN  = 0x03
} MAC_ScanType_t;


/******************************************************************************
                        Types section
******************************************************************************/
/**
 * \brief Scanning results union. Contains results of energy scanning
 * procedure and passive/active scan procedures.
 */
typedef union
{
  //! Energy scanning procedure results.
  PHY_EnergyLevel_t   (*energy)[];
  //! Active/passive scanning procedure results.
  MAC_PanDescriptor_t (*panDescr)[];
}  MAC_ScanResult_t;

/**
 * \brief MLME-SCAN confirm primitive's parameters structure declaration.
 * IEEE 802.15.4-2006 7.1.11.2 MLME-SCAN.confirm
 */
typedef struct
{
  //! The status of the scan request.
  MAC_Status_t     status;
  //! Indicates the type of scan performed:
  MAC_ScanType_t   type;
  //! The channel page on which the scan was performed TBD
  uint8_t          page;
  //! Indicates which channels given in the request were not scanned.
  uint32_t         unScannedChannels;
  //! The number of elements returned in the appropriate result lists.
  uint8_t          resultSize;
  //! EnergyDetectList and PANDescriptorList union. Non-standard addition.
  MAC_ScanResult_t result;
}  MAC_ScanConf_t;

/**
 * \brief MLME-SCAN request primitive's parameters structure declaration.
 * IEEE 802.15.4-2006 7.1.11.1 MLME-SCAN.request.
 */
typedef struct
{
  //! Service field - for internal needs.
  MAC_Service_t  service;
  //! Indicates the type of scan performed.
  MAC_ScanType_t type;
  //! The 27 bits indicate which channels are to be scanned.
  uint32_t       channels;
  //! A value used to calculate the length of time to spend scanning each channel
  //! for ED, active, and passive scans. This parameter is ignored for orphan scans.
  uint8_t        duration;
  //! The channel page on which to perform the scan.
  uint8_t        page;
  //! Non-standard addition. Max number of scan results.
  uint8_t        maxResultSize;
  // MLME-SCAN confirm callback function's pointer.
  void (*MAC_ScanConf)(MAC_ScanConf_t *conf);
  // MLME-SCAN confirm parameters structure.
  MAC_ScanConf_t confirm;
}  MAC_ScanReq_t;


/******************************************************************************
                        Prototypes section
******************************************************************************/
/**************************************************************************//**
  \brief MLME-SCAN request primitive's prototype.
  \param reqParams - MLME-SCAN request parameters' structure pointer.
  \return none.
******************************************************************************/
void MAC_ScanReq(MAC_ScanReq_t *reqParams);

#endif /* _MACSCAN_H */

// eof macScan.h
