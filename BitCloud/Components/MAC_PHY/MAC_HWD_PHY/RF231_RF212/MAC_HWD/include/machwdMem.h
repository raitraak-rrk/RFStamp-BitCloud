/******************************************************************************
  \file machwdMem.h

  \brief MACHWD memory type.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \brief
    History:
      24/04/08 A. Mandychev - Created.
******************************************************************************/

#ifndef _MACHWDMEM_H
#define _MACHWDMEM_H

/******************************************************************************
                    Includes section
******************************************************************************/
#include <sysTypes.h>
#include <macBuffers.h>
#include <phy.h>
#include <phyDataStatus.h>
#include <machwdData.h>

/******************************************************************************
                    Define(s) section
******************************************************************************/

/******************************************************************************
                    Types section
******************************************************************************/
/** MAC hardware dependent sublayer internal memory pool */
typedef struct
{
  bool beaconRxMode;
  FrameRxBufferState_t rxBufferState[FRAME_RX_BUFFER_TYPES_AMOUNT];
  uint8_t handlers;
  union
  {
    volatile PHY_EnergyLevel_t energyLevel;
    volatile PHY_DataStatus_t dataStatus;
  };
} MachwdMem_t;

#endif /*_MACHWDMEM_H*/

// eof machwdMem.h
