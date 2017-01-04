/**************************************************************************//**
  \file macPoll.h

  \brief Types', constants' and functions' declarations for IEEE 802.15.4-2006
    data requesting primitives.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      18/06/07 ALuzhetsky - Created.
******************************************************************************/

#ifndef _MACPOLL_H
#define _MACPOLL_H

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
 * \brief MLME-POLL confirm primitive's parameters structure.
 * IEEE 802.15.4-2006 7.1.16.2 MLME-POLL.confirm.
 */
typedef struct
{
  //! The status of the data request.
  MAC_Status_t status;
}  MAC_PollConf_t;

/**
 * \brief MLME-POLL request primitive's parameters structure declaration.
 * IEEE 802.15.4-2006 7.1.16.1 MLME-POLL.request.
 */
typedef struct
{
  //! Service field - for internal needs.
  MAC_Service_t  service;
  //! The addressing mode of the coordinator to which the poll is intended.
  MAC_AddrMode_t coordAddrMode;
  //! The PAN identifier of the coordinator to which the poll is intended.
  PanId_t        coordPANId;
  //! The address of the coordinator to which the poll is intended.
  MAC_Addr_t     coordAddr;
  //! MLME-POLL callback function's pointer.
  void (*MAC_PollConf)(MAC_PollConf_t *conf);
  //! MLME-POLL confirm parameters' structure.
  MAC_PollConf_t confirm;
}  MAC_PollReq_t;

/**
 * \brief MLME-POLL indication primitive's parameters.
 * This primitive is not described in IEEE 802.15.4-2006. Non-standard addition.
 */
typedef struct
{
  //! The addressing mode of the device from which poll request is isued.
  MAC_AddrMode_t srcAddrMode;
  //! The PAN identifier of the device from which poll request is isued.
  PanId_t        srcPANId;
  //! The address of the device from which poll request is isued.
  MAC_Addr_t     srcAddr;
  //! LQI value measured during reception of the data request command.
  uint8_t        linkQuality;
  //! The DSN of the received data request command
  uint8_t        dsn;
  // RSSI value measured during  reception of the data request command.
  int8_t         rssi;
}  MAC_PollInd_t;

/******************************************************************************
                        Prototypes section
******************************************************************************/
/**************************************************************************//**
  \brief MLME-POLL indication primitive's prototype.
  \param indParams - MLME-POLL indication parameters' structure pointer.
  \return none.
******************************************************************************/
extern void MAC_PollInd(MAC_PollInd_t *indParams);

/**************************************************************************//**
  \brief MLME-POLL request primitive's prototype.
  \param reqParams - MLME-POLL request parameters' structure pointer.
  \return none.
******************************************************************************/
void MAC_PollReq(MAC_PollReq_t *reqParams);

#endif /* _MACPOLL_H */

// eof macPoll.h
