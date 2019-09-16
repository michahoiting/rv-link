#include "interface/rvl-target.h"

static const char memory_map_str[] =
        "<memory-map>"
        "<memory type=\"ram\" start=\"0x00000000\" length=\"0x8000000\"/>"
        "<memory type=\"flash\" start=\"0x08000000\" length=\"0x20000\">"
        "<property name=\"blocksize\">0x400</property>"
        "</memory>"
        "<memory type=\"ram\" start=\"0x08020000\" length=\"0xf7fe0000\"/>"
        "</memory-map>";


const char *rvl_target_get_memory_map(void)
{
    return memory_map_str;
}


size_t rvl_target_get_memory_map_len(void)
{
    return sizeof(memory_map_str) - 1;
}

