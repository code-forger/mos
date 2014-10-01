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
    
    idt_encode_entry(0, (unsigned)&int_zero_division, 0x08, 0x8E);
    idt_encode_entry(1, (unsigned)&int_debugger, 0x08, 0x8E);
    idt_encode_entry(2, (unsigned)&int_nmi, 0x08, 0x8E);
    idt_encode_entry(3, (unsigned)&int_breakpoint, 0x08, 0x8E);
    idt_encode_entry(4, (unsigned)&int_overflow, 0x08, 0x8E);
    idt_encode_entry(5, (unsigned)&int_bounds, 0x08, 0x8E);
    idt_encode_entry(6, (unsigned)&int_invalid_opcode, 0x08, 0x8E);
    idt_encode_entry(7, (unsigned)&int_coprocessor_not_available, 0x08, 0x8E);
    idt_encode_entry(8, (unsigned)&int_double_fault, 0x08, 0x8E);
    idt_encode_entry(9, (unsigned)&int_coprocessor_segment_overrun, 0x08, 0x8E);
    idt_encode_entry(10, (unsigned)&int_invalid_tss, 0x08, 0x8E);
    idt_encode_entry(11, (unsigned)&int_segment_not_present, 0x08, 0x8E);
    idt_encode_entry(12, (unsigned)&int_stack_fault, 0x08, 0x8E);
    idt_encode_entry(13, (unsigned)&int_general_protection, 0x08, 0x8E);
    idt_encode_entry(14, (unsigned)&int_page_fault, 0x08, 0x8E);
    idt_encode_entry(15, (unsigned)&int_reserved, 0x08, 0x8E);
    idt_encode_entry(16, (unsigned)&int_math_fault, 0x08, 0x8E);
    idt_encode_entry(17, (unsigned)&int_alignment_check, 0x08, 0x8E);
    idt_encode_entry(18, (unsigned)&int_machine_check, 0x08, 0x8E);
    idt_encode_entry(19, (unsigned)&int_simd_floating_point, 0x08, 0x8E);

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