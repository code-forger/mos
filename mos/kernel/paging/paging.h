#pragma once
#include "../declarations.h"
#include "../init/idt.h"
#include "../init/gdt.h"
#include "../IPC/pipe.h"
#include "../memory/memory.h"
#include "../scheduler/events.h"
#include "../scheduler/scheduler.h"

static const uint32_t DIRECTORY            = 0xFFFFF000;
static const uint32_t KERNEL_HEAP          = 0xC0010000;
static const uint32_t PROCESS_STACK_TABLE  = 0xC000f000;
static const uint32_t PROCESS_HEAP_TABLE   = 0xC000E000;
static const uint32_t HDD_CACHE_TABLE___   = 0xC000D000;
static const uint32_t HDD_CACHE_TABLE__    = 0xC000C000;
static const uint32_t HDD_CACHE_TABLE_     = 0xC000B000;
static const uint32_t HDD_CACHE_TABLE      = 0xC000A000;
static const uint32_t EVENT_TABLE          = 0xC0009000;
static const uint32_t PIPE_TABLE           = 0xC0008000;
static const uint32_t SCRATCH_TOP          = 0xC0007000;
static const uint32_t SCRATCH              = 0xC0006000;
static const uint32_t PROCESS_TABLE        = 0xC0005000;
static const uint32_t VGA_PROCESS_BUFFER   = 0xC0004000;
static const uint32_t VGA_KERNEL_BUFFER    = 0xC0003000;
static const uint32_t VGA_BUFFER           = 0xC0002000;
static const uint32_t GDT_IDT_PROGRAMS     = 0xC0001000;
static const uint32_t MEMORY_MAP           = 0xC0000000;


#define R_W_NP 2 //read + write + not present
#define R_W_P 3 // read + write + present


pipe_descriptor* paging_get_pipe_table();
process_table_entry* paging_get_process_table();
idt_info_type* paging_get_idt();
gdt_info_type* paging_get_gdt();
memory_map_entry* paging_get_memory_map();
Node* paging_get_event_map();
uint8_t* paging_get_memory_free_map();
uint16_t* paging_get_terminal_buffer();
uint16_t* paging_get_process_terminal_buffer();
uint16_t* paging_get_kernel_terminal_buffer();
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

void paging_init();