#include "../declarations.h"

typedef struct mm_entry
{
    uint64_t base;
    uint64_t length;
    uint32_t type;
    uint32_t num_pages; // ignore this value.
} __attribute__((packed)) memory_map_entry;

void memory_init(multiboot_info_t *info);
uint32_t get_free_page_and_allocate();