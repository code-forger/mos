#pragma once
#include "../declarations.h"
#include "../init/idt.h"
#include "../init/gdt.h"
#include "../IPC/pipe.h"
#include "../memory/memory.h"
#include "../scheduler/events.h"
#include "../io/pci.h"

static const uint32_t DIRECTORY         = 0xFFFFF000;
static const uint32_t PCI_TABLE         = 0xC0007000;
static const uint32_t EVENT_TABLE       = 0xC0006000;
static const uint32_t PIPE_TABLE        = 0xC0005000;
static const uint32_t SCRATCH           = 0xC0004000;
static const uint32_t PROCESS_TABLE     = 0xC0003000;
static const uint32_t VGA_BUFFER        = 0xC0002000;
static const uint32_t GDT_IDT_PROGRAMS  = 0xC0001000;
static const uint32_t MEMORY_MAP        = 0xC0000000;


#define R_W_NP 2 //read + write + not present
#define R_W_P 3 // read +


pipe_descriptor* paging_get_pipe_table();
idt_info_type* paging_get_idt();
gdt_info_type* paging_get_gdt();
memory_map_entry* paging_get_memory_map();
Node* paging_get_event_map();
PCI* paging_get_pci_map();
uint8_t* paging_get_memory_free_map();
uint16_t* paging_get_terminal_buffer();
uint32_t* paging_get_programs();
uint32_t* paging_get_program_base();
uint32_t* paging_get_program_stack();
uint32_t* paging_get_directory();

void paging_map_new_page_table(uint32_t target_table);
void paging_map_phys_to_virtual(uint32_t physical, uint32_t address);
void paging_unmap_virtual(uint32_t address);
uint32_t paging_map_new_to_virtual(uint32_t address);
uint32_t paging_copy_virtual_to_new(uint32_t source);
void paging_copy_virtual_to_physical(uint32_t source, uint32_t target);
void paging_copy_physical_to_virtual(uint32_t source, uint32_t target);
void paging_copy_virtual_to_virtual(uint32_t source, uint32_t target);