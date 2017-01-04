/**************************************************************************//**
  \file N_Radio.c

  \brief Implementation of interface to transceiver fot BitCloud stack.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    28.08.12 A. Razinkov - created
    10.01.13 D. Kolmakov - modified
******************************************************************************/

/******************************************************************************
                    Includes section
******************************************************************************/
#include <macCommon.h>
#include <configServer.h>
#include <macSetGet.h>
#include <sysUtils.h>
#include <sysTaskManager.h>
#include <phyTxPowerConverter.h>

#include <N_Radio_Bindings.h>
#include <N_Radio.h>
#include <N_Radio_Init.h>
#include <N_ErrH.h>
#include <HalIrq.h>
#include <phySet.h>


/******************************************************************************
                    Defines section
******************************************************************************/
#define COMPID "N_Radio"

#ifndef N_RADIO_MAX_SUBSCRIBERS
  #define N_RADIO_MAX_SUBSCRIBERS   5
#endif

/** Maximum Tx Power for channel 26, to fit FCC requirements. Valid range is 0x00..0x0f. */
#ifndef N_RADIO_MAX_CH26_TX_POWER
  #define N_RADIO_MAX_CH26_TX_POWER  TX_PWR_3_2DBM
#endif /* N_RADIO_MAX_CH26_TX_POWER */

/** Length of the MAC Request pool. */
#ifndef N_RADIO_MAC_REQ_POOL_SIZE
  #define N_RADIO_MAC_REQ_POOL_SIZE 3
#endif /* N_RADIO_MAC_REQ_POOL_SIZE */

/******************************************************************************
                    Types section
******************************************************************************/
typedef struct _N_Radio_MacReq_t
{
  union
  {
    MAC_SetReq_t           set;
    MAC_GetReq_t           get;
  } request;
  N_Radio_SetTxPowerDone_t setTxPowerDone;
  uint8_t                  txPower;
  uint8_t                  channelSetValue;
  bool                     busy;
  void (*setCcaModeDone)(MAC_Status_t *status);
  void (*setCcaEdThreshdone)(MAC_Status_t *status);
  void (*channelSetDoneCb)(MAC_SetConf_t *conf);
} N_Radio_MacReq_t;

static N_Task_Id_t s_taskId = N_TASK_ID_NONE;

/******************************************************************************
                    Prototypes section
******************************************************************************/
static N_Radio_MacReq_t *getMacReq(void);
static void freeMacReq(MAC_SetConf_t *conf);
static void macSetTxPowerConf(MAC_SetConf_t *conf);
static void macSetChannelConf(MAC_SetConf_t *conf);
static void macSetCcaModeConf(MAC_SetConf_t *conf);
static void macSetCcaEdThreshConf(MAC_SetConf_t *conf);
void N_RadioTxBusyCallback(uint8_t);
void PHY_RegisterTxEndCallback(void (* fp)(uint8_t));
static void N_Radio_MacSetConf(MAC_SetConf_t *conf);
static void N_Radio_RxEnableConf(MAC_RxEnableConf_t *conf);


/******************************************************************************
                    Static variables section
******************************************************************************/
static N_Radio_MacReq_t macSetReqPool[N_RADIO_MAC_REQ_POOL_SIZE];
N_UTIL_CALLBACK_DECLARE(N_Radio_Callback_t, nRadioSubscribers, N_RADIO_MAX_SUBSCRIBERS);

static bool carrierModeEnabled;

/***********************************************************************************
                    Implementation section
***********************************************************************************/
/**************************************************************************//**
  \brief Gather free buffer for tx Power request.

  \return Pointer to free tx Power request structure, or NULL if no one exists.
******************************************************************************/
static N_Radio_MacReq_t *getMacReq(void)
{
  for (uint8_t i = 0; i < N_RADIO_MAC_REQ_POOL_SIZE; i++)
  {
    if (!macSetReqPool[i].busy)
    {
      macSetReqPool[i].busy = true;
      return &macSetReqPool[i];
    }
  }
  return NULL;
}

/**************************************************************************//**
  \brief Release occupied Tx Power request buffer.

  \param[in] conf - tx Power confirmation pointer.
******************************************************************************/
static void freeMacReq(MAC_SetConf_t *conf)
{
  N_Radio_MacReq_t *txPowerReq = GET_PARENT_BY_FIELD(N_Radio_MacReq_t, request.set.confirm, conf);

  N_ERRH_ASSERT_FATAL(txPowerReq->busy == true);
  txPowerReq->busy = false;
}

/**************************************************************************//**
  \brief Initialises the component.
******************************************************************************/
void N_Radio_Init(void)
{
  PHY_Enable_TxStartIRQ();
  PHY_RegisterTxEndCallback(N_RadioTxBusyCallback);
  s_taskId = N_Task_GetIdFromEventHandler(N_Radio_EventHandler);
}

#ifndef ATSAMR21G18A 
#ifndef ATSAMR21E18A
#ifndef ATSAMR21E19A
ISR(TRX24_TX_START_vect)
{
  if (TRX_STATUS_BUSY_TX_ARET == TRX_STATUS_REG_s.trxStatus)
  {
    N_UTIL_CALLBACK(N_Radio_Callback_t, nRadioSubscribers, MacEnteringTxMode,
                    (TRX_FRAME_BUFFER(0)));
  }
}
#endif
#endif
#endif
void N_RadioTxBusyCallback(uint8_t len)
{

  N_UTIL_CALLBACK(N_Radio_Callback_t, nRadioSubscribers, MacEnteringTxMode,
                    (len));

}

/**************************************************************************//**
  \brief Subscribe for events from this component.
******************************************************************************/
void N_Radio_Subscribe_Impl(const N_Radio_Callback_t* pCallback)
{
  N_UTIL_CALLBACK_SUBSCRIBE(N_Radio_Callback_t, nRadioSubscribers, pCallback);
}

/**************************************************************************//**
  \brief Get the transmit power.

  \return The TXPOWER radio register value.
******************************************************************************/
uint8_t N_Radio_GetTxPower_Impl(void)
{
  int8_t txPower;

  CS_ReadParameter(CS_RF_TX_POWER_ID, &txPower);

  return PHY_ConvertTxPowerFromDbm(txPower);
}

/**************************************************************************//**
  \brief Set the transmit power.

  \param[in] txPower - the TXPOWER radio register value to use.
  \param[in] setTxPowerDone - callback on tx power setting completion.
******************************************************************************/
void N_Radio_SetTxPower_Impl(uint8_t txPower, N_Radio_SetTxPowerDone_t setTxPowerDone)
{
  int8_t txPowerindBm = PHY_ConvertTxPowerToDbm(txPower);
  CS_WriteParameter(CS_RF_TX_POWER_ID, &txPowerindBm);
  if (!carrierModeEnabled)
  {
    N_Radio_MacReq_t *macReq = getMacReq();

    N_ERRH_ASSERT_FATAL(macReq);

    macReq->setTxPowerDone                                 = setTxPowerDone;
    macReq->request.set.attrId.phyPibId                    = PHY_PIB_TRANSMIT_POWER_ID;
    macReq->request.set.attrValue.phyPibAttr.transmitPower = txPowerindBm;
    macReq->request.set.MAC_SetConf                        = macSetTxPowerConf;

    MAC_SetReq(&macReq->request.set);
  }
  else
  {
    PHY_setTxPowerDirectly(txPower);
    if (setTxPowerDone != NULL)
    {
      setTxPowerDone();
    }
  }
}

/**************************************************************************//**
  \brief Confirmation on Tx Power setting.

  \param[in] conf - MAC-Get.Confirm parameters.
******************************************************************************/
static void macSetTxPowerConf(MAC_SetConf_t *conf)
{
  N_Radio_MacReq_t *macReq = GET_PARENT_BY_FIELD(N_Radio_MacReq_t, request.set.confirm, conf);

  if (MAC_SUCCESS_STATUS == conf->status)
  {
    if (macReq->setTxPowerDone != NULL)
    {
      macReq->setTxPowerDone();
      macReq->setTxPowerDone = NULL;
    }
    freeMacReq(conf);
  }
  /* Repeat until success */
  else
    MAC_SetReq(&macReq->request.set);
}

/**************************************************************************//**
  \brief Turn on the radio to continuously transmit a modulated or unmodulated carrier.

         This function can be used for emmission tests of the radio, but use it with care.
         \li The function should only be used on a factory new device.
         \li The carrier will block all communication on the channel.
         \li To return the radio to its normal operating mode the device must be reset.
         \li The transmit power cannot be changed after calling this function.

  \param[in] channel - the ZigBee channel on which to transmit.
  \param[in] carrierMode - select a modulated or unmodulated carrier.

  \return Corresponding tx Power in dBm.
******************************************************************************/
void N_Radio_SetCarrier_Impl(uint8_t channel, CarrierMode_t carrierMode)
{
  carrierModeEnabled = true;
  
  PHY_SetTestCarrierInit();
  
  PHY_SetReq_t  phySetReq;
  
  phySetReq.attr.phyPib.channel = channel;
  phySetReq.id = PHY_PIB_CURRENT_CHANNEL_ID;
  PHY_SetReq(&phySetReq);
  
  /* Disable BitCloud tasks for preventing calls to the radio. */
  SYS_DisableTask(ZDO_TASK_ID);
  SYS_DisableTask(APS_TASK_ID);
  SYS_DisableTask(NWK_TASK_ID);
  SYS_DisableTask(MAC_PHY_HWD_TASK_ID);
  
  uint8_t modulated = (CARRIER_MODE_UNMODULATED == carrierMode) ? false : true;
  PHY_SetTestCarrier(channel, modulated);

}

/**************************************************************************//**
  \brief Sets the current channel.

  \param[in] channel - the channel to switch to.
  \param[in] setChannelDoneCb - callback on channel setting completion.
******************************************************************************/
void N_Radio_SetChannel_Impl(uint8_t channel, void (*setChannelDoneCb)(MAC_SetConf_t *conf))
{
  uint8_t currentChannel;

  CS_ReadParameter(CS_RF_CURRENT_CHANNEL_ID, &currentChannel);

  if (currentChannel == channel)
  {
    MAC_SetConf_t confirm;
    confirm.status = MAC_SUCCESS_STATUS;
    N_ERRH_ASSERT_FATAL(setChannelDoneCb);
    setChannelDoneCb(&confirm);
  }
  else
  {
    N_Radio_MacReq_t *macReq = getMacReq();

    N_ERRH_ASSERT_FATAL(macReq);

    macReq->channelSetDoneCb = setChannelDoneCb;
    macReq->channelSetValue  = channel;
    macReq->request.set.attrId.phyPibId              = PHY_PIB_CURRENT_CHANNEL_ID;
    macReq->request.set.attrValue.phyPibAttr.channel = macReq->channelSetValue;
    macReq->request.set.MAC_SetConf                  = macSetChannelConf;
    MAC_SetReq(&macReq->request.set);
  }
}

/**************************************************************************//**
  \brief Sets CCA Mode.

  \param[in] ccaMode - CCA mode set request parameter.
  \param[in] setCcaModeDone - callback on CCA mode setting completion.
******************************************************************************/
void N_Radio_SetCcaMode_Impl(PHY_CcaMode_t ccaMode, void (*setCcaModeDone)(MAC_Status_t *status))
{
  N_Radio_MacReq_t *macReq = getMacReq();
  N_ERRH_ASSERT_FATAL(macReq);

  macReq->setCcaModeDone                           = setCcaModeDone;
  macReq->request.set.attrId.phyPibId              = PHY_PIB_CCA_MODE_ID;
  macReq->request.set.attrValue.phyPibAttr.ccaMode = ccaMode;
  macReq->request.set.MAC_SetConf                  = macSetCcaModeConf;
  MAC_SetReq(&macReq->request.set);
}

/**************************************************************************//**
  \brief Confirmation on MAC CCA setting.

  \param[in] conf - MAC-Set.Confirm parameters.
******************************************************************************/
static void macSetCcaModeConf(MAC_SetConf_t *conf)
{
  N_Radio_MacReq_t *macReq = GET_PARENT_BY_FIELD(N_Radio_MacReq_t, request.set.confirm, conf);

  if (macReq->setCcaModeDone != NULL)
  {
    macReq->setCcaModeDone(&conf->status);
    macReq->setCcaModeDone = NULL;
  }
  freeMacReq(conf);
}

/**************************************************************************//**
  \brief Sets ED Threshold value.

  \param[in] ccaEdThres - ED Threshold value set request parameter.
  \param[in] setCcaEdThreshdone - callback on CCA ED Threshold completion.
******************************************************************************/
void N_Radio_SetCcaEdThreshold_Impl(uint8_t ccaEdThres, void (*setCcaEdThreshdone)(MAC_Status_t *status))
{
  N_Radio_MacReq_t *macReq = getMacReq();
  N_ERRH_ASSERT_FATAL(macReq);

  macReq->setCcaEdThreshdone                          = setCcaEdThreshdone;
  macReq->request.set.attrId.macPibId                 = MAC_PIB_CCA_ED_THRES;
  macReq->request.set.attrValue.macPibAttr.ccaEdThres = ccaEdThres;
  macReq->request.set.MAC_SetConf                     = macSetCcaEdThreshConf;
  MAC_SetReq(&macReq->request.set);
}

/**************************************************************************//**
  \brief Confirmation on MAC CCA setting.

  \param[in] conf - MAC-Set.Confirm parameters.
******************************************************************************/
static void macSetCcaEdThreshConf(MAC_SetConf_t *conf)
{
  N_Radio_MacReq_t *macReq = GET_PARENT_BY_FIELD(N_Radio_MacReq_t, request.set.confirm, conf);

  if (macReq->setCcaEdThreshdone != NULL)
  {
    macReq->setCcaEdThreshdone(&conf->status);
    macReq->setCcaEdThreshdone = NULL;
  }
  freeMacReq(conf);
}

/**************************************************************************//**
  \brief Confirmation on MAC channel setting.

  \param[in] conf - MAC-Set.Confirm parameters.
******************************************************************************/
static void macSetChannelConf(MAC_SetConf_t *conf)
{
  N_Radio_MacReq_t *macReq = GET_PARENT_BY_FIELD(N_Radio_MacReq_t, request.set.confirm, conf);

  CS_WriteParameter(CS_RF_CURRENT_CHANNEL_ID, &macReq->channelSetValue);

  if (macReq->channelSetDoneCb != NULL)
  {
    macReq->channelSetDoneCb(conf);
    macReq->channelSetDoneCb = NULL;
  }
  freeMacReq(conf);
}

/**************************************************************************//**
  \brief Disable the Radio.
******************************************************************************/
void N_Radio_Disable_Impl(void)
{
  N_Task_SetEvent(s_taskId, EVENT_DISABLE_RADIO);
}

/**************************************************************************//**
  \brief event handler for disable radio

  \param[in] evt - Task event ID  parameters.
******************************************************************************/
bool N_Radio_EventHandler(N_Task_Event_t evt)
{
  static MAC_SetReq_t s_macSetReq;

  switch(evt)
  {
    case EVENT_DISABLE_RADIO:
      s_macSetReq.attrId.macPibId = MAC_PIB_RX_ON_WHEN_IDLE_ID;
      s_macSetReq.attrValue.macPibAttr.rxOnWhenIdle = FALSE;
      s_macSetReq.MAC_SetConf = N_Radio_MacSetConf;
      MAC_SetReq(&s_macSetReq);
    break;

    default:
      return FALSE;
  }

  return TRUE;
}

/**************************************************************************//**
  \brief MAC_SetReq confirmation callback

  \param[in] conf - MAC-Set.Confirm parameters.
******************************************************************************/
static void N_Radio_MacSetConf(MAC_SetConf_t *conf)
{
  static MAC_RxEnableReq_t s_disableRxReq;

  s_disableRxReq.rxOnDuration = 0u;
  s_disableRxReq.MAC_RxEnableConf = N_Radio_RxEnableConf;
  MAC_RxEnableReq(&s_disableRxReq);

  (void)conf;
}

/**************************************************************************//**
  \brief Confirmation call back for radio disable status to the application

  \param[in] conf - MAC-Set.Confirm parameters.
******************************************************************************/
static void N_Radio_RxEnableConf(MAC_RxEnableConf_t *conf)
{
  if (MAC_SUCCESS_STATUS == conf->status)
    N_UTIL_CALLBACK(N_Radio_Callback_t, nRadioSubscribers, MacRadioDisabled,());
}

/* eof N_Radio.c */
