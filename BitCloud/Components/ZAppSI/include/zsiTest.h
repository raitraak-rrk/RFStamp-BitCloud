/******************************************************************************
  \file zsiTest.h

  \brief
    Wrappers to internal functions. 

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2010 , Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2013-01-14  S. Dmitriev - Created.
   Last change:
    $Id: zsiTest.h 24441 2013-02-07 12:02:29Z akhromykh $
******************************************************************************/

#ifdef ZSI_TEST
  #include <testCommon.h>

  #define STATE_LOGGING testZsiStateLogging
  #define COLLISION_STATUS_LOGGING testZsiCollisionStateLogging
  #define RECEIVE_FRAME_LOGGING testZsiReceiveFrameLogging
#else 
  #define STATE_LOGGING(state) {}
  #define COLLISION_STATUS_LOGGING(state) {}
  #define RECEIVE_FRAME_LOGGING(frame) {}
#endif

#ifdef ZSI_TEST
/******************************************************************************
  \brief Adds FCS in the end of the frame.

  \param[in] frame - frame, which keeps serialized data.

  \return None.
 ******************************************************************************/
void TEST_zsiAddFrameFcs(void *const frame);

/******************************************************************************
  \brief Prepare ACK frame.

  \param[in] status - reception status: ZSI_NO_ERROR_STATUS,
                      ZSI_INVALID_FCS_STATUS or ZSI_OVERFLOW_STATUS.
  \param[in] sequenceNumber - sequnece number associated with received frame.
  \param[out] ackFrame - pointer to memory to prepare.

  \return None.
 ******************************************************************************/
void TEST_zsiPrepareAck(uint8_t status, uint8_t sequenceNumber,
  ZsiAckFrame_t *const ackFrame);

/******************************************************************************
  \brief Wrapper to routine. Send ACK frame with particular sequence number and status. If previous
         transmission wasn't finished - action will be postponed.

  \param[in] status - reception status: ZSI_NO_ERROR_STATUS,
                      ZSI_INVALID_FCS_STATUS or ZSI_OVERFLOW_STATUS.
  \param[in] sequenceNumber - sequnece number associated with received frame.

  \return None.
 ******************************************************************************/
void TEST_zsiReplyWithAck(uint8_t status, uint8_t sequenceNumber);

#endif /* ZSI_TEST */