/**************************************************************************//**
  \file apsRejectDuplicate.h

  \brief Private interface of the duplicate rejection table.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2011-07-03 Max Gekk - Created.
   Last change:
    $Id: apsRejectDuplicate.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#if !defined _APS_REJECT_DUPLICATE_H
#define _APS_REJECT_DUPLICATE_H
/******************************************************************************
                                Includes section
 ******************************************************************************/
#include <apsCommon.h>

/******************************************************************************
                               Prototypes section
 ******************************************************************************/
/*****************************************************************************
  \brief Is incoming packet is dublicate or not? Checks in the rejection table.

  \param[in] srcAddr - short address of source node.
  \param[in] counter - APS counter of incoming packet.

  \return true - if incoming packet is duplicate otherwise return false.
*****************************************************************************/
APS_PRIVATE bool apsIsDuplicatePacket(const ShortAddr_t srcAddr,
  const uint8_t counter);

/******************************************************************************
  \brief Reset the duplicate rejection table ana reinit timeouts.
 ******************************************************************************/
APS_PRIVATE void apsResetDuplicateRejectionTable(void);

#endif /* _APS_REJECT_DUPLICATE_H */
/** eof apsRejectDuplicate.h */

