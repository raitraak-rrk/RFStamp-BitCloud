/*****************************************************************************//**
\file  driversDbg.h

\brief Declarations of hal drivers, bsb mistake interface.

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal
  History:
    08/10/12 N. Fomin - Created
**********************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/

#ifndef _DRIVERSDBG_H
#define _DRIVERSDBG_H

/******************************************************************************
                   Includes section
******************************************************************************/
#include <dbg.h>

/******************************************************************************
                   Types section
******************************************************************************/
typedef enum _HalDriversDbgCode_t
{
  OFD_NULLCALLBACK0  = 0x2900,
  OFD_NULLCALLBACK1  = 0x2901,
  OFD_NULLCALLBACK2  = 0x2902,
  OFD_NULLCALLBACK3  = 0x2903,
  OFD_NULLCALLBACK4  = 0x2904,
  OFD_NULLCALLBACK5  = 0x2905,
  OFD_NULLCALLBACK6  = 0x2906,
  OFD_NULLCALLBACK7  = 0x2907,
  OFD_NULLCALLBACK8  = 0x2908,
  OFD_NULLCALLBACK9  = 0x2909,
  OFD_NULLCALLBACK10 = 0x290A,
  OFD_NULLCALLBACK11 = 0x290B,
  OFD_NULLCALLBACK12 = 0x290C,
  OFD_NULLCALLBACK13 = 0x290D,
  OFD_NULLCALLBACK14 = 0x290E,
  OFD_NULLCALLBACK15 = 0x290F,
  OFD_NULLCALLBACK16 = 0x2910,
  OFD_NULLCALLBACK17 = 0x2911,
  OFD_NULLCALLBACK18 = 0x2912,
  OFD_NULLCALLBACK19 = 0x2913,
  OFD_NULLCALLBACK20 = 0x2914,
  OFD_NULLCALLBACK21 = 0x2915,

  ISD_NULLCALLBACK0  = 0x2980,
  ISD_NULLCALLBACK1  = 0x2981,
  ISD_NULLCALLBACK2  = 0x2982,
  ISD_NULLCALLBACK3  = 0x2983,

  MSD_NULLCALLBACK0  = 0x2A00,
  MSD_NULLCALLBACK1  = 0x2A01,
  MSD_NULLCALLBACK2  = 0x2A02,
  MSD_NULLCALLBACK3  = 0x2A03,
  MSD_NULLCALLBACK4  = 0x2A04,
  MSD_NULLCALLBACK5  = 0x2A05,
  MSD_NULLCALLBACK6  = 0x2A06,
  MSD_NULLCALLBACK7  = 0x2A07,
} HalDriversDbgCode_t;

#endif /* _DRIVERSDBG_H */

// eof driversDbg.h

