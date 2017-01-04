/************************************************************************//**
  \file zclSecurityManager.c

  \brief
    The ZCL Security Manager implementation

    The file implements the ZCL Security Manager

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    18.03.09 A. Potashov - Created.
******************************************************************************/
#if ZCL_SUPPORT == 1
#if(defined _LINK_SECURITY_) && (!defined _LIGHT_LINK_PROFILE_)

/******************************************************************************
                   Includes section
******************************************************************************/
#include <aps.h>
#include <appFramework.h>
#include <zclSecurityManager.h>
#include <zclKeyEstablishmentCluster.h>
#include <sysUtils.h>
#include <configServer.h>
#include <zclTaskManager.h>
#include <zclDbg.h>
#include <sysAssert.h>
#include <clusters.h>
#include <zdo.h>

#define ZCL_SM_INVALID_SHORT_ADDRESS       0xffff
#define ZCL_SM_INVALID_EXT_ADDRESS         0xffffffffffffffffLL
#define ZCL_SM_KE_INIT_TIMEOUT             5000

#if CERTICOM_SUPPORT == 1
  #ifndef ZCL_SM_PERMISSION_TABLE_SIZE
    #define ZCL_SM_PERMISSION_TABLE_SIZE          0x05
  #endif
#endif // CERTICOM_SUPPORT == 1


#if CERTICOM_SUPPORT == 1
typedef enum
{
  ZCL_SM_TC_NOT_AUTHENTICATED_STATUS    = 0x00,
  ZCL_SM_TC_AUTHENTICATION_STARTED      = 0x01,
  ZCL_SM_TC_AUTHENTICATION_FAILED       = 0x02,
  ZCL_SM_TC_AUTHENTICATED_STATUS        = 0x03,
  ZCL_SM_TC_LINK_KEY_UPDATE_REQUIRED    = 0x04,
} ZclSmTcAuthenticateStatus_t;

typedef struct
{
  ShortAddr_t                   shortAddr;
  ExtAddr_t                     extAddr;
  Endpoint_t                    keEndpoint;
  ZclSmTcAuthenticateStatus_t   authenticateStatus;
  uint16_t                      linkKeyTimeout;     //in minutes (45 days as maximum)
} ZclSmPTEntry_t;
#endif // CERTICOM_SUPPORT == 1

#if CERTICOM_SUPPORT == 1
extern Endpoint_t   keLocalEndpoint;
extern ZCL_KECertificateDescriptor_t keCertificateDescriptor;

//API
extern void keReset(void);
extern bool keInitCluster(ExtAddr_t *remoteAddress);
extern bool keStartKE(void);
#endif // CERTICOM_SUPPORT == 1

static ZCL_StartSecurityReq_t *smStartReq;

/******************************************************************************
                          Prototypes section
******************************************************************************/
static void smMakeStartConf(void);

/******************************************************************************
                           Implementations section
******************************************************************************/
/**************************************************************************//**
  \brief Security task handler of ZCL layer to handle start request with gap.
******************************************************************************/
void zclSecurityTaskHandler(void)
{
  smMakeStartConf();
}

/***************************************************************************//**
\brief Reset ZCL security module activity.
********************************************************************************/
void ZCL_ResetSecurity(void)
{
  smStartReq = NULL;
#if CERTICOM_SUPPORT == 1
  keReset();
  APS_SetDefaultKeyPairStatus(APS_FLAG_NONE);
#endif
}

#if CERTICOM_SUPPORT == 1
/***************************************************************************//**
  \brief Notifies that Key Establishment is finished with status specified in parameters.

  \param status - status of Key Establishment.
********************************************************************************/
void keNotification(ZCL_SecurityStatus_t status)
{
  if (smStartReq)
  {
    if (ZCL_SECURITY_STATUS_INVALID_SETTINGS >= status)
    {
      smStartReq->confirm.status = status;
      zclPostTask(ZCL_SECURITY_TASK_ID);
    }
  }
}
#endif

/***************************************************************************//**
  \brief Start ZCL with certain security settings.

  \param req - pointer to security start request structure.
********************************************************************************/
void ZCL_StartSecurityReq(ZCL_StartSecurityReq_t *req)
{
  if (NULL == req)
  {
    SYS_E_ASSERT_ERROR(req, ZCL_STARTSECURITY_NULLREQUEST);
    return;
  }
  else if (NULL == req->ZCL_StartSecurityConf)
  {
    SYS_E_ASSERT_ERROR(req->ZCL_StartSecurityConf, ZCL_STARTSECURITY_NULLCONFIRM);
    return;
  }


  smStartReq = req;

#if CERTICOM_SUPPORT == 1
  if (!req->remoteAddress)
    COPY_EXT_ADDR(req->remoteAddress, *APS_GetTrustCenterAddress());
  if (!keInitCluster(&req->remoteAddress) || !keStartKE())
  {
    smStartReq->confirm.status = ZCL_SECURITY_STATUS_INVALID_SETTINGS;
    zclPostTask(ZCL_SECURITY_TASK_ID);
  }

  if (MAC_IsOwnExtAddr(&req->remoteAddress))
#endif // CERTICOM_SUPPORT == 1
  {
    smStartReq->confirm.status = ZCL_SECURITY_STATUS_SUCCESS;
    zclPostTask(ZCL_SECURITY_TASK_ID);
  }
}

/***************************************************************************//**
\brief Confirmation about ZCL security start.

\param req - pointer to security start request structure.
********************************************************************************/
static void smMakeStartConf(void)
{
  if (smStartReq)
  {
    void (*cf)(ZCL_StartSecurityConf_t *conf) = smStartReq->ZCL_StartSecurityConf;
    ZCL_StartSecurityConf_t *conf = &smStartReq->confirm;
    smStartReq = NULL;
    if (NULL == cf)
    {
      SYS_E_ASSERT_FATAL(false, ZCL_STARTSECURITY_NULLCALLBACK0);
    }
    else
    {
      cf(conf);
    }
  }
}

void ZCL_Set(ZCL_Set_t *req)
{
  req->status = ZCL_SUCCESS_STATUS;

  switch (req->attr.id)
  {
    case ZCL_LINK_KEY_DESC_ID:
#ifdef _PERMISSIONS_
      //Add to TC Permission Table
      if (false == TC_AddDeviceToPermissionTable(&req->attr.value.linkKeyDesc->addr))
        req->status = ZCL_TC_PERMISSION_TABLE_ERROR_STATUS;
#endif //#ifdef _PERMISSIONS_

      if (ZCL_SUCCESS_STATUS == req->status)
      {
        ExtAddr_t *addr =
          (ExtAddr_t*)GET_FIELD_PTR(req->attr.value.linkKeyDesc, ZCL_LinkKeyDesc_t, addr);

        if (!APS_KEY_HANDLE_IS_VALID(APS_SetLinkKey(addr, req->attr.value.linkKeyDesc->key)))
        {
          req->status = ZCL_APS_LINK_KEY_ERROR_STATUS;
        }
#ifdef _TC_PROMISCUOUS_MODE_
        else
        {
          APS_SetPreinstalledLinkKey(addr, req->attr.value.linkKeyDesc->key);
        }
#endif /* _TC_PROMISCUOUS_MODE_ */
      }
      break;


    case ZCL_REMOVE_LINK_KEY_ID:
      break;


#if CERTICOM_SUPPORT == 1
    case ZCL_KE_ACTIVE_ENDPOINT_ID:
      if ((APS_MIN_USER_ENDPOINT > req->attr.value.endpoint) ||
          (APS_MAX_USER_ENDPOINT < req->attr.value.endpoint))
      {
        req->status = ZCL_INVALID_ATTRIBUTE_VALUE_STATUS;
      }
      else
      {
        keLocalEndpoint = req->attr.value.endpoint;
      }
      break;

    case ZCL_CERTIFICATE_DESC_ID:
      if (NULL == req->attr.value.certificateDesc)
      {
        req->status = ZCL_INVALID_ATTRIBUTE_VALUE_STATUS;
      }
      else
      {
        SYS_BYTE_MEMCPY(&keCertificateDescriptor, req->attr.value.certificateDesc, sizeof(ZCL_KECertificateDescriptor_t));
      }
      break;
#endif // CERTICOM_SUPPORT == 1

    case ZCL_NETWORK_KEY_ID:
      NWK_SetKey(req->attr.value.networkKey, NWK_STARTUP_ACTIVE_KEY_SEQUENCE_NUMBER);
      NWK_ActivateKey(NWK_STARTUP_ACTIVE_KEY_SEQUENCE_NUMBER);
      break;

    default:
      req->status = ZCL_UNSUPPORTED_ATTRIBUTE_STATUS;
      break;
  }
}

void ZCL_Get(ZCL_Get_t *req)
{
  uint8_t *key;
  req->status = ZCL_SUCCESS_STATUS;
  req->attr.length = 0;

  switch (req->attr.id)
  {
    case ZCL_LINK_KEY_DESC_ID:
      {
        ExtAddr_t addr = *(ExtAddr_t*)GET_FIELD_PTR(req->attr.value.linkKeyDesc, ZCL_LinkKeyDesc_t, addr);
        const APS_KeyHandle_t apsKeyHandle = APS_FindKeys(&addr);

        key = APS_KEYS_FOUND(apsKeyHandle)? APS_GetLinkKey(apsKeyHandle): NULL;
        if (NULL == key)
        {
          req->status = ZCL_INVALID_ATTRIBUTE_VALUE_STATUS;
        }
        else
        {
          SYS_BYTE_MEMCPY(req->attr.value.linkKeyDesc->key, key, 16);
          req->attr.length = sizeof(ZCL_LinkKeyDesc_t);
        }
        break;
      }

    case ZCL_REMOVE_LINK_KEY_ID:
      break;
#if CERTICOM_SUPPORT == 1
    case ZCL_KE_ACTIVE_ENDPOINT_ID:
      req->attr.value.endpoint = keLocalEndpoint;
      if (ZCL_KE_INVALID_ENDPOINT == req->attr.value.endpoint)
        req->status = ZCL_INVALID_ATTRIBUTE_VALUE_STATUS;
      break;

    case ZCL_CERTIFICATE_DESC_ID:
      if (NULL == req->attr.value.certificateDesc)
      {
        req->status = ZCL_INVALID_ATTRIBUTE_VALUE_STATUS;
      }
      else
      {
        SYS_BYTE_MEMCPY(req->attr.value.certificateDesc, &keCertificateDescriptor, sizeof(ZCL_KECertificateDescriptor_t));
      }
      break;

#endif // CERTICOM_SUPPORT == 1
    default:
      req->status = ZCL_UNSUPPORTED_ATTRIBUTE_STATUS;
      break;
  }
}

#if 0

#if CERTICOM_SUPPORT == 1
static void smMatchDescResp(ZDO_ZdpResp_t *zdpResp)
{
  ZDO_MatchDescResp_t *zdoMatchResp = &zdpResp->respPayload.matchDescResp;
  APS_BindReq_t apsBindReq;

  if (ZDO_SUCCESS_STATUS == zdpResp->respPayload.status)
  {
      srvKEEndpoint = zdoMatchResp->matchList[0];
      srvShortAddr = zdoMatchResp->nwkAddrOfInterest;
  }
  else if (ZDO_CMD_COMPLETED_STATUS == zdpResp->respPayload.status)
  {
    if ((APS_ZDO_ENDPOINT != srvKEEndpoint) &&
        (ZCL_SM_INVALID_SHORT_ADDRESS != srvShortAddr))
    {
      smMakeStartConf(ZCL_SECURITY_STATUS_DISCOVERY_FAIL);
    }
    else
    {
/*
typedef struct
{
  //Standard fields
  ExtAddr_t srcAddr;
  Endpoint_t srcEndpoint;
  ClusterId_t clusterId;
  APS_AddrMode_t dstAddrMode;
  APS_Address_t dstAddr;
  Endpoint_t dstEndpoint;
  //Confirm
  APS_BindConf_t confirm;
} APS_BindReq_t;
*/
      apsBindReq.srcAddr = *MAC_GetExtAddr();
      apsBindReq.srcEndpoint = clKEEndpoint;
      apsBindReq.clusterId = ZCL_KEY_ESTABLISHMENT_CLUSTER_ID;
      apsBindReq.dstAddrMode = APS_EXT_ADDRESS;
      COPY_EXT_ADDR(apsBindReq.dstAddr.extAddress, *APS_GetTrustCenterAddress());
      apsBindReq.dstEndpoint = srvKEEndpoint;
      APS_BindReq(&apsBindReq);
      if (APS_SUCCESS_STATUS != apsBindReq.confirm.status)
      {
        smMakeStartConf(ZCL_SECURITY_STATUS_APS_BINDING_FAIL);
      }
      else
      {
        keInitiateKE(0x00, apsBindReq.dstAddr.extAddress, srvKEEndpoint);
      }
    }
  }
}


static void smMakeKEWithTrustCenter(void)
{
  //It's supposed that TrustCenter is in the coordinator which has
  //short address equal to 0x0000
  //TBD: For other cases the complex service discovery needed

  smMakeMatchDescReq();
}


//Function make discovery for KE Cluster at the TrustCenter
//It's assumed that TrustCenter is located at Coordinator
static smMakeMatchDescReq(void)
{
  ZDO_MatchDescReq_t *zdoMatchDescReq = &zdpReq.req.reqPayload.matchDescReq;

  zdpReq.ZDO_ZdpResp = smMatchDescResp;
  zdpReq.reqCluster = MATCH_DESCRIPTOR_CLID;
  zdoMatchDescReq->nwkAddrOfInterest = 0x0000;
  zdoMatchDescReq->profileId = PROFILE_ID_SMART_ENERGY;
  zdoMatchDescReq->numInClusters = 1;
  zdoMatchDescReq->numOutClusters = 0;
  zdoMatchDescReq->inClusterList[0] = ZCL_KEY_ESTABLISHMENT_CLUSTER_ID;
  ZDO_ZdpReq(&zdpReq);
}


static void smZdoIeeeAddrResp(ZDO_ZdpResp_t *zdpResp)
{
  ZDO_IeeeAddrResp_t    *ieeeAddrResp = (ZDO_IeeeAddrResp_t *) &zdpResp->respPayload.ieeeAddrResp;
  APS_BindReq_t         apsReq;
  ZCL_SecurityStatus_t  status = ZCL_SUCCESS_STATUS;

  if (ZDO_SUCCESS_STATUS == zdpResp->respPayload.status)
  {
    srvExtAddr = ieeeAddrResp->ieeeAddrRemote;
    COPY_EXT_ADDR(apsReq.srcAddr, *MAC_GetExtAddr());
    apsReq.srcEndpoint = clKEEndpoint;
    apsReq.clusterId = ZCL_KEY_ESTABLISHMENT_CLUSTER_ID;
    apsReq.dstAddrMode = APS_EXT_ADDRESS;
    apsReq.dstAddr.extAddress = ieeeAddrResp->ieeeAddrRemote;
    apsReq.dstEndpoint = clKEEndpoint;
    APS_BindReq(&apsReq);

    keSendInitiateKECommand();
  }
  else
  {
    status = ZCL_SECURITY_STATUS_DISCOVERY_FAIL;
  }
  smMakeStartConf(status);
}


static void smMakeIeeeAddrReq(ShortAddr_t shortAddr)
{
  ZDO_IeeeAddrReq_t *zdoIeeeAddrReq = &zdpReq.req.reqPayload.ieeeAddrReq;

  zdpReq.ZDO_ZdpResp = smZdoIeeeAddrResp;
  zdpReq.reqCluster = IEEE_ADDR_CLID;

  zdoIeeeAddrReq->nwkAddrOfInterest = shortAddr;

  zdoIeeeAddrReq->reqType = 0;
  zdoIeeeAddrReq->startIndex = 0;

  ZDO_ZdpReq(&zdpReq);
}
#endif // CERTICOM_SUPPORT == 1



void smTimoutHandler(void)
{
  smMakeStartConf(ZCL_SECURITY_STATUS_TIMEOUT);
}

void smSetTimeout(uint32_t timeout)
{
  apsInitAppTimer(&smApsTimer, TIMER_ONE_SHOT_MODE, timeout, smTimoutHandler);
  apsStartTimer(&smApsTimer);
}


static void smMakeKE(ZclSmPTEntry_t *permissionTableEntry)
{
  if (NULL == currentPermissionTableEntry)
  {
    currentPermissionTableEntry = permissionTableEntry;
  }

}







/***
CS_GetMemory(CS_TC_PERMISSION_TABLE_ID, (void*)&permissionDeviceTable);
CS_ReadParameter(CS_MAX_TC_ALLOWED_DEVICES_AMOUNT_ID, &maxAllowedDeviceAmount);
***/

static ZclSmPTEntry_t *smGetFromPermissionTable(ShortAddr_t *shortAddr, ExtAddr_t *extAddr)
{
  uint8_t i;
  uint8_t j = 0;
  uint8_t k;
  ShortAddr_t shortAddr_;
  ExtAddr_t   extAddr_;

  if (NULL != shortAddr)
  {
    j |= (1 << 0);
    shortAddr_ = *shortAddr;
  }
  if (NULL != extAddr)
  {
    j |= (1 << 1);
    COPY_EXT_ADDR(extAddr_, *extAddr);
  }

  if (0 != j)
  {
    for (i = 0; i < ZCL_SM_PERMISSION_TABLE_SIZE; i++)
    {
      k = 0;
      if (0 != (j & (1 << 0)))
      {
        if (smTcPermissionTable[i].shortAddr == shortAddr_)
          k |= (1 << 0);
        if (IS_EQ_EXT_ADDR(smTcPermissionTable[i].extAddr, extAddr_))
          k |= (1 << 1);
      }
      if (k == j)
        return &smTcPermissionTable[i];
    }
  }
  return NULL;
}

/*
ZclSmPermissionTableEntry_t *smAddToPermissionTable(ShortAddr_t shortAdddr, ExtAddr_t extAddr)
{
  ZclSmPermissionTableEntry_t *result = NULL;

  result = smGetFromPermissionTable(shortAddr, extAddr);
  if (NULL == result)
    result = smGetFromPermissionTable(ZCL_SM_INVALID_SHORT_ADDRESS, ZCL_SM_INVALID_EXT_ADDRESS);
  if (NULL != result)
  {
    result->shortAddr = shortAddr;
    result->extAddr = extAddr;
  }
  return result;
}
*/

#endif //#if 0

#else // (defined _LINK_SECURITY_) && (!defined _LIGHT_LINK_PROFILE_)
void zclSecurityTaskHandler(void)
{}
#endif // (defined _LINK_SECURITY_) && (!defined _LIGHT_LINK_PROFILE_)
#endif // ZCL_SUPPORT == 1

//eof zclSecurityManager.c
