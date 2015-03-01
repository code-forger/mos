#include "scheduler.h"
#include "../init/idt.h"
#include "../init/gdt.h"
#include "../paging/paging.h"
#include "events.h"
#include "../io/hdd.h"
#include "../ELF/elf.h"
#include "../mrfs/mrfs.h"

static uint32_t current_pid;
static uint32_t starting_pid;
static uint32_t next_pid;
static uint32_t source_pid;
static process_table_entry* process_table;
static uint32_t* program_pointers;
static uint32_t kernel_esp, kernel_ebp;

static uint64_t global_ms;

uint32_t fork()
{
    //printf("[scheduler.c] CALL : fork() : %d into %d \n", current_pid, next_pid);
    process_table[next_pid].flags = F_INIT;
    uint32_t pid = next_pid;
    process_table[next_pid].code_physical[0] = current_pid;



    char proc_num_str[7];
    sprintf(proc_num_str,"%d",next_pid);

    char procdir[7+7];
    sprintf(procdir,"/proc/%d/",next_pid);

    char parentprocdir[7+7];
    sprintf(parentprocdir,"/proc/%d/",current_pid);

//    printf("[scheduler.c] INFO : Got names:\n");
//    printf("                   : %s:\n", proc_num_str);
//    printf("                   : %s:\n", procdir);
//    printf("                   : %s:\n", parentprocdir);



    mrfsNewFolder("/proc/", proc_num_str);
    char* f = mrfsReadFile(parentprocdir, "name");
    mrfsNewFile(procdir, "name", f, strlen(f));


    mrfsNewFolder(procdir, "env");
    char envdir[18];
    sprintf(envdir, "/proc/%d/env/", next_pid);
    char parentenvdir[18];
    sprintf(parentenvdir, "/proc/%d/env/", current_pid);

    char* parentPATH = mrfsReadFile(parentenvdir, "PATH");
    char* parentcwd = mrfsReadFile(parentenvdir, "cwd");

    mrfsNewFile(envdir, "PATH", parentPATH, strlen(parentPATH));
    mrfsNewFile(envdir, "cwd", parentcwd, strlen(parentcwd));

    free(parentPATH);
    free(parentcwd);

    next_pid++;

    //printf("[scheduler.c] RETE : fork = %d\n", pid);
    free(f);
    return pid;
}

void scheduler_exec_string(char *program_name)
{
    char* parameters[1];
    parameters[0] = (char*)0;
    scheduler_exec_string_paramters(program_name, parameters);
}

static void set_parameters(char** parameters)
{
    uint32_t* num = (uint32_t*)0x80000000;
    *num = 0;
    for(uint32_t i = 0; parameters[i] != '\0'; i++) (*num)++;

    char ** params = malloc((*num + 1)*sizeof(char*));
    for(uint32_t i = 0; parameters[i] != '\0'; i++)
    {
        params[i] = malloc(strlen(parameters[i])+1);
        sprintf(params[i], "%s", parameters[i]);
    }
    char* p = (char*)0x80000004;

    for(uint32_t i = 0; i < *num; i++)
    {
        sprintf(p, "%s", params[i]);
        p += strlen(p) + 1;
    }
}

void scheduler_exec_string_paramters(char *program_name, char** parameters)
{

    set_parameters(parameters);

    char envdir[19];
    sprintf(envdir, "/proc/%d/env/", current_pid);
    char* path = mrfsReadFile(envdir, "PATH");

    char resloved_path_name[strlen(path) + strlen(program_name) + 1];
    sprintf(resloved_path_name, "%s%s",path, program_name);


    int64_t jump_target = -1;

    if (mrfsFileExists(resloved_path_name))
    {
        jump_target = elf_load(resloved_path_name, &(process_table[current_pid]));
    }
    else
    {

        char* cwd = mrfsReadFile(envdir, "cwd");

        char resloved_cwd_name[strlen(cwd) + strlen(program_name) + 1];
        sprintf(resloved_cwd_name, "%s%s",cwd, program_name);

        if (mrfsFileExists(resloved_cwd_name))
            jump_target = elf_load(resloved_cwd_name, &(process_table[current_pid]));
        free(cwd);
    }

    free(path);


    if (jump_target == -1) // {no such file}
    {
        return;
    }

    char procdir[7+7];

    sprintf(procdir, "/proc/%d/",current_pid);

    //printf("[scheduler.c] INFO : Got names:\n");
    //printf("                   : %s:\n", procdir);

    mrfsWriteFile(procdir, "name", program_name, strlen(program_name));

    //printf("LEAVING SCHEDULER to %h\n", jump_target);

    asm("movl %0, %%esp"::"r"(0xbfffffff));

    asm("jmp %0"::"r"(jump_target));
}

void scheduler_sleep(uint32_t milliseconds)
{
    process_table[current_pid].flags |= F_PAUSED;
    uint64_t time = global_ms + milliseconds;
    events_new_event((scheduler_get_pid()&0xFFFF) + (F_WAKE << 16), time);
}

void scheduler_pause()
{
    process_table[current_pid].flags |= F_PAUSED;
}

void scheduler_pause_process(uint32_t pid)
{
    process_table[pid].flags |= F_PAUSED;
}

void scheduler_wake_process(uint32_t pid)
{
    process_table[pid].flags &= ~(F_PAUSED | F_SKIP);
}

void scheduler_mark_process_as(uint32_t pid, uint32_t flags)
{
    process_table[pid].flags |= flags;
}

void scheduler_unmark_process_as(uint32_t pid, uint32_t flags)
{
    process_table[pid].flags &= ~flags;
}

void scheduler_kill(uint32_t pid)
{
    process_table[pid].flags |= F_DEAD;

    terminal_clear_process(pid);

    char proc_num_str[7] = "0";
    int p = pid;
    int i;

    for (i = p?0:1; p; i++)
    {
        proc_num_str[i] = p%10 + '0';
        p /= 10;
    }

    proc_num_str[i++] = '/';
    proc_num_str[i] = '\0';

    char procdir[7+7] = "/proc/";

    strcpy(procdir+6, proc_num_str);

    mrfsDeleteFolderRecursive(procdir);

}

static void events()
{
    global_ms += 52;
    int64_t event = 0;
    while ((event = events_get_event(global_ms)) >= 0)
    {
        if ((event >> 16) == F_WAKE)\
        {
            scheduler_wake_process(event&0xFFFF);
        }
    }
}

int32_t scheduler_get_next_input(uint32_t current_input)
{
    uint32_t starting_input = current_input;
    for (;;)
    {
        ++current_input;
        current_input = (current_input % next_pid);
        if (current_input == starting_input)
            return -1;
        if (process_table[current_input].flags & F_DEAD)
            continue;
        if (!(process_table[current_input].flags & F_HAS_INPUT))
            continue;
        break;
    }
    return current_input;
}

int32_t scheduler_get_next_process(uint32_t current_process)
{
    uint32_t starting_process = current_process;
    for (;;)
    {
        ++current_process;
        current_process = (current_process % next_pid);
        if (current_process == starting_process)
            return -1;
        if (process_table[current_process].flags & F_DEAD)
            continue;
        break;
    }
    return current_process;
}

int32_t scheduler_get_next_hidden(uint32_t current_process)
{
    uint32_t starting_input = current_process;
    for (;;)
    {
        ++current_process;
        current_process = (current_process % next_pid);
        if (current_process == starting_input)
            return -1;
        if (process_table[current_process].flags & F_DEAD)
            continue;
        if (!(process_table[current_process].flags & F_IS_HIDDEN))
            continue;
        break;
    }
    return current_process;
}

void scheduler_time_interupt()
{
    asm("cli");

    asm("movl %%esp, %0":"=r"(process_table[current_pid].esp)::"ebx");
    asm("movl %%ebp, %0":"=r"(process_table[current_pid].ebp)::"ebx");
    asm("movl %0, %%esp"::"r"(kernel_esp):"ebx");
    asm("movl %0, %%ebp"::"r"(kernel_ebp):"ebx");

    if (process_table[current_pid].io.outpipe != 0 && !(process_table[current_pid].flags & F_DEAD))
        terminal_string_for_process(&process_table[current_pid].io);
    hdd_write_cache();

    events();

    starting_pid = current_pid;
    for(uint32_t i = 0; i < next_pid; i++)
    {
        if (process_table[i].flags & F_INIT)
        {
            source_pid = process_table[i].code_physical[0];

            process_table[i].flags = 0;
            process_table[i].esp = process_table[source_pid].esp;
            process_table[i].ebp = process_table[source_pid].ebp;
            process_table[i].code_size = process_table[source_pid].code_size;
            process_table[i].stack_size = process_table[source_pid].stack_size;
            process_table[i].heap_size = process_table[source_pid].heap_size;
            process_table[i].io.outpipe = 0;
            process_table[i].io.inpipe = 0;
            process_table[i].io.px = 0;
            process_table[i].io.py = 0;
            process_table[i].io.wx = 0;
            process_table[i].io.wy = 0;
            process_table[i].io.column = 0;
            process_table[i].io.row = 0;
            process_table[i].padding = 0xBEEF;

            for (uint32_t p = 0; p < process_table[i].code_size; p++)
                process_table[i].code_physical[p] = paging_copy_virtual_to_new(0x08048000 + 0x1000 * p);

            process_table[i].stack_physical = paging_copy_virtual_to_new(0xbffff000);
            process_table[i].heap_physical = paging_copy_virtual_to_new(0x80000000);
        }
    }

    uint32_t pass_count = 0;
    for (;;)
    {
        ++current_pid;
        current_pid = (current_pid % next_pid);
        if (current_pid == starting_pid)
        {
            if (++pass_count >=2)
            {
                current_pid = 0;
                break;
            }
        }
        if (process_table[current_pid].flags & F_SKIP)
        {
            process_table[current_pid].flags -= F_SKIP;
            continue;
        }
        if (process_table[current_pid].flags & FS_DONT_SCHEDULE)
            continue;
        break;
    }

    for (uint32_t i = 0; i < process_table[current_pid].code_size; i++)
        paging_map_phys_to_virtual(process_table[current_pid].code_physical[i], 0x08048000 + 0x1000 * i);
    paging_map_phys_to_virtual(process_table[current_pid].stack_physical, 0xbffff000);
    paging_map_phys_to_virtual(process_table[current_pid].heap_physical, 0x80000000);

    asm("movl %%esp, %0":"=r"(kernel_esp)::"ebx");
    asm("movl %%ebp, %0":"=r"(kernel_ebp)::"ebx");
    asm("movl %0, %%esp"::"r"(process_table[current_pid].esp):"ebx");
    asm("movl %0, %%ebp"::"r"(process_table[current_pid].ebp):"ebx");

    asm("sti");
}

void scheduler_init(uint32_t esp, uint32_t ebp)
{
    kernel_esp = esp;
    kernel_ebp = ebp;

    global_ms = 0;
    current_pid = 0;
    next_pid = 1;

    process_table = paging_get_process_table();

    program_pointers = paging_get_programs();

    events_init();

    paging_map_new_page_table(0x020);
    paging_map_new_page_table(0x2ff);
    paging_map_new_page_table(0x200);

    uint32_t jump_target = elf_load("/bin/init", process_table);

    process_table[0].stack_physical = paging_map_new_to_virtual(0xBFFFFFFF);
    process_table[0].heap_physical = paging_map_new_to_virtual(0x80000000);

    mrfsNewFolder("/proc/", "0");
    mrfsNewFile("/proc/0/", "name", "/init", 5);

    mrfsNewFolder("/proc/0/", "env");
    mrfsNewFile("/proc/0/env/", "PATH", "/bin/", strlen("/bin/"));
    mrfsNewFile("/proc/0/env/", "cwd", "/", strlen("/"));

    //printf("LEAVING SCHEDULER to %h\n", jump_target);

    asm("movl %0, %%esp"::"r"(0xbfffffff));

    asm("jmp %0"::"r"(jump_target));
}

process_table_entry scheduler_get_process_table_entry(uint32_t pid)
{
    return process_table[pid];
}

process_table_entry* scheduler_get_process_table_entry_for_editing(uint32_t pid)
{
    return &(process_table[pid]);
}


uint32_t scheduler_get_pid()
{
    return current_pid;
}
