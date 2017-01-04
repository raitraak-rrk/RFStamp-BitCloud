/******************************************************************************
  \file machwiBeaconHandler.h

  \brief Declares beacon routine functions.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \intrenal
    History:
      01/07/07 ALuzhetsky - Created.
******************************************************************************/

#ifndef _MACHWIBEACONHANDLER_H
#define _MACHWIBEACONHANDLER_H
#ifdef _FFD_

/******************************************************************************
                        Includes section.
******************************************************************************/
#include <sysTypes.h>
#include <macFrame.h>

/******************************************************************************
                        Global variables.
******************************************************************************/

/******************************************************************************
                        Prototypes section
******************************************************************************/

/******************************************************************************
                        Implementations section.
******************************************************************************/
/**************************************************************************//**
  \brief Forms and sends MAC Beacon frame.

  \param none.
  \return none.
******************************************************************************/
void machwiSendBeacon(void);

#endif // _FFD_
#endif //_MACHWIBEACONHANDLER_H

// eof machwiBeaconHandler.h
