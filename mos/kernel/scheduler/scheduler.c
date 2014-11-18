#include "scheduler.h"
#include "../init/idt.h"
#include "../init/gdt.h"
#include "../paging/paging.h"
#include "events.h"

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
    //printf("INIT FORK %d to %d\n",current_pid, next_pid);
    process_table[next_pid].flags = F_INIT;
    uint32_t pid = next_pid;
    process_table[next_pid++].code_physical[0] = current_pid;
    return pid;
}

void scheduler_exec(uint32_t program_number)
{
    //printf("exec from %d to %d\n",current_pid,program_number);
    paging_copy_physical_to_virtual(program_pointers[1+(program_number * 2)], 0x08048000);
    paging_copy_physical_to_virtual(program_pointers[1+(program_number * 2)] + 0x1000, 0x08049000);

    process_table[current_pid].io.pipes = 0;
    process_table[current_pid].io.px = 0;
    process_table[current_pid].io.py = 0;
    process_table[current_pid].io.wx = 0;
    process_table[current_pid].io.wy = 0;
    process_table[current_pid].io.column = 0;
    process_table[current_pid].io.row = 0;
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

/*void kill(uint32_t PiD)
{
    //process_table[PiD].flags += F_DEAD;
}*/

/*static void print_proccess_table()
{
    for (uint32_t i = 0; i < next_pid; i++)
    {
        //printf("P%d: %h %h %h %h\n", i, process_table[i].esp, process_table[i].ebp, process_table[i].code_physical, process_table[i].stack_physical);
    }
}*/

static void events()
{
    global_ms += 52;
    int64_t event = 0;
    while ((event = events_get_event(global_ms)) >= 0)
    {
        if ((event >> 16) == F_WAKE)
        {
            process_table[(event&0xFFFF)].flags = 0;
        }
    }
}

void scheduler_time_interupt()
{
    asm("cli");

    if (process_table[current_pid].io.pipes != 0)
        terminal_string_for_process(&process_table[current_pid].io);
    //save esp and ebp into process table
    asm("movl %%esp, %0":"=r"(process_table[current_pid].esp)::"ebx");
    asm("movl %%ebp, %0":"=r"(process_table[current_pid].ebp)::"ebx");
    //printf("1. GOT ESP %h, %h\n", process_table[current_pid].esp, process_table[current_pid].ebp);
    asm("movl %0, %%esp"::"r"(kernel_esp):"ebx");
    asm("movl %0, %%ebp"::"r"(kernel_ebp):"ebx");
    //printf("2. KERNEL ESP IS AT: %h\n", kernel_esp);
    events();

    // INIT all processes marked for init
    starting_pid = current_pid;
    for(uint32_t i = 0; i < next_pid; i++)
    {
        if (process_table[i].flags & F_INIT)
        {
            source_pid = process_table[i].code_physical[0];

            //printf("4. INITING %d from %d\n", i, source_pid);
            process_table[i].code_physical[0] = paging_copy_virtual_to_new(0x08048000);
            process_table[i].code_physical[1] = paging_copy_virtual_to_new(0x08049000);
            process_table[i].stack_physical = paging_copy_virtual_to_new(0xbffff000);
            process_table[i].heap_physical = paging_copy_virtual_to_new(0x80000000);
            ////printf("5. COPPIED! %h, %h ,%h\n", process_table[i].code_physical[0], process_table[i].code_physical[1], process_table[i].stack_physical);
            

            process_table[i].flags = 0;
            process_table[i].esp = process_table[source_pid].esp;
            process_table[i].ebp = process_table[source_pid].ebp;
            process_table[i].code_size = process_table[source_pid].code_size;
            process_table[i].stack_size = process_table[source_pid].stack_size;
            process_table[i].heap_size = process_table[source_pid].heap_size;
            process_table[i].io.pipes = 0;
            process_table[i].io.px = 0;
            process_table[i].io.py = 0;
            process_table[i].io.wx = 0;
            process_table[i].io.wy = 0;
            process_table[i].io.column = 0;
            process_table[i].io.row = 0;
            
            //print_proccess_table();
            //printf("6. DONE!\n");
            current_pid = starting_pid;

            asm("movl %%esp, %0":"=r"(kernel_esp)::"ebx");
            asm("movl %%ebp, %0":"=r"(kernel_ebp)::"ebx");
            ////printf("8. PERFORM ESP SWITCH %h, %h\n", process_table[current_pid].esp, process_table[current_pid].ebp);
            asm("movl %0, %%esp"::"r"(process_table[current_pid].esp):"ebx");
            asm("movl %0, %%ebp"::"r"(process_table[current_pid].ebp):"ebx");
            return;
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
        if (process_table[current_pid].flags & F_DEAD)
            continue;
        if (process_table[current_pid].flags & F_PAUSED)
            continue;
        //printf("3. SWITCHING %d to %d\n", starting_pid, current_pid);
        break;
    }
    ////printf("7. PERFORM FULL SWITCH %h, %h, %h\n", process_table[current_pid].code_physical[0], process_table[current_pid].code_physical[1], process_table[current_pid].stack_physical);

    paging_map_phys_to_virtual(process_table[current_pid].code_physical[0], 0x08048000);
    paging_map_phys_to_virtual(process_table[current_pid].code_physical[1], 0x08049000);
    paging_map_phys_to_virtual(process_table[current_pid].stack_physical, 0xbffff000);
    paging_map_phys_to_virtual(process_table[current_pid].heap_physical, 0x80000000);

    asm("movl %%esp, %0":"=r"(kernel_esp)::"ebx");
    asm("movl %%ebp, %0":"=r"(kernel_ebp)::"ebx");
    ////printf("8. PERFORM ESP SWITCH %h, %h\n", process_table[current_pid].esp, process_table[current_pid].ebp);
    asm("movl %0, %%esp"::"r"(process_table[current_pid].esp):"ebx");
    asm("movl %0, %%ebp"::"r"(process_table[current_pid].ebp):"ebx");
    //printf("GO!: %d\n", current_pid);
    asm("sti");
}

void scheduler_init()
{
    global_ms = 0;
    current_pid = 0;
    next_pid = 1;

    paging_map_new_to_virtual(0xc0003000);

    process_table = (process_table_entry*)0xc0003000;
    
    program_pointers = paging_get_programs();

    process_table[0].flags = 0;
    process_table[0].esp = 0xbfffffff;
    process_table[0].ebp = 0xbfffffff;
    process_table[0].code_size = 2;
    process_table[0].stack_size = 1;
    process_table[0].heap_size = 1;

    paging_map_new_page_table(0x020);
    paging_map_new_page_table(0x2ff);
    paging_map_new_page_table(0x200);

    process_table[0].code_physical[0] = paging_map_new_to_virtual(0x08048000);
    process_table[0].code_physical[1] = paging_map_new_to_virtual(0x08049000);
    process_table[0].stack_physical = paging_map_new_to_virtual(0xBFFFFFFF);
    process_table[0].heap_physical = paging_map_new_to_virtual(0x80000000);

    paging_copy_physical_to_virtual(program_pointers[1], 0x08048000);
    paging_copy_physical_to_virtual(program_pointers[1] + 0x1000, 0x08049000);
    events_init();
}

void scheduler_register_kernel_stack(uint32_t esp, uint32_t ebp)
{
    kernel_esp = esp;
    kernel_ebp = ebp;
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
