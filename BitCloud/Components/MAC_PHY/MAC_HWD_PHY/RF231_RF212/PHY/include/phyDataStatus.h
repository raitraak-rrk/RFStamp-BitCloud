/******************************************************************************
  \file phyDataStatus.h

  \brief Statuses of data transmission.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      03/08/07 A. Mandychev - Created.
******************************************************************************/

#ifndef _PHYDATASTATUS_H
#define _PHYDATASTATUS_H

/******************************************************************************
                        Includes section.
******************************************************************************/
#include <at86rf2xx.h>

/******************************************************************************
                    Types section
******************************************************************************/
// PHY data status.
typedef enum
{
  PHY_SUCCESS_DATA_STATUS             = TRAC_STATUS_SUCCESS,
  PHY_CHANNEL_ACCESS_FAIL_DATA_STATUS = TRAC_STATUS_CHANNEL_ACCESS_FAILURE,
  PHY_NO_ACK_DATA_STATUS              = TRAC_STATUS_NO_ACK,
  PHY_DATA_PENDING_DATA_STATUS        = TRAC_STATUS_SUCCESS_DATA_PENDING,
  PHY_WAIT_FOR_ACK_DATA_STATUS        = TRAC_STATUS_WAIT_FOR_ACK,
  PHY_INVALID_DATA_STATUS             = TRAC_STATUS_INVALID,
} PHY_DataStatus_t;

#endif /* _PHYDATASTATUS_H */

// eof phyDataStatus.h
