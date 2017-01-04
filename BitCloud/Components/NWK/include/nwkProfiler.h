/**************************************************************************//**
 \file nwkProfiler.h

 \brief Interface of NWK Profiler.

 \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2010-10-29 Max Gekk - Created
   Last change:
    $Id: nwkProfiler.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#if !defined _NWK_PROFILER_H
#define _NWK_PROFILER_H

/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <sysTypes.h>

#if defined _PROFILER_
/******************************************************************************
                                Types section
 ******************************************************************************/
/* NWK Profiling information. */
BEGIN_PACK
typedef struct PACK _NWK_ProfilingInfo_t
{
  /** Maximum number of simultaneous join transaction. */
  uint8_t maxJoinTrans;
  /** Current number of processing join requests. */
  uint8_t incomingJoinReq;
  /** Current number of pending buffer for join response command. */
  uint8_t pendingJoinRespBuffers;
  /** Total NWK buffers. */
  uint8_t nwkBuffersAmount;
  /** Current number of busy output command buffers. */
  uint8_t outCmdBuffers;
  /** Total number of passive acks. */
  uint8_t passiveAckAmount;
  /** Used passive acks amount. */
  uint8_t usedPassiveAcks;
} NWK_ProfilingInfo_t;
END_PACK

/******************************************************************************
                           Global variables section
 ******************************************************************************/
extern NWK_ProfilingInfo_t nwkProfilingInfo;

/******************************************************************************
                              Prototypes section
 ******************************************************************************/
/**************************************************************************//**
  \brief Collect profiling information and update nwkProfInfo.
 ******************************************************************************/
void NWK_CollectProfilingInfo(void);

#endif /* _PROFILER_ */
#endif /* _NWK_PROFILER_H */
/** eof nwkProfiler.h */

