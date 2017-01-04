COMPONENTS_PATH = ..
include $(MAKERULES)
include ./BoardConfig

###### LIB ##########
ifeq ($(BOARD), BOARD_MESHBEAN)
  BUILDDIR = ./MESHBEAN
else ifeq ($(BOARD), BOARD_RAVEN)
  BUILDDIR = ./RAVEN
else ifeq ($(BOARD), BOARD_STK500)
  BUILDDIR = ./ATML_STK500
else ifeq ($(BOARD), BOARD_STK600)
  BUILDDIR = ./ATML_STK600
else ifeq ($(BOARD), BOARD_USB_DONGLE)
  BUILDDIR = ./ATML_USB_DONGLE
else ifeq ($(BOARD), BOARD_MESHBEAN_LAN)
  BUILDDIR = ./MESHBEAN_LAN
else ifeq ($(BOARD), BOARD_MESHBEAN_TURBO)
  BUILDDIR = ./MESHBEAN_TURBO
else ifeq ($(BOARD), BOARD_SAM7X_EK)
  BUILDDIR = ./ATML_SAM7X_EK
else ifeq ($(BOARD), BOARD_SAM3S_EK)
  BUILDDIR = ./ATML_SAM3S_EK
else ifeq ($(BOARD), BOARD_RF231USBRD)
  BUILDDIR = ./RF231USBRD
else ifeq ($(BOARD), BOARD_SAM4S_PIRRD)
  BUILDDIR = ./ATML_SAM4S_PIRRD
else ifeq ($(BOARD), BOARD_CUSTOM_1)
  BUILDDIR = ./CUSTOM_1
else ifeq ($(BOARD), BOARD_CUSTOM_2)
  BUILDDIR = ./CUSTOM_2
else ifeq ($(BOARD), BOARD_CUSTOM_3)
  BUILDDIR = ./CUSTOM_3
else ifeq ($(BOARD), BOARD_RCB)
  BUILDDIR = ./RCB
else ifeq ($(BOARD), BOARD_AVR32_EVK1105)
  BUILDDIR = ./AVR32_EVK1105
else ifeq ($(BOARD), BOARD_XPLAIN)
  BUILDDIR = ./XPLAIN
else ifeq ($(BOARD), BOARD_REB_CBB)
  BUILDDIR = ./REB_CBB
else ifeq ($(BOARD), BOARD_SIMULATOR)
  BUILDDIR = ./SIMULATOR
else
  $(error unknown BOARD)
endif

# Compiler flags extending with "debug info" compiler flag.
CFLAGS += $(CFLAGS_DBG)

# Hardware flags.
CFLAGS += $(CFLAGS_HW)

LIBDIR = ./lib
LISTDIR = ./list
LDFLAGS = -L$(LIBDIR)
PROG = BSP.elf
LIBS = $(LIBDIR)/lib$(BSP_LIB).a

##### PATHS FLAGS OF INCLUDES #########
CFLAGS += -I$(BUILDDIR)/include
CFLAGS += -I$(BUILDDIR)/../include
CFLAGS += -I$(HAL_PATH)/include  
CFLAGS += -I$(HAL_HWD_COMMON_PATH)/include
CFLAGS += -I$(SE_PATH)/include

BSPFLAGS=$(CFLAGS) 
ifeq ($(BOARD), BOARD_SAM7X_EK)
  BSPFLAGS=$(patsubst %$(ARM_MODE_THUMBS_CFLAGS),,$(CFLAGS)) $(ARM_MODE_ARM_CFLAGS)
  BSPFLAGS += -I$(FREE_RTOS_PATH)/
  BSPFLAGS += -I$(FREE_RTOS_PATH)/include
endif

ifeq ($(BOARD), BOARD_MESHBEAN)
modules =                       \
       leds                     \
       pwrCtrl                  \
       bspTaskManager           \
       buttons                  \
       lm73                     \
       tsl2550                  \
       battery                  \
       sliders                  \
       sensors                  \
       fakeBSP
else ifeq ($(BOARD), BOARD_RAVEN)
modules =                       \
       lcd                      \
       bspTaskManager           \
       joystik                  \
       sipc                     \
       sensors                  \
       fakeBSP
else ifeq ($(BOARD), BOARD_STK500)
modules =                       \
       leds                     \
       pwrCtrl                  \
       bspTaskManager           \
       buttons                  \
       battery                  \
       sliders                  \
       sensors                  \
       fakeBSP
else ifeq ($(BOARD), BOARD_STK600)
modules =                       \
       leds                     \
       buttons                  \
       bspTaskManager           \
       fakeBSP
else ifeq ($(BOARD), BOARD_USB_DONGLE)
modules =                       \
       leds                     \
       bspTaskManager           \
       fakeBSP
else ifeq ($(BOARD), BOARD_MESHBEAN_LAN)
modules =                       \
       leds                     \
       buttons                  \
       bspTaskManager           \
       fakeBSP
else ifeq ($(BOARD), BOARD_MESHBEAN_TURBO)
modules =                       \
       leds                     \
       buttons                  \
       sliders                  \
       bspTaskManager           \
       fakeBSP
else ifeq ($(BOARD), BOARD_SAM7X_EK)
modules =                       \
       leds                     \
       joystick                 \
       bspTaskManager           \
       fakeBSP
else ifeq ($(BOARD), BOARD_SAM3S_EK)
modules =                       \
       leds                     \
       bspTaskManager           \
       fakeBSP
else ifeq ($(BOARD), BOARD_SAM4S_PIRRD)
modules =                       \
       leds                     \
       bspTaskManager           \
       fakeBSP
else ifeq ($(BOARD), BOARD_RF231USBRD)
modules =                       \
       leds                     \
       bspTaskManager           \
       fakeBSP
else ifeq ($(BOARD), BOARD_CUSTOM_1)
modules =                       \
       leds                     \
       buttons                  \
       hexSwitch                \
       bspTaskManager           \
       rs485Controller          \
       fakeBSP
else ifeq ($(BOARD), BOARD_CUSTOM_2)
modules =                       \
       leds                     \
       buttons                  \
       hexSwitch                \
       bspTaskManager           \
       rs485Controller          \
       usbController            \
       fakeBSP
else ifeq ($(BOARD), BOARD_CUSTOM_3)
modules =                       \
       leds                     \
       bspTaskManager           \
       fakeBSP
else ifeq ($(BOARD), BOARD_RCB)
modules =                       \
       leds                     \
       bspTaskManager           \
       buttons                  \
       fakeBSP
else ifeq ($(BOARD), BOARD_AVR32_EVK1105)
modules =                       \
       leds                     \
       bspTaskManager           \
       buttons                  \
       fakeBSP
else ifeq ($(BOARD), BOARD_XPLAIN)
modules =                       \
       leds                     \
       bspTaskManager           \
       buttons                  \
       fakeBSP
else ifeq ($(BOARD), BOARD_REB_CBB)
modules =                       \
       leds                     \
       bspTaskManager           \
       buttons                  \
       fakeBSP
else ifeq ($(BOARD), BOARD_SIMULATOR)
modules =                       \
       leds                     \
       bspTaskManager           \
       fakeBSP
endif


objects = $(addsuffix .o,$(addprefix $(BUILDDIR)/objs/,$(modules)))
sources = $(addsuffix .c,$(addprefix $(BUILDDIR)/src/,$(modules)))

###### TARGETS ################
all: component_label LIB_BSP
component_label:
	@echo 
	@echo ----------------------------------------------------
	@echo Board Support library creation.
	@echo ----------------------------------------------------

################
$(BUILDDIR)/objs/%.o: $(BUILDDIR)/src/%.c
	$(CC) $(BSPFLAGS) $^ -o $@

################
LIB_BSP : $(objects)
	$(AR) $(AR_KEYS) $(LIBDIR)/lib$(BSP_LIB).a $(objects)
	$(SIZE) -td $(LIBDIR)/lib$(BSP_LIB).a
################
clean:
	@echo 
	@echo ----------------------------------------------------
	@echo Board Support component cleaning.
	@echo ----------------------------------------------------
	rm -f $(objects) $(LIBS) $(wildcard $(LISTDIR)/*.*)
