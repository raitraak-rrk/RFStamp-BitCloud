/**************************************************************************//**
  \file apsUpdateDevice.h

  \brief Private interface of APS Update Device.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2010-12-18 Max Gekk - Created.
   Last change:
    $Id: apsUpdateDevice.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#if !defined _APS_UPDATE_DEVICE_H
#define _APS_UPDATE_DEVICE_H

/******************************************************************************
                                Includes section
 ******************************************************************************/
#include <apsCommand.h>
#include <nwk.h>
#include <apsFrames.h>

#if defined _SECURITY_
/******************************************************************************
                                 Types section
 ******************************************************************************/
BEGIN_PACK
/** Type of APS update device command frame. */
typedef struct PACK _ApsUpdateDeviceFrame_t
{
  /** An APS header, which comprises frame control and addressing information. */
  ApduCommandHeader_t header;
  /** The 64-bit extended address of the device whose status is being updated.*/
  ExtAddr_t deviceAddress;
  /** The 16-bit network address of the device whose status is being updated. */
  ShortAddr_t deviceShortAddress;
  /** The status field shall be assigned a value as described for the Status
   * parameter in ZigBee spec r19, Table 4.20. */
  uint8_t status;
} ApsUpdateDeviceFrame_t;
END_PACK

/******************************************************************************
                               Prototypes section
 ******************************************************************************/
/**************************************************************************//**
  \brief Update device indication handler.

  \param[in] commandInd - pointer to received command descriptor.

  \return 'true' if NWK buffer with the command frame must be freed
          otherwise return 'false'.
 ******************************************************************************/
APS_PRIVATE bool apsUpdateDeviceInd(const ApsCommandInd_t *const commandInd);

#endif /* _SECURITY_ */
#endif /* _APS_UPDATE_DEVICE_H */
/** eof apsUpdateDevice.h */
