#include "memory.h"
#include "../io/terminal.h"
#include "../paging/paging.h"

static memory_map_entry *p_memory_map;
static uint8_t *memory_free_map;

static uint32_t last_page;

static void mark_page_allocated(uint32_t page)
{
    memory_free_map[page/8] |= (uint8_t)(1 << (page % 8));
    last_page = page;
}

/*static void mark_page_free(uint32_t page)
{
    memory_free_map[page/8] -= (1 << (page % 8));
    last_page = page;
}*/

static int64_t find_free_page()
{
    for (uint32_t block = last_page/8; block < p_memory_map[0].num_pages/8; block++)
        for (uint32_t i = last_page%8; i < 8; i++)
            if (!(memory_free_map[block] & (1 << i)))
                return (int64_t)((block*8) + i);
    return -1;
}

/*static void print_memory(multiboot_info_t *info)
{

    for (memory_map_t *map = (memory_map_t*)info->mmap_addr;
         (uint32_t)map < ((uint32_t)info->mmap_addr) + info->mmap_length;
         map = (memory_map_t*)(((uint32_t)map) + map->size + sizeof(uint32_t)))
    {
        terminal_print("1: ");
        terminal_putinthex(map->base_addr_high, 8);
        terminal_putinthex(map->base_addr_low, 8);
        terminal_print(" ");
        terminal_putinthex(map->length_high, 8);
        terminal_putinthex(map->length_low, 8);
        terminal_print(" ");
        terminal_putinthex(map->type, 8);
        terminal_print("\n");
    }
}*/

void memory_init()
{
    p_memory_map = paging_get_memory_map();
    memory_free_map = paging_get_memory_free_map(); // Give the first page to the memory free map 
    last_page = 0;
}

uint32_t get_free_page_and_allocate()
{
    int64_t page = find_free_page();
    if (page == -1)
        return -1;

    uint32_t upage = (uint32_t)page;

    mark_page_allocated(upage);
    return ((uint32_t)p_memory_map[0].base) + (upage * 0x1000);
}

uint32_t get_address_of_page(uint32_t page)
{
    return ((uint32_t)p_memory_map[0].base) + (page * 0x1000);
}

uint32_t get_page_of_address(uint32_t address)
{    
    return (address - (uint32_t)p_memory_map[0].base) / 0x1000;
}