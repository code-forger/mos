#include "init/gdt.h"
#include "init/idt.h"
#include "pic/pic.h"
#include "scheduler/scheduler.h"
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
    scheduler_init();
}

void rebase_stack(uint32_t, void(*)(void));


void higher_kernel()
{
    asm("sti"); // re-enable interrupts for normal kernel operation.
    fork();
    for (;;)
    {
        asm ("cli");
        terminal_print("I am process: ");
        terminal_putint(scheduler_get_pid(), 1);
        terminal_print(" ");

        terminal_print("my stack pointer is");
        uint32_t sp;

        asm("movl %%esp, %0":"=r"(sp)::);
        terminal_putinthex(sp, 8);
        terminal_print("\n");
        asm ("sti");
    }
}

void kernel_main()
{
    uint32_t page = get_free_page_and_allocate();
    scheduler_set_kernel_stack(page);
    rebase_stack(page, *higher_kernel);
}
