/**************************************************************************//**
\file   dithPwm.c

\brief  Pwm with dithering implementation.

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal
  History:
    19/02/13 A. Khromykh - Created
*******************************************************************************/

#if APP_ZLL_DEVICE_TYPE >= APP_DEVICE_TYPE_COLOR_LIGHT

/******************************************************************************
                   Includes section
******************************************************************************/
#include <dithTimerIsr.h>
#include <dithPwm.h>
#include <halClkCtrl.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
/* updating the three OCRx registers results in
 11 lds
 4 sub/subi/sbc/sbci
 3 in
 3 out
 1 cli
 2 cpi/cpc
 1 brcs
 7 sts
 6 cycles are added for safety, not knowing what other compilers do, to safe these cycles,
 the update of the OCRx registers should be done with inline assembler instead of C-code */
#define OCR_UPD_CYCLES 56

/******************************************************************************
                   Static Function Prototypes section
******************************************************************************/
static inline void setPwmRegs(void);
static void calcPwmRegs(uint16_t r, uint16_t g, uint16_t b);

/******************************************************************************
                   Global variables section
******************************************************************************/
static uint16_t red, green, blue;              //duty cycle values
PwmParams_t currPwmParam, newPwmParam;

/******************************************************************************
                   Implementations section
******************************************************************************/
/**************************************************************************//**
\brief Enable/Disable dithering

\param[in]
  cha - enable/disable dithering on timer channel A
\param[in]
  chb - enable/disable dithering on timer channel B
\param[in]
  chc - enable/disable dithering on timer channel C
******************************************************************************/
void dithSetDithering(uint8_t cha, uint8_t chb, uint8_t chc)
{
  newPwmParam.red_ena_dith = cha;
  newPwmParam.green_ena_dith = chb;
  newPwmParam.blue_ena_dith = chc;

  cli();
  setPwmRegs();
  sei();
}

/*************************************************************************//**
\brief Calculate duty cycle values
******************************************************************************/
static void calcPwmRegs(uint16_t r, uint16_t g, uint16_t b)
{
  // *******************
  // **** red LED ******
  // *******************
  newPwmParam.red_sum = 0;
  if (r == UINT16_MAX)
  {
    /* handling for special case of 100% duty cycle */
    newPwmParam.red_m  = 0;
    newPwmParam.ocra     = 0;
    newPwmParam.tccra    = (0 << COMxA1 | 0 << COMxA0 |
                           1 << WGMx1  | 0 << WGMx0);
    newPwmParam.red_mode = 1;
  }
  else
  {
    newPwmParam.red_m  = ((((newPwmParam.icr + 1L) * r) >> 8) & 0xFF);
    if (((newPwmParam.icr + 1L) * (UINT16_MAX - r)) > UINT16_MAX)
    {
      newPwmParam.ocra     = (uint16_t)(newPwmParam.icr - (((newPwmParam.icr + 1L) * r) >> 16));
      newPwmParam.tccra    = (1 << COMxA1 | 1 << COMxA0 |
                             1 << WGMx1  | 0 << WGMx0);
      newPwmParam.red_mode = 1;
    }
    else
    {
      /* duty cycle is less than 1 timer period, switch pwm off */
      newPwmParam.ocra     = 0;
      newPwmParam.tccra    = (0 << COMxA1 | 0 << COMxA0 |
                             1 << WGMx1  | 0 << WGMx0);
      newPwmParam.red_mode = 2;
    }
  }

  // *******************
  // **** green LED ****
  // *******************
  newPwmParam.green_sum = 0;
  if (g == UINT16_MAX)
  {
    /* special case: 100% duty cycle */
    newPwmParam.green_m  = 0;
    newPwmParam.ocrb       = newPwmParam.icr;
    newPwmParam.tccra     |= (1 << COMxB1 | 0 << COMxB0);
    newPwmParam.green_mode = 1;
  }
  else
  {
    newPwmParam.green_m  = ((((newPwmParam.icr + 1L) * g) >> 8) & 0xFF);
    if (((newPwmParam.icr + 1L) * g) > UINT16_MAX)
    {
      /* handling of regular */
      newPwmParam.ocrb       = (uint16_t)(((newPwmParam.icr + 1L) * g) >> 16) - 1;
      newPwmParam.tccra     |= (1 << COMxB1 | 0 << COMxB0);
      newPwmParam.green_mode = 1;
    }
    else
    {
      /* special case: duty cycle is less than 1 timer period, switch pwm off */
      newPwmParam.ocrb       = 0;
      newPwmParam.tccra     &= ~(1 << COMxB1 | 0 << COMxB0);
      newPwmParam.green_mode = 2;
    }
  }

  // *******************
  // **** blue LED  ****
  // *******************
#if !defined(APP_THIRD_CHANNEL_ON_8BITS_TIMER)
  newPwmParam.blue_sum = 0;
  if (b == UINT16_MAX)
  {
    /* handling for special case of 100% duty cycle */
    newPwmParam.blue_m  = 0;
    newPwmParam.ocrc      = newPwmParam.icr;
    newPwmParam.tccra    |= (1 << COMxC1 | 0 << COMxC0);
    newPwmParam.blue_mode = 1;
  }
  else
  {
    newPwmParam.blue_m  = ((((newPwmParam.icr + 1L) * b) >> 8) & 0xFF);
    if (((newPwmParam.icr + 1L) * b) > UINT16_MAX)
    {
      newPwmParam.ocrc      = (uint16_t)(((newPwmParam.icr + 1L) * b) >> 16) - 1;
      newPwmParam.tccra    |= (1 << COMxC1 | 0 << COMxC0);
      newPwmParam.blue_mode = 1;
    }
    else
    {
      /* duty cycle is less than 1 timer period, switch pwm off */
      newPwmParam.ocrc      = 0;
      newPwmParam.tccra    &= ~(1 << COMxC1 | 0 << COMxC0);
      newPwmParam.blue_mode = 2;
    }
  }
#else // !defined(APP_THIRD_CHANNEL_ON_8BITS_TIMER)
  b >>= 8;
  b &= 0x00FF;
  if (b == UINT8_MAX)
  {
    /* handling for special case of 100% duty cycle */
    OCR0B  = b;
    TCCR0A |= (1 << COM0B1);
  }
  else
  {
    if (((0xFF + 1L) * b) > UINT8_MAX)
    {
      OCR0B = (uint8_t)(((0xFF + 1L) * b) >> 8) - 1;
      TCCR0A |= (1 << COM0B1);
    }
    else
    {
      /* duty cycle is less than 1 timer period, switch pwm off */
      OCR0B = 0;
      TCCR0A &= ~(1 << COM0B1);
    }
  }
#endif // !defined(APP_THIRD_CHANNEL_ON_8BITS_TIMER)
}

/*************************************************************************//**
\brief Calculate duty cycle values
******************************************************************************/
static inline void setPwmRegs(void)
{
  OCRxA  = newPwmParam.ocra;
  OCRxB  = newPwmParam.ocrb;
  OCRxC  = newPwmParam.ocrc;
  TCCRxA = newPwmParam.tccra;

  currPwmParam = newPwmParam;
}

/**************************************************************************//**
\brief Set duty cycle on timer channels. From 0 to FFFF.

\param[in]
  dccha - duty cycle on channel A
\param[in]
  dcchb - duty cycle on channel B
\param[in]
  dcchc - duty cycle on channel C
******************************************************************************/
void dithSetDutyCycles(uint16_t dccha, uint16_t dcchb, uint16_t dcchc)
{
  uint16_t icr;
  uint16_t tcnt;

  calcPwmRegs(dccha, dcchb, dcchc);
  cli();          // disable interrupts
  icr = ICRx;
  tcnt = TCNTx;

  if ((icr - tcnt) > OCR_UPD_CYCLES)  // check if enough cycles left to update the
  {
    // OCRx register within the same timer period
    setPwmRegs();
  }
  else
  {
    while (!(TIFRx & (1 << TOVx))) ;            // wait until timer overflow,
    // now are enough cycles left to update OCRx registers
    setPwmRegs();                               // within the same timer period
  }
  sei();          // enable interrupts
}

/**************************************************************************//**
\brief Set pwm frequency. There are two diapasons 200..1000Hz and 20..40kHz

\param[in]
  freq - duty cycle on channel A
******************************************************************************/
void dithSetFrequency(uint16_t freq)
{
  uint32_t mainFreq = HAL_ReadFreq();

  if ((freq > 199) && (freq < 1001))                     // use prescaler 8
  {
    newPwmParam.icr = mainFreq / (8 * freq) - 1;
    calcPwmRegs(red, green, blue);

    cli();
    TCCRxB = (1 << WGMx3 | 1 << WGMx2 |
              0 << CSx2 | 0 << CSx1 | 0 << CSx0);       // stop timer
    TCNTx = 0;
    ICRx  = newPwmParam.icr;

    setPwmRegs();

    TIFRx  = TIFRx;                                     //clear pending interrupts

    TCCRxB = (1 << WGMx3 | 1 << WGMx2 |
              1 << CSx1);                               //start timer, prescaler 8
    sei();

  }
  else if ((freq > 19999) && (freq < 40001))
  {
    newPwmParam.icr = mainFreq / freq - 1;
    calcPwmRegs(red, green, blue);

    cli();
    TCCRxB = (1 << WGMx3 | 1 << WGMx2 |
              0 << CSx2 | 0 << CSx1 | 0 << CSx0);       // stop timer
    TCNTx = 0;
    ICRx  = newPwmParam.icr;

    setPwmRegs();

    TIFRx  = TIFRx;                                     //clear pending interrupts

    TCCRxB = (1 << WGMx3 | 1 << WGMx2 |
              1 << CSx0);                               //start timer, prescaler 1
    sei();
  }
}

/**************************************************************************//**
\brief Start hardware timer which is used for pwm with dithering
******************************************************************************/
void dithStartTimer(void)
{
  uint32_t mainFreq = HAL_ReadFreq();

#if defined(APP_THIRD_CHANNEL_ON_8BITS_TIMER)
  if (!(TCCR0B & (1 << CS02 | 1 << CS01 | 1 << CS00)))
  {
      // configure pwm pin as output
      DDRG |= (1 << PING5);
      PORTG &= ~(1 << PING5);

      // configure timer1
      TCCR0A = (1 << COM0B1 |                     //   clear pin on Compare Match
                1 << WGM00  | 1 << WGM01);        //   set pin on overflow

      // we can set only one frequency for 8-bits timer
      // Fpwm = 7812.5 Hz when Fcpu = 16 MHz
      // Fpwm = 3906.25 Hz when Fcpu = 8 MHz
      // Fpwm = Fcpu / (N * 256).  N = 8
      TCCR0B = (1 << CS01);

      blue  = UINT16_MAX / 2;
  }
#endif

  if (!(TCCRxB & (1 << CSx2 | 1 << CSx1 | 1 << CSx0)))    // do nothing if timer is already running
  {
    //configure PWM port
#if !defined(APP_THIRD_CHANNEL_ON_8BITS_TIMER)
    PWMPORT |=  (1 << PWMCHA);
    PWMPORT &= ~(1 << PWMCHB | 1 << PWMCHC);
    PWMDDR  |=  (1 << PWMCHA | 1 << PWMCHB | 1 << PWMCHC);
#else // !defined(APP_THIRD_CHANNEL_ON_8BITS_TIMER)
    PWMPORT |=  (1 << PWMCHA);
    PWMPORT &= ~(1 << PWMCHB);
    PWMDDR  |=  (1 << PWMCHA | 1 << PWMCHB);
#endif // !defined(APP_THIRD_CHANNEL_ON_8BITS_TIMER)

    //configure PWM timer
    TCCRxA = (1 << COMxA1 | 1 << COMxA0 |           // channel A inverted pwm
              1 << COMxB1 | 0 << COMxB0 |           // channel B normal pwm
#if !defined(APP_THIRD_CHANNEL_ON_8BITS_TIMER)
              1 << COMxC1 | 0 << COMxC0 |           // channel C normal pwm
#endif // !defined(APP_THIRD_CHANNEL_ON_8BITS_TIMER)
              1 << WGMx1  | 0 << WGMx0);
    TCCRxB = (1 << WGMx3  | 1 << WGMx2);            // WGM=14. fast pwm mode, TOP=ICRx
    newPwmParam.icr = mainFreq / 20000ul - 1;;
    ICRx    = newPwmParam.icr;                       // 20kHz

    red   = UINT16_MAX / 2;                         // initial duty cycle is 50% for all 3 channels
    green = UINT16_MAX / 2;
#if !defined(APP_THIRD_CHANNEL_ON_8BITS_TIMER)
    blue  = UINT16_MAX / 2;
#endif // !defined(APP_THIRD_CHANNEL_ON_8BITS_TIMER)

    dithSetDutyCycles(red, green, blue);            // set OCR register values

    TIFRx  = TIFRx;                                 // clear pending interrupts
    TIMSKx |= (1 << TOIEx);                         // enable overflow interrupt

    TCCRxB = (1 << WGMx3 | 1 << WGMx2 | 1 << CSx0); // start timer, prescaler 1
  }
}

/**************************************************************************//**
\brief Stop hardware timer which is used for pwm with dithering
******************************************************************************/
void dithStopTimer(void)
{
  // stop 16-bits timer
  TCCRxB = (1 << WGMx3 | 1 << WGMx2);       // stop timer
  TCCRxA = (1 << WGMx1 | 0 << WGMx0);       // WGM=14, fast pwm mode, TOP=ICRx
  PWMPORT &= ~(1 << PWMCHA | 1 << PWMCHB);
  PWMDDR  &= ~(1 << PWMCHA | 1 << PWMCHB);
  TIMSKx = 0x00;

#if defined(APP_THIRD_CHANNEL_ON_8BITS_TIMER)
  // stop 8-bits timer
  TCCR0B = 0;
  TCCR0A = 0;
  PORTG &= ~(1 << PING5);
  DDRG  &= ~(1 << PING5);
#endif // defined(APP_THIRD_CHANNEL_ON_8BITS_TIMER)
}

#endif // APP_ZLL_DEVICE_TYPE >= APP_DEVICE_TYPE_COLOR_LIGHT

