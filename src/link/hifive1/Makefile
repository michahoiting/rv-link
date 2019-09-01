
C_SOURCES =  \
link/hifive1/rvl_jtag.c 

C_INCLUDES =  \
-Ilink/hifive1 \
-Ilink/hifive1/include \
-Ilink/hifive1/env \
-Ilink/hifive1/env/freedom-e300-hifive1

C_DEFS = 


ASM_SOURCES =  \
link/hifive1/env/entry.S \
link/hifive1/env/start.S

AS_INCLUDES = 

AS_DEFS = 


PREFIX = riscv-none-embed-
MACHINE = -march=rv32i -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8
CFLAGS = $(MACHINE)
ASFLAGS = $(MACHINE)
LDSCRIPT = link/hifive1/env/freedom-e300-hifive1/flash.lds
LIBS = -nostartfiles -nostdlib
LIBDIR = 
LDFLAGS = $(MACHINE) -specs=nano.specs -T$(LDSCRIPT)