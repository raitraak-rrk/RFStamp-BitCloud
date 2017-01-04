/*************************************************************************//**
  \file machwdData.h

  \brief MACHWD data types types and function prototypes.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
      26/09/07 A. Mandychev - Created.
*****************************************************************************/

#ifndef _MACHWDDATA_H
#define _MACHWDDATA_H

/******************************************************************************
                    Includes section
******************************************************************************/
#include <machwdService.h>
#include <machwdSetTrx.h>
#include <macFrame.h>

/******************************************************************************
                    Define(s) section
******************************************************************************/

/******************************************************************************
                    Types section
******************************************************************************/
//! Set of frame transmission statuses.
typedef enum
{
  MACHWD_DATA_PENDING_DATA_STATUS,
  MACHWD_CHANNEL_ACCESS_FAIL_DATA_STATUS,
  MACHWD_NO_ACK_DATA_STATUS,
  MACHWD_SUCCESS_DATA_STATUS,
} MACHWD_DataStatus_t;

//! MACHWD data confirm structure.
typedef struct
{
  //! Frame transmission status.
  MACHWD_DataStatus_t status;
} MACHWD_DataConf_t;

//! MACHWD data request structure.
typedef struct
{
  //! Service field - for internal needs.
  MACHWD_Service_t  service;
  //! Description of the frame to be transmitted.
  MAC_FrameDescr_t  frameDescr;
  //! Command to set trx state after transmitting frame.
  MACHWD_TrxCmd_t   trxCmdToSetAfterTransmitTrxState;
  //! MACHWD data transmission confirm callback function's pointer.
  void (*MACHWD_DataConf)(MACHWD_DataConf_t *confParams);
  //! MACHWD confirm structure.
  MACHWD_DataConf_t confirm;
#ifdef _ZGPD_SPECIFIC_
  uint8_t performCsma;
#endif
} MACHWD_DataReq_t;

//! MACHWD data indication structure.
typedef struct
{
  //! Received frame description.
  MAC_FrameDescr_t frameDescr;
} MACHWD_DataInd_t;

/******************************************************************************
                    Prototypes section
******************************************************************************/
/**************************************************************************//**
  \brief MACHWD data request primitive's prototype.
  \param reqParams - MACHWD data request's parameters' structure pointer.
  \return none.
******************************************************************************/
void MACHWD_DataReq(MACHWD_DataReq_t *reqParams);

/**************************************************************************//**
  \brief MACHWD data indication primitive's prototype.
  \param indParams - MACHWD data indication parameters' structure pointer.
  \return none.
******************************************************************************/
extern void MACHWD_DataInd(MACHWD_DataInd_t *indParams);

#endif /* _MACHWDDATA_H */

// eof machwdData.h
