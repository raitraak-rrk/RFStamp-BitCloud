/******************************************************************************
  \file zsiInit.h

  \brief
    ZAppSI initialization module interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2010 , Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2011-08-26  A. Razinkov - Created.
   Last change:
    $Id: zsiInit.h 25629 2013-11-19 03:40:22Z viswanadham.kotla $
******************************************************************************/

#ifndef _ZSIINIT_H_
#define _ZSIINIT_H_

/******************************************************************************
                    Includes section
******************************************************************************/
#include <sysTypes.h>

/******************************************************************************
                    Prototypes section
******************************************************************************/
/******************************************************************************
  \brief ZAppSI initialization routine.
 ******************************************************************************/
void ZSI_Init(void);

/******************************************************************************
  \brief ZAppSI initialization handler
 ******************************************************************************/
void zsiInitTaskHandler(void);

#ifdef ZAPPSI_HOST
/**************************************************************************//**
  \brief Initial parameters synchronization between HOST and NP.

  \return None.
 ******************************************************************************/
void zsiSynchronizeStartupParameters(void);
#endif /* ZAPPSI_HOST */
/**************************************************************************//**
  \brief Resets all zsi related queues

  \return None.
 ******************************************************************************/
void zsiResetQueues(void);

#endif /* _ZSIINIT_H_ */
