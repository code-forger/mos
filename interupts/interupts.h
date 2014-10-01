#include "../io/terminal.h"
#include "../io/port.h"
#include "../pic/pic.h"


#include "../drivers/keyboard.h"

void c_int_zero_division(void);
void c_int_debugger(void);
void c_int_nmi(void);
void c_int_breakpoint(void);
void c_int_overflow(void);
void c_int_bounds(void);
void c_int_invalid_opcode(void);
void c_int_coprocessor_not_available(void);
void c_int_double_fault(void);
void c_int_coprocessor_segment_overrun(void);
void c_int_invalid_tss(void);
void c_int_segment_not_present(void);
void c_int_stack_fault(void);
void c_int_general_protection(void);
void c_int_page_fault(void);
void c_int_reserved(void);
void c_int_math_fault(void);
void c_int_alignment_check(void);
void c_int_machine_check(void);
void c_int_simd_floating_point(void);

void  c_default_irq(void);
void  c_timer_irq(void);
void  c_keyboard_irq(void);