COMPONENTS_PATH = ../../..
#include $(COMPONENTS_PATH)/Makerules
include $(MAKERULES)

DEBUG = NO
###### Option to switch on the fake driver ##########
ifndef APP_USE_FAKE_OFD_DRIVER
  APP_USE_FAKE_OFD_DRIVER = 0
endif  

# Hardware flags.
CFLAGS += $(CFLAGS_HW)

##### PATHS FLAGS OF INCLUDES #########
CFLAGS += -I./include
CFLAGS += -I./../include
CFLAGS += -I$(HAL_PATH)/include
CFLAGS += -I$(HAL_HWD_COMMON_PATH)/include
CFLAGS += -I$(SE_PATH)/include

ifeq ($(DEBUG), YES)
  CFLAGS += -D_DEBUG_
endif

#### ASM DEFINES FLAGS #######
ifeq ($(COMPILER_TYPE), GCC)
  ASMFLAGS = $(filter -mmcu%,$(CFLAGS))
  ASM_FILE_EXT = s
endif
ifeq ($(COMPILER_TYPE), IAR)  
  ASMFLAGS  = -s+ # Enables case sensitivity.
  ASMFLAGS += -w+ # Enables warnings.
  ASMFLAGS += -M'<>'# Change asm.macro argument quote chars.
  ASMFLAGS += -t8 # Set tab spacing.
  ASMFLAGS += -u_enhancedCore
  ASMFLAGS += -D__HAS_ENHANCED_CORE__=1
  ASMFLAGS += -D__HAS_MUL__=1
  ASMFLAGS += -D__MEMORY_MODEL__=2
  ifneq (, $(findstring $(HAL), ATMEGA1281 ATMEGA1284 AT90USB1287 ATMEGA128RFA1)) 
    ASMFLAGS += -v3 # Select processor option: Relative jumps do not wrap.  
  endif
  ifneq (, $(findstring $(HAL), ATMEGA2561 ATXMEGA256A3 ATXMEGA256D3 ATMEGA256RFR2))
    ASMFLAGS += -v5 # Select processor option: Relative jumps do not wrap.
  endif
  ifneq (, $(findstring $(HAL), ATXMEGA256A3 ATXMEGA256D3))
    ASMFLAGS += --xmega #Enable Xmega specific instructions
  endif
  ASMFLAGS += -D__HAS_ELPM__=1 
  ASMFLAGS += -S 
  ASM_FILE_EXT = s90
endif

###### LIB ##########
BUILDDIR = .

LIBDIR = ./../lib
LDFLAGS = -L$(LIBDIR)
PROG = OFD.elf
LIBS = $(LIBDIR)/lib$(OFD_LIB).a

driver_label:
	@echo 
	@echo ----------------------------------------------------
	@echo Otau flash driver library creation.
	@echo ----------------------------------------------------

ifneq ($(APP_USE_FAKE_OFD_DRIVER), 1)
modules =               \
       ofdAt25Driver    \
       ofdAt45dbDriver  \
       ofdCommand       \
       ofdCrcService    \
       ofdSpiSerializer

asm_modules =         \
       ofdIntFlashRead
else
modules =             \
       ofdFakeDriver
endif

objects = $(addsuffix .o,$(addprefix $(BUILDDIR)/objs/,$(modules)))
sources = $(addsuffix .c,$(addprefix $(BUILDDIR)/src/,$(modules)))
objects_asm = $(addsuffix .o,$(addprefix $(BUILDDIR)/objs/,$(asm_modules)))
sources_asm = $(addsuffix .$(ASM_FILE_EXT),$(addprefix $(BUILDDIR)/src/,$(asm_modules)))

###### TARGETS ################
all: driver_label LIB_OFD

################ c part ######################
$(BUILDDIR)/objs/%.o: $(BUILDDIR)/src/%.c
	$(CC) $(CFLAGS) $^ -o $@

################ assembler part ######################
$(BUILDDIR)/objs/%.o: $(BUILDDIR)/src/%.$(ASM_FILE_EXT)
	$(AS) $(ASMFLAGS) -o $@ $^

################
LIB_OFD : $(objects) $(objects_asm)
	$(AR) $(AR_KEYS) $(LIBDIR)/lib$(OFD_LIB).a $(objects) $(objects_asm)
	$(SIZE) -td $(LIBDIR)/lib$(OFD_LIB).a
################
clean:
	@echo ----------------------------------------------------
	@echo OTAU flash driver cleaning.
	@echo ----------------------------------------------------
	rm -f $(objects) $(objects_asm) $(LIBS) $(wildcard $(BUILDDIR)/list/*.*)