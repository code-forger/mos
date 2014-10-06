#include "../declarations.h"

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

void gdt_init();
void gdt_print_entry(uint32_t i);
void gdt_encode_entry(uint32_t i, uint32_t offset, uint32_t limit, uint8_t type, uint8_t granularity);

void set_gdtp(void); // from gdt.asm