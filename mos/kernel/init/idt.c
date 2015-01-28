#include "idt.h"
#include "../io/terminal.h"
#include "../paging/paging.h"

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
    idt_info = paging_get_idt();
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

    idt_encode_entry(80, (unsigned)&terminal_putchar_syscall, 0x08, 0x8E);
    idt_encode_entry(81, (unsigned)&terminal_init_syscall, 0x08, 0x8E);
    idt_encode_entry(82, (unsigned)&stdin_init_syscall, 0x08, 0x8E);

    idt_encode_entry(90, (unsigned)&scheduler_fork_syscall, 0x08, 0x8E);
    idt_encode_entry(91, (unsigned)&scheduler_pid_syscall, 0x08, 0x8E);
    idt_encode_entry(92, (unsigned)&scheduler_exec_syscall, 0x08, 0x8E);
    idt_encode_entry(93, (unsigned)&scheduler_sleep_syscall, 0x08, 0x8E);
    idt_encode_entry(94, (unsigned)&scheduler_pause_syscall, 0x08, 0x8E);
    idt_encode_entry(95, (unsigned)&scheduler_exec_string_syscall, 0x08, 0x8E);

    idt_encode_entry(100, (unsigned)&pipe_pipe_syscall, 0x08, 0x8E);
    idt_encode_entry(101, (unsigned)&pipe_write_syscall, 0x08, 0x8E);
    idt_encode_entry(102, (unsigned)&pipe_read_syscall, 0x08, 0x8E);

    idt_encode_entry(110, (unsigned)&file_read_syscall, 0x08, 0x8E);
    idt_encode_entry(111, (unsigned)&file_write_syscall, 0x08, 0x8E);
    idt_encode_entry(112, (unsigned)&dir_read_syscall, 0x08, 0x8E);
    construct_idtp();
}

void idt_print_entry(uint32_t i)
{
    printf("IDT ENTRY %d : %d %d %d\n", i, (idt_info->idt[i].hi_offset << 16) + idt_info->idt[i].lo_offset, idt_info->idt[i].selector, idt_info->idt[i].type);
}

void idt_encode_entry(uint32_t i, unsigned long offset, uint16_t selector, uint8_t type_attr)
{
    idt_info->idt[i].lo_offset = (offset & 0xFFFF);
    idt_info->idt[i].selector = selector;
    idt_info->idt[i].zero = 0;
    idt_info->idt[i].type = type_attr;
    idt_info->idt[i].hi_offset = offset >> 16;
}