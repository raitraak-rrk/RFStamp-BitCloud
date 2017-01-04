/**************************************************************************//**
  \file   bspAntDiversity.c

  \brief  Implementation of board specific antenna diversity pin configuration

  \author
      Atmel Corporation: http://www.atmel.com \n
      Support email: avr@atmel.com

    Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
    Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      28/08/15 Viswanadham kotla - Created
 ******************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/
#if BSP_SUPPORT != BOARD_FAKE
/******************************************************************************
                   Includes section
******************************************************************************/
#include <sysTypes.h>
#include <gpio.h>
#include <configServer.h>

/******************************************************************************
                    Defines section
******************************************************************************/
#ifndef BSP_ANTENNA_SELECTED
#define BSP_ANTENNA_SELECTED           (1)
#endif

/******************************************************************************
                   Implementations section
******************************************************************************/
    
/**************************************************************************//**
\brief Checks whether antenna diversity actions required based on the board 
       and antenna divesity enabled or not
******************************************************************************/
bool BSP_IsAntDiversityActionsRequired(void)
{
#ifdef _HAL_ANT_DIVERSITY_
  return true;
#else /* #ifdef _HAL_ANT_DIVERSITY_ */
#if (BSP_SUPPORT == BOARD_SAMR21_ZLLEK) || (BSP_SUPPORT == BOARD_SAMR21G18_MR210UA_MODULE)
  return true;
#else /* (BSP_SUPPORT == BOARD_SAMR21_ZLLEK) || (BSP_SUPPORT == BOARD_SAMR21G18_MR210UA_MODULE) */
  return false;
#endif  
#endif /* _HAL_ANT_DIVERSITY_ */
}

/**************************************************************************//**
\brief Configure the antenna diversity pins based on the board
******************************************************************************/
bool BSP_BoardSpecificAntennaDivesityPinInit(uint8_t* antennaSelected)
{
  if (BSP_IsAntDiversityActionsRequired())
  {
    PinConfig_t rfctrl1,rfctrl2;

    /* Configure PIOs for RFCTRL */
    rfctrl1.portNum = PORT_A;
    rfctrl1.pinNum = 9;
    rfctrl1.functionConfig = 5;
    GPIO_make_out(&rfctrl1);
    GPIO_pinfunc_config(&rfctrl1);

    rfctrl2.portNum = PORT_A;
    rfctrl2.pinNum = 12;
    rfctrl2.functionConfig = 5;
    GPIO_make_out(&rfctrl2);
    GPIO_pinfunc_config(&rfctrl2);

#ifndef _HAL_ANT_DIVERSITY_
    *antennaSelected = BSP_ANTENNA_SELECTED;
#else
    (void)antennaSelected;
#endif  /* ifndef _HAL_ANT_DIVERSITY_ */
    return true;
  }
  return false;
}

#endif // if BSP_SUPPORT != BOARD_FAKE
// eof bspAntDiversity.c
