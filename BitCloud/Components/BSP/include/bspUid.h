/**************************************************************************//**
  \file  bspUid.h

  \brief The header file describes the UID interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    26/12/14 Prashanth.Udumula - Created
 ******************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/

#ifndef _BSPUID_H
#define _BSPUID_H

/******************************************************************************
                   Includes section
******************************************************************************/
// \cond
#include <sysTypes.h>
// \endcond

/******************************************************************************
                   Prototypes section
******************************************************************************/
/************************************************************//**
\brief  Reads UID from the UID chip

\ingroup bsp_misc

\param[in]
    id - UID buffer pointer. \n
    Attention! Memory size must equal <i> (Number of devices) * 8 bytes </i>
\return
    0 - if UID of DS2411 has been found successfully; \n
   -1 - if some error occured during UID discovery.
****************************************************************/
int BSP_ReadUid(uint64_t *id);

#endif /* _BSPUID_H */

// eof bspUid.h
