/******************************************************************************
  \file machwdRxFrameHandler.h

  \brief Prototypes of Rx Frame handler.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    04/06/07 A. Mandychev - Created.
  Last change:
    $Id: machwdRxFrameHandler.h 27584 2015-01-09 14:45:42Z unithra.c $
******************************************************************************/

#ifndef _MACHWDRXFRAMEHANDLER_H
#define _MACHWDRXFRAMEHANDLER_H

/******************************************************************************
                        Includes section.
******************************************************************************/
#include <machwdManager.h>
#include <machwdHandlerCtrl.h>

/******************************************************************************
                        Prototypes section.
******************************************************************************/
#if defined(_FFD_)
/******************************************************************************
  \brief Data request indication task handler.
  \return the result of execution.
******************************************************************************/
MachwdHandlerResult_t machwdDataReqIndTaskHandler(void);
#endif // defined(_FFD_)

/******************************************************************************
  \brief Data indication task handler.
  \return the result of execution.
******************************************************************************/
MachwdHandlerResult_t machwdRxFrameIndTaskHandler(void);

#endif /* MACHWDRXFRAMEHANDLER_H */

// eof machwdRxFrameHandler.h
