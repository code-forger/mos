#pragma once
#include "../declarations.h"
#include "../memory/memory.h"

#define GDT_SIZE 3

struct gdt_entry
{
    uint16_t lo_limit;
    uint16_t lo_offset;
    uint8_t  mid_offset;
    uint8_t  type;
    uint8_t  granularity;
    uint8_t  hi_offset;
} __attribute__((packed));

struct gdt_location
{
    uint16_t  limit;
    uint32_t offset;
} __attribute__((packed));

typedef struct gdt_info_type_type
{
    struct gdt_location gdtp;
    struct gdt_entry gdt[GDT_SIZE];
} __attribute__((packed)) gdt_info_type;

void gdt_init();
void gdt_print_entry(uint32_t i);
void gdt_encode_entry(uint32_t i, uint32_t offset, uint32_t limit, uint8_t type, uint8_t granularity);

void set_gdtp(void); // from gdt.asm