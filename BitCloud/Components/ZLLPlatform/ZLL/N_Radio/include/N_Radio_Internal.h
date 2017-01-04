/**************************************************************************//**
  \file N_Radio_Internal.h

  \brief Implementation of internal interface to transceiver.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    10.01.13 D. Kolmakov - created
******************************************************************************/

#ifndef N_RADIO_INTERNAL_H
#define N_RADIO_INTERNAL_H

/******************************************************************************
                               Prototypes section
 ******************************************************************************/

/**************************************************************************//**
  \brief Sets the current channel.

  \param[in] channel - the channel to switch to.
  \param[in] setChannelDoneCb - callback on channel setting completion.
******************************************************************************/
void N_Radio_SetChannel_Impl(uint8_t channel, void (*setChannelDoneCb)(MAC_SetConf_t *conf));


#endif // N_RADIO_INTERNAL_H

/* eof N_Radio_Internal.h */