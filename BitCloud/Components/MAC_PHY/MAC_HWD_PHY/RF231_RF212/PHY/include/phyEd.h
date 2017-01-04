/******************************************************************************
  \file phyEd.h

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      26/05/07 A. Mandychev - Created.
******************************************************************************/

#ifndef _PHYED_H
#define _PHYED_H

/******************************************************************************
                        Includes section.
******************************************************************************/
#include <at86rf2xx.h>
#include <phy.h>
#include <phyCommon.h>
#include <phyRfSpiProto.h>
#include <machwdEdHandler.h>

/******************************************************************************
                        Prototypes section.
******************************************************************************/
/******************************************************************************
  TBD
******************************************************************************/
PHY_ReqStatus_t PHY_EdReq(void);

/******************************************************************************
                 Inline static functions prototypes section.
******************************************************************************/
/******************************************************************************
  TBD // concurrency (asynchronous) process
******************************************************************************/
INLINE void PHY_EdHandler(void)
{
  PHY_EnergyLevel_t energyLevel;
  // load PHY_ED_LEVEL from RF_chip.
  energyLevel = (PHY_EnergyLevel_t) phyReadRegisterInline(PHY_ED_LEVEL_REG);
  PHY_EdConf(energyLevel);
}

#endif /* _PHYED_H */

// eof phyEd.h
