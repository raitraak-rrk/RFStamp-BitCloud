/******************************************************************************
  \file machwiScanHandler.h

  \brief Header file of machwiScanHandler.c.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      24/06/07 ALuzhetsky - Created.
******************************************************************************/

#ifndef _MACHWISCANHANDLER_H
#define _MACHWISCANHANDLER_H

/******************************************************************************
                        Includes section
******************************************************************************/
#include <machwd.h>
#include <machwiManager.h>

/******************************************************************************
                        Defines section
******************************************************************************/

/******************************************************************************
                         Types section
******************************************************************************/
typedef enum
{
  MACHWI_IDLE_SCAN_STATE,
  MACHWI_SET_CHANNEL_SCAN_STATE,
  MACHWI_SET_BEACON_RX_MODE_SCAN_STATE,
  MACHWI_CLR_BEACON_RX_MODE_SCAN_STATE,
  MACHWI_SET_PAGE_SCAN_STATE,
  MACHWI_RESTORE_TRX_STATE_SCAN_STATE,
  MACHWI_SET_TRX_STATE_RX_ON_SCAN_STATE,
  MACHWI_SET_RF_IRQ_DISABLE_SCAN_STATE,
  MACHWI_SET_RF_IRQ_ENABLE_SCAN_STATE,
  MACHWI_SET_SHORT_ADDR_SCAN_STATE,
  MACHWI_SET_PANID_SCAN_STATE,
  MACHWI_SET_CHANNEL_AFTER_ORPHAN_SCAN_STATE,
  MACHWI_WAIT_SCAN_STATE,
  MACHWI_SCAN_TIME_EXHAUSTED_SCAN_STATE,
  MACHWI_SET_ACTIVE_CHANNEL_SCAN_STATE,
  MACHWI_SET_PANID_BROADCAST_SCAN_STATE,
  MACHWI_SET_PANID_ORIGINAL_SCAN_STATE
} MachwiScanState_t;

/******************************************************************************
                        Prototypes section
******************************************************************************/
/******************************************************************************
  TBD
 Parameters:
   TBD - TBD.
 Returns:
   TBD.
******************************************************************************/
MachwiHandlerResult_t machwiScanReqHandler(void);


/******************************************************************************
  Indicates, that beacon frame was received.
  Parameters:
    beaconDescr - beacon description.
  Returns:
    none.
******************************************************************************/
void machwiBeaconInd(MAC_FrameDescr_t *beaconDescr);

/******************************************************************************
  Indicates, that coordinator realignment connamd was received.
  Parameters:
    realignmentDescr - realignment command description.
  Returns:
    none.
******************************************************************************/
void machwiCoordRealignmentCommandInd(MAC_FrameDescr_t *realignmentDescr);

/******************************************************************************
  Resets scan handler.
  Parameters:
    none.
  Returns:
    none.
******************************************************************************/
void machwiResetScanHandler(void);

#endif /* _MACHWISCANHANDLER_H */

// eof machwiScanHandler.h
