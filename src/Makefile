
###############################################################################
# Verbosity control. Use  make V=1  to get verbose builds.

ifeq ($(V),1)
  TRACE_CC =
  TRACE_LD =
  TRACE_AR =
  TRACE_AS =
  TRACE_SIZE =
  TRACE_COPY =
  TRACE_DUMP =
  Q=
else
  TRACE_CC   = @echo "  CC   " $<
  TRACE_LD   = @echo "  LD   " $@
  TRACE_AR   = @echo "  AR   " $@
  TRACE_AS   = @echo "  AS   " $<
  TRACE_SIZE = @echo "  SIZE " $@
  TRACE_COPY = @echo "  COPY " $@
  TRACE_DUMP = @echo "  DUMP " $@
  Q=@
endif

###############################################################################
#
ifeq ($(LINK),)
	LINK = longan-nano
endif

include link/$(LINK)/Makefile

LINK_DEF_ = LINK_$(subst -,_,$(LINK))
LINK_DEF = $(shell echo $(LINK_DEF_) | tr a-z A-Z)

###############################################################################
#
ifeq ($(TARGET),)
	TARGET = gd32vf103
endif

include target/family/$(TARGET)/Makefile

TARGET_DEF_ = TARGET_$(subst -,_,$(TARGET))
TARGET_DEF = $(shell echo $(TARGET_DEF_) | tr a-z A-Z)

###############################################################################
#
ifeq ($(APP),)
	APP = gdb-server
endif

include app/$(APP)/Makefile

APP_DEF_ = APP_$(subst -,_,$(APP))
APP_DEF = $(shell echo $(APP_DEF_) | tr a-z A-Z)

###############################################################################
#
BUILD_DIR = ../build
OUTPUT_DIR = ../output

ifeq ($(APP),gdb-server)
	EXE = $(LINK)+$(TARGET)
else
	EXE = $(LINK)+$(TARGET)+$(APP)
endif


C_SOURCES +=  \
link/rvl-serial.c \
link/rvl-link-stub.c \
pt/timer.c

C_INCLUDES +=  \
-Iinclude \
-I.

C_DEFS += \
-D$(LINK_DEF) \
-D$(TARGET_DEF) \
-D$(APP_DEF) \
-DRVL_ASSERT_EN

###############################################################################
#
CC   = $(PREFIX)gcc
AS   = $(PREFIX)gcc -x assembler-with-cpp
COPY = $(PREFIX)objcopy
AR   = $(PREFIX)ar
SIZE = $(PREFIX)size
DUMP = $(PREFIX)objdump

OPT = -Og

CFLAGS += -Wall -fdata-sections -ffunction-sections -fshort-wchar
CFLAGS += -MMD -MF"$(@:%.o=%.d)"
CFLAGS += -fstack-usage
CFLAGS += -g $(OPT) $(C_DEFS) $(C_INCLUDES)

ASFLAGS += -Wall -fdata-sections -ffunction-sections
ASFLAGS += -MMD -MF"$(@:%.o=%.d)" 
ASFLAGS += -g $(OPT) $(C_DEFS) $(C_INCLUDES) $(AS_DEFS) $(AS_INCLUDES)

LDFLAGS += -g $(OPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(EXE).map,--cref -Wl,--gc-sections


###############################################################################
#

all: \
$(OUTPUT_DIR)/$(EXE).elf \
$(OUTPUT_DIR)/$(EXE).hex \
$(OUTPUT_DIR)/$(EXE).bin \
$(OUTPUT_DIR)/$(EXE).disasm \
$(OUTPUT_DIR)/rv-link.elf \
$(OUTPUT_DIR)/$(EXE).size

# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(C_SOURCES:.c=.o))
DEPS = $(addprefix $(BUILD_DIR)/,$(C_SOURCES:.c=.d))

# list of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(ASM_SOURCES:.S=.o))
DEPS += $(addprefix $(BUILD_DIR)/,$(ASM_SOURCES:.S=.d))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR)
	$(Q)mkdir -p $(dir $@)
	$(TRACE_CC)
	$(Q)$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(<:.c=.lst) $< -o $@

$(BUILD_DIR)/%.o: %.S Makefile | $(BUILD_DIR)
	$(Q)mkdir -p $(dir $@)
	$(TRACE_AS)
	$(Q)$(AS) -c $(ASFLAGS) $< -o $@

$(OUTPUT_DIR)/$(EXE).elf: $(OBJECTS) Makefile link/$(LINK)/Makefile | $(OUTPUT_DIR)
	$(TRACE_LD)
	$(Q)$(CC) $(OBJECTS) $(LDFLAGS) -o $@

$(OUTPUT_DIR)/$(EXE).size: $(OUTPUT_DIR)/$(EXE).elf | $(OUTPUT_DIR)
	$(TRACE_SIZE)
	$(Q)$(SIZE) $< | tee $@

$(OUTPUT_DIR)/rv-link.elf: $(OUTPUT_DIR)/$(EXE).elf | $(OUTPUT_DIR)
	$(Q)rm -f $(OUTPUT_DIR)/rv-link.elf
	$(Q)ln -s $(EXE).elf $(OUTPUT_DIR)/rv-link.elf

$(OUTPUT_DIR)/%.hex: $(OUTPUT_DIR)/%.elf | $(OUTPUT_DIR)
	$(TRACE_COPY)
	$(Q)$(COPY) -O ihex $< $@

$(OUTPUT_DIR)/%.bin: $(OUTPUT_DIR)/%.elf | $(OUTPUT_DIR)
	$(TRACE_COPY)
	$(Q)$(COPY) -O binary -S $< $@

$(OUTPUT_DIR)/%.disasm: $(OUTPUT_DIR)/%.elf | $(OUTPUT_DIR)
	$(TRACE_DUMP)
	$(Q)$(DUMP) -d $< > $@

$(BUILD_DIR):
	$(Q)mkdir $@

$(OUTPUT_DIR):
	$(Q)mkdir $@

#######################################
# clean up
#######################################
clean:
	-rm -fR $(BUILD_DIR)

#######################################
# dependencies
#######################################
-include $(DEPS)
