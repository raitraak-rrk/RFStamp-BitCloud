/**************************************************************************//**
  \file macAffix.h

  \brief MAC frames' header and footer size constants.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      18/06/07 ALuzhetsky - Created.
******************************************************************************/

#ifndef _MACAFFIX_H
#define _MACAFFIX_H

/******************************************************************************
                        Includes section
******************************************************************************/
#include <macphyPib.h>

/******************************************************************************
                        Defines section
******************************************************************************/
//! aMaxMACPayloadSize (IEEE 802.15.4-2006 Table 85).
#define MAC_MAX_MSDU_SIZE (PHY_MAX_PACKET_SIZE - MAC_MAX_MPDU_UNSECURED_OVERHEAD)
//! aMaxBeaconOverhead (IEEE 802.15.4-2006 Table 85).
#define MAC_MAX_BEACON_HEADER_LENGTH MAC_MAX_BEACON_OVERHEAD
//! aMaxMPDUUnsecuredOverhead (IEEE 802.15.4-2006 Table 85).
#define MAC_MAX_DATA_FRAME_HEADER_LENGTH MAC_MAX_MPDU_UNSECURED_OVERHEAD

#ifndef _IEEE_ZIGBEE_COMPLIANCE_
#define MAC_MAX_INTERPAN_MSDU_SIZE  MAC_MAX_MSDU_SIZE
#else
#define MAC_MAX_INTERPAN_MSDU_SIZE (PHY_MAX_PACKET_SIZE - MAC_MAX_INTERPAN_MPDU_UNSECURED_OVERHEAD)
#endif // _IEEE_ZIGBEE_COMPLIANCE_

/**
 * \brief MAC footer length in common frame buffer.
 * It has zero length becouse FCS is calculated by RF chip. MAC does't has to
 * reserve bytes to allocate it's footer.
 */
#define MAC_MAX_DATA_FRAME_FOOTER_LENGTH 0U
//! Offset in common frame buffer for allocating MPDU.
#define MAC_MSDU_OFFSET MAC_MAX_MPDU_UNSECURED_OVERHEAD
//! MAC affix.
#define MAC_AFFIX_LENGTH (MAC_MAX_MPDU_UNSECURED_OVERHEAD + MAC_MAX_DATA_FRAME_FOOTER_LENGTH)

#endif /* _MACAFFIX_H */

// eof macAffix.h
