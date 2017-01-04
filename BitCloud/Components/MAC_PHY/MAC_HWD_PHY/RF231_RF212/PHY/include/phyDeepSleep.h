/**************************************************************************//**
  \file phyDeepSleep.h

  \brief Transceiver Deep Sleep mode interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2012-03-02 A. Razinkov - Created.
   Last change:
    $Id: phyDeepSleep.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#ifndef _PHYDEEPSLEEP_H
#define _PHYDEEPSLEEP_H

#ifdef AT86RF233
/******************************************************************************
                               Prototypes section
 ******************************************************************************/
/******************************************************************************
  \brief Prepare transceiver to the Deep Sleep mode.
 ******************************************************************************/
void PHY_PrepareDeepSleep(void);

/******************************************************************************
  \brief Restore transceiver from the Deep Sleep mode.
 ******************************************************************************/
void PHY_RestoreFromDeepSleep(void);

#else
#define PHY_PrepareDeepSleep()     (void)0
#define PHY_RestoreFromDeepSleep() (void)0
#endif /* AT86RF233 */

#endif /* _PHYDEEPSLEEP_H */
/** eof phyDeepSleep.h */
