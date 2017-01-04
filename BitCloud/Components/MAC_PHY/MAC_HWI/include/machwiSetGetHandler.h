/******************************************************************************
  \file machwiSetGetHandler.h

  \brief machwiSetGetHandler header file.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      21/06/07 ALuzhetsky - Created.
******************************************************************************/

#ifndef _MACHWISETGETHANDLER_H
#define _MACHWISETGETHANDLER_H
/******************************************************************************
                        Includes section
******************************************************************************/
#include <machwiManager.h>
#include <macSetGet.h>

/******************************************************************************
                   Prototypes section
******************************************************************************/
/******************************************************************************
  Gets request parameters from PIB.
  Parameters:
    none.
  Returns:
    current status of operation (always success).
******************************************************************************/
MachwiHandlerResult_t machwiGetReqHandler(void);

/**************************************************************************
  Sets request parameters to PIB and RF chip (if it is needed).
  Parameters:
    none.
  Returns:
    current status of operation (success or "in progress").
******************************************************************************/
MachwiHandlerResult_t machwiSetReqHandler(void);


#endif //_MACHWISETGETHANDLER_H

// eof machwiSetGetHandler.h
