/****************************************************************************//**
  \file scsiProtocol.h

  \brief Declaration of scsi protocol commands.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    29/08/11 N. Fomin - Created
*******************************************************************************/
#ifndef _SCSIPROTOCOL_H
#define _SCSIPROTOCOL_H

/******************************************************************************
                   Includes section
******************************************************************************/
#include <sysTypes.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
// Maximum response length for scsi command
#define MAX_COMMAND_RESPONSE_LENGTH 36

/******************************************************************************
                   Prototypes section
******************************************************************************/
/**************************************************************************//**
\brief Checks if received scsi command is supported.
\param[in]
  data - pointer to received command with parameters.
\return
  false - command is not supported;
  true - command is supported.
******************************************************************************/
bool scsiIsValidCommand(uint8_t *data);

/**************************************************************************//**
\brief Checks if any response or data transfer needed for received
       scsi command.
\param[in]
  command - received command.
\return
  false - response is not needed;
  true - response is needed.
******************************************************************************/
bool scsiIsDataInOutPhaseNeeded(uint8_t *data);

/**************************************************************************//**
\brief Checks if command is read or write command.
\param[in]
  command - received command.
\return
  false - command is not read/write command;
  true - command is read/write command.
******************************************************************************/
bool scsiIsReadWriteCommand(uint8_t *data);

/**************************************************************************//**
\brief Checks if command is read command.
\param[in]
  command - received command.
\return
  false - command is not read command;
  true - command is read command.
******************************************************************************/
bool scsiIsReadCommand(uint8_t *data);

/**************************************************************************//**
\brief Blocks for read/write command.
\param[in]
  data - pointer to received command with parameters.
\return
  number of blocks to read or write from memory.
******************************************************************************/
uint16_t scsiBlocksAmount(uint8_t *data);

/**************************************************************************//**
\brief Response for scsi command.
\param[in]
  command - received command.
\param[out]
  buffer - buffer with scsi command response.
\return
  length of response
******************************************************************************/
uint8_t scsiGetCommandResponse(uint8_t *data, uint8_t *buffer);

/**************************************************************************//**
\brief Sets number of last available memory block for scsi response.
\param[in]
  lastBlock - received command.
\param[out]
  buffer - number of last available memory block
******************************************************************************/
void scsiSetCapacity(uint32_t lastBlock);

/**************************************************************************//**
\brief Block address for read/write command.
\param[in]
  data - pointer to received command with parameters.
\return
  block address for read or write from memory.
******************************************************************************/
uint32_t scsiGetBlockAddress(uint8_t *data);

#endif /* _SCSIPROTOCOL_H */
// eof scsiProtocol.h
