/*********************************************************************************************//**
\file  

\brief 

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal

$Id: N_Radio.h 24615 2013-02-27 05:14:24Z arazinkov $

***************************************************************************************************/

#ifndef N_RADIO_H
#define N_RADIO_H

/***************************************************************************************************
* INCLUDE FILES
***************************************************************************************************/

#include "N_Types.h"
#include <phy.h>
#include <macEnvironment.h>
#include <macCommon.h>

/***************************************************************************************************
* C++ DECLARATION WRAPPER
***************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************************************
* EXPORTED TYPES
***************************************************************************************************/
#define EVENT_DISABLE_RADIO     2u

typedef enum CarrierMode_t
{
    CARRIER_MODE_UNMODULATED,
    CARRIER_MODE_MODULATED_SPECTRUM
} CarrierMode_t;

typedef struct N_Radio_Callback_t
{
    /** Notify when the MAC layer starts the clear channel assessment before a transmission.
        \param txDataLength The length of the packet that will be transmitted

        \note The callback comes from an Interrupt Service Routine
    */
    void (*MacEnteringTxMode)(uint8_t txDataLength);

    /** Notify when the MAC layer has disabled the radio
    */
    void (*MacRadioDisabled)(void);

    int8_t endOfList;
} N_Radio_Callback_t;

/* Callback on TxPower setting */
typedef void (*N_Radio_SetTxPowerDone_t)(void);

/***************************************************************************************************
* EXPORTED FUNCTIONS
***************************************************************************************************/

/** Subscribe for events from this component.
*/
void N_Radio_Subscribe(const N_Radio_Callback_t* pCallback);

/** Set the transmit power.
    \param txPower The TXPOWER radio register value to use
    \param setTxPowerDone Callback on tx power setting completion
*/
void N_Radio_SetTxPower(uint8_t txPower, N_Radio_SetTxPowerDone_t setTxPowerDone);

/** Get the transmit power.
    \return The TXPOWER radio register value
*/
uint8_t N_Radio_GetTxPower(void);

/** Turn on the radio to continuously transmit a modulated or unmodulated carrier.
    \param channel The ZigBee channel on which to transmit
    \param carrierMode Select a modulated or unmodulated carrier

    This function can be used for emmission tests of the radio, but use it with care.
    \li The function should only be used on a factory new device
    \li The carrier will block all communication on the channel
    \li To return the radio to its normal operating mode the device must be reset
    \li The transmit power cannot be changed after calling this function
*/
void N_Radio_SetCarrier(uint8_t channel, CarrierMode_t carrierMode);

/** Sets CCA Mode.
  \param ccaMode - CCA mode set request parameter.
  \param setCcaModeDone - callback on CCA mode setting completion.
*/
void N_Radio_SetCcaMode(PHY_CcaMode_t ccaMode, void (*setCcaModeDone)(MAC_Status_t *status));

/** Sets ED Threshold value.
  \param ccaEdThres - ED Threshold value set request parameter.
  \param setCcaEdThreshdone - callback on CCA ED Threshold completion.
*/
void N_Radio_SetCcaEdThreshold_Impl(uint8_t ccaEdThres, void (*setCcaEdThreshdone)(MAC_Status_t *status));

/** Disables device's Radio
*/
void N_Radio_Disable(void);
/***************************************************************************************************
* END OF C++ DECLARATION WRAPPER
***************************************************************************************************/

#ifdef __cplusplus
}
#endif

#endif // N_RADIO_H
