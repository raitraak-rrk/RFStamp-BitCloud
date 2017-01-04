/***************************************************************************//**
  \file zsiSerialController.c

  \brief ZAppSI serial interface controller implementation.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2011-09-01  A. Razinkov - Created.
   Last change:
    $Id: zsiSerialController.c 25433 2013-10-04 11:38:06Z nfomin $
 ******************************************************************************/

/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <zsiSerialController.h>
#include <zsiMem.h>
#include <zsiDbg.h>
#include <sysUtils.h>
#include <zsiTest.h>

/******************************************************************************
                               Defines section
 ******************************************************************************/
#define ZSI_ACK_WAIT_PERIOD  500UL  /* 500 ms */
#if CERTICOM_SUPPORT == 1
#define ZSI_SRSP_WAIT_PERIOD 10000UL /* 10 seconds */
#else
#define ZSI_SRSP_WAIT_PERIOD 2000UL /* 2 seconds */
#endif
#define ZSI_OVERFLOW_DELAY   3000UL /* 3 seconds */

#define ZSI_ACK_RETRIES_AMOUNT 3U

/******************************************************************************
                               Prototypes section
 ******************************************************************************/
static void zsiAddFrameFcs(void *const frame);
static bool zsiCheckFrameFcs(const void *const frame);
static uint8_t *zsiGetFrameFcsField(const void *const frame);
static uint8_t zsiCalculateFrameFcs(const void *const frame);
static void zsiPrepareAck(uint8_t status, uint8_t sequenceNumber,
  ZsiAckFrame_t *const ackFrame);
static void zsiReplyWithAck(uint8_t status, uint8_t sequenceNumber);
static void zsiSerialAckTimerFired(void);
static void zsiSerialSrspTimerFired(void);
static void zsiSerialOverflowTimerFired(void);
static void zsiSerialAckReceived(const ZsiAckFrame_t *const ackFrame);
static void zsiSerialCommandReceived(ZsiCommandFrame_t *const cmdFrame);
static void zsiSerialResolveCollisions(void);

/******************************************************************************
                               External functions section
 ******************************************************************************/
extern void zsiMediumHandler(void);

/******************************************************************************
                   Local variables section
******************************************************************************/
uint8_t ackTxState;

/******************************************************************************
                              Implementations section
******************************************************************************/
/******************************************************************************
  \brief Change state of serial controller.

  \param[in] newState - new state.

  \return None.
 ******************************************************************************/
INLINE void zsiSerialChangeState(ZsiSerialState_t newState)
{
  STATE_LOGGING(newState);
  zsiSerial()->state = newState;
}

/******************************************************************************
  \brief Collision detection notification routine.

  \param[in] status - collision details.

  \return None.
 ******************************************************************************/
INLINE void zsiSerialRaiseCollision(uint8_t status)
{
  zsiSerial()->collisionStatus |= status;
  COLLISION_STATUS_LOGGING(zsiSerial()->collisionStatus);
}

/******************************************************************************
  \brief Collision resolved notification routine.

  \param[in] status - collision details.

  \return None.
 ******************************************************************************/
INLINE void zsiSerialCollisionResolved(uint8_t status)
{
  zsiSerial()->collisionStatus &= ~(status);
  COLLISION_STATUS_LOGGING(zsiSerial()->collisionStatus);
}

/******************************************************************************
  \brief Start synchronous mode timer.

  \param[in] interval - expiration interval in ms.

  \return None.
 ******************************************************************************/
INLINE void zsiSerialStartSynchroModeTimer(uint32_t interval)
{
  ZsiSerialSynchroModeTimer_t *timer = &zsiSerial()->synchroModeTimer;
  uint64_t tmp = HAL_GetSystemTime() + (BcTime_t)(interval);

  COPY_64BIT_VALUE(timer->expirationTime, tmp);
  timer->started = true;

  zsiPostTask(ZSI_SERIAL_TASK_ID);
}

/******************************************************************************
  \brief Stop synchronous mode timer.

  \return None.
 ******************************************************************************/
INLINE void zsiSerialStopSynchroModeTimer(void)
{
  zsiSerial()->synchroModeTimer.started = false;
}

/******************************************************************************
  \brief ZAppSI serial controller reset routine.

  \return None.
 ******************************************************************************/
void zsiResetSerial(void)
{
  memset(zsiSerial(), 0x00, sizeof(ZsiSerialController_t));

  HAL_StopAppTimer(&zsiSerial()->ackWaitTimer);
  zsiSerial()->ackWaitTimer.mode = TIMER_ONE_SHOT_MODE;
  zsiSerial()->ackWaitTimer.interval = ZSI_ACK_WAIT_PERIOD;
  zsiSerial()->ackWaitTimer.callback = zsiSerialAckTimerFired;
  zsiSerial()->ackRetries = ZSI_ACK_RETRIES_AMOUNT;

  HAL_StopAppTimer(&zsiSerial()->overflowTimer);
  zsiSerial()->overflowTimer.mode = TIMER_ONE_SHOT_MODE;
  zsiSerial()->overflowTimer.interval = ZSI_OVERFLOW_DELAY;
  zsiSerial()->overflowTimer.callback = zsiSerialOverflowTimerFired;
  
  zsiMediumInit();
  zsiSerialChangeState(ZSI_SERIAL_STATE_IDLE);
}

/******************************************************************************
  \brief Returns serial controller status.

  \return True, if serial controller is busy, otherwise - false.
 ******************************************************************************/
bool zsiSerialIsBusy(void)
{
  return (zsiSerial()->currentTransmission) ? true : false;
}

/******************************************************************************
  \brief Transaction has been finished. Clear pointer and memory.
*******************************************************************************/
void zsiSerialClearCurTransac(void)
{
  ZsiCommandFrame_t *frame = (ZsiCommandFrame_t *)zsiSerial()->currentTransmission;

  zsiFreeMemory(frame);
  zsiSerial()->currentTransmission = NULL;
  zsiSerialChangeState(ZSI_SERIAL_STATE_IDLE);
}

/******************************************************************************
  \brief ZAppSI serial controller task handler.

  \return None.
 ******************************************************************************/
void zsiSerialTaskHandler(void)
{
  zsiMediumHandler();

  switch (zsiSerial()->state)
  {
    case ZSI_SERIAL_STATE_IDLE:
    {
      ZsiMemoryBuffer_t *buffer;

      /* Process frames to transmit with highest priority */
      if (NULL != (buffer = getQueueElem(&zsiSerial()->txQueue)))
        if (!zsiSerialIsBusy())
        {
          deleteHeadQueueElem(&zsiSerial()->txQueue);
          zsiSerialSend(&buffer->commandFrame);
        }
    }
    break;

    case ZSI_SERIAL_STATE_WAITING_ACK:
    case ZSI_SERIAL_STATE_WAITING_SRSP:
    {
      /* Process synchroModeTimer if started */
      ZsiSerialSynchroModeTimer_t *timer = &zsiSerial()->synchroModeTimer;

      if (timer->started)
      {
        if (HAL_GetSystemTime() > timer->expirationTime)
        {
          timer->started = false;

          if (ZSI_SERIAL_STATE_WAITING_ACK == zsiSerial()->state)
            zsiSerialAckTimerFired();
          else
            zsiSerialSrspTimerFired();
        }
        else
        {
          zsiPostTask(ZSI_SERIAL_TASK_ID);
        }

        return;
      }
    }
    break;

    default:
      break;
  }

  /* Post task if any command is still pending */
  if (getQueueElem(&zsiSerial()->txQueue))
    zsiPostTask(ZSI_SERIAL_TASK_ID);
}

/******************************************************************************
  \brief Send frame to ZAppSI serial controller for further transmisson.

  \param[in] cmdFrame - frame, which keeps serialized data.

  \return None.
 ******************************************************************************/
void zsiSerialSend(ZsiCommandFrame_t *const cmdFrame)
{
  sysAssert(ZSI_SERIAL_STATE_IDLE == zsiSerial()->state,
         ZSISERIALCONTROLLER_ZSISERIALSEND0);

  zsiSerial()->currentTransmission = cmdFrame;
  zsiSerialChangeState(ZSI_SERIAL_STATE_SENDING);

  /* Add FCS to frame */
  zsiAddFrameFcs(cmdFrame);

  /* Start command sending through medium */
  zsiMediumSend(cmdFrame, zsiActualFrameLength(cmdFrame));
}

/******************************************************************************
  \brief Callback for medium transmission finished.

  \return None.
 ******************************************************************************/
void zsiMediumSendingDone(void)
{
  /* Resolve collision, if occured */
  if (zsiSerial()->collisionStatus)
    zsiSerialResolveCollisions();

  if (ZSI_ACK_TX_IS_IN_PROGRESS(ackTxState))
  {
    ZSI_ACK_TX_COMPLETE(ackTxState);
    ZSI_ACK_TX_COUNT_UP(ackTxState);
    zsiPostTask(ZSI_DRIVER_TASK_ID);
  }

  /* If serial controller is idle - release current transmission */
  if (ZSI_SERIAL_STATE_IDLE == zsiSerial()->state)
  {
    zsiSerial()->currentTransmission = NULL;
  }
  else if (ZSI_SERIAL_STATE_SENDING == zsiSerial()->state)
  {
    /* Start ACK wait timer */
    zsiSerialChangeState(ZSI_SERIAL_STATE_WAITING_ACK);
    /* Run syncronous mode timer during synchronous transaction */
    if (IS_SREQ_CMD_FRAME((ZsiCommandFrame_t *)zsiSerial()->currentTransmission))
      zsiSerialStartSynchroModeTimer(zsiSerial()->ackWaitTimer.interval);
    else
      HAL_StartAppTimer(&zsiSerial()->ackWaitTimer);
  }
}

/******************************************************************************
  \brief Indication of new frame received from remote device.

  \param[in] status - reception status:
                      ZSI_NO_ERROR_STATUS or ZSI_OVERFLOW_STATUS.
  \param[in] sequenceNumber - sequnece number associated with frame.
  \param[in] frame - pointer to received frame.

  \return None.
 ******************************************************************************/
void zsiMediumReceive(uint8_t status, uint8_t sequenceNumber, void *const frame)
{
  bool ackRequired = true;

  RECEIVE_FRAME_LOGGING(frame);

  /* Send ACK in case of overflow or invalid FCS */
  if (ZSI_OVERFLOW_STATUS == status)
  {
    status = status;
  }
  /* If FCS is incorrect - reply with corresponding ACK */
  else if (!zsiCheckFrameFcs(frame))
  {
    status = ZSI_INVALID_FCS_STATUS;
    zsiFreeMemory(frame);
  }
  /* Process frames received without errors */
  else if (IS_ACK_CMD_FRAME((ZsiCommandFrame_t *)frame))
  {
    /* Received ACK processing. No ACK should be sent on ACK frames. */
    if (zsiSerial()->currentTransmission)
      zsiSerialAckReceived(frame);
    else
      sysAssert(false, ZSISERIALCONTROLLER_ZSISERIALRECEIVE0);
    ackRequired = false;
  }

  /* Send ACK if required */
  if (ackRequired)
  {
    zsiReplyWithAck(status, sequenceNumber);
  }

  /* Process received AREQ, SREQ and SRSP */
  if ((ZSI_NO_ERROR_STATUS == status) &&
      !IS_ACK_CMD_FRAME((ZsiCommandFrame_t *)frame))
  {
    zsiSerialCommandReceived(frame);
  }
}

/******************************************************************************
  \brief Send ACK frame with particular sequence number and status. If previous
         transmission wasn't finished - action will be postponed.

  \param[in] status - reception status: ZSI_NO_ERROR_STATUS,
                      ZSI_INVALID_FCS_STATUS or ZSI_OVERFLOW_STATUS.
  \param[in] sequenceNumber - sequnece number associated with received frame.

  \return None.
 ******************************************************************************/
static void zsiReplyWithAck(uint8_t status, uint8_t sequenceNumber)
{
  ZsiAckFrame_t *const ackFrame = zsiAllocateMemory(ZSI_TX_ACK_MEMORY);

  /* Prepare ACK frame */
  zsiPrepareAck(status, sequenceNumber, ackFrame);
  /* Send ACK. Raise collision, if current transaction inprogress. */
  if (zsiSerialIsBusy())
  {
    zsiSerialRaiseCollision(ZSI_SERIAL_IMMIDIATE_ACK_REQUIRED);
    /* If there is no transmission performing - resolve the collision */
    if (ZSI_SERIAL_STATE_SENDING != zsiSerial()->state)
      zsiSerialResolveCollisions();
  }
  else
  {
    zsiSerial()->currentTransmission = ackFrame;
    ZSI_ACK_TX_IN_PROGRESS(ackTxState);
    zsiMediumSend(ackFrame, zsiActualFrameLength(ackFrame));
  }
}

#ifdef ZSI_TEST
/******************************************************************************
  \brief Wrapper to routine. Send ACK frame with particular sequence number and status. If previous
         transmission isn't finished - action will be postponed.

  \param[in] status - reception status: ZSI_NO_ERROR_STATUS,
                      ZSI_INVALID_FCS_STATUS or ZSI_OVERFLOW_STATUS.
  \param[in] sequenceNumber - sequnece number associated with received frame.

  \return None.
 ******************************************************************************/
void TEST_zsiReplyWithAck(uint8_t status, uint8_t sequenceNumber)
{
  zsiReplyWithAck( status, sequenceNumber);
}
#endif /* ZSI_TEST */

/******************************************************************************
  \brief Prepare ACK frame.

  \param[in] status - reception status: ZSI_NO_ERROR_STATUS,
                      ZSI_INVALID_FCS_STATUS or ZSI_OVERFLOW_STATUS.
  \param[in] sequenceNumber - sequnece number associated with received frame.
  \param[out] ackFrame - pointer to memory to prepare.

  \return None.
 ******************************************************************************/
static void zsiPrepareAck(uint8_t status, uint8_t sequenceNumber,
  ZsiAckFrame_t *const ackFrame)
{
  ackFrame->sof = ZSI_SOF_SEQUENCE;
  ackFrame->length = sizeof(ZsiAckFrame_t) - ZSI_COMMAND_FRAME_PREAMBLE_SIZE;
  ackFrame->sequenceNumber = sequenceNumber;
  ackFrame->frameControl = ZSI_ACK_CMD | status;
  zsiAddFrameFcs(ackFrame);
}

#ifdef ZSI_TEST
/******************************************************************************
  \brief Wrapper to prepare ACK frame.

  \param[in] status - reception status: ZSI_NO_ERROR_STATUS,
                      ZSI_INVALID_FCS_STATUS or ZSI_OVERFLOW_STATUS.
  \param[in] sequenceNumber - sequnece number associated with received frame.
  \param[out] ackFrame - pointer to memory to prepare.

  \return None.
 ******************************************************************************/
void TEST_zsiPrepareAck(uint8_t status, uint8_t sequenceNumber,
  ZsiAckFrame_t *const ackFrame)
{
  zsiPrepareAck(status, sequenceNumber, ackFrame);
}
#endif /* ZSI_TEST */

/******************************************************************************
  \brief Received ACK handling routine.

  \param[in] ackFrame - pointer to ACK frame received from medium.

  \return None.
 ******************************************************************************/
static void zsiSerialAckReceived(const ZsiAckFrame_t *const ackFrame)
{
  /* Generic frame: command if no collision occured, ACK - otherwise */
  ZsiCommandFrame_t *frame =
    (ZsiCommandFrame_t *)zsiSerial()->currentTransmission;
  bool collision = IS_ACK_CMD_FRAME(frame);

  /* Transaction state validation */
  sysAssert(zsiSerialIsBusy() && (ZSI_SERIAL_STATE_WAITING_ACK == zsiSerial()->state),
         ZSISERIALCONTROLLER_ZSISERIALACKRECEIVED0);

  /* Validate sequence number */
  if ((!collision) && (ackFrame->sequenceNumber != frame->sequenceNumber))
  {
    sysAssert(false, ZSISERIALCONTROLLER_ZSISERIALACKRECEIVED2);
    return;
  }

  /* Stop ACK timer and refresh ACK retry attempts */
  if (zsiSerial()->synchroModeTimer.started)
    zsiSerialStopSynchroModeTimer();
  else
    HAL_StopAppTimer(&zsiSerial()->ackWaitTimer);

  zsiSerial()->ackRetries = ZSI_ACK_RETRIES_AMOUNT;

  /* Processing errors */
  if (!IS_NO_ERROR_STATUS(ackFrame))
  {
    zsiSerialChangeState(ZSI_SERIAL_STATE_SENDING);
    /* If sent frame was corrupted during the transmission - resend */
    if (IS_INVALID_FCS_STATUS(ackFrame))
    {
      /* Resend if there is no ACK transmission raised from collision resolving */
      if (collision)
        zsiSerialRaiseCollision(ZSI_SERIAL_IMMIDIATE_CMD_SEND_REQUIRED);
      else
        zsiMediumSend(frame, zsiActualFrameLength(frame));
    }
    /* If overflow occured on the remote device - wait appropriate period and
       resend frame */
    else if (ZSI_OVERFLOW_STATUS == ackFrame->frameControl)
    {
      zsiSerialChangeState(ZSI_SERIAL_STATE_OVERFLOW_RESOLVING);
      HAL_StartAppTimer(&zsiSerial()->overflowTimer);
    }
    return;
  }

  /* If there is ACK transmission raised from collision resolving - raise
     collision */
  if (collision)
    zsiSerialRaiseCollision(ZSI_SERIAL_CMD_TRANSMISSION_FINISHED);
  /* Start SRSP waiting for SREQ */
  else if (IS_SREQ_CMD_FRAME(frame))
  {
    zsiSerialChangeState(ZSI_SERIAL_STATE_WAITING_SRSP);
    zsiSerialStartSynchroModeTimer(ZSI_SRSP_WAIT_PERIOD);
  }
  /* Release memory reserved for AREQ and SRSP */
  else
  {
    zsiSerialClearCurTransac();
  }

  /* No need to free memory for ACK as it is reserved */
  /* zsiFreeMemory(ackFrame); */
}

/******************************************************************************
  \brief Received command handling routine.

  \param[in] cmdFrame - pointer to frame received from medium.

  \return None.
 ******************************************************************************/
static void zsiSerialCommandReceived(ZsiCommandFrame_t *const cmdFrame)
{
  ZsiCommandFrame_t *frame =
    (ZsiCommandFrame_t *)zsiSerial()->currentTransmission;

  if (IS_SRSP_CMD_FRAME(cmdFrame))
  {
    sysAssert((ZSI_SERIAL_STATE_WAITING_SRSP == zsiSerial()->state) &&
           zsiSerial()->synchroModeTimer.started,
           ZSISERIALCONTROLLER_ZSISERIALCOMMANDRECEIVED0);

    if (cmdFrame->sequenceNumber != frame->sequenceNumber)
    {
      sysAssert(false, ZSISERIALCONTROLLER_ZSISERIALCOMMANDRECEIVED1);
      return;
    }

    zsiSerialStopSynchroModeTimer();
  }
  // store to queue till ack will be received
  zsiDriverStoreRxCmd(cmdFrame);
}

/******************************************************************************
  \brief Store new command for further transmission.

  \param[in] cmdFrame - frame, which keeps serialized data.

  \return None.
 ******************************************************************************/
void zsiSerialStoreTxCmd(const ZsiCommandFrame_t *const cmdFrame)
{
  ZsiMemoryBuffer_t *buffer = GET_PARENT_BY_FIELD(ZsiMemoryBuffer_t,
    commandFrame, cmdFrame);
  ZsiMemoryBuffer_t *topElement = getQueueElem(&zsiSerial()->txQueue);

  /* SRSP frames should be transmitted first */
  if (IS_SRSP_CMD_FRAME(cmdFrame) && topElement)
  {
    zsiSerial()->txQueue.head = NULL;
    putQueueElem(&zsiSerial()->txQueue, buffer);
    putQueueElem(&zsiSerial()->txQueue, topElement);
  }
  else
    putQueueElem(&zsiSerial()->txQueue, buffer);

  zsiPostTask(ZSI_SERIAL_TASK_ID);
}

/******************************************************************************
  \brief Serial controller transmission collision resolving routine.

  \return None.
 ******************************************************************************/
static void zsiSerialResolveCollisions(void)
{
  ZsiCommandFrame_t *frame;

  /* Store current context and immidiately send ACK */
  if (ZSI_SERIAL_IMMIDIATE_ACK_REQUIRED & zsiSerial()->collisionStatus)
  {
    ZsiAckFrame_t *const ackFrame = zsiAllocateMemory(ZSI_TX_ACK_MEMORY);
    void *tmpPtr;

    tmpPtr = zsiSerial()->currentTransmission;
    zsiSerial()->currentTransmission = ackFrame;
    zsiSerialCollisionResolved(ZSI_SERIAL_IMMIDIATE_ACK_REQUIRED);
    ZSI_ACK_TX_IN_PROGRESS(ackTxState);
    zsiMediumSend(ackFrame, zsiActualFrameLength(ackFrame));
    zsiSerial()->currentTransmission = tmpPtr;
    return;
  }

  /* At this point context is correct and frame keeps current transmission */
  frame = (ZsiCommandFrame_t *)zsiSerial()->currentTransmission;

  /* Immediate command sending */
  if (ZSI_SERIAL_IMMIDIATE_CMD_SEND_REQUIRED & zsiSerial()->collisionStatus)
  {
    zsiMediumSend(frame, zsiActualFrameLength(frame));
    zsiSerialCollisionResolved(ZSI_SERIAL_IMMIDIATE_CMD_SEND_REQUIRED);
  }

  /* Finishing current transaction */
  if (ZSI_SERIAL_CMD_TRANSMISSION_FINISHED & zsiSerial()->collisionStatus)
  {
    zsiSerialClearCurTransac();
    zsiSerialCollisionResolved(ZSI_SERIAL_CMD_TRANSMISSION_FINISHED);
  }
}

/******************************************************************************
  \brief ACK timer expiration callback. No ACK was received from remote device
         in appropriated time.

  \return None.
 ******************************************************************************/
static void zsiSerialAckTimerFired(void)
{
  /* If ACK retries attempts didn't expired */
  if (zsiSerial()->ackRetries)
  {
    zsiSerial()->ackRetries--;
    sysAssert(ZSI_SERIAL_STATE_WAITING_ACK == zsiSerial()->state,
           ZSISERIALCONTROLLER_ZSISERIALACKTIMERFIRED0);
    /* Change state to retransmit frame */
    zsiSerialChangeState(ZSI_SERIAL_STATE_SENDING);

    /* If already in collision state - raise new collision */
    if (zsiSerial()->collisionStatus)
    {
      zsiSerialRaiseCollision(ZSI_SERIAL_IMMIDIATE_CMD_SEND_REQUIRED);
    }
    else
    {
      ZsiCommandFrame_t *cmdFrame =
        (ZsiCommandFrame_t *)zsiSerial()->currentTransmission;
      zsiMediumSend(cmdFrame, zsiActualFrameLength(cmdFrame));
    }
  }
  /* All attempts are failed - raise LOST_SYNCHRONIZATION event */
  else
  {
    sysAssert(0U, ZSISERIALCONTROLLER_ZSISERIALACKTIMERFIRED1);
    SYS_PostEvent(BC_ZSI_LOST_SYNCHRONIZATION, 0U);
  }
}

/******************************************************************************
  \brief SRSP timer expiration callback. No SRSP was received from remote device
         in appropriated time. Appeals to syncronization lost event.

  \return None.
 ******************************************************************************/
static void zsiSerialSrspTimerFired(void)
{
  /* If this timer fired - abnormal situation occured.
     Lost syncronization event must be indicated. Today it is assert(0)   */
  sysAssert(ZSI_SERIAL_STATE_WAITING_SRSP == zsiSerial()->state,
         ZSISERIALCONTROLLER_ZSISERIALSRSPTIMERFIRED0);
  sysAssert(0U, ZSISERIALCONTROLLER_ZSISERIALSRSPTIMERFIRED1);
  SYS_PostEvent(BC_ZSI_LOST_SYNCHRONIZATION, 0U);
}

/******************************************************************************
  \brief Overflow timer expiration callback. Retransmission can be performed.

  \return None.
 ******************************************************************************/
static void zsiSerialOverflowTimerFired(void)
{
  /* Generic frame: command if no collision occured, ACK - otherwise */
  ZsiCommandFrame_t *frame =
    (ZsiCommandFrame_t *)zsiSerial()->currentTransmission;

  sysAssert(ZSI_SERIAL_STATE_OVERFLOW_RESOLVING == zsiSerial()->state,
         ZSISERIALCONTROLLER_ZSISERIALOVERFLOWTIMERFIRED0);

  zsiSerialChangeState(ZSI_SERIAL_STATE_SENDING);
  /* Resend if there is no ACK transmission raised from collision resolving */
  if (IS_ACK_CMD_FRAME(frame))
    zsiSerialRaiseCollision(ZSI_SERIAL_IMMIDIATE_CMD_SEND_REQUIRED);
  else
    zsiMediumSend(frame, zsiActualFrameLength(frame));
}

/******************************************************************************
  \brief Adds FCS in the end of the frame.

  \param[in] frame - frame, which keeps serialized data.

  \return None.
 ******************************************************************************/
static void zsiAddFrameFcs(void *const frame)
{
  if (frame)
  {
    /* Obtain pointer to FCS field */
    uint8_t *fcs = zsiGetFrameFcsField(frame);

    /* Put FCS in the end of the frame */
    *fcs = zsiCalculateFrameFcs(frame);
  }
}

#ifdef ZSI_TEST
/******************************************************************************
  \brief Wrapper to add FCS in the end of the frame.

  \param[in] frame - frame, which keeps serialized data.

  \return None.
 ******************************************************************************/
void TEST_zsiAddFrameFcs(void *const frame)
{
  zsiAddFrameFcs(frame);
}
#endif /* ZSI_TEST */

/******************************************************************************
  \brief Frame validation by FCS calculating and checking routine.

  \param[in] frame - frame, which keeps serialized data.

  \return True, if frame received with no errors, false - otherwise.
 ******************************************************************************/
static bool zsiCheckFrameFcs(const void *const frame)
{
  if (!frame)
    return false;

  /* Calculate actual FCS and obtain pointer to FCS field */
  return (zsiCalculateFrameFcs(frame) == *zsiGetFrameFcsField(frame));
}

/******************************************************************************
  \brief Frame FCS calculation routine.

  \param[in] frame - frame, which keeps serialized data.

  \return FCS of the frame.
 ******************************************************************************/
static uint8_t zsiCalculateFrameFcs(const void *const frame)
{
  uint8_t *ptr = (uint8_t *)frame;
  uint8_t fcs = *ptr;

  /* Frame length without FCS field (to prevent XOR with garbage in FCS) */
  uint16_t frameLength = zsiActualFrameLength((ZsiCommandFrame_t *)frame) -
                         sizeof(uint8_t);

  while (--frameLength)
  {
    fcs ^= *(++ptr);
  }

  return fcs;
}

/******************************************************************************
  \brief Returns pointer to FCS field in the frame.

  \param[in] frame - frame, which keeps serialized data.

  \return Pointer to FCS field.
 ******************************************************************************/
static uint8_t *zsiGetFrameFcsField(const void *const frame)
{
  uint16_t frameLength = zsiActualFrameLength((ZsiCommandFrame_t *)frame);

  /* Obtain pointer to frame's last byte - FCS position */
  return (uint8_t *)frame + frameLength - sizeof(uint8_t);
}

/* eof zsiSerialController.c */
