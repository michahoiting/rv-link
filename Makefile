OUTPUT_DIR = output

elf = $(OUTPUT_DIR)/gd32vf103c-start+gd32vf103+gdb-server.elf
elf += $(OUTPUT_DIR)/longan-nano+gd32vf103+gdb-server.elf
elf += $(OUTPUT_DIR)/gd32vf103c-start+riscv-debug-spec-v0p13+riscv-prober.elf
#elf += $(OUTPUT_DIR)/gd32vf103c-start+riscv-debug-spec-v0p11+riscv-prober.elf
elf += $(OUTPUT_DIR)/gd32vf103c-start+gd32vf103+test-usb-serial.elf

all: $(elf)

get_link = $(shell echo $@ | awk -F'.' '{print $$1}' | awk -F'/' '{print $$2}' | awk -F'+' '{print $$1}')
get_target = $(shell echo $@ | awk -F'.' '{print $$1}' | awk -F'/' '{print $$2}' | awk -F'+' '{print $$2}')
get_app = $(shell echo $@ | awk -F'.' '{print $$1}' | awk -F'/' '{print $$2}' | awk -F'+' '{print $$3}')

$(OUTPUT_DIR)/%.elf:
	make -C src clean
	make -C src LINK=$(get_link) TARGET=$(get_target) APP=$(get_app)

clean:
	rm -rf output
