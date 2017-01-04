/**************************************************************************//**
  \file   bspUid.c

  \brief  Implementation of UID interface.

  \author
      Atmel Corporation: http://www.atmel.com \n
      Support email: avr@atmel.com

    Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
    Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      07/04/14 karthik.p_u - Created
      14/05/14 karthik.p_u - Modified
 ******************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/

#if BSP_SUPPORT != BOARD_FAKE

#include <systypes.h>

#if BSP_SUPPORT == BOARD_SAMR21_XPRO
/******************************************************************************
                   Includes section
******************************************************************************/
#include <gpio.h>
#include <bspI2c.h>
/******************************************************************************
                   Define(s) section
******************************************************************************/
#define SLAVE_ADDRESS             0x28
#define SLAVE_TOKEN_DATA_LENGTH   0x02
#define IEEE_ADDRESS_LENGTH       0x08
/******************************************************************************
                   Types section
******************************************************************************/

/******************************************************************************
                   Global variables section
******************************************************************************/
/* slave address and kit token */
static uint8_t writeBuffer[SLAVE_TOKEN_DATA_LENGTH] = {0x51, 0xD2};
static HAL_I2cDescriptor_t appI2cDescriptor;

/*******************************************************************************
                             Prototypes section
*******************************************************************************/
static void userCallback(bool result);
#endif // if BSP_SUPPORT == BOARD_SAMR21_XPRO

/******************************************************************************
                   Implementations section
******************************************************************************/
/*******************************************************************************
                             Implementation section
*******************************************************************************/
/******************************************************************************
 Reads the MAC address from the EDBG through i2c 
 Parameters:
   id - UID buffer pointer.
 Returns:
   0 - if unique ID has been found without error;
  -1 - if there are some erros during UID discovery.
******************************************************************************/
int BSP_ReadUid(uint64_t *id)
{
#if BSP_SUPPORT == BOARD_SAMR21_XPRO
  int8_t status;

  appI2cDescriptor.tty             = TWI_CHANNEL_0;
  appI2cDescriptor.clockRate       = I2C_CLOCK_RATE_100;  
  appI2cDescriptor.lengthAddr      = 1;
  appI2cDescriptor.f               = userCallback;

  appI2cDescriptor.internalAddr = SLAVE_ADDRESS;
  appI2cDescriptor.data = writeBuffer;
  appI2cDescriptor.length = SLAVE_TOKEN_DATA_LENGTH;
  
  BSP_BoardSpecificI2cPinInit(appI2cDescriptor.tty);

  if (!(status = HAL_OpenI2cPacket(&appI2cDescriptor)))
  {
    if (!(status = HAL_WriteI2cPacket(&appI2cDescriptor)))
    {
      appI2cDescriptor.data = (uint8_t*)id;
      appI2cDescriptor.length = IEEE_ADDRESS_LENGTH;
  
      if (!(status = HAL_ReadI2cPacket(&appI2cDescriptor)))
      {
        HAL_CloseI2cPacket(&appI2cDescriptor);
        return status;
      }
    }
  }
  return status;
#elif (BSP_SUPPORT == BOARD_SAMR21B18_MZ210PA_MODULE) || (BSP_SUPPORT == BOARD_SAMR21_ZLLEK) || (BSP_SUPPORT == BOARD_SAMR21_CUSTOM)\
      || (BSP_SUPPORT == BOARD_SAMR21G18_MR210UA_MODULE)
  (void)id;
  return 0;
#endif
}
  
#if BSP_SUPPORT == BOARD_SAMR21_XPRO
/**************************************************************************//**
\brief User Call back
****************************************************************************/
static void userCallback(bool result)
{
  /* user specific implementation can be added */
  (void)result;
}
#endif // if BSP_SUPPORT == BOARD_SAMR21_XPRO

#endif // if BSP_SUPPORT != BOARD_FAKE
// eof bspUid.c