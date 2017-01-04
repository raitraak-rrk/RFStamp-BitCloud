/****************************************************************************//**
  \file sysSleep.h

  \brief
    Implementation of the system sleep service.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    8/06/09 A. Khromykh - Created
*********************************************************************************/

#ifndef _SYSSLEEP_H
#define _SYSSLEEP_H
/*********************************************************************************
                          Includes section.
**********************************************************************************/
#include <sleep.h>

/*********************************************************************************
                   Define(s) section
**********************************************************************************/
#define SYS_IDLE        HAL_IdleMode()

/*********************************************************************************
                          Function prototypes section.
**********************************************************************************/
/******************************************************************************//**
\brief Prepares system for sleep.

\param[in]
  sleepParam - pointer to sleep structure.
**********************************************************************************/
void SYS_Sleep(HAL_Sleep_t *sleepParam);

#endif /* _SYSSLEEP_H */
