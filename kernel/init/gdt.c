#include "gdt.h"
#include "../io/terminal.h"

static struct gdt_entry gdt[GDT_SIZE];
struct gdt_location gdtp;

static void construct_gdtp()
{
    gdtp.limit = sizeof(gdt);
    gdtp.offset = (uint32_t)&gdt;
    set_gdtp();
}

void gdt_init()
{
    for(uint32_t i = 0; i < GDT_SIZE; i++)
    {
        gdt[i].lo_limit = 0;
        gdt[i].lo_offset = 0;
        gdt[i].mid_offset = 0;
        gdt[i].type = 0;
        gdt[i].granularity = 0;
        gdt[i].hi_offset = 0;
    }

    gdt_encode_entry(0, 0, 0, 0,0);
    gdt_encode_entry(1, 0, 0xffffffff, 0x9A, 0xCF);
    gdt_encode_entry(2, 0, 0xffffffff, 0x92, 0xCF);

    construct_gdtp();
}

void gdt_print_entry(uint32_t i)
{
    printf("GDT ENTRY %d : %d %d %d\n", i, (gdt[i].hi_offset << 16) + (gdt[i].mid_offset << 8) + gdt[i].lo_offset, gdt[i].lo_limit, gdt[i].type);
}

void gdt_encode_entry(uint32_t i, uint32_t offset, uint32_t limit, uint8_t type, uint8_t granularity)
{
    gdt[i].lo_offset = (offset & 0xFFFF);
    gdt[i].mid_offset = (offset >> 16) & 0xFF;
    gdt[i].hi_offset = (offset >> 24) & 0xFF;

    gdt[i].lo_limit = (limit & 0xFFFF);
    gdt[i].granularity = ((limit >> 16) & 0x0F);

    gdt[i].granularity |= (granularity & 0xF0);
    gdt[i].type = type;
}