/**************************************************************************//**
  \file macOrphan.h

  \brief Types', constants' and functions' declarations for IEEE 802.15.4-2006
    orphan primitives.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      18/06/07 ALuzhetsky - Created.
******************************************************************************/

#ifndef _MACORPHAN_H
#define _MACORPHAN_H

/******************************************************************************
                        Includes section.
******************************************************************************/
#include <macAddr.h>
#include <macCommon.h>
#include <macCommStatus.h>

/******************************************************************************
                        Definitions section.
******************************************************************************/

/******************************************************************************
                        Types section.
******************************************************************************/
//! MLME-ORPHAN indication primitive's parameters. IEEE 802.15.4-2006 7.1.8.1
typedef struct
{
  //! The address of the orphaned device.
  ExtAddr_t extAddr;
}  MAC_OrphanInd_t;

//! MLME-ORPHAN response pritive's parameters. IEEE 802.15.4-2006 7.1.8.2
typedef struct
{
  //! Service field - for internal needs.
  MAC_Service_t service;
  //! The address of the orphaned device.
  ExtAddr_t     extAddr;
  //! The 16-bit short address allocated to the orphaned device if it is
  //! associated with this coordinator.
  ShortAddr_t   shortAddr;
  //! TRUE if the orphaned device is associated with this coordinator or FALSE otherwise.
  bool          associate;
  //! MLME-ORPHAN response callback function pointer.
  void (*MAC_CommStatusInd)(MAC_CommStatusInd_t *commStat);
  // MLME-COMM_STATUS parameters' structure.
  MAC_CommStatusInd_t commStatus;
}  MAC_OrphanResp_t;


/******************************************************************************
                        Prototypes section
******************************************************************************/
/**************************************************************************//**
  \brief MLME-ORPHAN indication primitive's prototype.
  \param indParams - MLME-ORPHAN indication parameters' structure pointer.
  \return none.
******************************************************************************/
extern void MAC_OrphanInd(MAC_OrphanInd_t *indParams);

/**************************************************************************//**
  \brief  MLME-ORPHAN response primitive's prototype.
  \param respParams - MLME-ORPHAN response parameters' structure pointer.
  \return none.
******************************************************************************/
void MAC_OrphanResp(MAC_OrphanResp_t *respParams);

#endif // _MACORPHAN_H

// eof macOrphan.h
