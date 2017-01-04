/**************************************************************************//**
  \file nwkUpdateCommand.h

   \brief Update command header file.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2008-12-10 M. Gekk - Reduction of usage of the RAM at separate compiling.
    2009-06-08 M. Gekk - Refactoring.
   Last change:
    $Id: nwkUpdateCommand.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#if !defined _NWK_UPDATE_COMMAND_H
#define _NWK_UPDATE_COMMAND_H

/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <nwkConfig.h>
#include <nwkSystem.h>
#include <mac.h>
#include <nwkCommon.h>
#include <appTimer.h>
#include <nwkFrame.h>

/******************************************************************************
                              Definitions section
 ******************************************************************************/
/** Types of update information command. */
#define NWK_UPDATE_TYPE_PANID_CONFLICT 0U
/** Minimum size of the update command payload without update information field.
 **/
#define NWK_SIZE_OF_UPDATE_CMD_PAYLOD 13U

/******************************************************************************
                                 Types section
 ******************************************************************************/
/** Internal states of the update command component. */
typedef enum _NwkUpdateCommandState_t
{
  /** State after reset */
  NWK_UPDATE_IDLE_STATE = 0x92,
  NWK_UPDATE_FIRST_STATE = 0x19,
  NWK_UPDATE_WAIT_BROADCAST_DELIVERY_STATE = NWK_UPDATE_FIRST_STATE,
  NWK_UPDATE_MAC_SET_PAN_ID_STATE = 0x1B,
  NWK_UPDATE_LAST_STATE
} NwkUpdateCommandState_t;

/** Confirmation of setting a new pan id. */
typedef void (* NwkUpdatePanIdConf_t)(void);

/** Internal variables of the update command component. */
typedef struct _NwkUpdateCommand_t
{
  /** Finite-state machine */
  NwkUpdateCommandState_t state;
  PanId_t newPanId;
  NwkUpdateId_t updateId;
  NwkUpdatePanIdConf_t callback;
  union
  {
    HAL_AppTimer_t timer;
    MAC_SetReq_t macSet;
  } req;
} NwkUpdateCommand_t;

/** Payload of network update command. ZigBee spec r17, Figure 3.28. */
BEGIN_PACK
typedef struct PACK _NwkUpdateCommandPayload_t
{
  /** Id of the update command. ZigBee spec r17, Table 3.40. */
  NwkCommandIdField_t commandId;
  /** The update information count sub-field contains an integer indicating
   * the number of records contained within the Update Information field. */
  LITTLE_ENDIAN_OCTET(2, (
    NwkBitField_t infoCount    :5,
    /** The update command identifier sub-field contains an integer indicating
     * the type of update information command.
     * ZigBee spec r17, Figure 3.30, page 337. */
    NwkBitField_t updateCmdId  :3
  ))
  /** The EPID field shall contain the 64bit EPID that identifies the network
   * that is to be updated. ZigBee spec r17, 3.4.10.3.2, page 337. */
  ExtPanId_t extPanId;
  /** The update information field provides the information being updated,
   * the format of this field depends upon the value of the Update Command
   * Identifier sub-field. ZigBee spec r17, 3.4.10.3.4, page 337. */
  NwkUpdateId_t updateId;
  /** The PAN identifier update shall be made up of a single 16-bit PAN
   * identifier that is the new PAN identifier for this network to use.
   * ZigBee spec r17, 3.4.10.3.4.1, page 338. */
  PanId_t newPanId;
} NwkUpdateCommandPayload_t;
END_PACK

/******************************************************************************
                             Prototypes section
 ******************************************************************************/
#if defined _RESOLVE_PANID_CONFLICT_
/**************************************************************************//**
  \brief Set a new pan id.

  \param[in] newPanId - a new identifier of PAN.
  \param[in] updateId - a new update identifier.
  \param[in] callback - pointer to a update confirmation function.

  \return None.
 ******************************************************************************/
NWK_PRIVATE void nwkUpdatePanId(const PanId_t newPanId,
  const NwkUpdateId_t updateId, const NwkUpdatePanIdConf_t callback);

/**************************************************************************//**
  \brief Action when a update command is received.

  \param[in] payload - pointer to NPDU of NWK update command.
  \param[in] header  - nwk header.
  \param[in] parse   - parsed header fields.

  \return 'true' - continue processing command otherwise 'false'.
 ******************************************************************************/
NWK_PRIVATE bool nwkUpdateFrameInd(const uint8_t *const payload,
  const NwkFrameHeader_t *const header, const NwkParseHeader_t *const parse);

/**************************************************************************//**
  \brief Reset the update command component.
 ******************************************************************************/
NWK_PRIVATE void nwkResetUpdateCommand(void);

#else /* _RESOLVE_PANID_CONFLICT_ */

/* Interface of the update command is disabled. */
#define nwkUpdatePanId(newPanId,updateId,callback)
#define nwkUpdateFrameInd NULL
#define nwkResetUpdateCommand() (void)0

#endif /* _RESOLVE_PANID_CONFLICT_ */
#endif /* _NWK_UPDATE_COMMAND_H */
/** eof nwkUpdateCommand.h */

