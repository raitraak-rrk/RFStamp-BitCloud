/*********************************************************************************************//**
\file  

\brief 

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal

$Id$

***************************************************************************************************/

// Make sure N_FSM_ENABLE_LOGGING is defined
#if !defined(N_FSM_ENABLE_LOGGING)
#  define N_FSM_ENABLE_LOGGING
#endif

#define N_FSM_NO_LOG_WARNING

// Include the actual code, which will expand to the logging version
#include "N_Fsm.c"
