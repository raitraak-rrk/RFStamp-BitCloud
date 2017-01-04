/************************************************************************//**
  \file N_ZllEndpoint.h

  \brief
    Bind manager interface declaration.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    28.08.12 N. Fomin - Created.
******************************************************************************/
#ifndef N_ZLLENDPOINT_H
#define N_ZLLENDPOINT_H

/******************************************************************************
                    Includes section
******************************************************************************/
#include <zcl.h>

/******************************************************************************
                    Types section
******************************************************************************/
typedef struct
{
  // Service structure
  struct
  {
    union
    {
      APS_RegisterEndpointReq_t   registerEndpointReq;
      APS_UnregisterEndpointReq_t unregEndpointReq;
    };
  } service;

  SimpleDescriptor_t *simpleDescriptor;   //!< Device endpoint Simple Descriptor
  uint8_t            groupsAmount;
} ZLL_Endpoint_t;

#endif // N_ZLLENDPOINT_H
//eof N_ZllEndpoint.h
