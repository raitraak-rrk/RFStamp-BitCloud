/**************************************************************************//**
  \file console.c

  \brief
    Serial interface console implementation.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    22.05.12 N. Fomin - Created.
******************************************************************************/
#if APP_ENABLE_CONSOLE == 1

/******************************************************************************
                    Includes section
******************************************************************************/
#include <ctype.h>
#include <string.h>
#include <stdint.h>
#include "console.h"

/******************************************************************************
                    Definitions section
******************************************************************************/
#define VT100_ESCAPE        0x1B
#define VT100_ERASE_LINE    "\x1B[2K"
#define CMD_PROMPT          "$"

#define CMD_BUF_SIZE        64
#define MAX_NUM_OF_ARGS     8
#define CRLF                "\r\n"
#define FORCE_LOWCASE 0

/******************************************************************************
                    Types section
******************************************************************************/
typedef struct
{
  ScanValue_t *top;
  ScanValue_t args[MAX_NUM_OF_ARGS];
  ScanValue_t end[0];
} ScanStack_t;

/******************************************************************************
                              Local variables
******************************************************************************/
static const ConsoleCommand_t *cmdTable;

/******************************************************************************
                    Prototypes section
******************************************************************************/
static uint8_t tokenizeStr(char *str, ScanStack_t *stk);
static uint8_t unpackArgs(const ConsoleCommand_t *cmd, ScanStack_t *args);

/******************************************************************************
                    Implementation section
******************************************************************************/
/**************************************************************************//**
\brief Converts decimal string to unsigned long

\param[in] str - string to be converted
\param[out] out - pointer to converted number

\returns 1 if conversation was successful, 0 - otherwise
******************************************************************************/
// No check for overflow
uint8_t decimalStrToUlong(const char *str, uint32_t *out)
{
  uint32_t decCnt = 1;
  uint32_t ret = 0;
  const char *p;

  if (!*str)
    return 0;  // Empty

  for (p = str; *p; p++);                    // Rewind to the end of string

  for (p--; p >= str; p--, decCnt *= 10)     // locStrP points to the last symbol at the loop start
  {
    if (!isdigit((int)*p))
      return 0;

    ret += (*p - '0') * decCnt;
  }

  *out = ret;
  return 1;
}

/**************************************************************************//**
\brief Converts decimal string to signed long

\param[in] str - string to be converted
\param[out] out - pointer to converted number

\returns 1 if conversation was successful, 0 - otherwise
******************************************************************************/
uint8_t decimalStrToSlong(const char *str, int32_t *out)
{
  uint32_t ret;

  if (*str == '-')
  {
    if (decimalStrToUlong(++str, &ret))
    {
      *out = -ret;
      return 1;
    }
  }
  else if (*str)
  {
    if (decimalStrToUlong(str, &ret))
    {
      *out = ret;
      return 1;
    }
  }

  return 0;
}

/**************************************************************************//**
\brief Converts hex string to unsigned long

\param[in] str - string to be converted
\param[out] out - pointer to converted number

\returns 1 if conversation was successful, 0 - otherwise
******************************************************************************/
uint8_t hexStrToUlong(const char *str, uint32_t *out)
{
  uint32_t shift = 0;
  int ret = 0;
  const char *p;

  if (!*str)
    return 0;  // Empty

  for (p = str; *p; p++);                  // Rewind to the end of string

  for (p--; p >= str && shift < 32; p--, shift += 4)     // locStrP points to the last symbol at the loop start
  {
    char chr = *p;

    if (chr >= 'a' && chr <= 'f')
      chr -= 'a' - 0xA;
    else if (chr >= 'A' && chr <= 'F')
      chr -= 'A' - 0xA;
    else if (chr >= '0' && chr <= '9')
      chr -= '0';
    else
      return 0;

    ret += chr << shift;
  }

  *out = ret;
  return 1;
}

/**************************************************************************//**
\brief Register commands in console

\param[in] table - pointer to an array of commands
******************************************************************************/
void consoleRegisterCommands(const ConsoleCommand_t *table)
{
  cmdTable = table;
}

/**************************************************************************//**
\brief Processes single char read from serial interface

\param[in] char - read char
******************************************************************************/
void consoleRx(char chr)
{
  static char cmdBuf[CMD_BUF_SIZE + 1];             // Additional space for end-of-string
  static char *p = cmdBuf;

  if (chr != '\n')                             // Not EOL
  {
    if (isprint((int)chr))
    {
      if ((p - cmdBuf) < CMD_BUF_SIZE)  // Put to buffer
      {
#if FORCE_LOWCASE
        chr = tolower(chr);
#endif
        *p++ = chr;
        //consoleTx(chr);                   // Echo
      }
    }
    else if (chr == '\b')                   // Backspace: erase symbol
    {
      if (p > cmdBuf)                     // There are symbols in buffer - delete'em
      {
        *(--p) = 0;                      // Rewind and terminate
        //console_tx_str(VT100_ERASE_LINE"\r" CMD_PROMPT);// Reprint string
        //console_tx_str(cmdBuf);
      }
    }
    else if (chr == VT100_ESCAPE)                  // Escape: flush buffer and erase symbols
    {
      p = cmdBuf;
      //console_tx_str(VT100_ERASE_LINE"\r" CMD_PROMPT);
    }
  }
  else                                            // End of command string
  {
    if (p > cmdBuf)                         // Non-empty command
    {
      *p = 0;                              // Mark an end of string
      processCommand(cmdBuf);
    }

    p = cmdBuf;                              // Drop buffer
    //console_tx_str(CRLF CMD_PROMPT);             // Command prompt
  }
}

/**************************************************************************//**
\brief Processes command

\param[in] Str - string with command
******************************************************************************/
void processCommand(char *Str)
{
  ScanStack_t stk;

  if (!tokenizeStr(Str, &stk))
    return;

  if (stk.top == stk.args)                        // No command name
    return;

  /** Seek for a matching command */
  for (const ConsoleCommand_t *cmd = cmdTable; cmd->name; cmd++)
  {
    if (strcmp(stk.args[0].str, cmd->name) == 0)    // Command match
    {
      if (!unpackArgs(cmd, &stk))
        consoleTxStr(cmd->helpMsg);
      else
        cmd->handler(stk.args + 1);
      return;
    }
  }

  consoleTxStr("unknown command\r\n");
}

/**************************************************************************//**
\brief Unpacks arguments according to the format string

\param[in] Cmd - command descriptor;
\param[in] Args - stack with pointers to arguments (some of them would be
               replaced by literal values after unpack)

\returns 1 if case of successful unpacking, 0 - otherwise
******************************************************************************/
static uint8_t unpackArgs(const ConsoleCommand_t *cmd, ScanStack_t *args)
{
  // Now we have a command and a stack of arguments
  const char *fmt = cmd->fmt;
  ScanValue_t *val = args->args + 1;

  // Unpack arguments of command according to format specifier
  for (; *fmt; fmt++, val++)
  {
    if (val >= args->top)
      return 0;

    switch (*fmt)
    {
      case 's':                                   // String, leave as is
        break;

      case 'c':                                   // Single char
        if (val->str[1])                        // Check next char - must be zero terminator
          return 0;
        val->chr = val->str[0];
        break;

      case 'd':                                   // Integer
      case 'i':
        // Lookup for hex prefix. Negative hex is not supported
        if (val->str[0] == '0' && (val->str[1] == 'x' || val->str[1] == 'X'))
        {
          if (!hexStrToUlong(&val->str[2], &val->uint32))
            return 0;
        }
        else                                    // Decimal
        {
          if (!decimalStrToSlong(val->str, &val->int32))
            return 0;
        }
        break;

      default:
        break;
    }
  }

  if (val != args->top)                           // Starvation of arguments
    return 0;

  return 1;
}

/**************************************************************************//**
\brief Splits string to white-separated tokens; "quotes" on tokens are supported

\param[in] Str - cinput string; each token(substring) would be zero-terminated;
\param[in] Stk - stack for pointers to tokens

\returns 1 if case of successful unpacking, 0 - otherwise
******************************************************************************/
static uint8_t tokenizeStr(char *str, ScanStack_t *stk)
{
  enum tokStates
  {
    WHITE,          // Inside of whitespaces
    TOK,            // Inside of token
    QTOK,           // Inside of quoted token
    POST_QTOK,      // Just after quoted token - waiting for whitespace
  };

  enum tokStates St = WHITE;

  stk->top = stk->args;

  for (; *str; str++)
  {
    switch (St)
    {
      case WHITE:
        if (*str == ' ')
          break;

        if (stk->top >= stk->end)          // No more space in stack
          return 0;

        if (*str == '"')
        {
          St = QTOK;
          (*stk->top++).str = str + 1;    // Store token position in stack (skipping quote)
        }
        else                                // Other symbol
        {
          St = TOK;
          (*stk->top++).str = str;        // Store token position in stack
        }
        break;

      case TOK:
        if (*str == '"')                    // Quotes are forbidden inside of plain token
          return 0;

        if (*str == ' ')
        {
          St = WHITE;
          *str = 0;                       // Put terminator
        }
        break;

      case QTOK:                              // Put terminator
        if (*str == '"')
        {
          St = POST_QTOK;
          *str = 0;
        }
        break;

      case POST_QTOK:
        if (*str != ' ')
          return 0;
        St = WHITE;
        break;
    }
  }

  if (St == QTOK)                             // Scan ended while inside of quote
    return 0;

  return 1;
}

#endif // APP_ENABLE_CONSOLE == 1