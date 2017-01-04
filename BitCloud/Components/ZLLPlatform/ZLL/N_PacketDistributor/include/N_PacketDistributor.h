/*********************************************************************************************//**
\file  
    N_PacketDistributor.h
\brief 
    Header file used in PacketDistributor functionality to register End points and get the informsation about the end points
\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal

$Id: N_PacketDistributor.h 24615 2013-02-27 05:14:24Z arazinkov $

***************************************************************************************************/

#ifndef N_PACKETDISTRIBUTOR_H
#define N_PACKETDISTRIBUTOR_H

/***************************************************************************************************
* INCLUDE FILES
***************************************************************************************************/

#include <N_ZllEndpoint.h>
#include <N_Types.h>
#include <N_Zcl_DataInd.h>

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
#define N_PACKETDISTRIBUTOR_CLUSTER_ID_TYPE ClusterId_t
#define N_PACKETDISTRIBUTOR_SIMPLE_DESCRIPTOR_TYPE SimpleDescriptor_t

/***************************************************************************************************
* EXPORTED FUNCTIONS
***************************************************************************************************/

/** Register a ZigBee end-point.
    \param ZLL_Endpoint_t Pointer to a filled structure describing
        the end-point. The memory must stay available after the call to
        this function
    \param groupCount The number of groups used by the the endpoint (see the note below)

    \note The groups count should include number of the groups that the endpoint uses as client of
        the ZCL groups cluster, NOT the number of groups used as server of the cluster: e.g. a
        remote control endpoint uses one group (groupCount == 1) to control some dimmable
        light endpoints (groupCount == 0).
*/
void N_PacketDistributor_RegisterEndpoint(ZLL_Endpoint_t* zllEp, uint8_t groupCount);

/** Returns the number of Zcl EndPoints (so excluding ZDO and InterPan endpoints)
*/
uint8_t N_PacketDistributor_GetNumberOfZclEndpoints(void);

/** Returns the zcl endpoint number for an index (0..NumberOfZclEndpoints-1)
*/
uint8_t N_PacketDistributor_GetZclEndpointFromIndex(uint8_t index);

/** Get the group count registered with \ref N_PacketDistributor_RegisterEndpoint.
    \param endpoint The endpoint number for which to get the group count
    \returns The group count
*/
uint8_t N_PacketDistributor_GetGroupCount(uint8_t endpoint);

/** Get the sum of the group counts registered with \ref N_PacketDistributor_RegisterEndpoint for all endpoints.
    \returns The total number of groups
*/
uint8_t N_PacketDistributor_GetTotalGroupCount(void);

/** Returns the number of Zll EndPoints
*/
uint8_t N_PacketDistributor_GetNumberOfZllEndpoints(void);

/** Returns the zll endpoint number for an index (0..NumberOfZllEndpoints-1)
*/
uint8_t N_PacketDistributor_GetZllEndpointFromIndex(uint8_t index);

/** Get the sum of the group counts registered with \ref N_PacketDistributor_RegisterEndpoint for all endpoints.
    \returns The total number of groups
*/
uint8_t N_PacketDistributor_GetTotalGroupCountZll(void);

/** Find a registered endpoint.
    \param endpoint Endpoint number
    \returns A pointer to the endpoint description or NULL if the endpoint could not be found.
             Do not free the memory
*/
ZLL_Endpoint_t* N_PacketDistributor_FindEndpoint(uint8_t endpoint);

/** Check if the received message is received Inter-PAN.
    \param panId The pan id from the source address
    \param endpoint The endpoint from the source address
    \returns TRUE if it is an Inter-PAN message, FALSE otherwise
*/
bool N_PacketDistributor_IsInterPan(uint16_t panId, uint8_t endpoint);

/** Subscribtion on PacketDistributor messages
 *  \param endpoint The endpoint to subscribe on messages
 *  \param pCallback Message handling routine pointer */
void N_PacketDistributor_Subscribe(uint8_t endpoint, const N_Zcl_DataInd_Callback_t* pCallback);

/***************************************************************************************************
* END OF C++ DECLARATION WRAPPER
***************************************************************************************************/

#ifdef __cplusplus
}
#endif

#endif // N_PACKETDISTRIBUTOR_H
