/**************************************************************************//**
  \file zdoCommands.h

  \brief Interface of ZDO commands: ZDP commands, stack requests and etc.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2012-10-29 Max Gekk - Created.
   Last change:
    $Id: zdoCommands.h 23840 2012-12-13 10:00:01Z mgekk $
 ******************************************************************************/
#if !defined _ZDO_COMMANDS_H
#define _ZDO_COMMANDS_H

/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <zdo.h>

/******************************************************************************
                             Definitions section
 ******************************************************************************/
#define ZDP_RESP_HEADER_SIZE 2U /*!< ZDO status + Transaction Sequence Number */
#define ZDP_REQ_HEADER_SIZE  1U /*!< Transaction Sequence Number */

/******************************************************************************
                                Types section
 ******************************************************************************/
/** Type of a ZDO command which can be allocated for ZDP command, NWK request. */
typedef struct _ZdoCommand_t
{
  /** The flag indicates that the command is used or not. */
  bool busy;
  /** Is the ZDO command response to a ZDP request unicast. */
  bool isResponseToUnicast;
  /** Stack requests. */
  ZDO_ZdpReq_t zdpReq;
  union
  {
    /** NWK request which is used to enable/disable association permit. */
    NWK_PermitJoiningReq_t permitJoining;
    /** NWK request to leave a child or this device from the network. */
    NWK_LeaveReq_t leave;
    /** MAC request to change values of a MAC attribute in MIB. */
    MAC_SetReq_t macSet;
    /** HAL Application timer to track timeouts. */
    HAL_AppTimer_t timer;
    /** NWK request to run the energy detection scan. */
    NWK_EDScanReq_t edScan;
  } req;
} ZdoCommand_t;

/******************************************************************************
                 Inline static functions prototypes section
 ******************************************************************************/
/**************************************************************************//**
  \brief Gets a pointer to APS_DataReq_t structure of the ZDO command.
   
  \param[in] zdoCommand - a pointer to allocated memory to perform a stack
                          request or to send ZDP command.
 ******************************************************************************/
INLINE APS_DataReq_t* zdoApsDataReq(ZdoCommand_t *const zdoCommand)
{
  return &zdoCommand->zdpReq.service.apsDataReq;
}

/**************************************************************************//**
  \brief Gets a pointer to the ZDP request frame of the ZDO command.
   
  \param[in] zdoCommand - a pointer to allocated memory to perform a stack
                          request or to send ZDP command.
 ******************************************************************************/
INLINE ZDO_ZdpReqFrame_t* zdoZdpReqFrame(ZdoCommand_t *const zdoCommand)
{
  return &zdoCommand->zdpReq.req.reqPayload;
}

/**************************************************************************//**
  \brief Gets a pointer to the ZDP response frame of the ZDO command.
   
  \param[in] zdoCommand - a pointer to allocated memory to perform a stack
                          request or to send ZDP command.
 ******************************************************************************/
INLINE ZDO_ZdpRespFrame_t* zdoZdpRespFrame(ZdoCommand_t *const zdoCommand)
{
  return &zdoCommand->zdpReq.req.respPayload;
}

/******************************************************************************
                              Prototypes section
 ******************************************************************************/
/**************************************************************************//**
  \brief Request to allocate memory for new ZDO command.

   This function initializes only the flag which indicates that the allocated
  entry is busy. It means that need to link apsData->asdu with zdp frame.
   
  \return Pointer to a ZDO command if there is enough space otherwise NULL.
 ******************************************************************************/
ZDO_PRIVATE ZdoCommand_t* zdoAllocCommand(void);

/**************************************************************************//**
  \brief Free memory which was allocated for the ZDO command.

  \param[in] zdoCommandField - pointer to any field of the ZDO command.
 ******************************************************************************/
ZDO_PRIVATE void zdoFreeCommand(void *zdoCommandField);

/**************************************************************************//**
  \brief Free all entries of the ZDO commands pool.
 ******************************************************************************/
ZDO_PRIVATE void zdoResetCommandsPool(void);

/******************************************************************************
  \brief Fill common parameters for the ZDP response.

  \param[in] apsDataInd - pointer to the APSDE-DATA indication structure of the
                          received ZDP command request.
  \param[in, out] zdoCommand - pointer to the ZDO Command structure which
                               is allocated for outgoing ZDP response or
                               a stack request.
  \param[in] APS_DataConf - callback function for apsDataReq
  \return None.
 ******************************************************************************/
ZDO_PRIVATE void zdoPrepareResponseParameters(const APS_DataInd_t *const apsDataInd,
  ZdoCommand_t *const zdoCommand,
  void (*APS_DataConf)(APS_DataConf_t *conf));

/******************************************************************************
  \brief Response is sent. Return memory into the ZDO Commands Pool.

  \param[in] apsDataConf - pointer to APSDE-DATA confirmation structure of
                           ZDP command response.
  \return None.
 ******************************************************************************/
ZDO_PRIVATE void zdoZdpResponseConf(APS_DataConf_t *apsDataConf);

#endif /* _ZDO_COMMANDS_H */
/** eof zdoCommands.h */
