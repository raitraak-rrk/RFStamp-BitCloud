/**************************************************************************//**
  \file  i2cPacket.h

  \brief The header file describes the i2cPacket interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    5/12/07 A. Khromykh - Created
 ******************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/

#ifndef _I2CPACKET_H
#define _I2CPACKET_H

/******************************************************************************
                   Includes section
******************************************************************************/
// \cond
#include <halTwi.h>
// \endcond

/******************************************************************************
                   Define(s) section
******************************************************************************/

/******************************************************************************
                   Types section
******************************************************************************/

/**************************************************************************//**
\brief I2C (TWI) access control structure
******************************************************************************/
typedef struct
{
  /** \brief TWI service field - contains variables for HAL TWI module's
  internal needs */
  HalI2cPacketService_t service;
  /** \brief tty - TWI_CHANNEL_n to be used. "n" range depends on the platform.
      See a halTwi.h platform-specific file for more details. */
  I2cChannel_t tty;
  /** \brief TWI clock rate. See a halTwi.h platform-specific file
      for more details. */
  I2cClockRate_t clockRate;
  /** \brief Slave's address */
  uint8_t id;
  /** \brief The number of bytes to be written to the bus */
  uint16_t length;
  /** \brief Pointer to data */
  uint8_t *data;
  /** \brief Specifies the size of address contained in the internalAddr field:
              HAL_NO_INTERNAL_ADDRESS - byte command protocol (not the combined mode) \n
              HAL_ONE_BYTE_SIZE       \n
              HAL_TWO_BYTE_SIZE       \n
              HAL_THREE_BYTE_SIZE */
  I2cInternalAddrSize_t lengthAddr;
  /** \brief Contains address from device's internal address space */
  uint32_t internalAddr;
  /** \brief Callback function. Shall not be set to NULL. */
  void (* f)(bool result);
} HAL_I2cDescriptor_t;

/******************************************************************************
                   Prototypes section
******************************************************************************/
/**************************************************************************//**
\brief Opens TWI resource. Clock rate and tty must be configured. \n

\ingroup hal_i2c

\param[in]
  descriptor - pointer to an instance of HAL_I2cDescriptor_t type

\return
  -1 - the resource is already opened or the pointer is NULL \n
   0 - success
******************************************************************************/
int HAL_OpenI2cPacket(HAL_I2cDescriptor_t *descriptor);

/**************************************************************************//**
\brief Closes the TWI resource

\ingroup hal_i2c

\param[in]
  descriptor - pointer to an instance of HAL_I2cDescriptor_t type

\return
  -1 - resource has not been opened \n
   0 - success
******************************************************************************/
int HAL_CloseI2cPacket(HAL_I2cDescriptor_t *descriptor);

/**************************************************************************//**
\brief Writes a series of bytes out to the TWI bus. Operation result will be
sent to the callback function specified int the HAL_I2cDescriptor_t instance.

\ingroup hal_i2c

\param[in]
  descriptor - pointer to an instance of HAL_I2cDescriptor_t type

\return
   0 - the request has been accepted and the bus is free \n
  -1 - otherwise
******************************************************************************/
int HAL_WriteI2cPacket(HAL_I2cDescriptor_t *descriptor);

/**************************************************************************//**
\brief Reads the series of bytes from the TWI bus. Operation result will be
sent to the callback function specified int the HAL_I2cDescriptor_t instance.

\ingroup hal_i2c

\param[in]
  descriptor - pointer to an instance of HAL_I2cDescriptor_t type

\return
  0 - the request has been accepted and the bus is free \n
 -1 - otherwise
******************************************************************************/
int HAL_ReadI2cPacket(HAL_I2cDescriptor_t *descriptor);

#endif /* _I2CPACKET_H */
// eof i2cPacket.h
