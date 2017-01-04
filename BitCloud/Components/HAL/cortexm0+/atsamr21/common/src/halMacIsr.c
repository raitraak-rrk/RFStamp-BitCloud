/**************************************************************************//**
\file  halMacIsr.c

\brief   mac timer interface implementation.

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2012, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal
  History:
    07/04/14 karthik.p_u - Created
******************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/

/******************************************************************************
                        Includes section.
******************************************************************************/
#include <halClkCtrl.h>
#include <halRfCtrl.h>
#include <halRfPio.h>
#include <halMacIsr.h>
#include <core_cm0plus.h>
/******************************************************************************
                   Define(s) section
******************************************************************************/
#define HAL_NUMBER_OF_TICKS_IN_USEC    (F_PERI/TIMER_FREQUENCY_PRESCALER/1000000ul)

/******************************************************************************
                   Global variables section
******************************************************************************/
RTimerDescr_t __rtimer;

/******************************************************************************
                    Prototypes section
******************************************************************************/
/******************************************************************************
  Initializes Rtimer and RF ext. interrupts.
******************************************************************************/
void HAL_InitMacIsr(void);

/******************************************************************************
  MAC timer handler.
******************************************************************************/
void halMacTimerHandler(void);

/******************************************************************************
  Redirect interrupt event depending on the TrxState.
  Parameters: none.
  Returns: none.
******************************************************************************/
void phyDispatcheRTimerEvent(void);

/******************************************************************************
                    Implementations section
******************************************************************************/
/******************************************************************************
 Polling the Sync. flag for register access
 Parameters:
   none
 Returns:
   none
 *****************************************************************************/
INLINE void halRtimerSync(void)
{
  while (TC4_16_STATUS_s.syncbusy);
}
/******************************************************************************
  Initializes Rtimer.
******************************************************************************/
void HAL_InitMacIsr(void)
{
  __rtimer.mode = HAL_RTIMER_STOPPED_MODE;

  GCLK_CLKCTRL_s.id = 0x1c;  // enabling clock for TC4 and TC5
  GCLK_CLKCTRL_s.gen = 3;
  GCLK_CLKCTRL_s.clken = 1;

  HAL_InitRfIrq();
}

/******************************************************************************
  Starts RTimer. Function should be invoked in critical section.
  Parameters:
    mode    - RTimer mode.
    period  - RTimer period.
******************************************************************************/
bool HAL_StartRtimer(HAL_RTimerMode_t mode, uint16_t period)
{
  if (HAL_RTIMER_STOPPED_MODE != __rtimer.mode)
    return false;

  __rtimer.period    = (uint16_t)((uint32_t)period * HAL_NUMBER_OF_TICKS_IN_USEC);
  __rtimer.mode      = mode;

  PM_APBCMASK_s.tc4 = 1;

  TC4_16_CTRLA_s.mode = 0x00;  //16 bit mode
  halRtimerSync();
  TC4_16_CTRLA_s.wavegen = 1;  // match frequency
  halRtimerSync();
  TC4_16_CTRLA_s.prescaler = 3;  // Prescaler DIV2
  halRtimerSync();
  TC4_16_CTRLA_s.prescsync = 0x01; // PRESC
  halRtimerSync();
  if (HAL_RTIMER_ONE_SHOT_MODE == mode)
  {
    TC4_16_CTRLBSET_s.oneshot = 1; // setting one shot mode
    halRtimerSync();
  }
  else
  {
    TC4_16_CTRLBSET_s.oneshot = 0; // clearing one shot mode
    halRtimerSync();
  }

  TC4_16_CTRLBCLR_s.dir = 1;
  halRtimerSync();

  TC4_16_COUNT = (uint16_t)0;
  halRtimerSync();
  TC4_16_CC0 = (uint16_t)__rtimer.period;
  halRtimerSync();

  TC4_16_CTRLA_s.enable = 1; //after configuration enable the timer
  halRtimerSync();

  TC4_16_INTENSET_s.mc0 = 1; // enabling interrupt MC0
  halRtimerSync();

  NVIC_EnableIRQ(TC4_IRQn);

  return true;
}

/******************************************************************************
  Stops RTimer. Function should be invoked in critical section.
******************************************************************************/
void HAL_StopRtimer(void)
{
  TC4_16_CTRLA_s.enable = 0; // disabling interrupt
  halRtimerSync();

  TC4_16_INTENCLR_s.mc0 = 1; // disabling interrupt MC0
  halRtimerSync();

  NVIC_DisableIRQ(TC4_IRQn);

  __rtimer.mode = HAL_RTIMER_STOPPED_MODE;
}

/******************************************************************************
  Returns status of RTimer
******************************************************************************/
bool HAL_IsRtimerStopped(void)
{
  if (__rtimer.mode == HAL_RTIMER_STOPPED_MODE)
    return true;
  else
    return false;
}

/******************************************************************************
  MAC timer handler.
******************************************************************************/
void halMacTimerHandler(void)
{
  if (TC4_16_INTFLAG_s.mc0 & TC4_16_INTENSET_s.mc0)
  {
    TC4_16_INTFLAG_s.mc0 = 1;
    if (HAL_RTIMER_ONE_SHOT_MODE == __rtimer.mode)
    {
      /* disable interrupt from CC1 */
      TC4_16_CTRLA_s.enable = 0;
      halRtimerSync();

      TC4_16_INTENCLR_s.mc0 = 1; // disabling interrupt MC0
      halRtimerSync();

      NVIC_DisableIRQ(TC4_IRQn);
      __rtimer.mode = HAL_RTIMER_STOPPED_MODE;
    }

    phyDispatcheRTimerEvent();
  }
}
// eof halMacIsr.c
