#include "interupts.h"

void  c_int_zero_division(void)
{
    terminal_print("ZERO_DIVISION_INTERRUPT_HIT!");
    send_byte_to(MASTER_PIC, 0x20);
}


void  c_int_debugger(void)
{
    terminal_print("DEBUGGER_INTERRUPT_HIT!");
    send_byte_to(MASTER_PIC, 0x20);
}


void  c_int_nmi(void)
{
    terminal_print("NMI_INTERRUPT_HIT!");
    send_byte_to(MASTER_PIC, 0x20);
}


void  c_int_breakpoint(void)
{
    terminal_print("BREAKPOINT_INTERRUPT_HIT!");
    send_byte_to(MASTER_PIC, 0x20);
}


void  c_int_overflow(void)
{
    terminal_print("OVERFLOW_INTERRUPT_HIT!");
    send_byte_to(MASTER_PIC, 0x20);
}


void  c_int_bounds(void)
{
    terminal_print("BOUNDS_INTERRUPT_HIT!");
    send_byte_to(MASTER_PIC, 0x20);
}


void  c_int_invalid_opcode(void)
{
    //terminal_print("INVALID_OPCODE_INTERRUPT_HIT!");
    send_byte_to(MASTER_PIC, 0x20);
}


void  c_int_coprocessor_not_available(void)
{
    terminal_print("COPROCERROR_NOT_AVAILABLE_INTERRUPT_HIT!");
    send_byte_to(MASTER_PIC, 0x20);
}


void  c_int_double_fault(void)
{
    terminal_print("DOUBLEFAULT_INTERRUPT_HIT!");
    send_byte_to(MASTER_PIC, 0x20);
}


void  c_int_coprocessor_segment_overrun(void)
{
    terminal_print("COPROCESSOR_SEGMENT_OVERRUN_INTERRUPT_HIT!");
    send_byte_to(MASTER_PIC, 0x20);
}


void  c_int_invalid_tss(void)
{
    terminal_print("INVALID_TSS_INTERRUPT_HIT!");
    send_byte_to(MASTER_PIC, 0x20);
}


void  c_int_segment_not_present(void)
{
    terminal_print("SEGMENT_NOT_PRESENT_INTERRUPT_HIT!");
    send_byte_to(MASTER_PIC, 0x20);
}


void  c_int_stack_fault(void)
{
    terminal_print("STACK_FAULT_INTERRUPT_HIT!");
    send_byte_to(MASTER_PIC, 0x20);
}


void  c_int_general_protection(void)
{
    terminal_print("GENERAL_PROTECTION_FAULT_INTERRUPT_HIT!");
    send_byte_to(MASTER_PIC, 0x20);
}


void  c_int_page_fault(void)
{
    terminal_print("PAGE_FAULT_INTERRUPT_HIT!");
    send_byte_to(MASTER_PIC, 0x20);
}


void  c_int_reserved(void)
{
    terminal_print("RESERVED_INTERRUPT_HIT!");
    send_byte_to(MASTER_PIC, 0x20);
}


void  c_int_math_fault(void)
{
    terminal_print("MATH_FAULT_INTERRUPT_HIT!");
    send_byte_to(MASTER_PIC, 0x20);
}


void  c_int_alignment_check(void)
{
    terminal_print("ALIGNMENT_CHECK_INTERRUPT_HIT!");
    send_byte_to(MASTER_PIC, 0x20);
}


void  c_int_machine_check(void)
{
    terminal_print("MACHINE_CHECK_INTERRUPT_HIT!");
    send_byte_to(MASTER_PIC, 0x20);
}


void  c_int_simd_floating_point(void)
{
    terminal_print("SIMD_FLOATING_POINT_INTERRUPT_HIT!");
    send_byte_to(MASTER_PIC, 0x20);
}

void  c_default_irq(void)
{
    terminal_print("DEFAULT_IRQ_HIT!");
    send_byte_to(MASTER_PIC, 0x20);
}

void  c_timer_irq(void)
{
    send_byte_to(MASTER_PIC, 0x20);
}

void  c_keyboard_irq(void)
{
    keyboard_interupt();
    send_byte_to(MASTER_PIC, 0x20);
}