#include "rvl-target.h"

static const rvl_target_memory_t memory_map_GD32VF103xB[] = {
        {
                .type = rvl_target_memory_type_ram,
                .start = 0x00000000,
                .length = 0x8000000,
        },
        {
                .type = rvl_target_memory_type_flash,
                .start = 0x08000000,
                .length = 0x20000,
                .blocksize = 0x400,
        },
        {
                .type = rvl_target_memory_type_ram,
                .start = 0x08020000,
                .length = 0xf7fe0000,
        },
};


const rvl_target_memory_t *rvl_target_get_memory_map(void)
{
    return memory_map_GD32VF103xB;
}


size_t rvl_target_get_memory_map_len(void)
{
    return sizeof(memory_map_GD32VF103xB) / sizeof(memory_map_GD32VF103xB[0]);
}

