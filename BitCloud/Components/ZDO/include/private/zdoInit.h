/**************************************************************************//**
  \file zdoInit.h

  \brief Interface for ZDO initialization routines.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2009-10-27 A. Taradov - Created
    2012-11-07 Max Gekk - Refactoring.
   Last change:
    $Id: zdoInit.h 23429 2012-11-07 08:19:23Z mgekk $
 ******************************************************************************/
#if !defined _ZDO_INIT_H
#define _ZDO_INIT_H

/******************************************************************************
                              Prototypes section
 ******************************************************************************/
/******************************************************************************
  \brief Initializate and reset stack components.
 ******************************************************************************/
ZDO_PRIVATE void zdoInitHandler(void);

#endif /* _ZDO_INIT_H */
/** eof zdoInit.h */
