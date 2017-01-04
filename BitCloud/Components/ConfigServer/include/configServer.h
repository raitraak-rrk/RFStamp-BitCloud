/**************************************************************************//**
  \file configServer.h

  \brief
    Configuration Server header file

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015 , Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    18.10.10 A. Razinkov - Created.
******************************************************************************/

#ifndef _CONFIG_SERVER_H
#define _CONFIG_SERVER_H

/******************************************************************************
                    Includes section
******************************************************************************/
#include <sysTypes.h>
#include <csDefaults.h>

#define CS_ID_MASK 0xFF
#define CS_TYPE_MASK 0xFF00
#define CS_RAM_PARAM_TYPE   0x0000
#define CS_FLASH_PARAM_TYPE 0x0100
#define CS_MEM_PARAM_TYPE   0x0200

#define RAM_PARAM_ID(n)    (CS_RAM_PARAM_TYPE + n)
#define FLASH_PARAM_ID(n)  (CS_FLASH_PARAM_TYPE + n)
#define MEM_PARAM_ID(n)    (CS_MEM_PARAM_TYPE + n)

#define RAM_PARAMETER(label, id, addr) label = RAM_PARAM_ID(id),
#define DUMMY_RAM_PARAMETER(label, id) label = RAM_PARAM_ID(id),
#define FLASH_PARAMETER(label, id, addr) label = FLASH_PARAM_ID(id),
#define DUMMY_FLASH_PARAMETER(label, id) label = FLASH_PARAM_ID(id),
#define MEMORY_REGION(label, id, addr) label = MEM_PARAM_ID(id),
#define DUMMY_MEMORY_REGION(label, id) label = MEM_PARAM_ID(id),

/******************************************************************************
                    Types section
******************************************************************************/
/*!
 * \brief List of the Configuration Server parameter identifiers.
 *        Identifiers are automatically sorted by memory location
 *        and item types (atomic parameter or memory region).
 *        The parameters themselves are defined in the csDefaults.h file.
 *        Parameter ID's name is got by adding "_ID" to the name of a parameter.
 */
typedef enum _CS_MemoryItemId_t
{
#include "csVarTable.h"
#include "csConstTable.h"
#include "csMemTable.h"
} CS_MemoryItemId_t;

#undef RAM_PARAMETER
#undef DUMMY_RAM_PARAMETER
#undef FLASH_PARAMETER
#undef DUMMY_FLASH_PARAMETER
#undef MEMORY_REGION
#undef DUMMY_MEMORY_REGION

/******************************************************************************
                    Defines section
******************************************************************************/
#define CS_MAX_PARAMETER_SIZE sizeof(UserDescriptor_t)

/******************************************************************************
                    Functions prototypes section
******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif


/****************************************************************************//**
\brief Configuration Server initialization
******************************************************************************/
void CS_Init(void);

/******************************************************************************//**
\brief Gets the value of the parameter specified by its ID and writes it to the provided address in memory

\ingroup cs_functions

The function reads the value of a ConfigServer parameter specified by its ID. A parameter ID is a constant
which name can be obtained by adding "_ID" suffix to the name of the parameter. For example, the identifier
of the CS_NWK_PANID parameter is CS_NWK_PANID_ID.

The function copies the value to the location in memory specified by the second argument.

For example, reading the extended address of the device is performed this way:

\code
ExtAddr_t ownExtAddr;
CS_ReadParameter(CS_UID_ID, &ownExtAddr);
\endcode

\param[in] parameterId -  the ID of the parameter to be read
\param[out] memoryPtr - a pointer to which the parameter's value is written
******************************************************************************/
void CS_ReadParameter(CS_MemoryItemId_t parameterId, void *memoryPtr);

/******************************************************************************//**
\brief Sets a value of a certain Configuration Server parameter specified by its ID

\ingroup cs_functions

The function assigns a new value to the specified Configuration Server parameter.
The value is copied from the addres specified by the second argument. The first argument is the
parameter's identifier formed by adding "_ID" suffix to the parameter's name. For example,
a specific value may be set to own extended address in the following way:

\code
ExtAddr_t ownExtAddr = 0x0123456789ABCDEF;
CS_WriteParameter(CS_UID_ID, &ownExtAddr);
\endcode

\param[in] parameterId - the ID of the parameter being written
\param[out] memoryPtr - the pointer to the parameter's new value
******************************************************************************/
void CS_WriteParameter(CS_MemoryItemId_t parameterId, const void *parameterValue);

/***********************************************************************************//**
\brief Gets a pointer to the memory allocated for a specific internal structure

\ingroup cs_functions

The function is used to obtain a pointer to the memory identified by a special value. The memory
is allocated by the stack for certain internal buffers and tables. For example, this function can be
employed to get the contents of bidning and routing tables, APS key-pair set, and other tables.

For some of these objects there is the API in BitCloud; the application must use such API rather than
this function to access the contents of a buffer. In case the API is absent the application may use
this function to observe the memory occupied by the buffer but is restricted to use the pointer
obtained with the function to write data to the memory, or the memory may become corrupted.

The function should be provided with an address of a pointer variable which will be assigned to
the starting point of the memory.

  \param[in] memoryId - and ID of a parameter or a specific part of the memory
  \param[out] memoryPtr - a pointer to which a starting address of the memory is written

***************************************************************************************/
void CS_GetMemory(CS_MemoryItemId_t memoryId, void **memoryPtr);

/**************************************************************************//**
\brief Set persist default values

\ingroup cs_functions
*****************************************************************************/
void CS_PdsDefaultValue(void);

/******************************************************************************
\brief Returns size of the particular item.

\param[in] itemId - item identifier.

\return size of the particular item.
******************************************************************************/
uint16_t CS_GetItemSize(CS_MemoryItemId_t itemId);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _CONFIG_SERVER_H */
/* eof configServer.h */
