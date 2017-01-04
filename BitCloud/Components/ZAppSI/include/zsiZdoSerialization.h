/******************************************************************************
  \file zsiZdoSerialization.h

  \brief ZAppSI ZDO primitives serialization interface.
         This file was automatically generated.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2010 , Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   Last change:
    $Id: zsiZdoSerialization.h 25433 2013-10-04 11:38:06Z nfomin $
******************************************************************************/

#ifndef _ZSIZDOSERIALIZATION_H_
#define _ZSIZDOSERIALIZATION_H_

/******************************************************************************
                    Includes section
******************************************************************************/
#include <zdo.h>
#include <zsiFrames.h>
#include <zsiDriver.h>

/******************************************************************************
                    Types section
******************************************************************************/

/******************************************************************************
                    Prototypes section
******************************************************************************/
/**************************************************************************//**
  \brief Finds routine for ZDO commands deserialization and processing.

  \param[in] commandId - command identifier.

  \return Pointer to appropriate routine.
 ******************************************************************************/
ZsiProcessingRoutine_t zsiZdoFindProcessingRoutine(uint8_t commandId);

#ifdef ZAPPSI_HOST
/**************************************************************************//**
  \brief ZDO-StartNetwork.Request primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeZDO_StartNetworkReq(const void *const req,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief ZDO-StartNetwork.Request frame size calculation routine.
         SOF and LENGTH fields are dismissed.

  \param[in] req - request parameters.

  \return ZDO-StartNetwork.Request frame size.
 ******************************************************************************/
INLINE uint16_t
zsiZDO_StartNetworkReqLength(const ZDO_StartNetworkReq_t *const req)
{
  (void)req;
  return ZSI_COMMAND_FRAME_OVERHEAD;
}

/**************************************************************************//**
  \brief ZDO-StartNetwork.Confirm primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeZDO_StartNetworkConf(void *memory,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief ZDO-StartNetwork.Confirm callback wrapper routine.

  \param[in] req - request parameters.

  \return None.
 ******************************************************************************/
INLINE void zsiZDO_StartNetworkConfCallback(void *const req)
{
  ZDO_StartNetworkReq_t *request = (ZDO_StartNetworkReq_t *)req;
  request->ZDO_StartNetworkConf(&request->confirm);
}

/**************************************************************************//**
  \brief ZDO-Sleep.Confirm callback wrapper routine.

  \param[in] req - request parameters.

  \return None.
 ******************************************************************************/
INLINE void zsiZDO_SleepConfCallback(void *const req)
{
  ZDO_SleepReq_t *request = (ZDO_SleepReq_t *)req;
  request->ZDO_SleepConf(&request->confirm);
}

/**************************************************************************//**
  \brief ZDO-Sleep.Request primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeZDO_SleepReq(const void *const req,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief ZDO-Sleep.Request frame size calculation routine.
         SOF and LENGTH fields are dismissed.

  \param[in] req - request parameters.

  \return ZDO-Sleep.Request frame size.
 ******************************************************************************/
INLINE uint16_t
zsiZDO_SleepReqLength(const void *const req)
{
  (void)req;
  return ZSI_COMMAND_FRAME_OVERHEAD;
}

/**************************************************************************//**
  \brief ZDO-Sleep.Confirm primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeZDO_SleepConf(void *memory,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief ZDO-Wakeup.Indication primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeZDO_WakeupInd(void *memory,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief ZDO-Zdp.Response callback wrapper routine.

  \param[in] req - request parameters.

  \return None.
 ******************************************************************************/
INLINE void zsiZDO_ZdpRespCallback(void *const req)
{
  ZDO_ZdpReq_t *request = (ZDO_ZdpReq_t *)req;
  request->ZDO_ZdpResp(&request->resp);
}

/**************************************************************************//**
  \brief ZDO-MgmtNwkUpdateNotf primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeZDO_MgmtNwkUpdateNotf(void *memory,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief ZDO-GetLqiRssi request primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeZDO_GetLqiRssiReq(const void *const req,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief ZDO-GetLqiRssi request frame size calculation routine.
         SOF and LENGTH fields are dismissed.

  \param[in] req - request parameters.

  \return ZDO-GetLqiRssi request frame size.
 ******************************************************************************/
INLINE uint16_t
zsiZDO_GetLqiRssiReqLength(const void *const req)
{
  (void)req;
  return ZSI_COMMAND_FRAME_OVERHEAD +
         sizeof(uint16_t); /* nodeAddr */
}

/**************************************************************************//**
  \brief ZDO-GetLqiRssi confirm primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeZDO_GetLqiRssiConf(void *memory,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief ZDO-ZdpReq primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeZDO_ZdpReq(const void *const req,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief ZDO-ResetNetwork.Request primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeZDO_ResetNetworkReq(const void *const req,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief ZDO-ResetNetwork.Request frame size calculation routine.
         SOF and LENGTH fields are dismissed.

  \param[in] req - request parameters.

  \return ZDO-ResetNetwork.Request frame size.
 ******************************************************************************/
INLINE uint16_t
zsiZDO_ResetNetworkReqLength(const ZDO_ResetNetworkReq_t *const req)
{
  (void)req;
  return ZSI_COMMAND_FRAME_OVERHEAD;
}

/**************************************************************************//**
  \brief ZDO-ResetNetwork.Confirm primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeZDO_ResetNetworkConf(void *memory,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief ZDO-ResetNetwork.Confirm callback wrapper routine.

  \param[in] req - request parameters.

  \return None.
 ******************************************************************************/
INLINE void zsiZDO_ResetNetworkConfCallback(void *const req)
{
  ZDO_ResetNetworkReq_t * request = (ZDO_ResetNetworkReq_t *)req;
  request->ZDO_ResetNetworkConf(&request->confParams);
}

/**************************************************************************//**
  \brief ZDO-BindingInd primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeZDO_BindIndication(void *memory,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief ZDO-BindingInd primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeZDO_UnbindIndication(void *memory,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief ZDO-GetNwkStatus primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeZDO_GetNwkStatusReq(const void *const req,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief ZDO-GetNwkStatus frame size calculation routine.
         SOF and LENGTH fields are dismissed.

  \param[in] ind - indication parameters.

  \return ZDO-GetNwkStatus request frame size.
 ******************************************************************************/
INLINE uint16_t
zsiSerializeZDO_GetNwkStatusReqLength(const void *const req)
{
  (void)req;
  return ZSI_COMMAND_FRAME_OVERHEAD;
}

/**************************************************************************//**
  \brief ZDO-GetNwkStatus confirm primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

    \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeZDO_GetNwkStatusConf(void *memory,
  ZsiCommandFrame_t *const cmdFrame);

#ifdef _ENDDEVICE_
/**************************************************************************//**
  \brief ZDO_StartSyncReq or ZDO_StopSyncReq  request frame size
         calculation routine. SOF and LENGTH fields are dismissed.

  \param[in] req - request parameters.

  \return ZDO-GetLqiRssi request frame size.
 ******************************************************************************/
INLINE uint16_t zsiZDO_StartStopSyncReq(const void *const req)
{
  (void)req;
  return ZSI_COMMAND_FRAME_OVERHEAD + sizeof(bool);
}

/**************************************************************************//**
  \brief ZDO_StartSyncReq or ZDO_StopSyncReq primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeZDO_StartStopSyncReq(const void *const req,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief ZDO_StartSyncConf primitive deserialization and
         processing routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
*******************************************************************************/
ZsiProcessingResult_t zsiDeserializeZDO_StartStopSyncConf(void *memory,
  ZsiCommandFrame_t *const cmdFrame);
#endif // _ENDDEVICE_
#elif defined(ZAPPSI_NP)
/**************************************************************************//**
  \brief ZDO-StartNetwork.Request primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeZDO_StartNetworkReq(void *memory,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief ZSI wrapper for ZDO-StartNetwork.Confirm primitive handling.

  \param[in] conf - confirm parameters.

  \return None.
 ******************************************************************************/
void zsiZDO_StartNetworkConf(ZDO_StartNetworkConf_t *conf);

/**************************************************************************//**
  \brief ZSI wrapper for ZDO-Sleep.Request primitive handling. Sleep mode
         emulation.

  \param[in] req - request parameters.

  \return None.
 ******************************************************************************/
void zsiZDO_SleepReq(ZDO_SleepReq_t *req);

/**************************************************************************//**
  \brief ZSI wrapper for ZDO-Sleep.Confirm primitive handling.

  \param[in] conf - confirm parameters.

  \return None.
 ******************************************************************************/
void zsiZDO_SleepConf(ZDO_SleepConf_t *conf);

/**************************************************************************//**
  \brief ZSI wrapper for ZDO-Zdp.Response primitive handling.

  \param[in] zdpResp - response parameters.

  \return None.
 ******************************************************************************/
void zsiZDO_ZdpResp(ZDO_ZdpResp_t *zdpResp);

/**************************************************************************//**
  \brief ZDO-StartNetwork.Confirm primitive serialization routine.

  \param[in] request - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeZDO_StartNetworkConf(const void *const req,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief ZDO-Sleep.Confirm primitive serialization routine.

  \param[in] request - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeZDO_SleepConf(const void *const req,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief ZDO-StartNetwork.Confirm frame size calculation routine.
         SOF and LENGTH fields are dismissed.

  \param[in] conf - confirm parameters.

  \return ZDO-StartNetwork.Confirm frame size.
 ******************************************************************************/
INLINE uint16_t
zsiZDO_StartNetworkConfLength(const ZDO_StartNetworkConf_t *const conf)
{
  (void)conf;
  return ZSI_COMMAND_FRAME_OVERHEAD +
         sizeof(uint8_t)  + /* activeChannel */
         sizeof(uint16_t) + /* shortAddr */
         sizeof(uint16_t) + /* PANId */
         sizeof(uint64_t) + /* extPANId */
         sizeof(uint16_t) + /* parentAddr */
         sizeof(uint8_t);   /* status */
}

/**************************************************************************//**
  \brief ZDO-Sleep.Request primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeZDO_SleepReq(void *memory,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief ZDO-Sleep.Confirm frame size calculation routine.
         SOF and LENGTH fields are dismissed.

  \param[in] conf - confirm parameters.

  \return ZDO-Sleep.Confirm frame size.
 ******************************************************************************/
INLINE uint16_t
zsiZDO_SleepConfLength(const ZDO_StartNetworkConf_t *const conf)
{
  (void)conf;
  return ZSI_COMMAND_FRAME_OVERHEAD +
         sizeof(uint8_t);   /* status */
}

/**************************************************************************//**
  \brief ZDO-Wakeup.Indication frame size calculation routine.
         SOF and LENGTH fields are dismissed.

  \param[in] ind - indication parameters.

  \return ZDO-Wakeup.Indication frame size.
 ******************************************************************************/
INLINE uint16_t
zsiZDO_WakeUpIndLength(const void *const ind)
{
  (void)ind;
  return ZSI_COMMAND_FRAME_OVERHEAD;
}

/**************************************************************************//**
  \brief ZDO-Wakeup.Indication primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeZDO_WakeUpInd(const void *const req,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief ZDO-MgmtNwkUpdateNotf primitive serialization routine.

  \param[in] ntfy - notification parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeZDO_MgmtNwkUpdateNotf(const void *const ntfy,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief ZDO-MgmtNwkUpdateNotf frame size calculation routine.
         SOF and LENGTH fields are dismissed.

  \param[in] ntfy - notification parameters.

  \return ZDO-MgmtNwkUpdateNotf frame size.
 ******************************************************************************/
uint16_t
zsiZDO_MgmtNwkUpdateNotfLength(const ZDO_MgmtNwkUpdateNotf_t *const ntfy);

/**************************************************************************//**
  \brief ZDO-GetLqiRssi request primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeZDO_GetLqiRssiReq(void *memory,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief ZDO-GetLqiRssi confirm frame size calculation routine.
         SOF and LENGTH fields are dismissed.

  \param[in] conf - confirm parameters.

  \return ZDO-GetLqiRssi confirm frame size.
 ******************************************************************************/
INLINE uint16_t
zsiZDO_GetLqiRssiConfLength(const void *const conf)
{
  (void)conf;
  return ZSI_COMMAND_FRAME_OVERHEAD +
         sizeof(uint8_t) + /* lqi */
         sizeof(uint8_t);  /* rssi */
}

/**************************************************************************//**
  \brief ZDO-ZdpResp primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeZDO_ZdpResp(const void *const req,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief ZDO-ResetNetwork.Request primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeZDO_ResetNetworkReq(void *memory,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief ZSI wrapper for ZDO-ResetNetwork.Confirm primitive handling.

  \param[in] conf - confirm parameters.

  \return None.
 ******************************************************************************/
void zsiZDO_ResetNetworkConf(ZDO_ResetNetworkConf_t *conf);

/**************************************************************************//**
  \brief ZDO-ResetNetwork.Confirm primitive serialization routine.

  \param[in] request - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeZDO_ResetNetworkConf(const void *const req,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief ZDO-ResetNetwork.Confirm frame size calculation routine.
         SOF and LENGTH fields are dismissed.

  \param[in] conf - confirm parameters.

  \return ZDO-ResetNetwork.Confirm frame size.
 ******************************************************************************/
INLINE uint16_t
zsiZDO_ResetNetworkConfLength(const ZDO_ResetNetworkConf_t *const conf)
{
  (void)conf;
  return ZSI_COMMAND_FRAME_OVERHEAD +
         sizeof(uint8_t); /* confirm->status */
}

/**************************************************************************//**
  \brief ZDO-BindingInd primitive serialization routine.

  \param[in] ind - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeZDO_BindIndication(const void *const ind,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief ZDO-BindingInd frame size calculation routine.
         SOF and LENGTH fields are dismissed.

  \param[in] ind - confirm parameters.

  \return ZDO-BindingInd frame size.
 ******************************************************************************/
INLINE uint16_t
zsiZDO_BindIndicationLength(const ZDO_BindInd_t *const ind)
{
  (void)ind;
  return ZSI_COMMAND_FRAME_OVERHEAD +
         sizeof(uint64_t) + /* srcAddr */
         sizeof(uint8_t)  + /* srcEndpoint */
         sizeof(uint16_t) + /* clusterId */
         sizeof(uint8_t)  + /* dstAddrMode */
         sizeof(uint64_t) + /* dstExtAddr */
         sizeof(uint8_t);   /* dstEndpoint */
}

/**************************************************************************//**
  \brief ZDO-UnbindingInd primitive serialization routine.

  \param[in] ind - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeZDO_UnbindIndication(const void *const ind,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief ZDO-UnbindingInd frame size calculation routine.
         SOF and LENGTH fields are dismissed.

  \param[in] ind - confirm parameters.

  \return ZDO-BindingInd frame size.
 ******************************************************************************/
INLINE uint16_t
zsiZDO_UnbindIndicationLength(const ZDO_UnbindInd_t *const ind)
{
  (void)ind;
  return ZSI_COMMAND_FRAME_OVERHEAD +
         sizeof(uint64_t) + /* srcAddr */
         sizeof(uint8_t)  + /* srcEndpoint */
         sizeof(uint16_t) + /* clusterId */
         sizeof(uint8_t)  + /* dstAddrMode */
         sizeof(uint64_t) + /* dstExtAddr */
         sizeof(uint8_t);   /* dstEndpoint */
}

/**************************************************************************//**
  \brief ZDO-ZDO_GetNwkStatus request primitive deserialization and
         processing routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

    \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeZDO_GetNwkStatusReq(void *memory,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief ZDO-ZDO_GetNwkStatus confirm frame size calculation routine.
         SOF and LENGTH fields are dismissed.

  \param[in] req - request parameters.

  \return ZDO-ZDO_GetNwkStatus confirm frame size.
 ******************************************************************************/
INLINE uint16_t
zsiZDO_GetNwkStatusConfLength(const void *const req)
{
  (void)req;
  return ZSI_COMMAND_FRAME_OVERHEAD +
         sizeof(ZDO_NwkStatus_t); /* status */
}

#ifdef _ENDDEVICE_
/**************************************************************************//**
  \brief ZDO_StartSyncConf request frame size calculation routine.
         SOF and LENGTH fields are dismissed.

  \param[in] req - request parameters.

  \return ZDO-GetLqiRssi request frame size.
 ******************************************************************************/
INLINE uint16_t zsiZDO_StartStopSyncConf(const void *const req)
{
  (void)req;
  return ZSI_COMMAND_FRAME_OVERHEAD + sizeof(ZDO_Status_t);
}

/**************************************************************************//**
  \brief ZDO_StartSyncReq or ZDO_StopSyncReq request primitive deserialization and
         processing routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
*******************************************************************************/
ZsiProcessingResult_t zsiDeserializeZDO_StartStopSyncReq(void *memory,
  ZsiCommandFrame_t *const cmdFrame);
#endif // _ENDDEVICE_
#endif /* ZAPPSI_NP */

#ifdef ZAPPSI_HOST
#define zsiDeserializeZDO_StartNetworkReq NULL
#define zsiDeserializeZDO_ResetNetworkReq NULL
#define zsiDeserializeZDO_GetLqiRssiReq   NULL
#define zsiDeserializeZDO_SleepReq        NULL

#define zsiDeserializeZDO_StartStopSyncReq NULL
#define zsiDeserializeZDO_GetNwkStatusReq  NULL
#ifndef _ENDDEVICE_
#define zsiDeserializeZDO_StartStopSyncConf NULL
#endif // _ENDDEVICE_

#elif defined(ZAPPSI_NP)
#define zsiDeserializeZDO_StartNetworkConf  NULL
#define zsiDeserializeZDO_ResetNetworkConf  NULL
#define zsiDeserializeZDO_MgmtNwkUpdateNotf NULL
#define zsiDeserializeZDO_GetLqiRssiConf    NULL
#define zsiDeserializeZDO_SleepConf         NULL
#define zsiDeserializeZDO_WakeupInd         NULL
#define zsiDeserializeZDO_BindIndication    NULL
#define zsiDeserializeZDO_UnbindIndication  NULL
#define zsiDeserializeZDO_StartStopSyncConf NULL
#define zsiDeserializeZDO_GetNwkStatusConf  NULL
#ifndef _ENDDEVICE_
#define zsiDeserializeZDO_StartStopSyncReq  NULL
#endif // _ENDDEVICE_

#endif /* ZAPPSI_NP */

#endif /* _ZSIZDOSERIALIZATION_H_ */
