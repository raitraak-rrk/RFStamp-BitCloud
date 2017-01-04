/******************************************************************************
  \file phyStateMachine.h

  \brief PHY sublayer state control functions prototypes.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      30/05/07 A. Mandychev - Created.
******************************************************************************/

#ifndef _PHYSTATEMACHINE_H
#define _PHYSTATEMACHINE_H

/******************************************************************************
                        Includes section.
******************************************************************************/
#include <sysTypes.h>
#include <machwdManager.h>
#include <phyRfSpiProto.h>
#include <phyMemAccess.h>

/******************************************************************************
                        External variables.
******************************************************************************/
extern bool __wasRequest;

/******************************************************************************
                 Inline static functions prototypes section.
******************************************************************************/
/******************************************************************************
  TBD
******************************************************************************/
INLINE void phySetState(PhyState_t state)
{
  if ((state == PHY_IDLE_STATE) && __wasRequest)
  {
    MACHWD_PostTask();
    __wasRequest = false;
  }
  *phyMemPhyState() = state;
}

/******************************************************************************
  TBD
******************************************************************************/
INLINE PhyState_t phyGetState(void)
{
  if (*phyMemPhyState() == PHY_BUSY_RX_STATE) __wasRequest = true;
    return *phyMemPhyState();
}

/******************************************************************************
  TODO
******************************************************************************/
INLINE void phyResetState(void)
{
  *phyMemPhyState() = PHY_IDLE_STATE;
}

/******************************************************************************
  Checks whether RF is in dst state
  Parameters:
    statusReg - status reg.
    destState - dest state
  Return: nothing
******************************************************************************/
void PHY_CheckStateTransition(uint8_t statusReg, uint8_t destState);

#endif /* _PHYSTATEMACHINE_H */

// eof phyStateMachine.h
