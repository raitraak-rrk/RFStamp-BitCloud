/**************************************************************************//**
  \file macStart.h

  \brief Types', constants' and functions' declarations for IEEE 802.15.4-2006
    start primitives.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      18/06/07 ALuzhetsky - Created.
******************************************************************************/

#ifndef _MACSTART_H
#define _MACSTART_H

/******************************************************************************
                        Includes section
******************************************************************************/
#include <macAddr.h>
#include <macCommon.h>

/******************************************************************************
                        Defines section
******************************************************************************/

/******************************************************************************
                        Types section
******************************************************************************/
/**
 * \brief MLME-START confirm primitive's parameters structure declaration.
 * IEEE 802.15.4-2006 7.1.14.2 MLME-START.confirm.
 */
typedef struct
{
  //! The result of the attempt to start using an updated superframe configuration.
  MAC_Status_t status;
}  MAC_StartConf_t;

/**
 * \brief MLME-START request primitive's parameters structure declaration.
 * IEEE 802.15.4-2006 7.1.14.1 MLME-START.request.
 */
typedef struct
{
  //! Service field - for internal needs.
  MAC_Service_t service;
  //! The PAN identifier to be used by the device.
  PanId_t       panId;
  //! The logical channel on which to start using the new configuration.
  uint8_t       channel;
  //! The channel page on which to begin using the new configuration.
  uint8_t       page;
  // NOT used - superframe functionality is not implemented.
  //uint8_t     beaconOrder;
  // NOT used - superframe functionality is not implemented.
  //uint8_t     superframeOrder;
  //! If this value is TRUE, the device will become the PAN coordinator of a new PAN.
  bool          panCoordinator;
  // NOT used - superframe functionality is not implemented.
  //bool        batteryLifeExt;
  //! TRUE if a coordinator realignment command is to be transmitted prior to
  //! changing the superframe configuration or FALSE otherwise. TBD.
  bool          coordRealignment;
  //! MLME-START confirm callback fubction's pointer.
  void (*MAC_StartConf)(MAC_StartConf_t *conf);
  //! MLME-START confirm parameters' structure.
  MAC_StartConf_t confirm;
}  MAC_StartReq_t;


/******************************************************************************
                        Prototypes section
******************************************************************************/
/**************************************************************************//**
  \brief MLME-START request primitive's prototype.
  \param reqParams - MLME-START request parameters' structure pointer.
  \return none.
******************************************************************************/
void MAC_StartReq(MAC_StartReq_t *reqParams);

#endif /* _MACSTART_H */

// eof macStart.h
