/**************************************************************************//**
\file  i2cPacket.c

\brief Provides the functionality for the writing and the reading \n
       of packets through the TWI.

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2012, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal
  History:
    05/02/14 Viswanadham Kotla - Created
*******************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/
#if defined(HAL_USE_TWI)
/******************************************************************************
                   Includes section
******************************************************************************/
#include <i2cPacket.h>
#include <halDbg.h>
#include <gpio.h>

/******************************************************************************
                   Global variables section
******************************************************************************/
HAL_I2cDescriptor_t *halI2cDesc;

I2cChannelInfo_t twiChannel[1];
/******************************************************************************
                   Implementations section
******************************************************************************/
/**************************************************************************//**
\brief Opens twi resource. Clock rate and tty must be configured. \n

\ingroup hal_i2c

\param[in]
  descriptor - pointer to HAL_I2cDescriptor_t structure

\return
  -1 - resource was already open or pointer is NULL. \n
   0 - success.
******************************************************************************/
int HAL_OpenI2cPacket(HAL_I2cDescriptor_t *descriptor)
{
  if (NULL == descriptor)
    return -1;
  if (halI2cDesc)
    return -1;

  halI2cDesc = descriptor;  
  halInitI2c(halI2cDesc->clockRate);
  halI2cDesc->service.state = I2C_PACKET_IDLE;
  halI2cDesc->service.index = 0;
  return 0;
}

/**************************************************************************//**
\brief Closes resource.

\ingroup hal_i2c

\param[in]
  descriptor - pointer to HAL_I2cDescriptor_t structure

\return
  -1 - resource was not open. \n
   0 - success.
******************************************************************************/
int HAL_CloseI2cPacket(HAL_I2cDescriptor_t *descriptor)
{
  if (!descriptor)
    return -1;
  if (descriptor != halI2cDesc)
    return -1;

  halI2cDesc = NULL;

  if (I2C_PACKET_CLOSE != descriptor->service.state)
  {
    descriptor->service.state = I2C_PACKET_CLOSE;
    return 0;
  }
  return -1;
}

/**************************************************************************//**
\brief Writes a series of bytes out to the TWI bus. Operation result will be
sent to the callback function of the HAL_I2cDescriptor_t structure.

\ingroup hal_i2c

\param[in]
  descriptor - pointer to HAL_I2cDescriptor_t structure

\return
   0 - the bus is free and the request is accepted. \n
  -1 - otherwise.
******************************************************************************/
int HAL_WriteI2cPacket(HAL_I2cDescriptor_t *descriptor)
{
  HalI2cPacketService_t *service;

  if (!descriptor)
    return -1;
  if (halI2cDesc != descriptor)
    return -1;
  if (!halI2cDesc->data && !halI2cDesc->length)
    return -1;

  service = &halI2cDesc->service;
  if (I2C_PACKET_IDLE != service->state)
    return -1;

  service->index = 0;
  service->state = I2C_PACKET_WRITE_ADDRESS;

  halSendStartWriteI2c();
  return 0;
}

/**************************************************************************//**
\brief Reads the series of bytes from the TWI bus. Operation result will be
sent to the callback function of the HAL_I2cDescriptor_t structure.

\ingroup hal_i2c

\param[in]
  descriptor - pointer to HAL_I2cDescriptor_t structure

\return
  0 - the bus is free and the request is accepted. \n
 -1 - otherwise.
******************************************************************************/
int HAL_ReadI2cPacket(HAL_I2cDescriptor_t *descriptor)
{
  HalI2cPacketService_t *service;

  if (!descriptor)
    return -1;
  if (halI2cDesc != descriptor)
    return -1;
  if (!halI2cDesc->data && !halI2cDesc->length)
    return -1;

  service = &halI2cDesc->service;
  if (I2C_PACKET_IDLE != service->state)
    return -1;

  service->index = 0;
  service->state = I2C_PACKET_READ_ADDRESS;

  halSendStartReadI2c();
  return 0;
}

/******************************************************************************
It's callback function for task manager
Parameters:
  none
Returns:
  none
******************************************************************************/
void halSig2WireSerialHandler(void)
{
  I2cPacketStates_t currestState;
  HalI2cPacketService_t *service = &halI2cDesc->service;

  currestState = service->state;
  service->state = I2C_PACKET_IDLE;

  sysAssert(halI2cDesc->f, I2C_NULLCALLBACK_0);

  if (I2C_PACKET_TRANSAC_SUCCESS == currestState)
  {
    halI2cDesc->f(true);
  }
  else
  {
    halI2cDesc->f(false);
  }
}
#endif // defined(HAL_USE_TWI)

// eof i2cPacket.c