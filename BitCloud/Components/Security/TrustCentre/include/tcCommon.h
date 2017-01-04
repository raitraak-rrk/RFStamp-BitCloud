/**************************************************************************//**
  \file tcCommon.h

  \brief Trust Centre common types and definitions header file.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      2008.12.01 - ALuzhetsky created.
******************************************************************************/
#ifndef _TCCOMMONH
#define _TCCOMMONH

/**************************************************************************//**
  \brief Trust Centre service field.

   Service field - for internal needs - to store requets in the requets' queue.
******************************************************************************/
typedef struct
{
  void *next;
} TC_Service_t;

#endif // _TCCOMMONH
// eof tcCommon.h
