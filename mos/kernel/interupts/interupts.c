    #include "interupts.h"
#include "../scheduler/scheduler.h"
#include "../IPC/pipe.h"
#include "../kstdlib/kstdlib.h"
#include "../fs/mrfs.h"

void  c_int_zero_division(void)
{
    printf("ZERO_DIVISION_INTERRUPT_HIT IN PROCESS %h!\n", scheduler_get_pid());
    asm("cli");
    asm("hlt");
    send_byte_to(MASTER_PIC, 0x20);
    scheduler_kill(scheduler_get_pid());
    scheduler_from = 1;
    scheduler_time_interupt();
    send_byte_to(MASTER_PIC, 0x20);
}


void  c_int_debugger(void)
{
    printf("DEBUGGER_INTERRUPT_HIT IN PROCESS %h!\n", scheduler_get_pid());
    asm("cli");
    asm("hlt");
    send_byte_to(MASTER_PIC, 0x20);
    scheduler_kill(scheduler_get_pid());
    scheduler_from = 1;
    scheduler_time_interupt();
    send_byte_to(MASTER_PIC, 0x20);
}


void  c_int_nmi(void)
{
    printf("NMI_INTERRUPT_HIT IN PROCESS %h!\n", scheduler_get_pid());
    asm("cli");
    asm("hlt");
    send_byte_to(MASTER_PIC, 0x20);
    scheduler_kill(scheduler_get_pid());
    scheduler_from = 1;
    scheduler_time_interupt();
    send_byte_to(MASTER_PIC, 0x20);
}


void  c_int_breakpoint(void)
{
    printf("BREAKPOINT_INTERRUPT_HIT IN PROCESS %h!\n", scheduler_get_pid());
    asm("cli");
    asm("hlt");
    send_byte_to(MASTER_PIC, 0x20);
    scheduler_kill(scheduler_get_pid());
    scheduler_from = 1;
    scheduler_time_interupt();
    send_byte_to(MASTER_PIC, 0x20);
}


void  c_int_overflow(void)
{
    printf("OVERFLOW_INTERRUPT_HIT IN PROCESS %h!\n", scheduler_get_pid());
    asm("cli");
    asm("hlt");
    send_byte_to(MASTER_PIC, 0x20);
    scheduler_kill(scheduler_get_pid());
    scheduler_from = 1;
    scheduler_time_interupt();
    send_byte_to(MASTER_PIC, 0x20);
}


void  c_int_bounds(void)
{
    printf("BOUNDS_INTERRUPT_HIT IN PROCESS %h!\n", scheduler_get_pid());
    asm("cli");
    asm("hlt");
    send_byte_to(MASTER_PIC, 0x20);
    scheduler_kill(scheduler_get_pid());
    scheduler_from = 1;
    scheduler_time_interupt();
    send_byte_to(MASTER_PIC, 0x20);
}


void  c_int_invalid_opcode(void)
{
    printf("INVALID_OPCODE_INTERRUPT_HIT IN PROCESS %h!\n", scheduler_get_pid());
    asm("cli");
    asm("hlt");
    send_byte_to(MASTER_PIC, 0x20);
    scheduler_kill(scheduler_get_pid());
    scheduler_from = 1;
    scheduler_time_interupt();
    send_byte_to(MASTER_PIC, 0x20);
}


void  c_int_coprocessor_not_available(void)
{
    printf("COPROCERROR_NOT_AVAILABLE_INTERRUPT_HIT IN PROCESS %h!\n", scheduler_get_pid());
    asm("cli");
    asm("hlt");
    send_byte_to(MASTER_PIC, 0x20);
    scheduler_kill(scheduler_get_pid());
    scheduler_from = 1;
    scheduler_time_interupt();
    send_byte_to(MASTER_PIC, 0x20);
}


void  c_int_double_fault(void)
{
    printf("DOUBLEFAULT_INTERRUPT_HIT IN PROCESS %h!\n", scheduler_get_pid());
    asm("cli");
    asm("hlt");
    send_byte_to(MASTER_PIC, 0x20);
    scheduler_kill(scheduler_get_pid());
    scheduler_from = 1;
    scheduler_time_interupt();
    send_byte_to(MASTER_PIC, 0x20);
}


void  c_int_coprocessor_segment_overrun(void)
{
    printf("COPROCESSOR_SEGMENT_OVERRUN_INTERRUPT_HIT IN PROCESS %h!\n", scheduler_get_pid());
    asm("cli");
    asm("hlt");
    send_byte_to(MASTER_PIC, 0x20);
    scheduler_kill(scheduler_get_pid());
    scheduler_from = 1;
    scheduler_time_interupt();
    send_byte_to(MASTER_PIC, 0x20);
}


void  c_int_invalid_tss(void)
{
    printf("INVALID_TSS_INTERRUPT_HIT IN PROCESS %h!\n", scheduler_get_pid());
    asm("cli");
    asm("hlt");
    send_byte_to(MASTER_PIC, 0x20);
    scheduler_kill(scheduler_get_pid());
    scheduler_from = 1;
    scheduler_time_interupt();
    send_byte_to(MASTER_PIC, 0x20);
}


void  c_int_segment_not_present(void)
{
    printf("SEGMENT_NOT_PRESENT_INTERRUPT_HIT IN PROCESS %h!\n", scheduler_get_pid());
    asm("cli");
    asm("hlt");
    send_byte_to(MASTER_PIC, 0x20);
    scheduler_kill(scheduler_get_pid());
    scheduler_from = 1;
    scheduler_time_interupt();
    send_byte_to(MASTER_PIC, 0x20);
}


void  c_int_stack_fault(void)
{
    printf("STACK_FAULT_INTERRUPT_HIT IN PROCESS %h!\n", scheduler_get_pid());
    asm("cli");
    asm("hlt");
    send_byte_to(MASTER_PIC, 0x20);
    scheduler_kill(scheduler_get_pid());
    scheduler_from = 1;
    scheduler_time_interupt();
    send_byte_to(MASTER_PIC, 0x20);
}


void  c_int_general_protection(void)
{
    printf("GENERAL_PROTECTION_FAULT_INTERRUPT_HIT IN PROCESS %h!\n"), scheduler_get_pid();
    asm("cli");
    asm("hlt");
    send_byte_to(MASTER_PIC, 0x20);
    scheduler_kill(scheduler_get_pid());
    scheduler_from = 1;
    scheduler_time_interupt();
    send_byte_to(MASTER_PIC, 0x20);
}

static uint32_t page;
//static uint32_t dump_mode;


void  c_int_page_fault(void)
{
    asm("mov %%CR2, %0":"=r"(page):);
    //printf("PAGE_FAULT_INTERRUPT_HIT AT %h IN PROCESS %d AKA %h\n",page, scheduler_get_pid(), ((page & 0xfffff000) - 0x80000000) / 0x1000);
    //clean_memory();
    if (((page & 0xfffff000) - 0x80000000) / 0x1000 < 0x1000 && ((page & 0xfffff000) - 0x80000000) / 0x1000 == scheduler_get_process_table_entry(scheduler_get_pid()).heap_size)
    {
        (scheduler_get_process_table_entry_for_editing(scheduler_get_pid())->heap_size) += 1;
        ((uint32_t*)PROCESS_HEAP_TABLE)[((page & 0xfffff000) - 0x80000000) / 0x1000] = paging_map_new_to_virtual(page&0xfffff000);
    }
    else
    {
        printf("PAGE_FAULT_INTERRUPT_HIT AT %h IN PROCESS %d\n", page, scheduler_get_pid());
        asm("cli");
        asm("hlt");
        send_byte_to(MASTER_PIC, 0x20);
        scheduler_kill(scheduler_get_pid());
        scheduler_from = 1;
        scheduler_time_interupt();
    }
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
    //printf("DEFAULT_IRQ_HIT!\n");
    send_byte_to(MASTER_PIC, 0x20);
}

void  c_timer_irq(void)
{
    asm("mov %%edi, %0":"=r"(scheduler_from):);
    scheduler_time_interupt();
    scheduler_from = 0;
    asm("mov %0, %%edi":"=r"(scheduler_from):);
    send_byte_to(MASTER_PIC, 0x20);
}

void  c_keyboard_irq(void)
{
    keyboard_interupt();
    send_byte_to(MASTER_PIC, 0x20);
}

void  c_terminal_putchar_syscall(void)
{
    uint32_t c32;
    asm("mov %%esi, %0":"=r"(c32):);
    char c = (char)c32;
    uint32_t x=0x1010, y=0x2020;
    asm("mov %%edi, %0":"=r"(x):);
    asm("mov %%edx, %0":"=r"(y):);

    terminal_putchar_at_for_process(c, x, y);
    send_byte_to(MASTER_PIC, 0x20);
}

void  c_terminal_init_syscall(void)
{
    PIPE* pipes;
    asm("mov %%esi, %0":"=r"(pipes):);
    terminal_setio(pipes);
    send_byte_to(MASTER_PIC, 0x20);
}

void  c_relinquish_input_syscall(void)
{
    int next_input = scheduler_get_next_process(terminal_get_active_input(), F_HAS_INPUT, F_DEAD);
    if (next_input != -1)
        terminal_set_active_input(next_input);
    send_byte_to(MASTER_PIC, 0x20);
}

void  c_stdin_init_syscall(void)
{
    PIPE* pipes;
    asm("mov %%esi, %0":"=r"(pipes):);
    terminal_setin(pipes);
    send_byte_to(MASTER_PIC, 0x20);
}

void  c_scheduler_fork_syscall(void)
{
    int32_t *pid;
    asm("mov %%esi, %0":"=r"(pid):);
    *pid = fork();
    send_byte_to(MASTER_PIC, 0x20);
}

void  c_scheduler_pid_syscall(void)
{
    uint32_t *pid;
    asm("mov %%esi, %0":"=r"(pid):);
    *pid = scheduler_get_pid();
    send_byte_to(MASTER_PIC, 0x20);
}

void  c_scheduler_exec_string_syscall(void)
{
    char *program_name;
    asm("mov %%esi, %0":"=r"(program_name):);
    scheduler_exec_string(program_name);
    scheduler_kill(scheduler_get_pid());
    send_byte_to(MASTER_PIC, 0x20);
}

void  c_scheduler_exec_string_parameters_syscall(void)
{
    char *program_name;
    char **paramters;
    asm("mov %%esi, %0":"=r"(program_name):);
    asm("mov %%edi, %0":"=r"(paramters):);
    scheduler_exec_string_paramters(program_name, paramters);
    scheduler_kill(scheduler_get_pid());
    send_byte_to(MASTER_PIC, 0x20);
}

void  c_scheduler_sleep_syscall(void)
{
    uint32_t milliseconds;
    asm("mov %%esi, %0":"=r"(milliseconds):);
    scheduler_sleep(milliseconds);
    send_byte_to(MASTER_PIC, 0x20);
}

void  c_scheduler_pause_syscall(void)
{
    scheduler_mark_process_as(scheduler_get_pid(), F_PAUSED);
    send_byte_to(MASTER_PIC, 0x20);
}

void  c_scheduler_kill_syscall(void)
{
    uint32_t pid;
    asm("mov %%esi, %0":"=r"(pid):);
    scheduler_kill(pid);
    send_byte_to(MASTER_PIC, 0x20);
}

void  c_scheduler_hide_syscall(void)
{
    uint32_t pid;
    asm("mov %%esi, %0":"=r"(pid):);
    terminal_hide_process(pid);
    send_byte_to(MASTER_PIC, 0x20);
}


void  c_scheduler_show_syscall(void)
{
    uint32_t pid;
    asm("mov %%esi, %0":"=r"(pid):);
    terminal_show_process(pid);
    send_byte_to(MASTER_PIC, 0x20);
}

void  c_pipe_pipe_syscall(void)
{
    uint32_t loc, *pipe_head, *pipe_tail, *ret;
    asm("mov %%esi, %0":"=r"(loc):);
    asm("mov %%edi, %0":"=r"(pipe_head):);
    asm("mov %%edx, %0":"=r"(pipe_tail):);
    asm("mov %%eax, %0":"=r"(ret):);
    *ret = pipe_create(loc, pipe_head, pipe_tail);
    send_byte_to(MASTER_PIC, 0x20);
}

void  c_pipe_write_syscall(void)
{
    uint32_t pipe, data, *ret;
    asm("mov %%esi, %0":"=r"(pipe):);
    asm("mov %%edi, %0":"=r"(data):);
    asm("mov %%eax, %0":"=r"(ret):);
    uint8_t data8 = (uint8_t)data;
    *ret = pipe_write(pipe, data8);
    send_byte_to(MASTER_PIC, 0x20);
}

void  c_pipe_read_syscall(void)
{
    uint32_t pipe, *ret;
    uint8_t *data;
    asm("mov %%esi, %0":"=r"(pipe):);
    asm("mov %%edi, %0":"=r"(data):);
    asm("mov %%eax, %0":"=r"(ret):);
    *ret = pipe_read(pipe, data);
    send_byte_to(MASTER_PIC, 0x20);
}

void  c_file_open_syscall(void)
{
    char *fname;
    FILE *data;
    uint32_t create;
    asm("mov %%esi, %0":"=r"(fname):);
    asm("mov %%edi, %0":"=r"(data):);
    asm("mov %%edx, %0":"=r"(create):);

    mrfsOpenFile(fname, create, data);

    send_byte_to(MASTER_PIC, 0x20);
}

void  c_file_putc_syscall(void)
{
    int c;
    FILE *fd;
    asm("mov %%esi, %0":"=r"(c):);
    asm("mov %%edi, %0":"=r"(fd):);

    mrfsPutC(fd, c);

    send_byte_to(MASTER_PIC, 0x20);
}

void  c_file_getc_syscall(void)
{
    uint32_t *ret;
    FILE *fd;
    asm("mov %%edi, %0":"=r"(fd):);
    asm("mov %%eax, %0":"=r"(ret):);
    *ret = mrfsGetC(fd);

    send_byte_to(MASTER_PIC, 0x20);
}

void  c_file_opendir_syscall(void)
{
    char *dname;
    FILE *dd;
    int create;
    asm("mov %%esi, %0":"=r"(dname):);
    asm("mov %%edi, %0":"=r"(dd):);
    asm("mov %%edx, %0":"=r"(create):);

    mrfsOpenDir(dname, create, dd);

    send_byte_to(MASTER_PIC, 0x20);
}

void  c_file_getfile_syscall(void)
{
    FILE *ret;
    FILE *dd;
    asm("mov %%edi, %0":"=r"(ret):);
    asm("mov %%eax, %0":"=r"(dd):);
    mrfsGetFile(dd, ret);

    send_byte_to(MASTER_PIC, 0x20);
}

void  c_file_getnamec_syscall(void)
{
    uint32_t *ret;
    FILE *fd;
    asm("mov %%edi, %0":"=r"(fd):);
    asm("mov %%eax, %0":"=r"(ret):);
    *ret = mrfsGetNameC(fd);

    send_byte_to(MASTER_PIC, 0x20);
}


void  c_file_delete_syscall(void)
{
    uint32_t *ret;
    FILE *fd;
    asm("mov %%edi, %0":"=r"(fd):);
    asm("mov %%eax, %0":"=r"(ret):);
    *ret = mrfsDeleteFileWithDescriptor(fd);

    send_byte_to(MASTER_PIC, 0x20);
}


void  c_file_delete_dir_syscall(void)
{
    uint32_t *ret;
    FILE *dd;
    asm("mov %%edi, %0":"=r"(dd):);
    asm("mov %%eax, %0":"=r"(ret):);
    *ret = mrfsDeleteDirWithDescriptor(dd);

    send_byte_to(MASTER_PIC, 0x20);
}


void  c_file_mv_syscall(void)
{
    char *fnew;
    char *fold;
    asm("mov %%esi, %0":"=r"(fold):);
    asm("mov %%edi, %0":"=r"(fnew):);

    mrfsRename(fold, fnew);
    send_byte_to(MASTER_PIC, 0x20);
}


void  c_ticks_ms_syscall(void)
{
    uint32_t *ret;
    asm("mov %%eax, %0":"=r"(ret):);
    *ret = scheduler_ticks_ms();

    send_byte_to(MASTER_PIC, 0x20);
}


void  c_seconds_syscall(void)
{
    uint32_t *ret;
    asm("mov %%eax, %0":"=r"(ret):);
    *ret = scheduler_seconds();

    send_byte_to(MASTER_PIC, 0x20);
}
