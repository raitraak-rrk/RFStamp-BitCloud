/****************************************************************************//**
  \file massStorageDevice.h

  \brief The header file describes the interface of the mass storage device

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    29/08/11 N. Fomin - Created
*******************************************************************************/
#ifndef _MASSSTORAGEDEVICE_H
#define _MASSSTORAGEDEVICE_H

/******************************************************************************
                   Includes section
******************************************************************************/
#include <sysTypes.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
#define MSD_TRANSMIT_PIPE      5
#define MSD_RECEIVE_PIPE       6

/******************************************************************************
                   Types section
******************************************************************************/
/** \brief Status messages for upper layer about msd status */
typedef enum
{
  MSD_STATUS_SUCCESS = 0,
  MSD_INTERFACE_BUSY,
  MSD_MEMORY_INIT_ERROR,
  MSD_READ_CAPACITY_ERROR,
  MSD_READ_ERROR,
  MSD_WRITE_ERROR
} MSD_Status_t;

typedef void (* MSD_Callback_t)(MSD_Status_t);

/******************************************************************************
                   Prototypes section
******************************************************************************/
/**************************************************************************//**
\brief Opens mass storage device.
\param[in]
  callback - pointer to function to notify about MSD errors and transactions; can be set to NULL
\param[in]
  responseBuffer - pointer to hsmci command response buffer; it should have
                   a size of four;
\param[in]
  buffer - pointer to buffer for hsmci data transfer; should be a multiplier of 512
\param[in]
  length - length of buffer for data transfer.
\return
  nothing
******************************************************************************/
void MSD_Open(MSD_Callback_t callback, uint32_t *responseBuffer, uint8_t *buffer, uint32_t length);

/**************************************************************************//**
\brief Closes mass storage device.
\return
  nothing
******************************************************************************/
void MSD_Close(void);

#endif /* _MASSSTORAGEDEVICE_H */
