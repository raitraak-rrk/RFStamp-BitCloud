/**************************************************************************//**
  \file zsiDriver.h

  \brief ZAppSI driver interface

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2011-08-23  A. Razinkov - Created.
   Last change:
    $Id: zsiDriver.h 24441 2013-02-07 12:02:29Z akhromykh $
 ******************************************************************************/

#ifndef _ZSIDRIVER_H
#define _ZSIDRIVER_H

/*****************************************************************************
                              Includes section
******************************************************************************/
#include <zsiFrames.h>
#include <sysQueue.h>
#include <appTimer.h>
#include <sysUtils.h>
#include <zsiTaskManager.h>

/******************************************************************************
                              Defines section
******************************************************************************/
#define ZSI_ENTER_SYNCHRONOUS_MODE() zsiMediumPerformHalHoldTasks()
#define ZSI_LEAVE_SYNCHRONOUS_MODE() zsiMediumReleaseAllHeldTasks()

/******************************************************************************
                              Types section
******************************************************************************/

typedef enum _ZsiDriverState_t
{
  ZSI_DRIVER_STATE_INIT,
  ZSI_DRIVER_STATE_IDLE,
  ZSI_DRIVER_STATE_BLOCKED
} ZsiDriverState_t;

typedef struct _ZsiDriver_t
{
  ZsiDriverState_t   state;
  uint8_t            sequenceNumber;
  QueueDescriptor_t  bearingEntities;
  QueueDescriptor_t  postponedAreqs;
  QueueDescriptor_t  commandsToReceive;
  QueueDescriptor_t  completedAreqs;
  ZsiCommandFrame_t  *srsp;
} ZsiDriver_t;

typedef struct _ZsiEntityService_t
{
  /* Link to next element for queue support. */
  QueueElement_t next;
  /* to be compatible with MAC and SSP service. */
  uint8_t requestId;
  /* Sequence number to identify request-response pair. */
  uint8_t sequenceNumber;
  union
  {
    /* Callback wrapper routine pointer */
    void (*callback)(void *);
    /* Processing routine pointer to call after required memory will become free */
    void (*process)(void *);
  } zsi;
} ZsiEntityService_t;

/**************************************************************************//**
\brief The flags are used for releasing of buffers after serialization\deserialization. 
Buffer will be released if flag is set. Do not release free buffers.
Memory allocates table for s\d routines:
                     | frame | memory
--------------------------------------
AREQ serialization   |   +   |   -
AREQ deserialization |   +   |   +
SREQ serialization   |   -   |   -
SREQ deserialization |   +   |   +
SRSP serialization   |   It's one case with SREQ deserialization
SRSP deserialization |   +   |   -
******************************************************************************/
typedef struct _ZsiProcessingResult_t
{
  bool keepMemory;
  bool keepCmdFrame;
} ZsiProcessingResult_t;

typedef ZsiProcessingResult_t (*ZsiSerializeRoutine_t)(const void *const,
  ZsiCommandFrame_t *const);

typedef ZsiProcessingResult_t (*ZsiProcessingRoutine_t)(void *, ZsiCommandFrame_t *const);

typedef ZsiProcessingRoutine_t (*ZsiDomainProcessingRoutine_t)(uint8_t);

/******************************************************************************
                             Prototypes section
 ******************************************************************************/
/******************************************************************************
  \brief ZAppSI driver reset routine.
 ******************************************************************************/
void zsiResetDriver(void);

/******************************************************************************
  \brief ZAppSI driver task handler.

  \return None.
 ******************************************************************************/
void zsiDriverTaskHandler(void);

/******************************************************************************
  \brief Store received command for further processing.

  \param[in] cmdFrame - frame, which keeps serialized data.

  \return None.
 ******************************************************************************/
void zsiDriverStoreRxCmd(const ZsiCommandFrame_t *const cmdFrame);

/******************************************************************************
  \brief AREQ processing completed. Callback to application can be used.

  \param[in] areq - entity, which bore AREQ.

  \return None.
 ******************************************************************************/
void zsiAreqProcessingComplete(void *const areq);

/******************************************************************************
  \brief Memory available notification from memory manager.

  \return None.
 ******************************************************************************/
INLINE void zsiMemoryAvailableNtfy(void)
{
  zsiPostTask(ZSI_DRIVER_TASK_ID);
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
  ZsiSerializeRoutine_t serialize, void *const dataOut);

/******************************************************************************
  \brief Pass command for transmission to HOST or NP.

  \param[in] command - prepared command to transmit.

  \return None.
 ******************************************************************************/
void zsiCommandReq(ZsiCommandFrame_t *const command);

/******************************************************************************
  \brief New command indicated from medium.

  \param[in] command - indicated command.

  \return None.
 ******************************************************************************/
void zsiCommandInd(const ZsiCommandFrame_t *const command);

/******************************************************************************
  \brief SRSP received indication.

  \param[in] srsp - frame, which keeps serialized SRSP.

  \return None.
 ******************************************************************************/
void zsiSrspReceived(ZsiCommandFrame_t *const srsp);

/******************************************************************************
  \brief ZAppSI frames sequence number getter routine.

  \return Frame sequence number.
 ******************************************************************************/
uint8_t zsiGetSequenceNumber(void);

/******************************************************************************
  \brief ZAppSI Driver prepared command transmission interface.

  \param[in] cmdFrame - frame, which keeps serialized command.
  \param[out] dataOut - pointer to memory to put returned value in case of SREQ.

  \return None.
 ******************************************************************************/
void zsiDriverSendCommand(ZsiCommandFrame_t *const cmdFrame, void *const dataOut);

/******************************************************************************
  \brief ZAppSI Driver received command processing routine.

  \param[in] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return None.
 ******************************************************************************/
void zsiDriverReceiveCommand(uint8_t *memory, ZsiCommandFrame_t *const cmdFrame);

/******************************************************************************
  \brief Stores AREQ-bearing user entity for further linking with
         corresponding response.

  \param[in] handle - pointer to user entity from wich AREQ spawned.
  \param[in] sequenceNumber - uniq sequence number of the frame. For correct
                              AREQ and ARSP linking.

  \return None.
 ******************************************************************************/
void zsiDriverKeepBearingEntity(void *handle, uint8_t sequenceNumber);

/**************************************************************************//**
  \brief Returns AREQ's bearing entity by it's sequence number.

  \param[in] sequenceNumber - entity sequence number.

  \return AREQ's bearing entity.
 ******************************************************************************/
void *zsiDriverReturnBearingEntity(uint8_t sequenceNumber);

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
  uint8_t sequenceNumber, uint8_t type, uint8_t domain, uint8_t commandId);

/******************************************************************************
  \brief Performs holding of all HAL tasks except particular medium channel one.
         Should be used during SREQ/SRSP iteration.
         HAL tasks releasing should be performed as soon as possible.

  \return None.
 ******************************************************************************/
void zsiMediumPerformHalHoldTasks(void);

/******************************************************************************
  \brief Releasing all previously held HAL tasks.

  \return None.
 ******************************************************************************/
void zsiMediumReleaseAllHeldTasks(void);

#endif /* _ZSIDRIVER_H */
/* eof zsiDriver.h */
