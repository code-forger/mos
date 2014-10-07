#include "init/gdt.h"
#include "init/idt.h"
#include "memory/memory.h"
#include "io/terminal.h"
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

void loader_main(multiboot_info_t *info, uint32_t magic)
{
    magic = magic;
    printf("HERE!");
    uint32_t kernel_physical, kernel_num_pages;
    if (info->flags & 100)
    {
        printf("\n%d Modules Loaded\n", info->mods_count);
        printf("    at: %h to %h\n", ((module_t*)info->mods_addr)->mod_start, ((module_t*)info->mods_addr)->mod_end);
        printf("  page: %d for %d pages\n", get_page_of_address(((module_t*)info->mods_addr)->mod_start), (((module_t*)info->mods_addr)->mod_end - ((module_t*)info->mods_addr)->mod_start)/(1024*4));
        kernel_physical = ((module_t*)info->mods_addr)->mod_start;
        kernel_num_pages =  (((module_t*)info->mods_addr)->mod_end - ((module_t*)info->mods_addr)->mod_start)/(1024*4) + 1;
    }

    uint32_t* directory = (uint32_t*)get_free_page_and_allocate();
    for (uint32_t i = 0; i < 1024; i++)
        directory[i] = 0|R_W_NP;
    uint32_t* page_table = (uint32_t*)get_free_page_and_allocate();
    for (uint32_t i = 0; i < 1024; i++) // identity map lower memory (so the loader doesnt page fault imediately.)
        page_table[i] = ((i * 4096) & 0xfffff000) | R_W_P;
    uint32_t* kernel_page_table = (uint32_t*)get_free_page_and_allocate();
    for (uint32_t address = kernel_physical, i = 0; i < kernel_num_pages; i++, address += (1024 * 4)) // map the kernel to live at 0xc0000000
        kernel_page_table[i] = ( address & 0xfffff000) | R_W_P;

    uint32_t* kernel_meta_page_table = (uint32_t*)get_free_page_and_allocate(); // memory managemnt, gdt, idt
    kernel_meta_page_table[0] = ( get_address_of_page(0) & 0xfffff000) | R_W_P;
    kernel_meta_page_table[1] = ( get_address_of_page(1) & 0xfffff000) | R_W_P;

    uint32_t* kernel_stack_page_table = (uint32_t*)get_free_page_and_allocate();
    kernel_stack_page_table[0x3ff] = ( get_free_page_and_allocate() & 0xfffff000) | R_W_P;



    directory[0] = (((uint32_t)page_table & 0xfffff000) | R_W_P);
    directory[0x300] = (((uint32_t)kernel_meta_page_table & 0xfffff000) | R_W_P);
    directory[0x301] = (((uint32_t)kernel_page_table & 0xfffff000) | R_W_P);
    directory[0x3fe] = (((uint32_t)kernel_stack_page_table & 0xfffff000) | R_W_P);
    directory[0x3ff] = (((uint32_t)directory & 0xfffff000) | R_W_P);
    asm("mov %0, %%cr3"::"b"(directory):);
    uint32_t control_register;
    asm("mov %%cr0, %0":"=b"(control_register)::);
    control_register |= 0x80000000;
    asm("mov %0, %%cr0"::"b"(control_register):);

    printf("Loader Done!");
    for (;;);
}
