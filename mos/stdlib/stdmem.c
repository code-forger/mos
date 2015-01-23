#include "stdmem.h"

typedef struct header_t {
    uint32_t size;
    uint8_t free;
} __attribute__ ((packed)) header;

#define HEAD_SIZE sizeof(header)

header *top;
int heap = 0;

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
void init_mem()
{
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


void free(void *memory)
{
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
