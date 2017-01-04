COMPONENTS_PATH = ..
include $(MAKERULES)

# Compiler flags extending with Confiuration server parameters.
CFLAGS += $(CFLAGS_CS)

# Compiler flags extending with "debug info" compiler flag.
CFLAGS += $(CFLAGS_DBG)

# Hardware flags.
CFLAGS += $(CFLAGS_HW)

##### PATHS FLAGS OF INCLUDES #########
CFLAGS += -I$(PDS_PATH)/include
CFLAGS += -I$(PDS_PATH)/include/private
CFLAGS += -I$(CS_PATH)/include
CFLAGS += -I$(BSP_PATH)/include
CFLAGS += -I$(SE_PATH)/include
CFLAGS += -I$(ZDO_PATH)/include
CFLAGS += -I$(NWK_PATH)/include
CFLAGS += -I$(APS_PATH)/include
CFLAGS += -I$(APS_PATH)/include/private
CFLAGS += -I$(MAC_PHY_PATH)/include
CFLAGS += -I$(HAL_PATH)/include
CFLAGS += -I$(HAL_HWD_COMMON_PATH)/include
CFLAGS += -I$(SSP_PATH)/include
CFLAGS += -I$(CS_PATH)/include/private
CFLAGS += -I$(MAC_PHY_PATH)/MAC_ENV/include
CFLAGS += -I$(MAC_PHY_PATH)/MAC_HWD_PHY/include
CFLAGS += -I$(MAC_PHY_PATH)/MAC_HWI/include

#We take address of packed structure members in pdsWriteData.c, IAR for ARM don't like this.
#Suppress this warning, code is correct.
ifeq ($(COMPILER_AND_MICRO_TYPE), IAR_ARM)
  CFLAGS += --diag_suppress Pa039
endif

###### LIB #########
BUILDDIR = .
LIBDIR = $(BUILDDIR)/lib
LISTDIR = $(BUILDDIR)/list
LDFLAGS = -L$(LIBDIR)

objects =                        \
$(BUILDDIR)/objs/pdsDataServer.o \
$(BUILDDIR)/objs/pdsWriteData.o  \
$(BUILDDIR)/objs/pdsCrcService.o
###### TARGETS ################
all: component_label LIB_PERSIST_DATA_SERVER
component_label:
	@echo 
	@echo ----------------------------------------------------
	@echo Persist Data Server library creation.
	@echo ----------------------------------------------------

################
$(objects): 
	$(CC) $(CFLAGS) $^ -o $@
$(BUILDDIR)/objs/pdsDataServer.o: $(BUILDDIR)/src/pdsDataServer.c
$(BUILDDIR)/objs/pdsWriteData.o: $(BUILDDIR)/src/pdsWriteData.c
$(BUILDDIR)/objs/pdsCrcService.o: $(BUILDDIR)/src/pdsCrcService.c
################
LIB_PERSIST_DATA_SERVER : $(objects)
	$(AR) $(AR_KEYS) $(LIBDIR)/lib$(PDS_LIB).a $(objects)
	$(SIZE) -td $(LIBDIR)/lib$(PDS_LIB).a
################

clean:
	@echo 
	@echo ----------------------------------------------------
	@echo Persist Data Server component cleaning.
	@echo ----------------------------------------------------
	rm -f $(objects) $(LIBDIR)/lib$(PDS_LIB).a $(LISTDIR)/*.*
