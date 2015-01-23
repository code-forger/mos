#include "malloc.h"
#include "../paging/paging.h"
#include "../io/terminal.h"

typedef struct header_t {
    uint32_t size;
    uint8_t free;
} __attribute__ ((packed)) header;

#define HEAD_SIZE sizeof(header)

header *top;
int next_heap = 0;

static void clean_memory()
{
    header *current = top;

    do
    {
        if (current->free)
        {
            header *next_node = (header*)((uint32_t)current + HEAD_SIZE + current->size);
            if (next_node->free && next_node->free != 3)
            {
                current->size = current->size + next_node->size + HEAD_SIZE;
                current = top;
                continue;
            }
        }

        current = (header*)((uint32_t)current + HEAD_SIZE + current->size);
    } while (current->free != 3);
}

static void get_more_memory()
{
    paging_map_new_to_virtual(next_heap);

    header* current = (header*)(next_heap - HEAD_SIZE);

    current->free = true;
    current->size = 0x1000 - HEAD_SIZE;

    current = (header*)(((uint32_t)current) + current->size + HEAD_SIZE);
    current->size = 0;
    current->free = 3;

    next_heap = next_heap + 0x1000;

    clean_memory();
}

void *malloc(uint32_t size) {
    //printf("[malloc.c] CALL : malloc(%d)\n", size);
    header* current = top;

    do {
        if (size + HEAD_SIZE < current->size && current->free)
        {
            uint32_t node_size = current->size;
            uint32_t ret = (uint32_t)current + HEAD_SIZE;

            current->size = size;
            current->free = false;

            current = (header*)((uint32_t)current + HEAD_SIZE + size);
            current->size = node_size - size - HEAD_SIZE;
            current->free = true;

            return (void *) ret;
        }

        current = (header*)((uint32_t)current + HEAD_SIZE + current->size);
    } while (current->free != 3);

    get_more_memory();
    return malloc(size);
}

void *malloc_for_process(uint32_t size, uint32_t memory) {
    header* current = (header*)memory;
    do
    {
        if (size + HEAD_SIZE < current->size && current->free)
        {
            uint32_t node_size = current->size;
            uint32_t ret = (uint32_t)current + HEAD_SIZE;

            current->size = size;
            current->free = false;

            current = (header*)((uint32_t)current + HEAD_SIZE + size);

            current->size = node_size - size - HEAD_SIZE;
            current->free = true;

            return (void *) ret;
        }

        current = (header*)((uint32_t)current + HEAD_SIZE + current->size);
    } while (current->free != 3);

    printf("Memory Manager our of memory!\n");
    asm("cli");
    asm("hlt");
    return 0;
}

void init_mem()
{
    top = (header*) KERNEL_HEAP;
    top->size = 0x1000 - HEAD_SIZE - HEAD_SIZE;
    top->free = true;

    top = (header*)(((uint32_t)top) + top->size + HEAD_SIZE);
    top->size = 0;
    top->free = 3;

    top = (header*) KERNEL_HEAP;

    next_heap = KERNEL_HEAP + 0x1000;
}


void free(void *memory)
{
    //printf("[malloc.c] CALL : free(%h)\n", memory);
    header *current = top;

    do
    {
        if ((uint32_t)current + HEAD_SIZE == (uint32_t) memory)
        {
            header *next_node = (header*)((uint32_t)current + HEAD_SIZE + current->size);
            if (next_node->free)
            {
                current->size = current->size + next_node->size + HEAD_SIZE;
                current->free = true;
            }
            else
            {
                current->free = true;
            }
            return;
        }

        current = (header*)((uint32_t)current + HEAD_SIZE + current->size);
    } while (current->free != 3);
}
