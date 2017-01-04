/**************************************************************************//**
\file   dithTimerIsr.h

\brief  Hardware timer interface declaration.

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal
  History:
    19/02/13 A. Khromykh - Created
*******************************************************************************/
#ifndef TIMER_ISR_H
#define TIMER_ISR_H

/******************************************************************************
                   Includes section
******************************************************************************/
#include <sysTypes.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
#define OCRxA                   OCR3A
#define OCRxB                   OCR3B
#define OCRxC                   OCR3C
#define ICRx                    ICR3
#define TCNTx                   TCNT3
#define TCCRxA                  TCCR3A
#define TCCRxB                  TCCR3B
#define TIMSKx                  TIMSK3
#define TOIEx                   TOIE3
#define OCIExA                  OCIE3A
#define OCIExB                  OCIE3B
#define OCIExC                  OCIE3C
#define TIFRx                   TIFR3
#define TOVx                    TOV3
#define OCFxA                   OCF3A
#define OCFxB                   OCF3B
#define OCFxC                   OCF3C
#define TIMERx_OVF_vect         TIMER3_OVF_vect
#define WGMx0                   WGM30
#define WGMx1                   WGM31
#define WGMx2                   WGM32
#define WGMx3                   WGM33
#define COMxA1                  COM3A1
#define COMxA0                  COM3A0
#define COMxB1                  COM3B1
#define COMxB0                  COM3B0
#define COMxC1                  COM3C1
#define COMxC0                  COM3C0
#define CSx0                    CS30
#define CSx1                    CS31
#define CSx2                    CS32
#define ICNCx                   ICNC3 //noise canceler
#define ICESx                   ICES3 //input capture edge select
#define ICIEx                   ICIE3 //input capture enable
#define PWMDDR                  DDRE
#define PWMPORT                 PORTE
#define PWMCHA                  PE3
#define PWMCHB                  PE4
#define PWMCHC                  PE5

/******************************************************************************
                   Types section
******************************************************************************/
typedef struct _PwmParams_t
{
    uint8_t  tccra;
    uint16_t icr;

    uint16_t ocra;
    uint16_t ocrb;
    uint16_t ocrc;

    uint16_t red_m;
    uint16_t green_m;
    uint16_t blue_m;

    uint16_t red_sum;
    uint16_t green_sum;
    uint16_t blue_sum;

    uint8_t  red_mode : 2;
    uint8_t  green_mode : 2;
    uint8_t  blue_mode : 2;

    uint8_t  red_ena_dith : 1;
    uint8_t  green_ena_dith : 1;
    uint8_t  blue_ena_dith : 1;
} PwmParams_t;

#endif // TIMER_ISR_H
