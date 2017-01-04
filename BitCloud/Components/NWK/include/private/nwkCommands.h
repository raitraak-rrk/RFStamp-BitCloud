/**************************************************************************//**
  \file nwkCommands.h

  \brief NWK commands header file.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2007-06-20 V. Panov - Created.
    2009-04-28 M. Gekk  - Refactoring.
   Last change:
    $Id: nwkCommands.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#if !defined _NWK_COMMANDS_H
#define _NWK_COMMANDS_H

/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <nwkConfig.h>
#include <sysTypes.h>
#include <nwkFrame.h>
#include <nwkTx.h>

/******************************************************************************
                               Definition section
 ******************************************************************************/
/* Total number of command requests. */
#define NWK_COMMAND_REQ_AMOUNT (NWK_MAX_COMMAND_TX_ID + 1U)

/******************************************************************************
                                Types section
 ******************************************************************************/
/** Type of command request counter. */
typedef uint8_t NwkCommandCount_t;

/** Internal variables of the commands component. */
typedef struct _NwkCommands_t
{
  NwkCommandCount_t reqCount[NWK_COMMAND_REQ_AMOUNT];
} NwkCommands_t;

/******************************************************************************
                              Prototypes section
 ******************************************************************************/
/**************************************************************************//**
  \brief Request to perform outgoing command allocated in data buffer.

  \param[in] txId - the transmission identifier of a command
                    which is requested to send.
  \return None.
 ******************************************************************************/
NWK_PRIVATE void nwkCommandReq(const NwkTxId_t txId);

/**************************************************************************//**
  \brief Action when any command received.

  \param[in] payload - command payload (with ID)
  \param[in] header  - pointer to network frame header.
  \param[in] parse   - parsed header fields
  \return 'true' if continue command processing otherwise 'false'.
******************************************************************************/
NWK_PRIVATE bool nwkCommandInd(const uint8_t *const payload,
  const NwkFrameHeader_t *const header, const NwkParseHeader_t *const parse);

/**************************************************************************//**
  \brief The packet for NWK command has been released.

  \return 'true' if free packet has been allocated otherwise return 'false'.
 ******************************************************************************/
NWK_PRIVATE bool nwkCommandFreePacketInd(void);

/**************************************************************************//**
  \brief Reset the commands component.
 ******************************************************************************/
NWK_PRIVATE void nwkResetCommands(void);

#endif /* _NWK_COMMANDS_H */
/** eof nwkCommands.h */

