    #include "interupts.h"
#include "../scheduler/scheduler.h"
#include "../IPC/pipe.h"
#include "../mrfs/malloc.h"
#include "../mrfs/mrfs.h"

void  c_int_zero_division(void)
{
    printf("ZERO_DIVISION_INTERRUPT_HIT!\n");
    asm("cli");
    asm("hlt");
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
    asm("cli");
    asm("hlt");
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

    uint32_t page;
    asm("mov %%CR2, %0":"=r"(page):);
    printf("PAGE_FAULT_INTERRUPT_HIT AT %h IN PROCESS %d\n",page, scheduler_get_pid());
    dump_memory(0xc0010000);
    asm("cli");
    asm("hlt");
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
    scheduler_time_interupt();
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

void  c_stdin_init_syscall(void)
{
    PIPE* pipes;
    asm("mov %%esi, %0":"=r"(pipes):);
    terminal_setin(pipes);
    send_byte_to(MASTER_PIC, 0x20);
}

void  c_scheduler_fork_syscall(void)
{
    uint32_t *pid;
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

void  c_scheduler_exec_syscall(void)
{
    uint32_t program_number;
    asm("mov %%esi, %0":"=r"(program_number):);
    scheduler_exec(program_number);
    send_byte_to(MASTER_PIC, 0x20);
}

void  c_scheduler_exec_string_syscall(void)
{
    char *program_name;
    asm("mov %%esi, %0":"=r"(program_name):);
    scheduler_exec_string(program_name);
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
    scheduler_pause();
    send_byte_to(MASTER_PIC, 0x20);
}

void  c_scheduler_kill_syscall(void)
{
    uint32_t pid;
    asm("mov %%esi, %0":"=r"(pid):);
    scheduler_kill(pid);
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

void  c_file_read_syscall(void)
{
    char **ret;
    char *fname;
    asm("mov %%esi, %0":"=r"(fname):);
    asm("mov %%eax, %0":"=r"(ret):);
    //printf("Reading file %s\n", fname);
   // printf("[interupts.c] CALL : c_file_read_syscall %s\n", fname);


    int n_length = strlen(fname);
    int fn_length = 0;
    for(int i = n_length-1; i >= 0; i--)
    {
        if (fname[i] != '/')
            fn_length++;
        else
            break;
    }

    int dn_length = n_length - fn_length;

    char fn[fn_length+1];
    fn[fn_length] = '\0';
    char dn[dn_length+1];
    dn[dn_length] = '\0';

    for(int i = 0; i < fn_length; i++)
    {
        fn[i] = fname[i + dn_length];
    }
    for(int i = dn_length - 1; i >= 0; i--)
    {
        dn[i] = fname[i];
    }



    char* buff = mrfsReadFile(dn, fn);
    //printf("[interupts.c] INFO : Read file %s\n", buff);
    uint32_t len = strlen(buff);
    *ret =  malloc_for_process(len+1, 0x80000000);
    strcpy(*ret, buff);
    //free(buff);
    //asm("hlt");
    //init_mem();
    //printf("RETURNING TO USER SPACE\n");
    send_byte_to(MASTER_PIC, 0x20);
}

void  c_file_write_syscall(void)
{
    uint32_t *ret;
    char *fname, *data;
    asm("mov %%esi, %0":"=r"(fname):);
    asm("mov %%edi, %0":"=r"(data):);
    asm("mov %%eax, %0":"=r"(ret):);




    int n_length = strlen(fname);
    int fn_length = 0;
    for(int i = n_length-1; i >= 0; i--)
    {
        if (fname[i] != '/')
            fn_length++;
        else
            break;
    }

    int dn_length = n_length - fn_length;

    char fn[fn_length+1];
    fn[fn_length] = '\0';
    char dn[dn_length+1];
    dn[dn_length] = '\0';

    for(int i = 0; i < fn_length; i++)
    {
        fn[i] = fname[i + dn_length];
    }
    for(int i = dn_length - 1; i >= 0; i--)
    {
        dn[i] = fname[i];
    }





    mrfsDeleteFile(dn, fn);
    strlen(data);
    *ret = mrfsNewFile(dn, fn, data, strlen(data));
    init_mem();
    send_byte_to(MASTER_PIC, 0x20);
}

void  c_dir_read_syscall(void)
{
    char ***ret;
    char *dir;
    asm("mov %%esi, %0":"=r"(dir):);
    asm("mov %%eax, %0":"=r"(ret):);
    char** buff = mrfsGetFolderChildren(dir);
    int count;
    for (count = 0; buff[count][0] != '\0' ;count++);
    count++;
    *ret = malloc_for_process(count*sizeof(char*), 0x80000000);
    for (int i = 0; buff[i][0] != '\0' ;i++)
    {
        //printf("read folder %s\n", buff[i]);
        uint32_t len = strlen(buff[i]);
        (*ret)[i] =  malloc_for_process(len+5, 0x80000000);
        strcpy((*ret)[i], buff[i]);
    }

    (*ret)[--count] = malloc_for_process(sizeof(char)*2, 0x80000000);
    (*ret)[count][0] = '\0';
    //free(buff);
    //asm("hlt");
    //init_mem();
    send_byte_to(MASTER_PIC, 0x20);
}

void  c_file_open_syscall(void)
{
    char *fname;
    FILE *data;
    asm("mov %%esi, %0":"=r"(fname):);
    asm("mov %%edi, %0":"=r"(data):);

    mrfsOpenFile(fname, true, data);

    send_byte_to(MASTER_PIC, 0x20);
}

void  c_file_putc_syscall(void)
{
    int c;
    FILE *data;
    asm("mov %%esi, %0":"=r"(c):);
    asm("mov %%edi, %0":"=r"(data):);
    mrfsPutCAt(data->inode, c, data->index++);

    send_byte_to(MASTER_PIC, 0x20);
}

void  c_file_getc_syscall(void)
{
    uint32_t *ret;
    uint32_t *datain;
    asm("mov %%edi, %0":"=r"(datain):);
    asm("mov %%eax, %0":"=r"(ret):);
    FILE* data = (FILE*)datain;
    *ret = mrfsGetC(data->inode, data->index++);

    send_byte_to(MASTER_PIC, 0x20);
}
