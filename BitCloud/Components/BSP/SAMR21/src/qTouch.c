/************************************************************************//**
\file qTouch.c

\brief
  Enabling Touch with Wireless on SAMR21

\author
Atmel Corporation: http://www.atmel.com \n
Support email: avr@atmel.com

Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal
History:
18.01.12 Yogesh  - Created to Support SAMR21 with Touch(Multi Cap) Integrated 
******************************************************************************/
#if((QTOUCH_SUPPORT == 1) && (BSP_SUPPORT == BOARD_SAMR21_ZLLEK))
/******************************************************************************
                    Includes section
******************************************************************************/
#include <touch_api_SAMR21.h>
#include <atsamr21.h>
#include <bspTaskManager.h>
#include <qTouch.h>
#include <halAppClock.h>
#include <halInterrupt.h>
#include <atsamr21.h>

/******************************************************************************
                    Local variables
******************************************************************************/    
static uint8_t touch_button_previous_state[2];
static BSP_TouchEventHandler_t bspTouchEventHandler = NULL;
void configure_port_pins(void);
static void qTimer_Start(void);
static void qTimer_Start(void);
static void timer2Handler(void);

/******************************************************************************
                   Implementations section
******************************************************************************/
/**************************************************************************//**
\brief Init QTouch 
******************************************************************************/
void BSP_InitQTouch(BSP_TouchEventHandler_t handler)
{
  /* Initialize TCC1 ISR */
  HAL_InstallInterruptVector(TCC1_IRQn,timer2Handler);

  /**
  * Initialize QTouch - Enabled sensors.
  */
  touch_sensors_init();

  /* To handle touch system */
  bspTouchEventHandler = handler;

  /**
  * Initialize timer.
  */
  qTimer_Start();
}

/**************************************************************************//**
\brief BSP Touch handler
******************************************************************************/
void bspTouchHandler(void)
{ 
  uint8_t button1_state,temp_button1_state;
  uint8_t button2_state,temp_button2_state;
  uint8_t slider_state;
  uint8_t button,event = TOUCHKEY_NO_EVENT;
  uint8_t slider_threshold;
  
  /**
  * Start touch sensor measurement, if touch_time.time_to_measure_touch flag is set by timer.
  */
  touch_sensors_measure();
  
  /**
  * Update touch status once measurement complete flag is set.
  */
  if ((p_mutlcap_measure_data->measurement_done_touch == 1u))
  {
    p_mutlcap_measure_data->measurement_done_touch = 0u;
    
    /**
    * Get touch sensor states
    */
    
    button1_state = GET_MUTLCAP_SENSOR_STATE(0);
    button2_state = GET_MUTLCAP_SENSOR_STATE(1);
    slider_state = GET_MUTLCAP_SENSOR_STATE(2);
    
    temp_button1_state = (button1_state) ^ touch_button_previous_state[0];
    temp_button2_state = button2_state ^ touch_button_previous_state[1];
    
    if(temp_button1_state || temp_button2_state || slider_state)
    {
      
      if(button1_state)
      {
        button = BUTTON_L;
        event = TOUCHKEY_PRESSED_EVENT;
      }
      else if(temp_button1_state)
      {
       button = BUTTON_L;
       event = TOUCHKEY_RELEASED_EVENT;
      }
      if(button2_state)
      {
        button = BUTTON_R; 
        event = TOUCHKEY_PRESSED_EVENT;
      }
      else if (temp_button2_state)
      {
       button = BUTTON_R;
       event = TOUCHKEY_RELEASED_EVENT;
      }
      if(slider_state)
      {
        slider_threshold = GET_MUTLCAP_ROTOR_SLIDER_POSITION(0);
        button = SLIDER;
        event = TOUCHKEY_PRESSED_EVENT;
      }
      /* Store back the current sensor status */ 
      temp_button1_state = 0;
      temp_button2_state = 0;
      touch_button_previous_state[0] = button1_state;
      touch_button_previous_state[1] = button2_state;
      
      if (event != TOUCHKEY_NO_EVENT && event != TOUCHKEY_RELEASED_EVENT)
      {                 
        bspTouchEventHandler(event,button,slider_threshold);
      }
    }
  }
}

/**************************************************************************//**
\brief callback from Timer
******************************************************************************/
void qTimer_overflow_cb(void)
{
  touch_time.time_to_measure_touch = 1;
  touch_time.current_time_ms = touch_time.current_time_ms + touch_time.measurement_period_ms;
  bspPostTask(BSP_TOUCH);
}

/******************************************************************************
 Polling the Sync. flag for register access
 *****************************************************************************/
INLINE void timerSync(void)
{
  while (TCC1_32_SYNCBUSY_s.status);
}

/**************************************************************************//**
\brief Timer handler
******************************************************************************/
static void timer2Handler(void)
{
  if ((TCC1_32_INTFLAG_s.mc0) && (TCC1_32_INTENSET_s.mc0))
  {
    TCC1_32_INTFLAG_s.mc0 = 1;
    timerSync();
    qTimer_overflow_cb();
  }
}

/**************************************************************************//**
\brief QTimer to kick start
*****************************************************************************/
void qTimer_Start(void)
{
  PM_APBCMASK_s.tcc1 = 1;

  GCLK_CLKCTRL_s.id = 0x1A;  // enabling clock for TCC1
  GCLK_CLKCTRL_s.gen = 3;
  GCLK_CLKCTRL_s.clken = 1;

  TCC1_32_CTRLA_s.swrst = 1;
  while(TCC1_32_SYNCBUSY_s.swrst);
  
  TCC1_32_CTRLA_s.cpten0 = 0;
  
  TCC1_32_CTRLA_s.runstdby = 0;
  
  TCC1_32_CTRLA_s.prescsync = 0x01;
  
  TCC1_32_CTRLA_s.prescaler = 0x03;
  
  TCC1_32_WAVE = 0x01;
  
  TCC1_32_CTRLA_s.enable = 1;
  while(TCC1_32_SYNCBUSY_s.enable);
  
  TCC1_32_CTRLBCLR_s.oneshot = 1; // clearing one shot mode
  while(TCC1_32_SYNCBUSY_s.ctrlb);

  TCC1_32_CTRLBCLR_s.dir = 1;
  while(TCC1_32_SYNCBUSY_s.ctrlb);

  TCC1_32_COUNT = (uint16_t)0;
  while(TCC1_32_SYNCBUSY_s.count);
  
  TCC1_32_CC0_s.cc = (uint32_t)0x2710;////4e2 (8Mhz)
  while(TCC1_32_SYNCBUSY_s.cc0);
 
  NVIC_EnableIRQ(TCC1_IRQn);

  TCC1_32_INTENSET_s.mc0 = 1;
  timerSync();
}
/**************************************************************************//**
\brief Application Clock Stop
*****************************************************************************/
void StopAppClock(void)
{
  /* Disable the clock */
  TCC1_32_CTRLA_s.enable = 0;
  timerSync();
}
#endif
