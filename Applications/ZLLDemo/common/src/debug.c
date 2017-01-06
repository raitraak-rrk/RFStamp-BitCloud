/**************************************************************************//**
  \file debug.c

  \brief
    Debugging routines implementation.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    24.06.11 A. Taradov - Created.
******************************************************************************/

#if APP_DEVICE_EVENTS_LOGGING == 1

/******************************************************************************
                    Includes section
******************************************************************************/
#include <zllDemo.h>
#include <debug.h>
#include <sysTypes.h>
#include <appTimer.h>
#if defined(BOARD_MEGARF) || defined(BOARD_SAMR21)
#include <lcdExt.h>
#endif
#include <stdarg.h>
#include <stdio.h>

/******************************************************************************
                    Local variables section
******************************************************************************/
static HAL_AppTimer_t timer;

/******************************************************************************
                    Implementations section
******************************************************************************/
/**************************************************************************//**
\brief Clear the LCD information line
******************************************************************************/
static void clearLine(void)
{
  const char clean[] = "                     ";

  LCD_PRINT(0, 3, clean);
  (void)clean;
}

/**************************************************************************//**
\brief Print a formatted string on the LCD for 10 seconds
******************************************************************************/
void dbgLcdMsg(const char *format, ...)
{
  uint8_t str[21];
  char *ptr = (char *)str;
  va_list ap;

  va_start(ap, format);
  vsnprintf(ptr, sizeof(str), format, ap);
  va_end(ap);
  clearLine();
  LCD_PRINT(0, 3, ptr);

  HAL_StopAppTimer(&timer);
  timer.mode = TIMER_ONE_SHOT_MODE;
  timer.interval = 5000;
  timer.callback = clearLine;
  HAL_StartAppTimer(&timer);
}

#endif // APP_DEVICE_EVENTS_LOGGING == 1

// eof debug.c
