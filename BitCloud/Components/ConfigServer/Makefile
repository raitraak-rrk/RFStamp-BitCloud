COMPONENTS_PATH = ..
include $(MAKERULES)
 
# Compiler flags extending with Confiuration server parameters.
CFLAGS += $(CFLAGS_CS)

# Compiler flags extending with "debug info" compiler flag.
CFLAGS += $(CFLAGS_DBG)

# Hardware flags.
CFLAGS += $(CFLAGS_HW)

##### PATHS FLAGS OF INCLUDES #########
CFLAGS += -I$(CS_PATH)/include/
CFLAGS += -I$(CS_PATH)/include/private
CFLAGS += -I$(SE_PATH)/include
CFLAGS += -I$(ZDO_PATH)/include
CFLAGS += -I$(ZDO_PATH)/include/private
CFLAGS += -I$(NWK_PATH)/include
CFLAGS += -I$(NWK_PATH)/include/private
CFLAGS += -I$(APS_PATH)/include
CFLAGS += -I$(APS_PATH)/include/private
CFLAGS += -I$(MAC_PHY_PATH)/include
CFLAGS += -I$(MAC_PHY_PATH)/MAC_ENV/include
CFLAGS += -I$(MAC_PHY_PATH)/MAC_HWD_PHY/include
CFLAGS += -I$(MAC_PHY_PATH)/MAC_HWI/include
CFLAGS += -I$(HAL_PATH)/include
CFLAGS += -I$(HAL_HWD_COMMON_PATH)/include
CFLAGS += -I$(SSP_PATH)/include
CFLAGS += -I$(PDS_PATH)/include
CFLAGS += -I$(PDS_PATH)/include/private
CFLAGS += -I$(TC_PATH)/include
CFLAGS += -I$(ZCL_PATH)/include

###### LIB #########
BUILDDIR = $(CS_PATH)
LIBDIR = $(BUILDDIR)/lib
LISTDIR = $(BUILDDIR)/list
LDFLAGS = -L$(LIBDIR)
LIB = $(LIBDIR)/lib$(CS_LIB).a

modules = \
  configServer \
  csMem \
  csPersistentMem

objects = $(addsuffix .o,$(addprefix $(BUILDDIR)/objs/,$(modules)))
sources = $(addsuffix .c,$(addprefix $(BUILDDIR)/src/,$(modules)))

###### TARGETS ################
all: component_label $(LIB)
component_label:
	@echo 
	@echo ----------------------------------------------------
	@echo Config Server library creation.
	@echo ----------------------------------------------------

################
$(BUILDDIR)/objs/%.o: $(BUILDDIR)/src/%.c
	$(CC_MSG)
	$(Q)$(CC) $(CFLAGS) $^ -o $@

################
$(LIB): $(objects)
	$(AR_MSG)
	$(Q)$(AR) $(AR_KEYS) $(LIB) $(objects)
	$(SIZE_MSG)
	$(Q)$(SIZE) -td $(LIB)

################
clean:
	@echo 
	@echo ----------------------------------------------------
	@echo Config Server component cleaning.
	@echo ----------------------------------------------------
	rm -f $(PROG) $(BUILDDIR)/objs/*.o $(LIB) $(LISTDIR)/*.*
