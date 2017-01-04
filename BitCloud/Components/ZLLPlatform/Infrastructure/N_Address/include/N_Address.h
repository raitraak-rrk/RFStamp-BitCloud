/*********************************************************************************************//**
\file  

\brief 

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal

$Id: N_Address.h 24615 2013-02-27 05:14:24Z arazinkov $

***************************************************************************************************/

#ifndef N_ADDRESS_H
#define N_ADDRESS_H

/***************************************************************************************************
* INCLUDE FILES
***************************************************************************************************/

#include "N_Types.h"

/***************************************************************************************************
* C++ DECLARATION WRAPPER
***************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************************************
* EXPORTED TYPES
***************************************************************************************************/

typedef uint8_t N_Address_Extended_t[8];

typedef uint8_t N_Address_ExtendedPanId_t[8];

typedef enum N_Address_Mode_t
{
    N_Address_Mode_Binding = 0,
    N_Address_Mode_Group = 1,
    N_Address_Mode_Short = 2,
    N_Address_Mode_Extended = 3,
    N_Address_Mode_Broadcast = 15,
} N_Address_Mode_t;

typedef struct N_Address_t
{
    union
    {
        uint16_t shortAddress;
        N_Address_Extended_t extendedAddress;
    } address;
    N_Address_Mode_t addrMode;
    uint8_t endPoint;
    uint16_t panId; // should typically be zero
} N_Address_t;

/***************************************************************************************************
* EXPORTED MACROS
***************************************************************************************************/

#define N_ADDRESS_INVALID_SHORT_ADDRESS 0xFFFEu
#define N_ADDRESS_INVALID_PAN_ID 0xFFFEu


/***************************************************************************************************
* EXPORTED CONSTANTS
***************************************************************************************************/

extern const N_Address_t c_AddrBinding;
extern const N_Address_t c_Addr64;
extern const N_Address_t c_Addr16;
extern const N_Address_t c_AddrAll;
extern const N_Address_t c_AddrRXON;
extern const N_Address_t c_AddrRoutersOnly;
extern const N_Address_t c_AddrG;

extern const N_Address_ExtendedPanId_t c_ExtPanId00;    // all zeroes
extern const N_Address_ExtendedPanId_t c_ExtPanIdFF;    // all 0xFF's

/***************************************************************************************************
* EXPORTED FUNCTIONS
***************************************************************************************************/


/***************************************************************************************************
* END OF C++ DECLARATION WRAPPER
***************************************************************************************************/

#ifdef __cplusplus
}
#endif

#endif // N_ADDRESS_H
