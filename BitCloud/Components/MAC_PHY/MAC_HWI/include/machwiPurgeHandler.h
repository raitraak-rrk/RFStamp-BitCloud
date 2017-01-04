/******************************************************************************
  \file machwiPurgeHandler.h

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      28/08/07 A. Mandychev - Created.
******************************************************************************/

#ifndef _MACHWIPURGEHANDLER_H
#define _MACHWIPURGEHANDLER_H
#ifdef _FFD_

/******************************************************************************
                    Includes section
******************************************************************************/
#include <sysTypes.h>
#include <macPurge.h>
#include <machwiManager.h>

/******************************************************************************
                    Define(s) section
******************************************************************************/

/******************************************************************************
                    Types section
******************************************************************************/

/******************************************************************************
                    Constants section
******************************************************************************/

/******************************************************************************
                    External variables
******************************************************************************/

/******************************************************************************
                    Prototypes section
******************************************************************************/
/******************************************************************************
 Purges an MSDU from the transaction queue.
 Parameters:
   none.
 Returns:
   current status of operation (always success).
******************************************************************************/
MachwiHandlerResult_t machwiPurgeReqHandler(void);

#endif //_FFD_
#endif /* _MACHWIPURGEHANDLER_H */

// eof machwiPurgeHandler.h
