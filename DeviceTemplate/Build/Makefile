############################################################################
#
# MODULE              JN-AN-1162 JenNet-IP Smart Home
#
# DESCRIPTION         DeviceTemplate Makefile
#
############################################################################
#
# This software is owned by NXP B.V. and/or its supplier and is protected
# under applicable copyright laws. All rights are reserved. We grant You,
# and any third parties, a license to use this software solely and
# exclusively on NXP products [NXP Microcontrollers such as JN5168, JN5164]. 
# You, and any third parties must reproduce the copyright and warranty notice
# and any other legend of ownership on each copy or partial copy of the 
# software.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#
# Copyright NXP B.V. 2014. All rights reserved
#
############################################################################
# Application target name
TARGET ?= DeviceTemplate

##############################################################################
# Default SDK is the JenNet-IP SDK
JENNIC_SDK ?= JN-SW-4165
JENNIC_SDK_VERSION ?= 1107

##############################################################################
# Path definitions
# Select definitions for either single or multiple targets

# Use if application directory contains multiple targets
ifeq ($(JENNIC_SDK), JN-SW-4165)
SDK_BASE_DIR   	 	?= $(abspath ../../../../sdk/$(JENNIC_SDK))
else
SDK_BASE_DIR   	 	?= $(abspath ../../../..)
endif
APP_BASE            	?= $(abspath ../..)
APP_BLD_DIR		?= $(APP_BASE)/$(CUSTOM)$(TARGET)/Build
APP_SRC_DIR 	        ?= $(APP_BASE)/$(CUSTOM)$(TARGET)/Source
APP_COMMON_SRC_DIR 	?= $(APP_BASE)/Common/Source
APP_BIN_DIR		?= $(APP_BASE)/Binary

##############################################################################
# User definable make parameters that may be overwritten from the command line

# Default target device is the JN5168, (JN5164 also valid)
JENNIC_CHIP ?= JN5168

# Figure out chip family and short chip name
ifeq ($(JENNIC_CHIP), JN5168)
JENNIC_CHIP_FAMILY = JN516x
JENNIC_CHIP_SHORT = 68
endif

ifeq ($(JENNIC_CHIP), JN5164)
JENNIC_CHIP_FAMILY = JN516x
JENNIC_CHIP_SHORT = 64
endif

# Default driver device is DR1174, (but really has no effect on this template router) 
DEVICE_NAME ?= DR1174

# Figure out node type ?
ifeq ($(JENNIC_CHIP_FAMILY), JN516x) 
# JN516x defaults to Router (can be EndDevice or Coordinator)
NODE_TYPE ?= Router
endif

# Calculate character for node type
ifeq ($(NODE_TYPE), Coordinator) 
NODE_TYPE_CHAR = c
endif
ifeq ($(NODE_TYPE), Router) 
NODE_TYPE_CHAR = r
endif
ifeq ($(NODE_TYPE), EndDevice) 
NODE_TYPE_CHAR = e
endif

# Optional config.mk for network id and channel
sinclude $(APP_BASE)/Common/Build/config.mk

# Default network id is 0x11111111
NETWORK_ID ?= 0x11121112

# Default all channels (11-26 selects a single channel)
CHANNEL ?= 0

# Default security on */
SECURITY ?= 1

# Default production build off */
PRODUCTION ?= 0

# Factory reset PDM magic number (0 for default)
FACTORY_RESET_MAGIC ?= 0

# Default DK4 development kit target hardware
JENNIC_PCB ?= DEVKIT4

# Default stack (may be overridden below)
JENNIC_STACK ?= JIP

# Default MAC (may be overridden below)
JENNIC_MAC ?= MiniMac

# Default OND Chipset 0=disabled (may be overidden below) 
OND_CHIPSET     ?= 0
OND_DEVICE_TYPE ?= 0

# Select the OND Chipset
ifeq ($(JENNIC_CHIP), JN5168)
OND_CHIPSET  = 5168
endif

ifeq ($(JENNIC_CHIP), JN5164)
OND_CHIPSET  = 5164
endif

# Library build required 
CFLAGS += -DLIBRARY_BUILD

# Select bootloader
JENNIC_BOOT ?= None

# Define TRACE to use with DBG module
TRACE ?=1

# Debug options define DEBUG for HW debug
#DEBUG ?=HW

# Define which UART to use for debug
DEBUG_PORT ?= UART0

# Production build ?
ifneq ($(PRODUCTION), 0)
# Enforce security
SECURITY := 1
endif

# Device type
JIP_DEVICE_TYPE ?= 0x0002
JIP_DEVICE_TYPE_CHAR = T

 # Set manufactuerer ID (0801 = NXP)
JIP_CR_MANUFACTURER_ID = 0801
JIP_ED_MANUFACTURER_ID = 8801

# Set appropriate IDs and Types for this device
ifeq ($(DEVICE_NAME), DR1174)
ifeq ($(NODE_TYPE), Coordinator) 
JIP_PRODUCT_ID    = 0074
JIP_DEVICE_ID     = 0x$(JIP_CR_MANUFACTURER_ID)$(JIP_PRODUCT_ID)
OND_DEVICE_TYPE   = 0x$(JIP_CR_MANUFACTURER_ID)$(JIP_PRODUCT_ID)
endif
ifeq ($(NODE_TYPE), Router) 
JIP_PRODUCT_ID    = 1174
JIP_DEVICE_ID     = 0x$(JIP_CR_MANUFACTURER_ID)$(JIP_PRODUCT_ID)
OND_DEVICE_TYPE   = 0x$(JIP_CR_MANUFACTURER_ID)$(JIP_PRODUCT_ID)
endif
ifeq ($(NODE_TYPE), EndDevice) 
JIP_PRODUCT_ID    = 1174
JIP_DEVICE_ID     = 0x$(JIP_ED_MANUFACTURER_ID)$(JIP_PRODUCT_ID)
OND_DEVICE_TYPE   = 0x$(JIP_ED_MANUFACTURER_ID)$(JIP_PRODUCT_ID)
endif
endif

# Build JIP Node Name (prefix of Node MIB Name variable
JIP_NODE_NAME ?= $(JIP_DEVICE_TYPE_CHAR)$(JIP_PRODUCT_ID)$(NODE_TYPE_CHAR)$(JENNIC_CHIP_SHORT)

# Flags to build stack MIB persistance support
BLD_MIB_NODE         = 1
ifeq ($(NODE_TYPE), EndDevice)
BLD_MIB_GROUP        = 0
else
BLD_MIB_GROUP        = 1
endif

# Flags to build and register common MIBs
BLD_MIB_NODE_STATUS  = 0
REG_MIB_NODE_STATUS  = 0

BLD_MIB_NODE_CONTROL = 1
REG_MIB_NODE_CONTROL = 1

BLD_MIB_ADC_STATUS   = 1
REG_MIB_ADC_STATUS   = 0

BLD_MIB_NWK_STATUS   = 1
REG_MIB_NWK_STATUS   = 1

BLD_MIB_NWK_SECURITY = 1
REG_MIB_NWK_SECURITY = 1

BLD_MIB_NWK_TEST     = 0
REG_MIB_NWK_TEST     = 0

BLD_MIB_CO_STATUS    = 1
REG_MIB_CO_STATUS    = 1

BLD_MIB_NO2_STATUS   = 1
REG_MIB_NO2_STATUS   = 1

BLD_MIB_O3_STATUS    = 1
REG_MIB_O3_STATUS    = 1

BLD_MIB_H2S_STATUS   = 1
REG_MIB_H2S_STATUS   = 1

BLD_PREH_STATUS      = 1
# Zero version if not passed in on the command line 
VERSION ?= 0
# Need to create a time based version number ? 
ifeq ($(VERSION), 0)
# Get touched time
TEMP := $(shell touch touch.txt)
TOUCHED := $(word 5, $(shell ls -g --time-style=+%w%H%M touch.txt))
VERSION := $(TOUCHED)
else ifeq ($(VERSION), 1)
# Get touched time
TEMP := $(shell touch touch.txt)
TOUCHED := $(word 5, $(shell ls -g --time-style=+%y%m%d touch.txt))
VERSION := $(TOUCHED)
else
TOUCHED := 0
endif

CFLAGS += -I$(LIBINCLUDE)
LDFLAGS += -L$(LIBB)

# Set makefile defines 
CFLAGS += -DMK_TARGET=\"$(TARGET)\"
CFLAGS += -DMK_JENNIC_SDK_VERSION=$(JENNIC_SDK_VERSION)
CFLAGS += -DMK_DEVICE_NAME=\"$(DEVICE_NAME)\"
CFLAGS += -DMK_JIP_NODE_NAME=\"$(JIP_NODE_NAME)\"
ifeq ($(NODE_TYPE), Coordinator)
CFLAGS += -DMK_NODE_TYPE=E_JIP_DEVICE_COORDINATOR
CFLAGS += -DMK_BLD_NODE_TYPE_COORDINATOR
endif
ifeq ($(NODE_TYPE), Router)
CFLAGS += -DMK_NODE_TYPE=E_JIP_DEVICE_ROUTER
endif
ifeq ($(NODE_TYPE), EndDevice)
CFLAGS += -DMK_NODE_TYPE=E_JIP_DEVICE_END_DEVICE
CFLAGS += -DMK_BLD_NODE_TYPE_END_DEVICE
endif
CFLAGS += -DMK_VERSION=\"$(VERSION)\"
CFLAGS += -DMK_JIP_DEVICE_ID=$(JIP_DEVICE_ID)
CFLAGS += -DMK_JIP_DEVICE_TYPE=$(JIP_DEVICE_TYPE)
CFLAGS += -DMK_NETWORK_ID=$(NETWORK_ID)
CFLAGS += -DMK_CHANNEL=$(CHANNEL)
CFLAGS += -DMK_SECURITY=$(SECURITY)
CFLAGS += -DMK_PRODUCTION=$(PRODUCTION)
ifneq ($(FACTORY_RESET_MAGIC), 0)
CFLAGS += -DMK_FACTORY_RESET_MAGIC=$(FACTORY_RESET_MAGIC)
endif
CFLAGS += -DMK_BLD_PREH_STATUS=$(BLD_PREH_STATUS)
CFLAGS += -DMK_SRC_BUS_VOLTS_INT=$(SRC_BUS_VOLTS_INT)
CFLAGS += -DMK_BLD_MIB_NODE=$(BLD_MIB_NODE)
CFLAGS += -DMK_BLD_MIB_GROUP=$(BLD_MIB_GROUP)
CFLAGS += -DMK_BLD_MIB_NODE_STATUS=$(BLD_MIB_NODE_STATUS)
CFLAGS += -DMK_REG_MIB_NODE_STATUS=$(REG_MIB_NODE_STATUS)
CFLAGS += -DMK_BLD_MIB_NODE_CONTROL=$(BLD_MIB_NODE_CONTROL)
CFLAGS += -DMK_REG_MIB_NODE_CONTROL=$(REG_MIB_NODE_CONTROL)
CFLAGS += -DMK_BLD_MIB_ADC_STATUS=$(BLD_MIB_ADC_STATUS)
CFLAGS += -DMK_REG_MIB_ADC_STATUS=$(REG_MIB_ADC_STATUS)
CFLAGS += -DMK_BLD_MIB_NWK_STATUS=$(BLD_MIB_NWK_STATUS)
CFLAGS += -DMK_REG_MIB_NWK_STATUS=$(REG_MIB_NWK_STATUS)
CFLAGS += -DMK_BLD_MIB_NWK_SECURITY=$(BLD_MIB_NWK_SECURITY)
CFLAGS += -DMK_REG_MIB_NWK_SECURITY=$(REG_MIB_NWK_SECURITY)
CFLAGS += -DMK_BLD_MIB_NWK_TEST=$(BLD_MIB_NWK_TEST)
CFLAGS += -DMK_REG_MIB_NWK_TEST=$(REG_MIB_NWK_TEST)
CFLAGS += -DMK_BLD_MIB_CO_STATUS=$(BLD_MIB_CO_STATUS)
CFLAGS += -DMK_REG_MIB_CO_STATUS=$(REG_MIB_CO_STATUS)
CFLAGS += -DMK_BLD_MIB_NO2_STATUS=$(BLD_MIB_NO2_STATUS)
CFLAGS += -DMK_REG_MIB_NO2_STATUS=$(REG_MIB_NO2_STATUS)
CFLAGS += -DMK_BLD_MIB_O3_STATUS=$(BLD_MIB_O3_STATUS)
CFLAGS += -DMK_REG_MIB_O3_STATUS=$(REG_MIB_O3_STATUS)
CFLAGS += -DMK_BLD_MIB_H2S_STATUS=$(BLD_MIB_H2S_STATUS)
CFLAGS += -DMK_REG_MIB_H2S_STATUS=$(REG_MIB_H2S_STATUS)
CFLAGS += -DEMBEDDED
CFLAGS += -DUSER_VSR_HANDLER
CFLAGS += -fno-common
ifeq ($(TRACE), 1)
CFLAGS += -DDBG_ENABLE
BUILD  = _DEBUG
endif

# Production build disabled ? */
ifeq ($(PRODUCTION), 0)
	# Security disabled ? */
	ifeq ($(SECURITY), 0)
	SECURITY_CHAR = u
	else
	SECURITY_CHAR = s
	endif
else
	SECURITY_CHAR = p
endif

# Build name with non single channel specified 
ifeq ($(CHANNEL), 0)
NAME_PREFIX = $(NETWORK_ID)$(SECURITY_CHAR)
else
NAME_PREFIX = $(NETWORK_ID)$(SECURITY_CHAR)_CH$(CHANNEL)
endif

# Build name with version 
ifeq ($(TOUCHED), 0)
NAME_POSTFIX = $(DEVICE_NAME)_$(NODE_TYPE)_$(JENNIC_CHIP)$(BUILD)_v$(VERSION)
else
NAME_POSTFIX = $(DEVICE_NAME)_$(NODE_TYPE)_$(JENNIC_CHIP)$(BUILD)
endif


##############################################################################
# Application Source files

# Note: Path to source file is found using vpath below, so only .c filename is required
# Router files 
APPSRC += DeviceTemplate.c
APPSRC += Node.c
APPSRC += Address.c
APPSRC += Exception.c
APPSRC += Security.c
#APPSRC += Uart.c
#APPSRC += Table.c
APPSRC += AHI_EEPROM.c
APPSRC += DriverREDsensor.c
APPSRC += DriverOXsensor.c
APPSRC += DriverMICS2614.c
APPSRC += DriverMQ136.c

# Mib Common files
ifneq ($(BLD_MIB_GROUP), 0)
APPSRC += MibGroup.c
APPSRC += MibGroupDec.c
endif
ifneq ($(BLD_MIB_NODE), 0)
APPSRC += MibNode.c
APPSRC += MibNodeDec.c
endif
ifneq ($(BLD_MIB_ADC_STATUS), 0)
APPSRC += MibAdcStatus.c
APPSRC += MibAdcStatusDec.c
endif
ifneq ($(BLD_MIB_NODE_STATUS), 0)
APPSRC += MibNodeStatus.c
APPSRC += MibNodeStatusDec.c
endif
ifneq ($(BLD_MIB_NODE_CONTROL), 0)
APPSRC += MibNodeControl.c
APPSRC += MibNodeControlDec.c
endif
ifneq ($(BLD_MIB_NWK_STATUS), 0)
APPSRC += MibNwkStatus.c
APPSRC += MibNwkStatusDec.c
endif
ifneq ($(BLD_MIB_NWK_SECURITY), 0)
APPSRC += MibNwkSecurity.c
APPSRC += MibNwkSecurityDec.c
endif
ifneq ($(BLD_MIB_NWK_TEST), 0)
APPSRC += MibNwkTest.c
APPSRC += MibNwkTestDec.c
endif
# Node Mib sensors files
ifneq ($(BLD_MIB_CO_STATUS), 0)
APPSRC += MibCOstatus.c
APPSRC += MibCOstatusDec.c
endif
ifneq ($(BLD_MIB_NO2_STATUS), 0)
APPSRC += MibNO2status.c
APPSRC += MibNO2statusDec.c
endif
ifneq ($(BLD_MIB_NO2_STATUS), 0)
APPSRC += MibO3status.c
APPSRC += MibO3statusDec.c
endif
ifneq ($(BLD_MIB_H2S_STATUS), 0)
APPSRC += MibH2Sstatus.c
APPSRC += MibH2SstatusDec.c
endif
# Other files
#APPSRC += Uart.c

##############################################################################
# Additional Application Source directories
# Define any additional application directories outside the application directory
# e.g. for AppQueueApi

ADDITIONAL_SRC_DIR += $(APP_BASE)/$(TARGET)/Source
#ADDITIONAL_SRC_DIR += $(COMPONENTS_BASE_DIR)/Utilities/Source

##############################################################################
# Standard Application header search paths
INCFLAGS += -I$(APP_SRC_DIR)
INCFLAGS += -I$(APP_SRC_DIR)/..
INCFLAGS += -I$(APP_COMMON_SRC_DIR)
INCFLAGS += -I$(ADDITIONAL_SRC_DIR) 
INCFLAGS += -I$(APP_BASE)/MibCommon/Include
INCFLAGS += -I$(APP_BASE)/MibMICS4514/Include
INCFLAGS += -I$(APP_BASE)/MibMICS2614/Include
INCFLAGS += -I$(APP_BASE)/MibMQ136/Include
INCFLAGS += -I$(APP_BASE)/DriverMICS2614
INCFLAGS += -I$(APP_BASE)/DriverMICS4514
INCFLAGS += -I$(APP_BASE)/DriverMQ136
# Application specific include files
INCFLAGS += -I$(COMPONENTS_BASE_DIR)/JenNet/Include
INCFLAGS += -I$(COMPONENTS_BASE_DIR)/6LP/Include
INCFLAGS += -I$(COMPONENTS_BASE_DIR)/OS/Include
INCFLAGS += -I$(COMPONENTS_BASE_DIR)/JIP/Include
INCFLAGS += -I$(COMPONENTS_BASE_DIR)/Utilities/Include
INCFLAGS += -I$(COMPONENTS_BASE_DIR)/Aes/Include

##############################################################################
# Chip family libraries 
APPLIBS += DBG
APPLIBS += Recal
APPLIBS += PDM_EEPROM
APPLIBS += JIP
APPLIBS += JenNet
APPLIBS += 6LP

##############################################################################

# You should not need to edit below this line

##############################################################################
##############################################################################
# Configure for the selected PLatform and stack
include $(SDK_BASE_DIR)/Chip/Common/Build/config.mk
include $(SDK_BASE_DIR)/Platform/Common/Build/Config.mk
include $(SDK_BASE_DIR)/Stack/Common/Build/config.mk

###############################################################################
# OND / 2 stage bootloader setup 
ifneq ($(OND_CHIPSET), 0)
LDFLAGS += -Wl,--defsym,OND_DEV_TYPE=$(OND_DEVICE_TYPE)
LDFLAGS += -Wl,--defsym,OND_CHIPSET=$(OND_CHIPSET)
LDFLAGS += -Wl,--defsym,OND_REVISION=$(VERSION)
endif

##############################################################################

APPOBJS = $(APPSRC:.c=.o)

##############################################################################
# Application dynamic dependencies

APPDEPS = $(APPOBJS:.o=.d)

#########################################################################
# Linker

# Add application libraries before chip specific libraries to linker so
# symbols are resolved correctly (i.e. ordering is significant for GCC)

LDMYLIBS := $(addsuffix _$(JENNIC_CHIP_FAMILY)$(BUILD),$(MYLIBS)) $(LDMYLIBS)
LDLIBS := $(addsuffix _$(JENNIC_CHIP_FAMILY),$(APPLIBS)) $(LDLIBS)
LDSTACKLIBS := $(addsuffix _$(JENNIC_CHIP),$(STACKLIBS)) $(LDSTACKLIBS)

#########################################################################
# Dependency rules

.PHONY: all clean
# Path to directories containing application source 
vpath % $(APP_SRC_DIR):$(APP_COMMON_SRC_DIR):$(ADDITIONAL_SRC_DIR):$(APP_BASE)/MibCommon/Source:$(APP_BASE)/MibMICS4514/Source:$(APP_BASE)/DriverMICS4514:$(APP_BASE)/MibMICS2614/Source:$(APP_BASE)/DriverMICS2614:$(APP_BASE)/MibMQ136/Source:$(APP_BASE)/DriverMQ136

.PRECIOUS: %.elf
# OND Disabled ? 
ifeq ($(OND_CHIPSET), 0)
all: $(APP_BIN_DIR)/$(NAME_PREFIX)_$(TARGET)_$(NAME_POSTFIX)$(BIN_SUFFIX).bin
# OND Enabled ?
else
ifeq ($(JENNIC_CHIP_FAMILY), JN516x)
all: $(APP_BIN_DIR)/$(NAME_PREFIX)_$(TARGET)_$(NAME_POSTFIX)$(BIN_SUFFIX).bin
else
all: $(APP_BIN_DIR)/$(NAME_PREFIX)_$(TARGET)_$(NAME_POSTFIX)$(BIN_SUFFIX).bin $(APP_BIN_DIR)/$(NAME_PREFIX)_$(TARGET)_$(NAME_POSTFIX)$(BIN_SUFFIX).ond
endif
endif

-include $(APPDEPS)
%.d:
	rm -f $*.o 


%.o: %.S
	$(info Assembling $< ...)
	$(CC) -c -o $(subst Source,Build,$@) $(CFLAGS) $(INCFLAGS) $< -MD -MF $*.d -MP
	@echo

%.o: %.c 
	$(info Compiling $< ...)
	$(CC) -c -o $(subst Source,Build,$@) $(CFLAGS) $(INCFLAGS) $< -MD -MF $*.d -MP
	@echo

$(APP_BIN_DIR)/$(NAME_PREFIX)_$(TARGET)_$(NAME_POSTFIX)$(BIN_SUFFIX).elf: $(APPOBJS) $(addsuffix _$(JENNIC_CHIP_FAMILY).a,$(addprefix $(COMPONENTS_BASE_DIR)/Library/lib,$(APPLIBS)))
	$(info Linking $@ ...)
	$(CC) -Wl,--gc-sections -Wl,-u_AppColdStart -Wl,-u_AppWarmStart $(LDFLAGS) -T$(LINKCMD) -o $@ -Wl,--start-group $(APPOBJS) $(addprefix -l,$(LDLIBS)) $(addprefix -l,$(LDSTACKLIBS)) $(addprefix -l,$(LDMYLIBS)$(addprefix -l,$(LDMYLIBS))) -Wl,--end-group -Wl,-Map,$(NAME_PREFIX)_$(TARGET)_$(NAME_POSTFIX)$(BIN_SUFFIX).map 
	@echo
	
%.nm: %.elf
	$(NM) -nS $< > $@

%.dmp: %.elf
	$(OBJDUMP) -d $< > $@
	
# OND Disabled ? 
ifeq ($(OND_CHIPSET), 0)

$(APP_BIN_DIR)/$(NAME_PREFIX)_$(TARGET)_$(NAME_POSTFIX)$(BIN_SUFFIX).bin: $(APP_BIN_DIR)/$(NAME_PREFIX)_$(TARGET)_$(NAME_POSTFIX)$(BIN_SUFFIX).elf 
	$(info Generating binary ...)
	$(OBJCOPY) $< $$( $(OBJDUMP) -h $< | sed -n 's/^ *[0-9]* \(.rom[._[:alnum:]]*\).*/-R \1/p' )
	$(OBJCOPY) -S -O binary $< $@
	$(info JENNIC_CHIP     = $(JENNIC_CHIP))
	$(info OND_CHIPSET     = $(OND_CHIPSET))
	$(info DEVICE_NAME     = $(DEVICE_NAME))
	$(info JIP_DEVICE_TYPE = $(JIP_DEVICE_TYPE))
	$(info JIP_DEVICE_ID   = $(JIP_DEVICE_ID))
	$(info OND_DEVICE_TYPE = $(OND_DEVICE_TYPE))
	$(info VERSION         = $(VERSION))
	${SIZE} --totals $(APPOBJS)
	${SIZE} $<

# OND Enabled  
else

$(NAME_PREFIX)_$(TARGET)_$(NAME_POSTFIX)$(BIN_SUFFIX).img: $(APP_BIN_DIR)/$(NAME_PREFIX)_$(TARGET)_$(NAME_POSTFIX)$(BIN_SUFFIX).elf 
	$(info Generating binary ...)
	$(OBJCOPY) $< $$( $(OBJDUMP) -h $< | sed -n 's/^ *[0-9]* \(.rom[._[:alnum:]]*\).*/-R \1/p' )
	$(OBJCOPY) -S -O binary $< $@
	$(info JENNIC_CHIP     = $(JENNIC_CHIP))
	$(info OND_CHIPSET     = $(OND_CHIPSET))
	$(info DEVICE_NAME     = $(DEVICE_NAME))
	$(info JIP_DEVICE_TYPE = $(JIP_DEVICE_TYPE))
	$(info JIP_DEVICE_ID   = $(JIP_DEVICE_ID))
	$(info OND_DEVICE_TYPE = $(OND_DEVICE_TYPE))
	$(info VERSION         = $(VERSION))
	$(OBJDUMP) -d $< > $(NAME_PREFIX)_$(TARGET)_$(NAME_POSTFIX)$(BIN_SUFFIX).dmp
	${SIZE} --totals $(APPOBJS)
	${SIZE} $<
	
$(APP_BIN_DIR)/%.bin: %.img
	$(info Generating OndFull $(VERSION) ...)
	-$(TOOL_BASE_DIR)/OND/OND_Checksum.exe -6 -m -t 62500 -v $(OND_DEVICE_TYPE) $(OND_CHIPSET) $(VERSION) $< $@
endif

#########################################################################

clean:
	rm -f $(APPOBJS) $(APPDEPS) $(APP_BIN_DIR)/$(NAME_PREFIX)_$(TARGET)_$(NAME_POSTFIX)$(BIN_SUFFIX).bin $(APP_BIN_DIR)/$(NAME_PREFIX)_$(TARGET)_$(NAME_POSTFIX)$(BIN_SUFFIX).elf $(NAME_PREFIX)_$(TARGET)_$(NAME_POSTFIX)$(BIN_SUFFIX).map $(APP_BIN_DIR)/$(NAME_PREFIX)_$(TARGET)_$(NAME_POSTFIX)$(BIN_SUFFIX).ond $(NAME_PREFIX)_$(TARGET)_$(NAME_POSTFIX)$(BIN_SUFFIX).dmp

#########################################################################
