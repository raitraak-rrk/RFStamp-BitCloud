/**************************************************************************//**
  \file apsConfig.h

  \brief Configuration constants and definitions of APS layer.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2011-07-03 Max Gekk - Created.
   Last change:
    $Id: apsConfig.h 27584 2015-01-09 14:45:42Z unithra.c $
******************************************************************************/
#if !defined _APS_CONFIG_H
#define _APS_CONFIG_H

/******************************************************************************
                             Definitions section
 ******************************************************************************/
/* Rejection table timeouts counted in 500ms intervals.
 * So we can use narrow data types. */
#define APS_DUPLICATE_REJECTION_INTERVAL 512U

#ifdef _APS_FRAGMENTATION_
  #if defined _CERTIFICATION_
    #define APS_INTERFRAME_DELAY (10*HAL_APPTIMERINTERVAL)
  #else
    #define APS_INTERFRAME_DELAY (HAL_APPTIMERINTERVAL)
  #endif
#endif

/* Random delay for retransmission to decrease amount of collisions */
#define RETRANSMISSION_RANDOM_DELAY_RANGE 0x1ff

#define RETRANSMISSION_DELAY_WITH_NO_PHY_ACK 1000U

#endif /* _APS_CONFIG_H */
/** eof apsConfig.h */

