#include "../declarations.h"

#define IDT_SIZE 256

struct interrupt_gate
{
    uint16_t lo_offset;
    uint16_t selector;
    uint8_t  zero;
    uint8_t  type;
    uint16_t hi_offset;
} __attribute__((packed));

struct idt_location
{
    uint16_t  limit;
    uint32_t offset;
} __attribute__((packed));

void idt_init();
void idt_print_entry(int i);
void idt_encode_entry(int i, unsigned long offset, uint16_t selector, uint8_t type_attr);

void set_idtp(void); // from igt.asm

// from interupts.asm
void default_interupt(void);
void int_general_protection(void);