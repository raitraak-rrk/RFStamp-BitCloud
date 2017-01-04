/***************************************************************************//**
  \file zsiStubs.c

  \brief ZAppSI stubs for HOST side.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2011-08-26  A. Razinkov - Created.
   Last change:
    $Id: zsiStubs.c 24441 2013-02-07 12:02:29Z akhromykh $
 ******************************************************************************/

/******************************************************************************
                              Implementations section
******************************************************************************/
#ifdef ZAPPSI_HOST
void RF_Init(void)
{
}

void MAC_PHY_HWD_TaskHandler(void)
{
}

void phyRxTxSwitcherOn(void)
{
}

void phyRxTxSwitcherOff(void)
{
}

#if defined(AT91SAM7X256) || defined (AT91SAM3S4C)
void phyDispatcheRTimerEvent(void)
{
}
#endif
#endif /* ZAPPSI_HOST */

/* eof zsiStubs.c*/
