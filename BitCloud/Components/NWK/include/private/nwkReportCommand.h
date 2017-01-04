/**************************************************************************//**
  \file nwkReportCommand.h

  \brief Report command header file.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2008-12-10 M. Gekk  - Reduction of usage of the RAM at separate compiling.
    2009-06-05 M. Gekk  - Refactoring.
   Last change:
    $Id: nwkReportCommand.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#if !defined _NWK_REPORT_COMMAND_H
#define _NWK_REPORT_COMMAND_H

/******************************************************************************
                             Includes section
 ******************************************************************************/
#include <nwkConfig.h>
#include <nwkSystem.h>
#include <mac.h>
#include <nldeData.h>
#include <nwkFrame.h>

/******************************************************************************
                            Definitions section
 ******************************************************************************/
/** Maximum size of a list of 16-bit PAN identifiers that are operating in the
 * neighborhood of the reporting device.*/
#define NWK_MAX_REPORT_PANID 16U
/** Types of report information command. */
#define NWK_REPORT_TYPE_PANID_CONFLICT 0U
/** Minimum size of the report command payload without
 * report information field. */
#define NWK_SIZE_OF_MIN_REPORT_CMD_PAYLOD 10U
#define NWK_REPORT_TX_PARAMETERS \
  {NWK_TX_DELAY_UNICAST_COMMAND, nwkPrepareReportTx, nwkConfirmReportTx, true}

/******************************************************************************
                               Types section
 ******************************************************************************/
/** Internal states of the report command component */
typedef enum _NwkReportCommandState_t
{
  /** State after reset */
  NWK_REPORT_IDLE_STATE = 0x27,
  /** States of preparing and sending a report command. */
  NWK_REPORT_FIRST_STATE = 0x51,
  NWK_REPORT_SCAN_STATE = NWK_REPORT_FIRST_STATE,
  NWK_REPORT_PREPARE_MAC_DATA_STATE = 0x52,
  NWK_REPORT_SEND_STATE = 0x54,
  NWK_REPORT_TIMEOUT_STATE = 0x55,
  NWK_REPORT_LAST_STATE
} NwkReportCommandState_t;

/** Internal variables of the report command component. */
typedef struct _NwkReportCommand_t
{
  /** Finite-state machine */
  NwkReportCommandState_t state;
  union
  {
    /** MLME-SCAN request primitive's parameters */
    MAC_ScanReq_t macScan;
    HAL_AppTimer_t timer;
  } req;
} NwkReportCommand_t;

/** Payload of network report command. ZigBee spec r17, Figure 3.24. */
BEGIN_PACK
typedef struct PACK _NwkReportCommandPayload_t
{
  /** Id of report command. ZigBee spec r17, Table 3.40. */
  NwkCommandIdField_t commandId;
  LITTLE_ENDIAN_OCTET(2, (
    /** Command Options Field. ZigBee spec r17, Figure 3.25. */
    /** The report information count sub-field contains an integer indicating
     * the number of records contained within the Report Information field.*/
    NwkBitField_t infoCount   : 5,
    /** The report command identifier sub-field contains an integer indicating
     * the type of report information command.
     * ZigBee spec r17, 3.4.9.3.1.2, page 334. */
    NwkBitField_t reportCmdId : 3
  ))
  /** The EPID field shall contain the 64-bit EPID that identifies the network
   * that the reporting device is a member of.
   * ZigBee spec r17, 3.4.9.3.2, page 334. */
  ExtPanId_t extPanId;
  /** The report information field provides the information being reported,
   * the format of this field depends upon the value of the Report Command
   * Identifier sub-field. */
  union
  {
    uint8_t info[NWK_MAX_REPORT_PANID * sizeof(PanId_t)];
    PanId_t panId[NWK_MAX_REPORT_PANID];
  } report;
} NwkReportCommandPayload_t;
END_PACK

/******************************************************************************
                              Prototypes section
 ******************************************************************************/
#if defined _RESOLVE_PANID_CONFLICT_
/**************************************************************************//**
  \brief Function detects the potential conflict of addresses.

  \param[in] beaconNotify - MLME-BEACON_NOTIFY indication parameters' pointer.
  \return None.
 ******************************************************************************/
NWK_PRIVATE
void nwkDetectPanIdConflict(const MAC_BeaconNotifyInd_t *const beaconNotify);

/**************************************************************************//**
  \brief Reset the report command component.
 ******************************************************************************/
NWK_PRIVATE void nwkResetReportCommand(void);

/**************************************************************************//**
  \brief Searching other network and fill MAC pan descriptors in configServer.

  \param[in] macScan - MLME-SCAN.request parameters.
  \param[in] MAC_ScanConf - MLME-SCAN confirm callback function.
 ******************************************************************************/
NWK_PRIVATE void nwkSearchOtherNetworks(MAC_ScanReq_t *const macScan,
  void (*MAC_ScanConf)(MAC_ScanConf_t *));

/**************************************************************************//**
  \brief Prepare header and payload of the report command.

  \param[in] outPkt - pointer to output packet.
  \return None.
 ******************************************************************************/
NWK_PRIVATE void nwkPrepareReportTx(NwkOutputPacket_t *const outPkt);

/**************************************************************************//**
  \brief Confirmation of the report command transmission.

  \param[in] outPkt - pointer to output packet.
  \param[in] status - network status of report transmission.
  \return None.
 ******************************************************************************/
NWK_PRIVATE void nwkConfirmReportTx(NwkOutputPacket_t *const outPkt,
  const NWK_Status_t status);

/******************************************************************************
  \brief nwkReportCommand idle checking.

  \return true, if nwkReportCommand performs no activity, false - otherwise.
 ******************************************************************************/
NWK_PRIVATE bool nwkReportCommandIsIdle(void);

#else /* _RESOLVE_PANID_CONFLICT_ */

/* Interface of the report command is disabled. */
#define nwkDetectPanIdConflict(beaconNotify)
#define nwkResetReportCommand() (void)0
#define nwkSearchOtherNetworks(macScan, MAC_ScanConf)
#define nwkPrepareReportTx NULL
#define nwkConfirmReportTx NULL
#define nwkReportCommandIsIdle NULL

#endif /* _RESOLVE_PANID_CONFLICT_ */
#endif /* _NWK_REPORT_COMMAND_H */
/** eof nwkReportCommand.h */

