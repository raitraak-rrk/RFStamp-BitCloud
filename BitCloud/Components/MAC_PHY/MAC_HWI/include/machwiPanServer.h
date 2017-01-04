/******************************************************************************
  \file machwiPanServer.h

  \brief machwiPanServer header file.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      27/06/07 ALuzhetsky - Created.
******************************************************************************/

#ifndef _MACHWIPANSERVER_H
#define _MACHWIPANSERVER_H

/******************************************************************************
                        Includes section.
******************************************************************************/

/******************************************************************************
                        Types section
******************************************************************************/

/******************************************************************************
                        Prototypes section.
******************************************************************************/
#include <sysTypes.h>
#include <machwd.h>

/******************************************************************************
  Indicates that frame was received.
  Parameters:
    indParams - parameters of indication.
  Returns:
    none.
******************************************************************************/
void MACHWD_DataInd(MACHWD_DataInd_t *indParams);

#ifdef _FFD_
/******************************************************************************
  Enable beacon transmission on beacon request.
  Parameters:
    none.
  Returns:
    none.
******************************************************************************/
void machwiEnableBeaconTransmission(void);

/******************************************************************************
  Resets PanServer logic. MAC doesn't transmit beacon on beacon request after
  resetting.
  Parameters:
    none.
  Returns:
    none.
******************************************************************************/
void machwiResetPanServer(void);
#endif //_FFD_

#endif //_MACHWIPANSERVER_H

// eof machwiPanServer.h
