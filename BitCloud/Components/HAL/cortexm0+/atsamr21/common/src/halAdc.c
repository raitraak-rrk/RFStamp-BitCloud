/**************************************************************************//**
\file  halAdc.c

\brief Implementation of hardware depended ADC interface.

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal
  History:
    11/06/14 P. Mahendran - Created
*******************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/
#if defined(HAL_USE_ADC)
/******************************************************************************
                   Includes section
******************************************************************************/
#include <adc.h>
#include <halTaskManager.h>
#include <halInterrupt.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
#define ADC_INPUTCTRL_MUXNEG_GND    (0x18u << 8)
#define AVGCTRL_SAMPLENUM_256       (0x08u)
#define AVGCTRL_DIVIDE_RESULT_16    (4u)
#define ADC_FUSES_LINEARITY_0_ADDR  (NVMCTRL_OTP4)
#define ADC_FUSES_BIASCAL_ADDR      (NVMCTRL_OTP4 + 4)
#define ADC_FUSES_BIASCAL_Pos       (3u)
#define ADC_FUSES_LINEARITY_0_Pos   (27u)
/* ADC is running at 8Mhz */
#define ADC_PRESCALER             (0x00u)
/******************************************************************************
                   Prototypes section
******************************************************************************/
/******************************************************************************
                   External variables section
******************************************************************************/
extern HAL_AdcDescriptor_t *adcDesc;

/******************************************************************************
                   Global variables section
******************************************************************************/
/******************************************************************************
                   Implementations section
******************************************************************************/
/**************************************************************************//**
\brief ADC conversion complete interrupt handler.
******************************************************************************/
void adcHandler(void)
{
  uint8_t intFlags = ADC_INTFLAG;

  if (intFlags & ADC_INTFLAG_RESRDY)
  {
      *((uint16_t *)adcDesc->bufferPointer) = ADC_RESULT;
      halPostTask(HAL_TASK_ADC);
  }
}

/**************************************************************************//**
\brief Set up parametres to ADC.
******************************************************************************/
void halOpenAdc(void)
{
  /* Enable the clock of ADC */
  PM_APBCMASK |= PM_APBCMASK_ADC;

  /* Configure the generic clk settings */
  GCLK_CLKCTRL_s.id = 0x1E;  // enabling clock for ADC
  GCLK_CLKCTRL_s.gen = 3;
  GCLK_CLKCTRL_s.clken = 1;

  /* Initialization and reset adc  */
  ADC_CTRLA = ADC_CTRLA_SWRST;
  while(ADC_STATUS_s.syncbusy);

  /* Calibration Settings */
  ADC_CALIB = ADC_CALIB_BIAS_CAL((*(uint32_t *)ADC_FUSES_BIASCAL_ADDR >> ADC_FUSES_BIASCAL_Pos)) | 
     ADC_CALIB_LINEARITY_CAL((*(uint64_t *)ADC_FUSES_LINEARITY_0_ADDR >> ADC_FUSES_LINEARITY_0_Pos));

  /* Set Conversion Result Resolution */
  ADC_CTRLB_s.ressel = adcDesc->resolution;

  ADC_CTRLB_s.prescaler = ADC_PRESCALER;

  ADC_CTRLB_s.corren = adcDesc->corrEn;

  ADC_AVGCTRL_s.samplenum = adcDesc->sampleNum;
  ADC_AVGCTRL_s.adjres = adcDesc->adjRes;

  ADC_SAMPCTRL_s.samplen = adcDesc->sampTime;

  /* Set Conversion Result Resolution */
  ADC_REFCTRL = adcDesc->voltageReference;

  /* disable all adc interrupt */
  ADC_INTENCLR =  ADC_INTENCLR_RESRDY | ADC_INTENCLR_OVERRUN |
  ADC_INTENCLR_WINMON | ADC_INTENCLR_SYNCRDY;

  /* Install Interrupt vector */
  HAL_InstallInterruptVector(ADC_IRQn, adcHandler);

  /* Clear & disable ADC interrupt on NVIC */
  NVIC_DisableIRQ(ADC_IRQn);
  NVIC_ClearPendingIRQ(ADC_IRQn);

  /* set priority & enable ADC interrupt on NVIC */
  NVIC_EnableIRQ(ADC_IRQn);
  ADC_INTENSET =  ADC_INTENCLR_RESRDY;

  /* enable ADC */
  ADC_CTRLA |= ADC_CTRLA_ENABLE;
}

/**************************************************************************//**
\brief starts convertion on the ADC channel.
\param[in]
  channel - channel number.
******************************************************************************/
void halStartAdc(HAL_AdcChannelNumber_t channel)
{
  /* Input Control Selection */
  ADC_INPUTCTRL = ADC_INPUTCTRL_MUXNEG_GND | channel;

  ADC_SWTRIG |= ADC_SWTRIG_START;
}

/**************************************************************************//**
\brief Closes the ADC.
******************************************************************************/
void halCloseAdc(void)
{
  ADC_CTRLA &= ~ADC_CTRLA_ENABLE;
}
#endif // defined(HAL_USE_ADC)

// eof halAdc.c
