/***************************************************************************//**
  \file zsiDriver.c

  \brief ZAppSI driver implementation

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2011-08-23  A. Razinkov - Created.
   Last change:
    $Id: zsiDriver.c 27025 2014-08-05 05:08:54Z unithra.c $
 ******************************************************************************/

/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <zsiDriver.h>
#include <zsiMem.h>
#include <zsiDbg.h>
#include <zsiMemoryManager.h>
#include <zsiZdoSerialization.h>
#include <zsiZdpSerialization.h>
#include <zsiApsSerialization.h>
#include <zsiNwkSerialization.h>
#include <zsiKeSerialization.h>
#include <zsiMacSerialization.h>
#include <zsiHalSerialization.h>
#include <zsiBspSerialization.h>
#include <sysTaskManager.h>

/*****************************************************************************
                              Defines section
******************************************************************************/
#if CERTICOM_SUPPORT != 1
#define zsiKeFindProcessingRoutine    NULL
#endif

/******************************************************************************
                   External global variables section
******************************************************************************/
extern uint8_t ackTxState;

/*****************************************************************************
                              Static variables section
******************************************************************************/
static PROGMEM_DECLARE(ZsiDomainProcessingRoutine_t zsiDriverDomainProcessingRoutines[]) =
{
  [ZSI_CMD_SYS] = zsiSysFindProcessingRoutine,
  [ZSI_CMD_APS] = zsiApsFindProcessingRoutine,
  [ZSI_CMD_ZDO] = zsiZdoFindProcessingRoutine,
  [ZSI_CMD_ZDP] = zsiZdpFindProcessingRoutine,
  [ZSI_CMD_NWK] = zsiNwkFindProcessingRoutine,
  [ZSI_CMD_MAC] = zsiMacFindProcessingRoutine,
  [ZSI_CMD_HAL] = zsiHalFindProcessingRoutine,
  [ZSI_CMD_SEC] = NULL,
  [ZSI_CMD_KE]  = zsiKeFindProcessingRoutine,
  [ZSI_CMD_BSP] = zsiBspFindProcessingRoutine
};

/******************************************************************************
                              Static function section
******************************************************************************/
static ZsiProcessingRoutine_t
zsiDriverFindProcessingRoutine(ZsiCommandHeader_t cmdHeader);

/******************************************************************************
                               Implementation section
 ******************************************************************************/
/******************************************************************************
  \brief ZAppSI driver reset routine.
 ******************************************************************************/
void zsiResetDriver(void)
{
  memset(zsiDriver(), 0x00, sizeof(ZsiDriver_t));

  resetQueue(&(zsiDriver()->bearingEntities));
  resetQueue(&(zsiDriver()->postponedAreqs));
  resetQueue(&(zsiDriver()->commandsToReceive));
  resetQueue(&(zsiDriver()->completedAreqs));
  zsiDriver()->state = ZSI_DRIVER_STATE_IDLE;
}

/******************************************************************************
  \brief ZAppSI driver task handler.

  \return None.
 ******************************************************************************/
void zsiDriverTaskHandler(void)
{
  switch (zsiDriver()->state)
  {
    case ZSI_DRIVER_STATE_IDLE:
    {
      ZsiMemoryBuffer_t *buffer;
      uint8_t *memory = NULL;

      /* Process AREQs received from remote device with highest priority */
      if ((NULL != (buffer = getQueueElem(&zsiDriver()->commandsToReceive))) &&
          ZSI_ACK_TX_QUANTITY(ackTxState))
      {
        ZSI_ACK_TX_COUNT_DOWN(ackTxState);

        if (IS_AREQ_CMD_FRAME(&buffer->commandFrame))
        {
          memory = zsiAllocateMemory(ZSI_MUTUAL_MEMORY);
        }
        else if (IS_SREQ_CMD_FRAME(&buffer->commandFrame))
        {
          memory = zsiAllocateMemory(ZSI_SREQ_CMD);
        }
        else
        {
          sysAssert(false, ZSIDRIVER_ZSISRSP_OUTSIDE_BLOCKING);
        }

        if (memory)
        {
          deleteHeadQueueElem(&zsiDriver()->commandsToReceive);
          zsiDriverReceiveCommand(memory, &buffer->commandFrame);
        }

        if (ZSI_ACK_TX_QUANTITY(ackTxState))
          zsiPostTask(ZSI_DRIVER_TASK_ID);
      }

      /* Process previously stored AREQ's from application */
      else if (NULL != (memory = getQueueElem(&zsiDriver()->postponedAreqs)))
      {
        if (memory && zsiIsMemoryAvailable())
        {
          deleteHeadQueueElem(&zsiDriver()->postponedAreqs);
          sysAssert(((ZsiEntityService_t *)memory)->zsi.process,
                 ZSIDRIVER_ZSIDRIVERTASKHANDLER0);
          /* Reprocess entity with appropriate processing routine. Evil cast..
             Any ideas? */
          zsiProcessCommand(ZSI_AREQ_CMD, memory,
                            (ZsiSerializeRoutine_t)(((ZsiEntityService_t *)memory)->zsi.process), NULL);
        }
      }

      /* Process completed AREQs */
      else if (NULL != (memory = deleteHeadQueueElem(&zsiDriver()->completedAreqs)))
      {
        sysAssert(((ZsiEntityService_t *)memory)->zsi.callback,
               ZSIDRIVER_ZSIDRIVERTASKHANDLER1);
        ((ZsiEntityService_t *)memory)->zsi.callback(memory);
      }

      /* Post task if any AREQ is still pending and required memory is available */
      if (getQueueElem(&zsiDriver()->completedAreqs) ||
          (zsiIsMemoryAvailable() &&
           (getQueueElem(&zsiDriver()->commandsToReceive) ||
            getQueueElem(&zsiDriver()->postponedAreqs))))
      {
        zsiPostTask(ZSI_DRIVER_TASK_ID);
      }
    }
    break;

    case ZSI_DRIVER_STATE_BLOCKED:
      {
        ZsiMemoryBuffer_t *curBuffer;
        ZsiMemoryBuffer_t *srspBuffer = NULL;
        uint8_t queueSize = 0;

        curBuffer = getQueueElem(&zsiDriver()->commandsToReceive);
        while (curBuffer)
        {
          queueSize++;
          if (IS_SRSP_CMD_FRAME(&curBuffer->commandFrame))
            srspBuffer = curBuffer;
          curBuffer = getNextQueueElem(curBuffer);
        }

        if ((queueSize == ZSI_ACK_TX_QUANTITY(ackTxState)) && srspBuffer)
        {
          ZSI_ACK_TX_COUNT_DOWN(ackTxState);
          deleteQueueElem(&zsiDriver()->commandsToReceive, srspBuffer);
          zsiSrspReceived(&srspBuffer->commandFrame);
        }
        zsiPostTask(ZSI_DRIVER_TASK_ID);
      }
      break;

    default:
      sysAssert(0U, ZSIDRIVER_ZSIDRIVERTASKHANDLER2);
      break;
  }
}

/**************************************************************************//**
  \brief Common entry point for all commands come to ZAppSI from upper layers.

  \param[in] cmdType - type of command AREQ, SREQ or SRSP.
  \param[in] dataIn - pointer to data come with command.
  \param[in] serialize - pointer to serialization routine.
  \param[out] dataOut - pointer to execution result.

  \return None.
 ******************************************************************************/
void zsiProcessCommand(uint8_t cmdType, void *const dataIn,
  ZsiSerializeRoutine_t serialize, void *const dataOut)
{
  ZsiProcessingResult_t result;
  /* Try to allocate memory for new command */
  ZsiCommandFrame_t *outFrame = zsiAllocateMemory(cmdType);

  sysAssert(dataIn && serialize, ZSIDRIVER_ZSIPROCESSCOMMAND0);
  if (!outFrame)
  {
    /* Postpone AREQ, if no free memory is available */
    if (ZSI_AREQ_CMD == cmdType)
    {
      /* Store pointer to serialization routine. Evil cast.. Ideas? */
      ((ZsiEntityService_t *)dataIn)->zsi.process = (void ( *)(void *))serialize;
      putQueueElem(&zsiDriver()->postponedAreqs, dataIn);
    }
    return;
  }

  /* Serialize command with appropriate routine */
  result = serialize(dataIn, outFrame);
#ifdef ZAPPSI_HOST
  /* Store AREQ context on HOST. TODO: Should we free memory for SREQ ? */
  if (ZSI_AREQ_CMD == cmdType)
  {
    zsiDriverKeepBearingEntity(dataIn, outFrame->sequenceNumber);
  }
#endif /* ZAPPSI_HOST */

  zsiDriverSendCommand(outFrame, dataOut);

  if (!result.keepMemory)
    zsiFreeMemory(dataIn);
  if (!result.keepCmdFrame)
    zsiFreeMemory(outFrame);
}

/**************************************************************************//**
  \brief ZAppSI command frame initialization routine.

  \param[out] cmdFrame - frame, which keeps serialized data.
  \param[in] length - length of the frame, excluding SOF and LENGTH fields.
  \param[in] sequenceNumber - frame sequemce number.
  \param[in] type - ZAppSI command type: SREQ, SRSP or AREQ.
  \param[in] domain - command domain.
  \param[in] commandId - command identifier within the domain.

  \return None.
 ******************************************************************************/
void zsiPrepareCommand(ZsiCommandFrame_t *const cmdFrame, uint16_t length,
  uint8_t sequenceNumber, uint8_t type, uint8_t domain, uint8_t commandId)
{
  cmdFrame->sof = ZSI_SOF_SEQUENCE;
  cmdFrame->frameControl = type;
  cmdFrame->length = CPU_TO_LE16(length);
  cmdFrame->sequenceNumber = sequenceNumber;
  cmdFrame->commandHeader.domain = domain;
  cmdFrame->commandHeader.commandId = commandId;
}

/******************************************************************************
  \brief ZAppSI Driver prepared command transmission interface.

  \param[in] cmdFrame - frame, which keeps serialized command.
  \param[out] dataOut - pointer to memory to put returned value in case of SREQ.

  \return None.
 ******************************************************************************/
void zsiDriverSendCommand(ZsiCommandFrame_t *const cmdFrame, void *const dataOut)
{
  /* Transfer SREQ in single task */
  if (IS_SREQ_CMD_FRAME(cmdFrame))
  {
    zsiDriver()->state = ZSI_DRIVER_STATE_BLOCKED;
    /* Hold on all tasks except required medium one during SREQ sending */
    ZSI_ENTER_SYNCHRONOUS_MODE();
    /* Block here until current transmission finished */
    while (zsiSerialIsBusy())
    {
      zsiPostTask(ZSI_DRIVER_TASK_ID);
      SYS_ForceRunTask();
    }
    /* Send frame to serial controller */
    zsiSerialSend(cmdFrame);
    /* Block here until SRSP received */
    while (NULL == zsiDriver()->srsp)
    {
      /* Force ZAppSI tasks to manage synchronous mode timers */
      ZSI_TaskHandler();
      zsiPostTask(ZSI_DRIVER_TASK_ID);
      /* All ZAppSI states will be changed in HAL serial medium's callbacks */
      SYS_ForceRunTask();
    }
    /* Process SRSP */
    zsiSerialClearCurTransac();
    zsiDriverReceiveCommand(dataOut, zsiDriver()->srsp);
    zsiDriver()->srsp = NULL;
    ZSI_LEAVE_SYNCHRONOUS_MODE();
    zsiDriver()->state = ZSI_DRIVER_STATE_IDLE;
  }
  /* Transfer AREQ and SRSP if serial controller is idle */
  else if (!zsiSerialIsBusy())
  {
    zsiSerialSend(cmdFrame);
  }
  /* Store command in queue */
  else
    zsiSerialStoreTxCmd(cmdFrame);
}

/******************************************************************************
  \brief ZAppSI Driver received command processing routine.

  \param[in] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return None.
 ******************************************************************************/
void zsiDriverReceiveCommand(uint8_t *memory, ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = false,
    .keepMemory = false
  };
  /* Obtain processing routine */
  ZsiProcessingRoutine_t processingRoutine =
    zsiDriverFindProcessingRoutine(cmdFrame->commandHeader);

  if (processingRoutine)
  {
    /* Process serialized frame with obtained routine */
    result = processingRoutine(memory, cmdFrame);
  }
  /* Unsupported command received */
  else
    sysAssert(0U, ZSIDRIVER_ZSICOMMANDRECEIVED0);

#ifdef ZAPPSI_NP
  /* In case of SREQ command SRSP should be returned at the same memory */
  if (processingRoutine)
    if (IS_SRSP_CMD_FRAME(cmdFrame))
      zsiDriverSendCommand(cmdFrame, NULL);
#endif /* ZAPPSI_NP */

  if (!result.keepMemory)
    zsiFreeMemory(memory);
  if (!result.keepCmdFrame)
    zsiFreeMemory(cmdFrame);
}

/**************************************************************************//**
  \brief Finds routine for ZDO command deserialization.

  \param[in] commandId - command identifier.

  \return None.
 ******************************************************************************/
static ZsiProcessingRoutine_t
zsiDriverFindProcessingRoutine(ZsiCommandHeader_t cmdHeader)
{
  ZsiProcessingRoutine_t routine = NULL;
  ZsiDomainProcessingRoutine_t domainRoutine = NULL;

  if (cmdHeader.domain < ARRAY_SIZE(zsiDriverDomainProcessingRoutines))
  {
    /* Obtain general domain search routine */
    memcpy_P((void *)&domainRoutine, &zsiDriverDomainProcessingRoutines[cmdHeader.domain],
             sizeof(domainRoutine));

    if (domainRoutine)
    {
      /* Obtain processing routine within domain */
      routine = domainRoutine(cmdHeader.commandId);
    }
  }

  return routine;
}

/******************************************************************************
  \brief Store received command for further processing.

  \param[in] cmdFrame - frame, which keeps serialized data.

  \return None.
 ******************************************************************************/
void zsiDriverStoreRxCmd(const ZsiCommandFrame_t *const cmdFrame)
{
  ZsiMemoryBuffer_t *buffer = GET_PARENT_BY_FIELD(ZsiMemoryBuffer_t,
    commandFrame, cmdFrame);
  putQueueElem(&zsiDriver()->commandsToReceive, buffer);
}

/******************************************************************************
  \brief AREQ processing completed. Callback to application can be used.

  \param[in] areq - entity, which bore AREQ.

  \return None.
 ******************************************************************************/
void zsiAreqProcessingComplete(void *const areq)
{
  putQueueElem(&zsiDriver()->completedAreqs, areq);
  zsiPostTask(ZSI_DRIVER_TASK_ID);
}

/******************************************************************************
  \brief ZAppSI frames sequence number getter routine.

  \return Frame sequence number.
 ******************************************************************************/
uint8_t zsiGetSequenceNumber(void)
{
  return zsiDriver()->sequenceNumber++;
}

/******************************************************************************
  \brief Stores AREQ-bearing user entity for further linking with
         corresponding response.

  \param[in] handle - pointer to user entity from wich AREQ spawned.
  \param[in] sequenceNumber - uniq sequence number of the frame. For correct
                              AREQ and ARSP linking.

  \return None.
 ******************************************************************************/
void zsiDriverKeepBearingEntity(void *handle, uint8_t sequenceNumber)
{
  QueueDescriptor_t *bearingEntities = &zsiDriver()->bearingEntities;

  sysAssert(handle, ZSIDRIVER_ZSIKEEPBEARINGENTITY0);

  /* Mark entity with required sequence number. */
  ((ZsiEntityService_t *)handle)->sequenceNumber = sequenceNumber;
  /* Store entity in queue. */
  putQueueElem(bearingEntities, handle);
}

/**************************************************************************//**
  \brief Returns AREQ's bearing entity by it's sequence number.

  \param[in] sequenceNumber - entity sequence number.

  \return AREQ's bearing entity.
 ******************************************************************************/
void *zsiDriverReturnBearingEntity(uint8_t sequenceNumber)
{
  ZsiEntityService_t *entity =
    (ZsiEntityService_t *)(zsiDriver()->bearingEntities.head);

  while (entity)
  {
    if (entity->sequenceNumber == sequenceNumber)
    {
      deleteQueueElem(&zsiDriver()->bearingEntities, entity);
      return entity;
    }

    entity = (ZsiEntityService_t *)entity->next.next;
  }

  return NULL;
}

/******************************************************************************
  \brief SRSP received indication.

  \param[in] srsp - frame, which keeps serialized SRSP.

  \return None.
 ******************************************************************************/
void zsiSrspReceived(ZsiCommandFrame_t *const srsp)
{
  zsiDriver()->srsp = srsp;
}

/* eof zsiDriver.c */
