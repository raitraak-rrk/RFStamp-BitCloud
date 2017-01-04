/**************************************************************************//**
  \file  sliders.h

  \brief The header file describes the sliders interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    29/06/07 E. Ivanov - Created
******************************************************************************/

#ifndef _SLIDERS_H
#define _SLIDERS_H

/******************************************************************************
                   Define(s) section
******************************************************************************/
#define SLIDER0 (1 << 0)
#define SLIDER1 (1 << 1)
#define SLIDER2 (1 << 2)
#define SLIDER3 (1 << 3)

/******************************************************************************
                   Prototypes section
******************************************************************************/
/**************************************************************************//**
\brief Reads the sliders.
\return
   State of onboard DIP switches. Use the ::SLIDER0, ::SLIDER1, ::SLIDER2,
   ::SLIDER3 constants to check state. Bit value 1 indicates that the
   corresponding slider is on.\n
   Note that the ::SLIDER3 is available on MeshBean LAN and Turbo board only.
******************************************************************************/
uint8_t BSP_ReadSliders(void);

#endif /* _SLIDERS_H */
// eof sliders.h
