/******************************************************************************
  \file csDbg.h

  \brief
    Configuration Server debug info

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015 , Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    22.10.10 A. Razinkov - Created.
******************************************************************************/

#ifndef _CSDBG_H_
#define _CSDBG_H_

/******************************************************************************
                    Includes section
******************************************************************************/
#include <dbg.h>

typedef enum
{
  CS_READ_PARAM0  = 0x6000,
  CS_READ_PARAM1  = 0x6001,
  CS_READ_PARAM2  = 0x6002,
  CS_READ_PARAM3  = 0x6003,

  CS_WRITE_PARAM0 = 0x6004,
  CS_WRITE_PARAM1 = 0x6005,

  CS_GET_MEM0     = 0x6006,

  CS_GET_ITEM0    = 0x6007,
  CS_GET_ITEM1    = 0x6008,
  CS_GET_ITEM2    = 0x6009,
  CS_GET_ITEM3    = 0x600A,
} CS_DbgCodeId_t;

#endif /* _CSDBG_H_ */
