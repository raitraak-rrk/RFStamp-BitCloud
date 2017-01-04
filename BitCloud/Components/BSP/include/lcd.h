/**************************************************************************//**
  \file  lcd.h

  \brief The header file describes enums and functions of BSP LCD controller.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    17/12/08 E. ALuzhetsky - Created
*******************************************************************************/

#ifndef _LCD_H
#define _LCD_H

/******************************************************************************
                   Includes section
******************************************************************************/
// \cond
#include <sysTypes.h>
#include <sysTaskManager.h>
// \endcond

/******************************************************************************
                   Define(s) section
******************************************************************************/
typedef enum
{
  // Symbols
  LCD_CMD_SYMB_RAVEN_ON = 0x00,
  LCD_CMD_SYMB_RAVEN_OFF = 0x01,

  LCD_CMD_SYMB_BELL_ON = 0x02,
  LCD_CMD_SYMB_BELL_OFF = 0x03,

  LCD_CMD_SYMB_TONE_ON = 0x04,
  LCD_CMD_SYMB_TONE_OFF = 0x05,

  LCD_CMD_SYMB_MIC_ON = 0x06,
  LCD_CMD_SYMB_MIC_OFF = 0x07,

  LCD_CMD_SYMB_SPEAKER_ON = 0x08,
  LCD_CMD_SYMB_SPEAKER_OFF = 0x09,

  LCD_CMD_SYMB_KEY_ON = 0x0a,
  LCD_CMD_SYMB_KEY_OFF = 0x0b,

  LCD_CMD_SYMB_ATT_ON = 0x0c,
  LCD_CMD_SYMB_ATT_OFF = 0x0d,

  LCD_CMD_SYMB_SPACE_SUN = 0x0e,
  LCD_CMD_SYMB_SPACE_MOON = 0x0f,
  LCD_CMD_SYMB_SPACE_OFF = 0x10,

  LCD_CMD_SYMB_CLOCK_AM = 0x11,
  LCD_CMD_SYMB_CLOCK_PM = 0x12,
  LCD_CMD_SYMB_CLOCK_OFF = 0x13,

  LCD_CMD_SYMB_TRX_RX = 0x14,
  LCD_CMD_SYMB_TRX_TX = 0x15,
  LCD_CMD_SYMB_TRX_OFF = 0x16,

  LCD_CMD_SYMB_IP_ON = 0x17,
  LCD_CMD_SYMB_IP_OFF = 0x18,

  LCD_CMD_SYMB_PAN_ON = 0x19,
  LCD_CMD_SYMB_PAN_OFF = 0x1a,

  LCD_CMD_SYMB_ZLINK_ON = 0x1b,
  LCD_CMD_SYMB_ZLINK_OFF = 0x1c,

  LCD_CMD_SYMB_ZIGBEE_ON = 0x1d,
  LCD_CMD_SYMB_ZIGBEE_OFF = 0x1e,

  LCD_CMD_SYMB_ANTENNA_LEVEL_0 = 0x1f,
  LCD_CMD_SYMB_ANTENNA_LEVEL_1 = 0x20,
  LCD_CMD_SYMB_ANTENNA_LEVEL_2 = 0x21,
  LCD_CMD_SYMB_ANTENNA_OFF = 0x22,

  //LCD_CMD_SYMB_BAT // bettery symbol is controlled by ATMega3290...

  LCD_CMD_SYMB_ENV_OPEN = 0x23,
  LCD_CMD_SYMB_ENV_CLOSE = 0x24,
  LCD_CMD_SYMB_ENV_OFF = 0x25,

  LCD_CMD_SYMB_TEMP_CELSIUS = 0x26,
  LCD_CMD_SYMB_TEMP_FAHRENHEIT = 0x27,
  LCD_CMD_SYMB_TEMP_OFF = 0x28,

  LCD_CMD_SYMB_MINUS_ON = 0x29,
  LCD_CMD_SYMB_MINUS_OFF = 0x2a,

  LCD_CMD_SYMB_DOT_ON = 0x2b,
  LCD_CMD_SYMB_DOT_OFF = 0x2c,

  LCD_CMD_SYMB_COL_ON = 0x2d,
  LCD_CMD_SYMB_COL_OFF = 0x2e,

  // Led
  LCD_CMD_LED_ON = 0x2f,
  LCD_CMD_LED_TOGGLE = 0x30,
  LCD_CMD_LED_OFF = 0x31,
} BspLcdCmd_t ;

/******************************************************************************
                   External variables section
******************************************************************************/
/******************************************************************************
                   Function prototypes section
******************************************************************************/
/**************************************************************************//**
\brief Initializes UART to control LCD.
******************************************************************************/
extern void BSP_OpenLcd(void);

/**************************************************************************//**
\brief Sends the command to the LCD.
******************************************************************************/
extern void BSP_SendLcdCmd(BspLcdCmd_t cmdId);

/**************************************************************************//**
\brief Sends the message to the LCD.
******************************************************************************/
extern void BSP_SendLcdMsg(const char *str);

/**************************************************************************//**
\brief Checks whether the LCD command transmission is finished or not.

\param  void
\return 1 in case if empty, 0 otherwise
******************************************************************************/
extern int BSP_IsLcdOperationCompleted(void);

#endif /* _LCD_H */
// eof lcd.h
