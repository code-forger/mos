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
    printf("%b\n", info->flags)
;
    uint32_t kernel_physical, kernel_num_pages;
    uint32_t program_physical[info->mods_count - 1], program_num_pages[info->mods_count - 1];
    if (info->flags & 0b1000)
    {
        printf("\n%d Modules Loaded", info->mods_count);
        printf(" at %h\n", &info->mods_addr);
        for (uint32_t i = 0; i < info->mods_count; i++)
        {
            printf("    at: %h to %h\n", ((module_t*)info->mods_addr)[i].mod_start, ((module_t*)info->mods_addr)[i].mod_end);
            printf("  page: %h for %d pages\n", ((module_t*)info->mods_addr)[i].mod_start, (((module_t*)info->mods_addr)[i].mod_end - ((module_t*)info->mods_addr)[i].mod_start)/0x1000 + 1);
        }
        kernel_physical = ((module_t*)info->mods_addr)->mod_start;
        kernel_num_pages =  (((module_t*)info->mods_addr)->mod_end - ((module_t*)info->mods_addr)->mod_start)/0x1000 + 1;

        for (uint32_t i = 1; i < info->mods_count; i++)
        {
            program_physical[i-1] = ((module_t*)info->mods_addr)[i].mod_start;
            program_num_pages[i-1] =  (((module_t*)info->mods_addr)[i].mod_end - ((module_t*)info->mods_addr)[i].mod_start)/0x1000 + 1;
        }
    }

    /******************************************************************/


    if (info->flags & 0b10000000)
    {
        printf("\n%d Drives Loaded", info->drives_length);
        printf(" at %h\n", &info->drives_addr);
        for (uint32_t i = 0; i < info->drives_length; i++)
        {
            //printf("    at: %h to %h\n", ((module_t*)info->mods_addr)[i].mod_start, ((module_t*)info->mods_addr)[i].mod_end);
            //printf("  page: %h for %d pages\n", ((module_t*)info->mods_addr)[i].mod_start, (((module_t*)info->mods_addr)[i].mod_end - ((module_t*)info->mods_addr)[i].mod_start)/0x1000 + 1);
        }
        /*kernel_physical = ((module_t*)info->mods_addr)->mod_start;
        kernel_num_pages =  (((module_t*)info->mods_addr)->mod_end - ((module_t*)info->mods_addr)->mod_start)/0x1000 + 1;

        for (uint32_t i = 1; i < info->drives_length; i++)
        {
            program_physical[i-1] = ((module_t*)info->mods_addr)[i].mod_start;
            program_num_pages[i-1] =  (((module_t*)info->mods_addr)[i].mod_end - ((module_t*)info->mods_addr)[i].mod_start)/0x1000 + 1;
        }*/
    }

    /******************************************************************/


    uint32_t* program_pointers = (uint32_t*)(get_address_of_page(1) + sizeof(gdt_info_type) + sizeof(idt_info_type));
    program_pointers[0] = info->mods_count - 1;
    for (uint32_t i = 0; i < info->mods_count-1; i++)
    {
        program_pointers[1 + (i*2)] = program_physical[i];
        program_pointers[2 + (i*2)] = program_num_pages[i];
    }

    uint32_t* directory = (uint32_t*)get_free_page_and_allocate();
    for (uint32_t i = 0; i < 1024; i++)
        directory[i] = 0|R_W_NP;
    uint32_t* page_table = (uint32_t*)get_free_page_and_allocate();
    for (uint32_t i = 0; i < 1024; i++) // identity map lower memory (so the loader doesnt page fault imediately.)
        page_table[i] = ((i * 4096) & 0xfffff000) | R_W_P;
    uint32_t* kernel_code_page_table = (uint32_t*)get_free_page_and_allocate();
    for (uint32_t address = kernel_physical, i = 0; i < kernel_num_pages; i++, address += 0x1000) // map the kernel to live at 0xc0400000
        kernel_code_page_table[i] = ( (address + 0x1000) & 0xfffff000) | R_W_P;

    uint32_t* kernel_meta_page_table = (uint32_t*)get_free_page_and_allocate(); // memory managemnt, gdt, idt
    kernel_meta_page_table[0] = ( get_address_of_page(0) & 0xfffff000) | R_W_P;
    kernel_meta_page_table[1] = ( get_address_of_page(1) & 0xfffff000) | R_W_P;
    kernel_meta_page_table[2] = ( (0xB8000) & 0xfffff000) | R_W_P;

    uint32_t* kernel_stack_page_table = (uint32_t*)get_free_page_and_allocate();
    kernel_stack_page_table[0x3ff] = ( get_free_page_and_allocate() & 0xfffff000) | R_W_P;


    directory[0] = (((uint32_t)page_table & 0xfffff000) | R_W_P);
    directory[0x300] = (((uint32_t)kernel_meta_page_table & 0xfffff000) | R_W_P);
    directory[0x301] = (((uint32_t)kernel_code_page_table & 0xfffff000) | R_W_P);
    directory[0x3fe] = (((uint32_t)kernel_stack_page_table & 0xfffff000) | R_W_P);
    directory[0x3ff] = (((uint32_t)directory & 0xfffff000) | R_W_P);
    asm("mov %0, %%cr3"::"b"(directory):);
    uint32_t control_register;
    asm("mov %%cr0, %0":"=b"(control_register)::);
    control_register |= 0x80000000;
    asm("mov %0, %%cr0"::"b"(control_register):);




    //printf("KERNEL start: %h\n", (uint32_t)*((uint32_t*)0x40000080));


    printf("Loader Done!");
    asm("cli");
    //asm("hlt");
    asm("mov %0, %%esp"::"r"(0xffbfffff));
    asm("jmp %0"::"r"(0xc04000f0));
    for (;;);
}
