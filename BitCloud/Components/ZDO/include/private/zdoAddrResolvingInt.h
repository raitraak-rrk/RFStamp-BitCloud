/**************************************************************************//**
  \file zdoAddrResolvingInt.h

  \brief Contains private prototypes and definitions for ZDO Address Resolving routines.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    21.06.12 D. Kolmakov - Created
  Last change:
    $Id: zdoAddrResolvingInt.h 28144 2015-07-09 08:58:32Z viswanadham.kotla $
******************************************************************************/
#ifndef _ZDOADDRRESOLVINGINTERNAL_H
#define _ZDOADDRRESOLVINGINTERNAL_H

/******************************************************************************
                        Includes section
******************************************************************************/
#include <zdo.h>

/******************************************************************************
                        Types section
******************************************************************************/
typedef struct _ZdoAddrResolving_t
{
  QueueDescriptor_t         queue;
} ZdoAddrResolving_t;

/******************************************************************************
                        Prototypes section
******************************************************************************/
/******************************************************************************
  \brief Address Resolving request task handler.
  \param None.
  \return None.
******************************************************************************/
ZDO_PRIVATE void zdoAddrResolvingTaskHandler(void);

/******************************************************************************
  \brief Reset Address Resolving component.
  \param None.
  \return None.
******************************************************************************/
ZDO_PRIVATE void zdoResetAddrResolving(void);

#endif // _ZDOADDRRESOLVINGINTERNAL_H
