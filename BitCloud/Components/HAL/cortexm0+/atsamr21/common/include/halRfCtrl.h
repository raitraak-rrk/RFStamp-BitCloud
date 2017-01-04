/**************************************************************************//**
\file  halRfCtrl.h

\brief Types and constants declaration for IEEE802.15.4 PHY implementation.

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

#ifndef _HALRFCTRL_H
#define _HALRFCTRL_H

/******************************************************************************
                        Includes section.
******************************************************************************/
#include <sysTypes.h>

/******************************************************************************
                    Types section
******************************************************************************/
typedef enum
{
  HAL_FREQ_NOCLK,
  HAL_FREQ_1MHZ,
  HAL_FREQ_2MHZ,
  HAL_FREQ_4MHZ,
  HAL_FREQ_8MHZ,
  HAL_FREQ_16MHZ
} HalSysFreq_t;

/******************************************************************************
                   Define(s) section
******************************************************************************/

/******************************************************************************
                    Prototypes section
******************************************************************************/
/******************************************************************************
  Delay in us
******************************************************************************/
void HAL_Delay(uint8_t us);

/******************************************************************************
  Clear the irq.
******************************************************************************/
void HAL_ClearRfIrqFlag(void);

/******************************************************************************
  Enables the irq.
******************************************************************************/
void HAL_EnableRfIrq(void);

/******************************************************************************
  Disables the irq.
******************************************************************************/
uint8_t HAL_DisableRfIrq(void);

/******************************************************************************
  Sets SLP_TR pin to 1.
******************************************************************************/
void HAL_SetRfSlpTr(void);

/******************************************************************************
  Clears SLP_TR pin to 0.
******************************************************************************/
void HAL_ClearRfSlpTr(void);

/******************************************************************************
  Makes SLP_TR pin as input.
******************************************************************************/
void HAL_MakeInRfSlpTr(void);

/******************************************************************************
  Makes SLP_TR pin as input.
******************************************************************************/
void HAL_MakeOutRfSlpTr(void);

/******************************************************************************
  Sets RST_TR pin to 1.
******************************************************************************/
void HAL_SetRfRst(void);

/******************************************************************************
  Clears RST_TR pin to 1.
******************************************************************************/
void HAL_ClearRfRst(void);

/******************************************************************************
  Init pins that controls RF chip.
******************************************************************************/
void HAL_InitRfPins(void);

/******************************************************************************
  Initialization rf irq.
******************************************************************************/
void HAL_InitRfIrq(void);

/******************************************************************************
  Returns current frequency code.
******************************************************************************/
HalSysFreq_t HAL_GetRfFreq(void);

/**************************************************************************//**
  \brief Enables RX TX indicator for radio if that is supported.
******************************************************************************/
void HAL_EnableRxTxSwitcher(void);

/**************************************************************************//**
  \brief Enables Antenna diversity option for radio if that is supported.
******************************************************************************/
void HAL_InitAntennaDiversity(void);

/**************************************************************************//**
  \brief Enables Antenna diversity in RX mode for radio if that is supported.
******************************************************************************/
void HAL_EnableRxAntennaDiversity(void);

/**************************************************************************//**
  \brief Enables Antenna diversity in TX mode for radio if that is supported.
******************************************************************************/
void HAL_EnableTxAntennaDiversity(void);

/**************************************************************************//**
  \brief To switch from the antenna used for last transmission to the other
******************************************************************************/
void HAL_SwitchLastTxAntenna(void);

/**************************************************************************//**
  \brief Wait for when radio will be waked up.
******************************************************************************/
void halWaitRadio(void);

/**************************************************************************//**
  \brief Check whether transceiver in TRX_OFF state
******************************************************************************/
bool halRadioInTrxOff(void);

/******************************************************************************
  \brief Init pins for CLKM
******************************************************************************/
void HAL_InitRfClkmPins(void);

/******************************************************************************
  Dissable pins for CLKM
******************************************************************************/
void HAL_DissableRfClkmPins(void);

/**************************************************************************//**
  \brief EIC IRQ handler to handle RF IRQ 
******************************************************************************/

void EIC_Handler(void);

/**************************************************************************//**
  \brief Pin configuration forAntenna diversity feature
******************************************************************************/
bool BSP_BoardSpecificAntennaDivesityPinInit(uint8_t* antennaSelected);

/**************************************************************************//**
\brief Checks whether Ant Div actions reuired based on the board and ant div enabled or not
******************************************************************************/
bool BSP_IsAntDiversityActionsRequired(void);

#endif /* _HALRFCTRL_H */

// eof halRfCtrl.h
