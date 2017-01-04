/**************************************************************************//**
  \file  dbgu.h

  \brief Declarations of debug message interface (feature only for atmel arm).

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    11/11/08 A. Khromykh - Created
 ******************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/

#ifndef _DBGU_H
#define _DBGU_H

/******************************************************************************
                   Prototypes section
******************************************************************************/
/**************************************************************************//**
\brief Open dbgu port (115200 baud, 8N1 format).
******************************************************************************/
void HAL_OpenDbgu(void);

/**************************************************************************//**
\brief Send string trough dbgu port.

\param[in]
  buffer - pointer to the string
******************************************************************************/
void HAL_SendDbguMessage(const char *buffer);

#endif /* _DBGU_H */
