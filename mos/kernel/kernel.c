#include "init/gdt.h"
#include "init/idt.h"
#include "pic/pic.h"
#include "scheduler/scheduler.h"
#include "io/terminal.h"
#include "IPC/pipe.h"
#include "io/pci.h"
#include "io/hdd.h"
#include "mrfs/malloc.h"
#include "mrfs/mrfs.h"
#include "paging/paging.h"
#include "io/hdd.h"
#include "io/port.h"
#include "ELF/elf.h"

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
    paging_init();
    terminal_initialize();
    gdt_init();
    idt_init();
    pic_init();
    pipe_init();
    pci_init();
    hdd_init();
    init_mem();

    mrfsFormatHdd(4*1024, 0);
    hdd_write_cache();

    mrfsDeleteFolderRecursive("/proc/");

    mrfsNewFolder("/", "proc");

    /*uint32_t rate = 3;
    rate &= 0x0F;            // rate must be above 2 and not over 15
    send_byte_to(0x70, 0x8A);        // set index to register A, disable NMI
    char prev=get_byte_from(0x71);    // get initial value of register A
    send_byte_to(0x70, 0x8A);        // reset index to A
    send_byte_to(0x71, (prev & 0xF0) | rate); //write only our rate to A. Note, rate is the bottom 4 bits.
    */




    uint32_t esp, ebp;

    asm("movl %%esp, %0":"=r"(esp)::"ebx");
    asm("movl %%ebp, %0":"=r"(ebp)::"ebx");

    scheduler_init(esp, ebp);
}
// 448145