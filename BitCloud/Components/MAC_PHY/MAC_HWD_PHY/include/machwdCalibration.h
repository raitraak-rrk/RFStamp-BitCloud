/**************************************************************************//**
  \file machwdCalib.h

  \brief MACHWD interface for PLL calibration and filter tuning.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    2011-10-04 Max Gekk - Created.
  Last change:
    $Id: machwdCalibration.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#if !defined _MACHWDCALIB_H
#define _MACHWDCALIB_H

/******************************************************************************
                                Includes section
 ******************************************************************************/
#include <rfCalibration.h>

/******************************************************************************
                               Definitions section
 ******************************************************************************/
/** Redefenition of RF calibration request function name. **/
#define MACHWD_CalibrationReq RF_CalibrationReq

/******************************************************************************
                                 Types section
 ******************************************************************************/
/** Redefenition of random generation request structure. **/
typedef RF_CalibrationReq_t MACHWD_CalibrationReq_t;

/******************************************************************************
                               Prototypes section
 ******************************************************************************/
/**************************************************************************//**
  \brief Request to perform PLL and FTN calibration.

  \param[in] reqParams - pointer to calibration parameters - callback and
                         confirmation status.
  \return None.
 ******************************************************************************/
void MACHWD_CalibrationReq(MACHWD_CalibrationReq_t *reqParams);

#endif /* _MACHWDCALIB_H */
/** eof machwdCalib.h */
