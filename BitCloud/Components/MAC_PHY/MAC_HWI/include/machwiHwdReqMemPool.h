/******************************************************************************
  \file machwiHwdReqMemPool.h

  \brief Types and constants declaration for machwi memory management for machwd
    requests.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      19/06/07 ALuzhetsky - Created.
******************************************************************************/

#ifndef _MACHWIHWDREQMEMPOOL_H
#define _MACHWIHWDREQMEMPOOL_H

/******************************************************************************
                        Includes section
******************************************************************************/
#include <sysTypes.h>
#include <machwd.h>

/******************************************************************************
                    Define(s) section
******************************************************************************/

/******************************************************************************
                        Types section
******************************************************************************/
/******************************************************************************
  Union of possible request types for the requests to machwd module.
******************************************************************************/
typedef union
{
  MACHWD_DataReq_t        data;
  MACHWD_SetTrxStateReq_t setTrxState;
  MACHWD_SetReq_t         set;
  MACHWD_EdReq_t          ed;
} MachwiHwdReqDescr_t;

#endif //_MACHWIHWDREQMEMPOOL_H

// eof machwiHwdReqMemPool.h
