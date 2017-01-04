/******************************************************************************
  \file phyRnd.h

  \brief Prototype of random handler.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      08/04/08 A. Mandychev - Created.
******************************************************************************/

#ifndef _PHYRND_H
#define _PHYRND_H

/******************************************************************************
                    Includes section
******************************************************************************/
#include <sysTypes.h>
#include <phyCommon.h>
#include <rfRandom.h>

/******************************************************************************
                    Types section
******************************************************************************/
typedef RF_RandomReq_t PHY_RndReq_t;

/******************************************************************************
                    Prototypes section
******************************************************************************/
/******************************************************************************
  Gets random value from RF chip.
  Parameters:
    req - pointer to request structure.
  Return: status of operation. PHY_BUSY_RX_REQ_STATUS and
    PHY_SUCCESS_REQ_STATUS are all possible.
******************************************************************************/
PHY_ReqStatus_t PHY_RndReq(PHY_RndReq_t *req);

#endif /*_PHYRND_H*/

// eof phyRnd.h
