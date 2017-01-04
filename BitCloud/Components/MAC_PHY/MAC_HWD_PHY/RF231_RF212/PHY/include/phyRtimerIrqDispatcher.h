/*************************************************************************//**
  \file phyRtimerIrqDispatcher.h

  \brief Declaration Rtimer dispatches interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    21/04/09 A.Khromykh - Created.
*****************************************************************************/

#ifndef _PHYRTIMERIRQDISPATCHER_H
#define _PHYRTIMERIRQDISPATCHER_H

/******************************************************************************
                        Includes section.
******************************************************************************/
#include <phyMemAccess.h>

/******************************************************************************
                    Definitions section
******************************************************************************/
#define STATE_POLL_PERIOD 50 // [us] Period of time for RF current state polling.

/******************************************************************************
                        Prototypes section.
******************************************************************************/
/******************************************************************************
  Set source of timer invocation.
******************************************************************************/
void phySetRTimerSource(HAL_RTimerSource_t source);

#endif /* _PHYRTIMERIRQDISPATCHER_H */

// eof phyRtimerIrqDispatcher.h
