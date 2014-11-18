#pragma once
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
void idt_print_entry(uint32_t i);
void idt_encode_entry(uint32_t i, unsigned long offset, uint16_t selector, uint8_t type_attr);

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

void terminal_putchar_syscall(void);
void terminal_init_syscall(void);
void scheduler_fork_syscall(void);
void scheduler_pid_syscall(void);
void scheduler_exec_syscall(void);
void scheduler_sleep_syscall(void);
void scheduler_pause_syscall(void);
void pipe_pipe_syscall(void);
void pipe_write_syscall(void);
void pipe_read_syscall(void);