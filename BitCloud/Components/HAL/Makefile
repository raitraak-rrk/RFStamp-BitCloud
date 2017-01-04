#-------------------------------------------------------------------------------------
# Paths.
HAL_PATH = .

#-------------------------------------------------------------------------------------
# Includes.
include $(HAL_PATH)/Makerules

ifeq ($(HAL), ATMEGA1281)
  TARGET=AVR
endif

ifeq ($(HAL), ATMEGA2561)
  TARGET=AVR
endif

ifeq ($(HAL), ATMEGA1284)
  TARGET=AVR
endif

ifeq ($(HAL), AT90USB1287)
  TARGET=AVR
endif

ifeq ($(HAL), ATMEGA128RFA1)
  TARGET=AVR
endif

ifeq ($(HAL), ATMEGA256RFR2)
  TARGET=AVR
endif

ifeq ($(HAL), ATMEGA2564RFR2)
  TARGET=AVR
endif

ifeq ($(HAL), ATXMEGA128A1)
  TARGET=XMEGA
endif

ifeq ($(HAL), ATXMEGA256A3)
  TARGET=XMEGA
endif

ifeq ($(HAL), ATXMEGA128B1)
  TARGET=XMEGA
endif

ifeq ($(HAL), ATXMEGA256D3)
  TARGET=XMEGA
endif

ifeq ($(HAL), AT91SAM7X256)
  TARGET=ARM7
endif

ifeq ($(HAL), AT91SAM3S4C)
  TARGET=CORTEX
endif

ifeq ($(HAL), ATSAMD20J18)
  TARGET=CORTEXM0
endif

ifeq ($(HAL), ATSAMR21G18A)
  TARGET=CORTEXM0
endif

ifeq ($(HAL), ATSAMR21E18A)
  TARGET=CORTEXM0
endif

ifeq ($(HAL), ATSAMR21E19A)
  TARGET=CORTEXM0
endif

ifeq ($(HAL), AT91SAM4S16C)
  TARGET=CORTEX
endif

ifeq ($(HAL), AT32UC3A0512)
  TARGET=AVR32
endif

ifeq ($(HAL), SIMULATOR)
  TARGET=SIMULATOR
endif

ifeq ($(HAL), WIN)
  TARGET=WIN
endif

ifeq ($(TARGET), AVR)
all:
	make all -C $(COMPONENTS_PATH)/HAL/avr

clean:
	make clean -C $(COMPONENTS_PATH)/HAL/avr
endif

ifeq ($(TARGET), XMEGA)
all:
	make all -C $(COMPONENTS_PATH)/HAL/xmega

clean:
	make clean -C $(COMPONENTS_PATH)/HAL/xmega
endif

ifeq ($(TARGET), ARM7)
all:
	make all -C $(COMPONENTS_PATH)/HAL/arm7tdmi

clean:
	make clean -C $(COMPONENTS_PATH)/HAL/arm7tdmi
endif

ifeq ($(TARGET), CORTEX)
all:
	make all -C $(COMPONENTS_PATH)/HAL/cortexm3

clean:
	make clean -C $(COMPONENTS_PATH)/HAL/cortexm3
endif

ifeq ($(TARGET), CORTEXM0)
all:
	make all -C $(COMPONENTS_PATH)/HAL/cortexm0+

clean:
	make clean -C $(COMPONENTS_PATH)/HAL/cortexm0+
endif

ifeq ($(TARGET), AVR32)
all:
	make all -C $(COMPONENTS_PATH)/HAL/avr32

clean:
	make clean -C $(COMPONENTS_PATH)/HAL/avr32
endif

ifeq ($(TARGET), SIMULATOR)
all:
	make all -C $(COMPONENTS_PATH)/HAL/simulator

clean:  
	make clean -C $(COMPONENTS_PATH)/HAL/simulator
endif

ifeq ($(TARGET), WIN)
all:
	make all -C $(COMPONENTS_PATH)/HAL/pc

clean:  
	make clean -C $(COMPONENTS_PATH)/HAL/pc
endif
