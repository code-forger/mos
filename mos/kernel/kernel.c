#include "init/gdt.h"
#include "init/idt.h"
#include "pic/pic.h"
#include "scheduler/scheduler.h"
#include "io/terminal.h"
#include "IPC/pipe.h"
#include "io/hdd.h"
#include "kstdlib/kstdlib.h"
#include "fs/mrfs.h"
#include "fs/kmrfs.h"
#include "paging/paging.h"
#include "io/hdd.h"
#include "io/port.h"
#include "ELF/elf.h"
#include "drivers/keyboard.h"
#include "kerneltest.h"

void kerror(const char* data)
{
    kprintf(data);
}

#if defined(__cplusplus)
extern "C" /* Use C linkage for kernel_main. */
#endif


#define R_W_NP 2 //read + write + not present
#define R_W_P 3 // read + write + present

void kernel_set_pit(void);

void init_kernel()
{

    // clean up after the bios
    uint32_t* directory = (uint32_t*)0xfffff000;
    directory[0] = 0 | R_W_NP;

    //initialize entire kernel
    memory_init();
    paging_init();
    keyboard_init();
    terminal_init();
    gdt_init();
    idt_init();
    pic_init();
    pipe_init();
    hdd_init();
    init_mem();


    //ensure hardrive is ready
    kmrfsFormatHdd(4*1024, 0);

    // set the timer to 10ms
    kernel_set_pit();

    if(keyboard_get_a_byte() == 't') // check for test mode
        kernel_test_mode(TEST_STRESS, TEST_QUIET);
        //kernel_run_cache_timing();

    uint32_t esp, ebp;

    asm("movl %%esp, %0":"=r"(esp)::"ebx");
    asm("movl %%ebp, %0":"=r"(ebp)::"ebx");

    scheduler_init(esp, ebp);// start the scheduler into the first process.
}