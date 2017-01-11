/**************************************************************************//**
\file  bspTaskManager.h

\brief Declarations of enums and functions of BSP task manager.

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal
  History:
    07/04/14 karthik.p_u - Created
*******************************************************************************/

#ifndef _BSPTASKMANAGER_H
#define _BSPTASKMANAGER_H

/******************************************************************************
                   Includes section
******************************************************************************/
#include <inttypes.h>
#include <sysTaskManager.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
enum
{
  BSP_BUTTONS  =     (uint8_t)1 << 0,
  BSP_TEMPERATURE  = (uint8_t)1 << 1,
  BSP_LIGHT        = (uint8_t)1 << 2,
  BSP_BATTERY      = (uint8_t)1 << 3,
  BSP_JOYSTICK     = (uint8_t)1 << 4,
#if((QTOUCH_SUPPORT == 1) && ((BSP_SUPPORT == BOARD_SAMR21_ZLLEK) ||\
    (BSP_SUPPORT == BOARD_RFRINGQM) || (BSP_SUPPORT == BOARD_RFRINGQT)))
  BSP_TOUCH        = (uint8_t)1 << 5,
#endif
#if (BSP_SUPPORT == BOARD_RFRINGQT) || (BSP_SUPPORT == BOARD_RFRINGQM)
  BSP_RGBLED       = (uint8_t)1 << 6,
#endif
};
/******************************************************************************
                   External global variables section
******************************************************************************/
extern volatile uint8_t bspTaskFlags;

/******************************************************************************
                   Inline static functions section
******************************************************************************/
/***************************************************************************//**
\brief posting bsp task for task manager.
\param[in]
  flag - task number
*******************************************************************************/
INLINE void bspPostTask(uint8_t flag)
{
  bspTaskFlags |= flag;
  SYS_PostTask(BSP_TASK_ID);
}

#endif /* _BSPTASKMANAGER_H */
// eof bspTaskManager.h
