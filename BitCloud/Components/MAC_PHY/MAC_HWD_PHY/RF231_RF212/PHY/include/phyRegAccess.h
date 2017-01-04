/**************************************************************************//**
  \file phyRegAccess.h

  \brief Phy interface of RF register accessing.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2011-10-17 Dmitry Kolmakov - Created.
   Last change:
    $Id: phyCalib.h 18970 2011-10-21 12:48:41Z mgekk $
 ******************************************************************************/
#ifndef _PHYREGACCESS_H
#define _PHYREGACCESS_H

/******************************************************************************
                                Includes section
 ******************************************************************************/
#include <sysTypes.h>
#include <phyCommon.h>
#include <rfRegAccess.h>

/******************************************************************************
                                 Types section
 ******************************************************************************/
typedef RF_RegAccessReq_t PHY_RegAccessReq_t;

/******************************************************************************
                               Prototypes section
 ******************************************************************************/
/******************************************************************************
  \brief Start writing to the radio module memory.

  \param[in] req - pointer to the original register access request structure.

  \return status of operation. PHY_BUSY_RX_REQ_STATUS and
    PHY_SUCCESS_REQ_STATUS are all possible.
 ******************************************************************************/
PHY_ReqStatus_t PHY_RegWriteReq(PHY_RegAccessReq_t *req);

/******************************************************************************
  \brief Start reading to the radio module memory.

  \param[in] req - pointer to the original register access request structure.

  \return status of operation. PHY_BUSY_RX_REQ_STATUS and
    PHY_SUCCESS_REQ_STATUS are all possible.
 ******************************************************************************/
PHY_ReqStatus_t PHY_RegReadReq(PHY_RegAccessReq_t *req);

#endif /* _PHYREGACCESS_H */
/** eof phyRegAccess.h */
