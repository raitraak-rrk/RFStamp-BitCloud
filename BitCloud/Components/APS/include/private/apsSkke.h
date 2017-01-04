/**************************************************************************//**
  \file  apsSkke.h

  \brief Private interface of symmetric-key key establishment implementation.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2011-04-23 Max Gekk - Created.
   Last change:
    $Id: apsSkke.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#if !defined _APS_SKKE_H
#define _APS_SKKE_H

/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <apsCommand.h>
#if defined _SECURITY_ && defined _LINK_SECURITY_ && defined _HIGH_SECURITY_
#include <apsmeEstablishKey.h>

/******************************************************************************
                              Define(s) section
 ******************************************************************************/
/** Size of the data field in SKKE command frames. */
#define APS_SKKE_DATA_SIZE 16U
/* Maximum value of SKKE TTL. */
#define APS_SKKE_INFINITY_TTL UINT32_MAX

/******************************************************************************
                                Types section
 ******************************************************************************/
/* Type of internal state of SKKE responder or initiator. */
typedef enum _ApsSkkeState_t
{
  APS_SKKE_REQ_STATE,
  APS_WAIT_SKKE_2_STATE,
  APS_WAIT_SKKE_4_STATE,
  APS_SKKE_RESP_STATE,
  APS_WAIT_SKKE_3_STATE
} ApsSkkeState_t;

BEGIN_PACK
/** Type of generic SKKE command frame. See ZigBee spec r19, page 478. */
typedef struct PACK _ApsSkkeFrame_t
{
  ApduCommandHeader_t header;
  /** The 64-bit extended address of the device that acts as the initiator
   *  in the key-establishment protocol. See ZigBee spec r19, 4.4.9.1.2 */
  uint8_t initiator[sizeof(ExtAddr_t)];
  /** The 64-bit extended address of the device that acts as the responder
   *  in the key-establishment protocol. See ZigBee spec r19, 4.4.9.1.3 */
  uint8_t responder[sizeof(ExtAddr_t)];
  /** The content of the data field depends on the command identifier field:
   * QEU for SKKE-1, QEV for SKKE-2, MacTag2 for SKKE-3, MacTag1 for SKKE-4. */
  uint8_t data[APS_SKKE_DATA_SIZE];
} ApsSkkeFrame_t;
END_PACK

/******************************************************************************
                              Prototypes section
 ******************************************************************************/
/**************************************************************************//**
  \brief Process an incoming SKKE command frame.

  \param[in] commandInd - pointer to parameters of the received command:
                            - secured or not,
                            - extended address of source node,
                            - pointer to NLDE-DATA.indication's parameters.

  \return 'true' - free NWK buffer for a SKKE command otherwise return 'false'.
 ******************************************************************************/
APS_PRIVATE bool apsSkkeInd(const ApsCommandInd_t *const commandInd);

/**************************************************************************//**
  \brief Initializes the common substructure of SKKE requests or response.

  \param[in, out] skkeElem - pointer to the common element of SKKE request
                        or SKKE response. \sa ApsSkkeElem_t
  \param[in] state - initial state of the SKKE request.
  \param[in] ttl - time to live of the SKKE request.
  \param[in] apsSkkeConf - confirmation function of the SKKE request or response.

  \return None.
 ******************************************************************************/
APS_PRIVATE void apsInitSkkeElem(ApsSkkeElem_t *const skkeElem,
  const ApsSkkeState_t state, const uint32_t ttl,
  void (* apsSkkeConf)(struct _ApsSkkeElem_t*, APS_SkkeStatus_t));

/**************************************************************************//**
  \brief Find a first SKKE element by state and remove it from internal queue.

  \param[in] state - state of the SKKE request or response.

  \return Pointer to SKKE element or NULL if element isn't found.
 ******************************************************************************/
APS_PRIVATE ApsSkkeElem_t* apsFetchSkkeElem(const ApsSkkeState_t state);

/**************************************************************************//**
  \brief Is there an SKKE element with the given state?

  \param[in] state - state of the SKKE request or response.

  \return 'true' if any element is found otherwise return 'false'.
 ******************************************************************************/
APS_PRIVATE bool apsIsThereSkkeElem(const ApsSkkeState_t state);

/**************************************************************************//**
  \brief Set new TTL of the SKKE element.

  \param[in, out] skkeElem - pointer to the common substructure of SKKE request
                        or SKKE response. \sa ApsSkkeElem_t
  \param[in] ttl - time to live of the SKKE request or response.
  \return None.
 ******************************************************************************/
APS_PRIVATE void apsSetSkkeTTL(ApsSkkeElem_t *const skkeElem, const uint32_t ttl);

/**************************************************************************//**
  \brief Set new state of the SKKE element.

  \param[in, out] skkeElem - pointer to the common substructure of SKKE request
                        or SKKE response. \sa ApsSkkeElem_t
  \param[in] state - new state of the SKKE request or response.
  \return None.
 ******************************************************************************/
APS_PRIVATE void apsSetSkkeState(ApsSkkeElem_t *const skkeElem,
  const ApsSkkeState_t state);

/**************************************************************************//**
  \brief Allocate memory for SKKE frame and NWK_DataReq_t structure in NWK.

  \param[in] allocNwkDataReq - pointer to NWK DataReq allocation's parameters.
  \param[in] nwkAllocDataConf - callback function. NWK-layer calls this function
                                when memory has been allocated.
  \return None.
 ******************************************************************************/
APS_PRIVATE void apsSkkeAllocFrameReq(NWK_AllocDataReq_t *const allocNwkDataReq,
  void (*nwkAllocDataConf)(NWK_AllocDataConf_t*));

/**************************************************************************//**
  \brief Prepare and fill fields of APS SKKE command frame.

  \param[in] allocConf - pointer to confirmation structure with pointer to
                         allocated NWK_DataReq_t structure.
  \param[in] commandId - identifier of APS security command frame.
  \param[in] nwkDataReq - pointer to field in the SKKE request (reponse)
                          for saving pointer to allocated NWK_DataReq_t structure.
  \param[in] data - pointer to value for the data field of APS SKKE frame.
  \param[in] initiatorAddress - pointer to extended address of SKKE initiator.
  \param[in] responderAddress - pointer to extended address of SKKE responser.

  \return None.
 ******************************************************************************/
APS_PRIVATE void apsPrepareSkkeCommand(NWK_AllocDataConf_t *const allocConf,
  const ApsCommandId_t commandId, NWK_DataReq_t **const skkeNwkDataReq,
  const uint8_t *const data, const ExtAddr_t *const initiatorAddress,
  const ExtAddr_t *const responderAddress);

/**************************************************************************//**
  \brief Prepare NWK_DataReq_t structure and send SKKE command frame.

  \param[in] nwkDataReq - pointer to parameters of NLDE-DATA.request.
  \param[in] nwkDataConf - NLDE-DATA confirm callback function's pointer.
  \param[in] dstShortAddr - short address of partner (initiator or responder).
  \param[in] secure - enable security on NWK layer.

  \return None.
 ******************************************************************************/
APS_PRIVATE void apsSkkeNwkDataReq(NWK_DataReq_t *const nwkDataReq,
  void (* nwkDataConf)(NWK_DataConf_t*), const ShortAddr_t dstShortAddr,
  const bool secure);

/**************************************************************************//**
  \brief Process confirmation from NWK layer of transmission SKKE command.

  \param[in] nwkDataConf - pointer to NLDE-DATA confirmation parameters.
  \param[in] state - state of SKKE request or response for which SKKE command
                     was transmitted.
  \param[in] apsSkkeCompareNwkReq - matching function of SKKE request
                          or response with NWK DataReq_t structure.
  \param[in] txErrorStatus - return this SKKE status if transmission of SKKE
                          command is failure.
  \return None.
 ******************************************************************************/
APS_PRIVATE void apsSkkeNwkDataConf(NWK_DataConf_t *const nwkDataConf,
  const ApsSkkeState_t state,
  bool (*apsSkkeCompareNwkReq)(const ApsSkkeElem_t*, NWK_DataReq_t*),
  const APS_SkkeStatus_t txErrorStatus);

/**************************************************************************//**
  \brief Process indication from NWK layer about SKKE command.

  \param[in] nwkDataInd - pointer to NLDE-DATA indication parameters.
  \param[in] state - state of SKKE request or response for which SKKE command
                     is expected.
  \param[in] apsSkkeCompareNwkInd - matching function of SKKE request
                          or response with NWK DataInd_t structure.
  \param[in, out] skkeFrame - pointer to pointer to SKKE raw frame.
  \return Pointer to SKKE element or NULL if it insn't found.
 ******************************************************************************/
APS_PRIVATE ApsSkkeElem_t* apsFindSkkeElemByNwkInd(const NWK_DataInd_t *const nwkDataInd,
  const ApsSkkeState_t state,
  bool (*apsSkkeCompareNwkInd)(const ApsSkkeElem_t*, const ApsSkkeFrame_t*),
  ApsSkkeFrame_t **skkeFrame);

/**************************************************************************//**
  \brief Reset the internal queue and variables of SKKE component.
 ******************************************************************************/
APS_PRIVATE void apsSkkeReset(void);

#else  /* not _HIGH_SECURITY_ */

#define apsSkkeInd NULL
#define apsSkkeReset() ((void)0)
#endif /* _HIGH_SECURITY_ */

#endif /* _APS_SKKE_H */
/** eof apsSkke.h */
