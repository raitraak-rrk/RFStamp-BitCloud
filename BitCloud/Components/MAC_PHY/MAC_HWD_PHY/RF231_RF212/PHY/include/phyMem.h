/******************************************************************************
  \file phyMem.h

  \brief PHY memory type.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \brief
    History:
      24/04/08 A. Mandychev - Created.
******************************************************************************/

#ifndef _PHYMEM_H
#define _PHYMEM_H

/******************************************************************************
                    Includes section
******************************************************************************/
#include <sysTypes.h>
#include <phyCommon.h>

/******************************************************************************
                    Define(s) section
******************************************************************************/

/******************************************************************************
                    Types section
******************************************************************************/
/** PHY sublayer internal memory pool */
typedef struct
{
  volatile PHY_TrxCmd_t      phyTrxCmd;

  // Source of timer invocation
  volatile HAL_RTimerSource_t  rTimerSource;

  volatile PhyTrxState_t     phyTrxState;
  volatile PhyState_t        phyState;

  // For use in phyRxFrame.h only.
  volatile  uint8_t          rxByteCounter;
  volatile  int8_t           rssi;

#if defined(_FFD_) && defined(_CONTROL_FRAME_PENDING_BIT_)
  volatile bool ackedWithoutPendBit;
#endif

  // pointer to a battery monitor indication
  void (* phyBatMonInd)(void);
} PhyMem_t;

#endif /*_PHYMEM_H*/

// eof macphyMem.h
