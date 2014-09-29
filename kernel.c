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

void kernel_main()
{
    asm("sti"); // re-enable interrupts for normal kernel operation.
    terminal_print("Kernel Success!\n");
    uint32_t page = get_free_page_and_allocate();
    terminal_putinthex(page, 8);
    terminal_print("\n");
    for (;;);
}
