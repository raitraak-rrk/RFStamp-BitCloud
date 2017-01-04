/************************************************************************//**
  \file zclCommandAnalyzer.h

  \brief
    ZCL command analyzer interface

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    21.03.13 N. Fomin - Created.
******************************************************************************/

#ifndef _ZCLCOMMANDANALYZER_H
#define _ZCLCOMMANDANALYZER_H

/******************************************************************************
                           Includes section
******************************************************************************/
#include <zcl.h>

/******************************************************************************
                           Prototypes section
******************************************************************************/
/**************************************************************************//**
\brief Checks if response for command is required based on its payload

\param[in] cluster - cluster id of command
\param[in] cmd     - command id
\param[in] dir     - command direction
\param[in] payload - command payload

\returns true if resposne is required, false otherwise
******************************************************************************/
bool zclIsResponseRequiredByPayload(ClusterId_t cluster, ZCL_CommandId_t cmd, uint8_t dir, uint8_t *payload);

/***************************************************************************//**
\brief Checks whether response for command is required based on its address mode

\param[in] req - pointer to request structure

\return true - if response is required, false - otherwise.
******************************************************************************/
bool zclIsResponseRequiredByAddrMode(ZCL_Request_t *req);

#endif //_ZCLCOMMANDANALYZER_H
//eof zclCommandAnalyzer.h

