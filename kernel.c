#include "io/terminal.h"
#include "init/gdt.h"
#include "init/idt.h"
#include "pic/pic.h"

void kerror(const char* data)
{
    terminal_print(data);
}

#if defined(__cplusplus)
extern "C" /* Use C linkage for kernel_main. */
#endif

void pre_init()
{
    terminal_initialize();
}

void init_kernel()
{
    gdt_init();
    idt_init();
    pic_init();
}

void kernel_main()
{
    asm("sti"); // re-enable interrupts for normal kernel operation.
    terminal_print("Kernel Success!\n");
    for (;;);
}
