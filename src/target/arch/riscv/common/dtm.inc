

void rvl_dtm_init(void)
{
    PT_INIT(&self.pt);
    self.last_jtag_reg = 0;

    rvl_tap_init();
    rvl_tap_go_idle();
}


PT_THREAD(rvl_dtm_idcode(uint32_t* idcode))
{
    PT_BEGIN(&self.pt);

    if(self.last_jtag_reg != RISCV_DTM_JTAG_REG_IDCODE) {
        self.in[0] = RISCV_DTM_JTAG_REG_IDCODE;
        self.last_jtag_reg = RISCV_DTM_JTAG_REG_IDCODE;
        rvl_tap_shift_ir(self.out, self.in, 5);
        PT_YIELD(&self.pt);
    }

    self.in[0] = 0;
    rvl_tap_shift_dr(self.out, self.in, 32);
    PT_YIELD(&self.pt);

    *idcode = self.out[0];

    PT_END(&self.pt);
}


PT_THREAD(rvl_dtm_dtmcs_dmireset(void))
{
    PT_BEGIN(&self.pt);

    if(self.last_jtag_reg != RISCV_DTM_JTAG_REG_DTMCS) {
        self.in[0] = RISCV_DTM_JTAG_REG_DTMCS;
        self.last_jtag_reg = RISCV_DTM_JTAG_REG_DTMCS;
        rvl_tap_shift_ir(self.out, self.in, 5);
        PT_YIELD(&self.pt);
    }

    self.in[0] = RISCV_DTMCS_DMI_RESET;
    rvl_tap_shift_dr(self.out, self.in, 32);
    PT_YIELD(&self.pt);

    if(self.idle) {
        rvl_tap_run(self.idle);
        PT_YIELD(&self.pt);
    }

    PT_END(&self.pt);
}


PT_THREAD(rvl_dtm_run(uint32_t ticks))
{
    PT_BEGIN(&self.pt);

    rvl_tap_run(ticks);
    PT_YIELD(&self.pt);

    PT_END(&self.pt);
}
