HAL_PATH = ..
include $(HAL_PATH)/Makerules

ifeq ($(USB), TRUE)
HAL_USB_CDC_PATH = $(HAL_PATH)/drivers/USBClasses/cdc
endif

######
LIBDIR = $(STACK_LIB_PATH)
LIB = $(LIBDIR)/lib$(HAL_LIB).a
ifeq ($(HAL), ATSAMD20J18)
  BUILDDIR = $(HAL_PATH)/cortexm0+/atsamd20j18
endif
ifeq ($(HAL), ATSAMR21G18A)
  BUILDDIR = $(HAL_PATH)/cortexm0+/atsamr21
endif
ifeq ($(HAL), ATSAMR21E18A)
  BUILDDIR = $(HAL_PATH)/cortexm0+/atsamr21
endif
ifeq ($(HAL), ATSAMR21E19A)
  BUILDDIR = $(HAL_PATH)/cortexm0+/atsamr21
endif
##### PATHS FLAGS OF INCLUDES #########
CFLAGS += -I$(HAL_PATH)/include
CFLAGS += -I$(HAL_HWD_COMMON_PATH)/include
CFLAGS += -I$(HAL_MAC_API_PATH)/include

CFLAGS += -I$(SE_PATH)/include

ifeq ($(USB), TRUE)
CFLAGS += -I$(HAL_USB_CDC_PATH)/include
ifeq ($(COMPILER_TYPE), GCC)
  USB_CFLAGS += -Wno-missing-field-initializers
  USB_CFLAGS += -Wno-unused-parameter
endif
endif

##### DECLARES USART CHANNELS TO BE USED ##########
CFLAGS += -DNUM_USART_CHANNELS=2
CFLAGS += -DHAL_USE_USART_CHANNEL_0
CFLAGS += -DHAL_USE_USART_CHANNEL_1

ifeq ($(COMPILER_TYPE), IAR)
  ASMFLAGS  = -I$(HAL_HWD_COMMON_PATH)/include 
  ASMFLAGS += -S
  ifneq (, $(findstring $(HAL), ATSAMD20J18 ATSAMR21G18A ATSAMR21E18A ATSAMR21E19A))
    ASMFLAGS += --cpu Cortex-M0+
  endif
endif
ifeq ($(COMPILER_TYPE), GCC)
  ASMFLAGS = $(CFLAGS)
  AS = $(CC)
endif

###### LIB ##########
  common_hwd += atomic
  common_hwd += halVectorTable
  common_hwd += halLowLevelInit
  common_hwd += halAppClock
  common_hwd += halInit
  common_hwd += halReset
  common_hwd += halSleep
  common_hwd += halSleepTimerClock
  common_hwd += halIrq
  common_hwd += halWdt
  common_hwd += halUsart
  common_hwd += halFlash
  common_hwd += halEeprom
  common_hwd += halTempSensor
ifneq (, $(findstring $(HAL), ATSAMR21G18A ATSAMR21E18A ATSAMR21E19A))
  common_hwd += halPwm
  common_hwd += halSpi
  common_hwd += halTwi
  common_hwd += halAdc
ifeq ($(USB), TRUE)
  common_hwd += halUsb
endif
endif

  personal_hwd += halRfCtrl
  personal_hwd += halRfSpi
  personal_hwd += halMacIsr

  hwi += appTimer
  hwi += halTaskManager
  hwi += sleep
  hwi += sleepTimer
  hwi += timer
  hwi += usart
  hwi += resetReason
  hwi += irq
  hwi += flash
  hwi += statStack
ifneq (, $(findstring $(HAL), ATSAMR21G18A ATSAMR21E18A ATSAMR21E19A))
  hwi += adc
  hwi += pwm
  hwi += spi
  hwi += eeprom
  hwi += i2cPacket
  hwi += tempSensor
ifeq ($(USB), TRUE)
  hwi += usb
endif
endif
ifeq ($(USB), TRUE)
  cdc += udc
  cdc += udi_cdc
  cdc += udi_cdc_desc
  cdc += usb_device_udd
endif
ifeq ($(COMPILER_TYPE), IAR)
  common_hwd += halStartupIar
endif
ifeq ($(COMPILER_TYPE), GCC)
  common_hwd += halStartupGcc
endif

objects_hwd = $(addsuffix .o,$(addprefix $(HAL_HWD_COMMON_PATH)/objs/,$(common_hwd)))
objects_hwd += $(addsuffix .o,$(addprefix $(HAL_MAC_API_PATH)/objs/,$(personal_hwd)))
sources_hwd = $(addsuffix .c,$(addprefix $(HAL_HWD_COMMON_PATH)/src/,$(common_hwd)))
sources_hwd += $(addsuffix .c,$(addprefix $(HAL_MAC_API_PATH)/src/,$(personal_hwd)))
objects_hwi = $(addsuffix .o,$(addprefix $(BUILDDIR)/../common/objs/,$(hwi)))
sources_hwi = $(addsuffix .c,$(addprefix $(BUILDDIR)/../common/src/,$(hwi)))
ifeq ($(USB), TRUE)
  objects_cdc = $(addsuffix .o,$(addprefix $(HAL_USB_CDC_PATH)/objs/,$(cdc)))
  sources_cdc = $(addsuffix .c,$(addprefix $(HAL_USB_CDC_PATH)/src/,$(cdc)))
endif
objects_asm_hwd = $(addsuffix .o,$(addprefix $(HAL_HWD_COMMON_PATH)/objs/,$(asm_hwd)))
sources_asm_hwd = $(addsuffix .s,$(addprefix $(HAL_HWD_COMMON_PATH)/src/,$(asm_hwd)))

###### TARGETS ################
all: component_label $(LIB)

component_label:
	@echo 
	@echo ----------------------------------------------------
	@echo HAL library creation.
	@echo ----------------------------------------------------

################ common part ##############################
$(HAL_HWD_COMMON_PATH)/objs/%.o: $(HAL_HWD_COMMON_PATH)/src/%.c
	$(CC_MSG)
	$(Q)$(CC) $(CFLAGS) $^ -o $@
################ common part ##############################	

################ personal part ##############################
$(HAL_MAC_API_PATH)/objs/%.o: $(HAL_MAC_API_PATH)/src/%.c
	$(CC_MSG)
	$(Q)$(CC) $(CFLAGS) $^ -o $@
################ personal part ##############################

################ hwi part ###################################
$(BUILDDIR)/../common/objs/%.o: $(BUILDDIR)/../common/src/%.c
	$(CC_MSG)
	$(Q)$(CC) $(CFLAGS) $^ -o $@
################ hwi part ###################################

ifeq ($(USB), TRUE)
################ cdc part ###################################
$(HAL_USB_CDC_PATH)/objs/%.o: $(HAL_USB_CDC_PATH)/src/%.c
	$(CC_MSG)
	$(Q)$(CC) $(CFLAGS) $(USB_CFLAGS) $^ -o $@
################ cdc part ###################################
endif

################ asm part ###################################
$(HAL_HWD_COMMON_PATH)/objs/%.o: $(HAL_HWD_COMMON_PATH)/src/%.s
	$(AS_MSG)
	$(Q)$(AS) $(ASMFLAGS) $^ -o $@
################ asm part ###################################

################
ifeq ($(USB), TRUE)
$(LIB): $(objects_asm_hwd) $(objects_hwd) $(objects_hwi) $(objects_cdc)
	$(AR_MSG)
	$(Q)$(AR) $(AR_KEYS) $(LIB) $(objects_hwd) $(objects_hwi) $(objects_asm_hwd) $(objects_cdc)
	$(SIZE_MSG)
	$(Q)$(SHOW_SIZE) -td $(LIB)	
else
$(LIB): $(objects_asm_hwd) $(objects_hwd) $(objects_hwi)
	$(AR_MSG)
	$(Q)$(AR) $(AR_KEYS) $(LIB) $(objects_hwd) $(objects_hwi) $(objects_asm_hwd)
	$(SIZE_MSG)
	$(Q)$(SHOW_SIZE) -td $(LIB)
endif

################
clean:
	@echo 
	@echo ----------------------------------------------------
	@echo HAL component cleaning is started...
ifeq ($(USB), TRUE)
	$(Q)rm -f $(objects_hwd) $(objects_hwi) $(objects_asm_hwd) $(objects_cdc) $(LIB)
else
	$(Q)rm -f $(objects_hwd) $(objects_hwi) $(objects_asm_hwd) $(LIB)
endif
	$(Q)rm -f $(wildcard $(HAL_PATH)/lib/*.a) $(wildcard $(HAL_PATH)/lib/*.o) $(wildcard $(HAL_PATH)/list/*.*)
	@echo HAL component cleaning is done!
	@echo ----------------------------------------------------
