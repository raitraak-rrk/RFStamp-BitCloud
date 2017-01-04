/******************************************************************************
  \file machwiDataHandler.h

  \brief machwiDataHandler header file.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      15/06/07 ALuzhetsky - Created.
******************************************************************************/

#ifndef _MACHWIDATAHANDLER_H
#define _MACHWIDATAHANDLER_H

/******************************************************************************
                        Includes section.
******************************************************************************/
#include <machwd.h>
#include <machwiManager.h>
#include <macCommon.h>
#include <macData.h>


/******************************************************************************
                   Prototypes section
******************************************************************************/
/******************************************************************************
 Checks data request parameters, fills frame descriptor and passes request
 further.
 Parameters:
   none.
 Returns:
   current status of operation (success or "in progress").
******************************************************************************/
MachwiHandlerResult_t machwiDataReqHandler(void);

/******************************************************************************
  Converts hwd data transmission status to IEEE802.15.4 status.
  Parameters:
    status - hwd data transmission status.
  Returns:
    IEEE802.15.4 data transmission status.
******************************************************************************/
MAC_Status_t machwiConvertmachwdDataConfirmTypes(MACHWD_DataStatus_t hwdStatus);

/******************************************************************************
  Fiils data frame fields.
  Parameters:
    frameDescr - pointer to frme descriptor to be filled.
    dataReq - pointer to data request params.
  Returns:
    none.
******************************************************************************/
void machwiPrepareDataFrame(MAC_FrameDescr_t *frameDescr, MAC_DataReq_t *dataReq);


#endif /* _MACHWIDATAHANDLER_H */

// eof machwiDataHandler.h
