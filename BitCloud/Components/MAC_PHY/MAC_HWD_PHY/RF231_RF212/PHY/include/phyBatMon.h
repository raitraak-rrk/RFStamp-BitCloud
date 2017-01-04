/******************************************************************************
  \file phyBatMon.h

  \brief Prototypes of battery monitor functions and corresponding types.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      16/04/08 A. Mandychev - Created.
******************************************************************************/

#ifndef _PHYBATMON_H
#define _PHYBATMON_H

/******************************************************************************
                    Includes section
******************************************************************************/
#include <rfBattery.h>
#include <phyCommon.h>
#include <machwdBatMonHandler.h>
#include <phyMemAccess.h>

/******************************************************************************
                    Types section
******************************************************************************/
typedef RF_BatteryMonReq_t PHY_BatMonReq_t;

/******************************************************************************
                 Inline static functions prototypes section.
******************************************************************************/
/******************************************************************************
  Inidicates that supply voltage drop below the configured threshold.
  Parameters: none.
  Returns:    none
******************************************************************************/
INLINE void phyBatMonHandler(void)
{
  MACHWD_BatMonInd();
}

/******************************************************************************
                    Prototypes section
******************************************************************************/
/******************************************************************************
  Sets battery monitor voltage.
  Parameters:
    reqParams - request parameters structure pointer.
  Returns: status of operation. PHY_BUSY_RX_REQ_STATUS and
    PHY_SUCCESS_REQ_STATUS are all possible.
******************************************************************************/
PHY_ReqStatus_t PHY_BatMonReq(PHY_BatMonReq_t *req);

#endif /*_PHYBATMON_H*/

// eof phyBatMon.h
