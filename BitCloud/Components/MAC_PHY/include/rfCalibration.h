/**************************************************************************//**
  \file rfCalibration.h

  \brief Interface for PLL calibration and filter tuning.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2011-10-04 Max Gekk - Created.
   Last change:
    $Id: rfCalibration.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#if !defined _RFCALIBRATION_H
#define _RFCALIBRATION_H

/******************************************************************************
                                Includes section
 ******************************************************************************/
#include <macCommon.h>

/******************************************************************************
                                 Types section
 ******************************************************************************/
/**
 * \brief RF calibration status type.
 *
 * - RF_CALIBRATION_SUCCESS_STATUS means that the calibration cycle has finished
 *     successfuly and the radio chip is ready to work.
 * - RF_CALIBRATION_FAIL_STATUS - MAC hardware depended subsystem cannot preform
 *   the calibration at this time.
 **/
typedef enum _RF_CalibrationStatus_t
{
  RF_CALIBRATION_SUCCESS_STATUS,
  RF_CALIBRATION_FAIL_STATUS
} RF_CalibrationStatus_t;

/**//**
 * \brief The structure declaration of RF calibration confirmation parameters.
 */
typedef struct _RF_CalibrationConf_t
{
  RF_CalibrationStatus_t status;
} RF_CalibrationConf_t;

/**//**
 * \brief Parameters of RF Calibration request.
 */
typedef struct _RF_CalibrationReq_t
{
  /*! Service field - for internal needs. */
  MAC_Service_t  service;

  /** RF Calibration confirm callback function's pointer. */
  void (*RF_CalibrationConf)(RF_CalibrationConf_t *conf);
  /** Status of processing this request. */
  RF_CalibrationConf_t confirm;
} RF_CalibrationReq_t;

/******************************************************************************
                              Prototypes section
 ******************************************************************************/
/**************************************************************************//**
  \brief initiates a calibration cycle.

  \param[in] req - pointer to calibration parameters - callback and conf status.
  \return None.
 ******************************************************************************/
void RF_CalibrationReq(RF_CalibrationReq_t *const req);

#endif /* _RFCALIBRATION_H */
/** eof rfCalibration.h */
