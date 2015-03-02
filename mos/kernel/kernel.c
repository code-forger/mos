#include "init/gdt.h"
#include "init/idt.h"
#include "pic/pic.h"
#include "scheduler/scheduler.h"
#include "io/terminal.h"
#include "IPC/pipe.h"
#include "io/hdd.h"
#include "kstdlib/kstdlib.h"
#include "mrfs/mrfs.h"
#include "paging/paging.h"
#include "io/hdd.h"
#include "io/port.h"
#include "ELF/elf.h"
#include "drivers/keyboard.h"
#include "kerneltest.h"

void kerror(const char* data)
{
    printf(data);
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
    terminal_initialize();
    gdt_init();
    idt_init();
    pic_init();
    pipe_init();
    hdd_init();
    init_mem();
    keryboard_init();

    mrfsFormatHdd(4*1024, 0);
    hdd_write_cache();

    mrfsDeleteFolderRecursive("/proc/");

    mrfsNewFolder("/", "proc");

    kernel_set_pit();
    //for(int i = 0 ; i < 10000000; i++);

    //kernel_test_mode();

    uint32_t esp, ebp;

    asm("movl %%esp, %0":"=r"(esp)::"ebx");
    asm("movl %%ebp, %0":"=r"(ebp)::"ebx");

    scheduler_init(esp, ebp);
}
// 448145