/**************************************************************************//**
  \file nwkManager.h

  \brief Interface of the network manager.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2009-06-08 Max Gekk - Created.
   Last change:
    $Id: nwkManager.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#if !defined _NWK_MANAGER_H
#define _NWK_MANAGER_H

/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <nwkConfig.h>
#include <appFramework.h>
#include <mac.h>
#include <nldeData.h>
#include <nwkFrame.h>

/******************************************************************************
                              Define(s) section
 ******************************************************************************/
#define NWK_UPDATE_TX_PARAMETERS \
  {NWK_TX_DELAY_BROADCAST, nwkPrepareUpdateTx, nwkConfirmUpdateTx, true}

/******************************************************************************
                                Types section
 ******************************************************************************/
/** Internal states of the report command component */
typedef enum _NwkManagerState_t
{
  NWK_MANAGER_IDLE_STATE = 0xFB,
  NWK_MANAGER_FIRST_STATE = 0x83,
  NWK_MANAGER_MAC_SCAN_STATE = NWK_MANAGER_FIRST_STATE,
  NWK_MANAGER_PREPARE_MAC_DATA_STATE = 0x84,
  NWK_MANAGER_SEND_UPDATE_CMD_STATE = 0x85,
  NWK_MANAGER_SET_PAN_ID_STATE = 0x86,
  NWK_MANAGER_LAST_STATE
} NwkManagerState_t;

/** Internal variables of the network manager component. */
typedef struct _NwkManager_t
{
  /** Finite-state machine */
  NwkManagerState_t state;
  PanId_t newPanId; /* New non conflicted panId. */
  /* Range non conflicted panId from newPanId field. */
  PanId_t nonConflictRange;
  /** MLME-SCAN request primitive's parameters. */
  MAC_ScanReq_t macScan;
} NwkManager_t;

/******************************************************************************
                                Prototypes section
 ******************************************************************************/
#if defined _RESOLVE_PANID_CONFLICT_ && defined _NETWORK_MANAGER_
/**************************************************************************//**
  \brief Processing a conflict of panId by the network manager.
 ******************************************************************************/
NWK_PRIVATE void nwkDetectPanIdConflictByNwkManager(void);

/**************************************************************************//**
  \brief Reset the report command component.
 ******************************************************************************/
NWK_PRIVATE void nwkResetNetworkManager(void);

/**************************************************************************//**
  \brief Action when a report command is received.

  \param[in] payload - pointer to NPDU of NWK report command.
  \param[in] header  - nwk header.
  \param[in] parse   - parsed header fields.
  \return 'true' - continue command processing otherwise 'false'.
******************************************************************************/
NWK_PRIVATE bool nwkReportFrameInd(const uint8_t *const payload,
  const NwkFrameHeader_t *const header, const NwkParseHeader_t *const parse);

/**************************************************************************//**
  \brief Prepare header and payload of the update command.

  \param[in] outPkt - pointer to output packet.
  \return None.
 ******************************************************************************/
NWK_PRIVATE void nwkPrepareUpdateTx(NwkOutputPacket_t *const outPkt);

/**************************************************************************//**
  \brief Confirmation of the update command transmission.

  \param[in] outPkt - pointer to output packet.
  \param[in] status - network status of the update command transmission.
  \return None.
 ******************************************************************************/
NWK_PRIVATE void nwkConfirmUpdateTx(NwkOutputPacket_t *const outPkt,
  const NWK_Status_t status);

/******************************************************************************
  \brief nwkNetworkManager idle checking.

  \return true, if nwkNetworkManager performs no activity, false - otherwise.
 ******************************************************************************/
NWK_PRIVATE bool nwkNetworkManagerIsIdle(void);

#else /* _RESOLVE_PANID_CONFLICT_ and _NETWORK_MANAGER_ */

/* Interface of the network manager is disabled. */
INLINE void nwkDetectPanIdConflictByNwkManager(void) {}
#define nwkResetNetworkManager() (void)0
#define nwkReportFrameInd NULL
#define nwkPrepareUpdateTx NULL
#define nwkConfirmUpdateTx NULL
#define nwkNetworkManagerIsIdle NULL

#endif /* _RESOLVE_PANID_CONFLICT_ and _NETWORK_MANAGER_ */
#endif /* _NWK_MANAGER_H */
/* eof nwkManager.h */

