// N_Cmi
#define N_Cmi_PlatformToBcAddressingConvert N_Cmi_PlatformToBcAddressingConvert_Impl
#define N_Cmi_PlatformToBcAddressModeConvert N_Cmi_PlatformToBcAddressModeConvert_Impl

#if !defined(ZIGBEE_END_DEVICE)
  #define N_Zdp_ZdpRequest    N_Cmi_ZdpRequest_Impl
#else
  #define N_Zdp_ZdpRequest    N_EndDeviceRobustness_ZdpRequest_Impl
  //#define N_Zdp_ZdpRequest    N_Cmi_ZdpRequest_Impl
#endif /* ZIGBEE_END_DEVICE */

#define N_Cmi_ZdpRequest N_Cmi_ZdpRequest_Impl
#define N_EndDeviceRobustness_ZdpRequest N_EndDeviceRobustness_ZdpRequest_Impl
