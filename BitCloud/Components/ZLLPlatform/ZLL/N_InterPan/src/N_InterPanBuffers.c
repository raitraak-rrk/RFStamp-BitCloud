/**************************************************************************//**
  \file N_InterPanBuffers.c

  \brief
    InterPan buffers implementation.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    26.08.12 N. Fomin - Created.
******************************************************************************/

/******************************************************************************
                    Includes section
******************************************************************************/
#include <N_InterPanBuffers.h>
#include <sysUtils.h>
#include <apsCommon.h>
#include <dbg.h>
#include <N_ErrH.h>

/******************************************************************************
                    Definitions section
******************************************************************************/
#define INTERPAN_BUFFERS_AMOUNT 5
#define COMPID "N_InterPanBuffers"

/******************************************************************************
                    Types section
******************************************************************************/
BEGIN_PACK
typedef struct
{
  BOTTOM_GUARD
  // Auxilliary header (stack required)
  uint8_t header[APS_MAX_INTERPAN_HEADER_LENGTH];
  // Actually application message (For InterPan messages MAC payload is used directly)
  uint8_t msg[APS_MAX_INTERPAN_ASDU_SIZE];
  TOP_GUARD
} PACK MessageBuffer_t;
END_PACK

typedef struct
{
  INTRP_DataReq_t req;
  MessageBuffer_t buffer;
  bool busy;
} InterPanBuffer_t;

/******************************************************************************
                    Local variables
******************************************************************************/
static InterPanBuffer_t interPanRequestPool[INTERPAN_BUFFERS_AMOUNT];

/******************************************************************************
                    Implementation section
******************************************************************************/
/**************************************************************************//**
\brief Initializes InterPan buffers
******************************************************************************/
void initInterPanBuffers(void)
{}

/**************************************************************************//**
\brief Gets free InterPan buffer

\returns pointer to buffer if it is available, NULL otherwise
******************************************************************************/
INTRP_DataReq_t *getFreeInterPanBuffer(void)
{
  for (uint8_t i = 0; i < INTERPAN_BUFFERS_AMOUNT; i++)
    if (!interPanRequestPool[i].busy)
    {
      interPanRequestPool[i].busy = true;
      INIT_GUARDS(&interPanRequestPool[i].buffer);
      return &interPanRequestPool[i].req;
    }

  return NULL;
}

/**************************************************************************//**
\brief Frees buffer

\param[in] req - buffer to be free
******************************************************************************/
void freeInterPanBuffer(INTRP_DataReq_t *req)
{
  InterPanBuffer_t *buffer = GET_PARENT_BY_FIELD(InterPanBuffer_t, req, req);
  
#if defined _SYS_ASSERT_ON_
  N_ERRH_ASSERT_FATAL(TOP_GUARD_VALUE == buffer->buffer.topGuard && 
                      BOTTOM_GUARD_VALUE == buffer->buffer.bottomGuard);
#endif
  buffer->busy = false;
}

/**************************************************************************//**
\brief Gets buffer for data for interPan request

\param[in] req - pointer to request for which buffers is allocated

\returns pointer to buffer for interPan data
******************************************************************************/
uint8_t *getDataBuffer(INTRP_DataReq_t *req)
{
  if (!req)
    return NULL;

  InterPanBuffer_t *buffer = GET_PARENT_BY_FIELD(InterPanBuffer_t, req, req);

  return buffer->buffer.msg;
}

// eof N_InterPanBuffers.c
