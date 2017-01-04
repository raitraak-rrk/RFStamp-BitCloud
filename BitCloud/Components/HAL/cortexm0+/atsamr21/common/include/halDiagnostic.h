/**************************************************************************//**
  \file  halDiagnostic.h

  \brief Implementation of diagnostics defines.

  \author
      Atmel Corporation: http://www.atmel.com \n
      Support email: avr@atmel.com

    Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
    Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    07/04/14 karthik.p_u - Created
 ******************************************************************************/

#ifndef _HALDIAGNOSTIC_H
#define _HALDIAGNOSTIC_H

/******************************************************************************
                   Includes section
******************************************************************************/
#include <halDbg.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
#if defined (MEASURE)
    #define BEGIN_MEASURE
    #define END_MEASURE(code)
#else
    #define BEGIN_MEASURE
    #define END_MEASURE(code)
#endif


#endif /* _HALDIAGNOSTIC_H */

