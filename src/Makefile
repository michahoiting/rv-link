

###############################################################################
#
ifeq ($(LINK),)
	LINK = gd32vf103c-start
endif

include link/$(LINK)/Makefile


###############################################################################
#
ifeq ($(TARGET),)
	TARGET = gd32vf103
endif

include target/family/$(TARGET)/Makefile


###############################################################################
#
ifeq ($(APP),)
	APP = riscv-prober
endif

include app/$(APP)/Makefile


###############################################################################
#
BUILD_DIR = ../build
OUTPUT_DIR = ../output
EXE = $(LINK)+$(APP)+$(TARGET)


C_SOURCES +=  \
common/rvl_tap.c \
pt/timer.c

C_INCLUDES +=  \
-Icommon \
-Ipt

###############################################################################
#
CC   = $(PREFIX)gcc
AS   = $(PREFIX)gcc -x assembler-with-cpp
COPY = $(PREFIX)objcopy
AR   = $(PREFIX)ar
SIZE = $(PREFIX)size
HEX  = $(COPY) -O ihex
BIN  = $(COPY) -O binary -S
DUMP = $(PREFIX)objdump

OPT = -Og

CFLAGS += -Wall -fdata-sections -ffunction-sections
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)"
CFLAGS += -fstack-usage
CFLAGS += -g $(OPT) $(C_DEFS) $(C_INCLUDES)

ASFLAGS += -Wall -fdata-sections -ffunction-sections
ASFLAGS += -g $(OPT) $(C_DEFS) $(C_INCLUDES) $(AS_DEFS) $(AS_INCLUDES)

LDFLAGS += -g $(OPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(EXE).map,--cref -Wl,--gc-sections


###############################################################################
#

all: $(OUTPUT_DIR)/$(EXE).elf $(OUTPUT_DIR)/$(EXE).hex $(OUTPUT_DIR)/$(EXE).bin $(BUILD_DIR)/$(EXE).disasm

# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
# list of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.S=.o)))
vpath %.S $(sort $(dir $(ASM_SOURCES)))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR) 
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.S Makefile | $(BUILD_DIR)
	$(AS) -c $(ASFLAGS) $< -o $@

$(OUTPUT_DIR)/$(EXE).elf: $(OBJECTS) Makefile link/$(LINK)/Makefile | $(OUTPUT_DIR)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SIZE) $@

$(OUTPUT_DIR)/%.hex: $(OUTPUT_DIR)/%.elf | $(OUTPUT_DIR)
	$(HEX) $< $@
	
$(OUTPUT_DIR)/%.bin: $(OUTPUT_DIR)/%.elf | $(OUTPUT_DIR)
	$(BIN) $< $@	
	
$(BUILD_DIR)/%.disasm: $(OUTPUT_DIR)/%.elf | $(BUILD_DIR)
	$(DUMP) -d $< > $@	
	
$(BUILD_DIR):
	mkdir $@

$(OUTPUT_DIR):
	mkdir $@

#######################################
# clean up
#######################################
clean:
	-rm -fR .dep $(BUILD_DIR)
  
#######################################
# dependencies
#######################################
-include $(shell mkdir .dep 2>/dev/null) $(wildcard .dep/*)
