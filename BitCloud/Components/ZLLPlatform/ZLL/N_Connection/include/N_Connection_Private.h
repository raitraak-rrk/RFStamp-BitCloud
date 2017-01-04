/**************************************************************************//**
  \file N_Connection_Private.h

  \brief Private API of N_Connection module

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    26.11.12 A. Razinkov - created
******************************************************************************/

#ifndef N_CONNECTION_PRIVATE_H
#define N_CONNECTION_PRIVATE_H

/***************************************************************************************************
* INCLUDE FILES
***************************************************************************************************/

#include "N_Types.h"

/***************************************************************************************************
* C++ DECLARATION WRAPPER
***************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************************************
* EXPORTED FUNCTIONS
***************************************************************************************************/

/** Checks if LinkTarget set the InterPan mode to on

    \returns TRUE if LinkTarget set the InterPan mode to on; FALSE - otherwise
*/
bool N_Connection_TargetIsInterpanModeOn(void);

/** Component internal networkJoined notification
*/
void N_Connection_NetworkJoined(void);

/** Component internal connected notification
*/
void N_Connection_Connected(void);

/** Component internal disconnected notification
*/
void N_Connection_Disconnected(void);

/** Leave network routine.
*/
void N_Connection_LeaveNetwork(void);

/** Returns the state of Reconnect urgent.
*/
bool N_Connection_Is_ReconnectUrgentOngoing(void);

/***************************************************************************************************
* END OF C++ DECLARATION WRAPPER
***************************************************************************************************/

#ifdef __cplusplus
}
#endif

#endif // N_CONNECTION_PRIVATE_H
