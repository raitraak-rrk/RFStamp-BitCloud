/**************************************************************************//**
  \file machwdCalibHandler.h

  \brief MAC HWD interface of RF calibration.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2011-10-11 Max Gekk - Created.
   Last change:
    $Id: machwdCalibHandler.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#ifndef _MACHWDCALIBHANDLER_H
#define _MACHWDCALIBHANDLER_H

/******************************************************************************
                                Includes section
 ******************************************************************************/
#include <machwdCalibration.h>
#include <machwdHandlerCtrl.h>

/******************************************************************************
                               Prototypes section
 ******************************************************************************/
/******************************************************************************
  \brief Try to perform RF calibration.

  \param[in] request - pointer to paramters of the original calibration request.
  \return result of FTN and PLL calibration request.
 *****************************************************************************/
MachwdHandlerResult_t machwdCalibReqHandler(MACHWD_CalibrationReq_t *request);

/******************************************************************************
  \brief Start calibration procedure if PHY is idle otherwise postpone the task.

  \return result of RF calibration.
 *****************************************************************************/
MachwdHandlerResult_t machwdCalibTaskHandler(void);

/******************************************************************************
  \brief Reset internal state and clear variables of the calibration module.
 *****************************************************************************/
void machwdResetCalibHandler(void);

#endif /* _MACHWDCALIBHANDLER_H */
/** eof machwdCalibHandler.h */
