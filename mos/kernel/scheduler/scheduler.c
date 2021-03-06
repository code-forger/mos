#include "../ELF/elf.h"
#include "../fs/kmrfs.h"
#include "../fs/mrfs.h"
#include "../init/gdt.h"
#include "../init/idt.h"
#include "../io/hdd.h"
#include "../io/port.h"
#include "../paging/paging.h"
#include "events.h"
#include "scheduler.h"

static uint32_t current_pid;
static uint32_t next_pid;
static uint32_t max_pid;
static uint32_t source_pid;
static process_table_entry* process_table;
static uint32_t kernel_esp, kernel_ebp;

static uint64_t global_ms;

static bool forking; // Are we waiting on a fork?

static plist active_list;
static plist inactive_list;

int32_t fork()
{
    //printf("[scheduler.c] CALL : fork()\n");
    int fork_pid = -1;

    if (next_pid == max_pid) // We have run out or PID's
        fork_pid = scheduler_get_next_process(0, F_DEAD, FS_NONE); // Take one from and dead process
    else
        fork_pid = next_pid++; // Next process is next pid

    if (fork_pid == -1)
        return -1; // Out of process entries

    forking = 1; // inform the next context shift we want a fork.

    process_table[fork_pid].flags = F_INIT; // we are initing the process
    process_table[fork_pid].code_physical[0] = current_pid; // Temporarily fill this slot with the parent ID.
    process_table[fork_pid].cpu_time = 0; // the only other thing that needs to be done now is to reset the CPU timer on the new process

    char proc_num_str[7];
    sprintf(proc_num_str,"%d",fork_pid);

    char procdir[7+7];
    sprintf(procdir,"/info/%d/",fork_pid);

    char parentprocdir[7+7];
    sprintf(parentprocdir,"/info/%d/",current_pid);

//    kprintf("[scheduler.c] INFO : Got names:\n");
//    kprintf("                   : %s:\n", proc_num_str);
//    kprintf("                   : %s:\n", procdir);
//    kprintf("                   : %s:\n", parentprocdir);

    kmrfsNewFolder("/info/", proc_num_str); // Copy name into new info area.
    char* f = kmrfsReadFile(parentprocdir, "name");
    kmrfsNewFile(procdir, "name", f, strlen(f));


    kmrfsNewFolder(procdir, "env"); // Copy env and path too.
    char envdir[18];
    sprintf(envdir, "/info/%d/env/", fork_pid);
    char parentenvdir[18];
    sprintf(parentenvdir, "/info/%d/env/", current_pid);

    char* parentPATH = kmrfsReadFile(parentenvdir, "PATH");
    char* parentcwd = kmrfsReadFile(parentenvdir, "cwd");

    kmrfsNewFile(envdir, "PATH", parentPATH, strlen(parentPATH));
    kmrfsNewFile(envdir, "cwd", parentcwd, strlen(parentcwd));

    free(parentPATH);
    free(parentcwd);
    free(f);

    plist_push_tail(&active_list, fork_pid);

    //printf("[scheduler.c] RETE : fork = %d\n", fork_pid);
    return fork_pid; // That is all that is done for fork now, the rest will be dealt with in the next context shift.
}

void scheduler_exec_string(char *program_name)
{
    char* parameters[1];
    parameters[0] = (char*)0;
    scheduler_exec_string_paramters(program_name, parameters);
}

static void set_parameters(char** parameters)
{
    //printf("[scheduler.c] CALL : set_parameters()\n");
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
        free(params[i]);
    }

    free(params);

    //printf("[scheduler.c] RETE : set_parameters()\n");
}

void scheduler_exec_string_paramters(char *program_name, char** parameters)
{
    //printf("[scheduler.c] CALL : scheduler_exec_string_paramters(%s)\n", program_name);
    set_parameters(parameters);

    char envdir[19];
    sprintf(envdir, "/info/%d/env/", current_pid);
    char* path = kmrfsReadFile(envdir, "PATH");

    char resloved_path_name[strlen(path) + strlen(program_name) + 1];
    sprintf(resloved_path_name, "%s%s",path, program_name);
    //printf("[scheduler.c] INFO : resloved_path_name = %s\n", resloved_path_name);

    int64_t jump_target = -1;

    if (kmrfsFileExists(resloved_path_name))
    {
        //printf("[scheduler.c] INFO : A\n");
        jump_target = elf_load(resloved_path_name, &(process_table[current_pid]));
        //printf("[scheduler.c] INFO : A.A\n");
    }
    else
    {
        //printf("[scheduler.c] INFO : B\n");

        char* cwd = kmrfsReadFile(envdir, "cwd");

        char resloved_cwd_name[strlen(cwd) + strlen(program_name) + 1];
        sprintf(resloved_cwd_name, "%s%s",cwd, program_name);

        if (kmrfsFileExists(resloved_cwd_name))
            jump_target = elf_load(resloved_cwd_name, &(process_table[current_pid]));
        free(cwd);
    }

    free(path);

    if (jump_target == -1) // {no such file}
    {
        return;
    }

    char namedir[7+7 + 8];

    sprintf(namedir, "/info/%d/name",current_pid);

    //printf("[scheduler.c] INFO : Got names:\n");
    //printf("                   : %s:\n", namedir);

    FILE fd;

    mrfsOpenFile(namedir, true, &fd);
    mrfsDeleteFileWithDescriptor(&fd);
    mrfsOpenFile(namedir, true, &fd);

    int len = strlen(program_name);
    for(int i = 0; i < len;i++)
        mrfsPutC(&fd, program_name[i]);


    //printf("LEAVING SCHEDULER to %h\n", jump_target);

    asm("movl %0, %%esp"::"r"(0xbfffffff));

    asm("jmp %0"::"r"(jump_target));
}

void scheduler_sleep(uint32_t milliseconds)
{
    scheduler_mark_process_as(current_pid, F_PAUSED);
    uint64_t time = global_ms + milliseconds;
    events_new_event((scheduler_get_pid()&0xFFFF) + (E_WAKE << 16), time);
}

/*
void scheduler_pause_process(uint32_t pid)
{
    process_table[pid].flags |= F_PAUSED;
}*/

void scheduler_wake_process(uint32_t pid)
{
    //kprintf("waking proc %d\n", pid);
    scheduler_unmark_process_as(pid, F_PAUSED);
    if(plist_remove_from(&inactive_list, pid))
        plist_push_head(&active_list, pid);
    //kprintf("woke proc %d\n", pid);
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
    if (pid > 1 && pid < next_pid && !(process_table[pid].flags & F_DEAD))
    {
        terminal_clear_process(pid);
        terminal_remove_process(pid);

        scheduler_mark_process_as(pid, F_DEAD);
        plist_remove_from(&active_list, pid);
        plist_remove_from(&inactive_list, pid);

        char procdir[7+7] = "/info/";

        sprintf(procdir, "/info/%d/", pid);

        FILE dd;

        mrfsOpenDir(procdir, false, &dd);

        mrfsDeleteDirWithDescriptor(&dd);

        for(uint32_t i = 0; i < process_table[pid].code_size; i++)
            mark_page_free(process_table[pid].code_physical[i]);

        for(uint32_t i = 0; i < process_table[pid].heap_size; i++)
            mark_page_free(((uint32_t*)PROCESS_HEAP_TABLE)[i]);
        mark_page_free(process_table[pid].heap_physical_page);
        mark_page_free(process_table[pid].stack_physical);
    }
}

static void events()
{
    global_ms += scheduler_from?0:10;
    int64_t event = 0;
    while ((event = events_get_event(global_ms)) >= 0)
    {
        if ((event >> 16) == E_WAKE)
        {
            scheduler_wake_process(event&0xFFFF);
        }
    }
}

int32_t scheduler_get_next_process(uint32_t current_input, uint32_t required, uint32_t ignore)
{
    uint32_t starting_input = current_input;
    for (;;)
    {
        ++current_input;
        current_input = (current_input % next_pid);
        if (current_input == starting_input)
            return -1;
        if (process_table[current_input].flags & ignore ||
            !((process_table[current_input].flags & required)==required))
            continue;
        break;
    }
    return current_input;
}

static uint32_t process_history[10];
static uint32_t process_history_count;

static void get_next_to_be_scheduled()
{
    /*starting_pid = current_pid;
    uint32_t failed = 0;
    for (;;)
    {
        if (failed)
        {
            current_pid = 0;
            break;
        }
        ++current_pid;
        current_pid = (current_pid % next_pid);
        if (current_pid == starting_pid)
            failed = 1;
        if (process_table[current_pid].flags & F_SKIP)
        {
            process_table[current_pid].flags -= F_SKIP;
            continue;
        }
        if (process_table[current_pid].flags & FS_DONT_SCHEDULE)
            continue;
        break;
    }*/
    //kprintf("selecting\n");
    if(current_pid > 0 && !(process_table[current_pid].flags & F_DEAD))
        plist_push_tail(&active_list, current_pid);
    do
    {
        int32_t current_pid_next = plist_pop_head(&active_list);
        if (current_pid_next == -1)
            current_pid = 0;
        else
        {
            current_pid = (uint32_t)current_pid_next;
            if(process_table[current_pid].flags & FS_DONT_SCHEDULE)
                plist_push_tail(&inactive_list, current_pid);
            else
                break;
        }
    }
    while(current_pid);

    //kprintf("Selected %d \n", current_pid);

}

void scheduler_time_interupt()
{
    asm("cli");

    asm("movl %%esp, %0":"=r"(process_table[current_pid].esp)::"ebx");
    asm("movl %%ebp, %0":"=r"(process_table[current_pid].ebp)::"ebx");
    asm("movl %0, %%esp"::"r"(kernel_esp):"ebx");
    asm("movl %0, %%ebp"::"r"(kernel_ebp):"ebx");

    if (scheduler_from && scheduler_from != 1)
        scheduler_from = 0;

    if (scheduler_from)
        process_history[process_history_count++] = current_pid;
    else if (process_history_count)
    {
        for(uint32_t i = 0; i < process_history_count; i++)
            process_table[process_history[i]].cpu_time += 10 / process_history_count;
        process_history_count = 0;
    }
    else
        process_table[current_pid].cpu_time += 10;

    //interupted = scheduler_from;

    if (process_table[current_pid].io.outpipe != 0 && !(process_table[current_pid].flags & F_DEAD))
        pterminal_sync(&process_table[current_pid].io);

    events();

    if (forking)
    {
        for(int32_t i = scheduler_get_next_process(0, F_INIT, FS_NONE);
            i != -1;
            i = scheduler_get_next_process(i, F_INIT, FS_NONE))
        {
            source_pid = process_table[i].code_physical[0];

            process_table[i].flags = F_FORKED;
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
            process_table[i].padding = 0xBE;

            for (uint32_t p = 0; p < process_table[i].code_size; p++)
                process_table[i].code_physical[p] = paging_copy_virtual_to_new(0x08048000 + 0x1000 * p);

            process_table[i].stack_physical = paging_copy_virtual_to_new(0xbffff000);

            process_table[i].heap_physical_page = paging_map_new_to_virtual(PROCESS_HEAP_TABLE);

            for (uint32_t j = 0; j < process_table[current_pid].heap_size; j++)
                ((uint32_t*)PROCESS_HEAP_TABLE)[j] = paging_copy_virtual_to_new(0x80000000 + 0x1000 * j);
        }
    }

    get_next_to_be_scheduled();

    for (uint32_t i = 0; i < process_table[current_pid].code_size; i++)
        paging_map_phys_to_virtual(process_table[current_pid].code_physical[i], 0x08048000 + 0x1000 * i);
    paging_map_phys_to_virtual(process_table[current_pid].stack_physical, 0xbffff000);


    paging_map_phys_to_virtual(process_table[current_pid].heap_physical_page,PROCESS_HEAP_TABLE);
    for (uint32_t i = 0; i < process_table[current_pid].heap_size; i++)
        paging_map_phys_to_virtual(((uint32_t*)PROCESS_HEAP_TABLE)[i],0x80000000 + 0x1000 * i);

    //bzr stkprintf("Leaving scheduler to %d\n", current_pid);

    asm("movl %%esp, %0":"=r"(kernel_esp)::"ebx");
    asm("movl %%ebp, %0":"=r"(kernel_ebp)::"ebx");
    asm("movl %0, %%esp"::"r"(process_table[current_pid].esp):"ebx");
    asm("movl %0, %%ebp"::"r"(process_table[current_pid].ebp):"ebx");

    asm("sti");
}

void scheduler_init(uint32_t esp, uint32_t ebp)
{
    plist_init_list(&active_list);
    plist_init_list(&inactive_list);

    kernel_esp = esp;
    kernel_ebp = ebp;

    global_ms = 0;
    current_pid = 0;
    next_pid = 1;
    forking = 0;

    process_history_count = 0;

    process_table = paging_get_process_table();

    max_pid = 0x1000 / sizeof(process_table_entry);

    events_init();

    paging_map_new_page_table(0x020);
    paging_map_new_page_table(0x2ff);
    paging_map_new_page_table(0x200);

    uint32_t jump_target = elf_load("/bin/init", process_table);

    process_table[0].stack_physical = paging_map_new_to_virtual(0xBFFFFFFF);

    process_table[0].heap_size = 1;

    process_table[0].heap_physical_page = paging_map_new_to_virtual(PROCESS_HEAP_TABLE);
    ((uint32_t*)PROCESS_HEAP_TABLE)[0] = paging_map_new_to_virtual(0x80000000);

    kmrfsNewFolder("/info/", "0");

    kmrfsNewFolder("/info/0/", "env");
    kmrfsNewFile("/info/0/env/", "PATH", "/bin/", strlen("/bin/"));
    kmrfsNewFile("/info/0/env/", "cwd", "/", strlen("/"));

    kmrfsNewFile("/info/0/", "name", "System Idle", 11);

    process_table[0].cpu_time = 0;

    //printf("LEAVING SCHEDULER to %h\n", jump_target);

    asm("movl %0, %%esp"::"r"(0xbfffffff));

    asm("jmp %0"::"r"(jump_target));
}

process_table_entry scheduler_get_process_table_entry(uint32_t pid)
{
    if (pid < next_pid && !(process_table[pid].flags & F_DEAD))
        return process_table[pid];
    return process_table[0];
}

process_table_entry* scheduler_get_process_table_entry_for_editing(uint32_t pid)
{
    if (pid < next_pid && !(process_table[pid].flags & F_DEAD))
        return &(process_table[pid]);
    return 0;
}


uint32_t scheduler_get_pid()
{
    return current_pid;
}


static int cmos_ready() {
    send_byte_to(0x70, 10);
    return (get_byte_from(0x71) & 0x80);
}

static int cmos_read_value(uint32_t val)
{
    send_byte_to(0x70, val);
    return get_byte_from(0x71);
}

uint32_t scheduler_ms()
{
    return global_ms;
}

int scheduler_seconds()
{
    uint8_t time[3];
    while (cmos_ready());
    for(int i = 0; i < 3; i++)
        time[i] = cmos_read_value(i*2);

    int status = cmos_read_value(0x0b);

    // clock in 12 hours, convert
    if (!(status & 2) && (time[2] & 0x80)) {
        time[2] = ((time[2] & ~0x80) + 12) % 24;
    }
    // clock in BCD. convert
    if (!(status & 4)) {
        time[0] = (time[0] & 0xf) + ((time[0] / 16) * 10);
        time[1] = (time[1] & 0xf) + ((time[1] / 16) * 10);
        time[2] = (time[2] & 0xf) + ((time[2] / 16) * 10);
    }

    return ((((time[2] * 60) + time[1]) * 60) + time[0]);
}