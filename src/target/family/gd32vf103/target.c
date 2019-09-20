#include "rvl-target.h"

typedef struct gd32vf103_target_s
{
    struct pt pt;
}gd32vf103_target_t;

static gd32vf103_target_t gd32vf103_target_i;
#define self gd32vf103_target_i


PT_THREAD(rvl_target_flash_erase(rvl_target_addr_t addr, size_t len, int* err))
{
    PT_BEGIN(&self.pt);

    *err = 0;

    PT_END(&self.pt);
}


PT_THREAD(rvl_target_flash_write(rvl_target_addr_t addr, size_t len, uint8_t* buffer, int* err))
{
    PT_BEGIN(&self.pt);

    *err = 0;

    PT_END(&self.pt);
}


PT_THREAD(rvl_target_flash_done(void))
{
    PT_BEGIN(&self.pt);

    PT_END(&self.pt);
}
