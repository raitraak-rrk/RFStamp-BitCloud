/**************************************************************************//**
  \file  apsSkkeInitiator.h

  \brief Private interface of APS SKKE Initiator.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2011-04-24 Max Gekk - Created.
   Last change:
    $Id: apsSkkeInitiator.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#if !defined _APS_SKKE_INITIATOR_H
#define _APS_SKKE_INITIATOR_H

/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <nwk.h>

#if defined _HIGH_SECURITY_
/******************************************************************************
                              Prototypes section
 ******************************************************************************/
/**************************************************************************//**
  \brief Main task handler of SKKE initiator.
 ******************************************************************************/
APS_PRIVATE void apsSkkeInitiatorTaskHandler(void);

/**************************************************************************//**
  \brief Process SKKE-2 command frame from a responder.

  \param[in] ind - pointer to NWK parameters of received SKKE-2 frame.
  \return None.
 ******************************************************************************/
APS_PRIVATE void apsISkke2Ind(NWK_DataInd_t *ind);

/**************************************************************************//**
  \brief Process SKKE-4 command frame from a responder.

  \param[in] ind - pointer to NWK parameters of received SKKE-4 frame.
  \return None.
 ******************************************************************************/
APS_PRIVATE void apsISkke4Ind(NWK_DataInd_t *ind);

#else /* no _HIGH_SECURITY_ */


#define apsSkkeInitiatorTaskHandler NULL

#endif /* _HIGH_SECURITY_ */

#endif /* _APS_SKKE_INITIATOR_H */
/** eof apsSkkeInitiator.h */

