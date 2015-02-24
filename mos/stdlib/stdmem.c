#include "stdmem.h"

typedef struct header_t {
    uint8_t pad1;
    uint32_t size;
    uint8_t pad2;
    uint8_t free;
    uint8_t pad3;
} __attribute__ ((packed)) header;

#define HEAD_SIZE sizeof(header)

header *top;
int heap = 0;

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

void *malloc(uint32_t size) {
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

    return 0;
}

void free(void *memory)
{
    header *current = top;

    do
    {
        if ((uint32_t)current + HEAD_SIZE == (uint32_t) memory)
        {
            current->free = true;
            clean_memory();
            return;
        }
        current = (header*)((uint32_t)current + HEAD_SIZE + current->size);
    } while (current->free != 3);
}

void create_heap()
{
    top = (header*) 0x80000000;
    top->size = 0x1000 - HEAD_SIZE - HEAD_SIZE;
    top->free = true;

    top = (header*)(((uint32_t)top) + top->size + HEAD_SIZE);
    top->size = 0;
    top->free = 3;

    top = (header*) 0x80000000;

    heap += top->size;
}