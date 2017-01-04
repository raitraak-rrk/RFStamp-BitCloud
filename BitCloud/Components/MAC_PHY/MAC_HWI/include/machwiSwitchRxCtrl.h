/******************************************************************************
  \file machwiSwitchRxCtrl.h

  \brief Implementation of switching receiver control.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      23/08/07 A. Mandychev - Created.
******************************************************************************/

#ifndef _MACHWISWITCHRXCTRL_H
#define _MACHWISWITCHRXCTRL_H

/******************************************************************************
                    Includes section
******************************************************************************/
#include <sysTypes.h>
#include <machwiMemAccess.h>
#include <machwiHwdReqMemPool.h>

/******************************************************************************
                    Define(s) section
******************************************************************************/

/******************************************************************************
                    Types section
******************************************************************************/

/******************************************************************************
                    Constants section
******************************************************************************/

/******************************************************************************
                    External variables
******************************************************************************/
extern bool __switchIsLocked;

/******************************************************************************
                    Prototypes section
******************************************************************************/
/******************************************************************************
  Switches receiver to TRX OFF if RxOnWhenIdle parameter isn't set.
  Parameters:
    hwdReq - pointer to MACHWD request memory.
    switchingCompleted - callback to indicate the end of switching receiver.
  Returns:
    none.
******************************************************************************/
void machwiSwitchRxAccordToRxOnWhenIdle(MachwiHwdReqDescr_t *hwdReq,
  void (*switchingCompleted)(void));

/******************************************************************************
                    Inline static functions section
******************************************************************************/
/******************************************************************************
  Resets switching receiver control.
  Parameters:
    none.
  Returns:
    none.
******************************************************************************/
static inline void machwiResetSwitchingRxCtrl(void)
{
  machwiGetMem()->switchIsLocked = false;
}

/******************************************************************************
 Locks switching receiver control.
  Parameters:
    none.
  Returns:
    none.
******************************************************************************/
static inline void machwiLockSwitchingRx(void)
{
  machwiGetMem()->switchIsLocked = true;
}

/******************************************************************************
 Unlocks switching receiver control.
  Parameters:
    none.
  Returns:
    none.
******************************************************************************/
static inline void machwiUnlockSwitchingRx(void)
{
  machwiGetMem()->switchIsLocked = false;
}

/******************************************************************************
 Tests switching receiver control state.
  Parameters:
    none.
  Returns:
    true - switching receiver is locked.
    false - otherwise.
******************************************************************************/
static inline bool machwiIsSwitchingRxLocked(void)
{
  return machwiGetMem()->switchIsLocked;
}

#endif /* _MACHWISWITCHRXCTRL_H */

// eof machwiSwitchRxCtrl.h
