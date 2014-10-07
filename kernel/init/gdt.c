#include "gdt.h"
#include "../io/terminal.h"

static gdt_info_type* gdt_info;

void gdt_init()
{
    gdt_info = (gdt_info_type*)get_address_of_page(1);
}

void gdt_print_entry(uint32_t i)
{
    printf("GDT ENTRY %d : %d %d %d\n", i, (gdt_info->gdt[i].hi_offset << 16) + (gdt_info->gdt[i].mid_offset << 8) + gdt_info->gdt[i].lo_offset, gdt_info->gdt[i].lo_limit, gdt_info->gdt[i].type);
}

void gdt_encode_entry(uint32_t i, uint32_t offset, uint32_t limit, uint8_t type, uint8_t granularity)
{
    gdt_info->gdt[i].lo_offset = (offset & 0xFFFF);
    gdt_info->gdt[i].mid_offset = (offset >> 16) & 0xFF;
    gdt_info->gdt[i].hi_offset = (offset >> 24) & 0xFF;

    gdt_info->gdt[i].lo_limit = (limit & 0xFFFF);
    gdt_info->gdt[i].granularity = ((limit >> 16) & 0x0F);

    gdt_info->gdt[i].granularity |= (granularity & 0xF0);
    gdt_info->gdt[i].type = type;
}