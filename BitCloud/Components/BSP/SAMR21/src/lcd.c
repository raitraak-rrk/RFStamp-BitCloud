/**************************************************************************//**
  \file lcd.c

  \brief
    UG-2832HSWEG04 LCD routines implementation.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    02.06.14 P. Mahendran - Created.
******************************************************************************/
#if BSP_SUPPORT == BOARD_SAMR21_ZLLEK

/******************************************************************************
                    Includes section
******************************************************************************/
#include <sysTypes.h>
#include <gpio.h>
#include <spi.h>
#include <lcd.h>
#include <lcdExt.h>
#include <stdio.h>
#include <stdarg.h>

/******************************************************************************
                    Definitions section
******************************************************************************/
#define LCD_CMD_COL_ADD_SET_LSB(column)         (0x00 | (column))
#define LCD_CMD_COL_ADD_SET_MSB(column)         (0x10 | (column))
#define LCD_CMD_SET_MEMORY_ADDRESSING_MODE      0x20
#define LCD_CMD_SET_COLUMN_ADDRESS              0x21
#define LCD_CMD_SET_PAGE_ADDRESS                0x22
#define LCD_CMD_SET_DISPLAY_START_LINE(line)    (0x40 | (line))
#define LCD_CMD_SET_CONTRAST_CONTROL_FOR_BANK0  0x81
#define LCD_CMD_SET_CHARGE_PUMP_SETTING         0x8D
#define LCD_CMD_SET_SEGMENT_RE_MAP_COL0_SEG0    0xA0
#define LCD_CMD_SET_SEGMENT_RE_MAP_COL127_SEG0  0xA1
#define LCD_CMD_ENTIRE_DISPLAY_AND_GDDRAM_ON    0xA4
#define LCD_CMD_ENTIRE_DISPLAY_ON               0xA5
#define LCD_CMD_SET_NORMAL_DISPLAY              0xA6
#define LCD_CMD_SET_INVERSE_DISPLAY             0xA7
#define LCD_CMD_SET_MULTIPLEX_RATIO             0xA8
#define LCD_CMD_SET_DISPLAY_ON                  0xAF
#define LCD_CMD_SET_DISPLAY_OFF                 0xAE
#define LCD_CMD_SET_PAGE_START_ADDRESS(page)    (0xB0 | (page))
#define LCD_CMD_SET_COM_OUTPUT_SCAN_UP          0xC0
#define LCD_CMD_SET_COM_OUTPUT_SCAN_DOWN        0xC8
#define LCD_CMD_SET_DISPLAY_OFFSET              0xD3
#define LCD_CMD_SET_DISPLAY_CLOCK_DIVIDE_RATIO  0xD5
#define LCD_CMD_SET_PRE_CHARGE_PERIOD           0xD9
#define LCD_CMD_SET_COM_PINS                    0xDA
#define LCD_CMD_SET_VCOMH_DESELECT_LEVEL        0xDB
#define LCD_CMD_NOP                             0xE3
#define LCD_CMD_SCROLL_H_RIGHT                  0x26
#define LCD_CMD_SCROLL_H_LEFT                   0x27
#define LCD_CMD_CONTINUOUS_SCROLL_V_AND_H_RIGHT 0x29
#define LCD_CMD_CONTINUOUS_SCROLL_V_AND_H_LEFT  0x2A
#define LCD_CMD_DEACTIVATE_SCROLL               0x2E
#define LCD_CMD_ACTIVATE_SCROLL                 0x2F
#define LCD_CMD_SET_VERTICAL_SCROLL_AREA        0xA3

#define NUMBER_OF_LCD_BANKS     8
#define NUMBER_OF_LCD_COLUMNS   128

#define NUMBER_OF_TEXT_LINES    4
#define NUMBER_OF_TEXT_COLUMNS  21

#define LCD_SPI_CS_CONFIG   PORT_B, 22u, 3u
#define LCD_CMDSEL_CONFIG   PORT_A, 22u, 3u
#define LCD_RESET_CONFIG    PORT_A, 23u, 3u

/******************************************************************************
                    Local variables section
******************************************************************************/
static HAL_SpiDescriptor_t lcdSpiDescriptor;
static uint8_t cursorx, cursory;

static uint8_t PROGMEM_DECLARE(fontdata[]) =
{
  0x00, 0x00, 0x00, 0x00, 0x00, // (space)    #32
  0x00, 0x00, 0x5F, 0x00, 0x00, // !
  0x00, 0x07, 0x00, 0x07, 0x00, // "
  0x14, 0x7F, 0x14, 0x7F, 0x14, // #
  0x24, 0x2A, 0x7F, 0x2A, 0x12, // $
  0x23, 0x13, 0x08, 0x64, 0x62, // %
  0x36, 0x49, 0x55, 0x22, 0x50, // &
  0x00, 0x05, 0x03, 0x00, 0x00, // '
  0x00, 0x1C, 0x22, 0x41, 0x00, // (          #40
  0x00, 0x41, 0x22, 0x1C, 0x00, // )
  0x08, 0x2A, 0x1C, 0x2A, 0x08, // *
  0x08, 0x08, 0x3E, 0x08, 0x08, // +
  0x00, 0x50, 0x30, 0x00, 0x00, // ,
  0x08, 0x08, 0x08, 0x08, 0x08, // -
  0x00, 0x60, 0x60, 0x00, 0x00, // .
  0x20, 0x10, 0x08, 0x04, 0x02, // /
  0x3E, 0x51, 0x49, 0x45, 0x3E, // 0
  0x00, 0x42, 0x7F, 0x40, 0x00, // 1
  0x42, 0x61, 0x51, 0x49, 0x46, // 2          #50
  0x21, 0x41, 0x45, 0x4B, 0x31, // 3
  0x18, 0x14, 0x12, 0x7F, 0x10, // 4
  0x27, 0x45, 0x45, 0x45, 0x39, // 5
  0x3C, 0x4A, 0x49, 0x49, 0x30, // 6
  0x01, 0x71, 0x09, 0x05, 0x03, // 7
  0x36, 0x49, 0x49, 0x49, 0x36, // 8
  0x06, 0x49, 0x49, 0x29, 0x1E, // 9
  0x00, 0x36, 0x36, 0x00, 0x00, // :
  0x00, 0x56, 0x36, 0x00, 0x00, // ;
  0x00, 0x08, 0x14, 0x22, 0x41, // <          #60
  0x14, 0x14, 0x14, 0x14, 0x14, // =
  0x41, 0x22, 0x14, 0x08, 0x00, // >
  0x02, 0x01, 0x51, 0x09, 0x06, // ?
  0x32, 0x49, 0x79, 0x41, 0x3E, // @
  0x7E, 0x11, 0x11, 0x11, 0x7E, // A
  0x7F, 0x49, 0x49, 0x49, 0x36, // B
  0x3E, 0x41, 0x41, 0x41, 0x22, // C
  0x7F, 0x41, 0x41, 0x22, 0x1C, // D
  0x7F, 0x49, 0x49, 0x49, 0x41, // E
  0x7F, 0x09, 0x09, 0x01, 0x01, // F          #70
  0x3E, 0x41, 0x41, 0x51, 0x32, // G
  0x7F, 0x08, 0x08, 0x08, 0x7F, // H
  0x00, 0x41, 0x7F, 0x41, 0x00, // I
  0x20, 0x40, 0x41, 0x3F, 0x01, // J
  0x7F, 0x08, 0x14, 0x22, 0x41, // K
  0x7F, 0x40, 0x40, 0x40, 0x40, // L
  0x7F, 0x02, 0x04, 0x02, 0x7F, // M
  0x7F, 0x04, 0x08, 0x10, 0x7F, // N
  0x3E, 0x41, 0x41, 0x41, 0x3E, // O
  0x7F, 0x09, 0x09, 0x09, 0x06, // P          #80
  0x3E, 0x41, 0x51, 0x21, 0x5E, // Q
  0x7F, 0x09, 0x19, 0x29, 0x46, // R
  0x46, 0x49, 0x49, 0x49, 0x31, // S
  0x01, 0x01, 0x7F, 0x01, 0x01, // T
  0x3F, 0x40, 0x40, 0x40, 0x3F, // U
  0x1F, 0x20, 0x40, 0x20, 0x1F, // V
  0x7F, 0x20, 0x18, 0x20, 0x7F, // W
  0x63, 0x14, 0x08, 0x14, 0x63, // X
  0x03, 0x04, 0x78, 0x04, 0x03, // Y
  0x61, 0x51, 0x49, 0x45, 0x43, // Z          #90
  0x00, 0x00, 0x7F, 0x41, 0x41, // [
  0x02, 0x04, 0x08, 0x10, 0x20, // "\"
  0x41, 0x41, 0x7F, 0x00, 0x00, // ]
  0x04, 0x02, 0x01, 0x02, 0x04, // ^
  0x40, 0x40, 0x40, 0x40, 0x40, // _
  0x00, 0x01, 0x02, 0x04, 0x00, // `
  0x20, 0x54, 0x54, 0x54, 0x78, // a
  0x7F, 0x48, 0x44, 0x44, 0x38, // b
  0x38, 0x44, 0x44, 0x44, 0x20, // c
  0x38, 0x44, 0x44, 0x48, 0x7F, // d         #100
  0x38, 0x54, 0x54, 0x54, 0x18, // e
  0x08, 0x7E, 0x09, 0x01, 0x02, // f
  0x08, 0x14, 0x54, 0x54, 0x3C, // g
  0x7F, 0x08, 0x04, 0x04, 0x78, // h
  0x00, 0x44, 0x7D, 0x40, 0x00, // i
  0x20, 0x40, 0x44, 0x3D, 0x00, // j
  0x00, 0x7F, 0x10, 0x28, 0x44, // k
  0x00, 0x41, 0x7F, 0x40, 0x00, // l
  0x7C, 0x04, 0x18, 0x04, 0x78, // m
  0x7C, 0x08, 0x04, 0x04, 0x78, // n         #110
  0x38, 0x44, 0x44, 0x44, 0x38, // o
  0x7C, 0x14, 0x14, 0x14, 0x08, // p
  0x08, 0x14, 0x14, 0x18, 0x7C, // q
  0x7C, 0x08, 0x04, 0x04, 0x08, // r
  0x48, 0x54, 0x54, 0x54, 0x20, // s
  0x04, 0x3F, 0x44, 0x40, 0x20, // t
  0x3C, 0x40, 0x40, 0x20, 0x7C, // u
  0x1C, 0x20, 0x40, 0x20, 0x1C, // v
  0x3C, 0x40, 0x30, 0x40, 0x3C, // w
  0x44, 0x28, 0x10, 0x28, 0x44, // x         #120
  0x0C, 0x50, 0x50, 0x50, 0x3C, // y
  0x44, 0x64, 0x54, 0x4C, 0x44, // z
  0x00, 0x08, 0x36, 0x41, 0x00, // {
  0x00, 0x00, 0x7F, 0x00, 0x00, // |
  0x00, 0x41, 0x36, 0x08, 0x00, // }
  0x02, 0x01, 0x02, 0x04, 0x02, // ~
  0x78, 0x44, 0x42, 0x44, 0x78  // DEL
};

/******************************************************************************
                    Prototypes section
******************************************************************************/
void HAL_Delay(uint8_t us);

/******************************************************************************
                    Implementations section
******************************************************************************/
/**************************************************************************//**
\brief Send command to the LCD
******************************************************************************/
static void lcdCommand(uint8_t command)
{
  PinConfig_t lcdSpiCSPin = {LCD_SPI_CS_CONFIG};
  PinConfig_t lcdCmdSelPin = {LCD_CMDSEL_CONFIG};
  GPIO_clr(&lcdSpiCSPin);
  GPIO_clr(&lcdCmdSelPin);
  HAL_WriteSpi(&lcdSpiDescriptor,&command, 1);
  GPIO_set(&lcdSpiCSPin);
}
/**************************************************************************//**
\brief Send data to the LCD
******************************************************************************/
static void lcdData(uint8_t data)
{
  PinConfig_t lcdSpiCSPin = {LCD_SPI_CS_CONFIG};
  PinConfig_t lcdCmdSelPin = {LCD_CMDSEL_CONFIG};
  GPIO_clr(&lcdSpiCSPin);
  GPIO_set(&lcdCmdSelPin);
  HAL_WriteSpi(&lcdSpiDescriptor,&data, 1);
  GPIO_set(&lcdSpiCSPin);
}

/**************************************************************************//**
\brief Print a character on the LCD at current position
******************************************************************************/
static void lcdChar(uint8_t chr)
{
  uint8_t font[5];
  uint16_t offset;
  uint8_t column = (cursorx)*6; // complex formula due to all the mirroring done by the LCD

  lcdCommand(LCD_CMD_COL_ADD_SET_LSB((column & 0x0f)));
  lcdCommand(LCD_CMD_COL_ADD_SET_MSB((column >> 4) & 0x0F));
  lcdCommand(LCD_CMD_SET_PAGE_START_ADDRESS((cursory & 0x03)));

  if (chr < 32 || chr > 127)
    chr = '.';

  offset = (chr-32) * 5;
  memcpy_P(font, &fontdata[offset], sizeof(font));

  for (uint8_t i = 0; i < 5; i++)
    lcdData(font[i]);

  lcdData(0); // space between characters

  cursorx++;
  if (cursorx == NUMBER_OF_TEXT_COLUMNS)
  {
    cursorx = 0;
    cursory++;
  }

  if (cursory > (NUMBER_OF_TEXT_COLUMNS-1))
    cursory = 0;
}

/**************************************************************************//**
\brief Clear screen
******************************************************************************/
void BSP_LcdCls(void)
{
  for (uint8_t i = 0; i < NUMBER_OF_LCD_BANKS; i++)
  {
    lcdCommand(LCD_CMD_COL_ADD_SET_LSB(0));
    lcdCommand(LCD_CMD_COL_ADD_SET_MSB(0));
    lcdCommand(LCD_CMD_SET_PAGE_START_ADDRESS(i));

    for (uint8_t j = 0; j < NUMBER_OF_LCD_COLUMNS; j++)
      lcdData(0x00);
  }
}
/**************************************************************************//**
\brief Fill the LCD screen (using internal test command)
******************************************************************************/
void BSP_LcdFill(bool fill)
{
  lcdCommand(LCD_CMD_SET_NORMAL_DISPLAY | (fill ? 1 : 0));
}

/**************************************************************************//**
\brief Print a formatted string on the LCD
******************************************************************************/
void BSP_LcdPrint(uint8_t x, uint8_t y, const char *format, ...)
{
  uint8_t str[100];
  char *ptr = (char *)str;
  uint8_t len;
  va_list ap;

  cursorx = x;
  cursory = y;

  va_start(ap, format);
  len = vsnprintf(ptr, 100, format, ap);
  va_end(ap);
  while (len--)
    lcdChar(*ptr++);
}

/**************************************************************************//**
\brief Initialize the LCD
******************************************************************************/
void BSP_LcdInit(void)
{
  PinConfig_t lcdResetPin = {LCD_RESET_CONFIG};
  PinConfig_t lcdCmdSelPin = {LCD_CMDSEL_CONFIG};

  /* Initialize LCD Pins Other than SPI connected Pins */
  GPIO_make_out(&lcdResetPin);
  GPIO_make_out(&lcdCmdSelPin);

  /* Initialize SPI Interface */
  lcdSpiDescriptor.tty       = SPI_CHANNEL_1;
  lcdSpiDescriptor.baudRate  = SPI_CLOCK_RATE_1000;
  lcdSpiDescriptor.clockMode = SPI_CLOCK_MODE0;
  lcdSpiDescriptor.dataOrder = SPI_DATA_MSB_FIRST;
  lcdSpiDescriptor.callback  = NULL;

  if (-1 == HAL_OpenSpi(&lcdSpiDescriptor))
  {
    return;
  }

  /* Reset LCD before Initialization*/
  GPIO_clr(&lcdResetPin);
  HAL_Delay(100);
  GPIO_set(&lcdResetPin);

  /* Multiplex ratio 1/32 Duty (0x0F~0x3F) */
  lcdCommand(LCD_CMD_SET_MULTIPLEX_RATIO);
  lcdCommand(0x1F);

  /* Set Display Offset */
  lcdCommand(LCD_CMD_SET_DISPLAY_OFFSET);
  lcdCommand(0x00);

  /* Set Display Start Line (0x00~0x3F) */
  lcdCommand(LCD_CMD_SET_DISPLAY_START_LINE(0x00));

  /* Set Column Address 0 Mapped to SEG0 */
  lcdCommand(LCD_CMD_SET_SEGMENT_RE_MAP_COL127_SEG0);

  /* Set COM/Row Scan Scan from COM63 to 0 */
  lcdCommand(LCD_CMD_SET_COM_OUTPUT_SCAN_DOWN);

  /* Set COM Pins hardware configuration */
  lcdCommand(LCD_CMD_SET_COM_PINS);
  lcdCommand(0x02);

  /* Set Contrast Control */
  lcdCommand(LCD_CMD_SET_CONTRAST_CONTROL_FOR_BANK0);
  lcdCommand(0x7F);

  /* Disable Entire display On */
  lcdCommand(LCD_CMD_ENTIRE_DISPLAY_AND_GDDRAM_ON);

  /* Set Normal Display */
  lcdCommand(LCD_CMD_SET_NORMAL_DISPLAY);

  /* Set Display Oscillator Frequency/Clock Divide Ratio */
  lcdCommand(LCD_CMD_SET_DISPLAY_CLOCK_DIVIDE_RATIO);
  lcdCommand(0x80);

  /* Enable charge pump regulator */
  lcdCommand(LCD_CMD_SET_CHARGE_PUMP_SETTING);
  lcdCommand(0x14);

  /* Set Display On */
  lcdCommand(LCD_CMD_SET_DISPLAY_ON);

  BSP_LcdCls();
}

#endif // BSP_SUPPORT == BOARD_SAMR21_ZLLEK
// eof lcd.c
