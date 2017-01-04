/**************************************************************************//**
\file  ofdCrcService.c

\brief Implementation of crc counting algorithm.

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal
  History:
    7/08/09 A. Khromykh - Created
*******************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/

#if APP_USE_OTAU == 1
#if (APP_USE_OTAU == 1)
#if APP_USE_FAKE_OFD_DRIVER == 0

/******************************************************************************
                   Includes section
******************************************************************************/
#include <sysTypes.h>

/******************************************************************************
                   Implementations section
******************************************************************************/
/**************************************************************************//**
\brief Counts crc current memory area. CRC-8. Polynom 0x31    x^8 + x^5 + x^4 + 1.

\param[in]
  crc - first crc state
\param[in]
  pcBlock - pointer to the memory for crc counting
\param[in]
  length - memory size

\return
  current area crc
******************************************************************************/
uint8_t ofdCrc(uint8_t crc, uint8_t *pcBlock, uint8_t length)
{
  uint8_t i;

  while (length--)
  {
    crc ^= *pcBlock++;

    for (i = 0; i < 8; i++)
      crc = crc & 0x80 ? (crc << 1) ^ 0x31 : crc << 1;
  }

  return crc;
}

#endif // APP_USE_FAKE_OFD_DRIVER == 0
#endif // (APP_USE_OTAU == 1)
#endif // APP_USE_OTAU == 1

// eof ofdCrcService.c
