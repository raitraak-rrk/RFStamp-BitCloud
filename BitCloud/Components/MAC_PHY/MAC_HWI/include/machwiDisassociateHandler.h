/******************************************************************************
  \file machwiDisassociate.h

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      14/08/07 A. Mandychev - Created.
******************************************************************************/

#ifndef _MACHWIDISASSOCIATE_H
#define _MACHWIDISASSOCIATE_H
#ifdef _MAC_DISASSOCIATE_


/******************************************************************************
                    Includes section
******************************************************************************/
#include <macFrame.h>
#include <macDisassociate.h>
#include <machwiManager.h>

/******************************************************************************
                    Define(s) section
******************************************************************************/

/******************************************************************************
                    Types section
******************************************************************************/
typedef enum
{
  MACHWI_IDLE_DISASSOCIATE_STATE,
  MACHWI_SET_PAN_ID_DISASSOCIATE_STATE,
  MACHWI_SET_SHORT_ADDR_DISASSOCIATE_STATE,
} MachwiDisassociateState_t;

/******************************************************************************
                    Constants section
******************************************************************************/

/******************************************************************************
                    External variables
******************************************************************************/

/******************************************************************************
                    Prototypes section
******************************************************************************/
/******************************************************************************
  Resets disassociate component.
  Parameters:
    none.
  Returns:
    none.
******************************************************************************/
void machwiResetDisassociateHandler(void);

/******************************************************************************
 Checks disassociation request parameters, sends disassociation notification
 command.
 Parameters:
   none.
 Returns:
   current status of operation (success or "in progress").
******************************************************************************/
MachwiHandlerResult_t machwiDisassociateReqHandler(void);

/******************************************************************************
  Sets disassociation notification command fields.
  Parameters:
    frameDescr      - pointer to frame descriptor which will be set.
    disassociateReq - pointer to disassociation notification command
      parameters.
  Returns:
    none.
******************************************************************************/
void machwiPrepareDisassociateNotifCmdFrame(
  MAC_FrameDescr_t *frameDescr,
  MAC_DisassociateReq_t *disassociateReq);

/******************************************************************************
  Indicates, that disassociation notification command frame was received.
  Parameters:
    frameDescr - frame description.
  Returns:
    none.
******************************************************************************/
void machwiDisassociateNotifInd(MAC_FrameDescr_t *frameDescr);

#endif /* _MAC_DISASSOCIATE_ */

#endif /* _MACHWIDISASSOCIATE_H */

// eof machwiDisassociate.h
