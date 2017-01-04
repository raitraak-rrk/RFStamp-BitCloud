/******************************************************************************
  \file machwiOrphanHandler.h

  \brief Header file of machwiOrphanHandler.c.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      17/08/07 ALuzhetsky - Created.
******************************************************************************/

#ifndef _MACHWIORPHANHANDLER_H
#define _MACHWIORPHANHANDLER_H
#ifdef _FFD_

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

/******************************************************************************
                        Prototypes section
******************************************************************************/
/******************************************************************************
  Checks orphan response parameters, fills frame descriptor and passes request
  further if it is needed.
  Parameters:
    none.
  Returns:
    current status of operation ("success" or "in progress").
******************************************************************************/
MachwiHandlerResult_t machwiOrphanRespHandler(void);

/******************************************************************************
  Indicates tha orphan notification command was received.
  Parameters:
    none.
  Returns:
    none.
******************************************************************************/
void machwiOrphanNotificationInd(MAC_FrameDescr_t *orphanNotificationDescr);

#endif // _FFD_
#endif /* _MACHWIORPHANHANDLER_H */

// eof machwiOrphanHandler.h
