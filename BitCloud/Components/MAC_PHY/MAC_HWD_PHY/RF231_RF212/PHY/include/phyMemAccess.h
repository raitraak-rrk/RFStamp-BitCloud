/******************************************************************************
  \file phyMemAccess.h

  \brief PHY memory access functions.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008 , Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \brief
    History:
      18/05/11 A. ALuzhetsky - Created.
******************************************************************************/

#ifndef _PHYMEMACCESS_H
#define _PHYMEMACCESS_H

/******************************************************************************
                           Includes section
******************************************************************************/
#include <macMem.h>

/******************************************************************************
                           External variables section
******************************************************************************/
/** Declared in .../MAC_ENV/src/macMem.c */
extern MAC_Mem_t macMem;

/******************************************************************************
                           Inline functions section
 ******************************************************************************/
INLINE volatile PHY_TrxCmd_t *phyMemPhyTrxCmd(void)
{
  return &macMem.phy.phyTrxCmd;
}

INLINE volatile HAL_RTimerSource_t *phyMemRTimerSource(void)
{
  return &macMem.phy.rTimerSource;
}

INLINE volatile PhyTrxState_t *phyMemPhyTrxState(void)
{
  return &macMem.phy.phyTrxState;
}

INLINE volatile PhyState_t *phyMemPhyState(void)
{
  return &macMem.phy.phyState;
}

// For use in phyRxFrame.h only.
INLINE volatile uint8_t *phyMemRxByteCounter(void)
{
  return &macMem.phy.rxByteCounter;
}

// For use in phyRxFrame.h only.
INLINE volatile int8_t *phyMemRssi(void)
{
  return &macMem.phy.rssi;
}

#if defined(_FFD_) && defined(_CONTROL_FRAME_PENDING_BIT_)
INLINE volatile bool *phyMemAckedWithoutPendBit(void)
{
  return &macMem.phy.ackedWithoutPendBit;
}
#endif

/******************************************************************************
  Gets address of PHY memory.
  Parameters: none.
  returns: Address of PHY memory.
******************************************************************************/
INLINE PhyMem_t* phyGetMem(void)
{
  return &macMem.phy;
}

#endif // _PHYMEMACCESS_H

/* eof phyMemAccess.h*/

