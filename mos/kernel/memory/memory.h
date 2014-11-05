#pragma once
#include "../declarations.h"

typedef struct mm_entry
{
    uint64_t base;
    uint64_t length;
    uint32_t type;
    uint32_t num_pages; // ignore this value.
} __attribute__((packed)) memory_map_entry;

void memory_init();
uint32_t get_free_page_and_allocate();
uint32_t get_address_of_page(uint32_t page);
uint32_t get_page_of_address(uint32_t address);