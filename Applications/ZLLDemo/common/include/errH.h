/**************************************************************************//**
  \file errH.h

  \brief
    Errors proccesing declaration.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    21.11.12 N. Fomin - Created.
******************************************************************************/
#ifndef ERRH_H
#define ERRH_H

/******************************************************************************
                    Includes section
******************************************************************************/
#include <N_ErrH.h>
#include <errH.h>

/******************************************************************************
                    Definitions section
******************************************************************************/
#define ERROR_HANDLERS_MAX_AMOUNT 5u

/******************************************************************************
                    Prototypes section
******************************************************************************/
void errHFatal(const char *compId, uint16_t line);

/******************************************************************************
                    Types section
******************************************************************************/
typedef void (*ErrorAbortCb_t)(const char *compId, uint16_t line);

/******************************************************************************
                    Prototypes section
******************************************************************************/
/**************************************************************************//**
\brief Initializes errH module
******************************************************************************/
void errHInit(void);

/**************************************************************************//**
\brief Subscribes for fatal error

\param[in] handler - callback function which is called is case of error
******************************************************************************/
void errHSubscribeForFatal(ErrorAbortCb_t handler);

#endif // ERRH_H
/* eof errH.h */
