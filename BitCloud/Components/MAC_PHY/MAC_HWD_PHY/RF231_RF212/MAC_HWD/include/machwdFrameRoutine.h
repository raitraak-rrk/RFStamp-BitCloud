/******************************************************************************
  \file machwdFrameRoutine.h

  \brief Types and constants declaration of IEEE802.15.4 MAC frame.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    15/06/07 ALuzhetsky - Created.
  Last change:
    $Id: machwdFrameRoutine.h 27584 2015-01-09 14:45:42Z unithra.c $
******************************************************************************/

#ifndef _MACHWDFRAMEROUTINE_H
#define _MACHWDFRAMEROUTINE_H

/******************************************************************************
                    Includes section
******************************************************************************/
#include <inttypes.h>
#include <macFrame.h>
#include <macAddr.h>
#include <macSuperframe.h>
#include <macBeacon.h>
#include <macAssociate.h>

/******************************************************************************
                    Prototypes section
******************************************************************************/
/***************************************************************************//**
 \brief Parses incoming frame

 \param  rawFrame - pointer to the incomming frame to parse
         frameDescr - frame descriptor to fill
 \return true if frame was parsed successfully, false otherwise
*******************************************************************************/
bool machwdParseFrame(MAC_FrameDescr_t *frameDescr, uint8_t *rawFrame);

/***************************************************************************//**
 \brief Composes outgoing frame.

 \param frameDescr - outgoing frame description structure pointer.
 \return pointer to the frame composed.
*******************************************************************************/
uint8_t* machwdComposeFrame(MAC_FrameDescr_t *frameDescr);

#endif /* _MACHWDFRAMEROUTINE_H */

// eof machwdFrameRoutine.h
