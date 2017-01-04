/*********************************************************************************************//**
\file  
  N_Zcl_DataRequest.h
\brief 
  ZCL data Request API interface header file
\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal

$Id: N_Zcl_DataRequest.h 24615 2013-02-27 05:14:24Z arazinkov $

***************************************************************************************************/

#ifndef N_ZCLDATAREQUEST_H
#define N_ZCLDATAREQUEST_H

/***************************************************************************************************
* INCLUDE FILES
***************************************************************************************************/

#include "N_Types.h"
#include "apsdeData.h"

/***************************************************************************************************
* C++ DECLARATION WRAPPER
***************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************************************
* EXPORTED FUNCTIONS
***************************************************************************************************/

/** Send a ZigBee AF layer packet.
    \param apsDataReq APS data request parameters
*/
void N_Zcl_DataRequest(APS_DataReq_t *apsDataReq);  

/***************************************************************************************************
* END OF C++ DECLARATION WRAPPER
***************************************************************************************************/

#ifdef __cplusplus
}
#endif

#endif // N_ZCLDATAREQUEST_H
