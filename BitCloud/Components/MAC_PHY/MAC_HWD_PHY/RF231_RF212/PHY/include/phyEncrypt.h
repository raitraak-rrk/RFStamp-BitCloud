/******************************************************************************
  \file machwdEncryptHandler.h

  \brief Prototype of ecnryption control function.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      18/01/08 A. Mandychev - Created.
******************************************************************************/

#ifndef _PHYENCRYPT_H
#define _PHYENCRYPT_H

/******************************************************************************
                    Includes section
******************************************************************************/
#include <sysTypes.h>
#include <phyCommon.h>
#include <rfEncrypt.h>

/******************************************************************************
                    Types section
******************************************************************************/
typedef RF_EncryptReq_t PHY_EncryptReq_t;

/******************************************************************************
                    Prototypes section
******************************************************************************/
/******************************************************************************
  Does encryption command.
  Parameters:
    request - pointer to request structure.
  Returns: status of operation. PHY_BUSY_RX_REQ_STATUS and
    PHY_SUCCESS_REQ_STATUS are all possible.
******************************************************************************/
PHY_ReqStatus_t PHY_Encrypt(PHY_EncryptReq_t *request);

#endif /*_PHYENCRYPT_H*/

// eof phyEncrypt.h
