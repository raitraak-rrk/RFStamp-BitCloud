COMPONENTS_PATH = ../../../..
#include $(COMPONENTS_PATH)/Makerules
include $(MAKERULES)

DEBUG = NO

# Hardware flags.
CFLAGS += $(CFLAGS_HW)

##### PATHS FLAGS OF INCLUDES #########
CFLAGS += -I./include
CFLAGS += -I$(HAL_PATH)/include
CFLAGS += -I./../common/include
CFLAGS += -I$(SE_PATH)/include
CFLAGS += -I$(HAL_PATH)/drivers/include
CFLAGS += -I$(HAL_HWD_COMMON_PATH)/include

#### DEFINES FLAGS #######
ifeq ($(DEBUG), YES)
  CFLAGS += -DUSB_TRACE
endif

###### LIB ##########
BUILDDIR = .
COMMONBUILDDIR = ./../common

LIBDIR = ./../../lib
LDFLAGS = -L$(LIBDIR)
PROG = VCP.elf
LIBS = $(LIBDIR)/lib$(VCP_LIB).a

driver_label:
	@echo 
	@echo ----------------------------------------------------
	@echo VCP library creation.
	@echo ----------------------------------------------------

modules =             \
       vcpCdcProtocol \
       vcpVirtualUsart

commonModules =       \
       usbDescriptors \
       usbEnumeration \
       usbSetupProcess

objects = $(addsuffix .o,$(addprefix $(BUILDDIR)/objs/,$(modules)))
sources = $(addsuffix .c,$(addprefix $(BUILDDIR)/src/,$(modules)))

commonObjects = $(addsuffix .o,$(addprefix $(COMMONBUILDDIR)/objs/,$(commonModules)))
CommonSources = $(addsuffix .c,$(addprefix $(COMMONBUILDDIR)/src/,$(commonModules)))

OBJS  =  $(objects)
OBJS +=  $(commonObjects)

###### TARGETS ################
all: driver_label LIB_VCP
################
$(BUILDDIR)/objs/%.o: $(BUILDDIR)/src/%.c
	$(CC) $(CFLAGS) $^ -o $@
$(COMMONBUILDDIR)/objs/%.o: $(COMMONBUILDDIR)/src/%.c
	$(CC) $(CFLAGS) $^ -o $@

################
LIB_VCP : $(OBJS)
	$(AR) $(AR_KEYS) $(LIBDIR)/lib$(VCP_LIB).a $(OBJS)
	$(SIZE) -td $(LIBDIR)/lib$(VCP_LIB).a
################
clean:
	rm -f $(objects) $(LIBS) $(wildcard $(BUILDDIR)/list/*.*)
	rm -f $(commonObjects) $(LIBS) $(wildcard $(COMMONBUILDDIR)/list/*.*)