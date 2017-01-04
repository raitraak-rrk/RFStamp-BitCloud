/*********************************************************************************************//**
\file  

\brief 

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal

$Id: N_Memory.h 24615 2013-02-27 05:14:24Z arazinkov $

***************************************************************************************************/

#ifndef N_MEMORY_H
#define N_MEMORY_H

/***************************************************************************************************
* INCLUDE FILES
***************************************************************************************************/

#include <stddef.h>

/***************************************************************************************************
* C++ DECLARATION WRAPPER
***************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************************************
* EXPORTED TYPES
***************************************************************************************************/


/***************************************************************************************************
* EXPORTED MACROS AND CONSTANTS
***************************************************************************************************/


/***************************************************************************************************
* EXPORTED FUNCTIONS
***************************************************************************************************/

/** Allocate memory from the heap and set it to all zeroes.
    \param size The number of bytes to allocate
    \returns Pointer to the allocated memory, or NULL if the memory is not available.
*/
void* N_Memory_Alloc(size_t size);

/** Allocate memory from the heap and assert if it fails.
    \see N_Memory_Alloc.
*/
void* N_Memory_AllocChecked(size_t size);

/** Free previously allocated memory.
*/
void N_Memory_Free(void* ptr);

/***************************************************************************************************
* END OF C++ DECLARATION WRAPPER
***************************************************************************************************/

#ifdef __cplusplus
}
#endif

#endif // N_MEMORY_H
