/**************************************************************************//**
  \file apsRemoveDevice.h

  \brief Private interface of APS Remove Device.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2011-03-18 A. Razinkov - Created.
   Last change:
    $Id: apsRemoveDevice.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#ifndef _APS_REMOVE_DEVICE_H
#define _APS_REMOVE_DEVICE_H

#ifdef _SECURITY_
/******************************************************************************
                                Includes section
 ******************************************************************************/
#include <apsCommand.h>
#include <nwk.h>
#include <apsFrames.h>
#include <apsmeRemoveDevice.h>

/******************************************************************************
                                 Types section
 ******************************************************************************/
BEGIN_PACK
/** Type of APS remove device command frame. */
typedef struct PACK _ApsRemoveDeviceFrame_t
{
  /** An APS header, which comprises frame control and addressing information. */
  ApduCommandHeader_t header;
  /** the 64-bit extended address of the device that is requested to be removed
   *  from the network. */
  ExtAddr_t targetAddress;
} ApsRemoveDeviceFrame_t;
END_PACK

/******************************************************************************
                               Prototypes section
 ******************************************************************************/
/**************************************************************************//**
  \brief Remove device indication handler.

  \param[in] commandInd - pointer to received command descriptor.

  \return 'true' if NWK buffer with the command frame must be freed
          otherwise return 'false'.
 ******************************************************************************/
APS_PRIVATE bool apsRemoveDeviceInd(const ApsCommandInd_t *const commandInd);

#endif /* _SECURITY_ */
#endif /* _APS_REMOVE_DEVICE_H */
/** eof apsRemoveDevice.h */
