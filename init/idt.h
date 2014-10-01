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
void int_zero_division(void);
void int_debugger(void);
void int_nmi(void);
void int_breakpoint(void);
void int_overflow(void);
void int_bounds(void);
void int_invalid_opcode(void);
void int_coprocessor_not_available(void);
void int_double_fault(void);
void int_coprocessor_segment_overrun(void);
void int_invalid_tss(void);
void int_segment_not_present(void);
void int_stack_fault(void);
void int_general_protection(void);
void int_page_fault(void);
void int_reserved(void);
void int_math_fault(void);
void int_alignment_check(void);
void int_machine_check(void);
void int_simd_floating_point(void);