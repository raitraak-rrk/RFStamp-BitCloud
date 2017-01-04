/**************************************************************************//**
  \file macEnvironment.h

  \brief Public interface of MAC Environment.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    22/02/11 M. Gekk - Created.
 ******************************************************************************/
#ifndef _MAC_ENVIRONMENT_H
#define _MAC_ENVIRONMENT_H

/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <macAddr.h>

/******************************************************************************
                              Prototypes section
 ******************************************************************************/
/**************************************************************************//**
  \brief Gets pointer to extended address.
  \return pointer to extended address.
 ******************************************************************************/
const ExtAddr_t* MAC_GetExtAddr(void);

/**************************************************************************//**
  \brief Sets extended address.
  \param extAddr - extended address.

  \return none.
 ******************************************************************************/
void MAC_SetExtAddr(const ExtAddr_t *const extAddr);

/**************************************************************************//**
  \brief Checks, if extended address is equal to our own one
  \param extAddr - pointer to extended address to check

  \return true, if extended address is equal to our own one; false - otherwise
 ******************************************************************************/
bool MAC_IsOwnExtAddr(const ExtAddr_t *const extAddr);

/**************************************************************************//**
  \brief Convert octet amount to transmit duration.
  \param octets - number of octets.

  \return Time of octets transmission in milliseconds.
 ******************************************************************************/
uint32_t MAC_GetOctetsTransmissionTimeInMs(const uint32_t octets);

/**************************************************************************//**
  \brief Checks if there are any active transactions on MAC layer.

  \return true, if there are any active transactions on MAC layer,
          false - otherwise
 ******************************************************************************/
bool MAC_IsActiveTransaction(void);

#endif /* _MAC_ENVIRONMENT_H */
/** eof macEnvironment.h */
