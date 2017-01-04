// implemented interface(s)
// ...

// used interfaces
#if defined(TESTHARNESS)
// ...bind to stubs...

// N_Cmi
#define N_Cmi_InitMacLayer N_Cmi_Stub_InitMacLayer
#define N_Cmi_UseNwkMulticast N_Cmi_Stub_UseNwkMulticast

#else
// ...bind to implementation...

// N_Cmi
#define N_Cmi_InitMacLayer N_Cmi_InitMacLayer_Impl
#define N_Cmi_UseNwkMulticast N_Cmi_UseNwkMulticast_Impl

#endif
