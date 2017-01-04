/******************************************************************************
  \file machwdMem.h
  
  \brief MACHWD memory access functions.
  
  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008 , Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \brief
    History:     
      18/05/11 A. ALuzhetsky - Created.
******************************************************************************/

#ifndef _MACHWDMEMACCESS_H
#define _MACHWDMEMACCESS_H

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
INLINE bool* machwdMemBeaconRxMode(void)
{
  return &macMem.hwd.beaconRxMode;
}

INLINE uint8_t* machwdMemHandlers(void)
{
  return &macMem.hwd.handlers;
}

INLINE volatile PHY_EnergyLevel_t* machwdMemEnergyLevel(void)
{
  return &macMem.hwd.energyLevel;
}

INLINE volatile PHY_DataStatus_t* machwdMemDataStatus(void)
{
  return &macMem.hwd.dataStatus;
}

#endif // _MACHWDMEMACCESS_H

/* eof machwdMemAccess.h*/

