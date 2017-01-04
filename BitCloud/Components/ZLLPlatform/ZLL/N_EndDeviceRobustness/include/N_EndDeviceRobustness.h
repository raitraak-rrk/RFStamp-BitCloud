/**************************************************************************//**
  \file N_EndDeviceRobustness.h

  \brief End Device Robustness API declaration.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    28.01.13 D. Kolmakov - Created
******************************************************************************/

#ifndef N_END_DEVICE_ROBUSTNESS_H
#define N_END_DEVICE_ROBUSTNESS_H

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

typedef enum _EndDeviceRobustnessState_t
{
  N_END_DEVICE_ROBUSTNESS_IDLE,
  N_END_DEVICE_ROBUSTNESS_CONNECTED,
  N_END_DEVICE_ROBUSTNESS_DISCONNECTED,
  N_END_DEVICE_ROBUSTNESS_WAIT_FOR_CONFIRM,
  N_END_DEVICE_ROBUSTNESS_TX_FAILURE,
  N_END_DEVICE_ROBUSTNESS_POLL_FAILURE
} EndDeviceRobustnessState_t;

/***************************************************************************************************
* EXPORTED FUNCTIONS
***************************************************************************************************/

/**************************************************************************//**
  \brief Performs APS data request.

  \param[in] apsDataReq - pointer to the request.
******************************************************************************/
void N_EndDeviceRobustness_DataRequest(APS_DataReq_t *apsDataReq);

/**************************************************************************//**
  \brief Performs ZDP request.

  \param[in] apsDataReq - pointer to the request.
******************************************************************************/
void N_EndDeviceRobustness_ZdpRequest(ZDO_ZdpReq_t *zdpReq);

/**************************************************************************//**
  \brief Returns current state of N_EndDeviceRobustness component.
******************************************************************************/
EndDeviceRobustnessState_t N_EndDeviceRobustness_GetState(void);

/***************************************************************************************************
* END OF C++ DECLARATION WRAPPER
***************************************************************************************************/

#ifdef __cplusplus
}
#endif

#endif // N_END_DEVICE_ROBUSTNESS_H
