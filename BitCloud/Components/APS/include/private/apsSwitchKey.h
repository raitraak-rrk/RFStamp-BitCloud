/**************************************************************************//**
  \file apsSwitchKey.h

  \brief Private interface of APS Switch Key.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2011-03-21 A. Razinkov - Created.
   Last change:
    $Id: apsSwitchKey.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#ifndef _APS_SWITCH_KEY_H
#define _APS_SWITCH_KEY_H

#ifdef _SECURITY_
/******************************************************************************
                                Includes section
 ******************************************************************************/
#include <apsCommand.h>
#include <nwk.h>
#include <apsFrames.h>
#include <apsmeSwitchKey.h>

/******************************************************************************
                                 Types section
 ******************************************************************************/
BEGIN_PACK
/** Type of APS remove device command frame. */
typedef struct PACK _ApsSwitchKeyFrame_t
{
  /** An APS header, which comprises frame control and addressing information. */
  ApduCommandHeader_t header;
  /** A sequence number assigned to a network key by the Trust Center and used to
   *  distinguish network keys. */
  uint8_t keySeqNumber;
} ApsSwitchKeyFrame_t;
END_PACK

/******************************************************************************
                               Prototypes section
 ******************************************************************************/
/**************************************************************************//**
  \brief Switch key command indication handler.

  \param[in] commandInd - pointer to received command descriptor.

  \return 'true' if NWK buffer with the command frame must be freed
          otherwise return 'false'.
 ******************************************************************************/
APS_PRIVATE bool apsSwitchKeyInd(const ApsCommandInd_t *const commandInd);

#endif /* _SECURITY_ */
#endif /* _APS_SWITCH_KEY_H */
/** eof apsSwitchKey.h */
