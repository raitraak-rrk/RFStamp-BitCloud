/********************************************************************************//**
  \file sspDbg.h

  \brief

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
**********************************************************************************/
#ifndef _SSPDBGH
#define _SSPDBGH

#include <dbg.h>

typedef enum
{
  SSPSFPHANDLERC_SSPENCRYPTFRAMEREQHANDLER_00 = 0x7800,
  SSPMANAGER_TASKHANDLER_00                   = 0x7801,
  SSPMANAGER_TASKHANDLER_01                   = 0x7802,
  SSPCCM_SSPAESCONF_00                        = 0x7803,
  SSPSFPHANDLERC_CCMCONFIRM_00                = 0x7804,
  SSPHASHHANDLERC_BCBHASHCONF_00              = 0x7805,
  SSPSFPHANDLERC_SSPKEYEDHASHMACCONFIRM_00    = 0x7806,
  SSPSKKEHANDLERC_HASHCONFIRM_00              = 0x7807,
  SSPMANAGER_SENDREQTOHANDLER_00              = 0x7808,
  SSPHASHHANDLERC_SSPBCBHASH_0                = 0x7809,
  SSPCCM_SSPAESCONFWHILEMICCHECK_00           = 0x780a,
  SSPCCM_SSPAESCONFWHILEMICCHECK_01           = 0x780b,
  SSPCCM_SSPAESCONFWHILEMICCHECK_02           = 0x780c,
  SSPCCMHANDLER_DETECTNEXTSTATE_0             = 0x780d,
  SSPCCMHANDLER_SSPAESCONF_00                 = 0x780e,
  SPP_CCM_NULLCALLBACK0                       = 0x780F,
  SPP_CCM_NULLCALLBACK1                       = 0x7810,
  SSP_CALCULATEAUTHENTIC_NULLCALLBACK0        = 0x781A,
  SSP_KEYEDHASHMAC_NULLCALLBACK0              = 0x781B,
  SSP_AESENCRYPT_NULLCALLBACK0                = 0x781C,
  SSPMANAGER_MUTEXLOCKCONF_00                 = 0x781E,
  SSPMANAGER_SSPCCMREQHANDLER_00              = 0x781F
} SspDbgCodeId_t;
#endif // _SSPDBGH
