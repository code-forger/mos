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
    uint32_t* directory = (uint32_t*)0xfffff000;
    directory[0] = 0 | R_W_NP;
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

    kmrfsFormatHdd(4*1024, 0);

    kernel_set_pit();
    for(int i = 0 ; i < 0000001; i++);

    kprintf("HERE\n");

    //if(keyboard_get_a_byte() == 't')
    //    kernel_test_mode(TEST_STRESS, TEST_QUIET);

    uint32_t esp, ebp;

    asm("movl %%esp, %0":"=r"(esp)::"ebx");
    asm("movl %%ebp, %0":"=r"(ebp)::"ebx");

    scheduler_init(esp, ebp);
}