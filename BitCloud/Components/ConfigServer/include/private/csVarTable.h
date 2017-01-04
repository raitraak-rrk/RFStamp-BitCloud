/******************************************************************************
  \file csVarTable.h

  \brief
    Configuration Server RAM parameters information table

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015 , Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    22.12.10 A. Razinkov - Created.
******************************************************************************/

/* This table contains information about particular paramter displacement address.
 *
 * This table should be inlined to external module.
 * RAM_PARAMETER(id, addr) and DUMMY_RAM_PARAMETER(id) macroses should be defined 
 * first to provide specific table morphing.
 *
 * DUMMY_RAM_PARAMETER(id) macro is used to specify few stack buffers sizes during 
 * the application compilation phase.
 */

#if defined(_USE_KF_MAC_)
RAM_PARAMETER(CS_UID_ID, 0x00, tal_pib_IeeeAddress)
RAM_PARAMETER(CS_MAX_FRAME_TRANSMISSION_TIME_ID, 0x01, maxFrameTransmissionTime)
#else // defined(_USE_KF_MAC_)
RAM_PARAMETER(CS_UID_ID, 0x00, csPIB.macAttr.extAddr)
RAM_PARAMETER(CS_MAX_FRAME_TRANSMISSION_TIME_ID, 0x01, csPIB.macAttr.maxFrameTransmissionTime)
#endif // defined(_USE_KF_MAC_)

RAM_PARAMETER(CS_MAC_TRANSACTION_TIME_ID, 0x02, csSIB.csMacTransactionTime)
RAM_PARAMETER(CS_RF_TX_POWER_ID, 0x03, csSIB.csRfTxPower)
RAM_PARAMETER(CS_CHANNEL_MASK_ID, 0x04, csSIB.csChannelMask)
#ifndef _MAC2_
RAM_PARAMETER(CS_MAX_NEIGHBOR_ROUTE_COST_ID, 0x05, csNIB.maxNeighborRouteCost)
RAM_PARAMETER(CS_MAX_LINK_ROUTE_COST_ID, 0x06, csNIB.maxLinkRouteCost)
RAM_PARAMETER(CS_NWK_EXT_PANID_ID, 0x07, csNIB.extendedPanId)
RAM_PARAMETER(CS_NWK_ADDR_ID, 0x08, csNIB.networkAddress)
RAM_PARAMETER(CS_NWK_PARENT_ADDR_ID, 0x09, csNIB.parentNetworkAddress)
RAM_PARAMETER(CS_NWK_DEPTH_ID, 0x0A, csNIB.depth)
RAM_PARAMETER(CS_NWK_UNIQUE_ADDR_ID, 0x0B, csNIB.uniqueAddr)
RAM_PARAMETER(CS_NWK_LEAVE_REQ_ALLOWED_ID, 0x0C, csNIB.leaveReqAllowed)
RAM_PARAMETER(CS_CHANNEL_PAGE_ID, 0x0D, csNIB.channelPage)
RAM_PARAMETER(CS_NWK_USE_MULTICAST_ID, 0x0E, csSIB.csNwkUseMulticast)
RAM_PARAMETER(CS_NWK_MAX_LINK_STATUS_FAILURES_ID, 0x0F, csSIB.csNwkMaxLinkStatusFailures)
RAM_PARAMETER(CS_NWK_END_DEVICE_MAX_FAILURES_ID, 0x10, csSIB.csNwkEndDeviceMaxFailures)
RAM_PARAMETER(CS_NWK_LOGICAL_CHANNEL_ID, 0x11, csSIB.csNwkLogicalChannel)
RAM_PARAMETER(CS_NWK_PANID_ID, 0x12, csSIB.csNwkPanid)
RAM_PARAMETER(CS_NWK_PREDEFINED_PANID_ID, 0x13, csSIB.csNwkPredefinedPanid)
RAM_PARAMETER(CS_ADDRESS_ASSIGNMENT_METHOD_ID, 0x14, csSIB.csAddressAssignmentMethod)
RAM_PARAMETER(CS_PROTOCOL_VERSION_ID, 0x15, csNIB.protocolVersion)
RAM_PARAMETER(CS_STACK_PROFILE_ID, 0x16, csNIB.stackProfile)
RAM_PARAMETER(CS_SCAN_DURATION_ID, 0x17, csZIB.scanDuration)
RAM_PARAMETER(CS_PERMIT_DURATION_ID, 0x18, csZIB.permitJoinDuration)
RAM_PARAMETER(CS_EXT_PANID_ID, 0x19, csSIB.csExtPANID)
RAM_PARAMETER(CS_INDIRECT_POLL_RATE_ID, 0x1A, csSIB.csIndirectPollRate)
RAM_PARAMETER(CS_END_DEVICE_SLEEP_PERIOD_ID, 0x1B, csSIB.csEndDeviceSleepPeriod)
RAM_PARAMETER(CS_FFD_SLEEP_PERIOD_ID, 0x1C, csSIB.csFfdSleepPeriod)
RAM_PARAMETER(CS_RX_ON_WHEN_IDLE_ID, 0x1D, csSIB.csRxOnWhenIdle)
RAM_PARAMETER(CS_COMPLEX_DESCRIPTOR_AVAILABLE_ID, 0x1E, csSIB.csComplexDescriptorAvailable)
RAM_PARAMETER(CS_USER_DESCRIPTOR_AVAILABLE_ID, 0x1F, csSIB.csUserDescriptorAvailable)
RAM_PARAMETER(CS_ZDP_USER_DESCRIPTOR_ID, 0x20, csSIB.csUserDescriptor)
RAM_PARAMETER(CS_DEVICE_TYPE_ID, 0x21, csNIB.deviceType)
RAM_PARAMETER(CS_ZDO_JOIN_ATTEMPTS_ID, 0x22, csSIB.csZdoJoinAttempts)
RAM_PARAMETER(CS_ZDO_JOIN_INTERVAL_ID, 0x23, csSIB.csZdoJoinInterval)
RAM_PARAMETER(CS_APS_MAX_FRAME_RETRIES_ID, 0x24, csAIB.maxFrameRetries)
RAM_PARAMETER(CS_ZDP_RESPONSE_TIMEOUT_ID, 0x25, csZIB.zdpResponseTimeout)
RAM_PARAMETER(CS_DTR_WAKEUP_ID, 0x26, csSIB.csDtrWakeup)
#endif

#ifdef _FFD_
#ifdef _PENDING_EMPTY_DATA_FRAME_
RAM_PARAMETER(CS_MAC_SEND_EMPTY_DATA_FRAME_ID, 0x27, csSIB.csMacSendEmptyDataFrame)
#endif /* _PENDING_EMPTY_DATA_FRAME_ */
#endif /* _FFD_ */

#ifdef AT86RF212
RAM_PARAMETER(CS_LBT_MODE_ID, 0x28, csSIB.csLbtMode)
#endif
#ifdef _NWK_CONCENTRATOR_
RAM_PARAMETER(CS_CONCENTRATOR_DISCOVERY_TIME_ID, 0x29, csSIB.csNwkConcentratorDiscoveryTime)
#endif
#ifdef _APS_FRAGMENTATION_
RAM_PARAMETER(CS_APS_MAX_TRANSMISSION_WINDOW_SIZE_ID, 0x2A, csAIB.fragWindowSize)
#endif

#if ZCL_SUPPORT == 1
  #if APP_USE_OTAU == 1
RAM_PARAMETER(CS_ZCL_OTAU_DEFAULT_UPGRADE_SERVER_IEEE_ADDRESS_ID, 0x2C, csSIB.csOtauDefaultServerAddress)
RAM_PARAMETER(CS_ZCL_OTAU_DEFAULT_SERVER_DISCOVERY_PERIOD_ID, 0x2D, csSIB.csOtauServerDiscoveryPeriod)
RAM_PARAMETER(CS_ZCL_OTAU_IMAGE_PAGE_REQUEST_ENABLE_ID, 0x2E, csSIB.csOtauServerPageRequestUsage)
  #endif /* APP_USE_OTAU == 1 */
#endif /* ZCL_SUPPORT == 1 */
#ifdef _SECURITY_
RAM_PARAMETER(CS_NETWORK_KEY_ID, 0x2F, defaultKey)
RAM_PARAMETER(CS_SECURITY_ON_ID, 0x30, csSIB.csSecurityOn)
RAM_PARAMETER(CS_SECURITY_LEVEL_ID, 0x31, csNIB.securityIB.securityLevel)
RAM_PARAMETER(CS_SECURITY_ALL_FRAMES_ID, 0x32, csNIB.securityIB.secureAllFrames)
RAM_PARAMETER(CS_ZDO_SECURITY_STATUS_ID, 0x33, csSIB.csZdoSecurityStatus)
RAM_PARAMETER(CS_APS_TRUST_CENTER_ADDRESS_ID, 0x34, csAIB.trustCenterAddress)
RAM_PARAMETER(CS_SHORT_TRUST_CENTER_ADDRESS_ID, 0x35, csAIB.tcNwkAddr)
#endif /* _SECURITY_ */
#ifndef _MAC2_
RAM_PARAMETER(CS_JOIN_CONTROL_ID, 0x36, csSIB.csJoinCtrl)
RAM_PARAMETER(CS_LEAVE_REJOIN_CONTROL_ID, 0x37, csSIB.csLeaveRejoinCtrl)
RAM_PARAMETER(CS_LEAVE_NON_REJOIN_CONTROL_ID, 0x38, csSIB.csLeaveNonRejoinCtrl)
#ifdef _NWK_THRESHOLD_ROUTING_
RAM_PARAMETER(CS_SOFT_LINK_COST_THRESHOLD_ID, 0x39, csNIB.softLinkCostThreshold)
#endif /* _NWK_THRESHOLD_ROUTING_ */
#endif /* _MAC2_ */
RAM_PARAMETER(CS_RF_CURRENT_CHANNEL_ID, 0x40, csPIB.phyAttr.channel)
#if ZCL_SUPPORT == 1
  #if APP_USE_OTAU == 1
RAM_PARAMETER(CS_ZCL_OTAU_QUERY_INTERVAL_ID, 0x41, csSIB.csOtauQueryInterval)
RAM_PARAMETER(CS_ZCL_OTAU_MAX_RETRY_COUNT_ID, 0x42, csSIB.csOtauMaxRetryCount)
  #endif /* APP_USE_OTAU == 1 */
#endif /* ZCL_SUPPORT == 1 */

#ifdef _ZGPD_SPECIFIC_
RAM_PARAMETER(CS_ZGP_SRCID_ID, 0x43, csZGIB.deviceParams.srcID)
RAM_PARAMETER(CS_ZGP_USE_CSMA_CA_ID, 0x44, csZGIB.deviceParams.useCsmaCA)
RAM_PARAMETER(CS_ZGP_NO_OF_FRAMES_IN_SEQ_ID, 0x45, csZGIB.stackparams.numOfFramesInSeq)
RAM_PARAMETER(CS_ZGP_COMMISSIONING_TYPE_ID, 0x46, csZGIB.deviceParams.commissioningType)
RAM_PARAMETER(CS_ZGP_SECURITY_LEVEL_ID, 0x47, csZGIB.deviceParams.secLevel)
#ifdef ZGP_SECURITY_ENABLE
RAM_PARAMETER(CS_ZGP_SECURITY_KEY_TYPE_ID, 0x48, csZGIB.secParams.secKey.keyType)
#endif //ZGP_SECURITY_ENABLE
RAM_PARAMETER(CS_ZGP_APPID_ID, 0x49, csZGIB.deviceParams.appId)
RAM_PARAMETER(CS_ZGP_RX_AFTER_TX_ID, 0x4A, csZGIB.deviceParams.rxAfterTx)
RAM_PARAMETER(CS_ZGP_USE_INC_SEQ_NUM_ID, 0x4B, csZGIB.deviceParams.useIncSeqNum)
#endif /* ZGP */
RAM_PARAMETER(CS_RF_MAX_CH26_TX_POWER_ID, 0x4C, csSIB.csRfch26MaxTxPower)
RAM_PARAMETER(CS_RF_CCA_MODE_ID, 0x4D, csPIB.phyAttr.ccaMode)
RAM_PARAMETER(CS_RF_CCA_ED_THRES_ID, 0x4E, csPIB.macAttr.ccaEdThres)