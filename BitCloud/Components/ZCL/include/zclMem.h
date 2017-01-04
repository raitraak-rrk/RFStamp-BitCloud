/************************************************************************//**
  \file zclMem.h

  \brief
    The header file describes the ZCL memory structure

    The file describes the structure of ZCL memory

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    03.12.08 I. Fedina - Created.
******************************************************************************/

#ifndef _ZCLMEM_H
#define _ZCLMEM_H

/******************************************************************************
                   Includes section
******************************************************************************/
#include <sysQueue.h>
#include <appTimer.h>

/******************************************************************************
                   Types section
******************************************************************************/
/******************************************************************************
Describes ZCL memory structure
******************************************************************************/
typedef struct
{
  uint8_t sequenceNumber;
  uint8_t *memoryBuffer;
} ZclMem_t;

/******************************************************************************
                           External variables section
******************************************************************************/
/******************************************************************************
ZCL memory global object
******************************************************************************/
extern ZclMem_t zclMem;

/******************************************************************************
                           Inline functions section
 ******************************************************************************/
/******************************************************************************
Function returns point to ZCL memory object
******************************************************************************/
static inline ZclMem_t * zclMemReq(void)
{
  return ((ZclMem_t *)&zclMem);
}

#endif // _ZCLMEM_H
