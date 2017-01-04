/**************************************************************************//**
  \file nwkPermitJoining.h

  \brief NWK permit joining header file.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2007-06-27 V. Panov - Created.
    2008-12-10 M. Gekk  - Reduction of usage of the RAM at separate compiling.
   Last change:
    $Id: nwkPermitJoining.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#if !defined _NWK_PERMIT_JOINING_H
#define _NWK_PERMIT_JOINING_H

/******************************************************************************
                              Includes section
 ******************************************************************************/
#include <nwkConfig.h>
#include <sysQueue.h>
#include <mac.h>
#include <nlmePermitJoining.h>
#include <appTimer.h>

/******************************************************************************
                               Types section
 ******************************************************************************/
/** Internal states of NLME-PERMIT_JOINING component */
typedef enum _NwkPermitJoiningStates_t
{
  NWK_PERMIT_JOINING_IDLE_STATE = 0x48,
  NWK_PERMIT_JOINING_FIRST_STATE = 0x4A,
  NWK_PERMIT_JOINING_WAIT_TIMER_FIRED_STATE = NWK_PERMIT_JOINING_FIRST_STATE,
  NWK_PERMIT_JOINING_BEGIN_STATE = 0x5D,
  NWK_PERMIT_JOINING_CONFIRM_STATE = 0x5E,
  NWK_PERMIT_JOINING_TIMER_FIRED_STATE = 0x5F,
  NWK_PERMIT_JOINING_SET_ASSOC_PERMIT_STATE = 0x60,
  NWK_PERMIT_JOINING_LAST_STATE
} NwkPermitJoiningState_t;

/** Internal parameters of NLME-PERMIT-JOINING component. */
typedef struct _NwkPermitJoining_t
{
  NwkPermitJoiningState_t state;
  QueueDescriptor_t       queue;
  MAC_SetReq_t            macReq;
  HAL_AppTimer_t          timer;
} NwkPermitJoining_t;

/******************************************************************************
                         Inline functions section
 ******************************************************************************/
#if defined _ROUTER_ || defined _COORDINATOR_
/*************************************************************************//**
  \brief Task handler of the NLME-PERMIT-JOINING component.
 *****************************************************************************/
NWK_PRIVATE void nwkPermitJoiningTaskHandler(void);

/*************************************************************************//**
  \brief Reset the NLME-PERMIT-JOINING component.
 *****************************************************************************/
NWK_PRIVATE void nwkResetPermitJoining(void);

/******************************************************************************
  \brief nwkPermitJoining idle checking.

  \return true, if nwkPermitJoining performs no activity, false - otherwise.
 ******************************************************************************/
NWK_PRIVATE bool nwkPermitJoiningIsIdle(void);

#else /* _ROUTER_ or _COORDINATOR_ */

#define nwkPermitJoiningTaskHandler NULL
#define nwkPermitJoiningIsIdle NULL
#define nwkResetPermitJoining() (void)0

#endif /* _ROUTER_ or _COORDINATOR_ */
#endif /* _NWK_PERMIT_JOINING_H */
/** eof nwkPermitJoining.h */

