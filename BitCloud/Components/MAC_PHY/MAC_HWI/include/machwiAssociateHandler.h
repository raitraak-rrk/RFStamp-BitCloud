/******************************************************************************
  \file machwiAssociateHandler.h

  \brief machwiAssociateHandler header file.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      15/06/07 ALuzhetsky - Created.
******************************************************************************/

#ifndef _MACHWIASSOCIATEHANDLER_H
#define _MACHWIASSOCIATEHANDLER_H

/******************************************************************************
                        Includes section.
******************************************************************************/
#include <machwiManager.h>

/******************************************************************************
                       Types section
******************************************************************************/
typedef enum
{
  MACHWI_IDLE_ASSOCIATE_STATE,
  MACHWI_SET_CHANNEL_ASSOCIATE_STATE,
  MACHWI_SET_PAGE_ASSOCIATE_STATE,
  MACHWI_SET_COORD_PAN_ID_ASSOCIATE_STATE,
  MACHWI_SET_SHORT_ADDR_ASSOCIATE_STATE,
  MACHWI_CLR_RX_ON_WHEN_IDLE_ASSOCIATE_STATE,
  MACHWI_TX_ASSOCIATE_REQUEST_ASSOCIATE_STATE,
  MACHWI_WAIT_TX_DATA_REQUEST_ASSOCIATE_STATE,
  MACHWI_TX_DATA_REQUEST_ASSOCIATE_STATE,
  MACHWI_WAIT_ASSOCIATE_RESP_FRAME_ASSOCIATE_STATE,
  MACHWI_SET_TRX_OFF_ASSOCIATE_STATE,
} MachwiAssociateState_t;

/******************************************************************************
                   Prototypes section
******************************************************************************/
/******************************************************************************
 Checks associate request parameters, sends associate and data request commands.
 Waits for the responce.
 Parameters:
   none.
 Returns:
   current status of operation (success or "in progress").
******************************************************************************/
MachwiHandlerResult_t machwiAssociateReqHandler(void);

#ifdef _FFD_
/******************************************************************************
 Checks associate responce parameters, and puts the responce to the transaction
 queue.
 Parameters:
   none.
 Returns:
   current status of operation (success or "in progress").
******************************************************************************/
MachwiHandlerResult_t machwiAssociateRespHandler(void);
#endif //_FFD_

/******************************************************************************
  Indicates, that associate response command frame was received.
  Parameters:
    associateRespDescr - associate response command frame description.
  Returns:
    none.
******************************************************************************/
void machwiAssociateRespInd(MAC_FrameDescr_t *associateRespDescr);

#ifdef _FFD_
/******************************************************************************
  Indicates, that associate request command frame was received.
  Parameters:
    associateReqDescr - associate request command frame description.
  Returns:
    none.
******************************************************************************/
void machwiAssociateReqInd(MAC_FrameDescr_t *associateReqDescr);
#endif //_FFD_

/******************************************************************************
  Resets associate handler.
  Parameters:
    none.
  Returns:
    none.
******************************************************************************/
void machwiResetAssociateHandler(void);


#endif /* _MACHWIASSOCIATEHANDLER_H */

// eof machwiAssociateHandler.h
