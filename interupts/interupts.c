#include "interupts.h"

void  c_int_zero_division(void)
{
    printf("ZERO_DIVISION_INTERRUPT_HIT!\n");
    send_byte_to(MASTER_PIC, 0x20);
}


void  c_int_debugger(void)
{
    printf("DEBUGGER_INTERRUPT_HIT!\n");
    send_byte_to(MASTER_PIC, 0x20);
}


void  c_int_nmi(void)
{
    printf("NMI_INTERRUPT_HIT!\n");
    send_byte_to(MASTER_PIC, 0x20);
}


void  c_int_breakpoint(void)
{
    printf("BREAKPOINT_INTERRUPT_HIT!\n");
    send_byte_to(MASTER_PIC, 0x20);
}


void  c_int_overflow(void)
{
    printf("OVERFLOW_INTERRUPT_HIT!\n");
    send_byte_to(MASTER_PIC, 0x20);
}


void  c_int_bounds(void)
{
    printf("BOUNDS_INTERRUPT_HIT!\n");
    send_byte_to(MASTER_PIC, 0x20);
}


void  c_int_invalid_opcode(void)
{
    printf("INVALID_OPCODE_INTERRUPT_HIT!\n");
    send_byte_to(MASTER_PIC, 0x20);
}


void  c_int_coprocessor_not_available(void)
{
    printf("COPROCERROR_NOT_AVAILABLE_INTERRUPT_HIT!\n");
    send_byte_to(MASTER_PIC, 0x20);
}


void  c_int_double_fault(void)
{
    printf("DOUBLEFAULT_INTERRUPT_HIT!\n");
    send_byte_to(MASTER_PIC, 0x20);
}


void  c_int_coprocessor_segment_overrun(void)
{
    printf("COPROCESSOR_SEGMENT_OVERRUN_INTERRUPT_HIT!\n");
    send_byte_to(MASTER_PIC, 0x20);
}


void  c_int_invalid_tss(void)
{
    printf("INVALID_TSS_INTERRUPT_HIT!\n");
    send_byte_to(MASTER_PIC, 0x20);
}


void  c_int_segment_not_present(void)
{
    printf("SEGMENT_NOT_PRESENT_INTERRUPT_HIT!\n");
    send_byte_to(MASTER_PIC, 0x20);
}


void  c_int_stack_fault(void)
{
    printf("STACK_FAULT_INTERRUPT_HIT!\n");
    send_byte_to(MASTER_PIC, 0x20);
}


void  c_int_general_protection(void)
{
    printf("GENERAL_PROTECTION_FAULT_INTERRUPT_HIT!\n");
    send_byte_to(MASTER_PIC, 0x20);
}


void  c_int_page_fault(void)
{
    printf("PAGE_FAULT_INTERRUPT_HIT!\n");
    send_byte_to(MASTER_PIC, 0x20);
}


void  c_int_reserved(void)
{
    printf("RESERVED_INTERRUPT_HIT!\n");
    send_byte_to(MASTER_PIC, 0x20);
}


void  c_int_math_fault(void)
{
    printf("MATH_FAULT_INTERRUPT_HIT!\n");
    send_byte_to(MASTER_PIC, 0x20);
}


void  c_int_alignment_check(void)
{
    printf("ALIGNMENT_CHECK_INTERRUPT_HIT!\n");
    send_byte_to(MASTER_PIC, 0x20);
}


void  c_int_machine_check(void)
{
    printf("MACHINE_CHECK_INTERRUPT_HIT!\n");
    send_byte_to(MASTER_PIC, 0x20);
}


void  c_int_simd_floating_point(void)
{
    printf("SIMD_FLOATING_POINT_INTERRUPT_HIT!\n");
    send_byte_to(MASTER_PIC, 0x20);
}

void  c_default_irq(void)
{
    printf("DEFAULT_IRQ_HIT!\n");
    send_byte_to(MASTER_PIC, 0x20);
}

void  c_timer_irq(void)
{
    scheduler_time_interupt();
    send_byte_to(MASTER_PIC, 0x20);
}

void  c_keyboard_irq(void)
{
    keyboard_interupt();
    send_byte_to(MASTER_PIC, 0x20);
}