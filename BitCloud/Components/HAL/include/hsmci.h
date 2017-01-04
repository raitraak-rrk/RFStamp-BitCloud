/****************************************************************************//**
  \file hsmci.h

  \brief The header file describes the HSMCI interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    24/08/11 N. Fomin - Created
 ******************************************************************************/
#ifndef _HSMCI_H
#define _HSMCI_H

/******************************************************************************
                   Includes section
******************************************************************************/
#include <halHsmci.h>

/******************************************************************************
                   Types section
******************************************************************************/
/** \brief Hsmci descriptor */
typedef struct
{
  /** \brief HSMCI bus clock rate. */
  HsmciClockRate_t clockRate;
  /** \brief HSMCI bus width. */
  HsmciBusWidth_t  busWidth;
  /** \brief HSMCI high speed mode. */
  bool  highSpeedMode;
  /** \brief HSMCI command desciptor.
  Take a look into halHsmci.h platform specific file fore more details. */
  HAL_HsmciCommandDescriptor_t *commandDescriptor;
  /** \brief HSMCI data transfer descriptor.
  Take a look into halHsmci.h platform specific file fore more details. */
  HAL_HsmciDataTransferDescriptor_t *dataTransferDescriptor;
  /** \brief Address of the function to notify the upper layer when
             a transfer was completed. Can be set to NULL. */
  void (*callback)(void);
} HAL_HsmciDescriptor_t;

/**************************************************************************//**
\brief Open the HSMCI interface and configure pins.
\param[in]
    descriptor - pointer to the hsmci descriptor.
\return
   -1 - there are no free resources;
    0 - HSMCI is ready.
******************************************************************************/
int HAL_OpenHsmci(HAL_HsmciDescriptor_t *descriptor);

/**************************************************************************//**
\brief Close the HSMCI interface.
\param[in]
  descriptor - pointer to the hsmci descriptor.
\return
  Returns 0 on success or -1 if interface was not opened.
******************************************************************************/
int HAL_CloseHsmci(HAL_HsmciDescriptor_t *descriptor);

/**************************************************************************//**
\brief Sends HSMCI command.
 Callback function will be used to notify about the finishing transmitting.
\param[in]
  descriptor - pointer to hsmci descriptor.
\return
  -1 - hsmci interface was not opened, there is unsent data,
       or command response buffer is zero;
   0 - on succes.
******************************************************************************/
int HAL_WriteHsmciCommand(HAL_HsmciDescriptor_t *descriptor);

/**************************************************************************//**
\brief Writes data to the HSMCI.
 Callback function will be used to notify about the finishing transmitting.
\param[in]
  descriptor - pointer to hsmci descriptor.
\return
  -1 - hsmci interface was not opened, there is unsent data, pointer to the data,
       the length are zero or command response buffer is zero;
   0 - on success.
******************************************************************************/
int HAL_WriteHsmci(HAL_HsmciDescriptor_t *descriptor);

/**************************************************************************//**
\brief Reads data from the HSMCI.
 Callback function will be used to notify about the finishing transmitting.
\param[in]
  descriptor - pointer to hsmci descriptor.
\return
  -1 - hsmci module was not opened, there is unsent data, pointer to the data,
       the interface are zero or command response buffer is zero;
   0 - on success.
******************************************************************************/
int HAL_ReadHsmci(HAL_HsmciDescriptor_t *descriptor);

/**************************************************************************//**
\brief Sets HSMCI bus speed.
\param[in]
  descriptor - pointer to hsmci descriptor.
\return
  -1 - hsmci interface was not opened or there is unsent data;
   0 - on success.
******************************************************************************/
int HAL_SetHsmciSpeed(HAL_HsmciDescriptor_t *descriptor);

/**************************************************************************//**
\brief Sets HSMCI bus width.
\param[in]
  descriptor - pointer to hsmci descriptor.
\return
  -1 - hsmci interface was not opened or there is unsent data;
   0 - on success.
******************************************************************************/
int HAL_SetHsmciBusWidth(HAL_HsmciDescriptor_t *descriptor);

/**************************************************************************//**
\brief Sets HSMCI high speed mode.
\param[in]
  descriptor - pointer to hsmci descriptor.
\return
  -1 - hsmci interface was not opened or there is unsent data;
   0 - on success.
******************************************************************************/
int HAL_SetHsmciHighSpeedMode(HAL_HsmciDescriptor_t *descriptor);

/**************************************************************************//**
\brief Checks if HSMCI interface is free and ready.
\return
   false - hsmci interface was not opened or there is unsent data;
   true - hsmci interface is ready.
******************************************************************************/
bool HAL_HsmciCheckReady(void);

#endif /* _HSMCI_H */
// eof hsmci.h
