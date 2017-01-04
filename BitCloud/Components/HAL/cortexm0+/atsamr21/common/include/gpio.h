/**************************************************************************//**
\file  gpio.h

\brief This module contains a set of functions to manipulate GPIO pins.

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal
  History:
    07/04/14 karthik.p_u - Created
******************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/
#ifndef _GPIO_H
#define _GPIO_H

/******************************************************************************
                   Includes section
******************************************************************************/
#include <sysTypes.h>
#include <atsamr21.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
/******************************************************************************
* void gpioX_set() sets GPIOX pin to logical 1 level.
* void gpioX_clr() clears GPIOX pin to logical 0 level.
* void gpioX_make_in makes GPIOX pin as input.
* void gpioX_make_in makes GPIOX pin as output.
* uint8_t gpioX_read() returns logical level GPIOX pin.
* uint8_t gpioX_state() returns configuration of GPIOX port.
*******************************************************************************/
#define HAL_ASSIGN_PIN(name, port, bit) \
INLINE void  GPIO_##name##_set()           {PORT##port##_OUTSET = (1 << bit); } \
INLINE void  GPIO_##name##_clr()           {PORT##port##_OUTCLR = (1 << bit);} \
INLINE uint8_t  GPIO_##name##_read()       {return (PORT##port##_IN & (1 << bit)) != 0;} \
INLINE uint8_t  GPIO_##name##_state()      {return (PORT##port##_DIR & (1 << bit)) != 0;} \
INLINE void  GPIO_##name##_make_out()      {PORT##port##_DIRSET = (1 << bit);\
                                            PORT##port##_PINCFG##bit |= PORTA_PINCFG##bit##_INEN; } \
INLINE void  GPIO_##name##_make_in()       {PORT##port##_DIRCLR = (1 << bit);   \
                                            PORT##port##_PINCFG##bit |= PORTA_PINCFG##bit##_INEN; \
                                            PORT##port##_PINCFG##bit &= ~PORTA_PINCFG##bit##_PULLEN;  } \
INLINE void  GPIO_##name##_make_pullup()   {PORT##port##_OUTSET = (1 << bit); \
                                            PORT##port##_PINCFG##bit |= PORTA_PINCFG##bit##_PULLEN; }  \
INLINE void  GPIO_##name##_make_pulldown() {PORT##port##_WRCONFIG_s.hwsel = (bit/16) &0x01; \
                                            PORT##port##_WRCONFIG_s.pinmask = (bit%16) & 0xF; \
                                            PORT##port##_WRCONFIG_s.pullen = 0; \
                                            PORT##port##_WRCONFIG_s.wrpincfg = 1; } \
INLINE void  GPIO_##name##_toggle()        {PORT##port##_OUTTGL = (1 << bit);} \
INLINE void  GPIO_##name##_pmuxen(void)   { PORT##port##_PINCFG##bit |= PORTA_PINCFG##bit##_PMUXEN; } \
INLINE void  GPIO_##name##_pmuxdis(void)  { PORT##port##_PINCFG##bit &= ~PORTA_PINCFG##bit##_PMUXEN; }

/* port - port A, B, C
   bit - bit position 0-32
   funcionality - refer I/O multiplexing
*/
#define HAL_ASSIGN_PIN_FUNC(name, port, bit, func) \
INLINE void GPIO_##name##_config_pin()    { \
                                          uint8_t bit_pos = bit/2; \
                                          uint8_t pmux_pos = (bit % 2) ? 4 : 0; \
                                          *(&PORT##port##_PMUX0 + bit_pos) |= (func << pmux_pos) ; \
                                          }

/******************************************************************************
                   Types section
******************************************************************************/
typedef enum
{
  PORT_A,
  PORT_B,
  PORT_C
} PortNum_t;

typedef struct
{
  PortNum_t portNum;
  uint8_t pinNum;
  uint8_t functionConfig;
} PinConfig_t;

/******************************************************************************
                   Inline static functions section
******************************************************************************/ 
// the macros for the manipulation by A4
HAL_ASSIGN_PIN(A4, A, 4);
// the macros for the manipulation by A5
HAL_ASSIGN_PIN(A5, A, 5);
// the macros for the manipulation by A6
HAL_ASSIGN_PIN(A6, A, 6);
// the macros for the manipulation by A7
HAL_ASSIGN_PIN(A7, A, 7);
// the macros for the manipulation by A8
HAL_ASSIGN_PIN(A8, A, 8);
// the macros for the manipulation by A9
HAL_ASSIGN_PIN(A9, A, 9);
// the macros for the manipulation by A12
HAL_ASSIGN_PIN(A12, A, 12);
// the macros for the manipulation by A13
HAL_ASSIGN_PIN(A13, A, 13);
// the macros for the manipulation by A14
HAL_ASSIGN_PIN(A14, A, 14);
// the macros for the manipulation by A15
HAL_ASSIGN_PIN(A15, A, 15);
// the macros for the manipulation by A16
HAL_ASSIGN_PIN(A16, A, 16);
// the macros for the manipulation by A17
HAL_ASSIGN_PIN(A17, A, 17);
// the macros for the manipulation by A18
HAL_ASSIGN_PIN(A18, A, 18);
// the macros for the manipulation by A19
HAL_ASSIGN_PIN(A19, A, 19);
// the macros for the manipulation by A22
HAL_ASSIGN_PIN(A22, A, 22);
// the macros for the manipulation by A23
HAL_ASSIGN_PIN(A23, A, 23);
// the macros for the manipulation by A27
HAL_ASSIGN_PIN(A27, A, 27);
// the macros for the manipulation by A28
HAL_ASSIGN_PIN(A28, A, 28);
// the macros for the manipulation by B22
HAL_ASSIGN_PIN(B22, B, 22);
// the macros for the manipulation by B23
HAL_ASSIGN_PIN(B23, B, 23);
// the macros for the manipulation by B3
HAL_ASSIGN_PIN(B3, B, 3);
// the macros for the manipulation by B2
HAL_ASSIGN_PIN(B02, B, 2);

INLINE void GPIO_make_out(PinConfig_t *pinConfig)
{
  *(&PORTA_DIRSET + pinConfig->portNum * 0x20) = ((uint32_t)1 <<  pinConfig->pinNum);
}

INLINE void GPIO_make_in(PinConfig_t *pinConfig)
{
  *(&PORTA_DIRCLR + pinConfig->portNum * 0x20) = ((uint32_t)1 <<  pinConfig->pinNum);
  *(&PORTA_PINCFG0 + pinConfig->portNum * 0x80 +  pinConfig->pinNum) |= PORT_PINCFG_INEN;
  *(&PORTA_PINCFG0 + pinConfig->portNum * 0x80 +  pinConfig->pinNum) &= ~PORT_PINCFG_PULLEN;
}

INLINE void GPIO_pinfunc_config(PinConfig_t *pinConfig)
{
  *(&PORTA_PINCFG0 +  pinConfig->portNum * 0x80 +  pinConfig->pinNum) |= PORT_PINCFG_PMUXEN;
  if (pinConfig->pinNum % 2)
  {
   *(&PORTA_PMUX0 +  pinConfig->portNum * 0x80 +  pinConfig->pinNum / 2) = \
     ((*(&PORTA_PMUX0 +  pinConfig->portNum * 0x80 +  pinConfig->pinNum / 2) & 0x0F) |  (pinConfig->functionConfig << 4));
  }
  else
  {
   *(&PORTA_PMUX0 +  pinConfig->portNum * 0x80 +  pinConfig->pinNum / 2) = \
     ((*(&PORTA_PMUX0 +  pinConfig->portNum * 0x80 +  pinConfig->pinNum / 2) & 0xF0) |  (pinConfig->functionConfig));   
  }
}

INLINE void GPIO_disable_pmux(PinConfig_t *pinConfig)
{
  *(&PORTA_PINCFG0 +  pinConfig->portNum * 0x80 +  pinConfig->pinNum) &= ~PORT_PINCFG_PMUXEN;
}

INLINE void GPIO_set(PinConfig_t *pinConfig)
{
  *(&PORTA_OUTSET + pinConfig->portNum * 0x20) = ((uint32_t)1 <<  pinConfig->pinNum);
}

INLINE void GPIO_clr(PinConfig_t *pinConfig)
{
  *(&PORTA_OUTCLR + pinConfig->portNum * 0x20) = ((uint32_t)1 <<  pinConfig->pinNum);
}

INLINE uint8_t GPIO_read(PinConfig_t *pinConfig)
{
  return ((*(&PORTA_IN + pinConfig->portNum * 0x20) >> pinConfig->pinNum) & 0x01);
}

INLINE void GPIO_make_pullup(PinConfig_t *pinConfig)
{
  *(&PORTA_OUTSET + pinConfig->portNum * 0x20) = ((uint32_t)1 <<  pinConfig->pinNum);
  *(&PORTA_PINCFG0 + pinConfig->portNum * 0x80 +  pinConfig->pinNum) |= PORT_PINCFG_PULLEN;
}
#endif
//eof gpio.h
