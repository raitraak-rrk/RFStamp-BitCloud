/***************************************************************************//**
\file  BoardConfig.h

\brief Definition of board-specific periphery for each platform.

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).
*******************************************************************************/
#ifdef BOARD_MESHBEAN
  #define _BUTTONS_
  #define _TEMPERATURE_SENSOR_
  #define _LIGHT_SENSOR_
  #define _LEDS_
  #define _SLIDERS_
#endif
#ifdef BOARD_RAVEN
  #define _LCD_
  #define _TEMPERATURE_SENSOR_
  #define _BATTERY_SENSOR_
#endif
#ifdef BOARD_USB_DONGLE
  #define _LEDS_
#endif
#ifdef BOARD_STK600
  #define _BUTTONS_
  #define _LEDS_
#endif
#ifdef BOARD_STK500
  #define _BUTTONS_
  #define _LEDS_
#endif
#ifdef BOARD_MESHBEAN_TURBO
  #define _LEDS_
  #define _SLIDERS_
  #define _BUTTONS_
#endif
#ifdef BOARD_MESHBEAN_LAN
  #define _LEDS_
  #define _BUTTONS_
#endif
#ifdef BOARD_SAM7X_EK
  #define _LEDS_
  #define _JOYSTICK_
#endif
#ifdef BOARD_SAM3S_EK
  #define _LEDS_
#endif
#ifdef BOARD_SAM3S_PIRRD
  #define _LEDS_
#endif
#ifdef BOARD_SAM4S_PIRRD
  #define _LEDS_
#endif
#ifdef BOARD_RF231USBRD
  #define _LEDS_
#endif
#ifdef BOARD_CUSTOM_3
  #define _LEDS_
#endif
#ifdef BOARD_CUSTOM_1
  #define _LEDS_
  #define _BUTTONS_
  #define _RS485CONTROL_
#endif
#ifdef BOARD_CUSTOM_2
  #define _LEDS_
  #define _BUTTONS_
  #define _RS485CONTROL_
  #define _USBCONTROL_
#endif
#ifdef BOARD_MEGARF
  #define _LEDS_
  #define _BUTTONS_
#endif
#ifdef BOARD_AVR32_EVK1105
  #define _LEDS_
  #define _BUTTONS_
#endif
#ifdef BOARD_XPLAIN
  #define _LEDS_
  #define _BUTTONS_
#endif
#ifdef BOARD_REB_CBB
  #define _LEDS_
  #define _BUTTONS_
#endif
#ifdef BOARD_SIMULATOR
  #define _LEDS_
#endif
#ifdef BOARD_SAMR21
  #define _BUTTONS_
#endif
