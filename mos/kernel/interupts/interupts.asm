extern c_int_zero_division
extern c_int_debugger
extern c_int_nmi
extern c_int_breakpoint
extern c_int_overflow
extern c_int_bounds
extern c_int_invalid_opcode
extern c_int_coprocessor_not_available
extern c_int_double_fault
extern c_int_coprocessor_segment_overrun
extern c_int_invalid_tss
extern c_int_segment_not_present
extern c_int_stack_fault
extern c_int_general_protection
extern c_int_page_fault
extern c_int_reserved
extern c_int_math_fault
extern c_int_alignment_check
extern c_int_machine_check
extern c_int_simd_floating_point

extern c_default_irq
extern c_timer_irq
extern c_keyboard_irq

extern c_terminal_putchar_syscall
extern c_terminal_init_syscall
extern c_stdin_init_syscall
extern c_relinquish_input_syscall
extern c_scheduler_fork_syscall
extern c_scheduler_pid_syscall
extern c_scheduler_exec_string_syscall
extern c_scheduler_exec_string_parameters_syscall
extern c_scheduler_sleep_syscall
extern c_scheduler_pause_syscall
extern c_scheduler_kill_syscall
extern c_scheduler_hide_syscall
extern c_scheduler_show_syscall
extern c_pipe_pipe_syscall
extern c_pipe_write_syscall
extern c_pipe_read_syscall
extern c_file_open_syscall
extern c_file_putc_syscall
extern c_file_getc_syscall
extern c_file_opendir_syscall
extern c_file_getfile_syscall
extern c_file_getnamec_syscall
extern c_file_delete_syscall

global int_zero_division
global int_debugger
global int_nmi
global int_breakpoint
global int_overflow
global int_bounds
global int_invalid_opcode
global int_coprocessor_not_available
global int_double_fault
global int_coprocessor_segment_overrun
global int_invalid_tss
global int_segment_not_present
global int_stack_fault
global int_general_protection
global int_page_fault
global int_reserved
global int_math_fault
global int_alignment_check
global int_machine_check
global int_simd_floating_point

global default_irq
global timer_irq
global keyboard_irq

global terminal_putchar_syscall
global terminal_init_syscall
global stdin_init_syscall
global relinquish_input_syscall
global scheduler_fork_syscall
global scheduler_pid_syscall
global scheduler_exec_string_syscall
global scheduler_exec_string_parameters_syscall
global scheduler_sleep_syscall
global scheduler_pause_syscall
global scheduler_kill_syscall
global scheduler_hide_syscall
global scheduler_show_syscall
global pipe_pipe_syscall
global pipe_write_syscall
global pipe_read_syscall
global file_open_syscall
global file_putc_syscall
global file_getc_syscall
global file_opendir_syscall
global file_getfile_syscall
global file_getnamec_syscall
global file_delete_syscall

int_zero_division:
    pushad
    call c_int_zero_division
    popad
    iret

int_debugger:
    pushad
    call c_int_debugger
    popad
    iret

int_nmi:
    pushad
    call c_int_nmi
    popad
    iret

int_breakpoint:
    pushad
    call c_int_breakpoint
    popad
    iret

int_overflow:
    pushad
    call c_int_overflow
    popad
    iret

int_bounds:
    pushad
    call c_int_bounds
    popad
    iret

int_invalid_opcode:
    pushad
    call c_int_invalid_opcode
    popad
    iret

int_coprocessor_not_available:
    pushad
    call c_int_coprocessor_not_available
    popad
    iret

int_double_fault:
    pushad
    call c_int_double_fault
    popad
    iret

int_coprocessor_segment_overrun:
    pushad
    call c_int_coprocessor_segment_overrun
    popad
    iret

int_invalid_tss:
    pushad
    call c_int_invalid_tss
    popad
    iret

int_segment_not_present:
    pushad
    call c_int_segment_not_present
    popad
    iret

int_stack_fault:
    pushad
    call c_int_stack_fault
    popad
    iret

int_general_protection:
    pushad
    call c_int_general_protection
    popad
    iret

int_page_fault:
    pushad
    call c_int_page_fault
    popad
    iret

int_reserved:
    pushad
    call c_int_reserved
    popad
    iret

int_math_fault:
    pushad
    call c_int_math_fault
    popad
    iret

int_alignment_check:
    pushad
    call c_int_alignment_check
    popad
    iret

int_machine_check:
    pushad
    call c_int_machine_check
    popad
    iret

int_simd_floating_point:
    pushad
    call c_int_simd_floating_point
    popad
    iret



default_irq:
    pushad
    call c_default_irq
    popad
    iret

timer_irq:
    cli
    pushad
    call c_timer_irq
    popad
    iret

keyboard_irq:
    pushad
    call c_keyboard_irq
    popad
    iret



terminal_putchar_syscall:
    pushad
    call c_terminal_putchar_syscall
    popad
    iret

terminal_init_syscall:
    pushad
    call c_terminal_init_syscall
    popad
    iret

stdin_init_syscall:
    pushad
    call c_stdin_init_syscall
    popad
    iret

relinquish_input_syscall:
    pushad
    call c_relinquish_input_syscall
    popad
    iret

scheduler_fork_syscall:
    pushad
    call c_scheduler_fork_syscall
    popad
    iret

scheduler_pid_syscall:
    pushad
    call c_scheduler_pid_syscall
    popad
    iret

scheduler_exec_string_syscall:
    cli
    pushad
    call c_scheduler_exec_string_syscall
    popad
    iret

scheduler_exec_string_parameters_syscall:
    cli
    pushad
    call c_scheduler_exec_string_parameters_syscall
    popad
    iret

scheduler_sleep_syscall:
    pushad
    call c_scheduler_sleep_syscall
    popad
    iret

scheduler_pause_syscall:
    pushad
    call c_scheduler_pause_syscall
    popad
    iret

scheduler_kill_syscall:
    pushad
    call c_scheduler_kill_syscall
    popad
    iret

scheduler_hide_syscall:
    pushad
    call c_scheduler_hide_syscall
    popad
    iret

scheduler_show_syscall:
    pushad
    call c_scheduler_show_syscall
    popad
    iret

pipe_pipe_syscall:
    cli
    pushad
    call c_pipe_pipe_syscall
    popad
    sti
    iret

pipe_write_syscall:
    cli
    pushad
    call c_pipe_write_syscall
    popad
    sti
    iret

pipe_read_syscall:
    cli
    pushad
    call c_pipe_read_syscall
    popad
    sti
    iret

file_open_syscall:
    cli
    pushad
    call c_file_open_syscall
    popad
    sti
    iret

file_putc_syscall:
    cli
    pushad
    call c_file_putc_syscall
    popad
    sti
    iret

file_getc_syscall:
    cli
    pushad
    call c_file_getc_syscall
    popad
    sti
    iret

file_opendir_syscall:
    cli
    pushad
    call c_file_opendir_syscall
    popad
    sti
    iret

file_getfile_syscall:
    cli
    pushad
    call c_file_getfile_syscall
    popad
    sti
    iret

file_getnamec_syscall:
    cli
    pushad
    call c_file_getnamec_syscall
    popad
    sti
    iret

file_delete_syscall:
    cli
    pushad
    call c_file_delete_syscall
    popad
    sti
    iret