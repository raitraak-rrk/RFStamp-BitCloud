/******************************************************************************
  \file machwiResetHandler.h

  \brief MAC reset routine header file.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      21/06/07 ALuzhetsky - Created.
******************************************************************************/

#ifndef _MACHWIRESETHANDLER_H
#define _MACHWIRESETHANDLER_H

/******************************************************************************
                        Includes section.
******************************************************************************/
#include <machwiManager.h>


/******************************************************************************
                        Prototypes section
******************************************************************************/
/******************************************************************************
  TBD
  Parameters:
    none.
  Returns:
    none.
******************************************************************************/
MachwiHandlerResult_t machwiResetReqHandler(void);

// eof machwiResetHandler.h

#endif //_MACHWIRESETHANDLER_H
