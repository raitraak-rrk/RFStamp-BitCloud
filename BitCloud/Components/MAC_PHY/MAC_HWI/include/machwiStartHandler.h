/******************************************************************************
  \file machwiStartHandler.h

  \brief Header file of machwiStartHandler.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      24/06/07 ALuzhetsky - Created.
******************************************************************************/

#ifndef _MACHWISTARTHANDLER_H
#define _MACHWISTARTHANDLER_H
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
typedef enum
{
  MACHWI_IDLE_START_STATE,
  MACHWI_SET_PAN_ID_START_STATE,
  MACHWI_SET_PAN_COORDINATOR_START_STATE,
  MACHWI_SET_PAGE_START_STATE,
  MACHWI_SET_CHANNEL_START_STATE,
  MACHWI_TX_COORD_REALIG_CMD_STATE,
} MachwiStartState_t;

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
MachwiHandlerResult_t machwiStartReqHandler(void);

/******************************************************************************
  Resets start handler.
  Parameters:
    none.
  Returns:
    none.
******************************************************************************/
void machwiResetStartHandler(void);

#endif // _FFD_
#endif /* _MACHWISTARTHANDLER_H */

// eof machwiStartHandler.h
