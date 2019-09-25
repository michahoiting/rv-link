
C_SOURCES =  \
link/gd32vf103c-start/RISCV/gcc/init.c \
link/gd32vf103c-start/RISCV/gcc/handlers.c \
link/gd32vf103c-start/RISCV/stubs/sbrk.c \
link/gd32vf103c-start/RISCV/drivers/n200_func.c \
link/gd32vf103c-start/GD32VF103_standard_peripheral/system_gd32vf103.c \
link/gd32vf103c-start/GD32VF103_standard_peripheral/Source/gd32vf103_gpio.c \
link/gd32vf103c-start/GD32VF103_standard_peripheral/Source/gd32vf103_rcu.c \
link/gd32vf103c-start/GD32VF103_standard_peripheral/Source/gd32vf103_timer.c \
link/gd32vf103c-start/GD32VF103_standard_peripheral/Source/gd32vf103_eclic.c \
link/gd32vf103c-start/GD32VF103_standard_peripheral/Source/gd32vf103_exti.c \
link/gd32vf103c-start/GD32VF103_standard_peripheral/Source/gd32vf103_pmu.c \
link/gd32vf103c-start/GD32VF103_usbfs_driver/Source/drv_usb_core.c \
link/gd32vf103c-start/GD32VF103_usbfs_driver/Source/drv_usb_dev.c \
link/gd32vf103c-start/GD32VF103_usbfs_driver/Source/drv_usbd_int.c \
link/gd32vf103c-start/GD32VF103_usbfs_driver/Source/usbd_core.c \
link/gd32vf103c-start/GD32VF103_usbfs_driver/Source/usbd_enum.c \
link/gd32vf103c-start/GD32VF103_usbfs_driver/Source/usbd_transc.c \
link/gd32vf103c-start/gd32vf103_hw.c \
link/gd32vf103c-start/gd32vf103_it.c \
link/gd32vf103c-start/cdc_acm_core.c \
link/gd32vf103c-start/usb-serial.c \
link/gd32vf103c-start/pt-clock.c \
link/gd32vf103c-start/rvl-link.c \
link/gd32vf103c-start/rvl-button.c \
link/gd32vf103c-start/rvl-led.c \
link/gd32vf103c-start/rvl-jtag.c

C_INCLUDES =  \
-Ilink/gd32vf103c-start \
-Ilink/gd32vf103c-start/GD32VF103_standard_peripheral \
-Ilink/gd32vf103c-start/GD32VF103_standard_peripheral/Include \
-Ilink/gd32vf103c-start/GD32VF103_usbfs_driver/Include \
-Ilink/gd32vf103c-start/RISCV/drivers

C_DEFS = -DGD32VF103C_START -DUSE_STDPERIPH_DRIVER -DUSE_USB_FS


ASM_SOURCES =  \
link/gd32vf103c-start/RISCV/gcc/entry.S \
link/gd32vf103c-start/RISCV/gcc/start.S

AS_INCLUDES = 

AS_DEFS = 


PREFIX = riscv-none-embed-
MACHINE = -march=rv32imac -mabi=ilp32 -msmall-data-limit=8
CFLAGS = $(MACHINE)
ASFLAGS = $(MACHINE)
LDSCRIPT = link/gd32vf103c-start/RISCV/gcc/GD32VF103xB.lds
LIBS = -nostartfiles
LIBDIR = 
LDFLAGS = $(MACHINE) -specs=nano.specs -T$(LDSCRIPT)