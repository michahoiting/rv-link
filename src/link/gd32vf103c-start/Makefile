
C_SOURCES =  \
link/gd32vf103c-start/RISCV/gcc/init.c \
link/gd32vf103c-start/RISCV/gcc/handlers.c \
link/gd32vf103c-start/RISCV/drivers/n200_func.c \
link/gd32vf103c-start/GD32VF103_standard_peripheral/system_gd32vf103.c \
link/gd32vf103c-start/GD32VF103_standard_peripheral/Source/gd32vf103_gpio.c \
link/gd32vf103c-start/rvl_jtag.c

C_INCLUDES =  \
-Ilink/gd32vf103c-start \
-Ilink/gd32vf103c-start/GD32VF103_standard_peripheral \
-Ilink/gd32vf103c-start/GD32VF103_standard_peripheral/Include \
-Ilink/gd32vf103c-start/RISCV/drivers

C_DEFS = -DGD32VF103C_START


ASM_SOURCES =  \
link/gd32vf103c-start/RISCV/gcc/entry.S \
link/gd32vf103c-start/RISCV/gcc/start.S

AS_INCLUDES = 

AS_DEFS = 


PREFIX = riscv-none-embed-
MACHINE = -march=rv32i -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8
CFLAGS = $(MACHINE)
ASFLAGS = $(MACHINE)
LDSCRIPT = link/gd32vf103c-start/RISCV/gcc/GD32VF103xB.lds
LIBS = -nostartfiles -nostdlib
LIBDIR = 
LDFLAGS = $(MACHINE) -specs=nano.specs -T$(LDSCRIPT)