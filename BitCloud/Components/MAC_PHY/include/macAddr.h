/**************************************************************************//**
  \file macAddr.h

  \brief MAC addressing types and constants.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      18/06/07 ALuzhetsky - Created.
******************************************************************************/

#ifndef _MACADDR_H
#define _MACADDR_H

/******************************************************************************
                        Includes section.
******************************************************************************/
#include <sysTypes.h>

/******************************************************************************
                        Define(s) section.
******************************************************************************/
#define MAC_BROADCAST_PANID       0xFFFFU ///< Broadcast PANID.
#define MAC_BROADCAST_SHORT_ADDR  0xFFFFU ///< Broadcast short address, no association.
#define MAC_NO_SHORT_ADDR         0xFFFEU ///< After association no short address.
#define MAC_NO_EXT_ADDR           0x00ULL

/******************************************************************************
                        Types section.
******************************************************************************/
typedef uint16_t PanId_t;     ///< PAN identifier type declaration.
typedef uint16_t ShortAddr_t; ///< Short address type declaration.
typedef uint64_t ExtAddr_t;   ///< Extended address type declaration.

/// Union of possible MAC address types.
typedef union
{
  ShortAddr_t sh;
  ExtAddr_t   ext;
  uint8_t     raw[8];
} MAC_Addr_t;

//! MAC address mode types (IEEE 802.15.4-2006 Table 80).
 typedef enum
{
  MAC_NO_ADDR_MODE    = 0x00,
  MAC_RSRV_ADDR_MODE  = 0x01,
  MAC_SHORT_ADDR_MODE = 0x02,
  MAC_EXT_ADDR_MODE   = 0x03
} MAC_AddrMode_t;

/******************************************************************************
                        Definitions section.
******************************************************************************/
#if defined UINT64_MEMCMP
  #define IS_EQ_EXT_ADDR(A, B)    IS_EQ_64BIT_VALUES(A, B)
  #define IS_CORRECT_EXT_ADDR(A)  isCorrectExtAddr((const uint8_t *)&A)
  #define COPY_EXT_ADDR(DST, SRC) COPY_64BIT_VALUE(DST, SRC)
#else
  #define IS_EQ_EXT_ADDR(A, B)    IS_EQ_64BIT_VALUES(A, B)
  #define IS_CORRECT_EXT_ADDR(A)  (MAC_NO_EXT_ADDR != (A))
  #define COPY_EXT_ADDR(DST, SRC) COPY_64BIT_VALUE(DST, SRC)
#endif

/******************************************************************************
                Inline static functions prototypes section
******************************************************************************/
INLINE bool isCorrectExtAddr(const uint8_t *extAddr)
{
  uint8_t i = 0U;

  do
    if (extAddr[i]) return true;
  while (++i < sizeof(uint64_t));
  return false;
}

#endif //_MACADDR_H

// eof macAddr.h
