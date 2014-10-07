#include "../declarations.h"
#include "gdt.h"

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

typedef struct idt_info_type_type
{
    struct idt_location idtp;
    struct interrupt_gate idt[IDT_SIZE];
} __attribute__((packed)) idt_info_type;

void idt_init();
void set_idtp(void);