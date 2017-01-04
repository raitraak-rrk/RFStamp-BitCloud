/*********************************************************************************************//**
\file  

\brief 

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal

$Id: N_Types.h 24615 2013-02-27 05:14:24Z arazinkov $

***************************************************************************************************/

#ifndef N_TYPES_H
#define N_TYPES_H

/***************************************************************************************************
* INCLUDE FILES
***************************************************************************************************/

#  include <stdint.h>
#  include <stdbool.h>

#  if defined(__bool_true_false_are_defined) // should be only defined in bootloader and sb_crypt by adding pre-include file stdbool.h
# ifndef TRUE
#    define TRUE true
# endif
# ifndef FALSE
#    define FALSE false
# endif
#  else
      typedef unsigned char   bool;
#  endif


#ifndef TRUE
#  define TRUE 1u
#endif

#ifndef FALSE
#  define FALSE 0u
#endif

#ifndef NULL
#  define NULL 0u
#endif


#endif // N_TYPES_H
