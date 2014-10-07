#include "init/gdt.h"
#include "init/idt.h"
#include "pic/pic.h"
#include "scheduler/scheduler.h"
#include "io/terminal.h"
void kerror(const char* data)
{
    printf(data);
}

#if defined(__cplusplus)
extern "C" /* Use C linkage for kernel_main. */
#endif


#define R_W_NP 2 //read + write + not present
#define R_W_P 3 // read + write + present

void init_kernel()
{
    uint32_t* directory = (uint32_t*)0xfffff000;
    directory[0] = 0 | R_W_NP;
    memory_init();
    gdt_init();
    idt_init();
    pic_init();
    terminal_initialize();
    printf("Hello!!\n");
    //scheduler_init();
}

void kernel_main()
{
    //printf("HELLO KERNEL!\n");
    //asm("sti"); // re-enable interrupts for normal kernel operation.
    //if (fork() != scheduler_get_pid())
    //    fork();
    for (;;)
    {
        uint32_t sp;
        asm("movl %%esp, %0":"=r"(sp)::);
        printf("I am process: %d my stack pointer is %h\n", scheduler_get_pid(), sp);;
    }
}
