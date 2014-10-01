#include "io/terminal.h"
#include "init/gdt.h"
#include "init/idt.h"
#include "pic/pic.h"
#include "memory/memory.h"

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

void init_kernel(multiboot_info_t *mbd, uint32_t magic)
{
    magic = magic; /* -Werror */
    gdt_init();
    idt_init();
    pic_init();
    memory_init(mbd);
}

void rebase_stack(uint32_t, void(*)(void));


void higher_kernel()
{
    asm("sti"); // re-enable interrupts for normal kernel operation.
    terminal_print("Higher Kernel Ready!");
    for (;;);
}

void kernel_main()
{
    uint32_t page = get_free_page_and_allocate();
    rebase_stack(page, *higher_kernel);
}
