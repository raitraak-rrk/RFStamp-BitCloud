/******************************************************************************
  \file tcPromiscuousMode.h

  \brief
    TC Promiscuous mode module interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2010 , Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2010-25-01 arazinkov - Created.
   Last change:
    $Id: tcPromiscuousMode.h 14525 2010-12-23 10:18:45Z arazinkov $
******************************************************************************/

#ifndef _TCPROMISCUOUSMODE_H_
#define _TCPROMISCUOUSMODE_H_

#ifdef _LINK_SECURITY_ 
#ifdef _TC_PROMISCUOUS_MODE_

/******************************************************************************
                              Includes section
******************************************************************************/
#include <sysTypes.h>

/******************************************************************************
                              Prototypes section
******************************************************************************/
/**************************************************************************//**
  \brief Forces TC to enter the Promiscuous mode

  \return None.
******************************************************************************/
void TC_EnterPromiscuousMode(void);

/**************************************************************************//**
  \brief Forces TC to leave the Promiscuous mode

  \return None.
******************************************************************************/
void TC_LeavePromiscuousMode(void);

/**************************************************************************//**
  \brief Checks, if TC is in the Promiscuous mode

  \return True, if TC is in the Promiscuous mode, False - otherwise
******************************************************************************/
bool TC_IsPromiscuousMode(void);

#endif /* _TC_PROMISCUOUS_MODE_ */
#endif /* _LINK_SECURITY_ */

#endif /* _TCPROMISCUOUSMODE_H_ */
