#include "idt.h"
#include "../interupts/interupts.h"

static struct interrupt_gate idt[IDT_SIZE];
struct idt_location idtp;

static void construct_idtp()
{
    idtp.limit = sizeof(idt);
    idtp.offset = (uint32_t)&idt;
    set_idtp();
}

void idt_init()
{
    for(int i = 0; i < IDT_SIZE; i++)
    {
        idt[i].lo_offset = 0;
        idt[i].selector = 0;
        idt[i].zero = 0;
        idt[i].type = 0;
        idt[i].hi_offset = 0;
    }

    for(int i = 0; i < IDT_SIZE; i++)
        idt_encode_entry(i, (unsigned)&default_interupt, 0x08, 0x8E);
    idt_encode_entry(13, (unsigned)&int_general_protection, 0x08, 0x8E);

    construct_idtp();
}

void idt_print_entry(int i)
{
    terminal_print("IDT ENTRY ");
    terminal_putint(i, 3);
    terminal_print("\n");
    terminal_putint((idt[i].hi_offset << 16) + idt[i].lo_offset, 8);
    terminal_print(" ");
    terminal_putint(idt[i].selector, 5);
    terminal_print(" ");
    terminal_putint(idt[i].type, 4);
    terminal_print("\nIDT ENTRY ");
    terminal_putint(i, 3);
    terminal_print(" FINISH\n");
}

void idt_encode_entry(int i, unsigned long offset, uint16_t selector, uint8_t type_attr)
{
    idt[i].lo_offset = (offset & 0xFFFF);
    idt[i].selector = selector;
    idt[i].zero = 0;
    idt[i].type = type_attr;
    idt[i].hi_offset = offset >> 16;
}