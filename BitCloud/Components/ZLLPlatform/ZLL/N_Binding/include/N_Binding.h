/*********************************************************************************************//**
\file  

\brief 

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal

$Id: N_Binding.h 24615 2013-02-27 05:14:24Z arazinkov $

***************************************************************************************************/

#ifndef N_BINDING_H
#define N_BINDING_H

/***************************************************************************************************
* INCLUDE FILES
***************************************************************************************************/

#include "N_Address.h"
#include "N_Types.h"

/***************************************************************************************************
* C++ DECLARATION WRAPPER
***************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************************************
* EXPORTED TYPES
***************************************************************************************************/
typedef struct N_Binding_Record_t
{
    uint8_t sourceEndpoint;
    N_Address_t destinationAddress;
    uint16_t clusterId;
} N_Binding_Record_t;

/***************************************************************************************************
* EXPORTED MACROS AND CONSTANTS
***************************************************************************************************/

#define INVALID_BINDING_RECORD_INDEX 0xFFu

/***************************************************************************************************
* EXPORTED VARIABLES
***************************************************************************************************/
extern N_Address_t g_N_Binding_Address;


/***************************************************************************************************
* EXPORTED FUNCTIONS
***************************************************************************************************/

/** Add an entry to the binding table
    \param sourceEndpoint The source endpoint
    \param pDestinationAddress The destination address
    \param numClusterIds The number of ClusterIds
    \param pClusterIds Array of clusterIds
*/
void N_Binding_AddBinding( uint8_t sourceEndpoint,
                           const N_Address_t *pDestinationAddress,
                           uint8_t numClusterIds,
                           uint16_t *pClusterIds );

/** Add an entry to the binding table
    \param sourceEndpoint The source endpoint
    \param pDestinationAddress The destination address
    \param numClusterIds The number of ClusterIds
    \param pClusterIds Array of clusterIds
    \returns TRUE if operation succeeded, FALSE otherwise
*/
bool N_Binding_AddBindingExt( uint8_t sourceEndpoint,
                              const N_Address_t* pDestinationAddress,
                              uint8_t numClusterIds,
                              uint16_t *pClusterIds );

/** Remove a binding from the binding table
    \param sourceEndpoint The source endpoint
    \param pDestinationAddress The destination address
    \param clusterId the clusterId
    \returns TRUE if operation succeeded, FALSE otherwise
*/
bool N_Binding_RemoveBinding( uint8_t sourceEndpoint,
                              const N_Address_t* pDestinationAddress,
                              uint16_t clusterId );

/** Remove all bindings for an endpoint
    \param endpoint Endpoint to remove, 0xFF is all endpoints
*/
void N_Binding_RemoveBindings(uint8_t endpoint);


/** find an entry to the binding table
    \param sourceEndpoint The source endpoint
    \param clusterId The concerning clusterId
    \param skip The number of bindings to skip. '0' returns the index to the first binding record
                with sourceEndpoint and clusterId, '1' the second, etc.
    \returns the index of the binding record, returns INVALID_BINDING_RECORD_INDEX if no such
             binding exist
*/
uint8_t N_Binding_FindBinding( uint8_t sourceEndpoint,
                             uint16_t clusterId,
                             uint8_t skip );


/** Get the number of binding table records
    \returns the number of binding table records
*/
uint8_t N_Binding_GetNumberOfBindingRecords(void);

/** Get the binding table records specified by the index
    \param[in] index Index of the binding record to retrieve
    \param[out] pRecord The binding table record
*/
void N_Binding_GetBindingRecord(uint8_t index, N_Binding_Record_t *pRecord);

/** Remove a binding record
    \param index Index of the binding to remove
*/
void N_Binding_RemoveBindingRecord(uint8_t index);

/***************************************************************************************************
* END OF C++ DECLARATION WRAPPER
***************************************************************************************************/

#ifdef __cplusplus
}
#endif

#endif // N_BINDING_H
