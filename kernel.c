#include "init/gdt.h"
#include "init/idt.h"
#include "pic/pic.h"
#include "scheduler/scheduler.h"
void kerror(const char* data)
{
    printf(data);
}

#if defined(__cplusplus)
extern "C" /* Use C linkage for kernel_main. */
#endif

void pre_init()
{
    terminal_initialize();
}

#define R_W_NP 2 //read + write + not present
#define R_W_P 3 // read + write + present

void init_kernel(multiboot_info_t *mbd, uint32_t magic)
{
    magic = magic; /* -Werror */
    gdt_init();
    idt_init();
    pic_init();
    memory_init(mbd);
    scheduler_init();

    uint32_t* directory = (uint32_t*)get_free_page_and_allocate();
    for (int i = 0; i < 1024; i++)
        directory[i] = 0|R_W_NP;
    uint32_t* page_table = (uint32_t*)get_free_page_and_allocate();
    for (int i = 0; i < 1024 /* map the first MB */; i++)
        page_table[i] = (i * 4096) | R_W_P;
    uint32_t* stack_page_table = (uint32_t*)get_free_page_and_allocate();
    for (int i = 0; i < 1024 /* map the first MB */; i++)
        stack_page_table[i] = 0 | R_W_P;

    directory[0] = ((uint32_t)page_table | R_W_P);
    directory[256] = ((uint32_t)stack_page_table | R_W_P);
    asm("mov %0, %%cr3"::"b"(directory):);
    uint32_t control_register;
    asm("mov %%cr0, %0":"=b"(control_register)::);
    control_register |= 0x80000000;
    printf("Pre fuck\n");
    asm("mov %0, %%cr0"::"b"(control_register):);
    printf("Post fuck\n");

}

void rebase_stack(uint32_t, void(*)(void));


void higher_kernel()
{
    printf("HIGHER_KERNAL!\n");
    asm("sti"); // re-enable interrupts for normal kernel operation.
    if (fork() != scheduler_get_pid())
        fork();
    for (;;)
    {
        uint32_t sp;
        asm("movl %%esp, %0":"=r"(sp)::);
        printf("I am process: %d my stack pointer is %h\n", scheduler_get_pid(), sp);;
    }
}

void kernel_main()
{
    asm("sti");
    printf("MAIN!\n");
    uint32_t page = 0x40000000;
    printf("made page!\n");
    scheduler_set_kernel_stack(page);
    printf("set page!\n");
    rebase_stack(page, *higher_kernel);
}
