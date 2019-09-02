#include <stdlib.h>

#include "dmi.h"
#include "dtm-jtag.h"
#include "rvl_assert.h"

typedef struct rvl_dmi_s
{
    struct pt pt;
    uint32_t i;
    uint32_t data;
    uint32_t op;
}rvl_dmi_t;

static rvl_dmi_t rvl_dmi_i;
#define self rvl_dmi_i


#define RISCV_DMI_OP_NOP        0
#define RISCV_DMI_OP_READ       1
#define RISCV_DMI_OP_WRITE      2

void rvl_dmi_init(void)
{
    PT_INIT(&self.pt);

    rvl_dtm_init();
}


PT_THREAD(rvl_dmi_nop(uint32_t* prev_data, uint32_t *prev_result))
{
    PT_BEGIN(&self.pt);

    self.data = 0;
    self.op = RISCV_DMI_OP_NOP;

    PT_WAIT_THREAD(&self.pt, rvl_dtm_dmi(0, &self.data, &self.op));

    if(prev_result) {
        *prev_result = self.op;
    }

    if(prev_data) {
        *prev_data = self.data;
    }

    PT_END(&self.pt);
}


PT_THREAD(rvl_dmi_read(uint32_t addr, uint32_t* data, uint32_t *result))
{
    PT_BEGIN(&self.pt);

    self.data = 0;
    self.op = RISCV_DMI_OP_READ;

    PT_WAIT_THREAD(&self.pt, rvl_dtm_dmi(addr, &self.data, &self.op));

    PT_YIELD(&self.pt);

    self.data = 0;
    self.op = RISCV_DMI_OP_NOP;

    PT_WAIT_THREAD(&self.pt, rvl_dtm_dmi(0, &self.data, &self.op));

    if(self.op != 0)
    {
        PT_YIELD(&self.pt);
        PT_WAIT_THREAD(&self.pt, rvl_dtm_dtmcs_dmireset());
    } else {
        *data = self.data;
    }

    if(result) {
        *result = self.op;
    }

    PT_END(&self.pt);
}


PT_THREAD(rvl_dmi_write(uint32_t addr, uint32_t data, uint32_t *result))
{
    PT_BEGIN(&self.pt);

    self.data = data;
    self.op = RISCV_DMI_OP_WRITE;

    PT_WAIT_THREAD(&self.pt, rvl_dtm_dmi(addr, &self.data, &self.op));

    PT_YIELD(&self.pt);

    self.data = 0;
    self.op = RISCV_DMI_OP_NOP;

    PT_WAIT_THREAD(&self.pt, rvl_dtm_dmi(0, &self.data, &self.op));

    if(self.op != 0)
    {
        PT_YIELD(&self.pt);
        PT_WAIT_THREAD(&self.pt, rvl_dtm_dtmcs_dmireset());
    }

    if(result) {
        *result = self.op;
    }

    PT_END(&self.pt);
}

PT_THREAD(rvl_dmi_read_vector(uint32_t start_addr, uint32_t* buffer, uint32_t len, uint32_t *result))
{
    PT_BEGIN(&self.pt);

    for(self.i = 0; self.i < len; self.i++) {
        self.data = 0;
        self.op = RISCV_DMI_OP_READ;
        PT_WAIT_THREAD(&self.pt, rvl_dtm_dmi(start_addr + self.i, &self.data, &self.op));
        if(self.i > 0) {
            buffer[self.i - 1] = self.data;
        }
        PT_YIELD(&self.pt);
    }

    self.data = 0;
    self.op = RISCV_DMI_OP_NOP;

    PT_WAIT_THREAD(&self.pt, rvl_dtm_dmi(0, &self.data, &self.op));
    buffer[self.i - 1] = self.data;

    *result = self.op;

    if(self.op != 0)
    {
        PT_YIELD(&self.pt);
        PT_WAIT_THREAD(&self.pt, rvl_dtm_dtmcs_dmireset());
    }

    PT_END(&self.pt);
}
