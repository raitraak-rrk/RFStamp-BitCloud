/**************************************************************************//**
  \file N_InterPanBuffers.h

  \brief
    InterPan buffers interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    26.08.12 N. Fomin - Created.
******************************************************************************/
#ifndef _INTERPAN_BUFFERS_H
#define _INTERPAN_BUFFERS_H

/******************************************************************************
                    Includes section
******************************************************************************/
#include <intrpData.h>

/******************************************************************************
                    Prototypes section
******************************************************************************/
/**************************************************************************//**
\brief Initializes InterPan buffers
******************************************************************************/
void initInterPanBuffers(void);

/**************************************************************************//**
\brief Gets free InterPan buffer

\returns pointer to buffer if it is available, NULL otherwise
******************************************************************************/
INTRP_DataReq_t *getFreeInterPanBuffer(void);

/**************************************************************************//**
\brief Frees buffer

\param[in] req - buffer to be free
******************************************************************************/
void freeInterPanBuffer(INTRP_DataReq_t *req);

/**************************************************************************//**
\brief Gets buffer for data for interPan request

\param[in] req - pointer to request for which buffers is allocated

\returns pointer to buffer for interPan data
******************************************************************************/
uint8_t *getDataBuffer(INTRP_DataReq_t *req);

#endif // _INTERPAN_BUFFERS_H

// eof N_InterPanBuffers.h
