#include "platform.h"
#include "encoding.h"
#include "rvl-jtag.h"

#define TCK_PIN		5 // GPIO5, Arduino D13
#define TMS_PIN		4 // GPIO4, Arduino D12
#define TDI_PIN		3 // GPIO3, Arduino D11
#define TDO_PIN   	2 // GPIO2, Arduino D10

void rvl_jtag_init(void)
{
	GPIO_REG(GPIO_INPUT_EN) |= 1 << TDO_PIN;
	GPIO_REG(GPIO_OUTPUT_EN) |= (1 << TCK_PIN) | (1 << TMS_PIN) | (1 << TDI_PIN);
	GPIO_REG(GPIO_OUTPUT_VAL) |= (1 << TMS_PIN) | (1 << TDI_PIN);
	GPIO_REG(GPIO_OUTPUT_VAL) &= ~(1 << TCK_PIN);
}

void rvl_jtag_tms_put(int tms)
{
	if(tms) {
		GPIO_REG(GPIO_OUTPUT_VAL) |= 1 << TMS_PIN;
	} else {
		GPIO_REG(GPIO_OUTPUT_VAL) &= ~(1 << TMS_PIN);
	}
}

void rvl_jtag_tdi_put(int tdi)
{
	if(tdi) {
		GPIO_REG(GPIO_OUTPUT_VAL) |= 1 << TDI_PIN;
	} else {
		GPIO_REG(GPIO_OUTPUT_VAL) &= ~(1 << TDI_PIN);
	}
}

void rvl_jtag_tck_put(int tck)
{
	if(tck) {
		GPIO_REG(GPIO_OUTPUT_VAL) |= 1 << TCK_PIN;
	} else {
		GPIO_REG(GPIO_OUTPUT_VAL) &= ~(1 << TCK_PIN);
	}
}

void rvl_jtag_delay_half_period()
{
	uint32_t start = read_csr(mcycle);

	while(read_csr(mcycle) - start < 100);
}

int rvl_jtag_tdo_get()
{
	int tdo;

	if(GPIO_REG(GPIO_INPUT_VAL) & (1 << TDO_PIN)) {
		tdo = 1;
	} else {
		tdo = 0;
	}

	return tdo;
}
