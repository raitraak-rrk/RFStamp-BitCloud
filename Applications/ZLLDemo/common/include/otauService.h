/**************************************************************************//**
  \file otauService.h

  \brief
    OTAU interface declaration.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    06.02.12 S. Dmitriev - Created.
******************************************************************************/
#ifndef _OTAUSERVICE_H_
#define _OTAUSERVICE_H_

/******************************************************************************
                    Prototypes section
******************************************************************************/
#if APP_ZLL_DEVICE_TYPE == APP_DEVICE_TYPE_BRIDGE
/**************************************************************************//**
\brief Starts Over-the-air update server service.
******************************************************************************/
void startOtauServer(void);
#endif

/**************************************************************************//**
\brief Starts Over-the-air update client service.
******************************************************************************/
void startOtauClient(void);

/**************************************************************************//**
\brief Starts Over-the-air update client service.
******************************************************************************/
void stopOtauClient(void);

#endif // _OTAUSERVICE_H_
// eof networkJoining.h