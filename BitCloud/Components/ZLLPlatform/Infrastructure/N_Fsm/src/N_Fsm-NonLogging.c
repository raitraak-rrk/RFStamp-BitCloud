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

// Make sure N_FSM_ENABLE_LOGGING is not defined
#if defined(N_FSM_ENABLE_LOGGING)
#  undef N_FSM_ENABLE_LOGGING
#endif

// Include the actual code, which will expand to the non-logging version
#include "N_Fsm.c"
