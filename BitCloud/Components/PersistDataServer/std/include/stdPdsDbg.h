/******************************************************************************
  \file pdsDbg.h

  \brief
    Persistent Data Server debug info

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2010 , Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    29.10.10 A. Razinkov - Created.
  Last change:
    $Id: stdPdsDbg.h 22336 2012-08-16 15:09:46Z arazinkov $
******************************************************************************/

#ifndef _PDSDBG_H_
#define _PDSDBG_H_

/******************************************************************************
                    Includes section
******************************************************************************/
#include <dbg.h>

/******************************************************************************
                    Types section
******************************************************************************/

typedef enum
{
  PDSMEMACCESS_PDSPREPAREMEMORYACCESS0  = 0xA000,
  PDSMEMACCESS_PDSPREPAREMEMORYACCESS1  = 0xA001,
  PDSMEMACCESS_PDSPREPAREACCESSCONTEXT0 = 0xA003,
  PDSMEMACCESS_PDSPREPAREACCESSCONTEXT1 = 0xA004,

  PDSWRITEDATA_PDSCOMMIT0               = 0xA010,

  PDSINIT_PDSINIT0                      = 0xA020,
  PDSINIT_PDSINIT1                      = 0xA021,
  PDSINIT_PDSINIT2                      = 0xA022,

  PDSDATASERVER_PDSMARKDATATORESTORE0   = 0xA030,
  PDSDATASERVER_PDSSETTODEFAULT0        = 0xA031,
  PDSDATASERVER_PDSCLEARRESTOREDMEMORY0 = 0xA032,

  PDSEVENTS_PDSOBSERVER0                = 0xA040,
} PDS_DbgCodeId_t;

#endif /* _PDSDBG_H_ */
