
C_SOURCES =  \
src/link/hifive1/rvl_jtag.c 

C_INCLUDES =  \
-Isrc/link/hifive1 \
-Isrc/link/hifive1/include \
-Isrc/link/hifive1/env \
-Isrc/link/hifive1/env/freedom-e300-hifive1

C_DEFS = 


ASM_SOURCES =  \
src/link/hifive1/env/entry.S \
src/link/hifive1/env/start.S

AS_INCLUDES = 

AS_DEFS = 


PREFIX = riscv-none-embed-
MACHINE = -march=rv32i -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8
CFLAGS = $(MACHINE)
ASFLAGS = $(MACHINE)
LDSCRIPT = src/link/hifive1/env/freedom-e300-hifive1/flash.lds
LIBS = -nostartfiles -nostdlib
LIBDIR = 
LDFLAGS = $(MACHINE) -specs=nano.specs -T$(LDSCRIPT)