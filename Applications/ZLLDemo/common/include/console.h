/**************************************************************************//**
  \file console.h

  \brief
    Serial interface console interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    22.05.12 N. Fomin - Created.
******************************************************************************/
#ifndef _CONSOLE_H
#define _CONSOLE_H

#if APP_ENABLE_CONSOLE == 1
/**************************************************************************//**
                    Types section
******************************************************************************/
typedef union
{
  int8_t int8;
  uint8_t uint8;
  int16_t int16;
  uint16_t uint16;
  int32_t int32;
  uint32_t uint32;
  char chr;
  char *str;
} ScanValue_t;

/**************************************************************************//**
Valid format codes are:
  d, i - signed integer. '0x' perfix before value treat it like hex, otherwise its decimal.
  Hex couldn't be negative (have a '-' before value);
  s - string;
  c - single character.
******************************************************************************/
typedef struct
{
  const char *const name;              // Command name
  const char *const fmt;               // Format string for arguments
  void (*handler)(const ScanValue_t *);// Process function
  const char *helpMsg;                 // Plain text help message for command
} ConsoleCommand_t;

/**************************************************************************//**
\brief Sends string to serial interface
******************************************************************************/
void consoleTxStr(const char *str);

/**************************************************************************//**
\brief Sends single char to serial interface
******************************************************************************/
void consoleTx(char chr);

/**************************************************************************//**
\brief Processes single char read from serial interface

\param[in] char - read char
******************************************************************************/
void consoleRx(char chr);

/**************************************************************************//**
\brief Register commands in console

\param[in] table - pointer to an array of commands
******************************************************************************/
void consoleRegisterCommands(const ConsoleCommand_t *table);

/**************************************************************************//**
\brief Initializes console
******************************************************************************/
void initConsole(void);

/**************************************************************************//**
\brief Processes data received by console
******************************************************************************/
void processConsole(uint16_t length);

/**************************************************************************//**
\brief Processes command received from HAL (in text mode)
******************************************************************************/
void processCommand(char *Str);

#endif // APP_ENABLE_CONSOLE == 1
#endif // _CONSOLE_H
