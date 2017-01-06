/**************************************************************************//**
  \file networkJoining.h

  \brief
    Application network joining declaration.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    23.11.12 N. Fomin - Created.
******************************************************************************/
#ifndef NETWORK_JOINING_H_
#define NETWORK_JOINING_H_

/******************************************************************************
                    Prototypes section
******************************************************************************/
/**************************************************************************//**
\brief Performs network joining via association or linking

\param[in] doneCb - callback which is called on the end of start network procedure
******************************************************************************/
void startNetwork(void (*doneCb)(void));

/**************************************************************************//**
\brief Performs network joining via association or linking
******************************************************************************/
void startNetworkHandler(void);

#endif // NETWORK_JOINING_H_
// eof networkJoining.h