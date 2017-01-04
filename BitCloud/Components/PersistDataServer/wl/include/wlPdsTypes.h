/**************************************************************************//**
  \file wlPdsTypes.h

  \brief PDS types declaration.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    23.05.13 A. Fomin - Created.
******************************************************************************/

#ifndef _WLPDSTYPES_H_
#define _WLPDSTYPES_H_

/******************************************************************************
                   Includes section
******************************************************************************/
#include <sysTypes.h>
#include <wlPdsTypesConverter.h>

/******************************************************************************
                   Definitions section
******************************************************************************/
#define NO_FILE_MARKS 0U

/******************************************************************************
                   Types section
******************************************************************************/
typedef enum
{
  PDS_SUCCESS,            //!< Command completed successfully
} PDS_DataServerState_t;

typedef PDS_DataServerState_t PDS_Status_t;

typedef uint16_t PDS_DataSize_t;

#endif // _WLPDSTYPES_H_
// eof wlPdsTypes.h