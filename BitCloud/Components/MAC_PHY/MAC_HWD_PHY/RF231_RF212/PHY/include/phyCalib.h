/**************************************************************************//**
  \file phyCalib.h

  \brief Phy interface of RF calibration.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2011-10-17 Max Gekk - Created.
   Last change:
    $Id: phyCalib.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#ifndef _PHYCALIB_H
#define _PHYCALIB_H

/******************************************************************************
                                Includes section
 ******************************************************************************/
#include <sysTypes.h>
#include <phyCommon.h>
#include <rfCalibration.h>

/******************************************************************************
                                 Types section
 ******************************************************************************/
typedef RF_CalibrationReq_t PHY_CalibReq_t;

/******************************************************************************
                               Prototypes section
 ******************************************************************************/
/******************************************************************************
  \brief Start RF calibration procedure - FTN and PLL calibration cycle.

  \param[in] req - pointer to the original calibration request structure.

  \return status of operation. PHY_BUSY_RX_REQ_STATUS and
    PHY_SUCCESS_REQ_STATUS are all possible.
 ******************************************************************************/
PHY_ReqStatus_t PHY_CalibReq(PHY_CalibReq_t *req);

#endif /* _PHYCALIB_H */
/** eof phyCalib.h */
