#include "idt.h"

static idt_info_type* idt_info;
struct idt_location idtp;

void set_idtp(void);
static void construct_idtp()
{
    idtp.limit = idt_info->idtp.limit = sizeof(idt_info->idt);
    idtp.offset = idt_info->idtp.offset = (uint32_t)&(idt_info->idt);
    set_idtp();
}

void idt_init()
{
    idt_info = (idt_info_type*)(get_address_of_page(1) + sizeof(gdt_info_type));
    for(uint32_t i = 0; i < IDT_SIZE; i++)
    {
        idt_info->idt[i].lo_offset = 0;
        idt_info->idt[i].selector = 0;
        idt_info->idt[i].zero = 0;
        idt_info->idt[i].type = 0;
        idt_info->idt[i].hi_offset = 0;
    }
    construct_idtp();
}