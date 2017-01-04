/**************************************************************************//**
  \file  w1.h

  \brief The header file describes the 1-Wire interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    10/12/07 A. Khromykh - Created
 ******************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/

#ifndef _W1_H
#define _W1_H

/******************************************************************************
                   Includes section
******************************************************************************/
#include <halW1.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
#define W1_ANY_FAMILY 0x00
#define DS2411 0x01

/******************************************************************************
                   Prototypes section
******************************************************************************/
/*************************************************************************//**
\brief 1-Wire search procedure with search ROM command only

\ingroup hal_1wire

\param[in]
   family   - 8-bit family code.
\param[in]
   data     - pointer of SRAM where the 8-bytes ROM codes returned by the
              devices are stored. \n
              Attention! Memory size must be equal to (Number of devices) * 8 bytes
\param[in]
   count    - number of devices we wish to find.
\param[in]
   actCount - number of devices actually found.
\return
   W1_SUCCESS_STATUS   - if at least one device has been found. \n
   W1_NO_DEVICE_STATUS - if there are no any devices presented
                         on the bus with specified family code. \n
   W1_INVALID_CRC      - if an invalid CRC has been read during the search and
                         no devices with specified family code was found.
******************************************************************************/
W1Status_t HAL_SearchW1Device(uint8_t family,
                              uint8_t *data,
                              uint8_t count,
                              uint8_t *actCount);

/**************************************************************************//**
\brief 1-Wire search procedure with alarm search command only

\ingroup hal_1wire

\param[in]
   family   - 8-bit family code.
\param[in]
   data     - pointer of SRAM where the 8-bytes ROM codes returned by the
              devices are stored. \n
\param[in]
   count    - number of devices we wish to find.
\param[in]
   actCount - number of devices actually found.
\return
   W1_SUCCESS_STATUS   - if at least one device has been found. \n
   W1_NO_DEVICE_STATUS - if there are no any devices presented
                         on the bus with specified family code. \n
   W1_INVALID_CRC      - if an invalid CRC has been read during the search and
                         no devices with specified family code was found.
******************************************************************************/
W1Status_t HAL_AlarmSearchW1Device(uint8_t family,
                                   uint8_t *data,
                                   uint8_t count,
                                   uint8_t *actCount);

/***************************************************************************//**
\brief Resets all devices connected to the bus.

\ingroup hal_1wire

\return
  0 - there are some devices at the bus. \n
  1 - there are no devices at the bus.
*******************************************************************************/
uint8_t HAL_ResetW1(void);

/***************************************************************************//**
\brief Writes a single byte to the bus

\ingroup hal_1wire

\param[in]
    value - byte to write.
*******************************************************************************/
void HAL_WriteW1(uint8_t value);

/***************************************************************************//**
\brief Reads a single byte from the bus.

\ingroup hal_1wire

\return
  byte read from the bus.
*******************************************************************************/
uint8_t HAL_ReadW1(void);

#endif /* _W1_H */
// eof w1.h
