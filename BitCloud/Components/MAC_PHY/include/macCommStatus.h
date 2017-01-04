/**************************************************************************//**
  \file macCommStatus.h

  \brief Types', constants' and functions' declarations for IEEE 802.15.4-2006
    comm status primitives.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      18/06/07 ALuzhetsky - Created.
******************************************************************************/

#ifndef _MACCOMMSTATUS_H
#define _MACCOMMSTATUS_H

/******************************************************************************
                        Includes section.
******************************************************************************/
#include <macAddr.h>
#include <macCommon.h>

/******************************************************************************
                        Definitions section.
******************************************************************************/

/******************************************************************************
                        Types section.
******************************************************************************/

/**
 * \brief MLME-COMM_STATUS indication primitive's parameters.
 * IEEE 802.15.4-2006 7.1.12.1 MLME-COMM-STATUS.indication.
*/
typedef struct
{
  //! The 16-bit PAN identifier of the device from which the frame was received
  //! or to which the frame was being sent.
  PanId_t        panId;
  //! The source addressing mode for this primitive.
  MAC_AddrMode_t srcAddrMode;
  //! The individual device address of the entity from which the frame causing
  //! the error originated.
  MAC_Addr_t     srcAddr;
  //! The destination addressing mode for this primitive.
  MAC_AddrMode_t dstAddrMode;
  //! The individual device address of the device for which the frame was intended.
  MAC_Addr_t     dstAddr;
  //! The communications status.
  MAC_Status_t   status;
}  MAC_CommStatusInd_t;


/******************************************************************************
                        Prototypes section
******************************************************************************/
#ifdef _ASYNC_COMM_STATUS_
/**************************************************************************//**
  \brief This function is called if any error is occured during frame processing.

    Memory which was allocated for parameters will be free immediately after
  return from this function.

  \param commStat - MLME-COMM_STATUS indication parameters' structure pointer.
  \return none.
******************************************************************************/
void MAC_CommStatusInd(MAC_CommStatusInd_t *commStat);
#endif //_ASYNC_COMM_STATUS_
#endif //_MACCOMMSTATUS_H

// eof macCommStatus.h
