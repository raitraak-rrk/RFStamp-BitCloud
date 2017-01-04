/**************************************************************************//**
\file  halClkCtrl.h

\brief Declarations of clock control hardware-dependent module.

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

#ifndef _HALCLKCTRL_H
#define _HALCLKCTRL_H

/******************************************************************************
                   Includes section
******************************************************************************/
#include <atsamr21.h>
#include <inttypes.h>
#include <sysTypes.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
// CPU runs at a different freq compared to the peripheral freq
// We are fixing the peripheral freq to 8MHz and are changing the CPU freq alone
// CPU freq is defined by HAL_xMHz and Peripheral freq is defined by F_CPU
// Exception : For HAL_4MHz the Peripherals are also run at 4Mhz
#if defined(HAL_4MHz)
  #define F_CPU  4000000ul
  #define F_PERI 4000000ul
#elif defined(HAL_8MHz)
  #define F_CPU  8000000ul
  #define F_PERI 8000000ul
#elif defined(HAL_16MHz)
  #define F_CPU  16000000ul
  #define F_PERI  8000000ul
#elif defined(HAL_24MHz)
  #define F_CPU  24000000ul
  #define F_PERI  8000000ul
#elif defined(HAL_32MHz)
  #define F_CPU  32000000ul
  #define F_PERI 8000000ul
#elif defined(HAL_48MHz)
  #define F_CPU  48000000ul
  #define F_PERI  8000000ul
#endif

#define ALL_PERIPHERIAL_INTERRUPT_DISABLE  0xFFFFFFFF

/**
 * \brief Available start-up times for the XOSC32K
 *
 * Available external 32KHz oscillator start-up times, as a number of external
 * clock cycles.
 */
enum system_xosc32k_startup {
  /** Wait 0 clock cycles until the clock source is considered stable */
  SYSTEM_XOSC32K_STARTUP_0,
  /** Wait 32 clock cycles until the clock source is considered stable */
  SYSTEM_XOSC32K_STARTUP_32,
  /** Wait 2048 clock cycles until the clock source is considered stable */
  SYSTEM_XOSC32K_STARTUP_2048,
  /** Wait 4096 clock cycles until the clock source is considered stable */
  SYSTEM_XOSC32K_STARTUP_4096,
  /** Wait 16384 clock cycles until the clock source is considered stable */
  SYSTEM_XOSC32K_STARTUP_16384,
  /** Wait 32768 clock cycles until the clock source is considered stable */
  SYSTEM_XOSC32K_STARTUP_32768,
  /** Wait 65536 clock cycles until the clock source is considered stable */
  SYSTEM_XOSC32K_STARTUP_65536,
  /** Wait 131072 clock cycles until the clock source is considered stable */
  SYSTEM_XOSC32K_STARTUP_131072,
};

/**
 * \brief Available generic clock generators
 *
 * Available GCLK generators IDs
 */
enum gclk_gen_id
{
  GCLK_GEN_0,
  GCLK_GEN_1,
  GCLK_GEN_2,
  GCLK_GEN_3,
  GCLK_GEN_4,
  GCLK_GEN_5,
  GCLK_GEN_6,
  GCLK_GEN_7,
  GCLK_GEN_8,
};

/**
 * \brief Available generic clock  peripherals 
 *
 * Available generic clock  peripherals to select the clock
 */
enum gclk_clk_sel_id
{
  GCLK_DFLL48M_REF,       // DFLL48M Reference
  GCLK_DPLL,              //  FDPLL96M input clock source for reference
  GCLK_DPLL_32K,          // FDPLL96M 32kHz clock for FDPLL96M internal lock timer
  GCLK_WDT,               //   WDT
  GCLK_RTC,               //  RTC
  GCLK_EIC,               //  EIC
  GCLK_USB,               //  USB
  GCLK_EVSYS_CHANNEL_0,   //  EVSYS_CHANNEL_0
  GCLK_EVSYS_CHANNEL_1,   //  EVSYS_CHANNEL_1
  GCLK_EVSYS_CHANNEL_2,   //  EVSYS_CHANNEL_2
  GCLK_EVSYS_CHANNEL_3,   //  EVSYS_CHANNEL_3
  GCLK_EVSYS_CHANNEL_4,   //  EVSYS_CHANNEL_4
  GCLK_EVSYS_CHANNEL_5,   //  EVSYS_CHANNEL_5
  GCLK_EVSYS_CHANNEL_6,   //  EVSYS_CHANNEL_6
  GCLK_EVSYS_CHANNEL_7,   //  EVSYS_CHANNEL_7
  GCLK_EVSYS_CHANNEL_8,   //  EVSYS_CHANNEL_8
  GCLK_EVSYS_CHANNEL_9,   //  EVSYS_CHANNEL_9
  GCLK_EVSYS_CHANNEL_10,  //  EVSYS_CHANNEL_10
  GCLK_EVSYS_CHANNEL_11,  //  EVSYS_CHANNEL_11
  GCLK_SERCOMx_SLOW,      // SERCOMx_SLOW
  GCLK_SERCOM0_CORE,      // SERCOM0_CORE
  GCLK_SERCOM1_CORE,      // SERCOM1_CORE
  GCLK_SERCOM2_CORE,      // SERCOM2_CORE
  GCLK_SERCOM3_CORE,      // SERCOM3_CORE
  GCLK_SERCOM4_CORE,      // SERCOM4_CORE
  GCLK_SERCOM5_CORE,      // SERCOM5_CORE
  GCLK_TCC0_GCLK_TCC1,    // TCC0,TCC1
  GCLK_TCC2_GCLK_TC3,     // TCC2,TC3
  GCLK_TC4_GCLK_TC5,      // TC4,TC5
  GCLK_ADC = 0x1E,        // ADC
  GCLK_AC_DIG,            // AC_DIG
  GCLK_AC_ANA,            // AC_ANA
  GCLK_PTC = 0x22,        // PTC
};

/**
 * \brief Available source clocks for GCLK
 *
 * Available source clocks IDs for GCLK generators to operate on
 */
enum gclk_source_sel
{
  XOSC,       // XOSC oscillator output
  GCLKIN,     // Generator input pad
  GCLKGEN1,   // Generic clock generator 1 output
  OSCULP32K,  // OSCULP32K oscillator output
  OSC32K,     // OSC32K oscillator output
  EXOSC32K,    // XOSC32K oscillator output
  OSC8M,      // OSC8M oscillator output
  DFLL48M,    // DFLL48M output
  FDPLL96M,   // FDPLL96M output
};

/******************************************************************************
                   Prototypes section
******************************************************************************/
/******************************************************************************
System clock.
Returns:
  system clock in Hz.
******************************************************************************/
uint32_t HAL_ReadFreq(void);

/**************************************************************************//**
\brief Initialize the 32KHz external RC oscillator.
******************************************************************************/
void halXOSC32KInit(void);

/******************************************************************************
 Polling the Sync. flag for register access
 Parameters:
   none
 Returns:
   none
 *****************************************************************************/
INLINE void halGclkSync(void)
{
  while (GCLK_STATUS & GCLK_STATUS_SYNCBUSY);
}

/******************************************************************************
 Polling the DFLL_RDY to check synchronization.
 Parameters:
   none
 Returns:
   none
 *****************************************************************************/
INLINE void halDfllReady(void)
{
  while (!(SYSCTRL_PCLKSR & SYSCTRL_PCLKSR_DFLLRDY));
}

/******************************************************************************
 Polling the DFLLLCKF to see if the DFLL fineLock has locked.
 Parameters:
   none
 Returns:
   none
 *****************************************************************************/
INLINE void halFineLock(void)
{
  while (!(SYSCTRL_PCLKSR & SYSCTRL_PCLKSR_DFLLLCKF));
}

/******************************************************************************
 Polling the DFLLLCKC to see if the DFLL coarseLock has locked.
 Parameters:
   none
 Returns:
   none
 *****************************************************************************/
INLINE void halCoarseLock(void)
{
  while (!(SYSCTRL_PCLKSR & SYSCTRL_PCLKSR_DFLLLCKC));
}

/******************************************************************************
 Polling the OSC8M to check if internal RC has set.
 Parameters:
   none
 Returns:
   none
 *****************************************************************************/
INLINE void halCheckIntRcReady(void)
{
  while( !(SYSCTRL_PCLKSR & SYSCTRL_PCLKSR_OSC8MRDY) );
}

/******************************************************************************
 Polling the XOSC32KRDY to check if XOSC32K has set.
 Parameters:
   none
 Returns:
   none
 *****************************************************************************/
INLINE void halCheckXOSC32KReady(void)
{
  /* Check the XOSC32K Clock source is ready to use */
  while(!((SYSCTRL_XOSC32K & SYSCTRL_PCLKSR_XOSC32KRDY) == \
                SYSCTRL_PCLKSR_XOSC32KRDY));
}

#endif /* _HALCLKCTRL_H */

// eof halClkCtrl.h
