/***************************************************************************//**
  \file zsiZdo.c

  \brief ZAppSI ZDO API wrapper.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2011-08-26  A. Razinkov - Created.
   Last change:
    $Id: zsiZdo.c 26492 2014-03-26 10:49:30Z karthik.p_u $
 ******************************************************************************/

/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <zdo.h>
#include <zsiZdoSerialization.h>
#include <zsiDriver.h>
#include <zsiDbg.h>

/******************************************************************************
                              Defines section
******************************************************************************/
#define ZSI_SLEEP_CONFIRM_DELAY 50UL /* ms */

/******************************************************************************
                              Types section
******************************************************************************/
typedef enum _ZsiSleepState_t
{
  ZSI_SLEEP_STATE_IDLE = 0x00,
  ZSI_SLEEP_STATE_SENDING_CONFIRM = 0x01
} ZsiSleepState_t;

/* ZSI sleep mode emulation */
typedef struct _ZsiSleepEmulator_t
{
  ZsiSleepState_t    state;
  HAL_AppTimer_t     sleepTimer;
  ZDO_SleepReq_t     *sleepReq;
  ZsiEntityService_t wakeupInd;
} ZsiSleepEmulator_t;

/******************************************************************************
                              Prototypes section
******************************************************************************/
#ifdef ZAPPSI_NP
void zsiSleepTimerFired(void);
#endif /* ZAPPSI_NP */

/******************************************************************************
                              Static variables section
******************************************************************************/
static PROGMEM_DECLARE(ZsiProcessingRoutine_t zsiZdoProcessingRoutines[]) =
{
  [ZSI_ZDO_START_NETWORK_REQUEST]  = zsiDeserializeZDO_StartNetworkReq,
  [ZSI_ZDO_START_NETWORK_CONFIRM]  = zsiDeserializeZDO_StartNetworkConf,
  [ZSI_ZDO_RESET_NETWORK_REQUEST]  = zsiDeserializeZDO_ResetNetworkReq,
  [ZSI_ZDO_RESET_NETWORK_CONFIRM]  = zsiDeserializeZDO_ResetNetworkConf,
  [ZSI_ZDO_MGMT_NWK_UPDATE_NOTIFY] = zsiDeserializeZDO_MgmtNwkUpdateNotf,
  [ZSI_ZDO_GET_LQI_RSSI_REQUEST]   = zsiDeserializeZDO_GetLqiRssiReq,
  [ZSI_ZDO_GET_LQI_RSSI_CONFIRM]   = zsiDeserializeZDO_GetLqiRssiConf,
  [ZSI_ZDO_SLEEP_REQUEST]          = zsiDeserializeZDO_SleepReq,
  [ZSI_ZDO_SLEEP_CONFIRM]          = zsiDeserializeZDO_SleepConf,
  [ZSI_ZDO_WAKEUP_INDICATION]      = zsiDeserializeZDO_WakeupInd,
  [ZSI_ZDO_BIND_INDICATION]        = zsiDeserializeZDO_BindIndication,
  [ZSI_ZDO_UNBIND_INDICATION]      = zsiDeserializeZDO_UnbindIndication,
  [ZSI_ZDO_START_SYNC_REQUEST]     = zsiDeserializeZDO_StartStopSyncReq,
  [ZSI_ZDO_START_SYNC_CONFIRM]     = zsiDeserializeZDO_StartStopSyncConf,
  [ZSI_ZDO_STOP_SYNC_REQUEST]      = zsiDeserializeZDO_StartStopSyncReq,
  [ZSI_ZDO_STOP_SYNC_CONFIRM]      = zsiDeserializeZDO_StartStopSyncConf,
  [ZSI_ZDO_NETWORK_STATUS_REQUEST] = zsiDeserializeZDO_GetNwkStatusReq,
  [ZSI_ZDO_NETWORK_STATUS_CONFIRM] = zsiDeserializeZDO_GetNwkStatusConf
};

#ifdef ZAPPSI_NP
/* Sleep mode emulation */
static ZsiSleepEmulator_t zsiSleepEmulator =
{
  .state = ZSI_SLEEP_STATE_IDLE,
  .sleepTimer =
  {
    .mode = TIMER_ONE_SHOT_MODE,
    .callback = zsiSleepTimerFired
  },
  .wakeupInd =
  {
    .zsi =
    {
      .process = (void ( *)(void *))zsiSerializeZDO_WakeUpInd
    }
  }
};
#endif /* ZAPPSI_NP */

/******************************************************************************
                               Implementation section
 ******************************************************************************/
/**************************************************************************//**
  \brief Finds routine for ZDO command deserialization.

  \param[in] commandId - command identifier.

  \return Pointer to appropriate routine.
 ******************************************************************************/
ZsiProcessingRoutine_t zsiZdoFindProcessingRoutine(uint8_t commandId)
{
  ZsiProcessingRoutine_t routine = NULL;

  if (commandId < ARRAY_SIZE(zsiZdoProcessingRoutines))
  {
    memcpy_P(&routine, &zsiZdoProcessingRoutines[commandId],
             sizeof(routine));
  }

  return routine;
}

#ifdef ZAPPSI_HOST
/**************************************************************************//**
  \brief ZDO Start Network request handler wrapper routine.

  \param[in] req - request parameters.

  \return None.
 ******************************************************************************/
void ZDO_StartNetworkReq(ZDO_StartNetworkReq_t *req)
{
  zsiProcessCommand(ZSI_AREQ_CMD, req, zsiSerializeZDO_StartNetworkReq, NULL);
}

/**************************************************************************//**
  \brief ZDO Reset Network request handler wrapper routine.

  \param[in] req - request parameters.

  \return None.
******************************************************************************/
void ZDO_ResetNetworkReq(ZDO_ResetNetworkReq_t *req)
{
  zsiProcessCommand(ZSI_AREQ_CMD, req, zsiSerializeZDO_ResetNetworkReq, NULL);
}

/**************************************************************************//**
  \brief ZDO children information request wrapper routine.

  \param[out] childrenAddr - memory for result.

  \return None.
 ******************************************************************************/
void ZDO_GetChildrenAddr(ZDO_GetChildrenAddr_t *childrenAddr)
{
  sysAssert(0U, 0xFFFF);
  (void)childrenAddr;
}

/**************************************************************************//**
  \brief ZDP request execution wrapper routine.

  \param[in] zdpReq - request parameters pointer

  \return None.
 ******************************************************************************/
void ZDO_ZdpReq(ZDO_ZdpReq_t *zdpReq)
{
  zsiProcessCommand(ZSI_AREQ_CMD, zdpReq, zsiSerializeZDO_ZdpReq, NULL);
}

/**************************************************************************//**
  \brief ZDO LQI and RSSI request wrapper routine.

  \param[out] lqiRssi - memory for result

  \return None.
 ******************************************************************************/
void ZDO_GetLqiRssi(ZDO_GetLqiRssi_t *lqiRssi)
{
  zsiProcessCommand(ZSI_SREQ_CMD, lqiRssi, zsiSerializeZDO_GetLqiRssiReq, lqiRssi);
}

/**************************************************************************//**
  \brief Wrapper to ZDO sleep request routine

  \param[in] req - sleep request parameters

  \return None.
******************************************************************************/
void ZDO_SleepReq(ZDO_SleepReq_t *req)
{
  zsiProcessCommand(ZSI_AREQ_CMD, req, zsiSerializeZDO_SleepReq, NULL);
}

/**************************************************************************//**
  \brief Wrapper to ZDO get network status routine

  \return Network status.
******************************************************************************/
ZDO_NwkStatus_t ZDO_GetNwkStatus(void)
{
  //ZDO_NwkStatus_t status;
  uint8_t status;
  bool service = true;

  zsiProcessCommand(ZSI_SREQ_CMD, &service, zsiSerializeZDO_GetNwkStatusReq, &status);

  return status;
}

#ifdef _ENDDEVICE_
/**************************************************************************//**
  \brief Start sending sync requests (polling the parent)

  \return - one of the following:
    \li ZDO_SUCCESS_STATUS - operation completed successfully
    \li ZDO_INVALID_REQUEST_STATUS - sync requests were already started
******************************************************************************/
ZDO_Status_t ZDO_StartSyncReq(void)
{
  ZDO_Status_t dataOut;
  bool startReq = true;

  zsiProcessCommand(ZSI_SREQ_CMD, &startReq, zsiSerializeZDO_StartStopSyncReq, &dataOut);

  return dataOut;
}

/**************************************************************************//**
  \brief Stop sending sync requests (polling the parent)

  \return - one of the following:
    \li ZDO_SUCCESS_STATUS - operation completed successfully
    \li ZDO_INVALID_REQUEST_STATUS - sync requests were already stopped
******************************************************************************/
ZDO_Status_t ZDO_StopSyncReq(void)
{
  ZDO_Status_t dataOut;
  bool startReq = false;

  zsiProcessCommand(ZSI_SREQ_CMD, &startReq, zsiSerializeZDO_StartStopSyncReq, &dataOut);

  return dataOut;
}
#endif // _ENDDEVICE_

#elif defined(ZAPPSI_NP)
/**************************************************************************//**
  \brief ZSI sleep timer fired handler. Used for sleep mode emulation.

  \return None.
 ******************************************************************************/
void zsiSleepTimerFired(void)
{
  switch (zsiSleepEmulator.state)
  {
    case ZSI_SLEEP_STATE_SENDING_CONFIRM:
    {
      /* Send confirm to HOST */
      ZDO_SleepReq_t *request = zsiSleepEmulator.sleepReq;
      request->confirm.status = ZDO_SUCCESS_STATUS;
      request->ZDO_SleepConf(&request->confirm);

      /* Immidiate wakeup */
      zsiSleepEmulator.state = ZSI_SLEEP_STATE_IDLE;
      ZDO_WakeUpInd();
    }
    break;

    default:
      break;
  }
}

/**************************************************************************//**
  \brief ZSI wrapper for ZDO-StartNetwork.Confirm primitive handling.

  \param[in] conf - confirm parameters.

  \return None.
 ******************************************************************************/
void zsiZDO_StartNetworkConf(ZDO_StartNetworkConf_t *conf)
{
  ZDO_StartNetworkReq_t *const req =
    GET_PARENT_BY_FIELD(ZDO_StartNetworkReq_t, confirm, conf);
  zsiProcessCommand(ZSI_AREQ_CMD, req, zsiSerializeZDO_StartNetworkConf, NULL);
}

/**************************************************************************//**
  \brief ZSI wrapper for ZDO-ResetNetwork.Confirm primitive handling.

  \param[in] conf - confirm parameters.

  \return None.
 ******************************************************************************/
void zsiZDO_ResetNetworkConf(ZDO_ResetNetworkConf_t *conf)
{
  ZDO_ResetNetworkReq_t *const req =
    GET_PARENT_BY_FIELD(ZDO_ResetNetworkReq_t, confParams, conf);
  zsiProcessCommand(ZSI_AREQ_CMD, req, zsiSerializeZDO_ResetNetworkConf, NULL);
}

/**************************************************************************//**
  \brief ZSI wrapper for ZDO-Sleep.Confirm primitive handling.

  \param[in] conf - confirm parameters.

  \return None.
 ******************************************************************************/
void zsiZDO_SleepConf(ZDO_SleepConf_t *conf)
{
  ZDO_SleepReq_t *const request =
    GET_PARENT_BY_FIELD(ZDO_SleepReq_t, confirm, conf);
  zsiProcessCommand(ZSI_AREQ_CMD, request, zsiSerializeZDO_SleepConf, NULL);
}

/**************************************************************************//**
  \brief ZSI wrapper for ZDO-Sleep.Request primitive handling.

  \param[in] req - request parameters.

  \return None.
 ******************************************************************************/
void zsiZDO_SleepReq(ZDO_SleepReq_t *req)
{
  /* Sleep mode emulation, as sleep isn't supported yet */
  zsiSleepEmulator.state = ZSI_SLEEP_STATE_SENDING_CONFIRM;
  zsiSleepEmulator.sleepReq = req;
  zsiSleepEmulator.sleepTimer.interval = ZSI_SLEEP_CONFIRM_DELAY;
  HAL_StartAppTimer(&zsiSleepEmulator.sleepTimer);
}

/**************************************************************************//**
  \brief Wrapper for ZDO wake up indication routine.

  \return None.
******************************************************************************/
void ZDO_WakeUpInd(void)
{
  zsiProcessCommand(ZSI_AREQ_CMD, &zsiSleepEmulator.wakeupInd,
                    zsiSerializeZDO_WakeUpInd, NULL);
}

/**************************************************************************//**
  \brief ZSI wrapper for ZDO-MgmtNwkUpdateNotf primitive handling.

  \param[in] nwkParams - notification parameters.

  \return None.
 ******************************************************************************/
void ZDO_MgmtNwkUpdateNotf(ZDO_MgmtNwkUpdateNotf_t *nwkParams)
{
  zsiProcessCommand(ZSI_AREQ_CMD, nwkParams, zsiSerializeZDO_MgmtNwkUpdateNotf, NULL);
}

/**************************************************************************//**
  \brief ZSI wrapper for ZDO-Zdp.Response primitive handling.

  \param[in] zdpResp - response parameters.

  \return None.
 ******************************************************************************/
void zsiZDO_ZdpResp(ZDO_ZdpResp_t *zdpResp)
{
  ZDO_ZdpReq_t *const req =
    GET_PARENT_BY_FIELD(ZDO_ZdpReq_t, resp, zdpResp);
  zsiProcessCommand(ZSI_AREQ_CMD, req, zsiSerializeZDO_ZdpResp, NULL);
}

/********************************************************************
\brief  ZDO binding indication handler wrapper

\param[in] bindInd - indication

\return None.
********************************************************************/
void ZDO_BindIndication(ZDO_BindInd_t *bindInd)
{
  zsiProcessCommand(ZSI_AREQ_CMD, bindInd, zsiSerializeZDO_BindIndication, NULL);
}

/********************************************************************
\brief  ZDO unbinding indication handler wrapper

\param[in] unbindInd - indication

\return None.
********************************************************************/
void ZDO_UnbindIndication(ZDO_UnbindInd_t *unbindInd)
{
  zsiProcessCommand(ZSI_AREQ_CMD, unbindInd, zsiSerializeZDO_UnbindIndication, NULL);
}

#endif /* ZAPPSI_NP */

/* eof zsiZdo.c */
