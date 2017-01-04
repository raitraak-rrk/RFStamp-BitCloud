/**************************************************************************//**
  \file zdoMgmtNwkUpdate.h

  \brief Interface of ZDO Mgmt Nwk Update Request/Notify.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2012-12-18 Max Gekk - Created.
   Last change:
    $Id: zdoMgmtNwkUpdate.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#if !defined _ZDO_MGMT_NWK_UPDATE_H
#define _ZDO_MGMT_NWK_UPDATE_H

/******************************************************************************
                                Includes section
 ******************************************************************************/
#include <appTimer.h>
#include <aps.h>
#include <zdoCommands.h>

/******************************************************************************
                                 Types section
 ******************************************************************************/
/** Enumerations of internal states of the component. */
typedef enum _ZdoMgmtNwkUpdateState_t
{
  ZDO_MGMT_NWK_UPDATE_IDLE_STATE,
  ZDO_MGMT_NWK_UPDATE_SEND_ERROR_STATE,
  ZDO_MGMT_NWK_UPDATE_SET_TIMER_STATE,
  ZDO_MGMT_NWK_UPDATE_SET_CHANNEL_STATE,
  ZDO_MGMT_NWK_UPDATE_ED_SCAN_STATE,
  ZDO_MGMT_NWK_UPDATE_ED_SCAN_REPORT_STATE
} ZdoMgmtNwkUpdateState_t;

/** Internale state and variable to process ZDO Mgmt NWK Update commands. */
typedef struct _ZdoMgmtNwkUpdate_t
{
  /* Internal state of the Mgmt Nwk Update component. */
  ZdoMgmtNwkUpdateState_t state;
  /* Pointer to allocated ZDO Command to perform requests to the stack and
   * send ZDP Mgmt Nwk Update Notify/Request commands. */  
  ZdoCommand_t *zdoCommand;
  /** This field represents the number of energy scans to be conducted and
   * reported. */
  uint8_t scanCount;
  /** Transaction Sequence Number of the original Mgmt Nwk Update Request. */
  uint8_t seqNum;
} ZdoMgmtNwkUpdate_t;

/******************************************************************************
                               Prototypes section
 ******************************************************************************/
/******************************************************************************
  \brief Prepare Mgmt Nwk Update Request command payload.
  \param[in] zdpReq - the pointer to the ZDP request's structure for
                      the Mgmt NWK Update Request Command.
  \return None.
 ******************************************************************************/
ZDO_PRIVATE void zdoPrepareMgmtNwkUpdateRequest(ZDO_ZdpReq_t *zdpReq);

/**************************************************************************//**
  \brief Prepare Mgmt Nwk Update Notify command payload.
  \param[in] zdpReq - the pointer to the ZDP request's structure for
                      the Mgmt NWK Update Notify Command.
  \return None.
 ******************************************************************************/
ZDO_PRIVATE void zdoPrepareMgmtNwkUpdateNotify(ZDO_ZdpReq_t *zdpReq);

/******************************************************************************
  \brief Process a Mgmt Nwk Update Request command.
 
  \param[in] apsDataInd - pointer to APSDE-DATA indication parameters structure
                          of received Mgmt Nwk Update Request command.
  \return None.
 ******************************************************************************/
ZDO_PRIVATE void zdoMgmtNwkUpdateInd(const APS_DataInd_t *const apsDataInd);

/******************************************************************************
  \brief Process Mgmt Nwk Update Notify command.
 
  \param[in] apsDataInd - pointer to APSDE-DATA indication parameters structure
                          of received Mgmt Nwk Update Notify response command.
  \return None.
 ******************************************************************************/
ZDO_PRIVATE void zdoMgmtNwkUpdateNotifyInd(const APS_DataInd_t *const apsDataInd);

/**************************************************************************//**
  \brief Resets the Mgmt Nwk Update module to the idle state.
 ******************************************************************************/
ZDO_PRIVATE void zdoResetMgmtNwkUpdate(void);

#ifdef _FREQUENCY_AGILITY_
/**************************************************************************//**
  \brief Reports to the NWK Manager about link problems.

  \return 'true' if the procedure was initiated successfully otherwise 'false'.
 ******************************************************************************/
ZDO_PRIVATE bool zdoBadLinkReport(void);
#endif // _FREQUENCY_AGILITY_
#endif /* _ZDO_MGMT_NWK_UPDATE_H */
/** eof zdoMgmtNwkUpdate.h */
