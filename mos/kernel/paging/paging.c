#include "paging.h"
#include "../io/terminal.h"

static void flush_tlb()
{
     __asm__ __volatile__("movl    %cr3,%eax");
     __asm__ __volatile__("movl    %eax,%cr3");
}


pipe_descriptor* paging_get_pipe_table()
{
    return (pipe_descriptor*)PIPE_TABLE;
}

idt_info_type* paging_get_idt()
{
    return (idt_info_type*)(GDT_IDT_PROGRAMS + sizeof(gdt_info_type));
}

gdt_info_type* paging_get_gdt()
{
    return ((gdt_info_type*)GDT_IDT_PROGRAMS);
}

memory_map_entry* paging_get_memory_map()
{
    return (memory_map_entry*)MEMORY_MAP;
}

Node* paging_get_event_map()
{
    return (Node*)EVENT_TABLE;
}
PCI* paging_get_pci_map()
{
    return (PCI*)PCI_TABLE;
}
uint8_t* paging_get_memory_free_map()
{
    return (uint8_t*)(uint32_t)(MEMORY_MAP + sizeof(memory_map_entry));
}

uint16_t* paging_get_terminal_buffer()
{
    return(uint16_t*) VGA_BUFFER;
}

uint32_t* paging_get_programs()
{
    return (uint32_t*)(GDT_IDT_PROGRAMS + sizeof(gdt_info_type) + sizeof(idt_info_type));
}

uint32_t* paging_get_program_base()
{
    return 0;
}

uint32_t* paging_get_program_stack()
{
    return 0;
}

uint32_t* paging_get_directory()
{
    return (uint32_t*)DIRECTORY;
}


void paging_map_new_page_table(uint32_t target_table)
{
    ((uint32_t*)DIRECTORY)[target_table] = (get_free_page_and_allocate() & 0xfffff000) | R_W_P;
    flush_tlb();
}

void paging_map_phys_to_virtual(uint32_t physical, uint32_t address)
{
    //printf("Mappin %h to %h\n", physical, address);
    uint32_t *pd_lookup = (uint32_t*)(((address & 0xffc00000) >> 10) + 0xFFC00000);
    uint32_t pt_index = ((address & 0x003FF000) >> 12);
    pd_lookup[pt_index] = (physical & 0xfffff000) | R_W_P;
    flush_tlb();
}

void paging_unmap_virtual(uint32_t address)
{
    uint32_t *pd_lookup = (uint32_t*)(((address & 0xffc00000) >> 10) + 0xFFC00000);
    uint32_t pt_index = ((address & 0x003FF000) >> 12);
    pd_lookup[pt_index] = 0 | R_W_NP;
    flush_tlb();
}

uint32_t paging_map_new_to_virtual(uint32_t address)
{
    uint32_t *pd_lookup = (uint32_t*)(((address & 0xffc00000) >> 10) + 0xFFC00000);
    uint32_t pt_index = ((address & 0x003FF000) >> 12);
    uint32_t new_page = get_free_page_and_allocate();
    //printf("Mappin %h to %h\n", new_page, address);
    pd_lookup[pt_index] = (new_page & 0xfffff000) | R_W_P;
    flush_tlb();
    return new_page;
}

uint32_t paging_copy_virtual_to_new(uint32_t source)
{
    uint32_t new_page = paging_map_new_to_virtual(SCRATCH);
    paging_copy_virtual_to_virtual(source, SCRATCH);
    paging_unmap_virtual(SCRATCH);
    return new_page;
}

void paging_copy_virtual_to_physical(uint32_t source, uint32_t target)
{
    paging_map_phys_to_virtual(target, SCRATCH);
    paging_copy_virtual_to_virtual(source, SCRATCH);
    paging_unmap_virtual(SCRATCH);
}

void paging_copy_physical_to_virtual(uint32_t source, uint32_t target)
{
    paging_map_phys_to_virtual(source, SCRATCH);
    paging_copy_virtual_to_virtual(SCRATCH, target);
    paging_unmap_virtual(SCRATCH);
}

void paging_copy_virtual_to_virtual(uint32_t source, uint32_t target)
{
    for (uint32_t i = 0; i < 0x400; i++)
    {
        ((uint32_t*)target)[i] = ((uint32_t*)source)[i];
    }
}