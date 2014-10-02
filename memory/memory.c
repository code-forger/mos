#include "memory.h"
#include "../io/terminal.h"

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

void memory_init(multiboot_info_t *info)
{
    //print_memory(info);
    for (memory_map_t *map = (memory_map_t*)info->mmap_addr;
         (uint32_t)map < ((uint32_t)info->mmap_addr) + info->mmap_length;
         map = (memory_map_t*)(((uint32_t)map) + map->size + sizeof(uint32_t)))
    {
        if (map->type == 1)
        {
            uint64_t start_address = (((uint64_t)map->base_addr_low) + (((uint64_t)map->base_addr_high) << 32));
            uint64_t length = (map->length_low + (((uint64_t)map->length_high) << 32));

            if (start_address + length < 0x100000)
                continue; // This memory block is over the lower memory!

            if (start_address < 0x110000)
            {
                start_address += 0x10000; //compensate for kernel stack
                length -= 0x10000; //compensate for kernel stack  
            }

            p_memory_map = (memory_map_entry*)(uint32_t)start_address;
            p_memory_map[0].base = start_address;
            p_memory_map[0].length = length;
            p_memory_map[0].type = 1;

            memory_free_map = (uint8_t*)(uint32_t)(start_address + sizeof(memory_map_entry)); // Give the first page to the memory free map 

            uint64_t num_pages = length / (1024 * 4);

            if (num_pages / 8 > ((1024 * 4) - sizeof(memory_map_entry))) // Clip the free page list to fit into first page!
                num_pages = (((1024 * 4) - sizeof(memory_map_entry)) * 8);

            p_memory_map[0].num_pages = num_pages;

            for (uint64_t i = 0; i < num_pages/8; i++)
                memory_free_map[0] = (uint8_t)0; // FREE ALL PAGES!

            mark_page_allocated(0); // MARK THIS PAGE ALLOCATED

            //terminal_print("MEMORY MANAGED!\n");
            break;
        }
    }
}

uint32_t get_free_page_and_allocate()
{
    int64_t page = find_free_page();
    if (page == -1)
        return -1;

    uint32_t upage = (uint32_t)page;

    mark_page_allocated(upage);
    return ((uint32_t)p_memory_map[0].base) + (upage * (1024 * 4));
}
