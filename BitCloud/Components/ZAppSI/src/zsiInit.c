/***************************************************************************//**
  \file zsiInit.c

  \brief ZAppSI initialization module.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2011-08-26  A. Razinkov - Created.
   Last change:
    $Id: zsiInit.c 25629 2013-11-19 03:40:22Z viswanadham.kotla $
 ******************************************************************************/

/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <zsiInit.h>
#include <zsiNotify.h>
#include <sysTaskManager.h>
#include <sysInit.h>
#include <zsiTaskManager.h>
#include <zsiMemoryManager.h>
#include <zsiSerialController.h>
#include <zsiDriver.h>
#include <zsiApsSerialization.h>
#if ZCL_SUPPORT == 1
#include <zcl.h>
#endif // ZCL_SUPPORT == 1

/******************************************************************************
                        Prototypes section
******************************************************************************/
#ifdef ZAPPSI_HOST
static void zsiInitResetNetworkConf(ZDO_ResetNetworkConf_t *conf);
#endif
/******************************************************************************
                        Static variables section
******************************************************************************/
static ZDO_ResetNetworkReq_t resetNetworkReq;

/******************************************************************************
                              Implementations section
******************************************************************************/

/******************************************************************************
  \brief ZAppSI initialization handler
 ******************************************************************************/
void zsiInitTaskHandler(void)
{
  CS_Init();
  SYS_InitZsiLayer();

#ifdef ZAPPSI_HOST
  resetNetworkReq.ZDO_ResetNetworkConf = zsiInitResetNetworkConf;
  ZDO_ResetNetworkReq(&resetNetworkReq);
#else
  (void)resetNetworkReq;
#endif // ZAPPSI_HOST
}

#ifdef ZAPPSI_HOST
/******************************************************************************
  \brief ZAppSI NP reset confirm handler.
 ******************************************************************************/
void zsiInitResetNetworkConf(ZDO_ResetNetworkConf_t *conf)
{
  sysAssert(ZDO_SUCCESS_STATUS == conf->status, ZSIINIT_ZSIINITRESETNETWORKCONF0);

  SYS_InitZclLayer();
  SYS_PostTask(APL_TASK_ID);
}
#endif
/******************************************************************************
  \brief ZAppSI initialization routine.
 ******************************************************************************/
void ZSI_Init(void)
{
  zsiInitNotifications();
  zsiResetTaskManager();
  zsiResetMemoryManager();
  zsiResetSerial();
  zsiResetDriver();
#ifdef ZAPPSI_HOST
  zsiResetQueues();
#endif
  /* Add handshake mechanism which allows the host processor and the network processor to tune their
       timeouts, interfaces, interfaces parameters, etc. */
#ifdef ZAPPSI_HOST
  zsiSynchronizeStartupParameters();
#endif /* ZAPPSI_HOST */
}
#ifdef ZAPPSI_HOST
/**************************************************************************//**
  \brief Resets all zsi related queues

  \return None.
 ******************************************************************************/
void zsiResetQueues(void)
{
  zsiApsclearEndPointQueue();
}
#endif
/* eof zsiInit.c */
