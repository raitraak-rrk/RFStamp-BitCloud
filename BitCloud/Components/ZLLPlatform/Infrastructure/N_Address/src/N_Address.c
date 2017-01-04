/*********************************************************************************************//**
\file  

\brief 

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal

$Id: N_Address.c 24615 2013-02-27 05:14:24Z arazinkov $

***************************************************************************************************/

/***************************************************************************************************
* LOCAL INCLUDE FILES
***************************************************************************************************/

#include "N_Address.h"


/***************************************************************************************************
* EXPORTED CONSTANTS
***************************************************************************************************/

const N_Address_t c_AddrBinding =
{
    { 0u },
    N_Address_Mode_Binding,
    0u,
    0u
};

const N_Address_t c_Addr64 =
{
    { 0u },
    N_Address_Mode_Extended,
    0u,
    0u
};

const N_Address_t c_Addr16 =
{
    { 0u },
    N_Address_Mode_Short,
    0u,
    0u
};

const N_Address_t c_AddrAll =
{
    { 0xffffu },
    N_Address_Mode_Broadcast,
    255u,
    0u
};

const N_Address_t c_AddrRXON =
{
    { 0xfffdu },
    N_Address_Mode_Broadcast,
    255u,
    0u
};

const N_Address_t c_AddrRoutersOnly =
{
    { 0xfffcu },
    N_Address_Mode_Broadcast,
    255u,
    0u
};

const N_Address_t c_AddrG =
{
    { 0x0u },
    N_Address_Mode_Group,
    0u,
    0u
};

const N_Address_ExtendedPanId_t c_ExtPanId00 =
{
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u
};

const N_Address_ExtendedPanId_t c_ExtPanIdFF =
{
    0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu
};
