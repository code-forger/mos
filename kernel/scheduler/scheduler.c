#include "scheduler.h"

static uint32_t current_pid;
static uint32_t next_pid;
static process_table_entry* process_table;

uint32_t fork()
{
    asm("cli");
    process_table[next_pid].flags = F_INIT;
    uint32_t pid = next_pid;
    process_table[next_pid++].stack_bottom = current_pid;
    //asm("sti");
    asm("int $32");
    return pid;
}

void kill(uint32_t PiD)
{
    process_table[PiD].flags += F_DEAD;
}

/*static void print_proccess_table()
{
    for (uint32_t i = 0; i < next_pid; i++)
    {
        terminal_print("P");
        terminal_putint(i, 1);
        terminal_print(": ");
        terminal_putintbin(process_table[i].flags,4);
        terminal_print(" ");
        terminal_putinthex(process_table[i].esp,8);
        terminal_print(" ");
        terminal_putinthex(process_table[i].ebp,8);
        terminal_print(" ");
        terminal_putinthex(process_table[i].stack_top,8);
        terminal_print(" ");
        terminal_putinthex(process_table[i].stack_bottom,8);
        terminal_print("\n");
    }
}*/

void scheduler_set_kernel_stack(uint32_t page)
{
    process_table[0].stack_bottom = page;
    process_table[0].stack_top = page + 4096;
}

void scheduler_time_interupt()
{
    asm("cli");
    //save esp and ebp into process table
    asm("movl %%esp, %0":"=r"(process_table[current_pid].esp)::);
    asm("movl %%ebp, %0":"=r"(process_table[current_pid].ebp)::);
    ++current_pid;
    for (;; ++current_pid)
    {
        current_pid = current_pid % next_pid;
        if (process_table[current_pid].flags & F_SKIP)
        {
            process_table[current_pid].flags -= F_SKIP;
            continue;
        }
        if (process_table[current_pid].flags & F_DEAD)
            continue;
        if (process_table[current_pid].flags & F_PAUSED)
            continue;
        //terminal_print("SHIFTING INTO: ");
        //terminal_putint(current_pid, 8);
        //terminal_print("\n");
        break;
    }
    if (process_table[current_pid].flags & F_INIT)
    {
        uint32_t source_pid = process_table[current_pid].stack_bottom;
        uint32_t stop = process_table[source_pid].stack_top;
        uint32_t sbottom = process_table[source_pid].stack_bottom;

        process_table[current_pid].stack_bottom = get_free_page_and_allocate();
        process_table[current_pid].stack_top = process_table[current_pid].stack_bottom + (4 * 1024);
        uint32_t ttop = process_table[current_pid].stack_top;
        //uint32_t tbottom = process_table[current_pid].stack_bottom;
        uint32_t revs = 0;
        for (uint32_t scurrent = stop; scurrent > sbottom; scurrent -= 4) // COPY THE WHOLLLLEEEE STACK!
        {
            uint32_t* pscurrent = (uint32_t*)scurrent;
            uint32_t targ = stop - scurrent;
            uint32_t tcurrent = ttop - targ;
            uint32_t* ptcurrent = (uint32_t*)tcurrent;

            uint32_t val = * pscurrent;
            if (val < stop && val > sbottom)
            {
                val = (ttop - (stop - val));
            }

            *ptcurrent = val;
            revs++;
        }
        process_table[current_pid].flags = 0;
        process_table[current_pid].esp = ttop - (stop - process_table[source_pid].esp);
        process_table[current_pid].ebp = ttop - (stop - process_table[source_pid].ebp);
        //print_proccess_table();
    }
    
    asm("movl %0, %%esp"::"r"(process_table[current_pid].esp):);
    asm("movl %0, %%ebp"::"r"(process_table[current_pid].ebp):);
    //asm("sti");
}

uint32_t kernel_get_stack_top();
uint32_t kernel_get_stack_bottom();

void scheduler_init()
{
    current_pid = 0;
    next_pid = 1;
    process_table = (process_table_entry*)get_free_page_and_allocate();
    process_table[0].flags = 0;
}

uint32_t scheduler_get_pid()
{
    return current_pid;
}