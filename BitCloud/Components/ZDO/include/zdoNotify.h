/**************************************************************************//**
  \file zdoNotify.h

  \brief Functions for raising ZDO notifications

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    13.11.09 A. Taradov - Created
******************************************************************************/
#ifndef _ZDONOTIFY_H
#define _ZDONOTIFY_H

/******************************************************************************
                    Includes section
******************************************************************************/
#include <zdo.h>
#include <aps.h>

/******************************************************************************
                    Prototypes section
******************************************************************************/

/**************************************************************************//**
\brief Subscribe legacy ZDO notifications to stack events
******************************************************************************/
void zdoInitNotifications(void);

/******************************************************************************
  Call ZDO_MgmtNwkUpdateNotf() with prepared notification information
  Parameters:
    notf - notification information
  Return:
    none
******************************************************************************/
void ZDO_NotifyGeneral(ZDO_MgmtNwkUpdateNotf_t *notf);

#ifdef _TC_PROMISCUOUS_MODE_
/**************************************************************************//**
\brief Indication of a failed attempt of a remote device to rejoin the network

 The function should be implemented by the trust center's application.

 The function is invoked to notify the application that a remote node
 has attempted to rejoin the network and failed, perhaps, because it has
 provided the pre-configured link key, while the trust center holds the
 randomly generated one.

 If the application previously saved the pre-configured link key by calling
 the APS_SetPreinstalledLinkKey() function, it can now restore the key
 by calling the APS_RestorePreinstalledLinkKey() function.

\param[in] extAddr - pointer to extended address of the lost device
******************************************************************************/
void ZDO_NotifyBindLost(ExtAddr_t* extAddr);
#endif /* _TC_PROMISCUOUS_MODE_ */

#endif // _ZDONOTIFY_H
// eof zdoNotify.h
