/***************************************************************************//**
  \file zsiAps.c

  \brief ZAppSI APS API wrapper.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2011-08-26  A. Razinkov - Created.
   Last change:
    $Id: zsiAps.c 27025 2014-08-05 05:08:54Z unithra.c $
 ******************************************************************************/

/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <apsdeEndpoint.h>
#include <zsiApsSerialization.h>
#include <zsiDriver.h>
#include <zsiDbg.h>
#include <sysQueue.h>

/******************************************************************************
                              Static variables section
******************************************************************************/
static PROGMEM_DECLARE(ZsiProcessingRoutine_t zsiApsProcessingRoutines[]) =
{
  [ZSI_APS_REGISTER_ENDPOINT_REQUEST]           = zsiDeserializeAPS_RegisterEndpointReq,
  [ZSI_APS_REGISTER_ENDPOINT_CONFIRM]           = zsiDeserializeAPS_RegisterEndpointConf,
  [ZSI_APS_DATA_INDICATION]                     = zsiDeserializeAPS_DataInd,
  [ZSI_APS_DATA_REQUEST]                        = zsiDeserializeAPS_DataReq,
  [ZSI_APS_DATA_CONFIRM]                        = zsiDeserializeAPS_DataConf,
  [ZSI_APS_SET_LINK_KEY_REQUEST]                = zsiDeserializeAPS_SetLinkKeyReq,
  [ZSI_APS_SET_LINK_KEY_CONFIRM]                = zsiDeserializeAPS_SetLinkKeyConf,
  [ZSI_APS_GET_LINK_KEY_REQUEST]                = zsiDeserializeAPS_GetLinkKeyReq,
  [ZSI_APS_GET_LINK_KEY_CONFIRM]                = zsiDeserializeAPS_GetLinkKeyConf,
  [ZSI_APS_FIND_KEYS_REQUEST]                   = zsiDeserializeAPS_FindKeysReq,
  [ZSI_APS_FIND_KEYS_CONFIRM]                   = zsiDeserializeAPS_FindKeysConf,
  [ZSI_APS_BIND_REQUEST]                        = zsiDeserializeAPS_BindReq,
  [ZSI_APS_BIND_CONFIRM]                        = zsiDeserializeAPS_BindConf,
  [ZSI_APS_GET_KEY_PAIR_DEV_ADDR_REQUEST]       = zsiDeserializeAPS_GetKeyPairDeviceAddressReq,
  [ZSI_APS_GET_KEY_PAIR_DEV_ADDR_CONFIRM]       = zsiDeserializeAPS_GetKeyPairDeviceAddressConf,
  [ZSI_APS_NEXT_KEYS_REQUEST]                   = zsiDeserializeAPS_NextKeysReq,
  [ZSI_APS_NEXT_KEYS_CONFIRM]                   = zsiDeserializeAPS_NextKeysConf,
  [ZSI_APS_MARK_TC_KEY_REQUEST]                 = zsiDeserializeAPS_MarkGlobalKeyAsTCKeyReq,
  #ifdef __LINK_SECURITY__
  [ZSI_APS_MARK_TC_KEY_CONFIRM]                 = zsiDeserializeAPS_MarkGlobalKeyAsTCkeyConf,
  #endif
  [ZSI_APS_SET_AUTHORIZED_STATUS_REQUEST]       = zsiDeserializeAPS_SetAuthorizedStatusReq,
  [ZSI_APS_SET_AUTHORIZED_STATUS_CONFIRM]       = zsiDeserializeAPS_SetAuthorizedStatusConf,
  [ZSI_APS_SET_DEFAULT_KEY_PAIR_STATUS_REQUEST] = zsiDeserializeAPS_SetDefaultKeyPairStatusReq,
  #ifdef __LINK_SECURITY__
  [ZSI_APS_SET_DEFAULT_KEY_PAIR_STATUS_CONFIRM] = zsiDeserializeAPS_SetDefaultKeyPairStatusConf,
  #endif
  [ZSI_APS_ARE_KEYS_AUTHORIZED_REQUEST]         = zsiDeserializeAPS_AreKeysAuthorizedReq,
  [ZSI_APS_ARE_KEYS_AUTHORIZED_CONFIRM]         = zsiDeserializeAPS_AreKeysAuthorizedConf,
  [ZSI_APS_GET_TRUST_CENTRE_ADDRESS_REQUEST]    = zsiDeserializeAPS_GetTrustCenterAddressReq,
  [ZSI_APS_GET_TRUST_CENTRE_ADDRESS_CONFIRM]    = zsiDeserializeAPS_GetTrustCenterAddressConf,
  [ZSI_APS_NEXT_BINDING_ENTRY_REQUEST]          = zsiDeserializeAPS_NextBindingEntryReq,
  [ZSI_APS_NEXT_BINDING_ENTRY_CONFIRM]          = zsiDeserializeAPS_NextBindingEntryConf
};

#ifdef ZAPPSI_HOST
/* Queue of registered endpoints */
static ApsBindingEntry_t nextEntry;
static DECLARE_QUEUE(apsEndpointsQueue);
#ifdef _LINK_SECURITY_
static uint8_t zsiLinkKey[SECURITY_KEY_SIZE];
static ExtAddr_t zsiDeviceAddr;
static ExtAddr_t zsiTcAddr;
#endif /* _LINK_SECURITY_ */
#elif defined(ZAPPSI_NP)
/* Memory to register endpoints on NP */
static ZsiEndpointDescriptor_t zsiEndpoint[ZSI_MAX_ENDPOINTS_AMOUNT];
#endif /* ZAPPSI_HOST */

/******************************************************************************
                               Implementation section
 ******************************************************************************/
/**************************************************************************//**
  \brief Finds routine for APS commands deserialization.

  \param[in] commandId - command identifier.

  \return Pointer to appropriate routine.
 ******************************************************************************/
ZsiProcessingRoutine_t zsiApsFindProcessingRoutine(uint8_t commandId)
{
  ZsiProcessingRoutine_t routine = NULL;

  if (commandId < ARRAY_SIZE(zsiApsProcessingRoutines))
  {
    memcpy_P(&routine, &zsiApsProcessingRoutines[commandId],
             sizeof(routine));
  }

  return routine;
}

#ifdef ZAPPSI_HOST
/**************************************************************************//**
  \brief Finds APS endpoint descriptor whithin endpoints queue.

  \param[in] endpoint - endpoint identifier.

  \return Pointer to appropriate endpoint descriptor, or NULL if wasn't found.
 ******************************************************************************/
APS_RegisterEndpointReq_t *zsiApsFindEndpoint(Endpoint_t endpoint)
{
  APS_RegisterEndpointReq_t *descriptor = getQueueElem(&apsEndpointsQueue);

  while (descriptor)
  {
    if (descriptor->simpleDescriptor->endpoint == endpoint)
      return descriptor;

    descriptor = descriptor->service.next;
  }

  return NULL;
}
/**************************************************************************//**
  \brief Clears the registered Endpoints  whithin endpoints queue.
******************************************************************************/
void zsiApsclearEndPointQueue(void)
{
  resetQueue(&apsEndpointsQueue);
}
/**************************************************************************//**
  \brief New endpoint registration wrapper routine.

  \param[in] req - pointer to the endpoint register request.

  \return APS_SUCCESS_STATUS in req->status if registration is successfully,
          otherwise APS_INVALID_PARAMETER_STATUS.
 ******************************************************************************/
void APS_RegisterEndpointReq(APS_RegisterEndpointReq_t *const req)
{
  zsiProcessCommand(ZSI_SREQ_CMD, req, zsiSerializeAPS_RegisterEndpointReq,
                    &req->status);

  if (APS_SUCCESS_STATUS == req->status)
  {
    putQueueElem(&apsEndpointsQueue, req);
  }
}

/**************************************************************************//**
  \brief Wrapper routine to unregister endpoint from APS-layer.

  \param[in] req - pointer to the endpoint unregister request.

  \return None.
 ******************************************************************************/
void APS_UnregisterEndpointReq(APS_UnregisterEndpointReq_t *const req)
{
  sysAssert(0U, 0xFFFF);
  (void)req;
}

/**************************************************************************//**
  \brief Wrapper routine to find a next busy entry from the binding table.

  \param[in] entry - pointer to an binding entry or NULL.

  \return first busy entry if entry is NULL
          or next busy entry,
          or NULL if next busy entry is not found.
 ******************************************************************************/
ApsBindingEntry_t* APS_NextBindingEntry(ApsBindingEntry_t *entry)
{
  zsiProcessCommand(ZSI_SREQ_CMD, entry, zsiSerializeAPS_NextBindingEntryReq,
                    &nextEntry);
  return &nextEntry;
}

/**************************************************************************//**
  \brief Wrapper routine to stop APS Data indications of specific endpoint number.

  \param[in] endpoint - the registered endpoint number. The 0xff special value
                        means all registered endpoints.
  \return None.
 ******************************************************************************/
void APS_StopEndpointIndication(const Endpoint_t endpoint)
{
  sysAssert(0U, 0xFFFF);
  (void)endpoint;
}

/**************************************************************************//**
  \brief Wrapper routine resume APS Data indications of specific endpoint number.

  \param[in] endpoint - the registered endpoint number. The 0xff special value
                        means all registered endpoints.
  \return None.
 ******************************************************************************/
void APS_ResumeEndpointIndication(const Endpoint_t endpoint)
{
  sysAssert(0U, 0xFFFF);
  (void)endpoint;
}

/**************************************************************************//**
  \brief Wrapper routine to get next registered endpoint descriptor by previous
         one.

  \param[in] prev - previous endpoint descriptor pointer.It is must be non NULL
                    sequential access required if. And it is must be NULL the
                    first descriptor as registered endpoints queue head element
                    access required if.

  \return Valid endpoint descriptor pointer descriptor found if,
           NULL - other case.
 ******************************************************************************/
APS_RegisterEndpointReq_t *APS_NextEndpoint(const APS_RegisterEndpointReq_t *const prev)
{
  if (NULL == prev)
    return getQueueElem(&apsEndpointsQueue);
  else
    return getNextQueueElem(prev);
}

#ifdef _BINDING_
/**************************************************************************//**
  \brief Wrapper routine to check binding parameters and create new entry
         in the binding table.

  \param[in] req - pointer to request's parameters: source and destination
                  addresses, cluster identifier and etc. See APS_BindReq_t.
  \return None.
 ******************************************************************************/
void APS_BindReq(APS_BindReq_t *const req)
{
  // initialize status field, because on different platforms lengths of this field can be different.
  req->confirm.status = 0;
  zsiProcessCommand(ZSI_SREQ_CMD, req, zsiSerializeAPS_BindReq,
    &req->confirm.status);
}
#endif /* _BINDING_ */

/**************************************************************************//**
  \brief APDE-DATA.request handler wrapper routine.

  \param[in] req - primitive pointer.

  \return None.
 ******************************************************************************/
void APS_DataReq(APS_DataReq_t *const req)
{
  zsiProcessCommand(ZSI_AREQ_CMD, req, zsiSerializeAPS_DataReq, NULL);
}

#ifdef _LINK_SECURITY_
/**************************************************************************//**
  \brief Wrapper routine for a setting of a new link key for a given device.
         This function copies value of link key to APS Key-Pair Set.

  \param[in] deviceAddress - pointer to extended IEEE device address.
  \param[in] linkKey - pointer to new cryptographic link key.

  \return Valid key handle if operation is successfully completed otherwise
          invalid key handle (Use APS_KEY_HANDLE_IS_VALID to check it out).
 ******************************************************************************/
APS_KeyHandle_t APS_SetLinkKey(const ExtAddr_t *const deviceAddress,
  const uint8_t *const linkKey)
{
  ZsiKeyDescriptor_t descriptor =
  {
    .key = (uint8_t *)linkKey
  };
  APS_KeyHandle_t handle;

  COPY_EXT_ADDR(descriptor.extAddress, deviceAddress);
  zsiProcessCommand(ZSI_SREQ_CMD, &descriptor, zsiSerializeAPS_SetLinkKeyReq,
    &handle);

  return handle;
}

/**************************************************************************//**
  \brief Get link key.

  \param[in] handle - valid key handle.

  \return Pointer to link key or NULL if handle is invalid.
 ******************************************************************************/
uint8_t *APS_GetLinkKey(const APS_KeyHandle_t handle)
{
  ZsiKeyDescriptor_t descriptor =
  {
    .key = zsiLinkKey
  };
  zsiProcessCommand(ZSI_SREQ_CMD, (int16_t*)&handle, zsiSerializeAPS_GetLinkKeyReq,
    &descriptor);

  if (descriptor.keyFond)
    return descriptor.key;

  return NULL;
}

/**************************************************************************//**
  \brief Wrapper routine to find initial (master key for High Security and
         preinstalled link key for SE security) or link key of device.

  \param[in] deviceAddress - pointer to extended IEEE device address.

  \return Valid key handle if any key is found otherwise
          invalid key handle (Use APS_KEYS_FOUND to check it out).
 ******************************************************************************/
APS_KeyHandle_t APS_FindKeys(const ExtAddr_t *const deviceAddress)
{
  APS_KeyHandle_t handle;

  zsiProcessCommand(ZSI_SREQ_CMD, (ExtAddr_t *)deviceAddress, zsiSerializeAPS_FindKeysReq,
                    &handle);

  return handle;
}

/**************************************************************************//**
  \brief Wrapper routine to request either the active network key or a new
         end-to-end app key.

     See ZigBee Specification r19, 4.4.6.1, page 465.

  \param[in] req - pointer to APSME-REQUEST-KEY.request's parameters.
                   \sa APS_RequestKeyReq_t
  \return None.
 ******************************************************************************/
void APS_RequestKeyReq(APS_RequestKeyReq_t *req)
{
  sysAssert(0U, 0xFFFF);
  (void)req;
}

/**************************************************************************//**
  \brief Wrapper routine to request extended device address of key-pair.

  \param[in] handle - valid key handle

  \return Pointer to device address or NULL if key-pair is not found.
******************************************************************************/
ExtAddr_t* APS_GetKeyPairDeviceAddress(const APS_KeyHandle_t handle)
{
  zsiProcessCommand(ZSI_SREQ_CMD, (APS_KeyHandle_t *)&handle, zsiSerializeAPS_GetKeyPairDeviceAddressReq,
                    &zsiDeviceAddr);

  return &zsiDeviceAddr;
}

/**************************************************************************//**
  \brief Wrapper routine to request next key handle.

  \param[in] handle - handle of previous key-pair or APS_KEY_PAIR_INITIAL
                      if it's initial call.

  \return if next key-pair is found then return valid key handle
          otherwise return APS_KEY_PAIR_FINISH.
******************************************************************************/
APS_KeyHandle_t APS_NextKeys(const APS_KeyHandle_t handle)
{
  APS_KeyHandle_t keyHandle;

  zsiProcessCommand(ZSI_SREQ_CMD, (APS_KeyHandle_t *)&handle, zsiSerializeAPS_NextKeysReq,
                    &keyHandle);

  return keyHandle;
}

/**************************************************************************//**
  \brief Wrapper routine to mark specified global key as 'TC key'. It will be used for communication
         with TC.

  \param[in] handle - valid key handle.

  \return None.
 ******************************************************************************/
void APS_MarkGlobalKeyAsTCKey(APS_KeyHandle_t handle)
{
  APS_KeyHandle_t keyHandle;
  keyHandle = handle;
  zsiProcessCommand(ZSI_SREQ_CMD, (void *const)&keyHandle, zsiSerializeAPS_MarkGlobalKeyAsTCKeyReq, NULL);
}

/**************************************************************************//**
  \brief (Un)authorize cryptographic key-pair of given device.

  \param[in] deviceAddress \ref Endian "[LE]" - pointer to extended IEEE
                                                device address.
  \param[in] status - 'true' for authorized keys otherwise 'false'.

  \return None.
 ******************************************************************************/
void APS_SetAuthorizedStatus(const ExtAddr_t *const deviceAddress, const bool status)
{
  ZsiSetAuthorizedStatus_t dataIn;

  COPY_EXT_ADDR(dataIn.extAddress, deviceAddress);
  dataIn.status = status;
  zsiProcessCommand(ZSI_SREQ_CMD, (void *const)&dataIn, zsiSerializeAPS_SetAuthorizedStatusReq, NULL);
}

/**************************************************************************//**
  \brief Sets default key pair status.

  \param[in] flag - new default status.

  \return None.
 ******************************************************************************/
void APS_SetDefaultKeyPairStatus(APS_KeyPairFlags_t flag)
{
  zsiProcessCommand(ZSI_SREQ_CMD, (void *const)&flag, zsiSerializeAPS_SetDefaultKeyPairStatusReq, NULL);
}

/**************************************************************************//**
  \brief Check authorization of crypthographic key-pair.

  \param[in] deviceAddress \ref Endian "[LE]" - pointer to extended
                                                IEEE device address.

  \return 'true' if key-pair is authorized otherwise 'false'.
 ******************************************************************************/
bool APS_AreKeysAuthorized(const ExtAddr_t *const deviceAddress)
{
  bool result;

  zsiProcessCommand(ZSI_SREQ_CMD, (void *const)deviceAddress, zsiSerializeAPS_AreKeysAuthorizedReq, &result);

  return result;
}

/**************************************************************************//**
  \brief Get a pointer to the Trust Center address

  \return pointer to the Trust Center extended address
 ******************************************************************************/
ExtAddr_t* APS_GetTrustCenterAddress(void)
{
  ZsiEntityService_t service;

  zsiProcessCommand(ZSI_SREQ_CMD, &service, zsiSerializeAPS_GetTrustCenterAddressReq,
                    &zsiTcAddr);

  return &zsiTcAddr;
}

#endif /* _LINK_SECURITY_ */

#elif defined(ZAPPSI_NP)

/********************************************************************
\brief Application endpoint indication handler wrapper

\param[in] ind - APS Data Indication primitive pointer

\return None.
********************************************************************/
void APS_DataInd(APS_DataInd_t *ind)
{
  zsiProcessCommand(ZSI_AREQ_CMD, ind, zsiSerializeAPS_DataInd, NULL);
}

/**************************************************************************//**
\brief APS-Data.Confirm primitive wrapper routine.

\param[in] conf - confirm parameters.

\return None.
******************************************************************************/
void zsiAPS_DataConf(APS_DataConf_t *conf)
{
  APS_DataReq_t *const req =
    GET_PARENT_BY_FIELD(APS_DataReq_t, confirm, conf);
  zsiProcessCommand(ZSI_AREQ_CMD, req, zsiSerializeAPS_DataConf, NULL);
}

/**************************************************************************//**
  \brief Allocate memory for new endpoint.

  \return Pointer to memory if allocated.
 ******************************************************************************/
ZsiEndpointDescriptor_t *zsiApsAllocateEndpointMemory(void)
{
  uint8_t it;

  for (it = 0; it < ZSI_MAX_ENDPOINTS_AMOUNT; it++)
  {
    if (!zsiEndpoint[it].busy)
    {
      zsiEndpoint[it].busy = true;
      return &zsiEndpoint[it];
    }
  }

  return NULL;
}
#endif /* ZAPPSI_NP */

/* eof zsiAps.c */
