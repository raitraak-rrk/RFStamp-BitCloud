/**************************************************************************//**
  \file machwiPollHandler.h

  \brief MAC poll routine header file.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      09/07/07 ALuzhetsky - Created.
******************************************************************************/

#ifndef _MACHWIPOLLHANDLER_H
#define _MACHWIPOLLHANDLER_H

/******************************************************************************
                        Includes section.
******************************************************************************/
#include <macFrame.h>
#include <machwiManager.h>

/******************************************************************************
                        Typesa definition section
******************************************************************************/
/******************************************************************************
  MAC Hardware Independent part POLL handler possible states.
******************************************************************************/
typedef enum
{
  MACHWI_POLL_HANDLER_STATE_IDLE,
  MACHWI_POLL_HANDLER_STATE_DATA_REQ_TRANSMISSION,
  MACHWI_POLL_HANDLER_STATE_ANSWER_WAITING,
  MACHWI_POLL_HANDLER_STATE_TRX_STATE_SETTING,
} MachwiPollHandlerState_t;

/******************************************************************************
                        Prototypes section
******************************************************************************/
/******************************************************************************
  Resets poll handler.
  Parameters:
    none.
  Returns:
    none.
******************************************************************************/
void machwiResetPollHandler(void);

/******************************************************************************
  Indicates data reception to upper layer.
  Parameters:
    frameDescr - pointer to frame descriptor.
  Returns:
    none.
******************************************************************************/
void machwiDataIndication(MAC_FrameDescr_t *frameDescr);

/******************************************************************************
 Checks poll request parameters, sends data request commands.
 Waits for the data.
 Parameters:
   none.
 Returns:
   current status of operation (success or "in progress").
******************************************************************************/
MachwiHandlerResult_t machwiPollReqHandler(void);

/******************************************************************************
  Indicates, that data request frame was received.
  Parameters:
    pollDescr - Poll description.
  Returns:
    none.
******************************************************************************/
void machwiPollInd(MAC_FrameDescr_t *frameDescr);

#ifdef _FFD_
#ifdef _PENDING_EMPTY_DATA_FRAME_
/**************************************************************************//**
  \brief Sends an empty data frame in reply to the data request.
  \param[in] frameDescr - data request frame description.
******************************************************************************/
void machwiPollHandlerReplyWithEmptyDataFrame(MAC_FrameDescr_t *frameDescr);
#endif // _PENDING_EMPTY_DATA_FRAME_
#endif /*_FFD_*/

#endif //_MACHWIPOLLHANDLER_H
// eof machwiPollHandler.h
